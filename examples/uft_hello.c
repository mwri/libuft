/// Obligatory hello world example.

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <uft.h>


void do_stuff (uft_tx * tx);


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
    printf("Transaction failed! :(\n");
  }

  // free up memory used by the transaction
  uft_tx_end(tx);
}


// This transaction writes to a file 'uft_example_hello_1' (creating or
// truncating it), and then creates a directory 'uft_example_hello_2'.
// BOTH these changes will happen, or neither, because it's a transaction...

void
do_stuff (uft_tx * tx)
{
  // add path to the transaction, allowing it not to exist
  if (uft_status_error(uft_tx_add_ent(tx, "uft_example_hello_1", UFT_ALLOW_NOENT))) {
    fprintf(stderr, "Error adding uft_example_hello_1 to transaction\n");
    uft_tx_fail(tx);
    return;
  }
  // add a second path to the transaction
  uft_status * status = uft_tx_add_ent(tx, "uft_example_hello_2", UFT_ALLOW_NOENT);
  if (uft_status_error(status)) {
    fprintf(stderr, "Error adding uft_example_hello_2 to transaction: %s\n", uft_status_error_msg(status));
    uft_tx_fail(tx);
    return;
  }

  // open a file and write to it
  int fd = open("uft_example_hello_1", O_CREAT | O_TRUNC | O_WRONLY, 0666);
  if (fd < 0) {
    fprintf(stderr, "Error opening file uft_example_hello_1: %s\n", strerror(errno));
    uft_tx_fail(tx);
    return;
  }
  if (write(fd, "some_data\n", 10) != 10) {
    fprintf(stderr, "Error writing to file uft_example_hello_1: %s\n", strerror(errno));
    uft_tx_fail(tx);
    return;
  }
  close(fd);
  fprintf(stderr, "Wrote some_data to uft_example_hello_1 successfully...\n");

  // create a directory
  if (mkdir("uft_example_hello_2", 0777) != 0) {
    fprintf(stderr, "Error creating directory uft_example_hello_2: %s\n", strerror(errno));
    uft_tx_fail(tx);
    return;
  }
  fprintf(stderr, "Created directory uft_example_hello_2 successfully...\n");

  return uft_tx_success(tx);
}
