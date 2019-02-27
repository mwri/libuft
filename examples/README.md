# libuft examples

Compile the examples in the *examples* directory (after building *libuft*):

```sh
make
```

## uft_hello

This is more or less as slim a hello world as is actually useful (though
the code can be a lot more concise (see [uft_shortcuts](#uft_shortcuts)). There
are two filesystem operations, the creation or truncation and write of
a file, and the creation of a directory. They both succeeed or they
both fail.

Running it the first time should definitely work fine:

```sh
$ ./uft_hello
Wrote some_data to uft_example_hello_1 successfully...
Created directory uft_example_hello_2 successfully...
Transaction successful, good job!
$
```

Running it again will fail, because of the changes made the first time:

```sh
$ ./uft_hello
Error adding uft_example_hello_2 to transaction: cannot add existing directory "uft_example_hello_2" to transaction
Transaction failed! :(
$
```

## uft_shortcuts

This is essentially the same as the above hello world example, but it
demonstrates more concise code. Though the code is less verbose, the
transaction is functionally identical. It also demonstrates getting
the error log entries from the transaction, which is more important
in this case.

Running the transaction the first time succeeds:

```sh
$ ./uft_shortcuts
Wrote some_data to uft_example_shortcuts_1 successfully...
Created directory uft_example_shortcuts_2 successfully...
Transaction successful, good job!
$
```

Running it again fails calling `uft_tx_add_ent(tx, "uft_example_shortcuts_2", UFT_ALLOW_NOENT)`:

```sh
$ ./uft_shortcuts
Transaction failed!

Tx_00 error 00 - cannot add existing directory "uft_example_shortcuts_2" to transaction
$
```

Now, resetting the transaction:

```sh
$ rm uft_example_shortcuts_1
$ rmdir uft_example_shortcuts_2
```

We can now interfere with the creation of *uft_example_shortcuts_2* by
creating a file of that name:

```sh
$ touch uft_example_shortcuts_2
$ ./uft_shortcuts
Wrote some_data to uft_example_shortcuts_1 successfully...
Transaction failed!

Tx_00 error 00 - error creating directory uft_example_shortcuts_2: File exists
$
```

There can be any number of errors, not just one as in this cases, because
an error does not have to fail a transaction, it may be recoverable, or
even if it isn't multiple error messages may be useful. A transaction can
add an error to the transaction by calling `uft_tx_log_error(uft_tx *, char *, ...)`.

## uft_nested

This is an example of two nested transactions.

Run the example:

```sh
./uft_nested
```

The result, if this is the first time you are running it, should be a
transaction failure like this:

```sh
$ ./uft_nested
Transaction failed!

Logs:
  Tx00: error adding non existent entity "uft_example_nested_3", set UFT_ALLOW_NOENT if this is allowed
$
```

This is caused by the exanple adding `uft_example_nested_3` to the
transaction, and it doesn't exist. Adding non existent objects can be done
but you have to specify with a flag that it is allowed not to exist.

To make the transaction happy, create a file of that name and try again:

```sh
$ echo 1 > uft_example_nested_3
$ ./uft_nested
Transaction successful, good job!
$
```

Now, as a result of the transaction being successful a `uft_example_nested_1`
directory should have been created.
