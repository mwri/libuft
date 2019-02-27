/// libuft top level / misc functions


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "uft.h"


/// Pass through to mkdir, but fail the transaction and log a transactional
/// error if the operation fails.

int
uft_mkdir (uft_tx * tx, char * path, int mode)
{
  int retval = mkdir(path, mode);

  if (retval != 0) {
    uft_tx_log_error(tx, "error creating directory %s: %s", path, strerror(errno));
    uft_tx_fail(tx);
  }

  return retval;
}


/// Pass through to open, but fail the transaction and log a transactional
/// error if the operation fails.

int uft_open (uft_tx * tx, char * path, int flags, mode_t mode)
{
  int retval = open(path, flags, mode);

  if (retval < 0) {
    uft_tx_log_error(tx, "error opening %s (flags %d): %s", path, flags, strerror(errno));
    uft_tx_fail(tx);
  }

  return retval;
}


/// Pass through to read, but fail the transaction and log a transactional
/// error if the operation fails.

int uft_read (uft_tx * tx, int fd, char * buf, int len)
{
  int retval = read(fd, buf, len);

  if (retval < 0) {
    uft_tx_log_error(tx, "error writing to FD %d (%d of %d bytes): %s", fd, retval, len, strerror(errno));
    uft_tx_fail(tx);
  }

  return retval;
}


/// Pass through to write, but fail the transaction and log a transactional
/// error if the operation fails.

int uft_write (uft_tx * tx, int fd, char * buf, int len)
{
  int retval = write(fd, buf, len);

  if (retval != len) {
    uft_tx_log_error(tx, "error writing to FD %d (%d of %d bytes): %s", fd, retval, len, strerror(errno));
    uft_tx_fail(tx);
  }

  return retval;
}
