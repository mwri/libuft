/// libuft transactions


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>

#include "uft.h"
#include "uft_tx.h"
#include "config.h"
#include "uft_status.h"
#include "uft_ll.h"


static int uft_tx_next_id = 0;


uft_status * tx_add_ent (uft_tx * tx, uft_status * status);
uft_status * add_ent_dir (uft_tx * tx, char * path);
uft_status * add_ent_file (uft_tx * tx, char * path, int flags, struct stat * statbufp);
uft_status * add_ent_symlink (uft_tx * tx, char * path, int flags, struct stat * statbufp);
uft_status * add_ent_noent (uft_tx * tx, char * path, int flags);
void         uft_tx_rollback (uft_tx * tx);
void         destroy_ent_state (uft_ent_state * ent_state);
void         destroy_tx_error (uft_tx_error * tx_error);
void         uft_rollback_file (uft_tx * tx, uft_ent_state * es);
void         uft_rollback_symlink (uft_tx * tx, uft_ent_state * es);
void         uft_rollback_noent (uft_tx * tx, uft_ent_state * es);


/// Create a new transaction.

uft_tx *
uft_tx_new (void * extra)
{
  uft_tx * tx = (uft_tx *) malloc(sizeof(uft_tx));

  tx->id = uft_tx_next_id++;
  tx->code = 0;
  tx->extra = extra;

  if (tx == NULL)
    return NULL;
  tx->ents = uft_ll_create();
  if (tx->ents == NULL) {
    free(tx);
    return NULL;
  }
  tx->errors = uft_ll_create();
  if (tx->errors == NULL) {
    free(tx->ents);
    free(tx);
    return NULL;
  }
  tx->children = uft_ll_create();
  if (tx->children == NULL) {
    free(tx->errors);
    free(tx->ents);
    free(tx);
    return NULL;
  }

  return tx;
}


/// Return the transactions ID.

int
uft_tx_id (uft_tx * tx)
{
  return tx->id;
}


/// Begin / run the transaction.

uft_tx *
uft_tx_begin (uft_tx * tx, void (*txfp)(uft_tx *))
{
  txfp(tx);

  if (tx->code == 0 || ((tx->code & UFT_TX_ERROR) != 0)) {
    uft_tx_rollback(tx);
  }

  return tx;
}


/// Free up resources held by the transaction.

void
uft_tx_end (uft_tx * tx)
{
  for (uft_ll_node * lln = uft_ll_head(tx->children); lln != NULL; lln = uft_ll_head(tx->children)) {
    uft_tx * child_tx = uft_ll_data(lln);
    uft_tx_end(child_tx);
    uft_ll_rmnode(lln);
  }
  uft_ll_rm(tx->children);
  for (uft_ll_node * lln = uft_ll_head(tx->errors); lln != NULL; lln = uft_ll_head(tx->errors)) {
    uft_tx_error * tx_error = uft_ll_data(lln);
    destroy_tx_error(tx_error);
    uft_ll_rmnode(lln);
  }
  uft_ll_rm(tx->errors);
  for (uft_ll_node * lln = uft_ll_head(tx->ents); lln != NULL; lln = uft_ll_head(tx->ents)) {
    uft_ent_state * ent_state = uft_ll_data(lln);
    destroy_ent_state(ent_state);
    uft_ll_rmnode(lln);
  }
  uft_ll_rm(tx->ents);
  free(tx);
}


/// Create a child / subordinate transaction.

uft_tx *
uft_tx_child (uft_tx * tx, void * extra)
{
  uft_tx * child_tx = uft_tx_new(extra);
  uft_ll_insert_tail(tx->children, child_tx);

  return child_tx;
}


/// Destroy / free an entity state.

void
destroy_ent_state (uft_ent_state * ent_state)
{
  free(ent_state->path);
  free(ent_state->data);
  free(ent_state);
}


/// Destroy / free a transaction error.

void
destroy_tx_error (uft_tx_error * tx_error)
{
  free(tx_error->msg);
  free(tx_error);
}


/// Return the pointer that is the transactions 'extra' data.

void *
uft_tx_extra (uft_tx * tx)
{
  return tx->extra;
}


/// Set the transactions 'extra' data pointer.

void *
uft_tx_set_extra (uft_tx * tx, void * extra)
{
  void * old_extra = tx->extra;
  tx->extra = extra;

  return old_extra;
}


/// Add a filesystem entity to the transaction.

uft_status *
uft_tx_add_ent(uft_tx * tx, char * path, int flags)
{
  struct stat statbuf;

  if (lstat(path, &statbuf) == 0) {
    if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
      return tx_add_ent(tx, add_ent_dir(tx, path));
    } else if ((statbuf.st_mode & S_IFMT) == S_IFREG) {
      return tx_add_ent(tx, add_ent_file(tx, path, flags, &statbuf));
    } else if ((statbuf.st_mode & S_IFMT) == S_IFLNK) {
      return tx_add_ent(tx, add_ent_symlink(tx, path, flags, &statbuf));
    }
  } else if (errno == ENOENT) {
    return tx_add_ent(tx, add_ent_noent(tx, path, flags));
  }
}


