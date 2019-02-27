/// Example of a tested transaction.

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <uft.h>


void do_stuff_1 (uft_tx * tx);
void do_stuff_2 (uft_tx * tx);
void dump_tx_error_log (uft_tx * tx);


int
main (int argc, char ** argv)
{
  // arguments affect what happens in this rather contrived
  // and rigged to explode example nested transaction
  int extra = argc == 2
    ? atoi(argv[1])
    : 0;

  // create a transaction
  uft_tx * tx = uft_tx_new(&extra);

  // call `do_stuff_1` to execute it
  uft_tx_begin(tx, do_stuff_1);

  // report result of transaction
  if (uft_tx_ok(tx)) {
    printf("Main transaction successful, good job!\n");
  } else {
    printf("Main transaction failed!\n");
    dump_tx_error_log(tx);
  }

  // free up memory used by the transaction
  uft_tx_end(tx);
}


// This transaction creates a directory 'uft_example_nested_1', then  truncates and
// writes 7 bytes "foobar\n" to file 'uft_example_nested_2'. Then it executres a
// child transaction (see the 'do_stuff_2' function). Then it reads a counter from
// 'uft_example_nested_3', increments it and writes it back.
//
// Note that all the operations in the child transaction should happen, or none of
// them (or rather they will but they might be put back in the case of a failure).
// If the parent ultimately fails, then the child transaction will also be rolled
// back.
//
// It is rigged to explode if bit 1 (LSB << 1) of the extra data is set, which can
// be triggered by running 'uft_nested' with an argument of '2' or '3'.
//
// If the transaction is failed, then the contents of all
// will revert to what it was before it was added to this child transaction.

void
do_stuff_1 (uft_tx * tx)
{
  int fd;
  int read_len;
  uft_status * status;

  // set the transaction as successful by default
  uft_tx_success(tx);

  // get the 'extra', or 'user', data
  int * extra_ptr = uft_tx_extra(tx);

  // add path to the transaction, allowing it not to exist
  status = uft_tx_add_ent(tx, "uft_example_nested_1", UFT_ALLOW_NOENT);
  if (uft_status_error(status))
    // if adding the path to the transaction fails, fail the transaction and return
    return uft_tx_fail(tx);

  // add a second path to the transaction, also allowing it not to exist
  status = uft_tx_add_ent(tx, "uft_example_nested_2", UFT_ALLOW_NOENT);
  if (uft_status_error(status))
    // if adding the path to the transaction fails, fail the transaction and return
    return uft_tx_fail(tx);

  // add a third path to the transaction, this time expecting it to exist
  status = uft_tx_add_ent(tx, "uft_example_nested_3", 0);
  if (uft_status_error(status))
    // if adding the path to the transaction fails, fail the transaction and return
    return uft_tx_fail(tx);

  // now lets do some things to affect the filesystem, first create a directory
  // note that 'uft_mkdir' is exactly the same as 'mkdir', it just logs an error
  // in the transaction and fails it; it's a handy short hand, nothing special
  // about it
  if (uft_mkdir(tx, "uft_example_nested_1", 0777) != 0)
    return;

  // now open a file, again 'uft_open' is really just 'open' but a call to
  // 'uft_tx_log_error' and 'uft_tx_fail' is made for you if there is an error
  if ((fd = uft_open(tx, "uft_example_nested_2", O_CREAT | O_TRUNC | O_WRONLY, 0666)) < 0)
    return;
  // write to the file, here 'uft_write' could be used but 'write' is ued for
  // demonstration purposes only
  if (write(fd, "foobar\n", 7) != 7) {
    uft_tx_log_error(tx, "error writing to uft_example_nested_2: %s", strerror(errno));
    uft_tx_fail(tx);
    return;
  }
  // close the file
  close(fd);

  // now lets create a child transaction, which is done very like this transaction
  // was created, but it is linked, so that the child can be rolled back if the
  // parent is
  uft_tx * child_tx = uft_tx_child(tx, extra_ptr);
  uft_tx_begin(child_tx, do_stuff_2);
  if (uft_tx_ok(child_tx)) {
    printf("Child transaction successful!\n");
  } else {
    printf("Child transaction failed!\n");
  }

  // now open, read and write the other file, to increment the number in it
  if ((fd = uft_open(tx, "uft_example_nested_3", O_RDONLY, 0666)) < 0)
    return;
  char buf[10];
  read_len = uft_read(tx, fd, buf, 10);
  if (read_len < 0)
    return;
  close(fd);
  buf[read_len] = '\0';
  int count = atoi(buf);
  sprintf(buf, "%d\n", count + 1);
  if ((fd = uft_open(tx, "uft_example_nested_3", O_TRUNC | O_WRONLY, 0666)) < 0)
    return;
  if (uft_write(tx, fd, buf, strlen(buf)) != strlen(buf))
    return;
  close(fd);

  if (*extra_ptr & 0x02) {
    uft_tx_log_error(tx, "deliberate failure at end of main transaction");
    uft_tx_fail(tx);
  }

  return;
}


// This transaction simply truncates and writes to file 'uft_example_nested_2'
// 4 bytes "xxx\n".
//
// It is rigged to explode if the LSB bit of the extra data is set, which can
// be triggered by running 'uft_nested' with an argument of '1' or '3'.
//
// If the transaction is failed, then the contents of 'uft_example_nested_2'
// will revert to what it was before it was added to this child transaction.

void
do_stuff_2 (uft_tx * tx)
{
  int fd;

  // set the transaction as successful by default
  uft_tx_success(tx);

  // get the 'extra', or 'user', data
  int * extra_ptr = uft_tx_extra(tx);

  // add two paths to the transaction, one of which is involved in the parent
  // transaction, and one of which is unknown to it
  if (uft_status_error(uft_tx_add_ent(tx, "uft_example_nested_2", 0)))
    return uft_tx_fail(tx);
  if (uft_status_error(uft_tx_add_ent(tx, "uft_example_nested_4", UFT_ALLOW_NOENT)))
    return uft_tx_fail(tx);

  // now open the file and write to it
  if ((fd = uft_open(tx, "uft_example_nested_2", O_CREAT | O_TRUNC | O_WRONLY, 0666)) < 0)
    return;
  if (uft_write(tx, fd, "xxx\n", 4) != 4)
    return;
  close(fd);

  // get rid of an existing 'uft_example_nested_4' directory, file or symlink
  // and create a symlink
  rmdir("uft_example_nested_4");
  unlink("uft_example_nested_4");
  symlink(".", "uft_example_nested_4");

  if (*extra_ptr & 0x01) {
    uft_tx_log_error(tx, "deliberate failure at end of child transaction");
    uft_tx_fail(tx);
  }

  return;
}


// Dump out the errors accumulated during the transaction.

void
dump_tx_error_log (uft_tx * tx)
{
  char ** errors = uft_tx_error_msgs(tx);

  fprintf(stderr, "\n");
  for (int i = 0; errors[i] != NULL; i++)
    fprintf(stderr, "Tx_%02d error %02d - %s\n", uft_tx_id(tx), i, errors[i]);

  free(errors);
}
