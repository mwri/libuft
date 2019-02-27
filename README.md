# libutf (Userspace FileSystem Transactions)

If you want to execute a series of filesystem changes (such as creating or
deleting files, directories and symbolic links) where all the changes should
succeed, and if it goes wrong half way through you want to roll back what has
been done so far leaving the filesystem apparently untouched, the UFT library
can help you.

The way a UFT transaction works is, you create a transaction, you add objects
(files, directories and symlinks) to it... and then you make your changes. If
any of your changes fail, you fail the transaction and everthing you added to
the transaction is put back the way it was for you.

There are a few things to bear in mind:

 * If you crash during a transaction, that's it, game over; no roll back.
 * If you don't add an object to the transaction, the transaction doesn't know about it, and if you change it then it won't be rolled back, so make sure you include all the objects your transaction affects.
 * You can add non existent paths, and they will be deleted (if they now exist) by any rollback that takes place.

Transactions may be nested, which means a transaction may initiate a child
transaction, and if that child transaction fails, it is up to the parent if
it fails, or continues, potentially still completing successfully.

## Build

Running `libtoolize` followed by `autoreconf -i` followed by `./configure`
and finally `make` should result in a successful build.

## Quick start

Here a transaction is created and run, and the result printed to standard
out:

```c
#include <uft.h>

// create a transaction, call `tx_work_1` to execute it passing `user_ptr` to it
uft * tx = uft_tx(&tx_work, user_ptr);
if (uft_tx_success(tx)) {
  printf("successful\n");
} else {
  printf("failed\n");
}
```

The actual work of the transaction is carried out by the function `tx_work`
and `user_ptr` is pointer that is passed to the transaction, if you need to
pass your own data in.

The transaction in this case looks something like this:

```c
uft_status * tx_work_1 (uft * tx, char ** user_ptr) {
  if (uft_status_error(uft_tx_add_ent(tx, "some/file", 0)))
    return uft_tx_fail(tx);

  if (uft_status_error(uft_tx_add_ent(tx, "some/directory", UFT_ALLOW_NOENT)))
    return uft_tx_fail(tx);

  /*** create a new directory "some/directory" ***/

  if (any_errors)
    return uft_tx_fail(tx);

  /*** edit file "some/file" ***/

  if (any_errors)
    return uft_tx_fail(tx);

  return uft_tx_success(tx_);
}
```

There are three fully commented examples available in the `examples`
directrory, see the [examples README](examples/README.md) for build and
run instructions.

## Contents