uft_status *
tx_add_ent(uft_tx * tx, uft_status * status)
{
  if (uft_status_error(status)) {
    uft_tx_log_error(tx, uft_status_error_msg(status));
    return status;
  }

  uft_ll_insert_tail(tx->ents, uft_status_data(status));
  return uft_status_set_success(status, tx);
}


uft_status *
add_ent_dir(uft_tx * tx, char * path)
{
  static uft_status status;

  return uft_status_set_error(&status, "cannot add existing directory \"%s\" to transaction", path);
}


uft_status *
add_ent_file(uft_tx * tx, char * path, int flags, struct stat * statbufp)
{
  static uft_status status;

  int fd = open(path, O_RDONLY);
  if (fd < 0)
    return uft_status_set_error(&status, "error adding file \"%s\", could not open for read: %s", path, strerror(errno));

  char * file_data = (char *) malloc(statbufp->st_size);
  if (read(fd, file_data, statbufp->st_size) != statbufp->st_size) {
    free(file_data);
    return uft_status_set_error(&status, "error adding file \"%s\", failed to read: %s", path, strerror(errno));
  }

  close(fd);

  uft_ent_state * ent_state = (uft_ent_state *) malloc(sizeof(uft_ent_state));
  ent_state->flags = UFT_ES_FILE;
  ent_state->path = (char *) malloc(strlen(path)+1);
  strcpy(ent_state->path, path);
  ent_state->data_len = statbufp->st_size;
  ent_state->data = file_data;

  return uft_status_set_success(&status, ent_state);
}


uft_status *
add_ent_symlink(uft_tx * tx, char * path, int flags, struct stat * statbufp)
{
  static uft_status status;

  char * link_data = (char *) malloc(statbufp->st_size + 1);
  if (readlink(path, link_data, statbufp->st_size) != statbufp->st_size) {
    link_data[statbufp->st_size] = '\0';
    free(link_data);
    return uft_status_set_error(&status, "error adding symlink \"%s\", failed to read: %s", path, strerror(errno));
  }

  uft_ent_state * ent_state = (uft_ent_state *) malloc(sizeof(uft_ent_state));
  ent_state->flags |= UFT_ES_SYMLINK;
  ent_state->path = (char *) malloc(strlen(path)+1);
  strcpy(ent_state->path, path);
  ent_state->data_len = statbufp->st_size;
  ent_state->data = link_data;

  return uft_status_set_success(&status, ent_state);
}


uft_status *
add_ent_noent(uft_tx * tx, char * path, int flags)
{
  static uft_status status;

  if ((flags & UFT_ALLOW_NOENT) == 0)
    return uft_status_set_error(&status, "error adding non existent entity \"%s\", set UFT_ALLOW_NOENT if this is allowed", path);

  uft_ent_state * ent_state = (uft_ent_state *) malloc(sizeof(uft_ent_state));
  ent_state->flags |= UFT_ES_NOENT;
  ent_state->path = (char *) malloc(strlen(path)+1);
  strcpy(ent_state->path, path);
  ent_state->data_len = 0;
  ent_state->data = NULL;

  return uft_status_set_success(&status, ent_state);
}


/// Rollback the transaction. Should not be called directly, it is called
/// by 'uft_tx_begin' if the transaction fails.

void
uft_tx_rollback (uft_tx * tx)
{
  uft_ll_node * lln;

  while ((lln = uft_ll_tail(tx->children)) != NULL) {
    uft_tx * child_tx = (uft_tx *) uft_ll_data(lln);
    uft_tx_rollback(child_tx);
    uft_ll_rmnode(lln);
  }

  while ((lln = uft_ll_tail(tx->ents)) != NULL) {
    uft_ent_state * ent_state = (uft_ent_state *) uft_ll_data(lln);
    if ((ent_state->flags & UFT_ES_FILE) != 0) {
      uft_rollback_file(tx, ent_state);
    } else if ((ent_state->flags & UFT_ES_SYMLINK) != 0) {
      uft_rollback_symlink(tx, ent_state);
    } else if ((ent_state->flags & UFT_ES_NOENT) != 0) {
      uft_rollback_noent(tx, ent_state);
    }
    destroy_ent_state(ent_state);
    uft_ll_rmnode(lln);
  }

  if (tx->code & UFT_TX_ROLLBACK_FAILED)
    tx->code &= ~UFT_TX_ROLLBACK_OK;
  else
    tx->code |= UFT_TX_ROLLBACK_OK;
}


