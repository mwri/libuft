/// Shortcuts, like hello world, but more concise.

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <uft.h>


void do_stuff (uft_tx * tx);
void dump_tx_error_log (uft_tx * tx);


int
main (int argc, char ** argv)
{
  // create a transaction
  uft_tx * tx = uft_tx_new(NULL);

  // call 'do_stuff' to execute it
  uft_tx_begin(tx, do_stuff);

  // report result of transaction
  if (uft_tx_ok(tx)) {
    printf("Transaction successful, good job!\n");
  } else {
    printf("Transaction failed!\n");
    dump_tx_error_log(tx);
  }

  // free up memory used by the transaction
  uft_tx_end(tx);
}


// This transaction writes to a file 'uft_example_shortcuts_1' (creating or
// truncating it), and then creates a directory 'uft_example_shortcuts_2'.
// BOTH these changes will happen, or neither, because it's a transaction...
//
// This is the same as uft_hello.c but less windy error checking is employed.
// The behaviour is exactly the same though. Note that the transaction does
// retain error logging internally, which can be helpful in keeping code
// more concise like this but still being able to find out what went wrong
// afterwards. See the 'dump_tx_error_log' function below, which is called
// from 'main' if the transaction fails.
//
// Note that 'uft_open' is the same as 'open', but it logs an error in the
// transaction, and fails the transaction automatically in the case of an error.
// Also 'uft_write' is the same as 'write' and 'uft_mkdir' is the same as
// 'mkdir', with the same behaviour.

void
do_stuff (uft_tx * tx)
{
  // add path to the transaction, allowing it not to exist
  if (uft_status_error(uft_tx_add_ent(tx, "uft_example_shortcuts_1", UFT_ALLOW_NOENT)))
    return uft_tx_fail(tx);
  // add a second path to the transaction
  if (uft_status_error(uft_tx_add_ent(tx, "uft_example_shortcuts_2", UFT_ALLOW_NOENT)))
    return uft_tx_fail(tx);

  // open a file and write to it
  int fd = uft_open(tx, "uft_example_shortcuts_1", O_CREAT | O_TRUNC | O_WRONLY, 0666);
  if (fd < 0)
    return;
  if (uft_write(tx, fd, "some_data\n", 10) != 10)
    return;
  close(fd);
  fprintf(stderr, "Wrote some_data to uft_example_shortcuts_1 successfully...\n");

  // create a directory
  if (uft_mkdir(tx, "uft_example_shortcuts_2", 0777) != 0)
    return;
  fprintf(stderr, "Created directory uft_example_shortcuts_2 successfully...\n");

  return uft_tx_success(tx);
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