1. [Quick start](#quick-start).
2. [API](#api).

   1. [Creating transactions](#creating-transactions).
      1. [uft_tx_new](#uft_tx_new).
      2. [uft_tx_begin](#uft_tx_begin).
      3. [uft_tx_end](#uft_tx_end).
   2. [Usually run inside a transaction](#usually-run-inside-a-transaction).
      1. [uft_tx_id](#uft_tx_id).
      2. [uft_tx_success](#uft_tx_success).
      3. [uft_tx_fail](#uft_tx_fail).
      4. [uft_tx_log_error](#uft_tx_log_error).
      5. [uft_tx_add_ent](#uft_tx_add_ent).
      6. [uft_tx_extra](#uft_tx_extra).
      7. [uft_tx_set_extra](#uft_tx_set_extra).
      8. [uft_tx_child](#uft_tx_child).
   3. [Transaction result inspection](#transaction-result-inspection).
      1. [uft_tx_ok](#uft_tx_ok).
      2. [uft_tx_rollback_ok](#uft_tx_rollback_ok).
      3. [uft_tx_rollback_failed](#uft_tx_rollback_failed).
      4. [uft_tx_rollback_attempted](#uft_tx_rollback_attempted).
      5. [uft_tx_error_msgs](#uft_tx_error_msgs).

## API

### Creating transactions

#### uft_tx_new

`uft_tx * uft_tx_new (void * extra)`

Create a new transaction. The `extra` parameter is passed to the transaction
so that you can have access to any necessary working data. Set it to `NULL`
if you don't need it.

#### uft_tx_begin

`uft_tx * uft_tx_begin (uft_tx * tx, void (*txfp)(uft_tx *))`

Actually run the transaction. You must pass a `void (*txfp)(uft_tx *)`
function, which is called to carry out the function work. This function
must ultimately mark the transaction as succeeded or failed, and if it
is marked failed then the filesystem should be unchanged.

#### uft_tx_end

`void uft_tx_end (uft_tx * tx)`

Call this to clean up any memory after you've entirely finished with
the transaction. You may not call any other functions with this `tx`
after this call.

### Usuaully run inside a transaction

#### uft_tx_id

`int uft_tx_id (uft_tx * tx)`

Return an integer ID for the transaction. Each transaction will have
a unique ID, they start at zero and go up.

#### uft_tx_success

`void uft_tx_success (uft_tx * tx)`

Mark the transaction as successful.

#### uft_tx_fail

`void uft_tx_fail (uft_tx * tx)`

Mark the transaction as failed.

#### uft_tx_log_error

`uft_tx * uft_tx_log_error (uft_tx * tx, const char * fmt, ...)`

Add a an error message to the transaction. Usually this would
proceed a call to `uft_tx_fail` but it doesn't have to. The first parameter
is the transaction obviously, and the following parameters are printf
style, so `uft_tx_log_error(tx, "ick, cant do file %s: %s", path, errmsg)`.

#### uft_tx_add_ent

`uft_status * uft_tx_add_ent (uft_tx * tx, char * path, int flags)`

Add a path to the transaction. If
the transaction is marked as failed when it returns then this path will
be returned to how it is when `uft_tx_add_ent` is called.

#### uft_tx_extra

`void * uft_tx_extra (uft_tx * tx)`

Get the 'extra' or 'user' data (the pointer passed to
`uft_tx_new`).

#### uft_tx_set_extra

`void * uft_tx_set_extra (uft_tx * tx, void * extra)`

SET the extra data. This can be used to return data to the
caller, though setting data in a structure passed in is more usually
expected.

#### uft_tx_child

`uft_tx * uft_tx_child (uft_tx * tx, void * extra)`

Run a subordinate transaction, which will be rolled back if the calling
parent transaction is rolled back, but may or may not cause the parent
to fail (that's up to the parent).

For example:

```c
uft_tx * child_tx = uft_tx_child(tx, extra_ptr);
uft_tx_begin(child_tx, do_stuff_2);
if (uft_tx_ok(child_tx)) {
  printf("Child transaction successful!\n");
} else {
  printf("Child transaction failed!\n");
}
```

The parent transaction can take whatever action is required on the
failure of the child, including calling `utf_tx_fail()` to fail
itself, though if it always fails as a result of the child failure
then really there's not much point in the childs operations being
a child transaction; the operations could all be in the parent.

### Transaction result inspection

#### uft_tx_ok

`int uft_tx_ok (uft_tx * tx)`

Return true if the transaction was OK (succesful).

#### uft_tx_rollback_ok

`int uft_tx_rollback_ok (uft_tx * tx)`

Return true if the transaction was rolled back and the roll back was
successful.

#### uft_tx_rollback_failed

`int uft_tx_rollback_failed (uft_tx * tx)`

Return true if the transaction was rolled back and the roll back
met with an error. The filesystem will be in an unknown state in
this case.

#### uft_tx_rollback_attempted

`int uft_tx_rollback_attempted (uft_tx * tx)`

Return true if the transaction was rolled back (successfully or
not).

#### uft_tx_error_msgs

`char ** uft_tx_error_msgs (uft_tx * tx)`

Return an array of strings, which are the errors (if any) accrued
during the transaction. Any errors added with `uft_tx_log_error`
will appear in this list, but other entries may be added implicitly
by failures (for example from calls to `uft_tx_add_ent`).

You must call `free()` on the result when you are done with it
and you must not call `uft_tx_end()` until it is freed (or at
least not access it after you do).