void
uft_rollback_file (uft_tx * tx, uft_ent_state * ent_state)
{
  struct stat statbuf;
  if (lstat(ent_state->path, &statbuf) == 0) {
    if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
      if (rmdir(ent_state->path) != 0) {
        uft_tx_log_error(tx,
                         "rolling back transaction %p, error %d restoring file \"%s\" by rmdir: %s",
                         tx, errno, ent_state->path, strerror(errno));
        return;
      }
    } else if ((statbuf.st_mode & S_IFMT) == S_IFLNK) {
      if (unlink(ent_state->path) != 0) {
        uft_tx_log_error(tx,
                         "rolling back transaction %p, error %d restoring file \"%s\" by unlink symlink: %s",
                         tx, errno, ent_state->path, strerror(errno));
        return;
      }
    }
  }

  int fd = open(ent_state->path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    uft_tx_log_error(tx,
                     "rolling back transaction %p, error %d restoring file \"%s\": %s",
                     tx, errno, ent_state->path, strerror(errno));
    tx->code |= UFT_TX_ROLLBACK_FAILED;
  }
  if (write(fd, ent_state->data, ent_state->data_len) != ent_state->data_len) {
    uft_tx_log_error(tx,
                     "rolling back transaction %p, error %d restoring file \"%s\": %s",
                     tx, errno, ent_state->path, strerror(errno));
    tx->code |= UFT_TX_ROLLBACK_FAILED;
  }
  close(fd);
}


void
uft_rollback_symlink (uft_tx * tx, uft_ent_state * ent_state)
{
  char * buf = (char *) malloc(ent_state->data_len + 1);
  if (readlink(ent_state->path, buf, ent_state->data_len + 1) == ent_state->data_len)
    if (strncmp(buf, ent_state->data, ent_state->data_len) == 0)
      return;

  if (unlink(ent_state->path) != 0 && errno != ENOENT) {
    uft_tx_log_error(tx,
                     "rolling back transaction %p, error %d restoring symlink \"%s\": %s",
                     tx, errno, ent_state->path, strerror(errno));
    tx->code |= UFT_TX_ROLLBACK_FAILED;
  }
  if (symlink(ent_state->data, ent_state->path) != 0) {
    uft_tx_log_error(tx,
                     "rolling back transaction %p, error %d restoring symlink \"%s\": %s",
                     tx, errno, ent_state->path, strerror(errno));
    tx->code |= UFT_TX_ROLLBACK_FAILED;
  }
}


void
uft_rollback_noent (uft_tx * tx, uft_ent_state * ent_state)
{
  struct stat statbuf;
  if (lstat(ent_state->path, &statbuf) != 0 && errno != ENOENT) {
    uft_tx_log_error(tx,
                     "rolling back transaction %p, error %d restoring noent \"%s\": %s",
                     tx, errno, ent_state->path, strerror(errno));
    tx->code |= UFT_TX_ROLLBACK_FAILED;
    return;
  }

  if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
    if (rmdir(ent_state->path) != 0) {
      uft_tx_log_error(tx,
                       "rolling back transaction %p, error %d restoring noent dir \"%s\": %s",
                       tx, errno, ent_state->path, strerror(errno));
      tx->code |= UFT_TX_ROLLBACK_FAILED;
    }
  } else if (((statbuf.st_mode & S_IFMT) == S_IFREG) || ((statbuf.st_mode & S_IFMT) == S_IFLNK)) {
    if (unlink(ent_state->path) != 0 && errno != ENOENT) {
      uft_tx_log_error(tx,
                       "rolling back transaction %p, error %d restoring noent \"%s\": %s",
                       tx, errno, ent_state->path, strerror(errno));
      tx->code |= UFT_TX_ROLLBACK_FAILED;
    }
  }
}


/// Log / add an error to the transaction.

uft_tx *
uft_tx_log_error(uft_tx * tx, const char * fmt, ...)
{
  char * msg = (char *) malloc(UFT_MAX_MSG_LEN);
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, UFT_MAX_MSG_LEN, fmt, args);
  va_end(args);

  uft_tx_error * tx_error = (uft_tx_error *) malloc(sizeof(uft_tx_error));
  tx_error->msg = msg;

  uft_ll_insert_tail(tx->errors, tx_error);

  return tx;
}


/// Set the transaction as successful.

void
uft_tx_success(uft_tx * tx)
{
  tx->code |= UFT_TX_SUCCESS;

  return;
}


/// Set the transaction as failed.

void
uft_tx_fail(uft_tx * tx)
{
  tx->code |= UFT_TX_ERROR;

  return;
}


/// Return the error message of a transaction error.

char *
uft_tx_error_msg (uft_tx_error * tx_error)
{
  return tx_error->msg;
}


/// Return an array of strings containing the transactions errors. A
/// NULL means 'no more errors'. The caller must free the pointer
/// returned.

char **
uft_tx_error_msgs (uft_tx * tx)
{
  int error_count = uft_ll_count(tx->errors);
  char ** errors = (char **) malloc(sizeof(char *) * (error_count + 1));

  errors[error_count--] = NULL;
  for (uft_ll_node * lln = uft_ll_tail(tx->errors); lln != NULL; lln = uft_ll_prev(lln))
    errors[error_count--] = uft_tx_error_msg(uft_ll_data(lln));

  return errors;
}
