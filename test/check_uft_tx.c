
#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "uft_check.h"

#include "uft.h"
#include "uft_tx.h"
#include "uft_ll.h"
#include "uft_status.h"


int g_txfp_called;
uft_tx * g_tx;


void
tx_do_nothing (uft_tx * tx)
{
  g_txfp_called++;

  return;
}


void
tx_do_fail (uft_tx * tx)
{
  g_txfp_called++;

  uft_tx_fail(tx);

  return;
}


void
tx_do_succeed (uft_tx * tx)
{
  g_txfp_called++;

  uft_tx_success(tx);

  return;
}


void
tx_do_fail_with_error_msg (uft_tx * tx)
{
  g_txfp_called++;

  uft_tx_fail(uft_tx_log_error(tx, "broke"));

  return;
}


void
tx_do_fail_with_two_error_msgs (uft_tx * tx)
{
  g_txfp_called++;

  uft_tx_fail(uft_tx_log_error(tx, "broke"));
  uft_tx_fail(uft_tx_log_error(tx, "badly"));

  return;
}


void
tx_do_fail_with_file_edit (uft_tx * tx)
{
  g_txfp_called++;

  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_file1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_symlink1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir3", UFT_ALLOW_NOENT)));

  int fd = open(".test_dir2/test_file1.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  ck_assert_msg(fd >= 0, strerror(errno));
  ck_assert_msg(write(fd, "abc\ndef\nghi\n", 12) == 12, strerror(errno));
  close(fd);

  uft_tx_fail(tx);

  return;
}


void
tx_do_fail_with_file_rm (uft_tx * tx)
{
  g_txfp_called++;

  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_file1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_symlink1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir3", UFT_ALLOW_NOENT)));

  ck_assert_msg(unlink(".test_dir2/test_file1.txt") == 0, strerror(errno));

  uft_tx_fail(tx);

  return;
}


void
tx_do_fail_with_file_dir_replace (uft_tx * tx)
{
  g_txfp_called++;

  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_file1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_symlink1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir3", UFT_ALLOW_NOENT)));

  ck_assert_msg(unlink(".test_dir2/test_file1.txt") == 0, strerror(errno));
  ck_assert_msg(mkdir(".test_dir2/test_file1.txt", 0755) == 0, strerror(errno));

  uft_tx_fail(tx);

  return;
}


void
tx_do_fail_with_file_symlink_replace (uft_tx * tx)
{
  g_txfp_called++;

  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_file1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_symlink1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir3", UFT_ALLOW_NOENT)));

  ck_assert_msg(unlink(".test_dir2/test_file1.txt") == 0, strerror(errno));
  ck_assert_msg(symlink("..", ".test_dir2/test_file1.txt") == 0, strerror(errno));

  uft_tx_fail(tx);

  return;
}


void
tx_do_fail_with_symlink_edit (uft_tx * tx)
{
  g_txfp_called++;

  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_file1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_symlink1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir3", UFT_ALLOW_NOENT)));

  ck_assert_msg(unlink(".test_dir2/test_symlink1.txt") == 0, strerror(errno));
  ck_assert_msg(symlink("..", ".test_dir2/test_symlink1.txt") == 0, strerror(errno));

  uft_tx_fail(tx);

  return;
}


void
tx_do_fail_with_symlink_rm (uft_tx * tx)
{
  g_txfp_called++;

  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_file1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_symlink1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir3", UFT_ALLOW_NOENT)));

  ck_assert_msg(unlink(".test_dir2/test_symlink1.txt") == 0, strerror(errno));

  uft_tx_fail(tx);

  return;
}


void
tx_do_fail_with_symlink_replacement (uft_tx * tx)
{
  g_txfp_called++;

  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_file1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_symlink1.txt", 0)));
  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir3", UFT_ALLOW_NOENT)));

  ck_assert_msg(unlink(".test_dir2/test_symlink1.txt") == 0, strerror(errno));
  int fd = open(".test_dir2/test_file1.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  ck_assert_msg(fd >= 0, strerror(errno));
  ck_assert_msg(write(fd, "blah\n", 5) == 5, strerror(errno));
  close(fd);

  uft_tx_fail(tx);

  return;
}


void
tx_do_fail_with_new_file (uft_tx * tx)
{
  g_txfp_called++;

  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".test_dir2/test_file2.txt", UFT_ALLOW_NOENT)));

  int fd = open(".test_dir2/test_file2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  ck_assert_msg(fd >= 0, strerror(errno));
  ck_assert_msg(write(fd, "blah\n", 5) == 5, strerror(errno));
  close(fd);

  struct stat statbuf;
  ck_assert(lstat(".test_dir2/test_file2.txt", &statbuf) == 0);
  ck_assert_int_eq((statbuf.st_mode & S_IFMT), S_IFREG);

  uft_tx_fail(tx);

  return;
}


void
tx_do_fail_with_new_dir (uft_tx * tx)
{
  g_txfp_called++;

  ck_assert(uft_status_success(uft_tx_add_ent(tx, ".no_test_dir2", UFT_ALLOW_NOENT)));

  ck_assert_msg(mkdir(".no_test_dir2", 0755) == 0);

  struct stat statbuf;
  ck_assert(lstat(".no_test_dir2", &statbuf) == 0);
  ck_assert_int_eq((statbuf.st_mode & S_IFMT), S_IFDIR);

  uft_tx_fail(tx);

  return;
}

void
tx_do_child_ok_parent_fail (uft_tx * tx)
{
  void * extra = uft_tx_extra(tx);

  uft_tx * child_tx = uft_tx_child(tx, extra);
  uft_tx_begin(child_tx, tx_do_succeed);

  uft_tx_set_extra(tx, child_tx);

  uft_tx_fail(tx);
}


// Tests.

START_TEST (test_new_tx_not_null)
{
  uft_tx * tx = uft_tx_new(NULL);
  ck_assert(tx != NULL);
}
END_TEST


START_TEST (test_new_tx_stores_extra_data)
{
  uft_tx * tx = uft_tx_new((void *) 100);
  ck_assert(tx->extra == (void *) 100);
}
END_TEST


START_TEST (test_extra_returns_extra_data)
{
  uft_tx * tx = uft_tx_new((void *) 123456);
  ck_assert_ptr_eq(uft_tx_extra(tx), (void *) 123456);
}
END_TEST


START_TEST (test_begin_tx_returns_tx)
{
  uft_tx * tx = uft_tx_begin(g_tx, tx_do_nothing);
  ck_assert(tx == g_tx);
}
END_TEST


START_TEST (test_begin_tx_calls_exec_fun)
{
  uft_tx_begin(g_tx, tx_do_nothing);
  ck_assert(g_txfp_called == 1);
}
END_TEST


START_TEST (test_begin_tx_rollback_when_no_update)
{
  uft_tx_begin(g_tx, tx_do_nothing);
  ck_assert(uft_tx_rollback_attempted(g_tx));
}
END_TEST


START_TEST (test_begin_tx_rollback_when_failed)
{
  uft_tx_begin(g_tx, tx_do_fail);
  ck_assert(uft_tx_rollback_attempted(g_tx));
}
END_TEST


START_TEST (test_begin_tx_no_rollback_when_succeeded)
{
  uft_tx_begin(g_tx, tx_do_succeed);
  ck_assert(uft_tx_ok(g_tx));
}
END_TEST


START_TEST (test_begin_tx_error_msg_recorded)
{
  uft_tx_begin(g_tx, tx_do_fail_with_error_msg);
  ck_assert(uft_ll_count(g_tx->errors) == 1);
  ck_assert_str_eq((char *) uft_tx_error_msg(uft_ll_data(uft_ll_nth(g_tx->errors, 0))), "broke");
}
END_TEST


START_TEST (test_begin_tx_multiple_error_msgs_recorded)
{
  uft_tx_begin(g_tx, tx_do_fail_with_two_error_msgs);
  ck_assert(uft_ll_count(g_tx->errors) == 2);
  ck_assert_str_eq((char *) uft_tx_error_msg(uft_ll_data(uft_ll_nth(g_tx->errors, 0))), "broke");
  ck_assert_str_eq((char *) uft_tx_error_msg(uft_ll_data(uft_ll_nth(g_tx->errors, 1))), "badly");
}
END_TEST


START_TEST (test_add_ent_existing_dir_fails)
{
  uft_status * status = uft_tx_add_ent(g_tx, ".test_dir1", 0);
  ck_assert(uft_status_error(status));
}
END_TEST


START_TEST (test_add_ent_nonexisting_fails_without_flag)
{
  uft_status * status = uft_tx_add_ent(g_tx, ".no_test_dir1", 0);
  ck_assert(uft_status_error(status));
}
END_TEST


START_TEST (test_add_ent_nonexisting_succeeds)
{
  uft_status * status = uft_tx_add_ent(g_tx, ".no_test_dir1", UFT_ALLOW_NOENT);
  ck_assert(uft_status_success(status));
}
END_TEST


START_TEST (test_add_ent_existing_file_succeeds)
{
  uft_status * status = uft_tx_add_ent(g_tx, ".test_dir2/test_file1.txt", 0);
  ck_assert(uft_status_success(status));
}
END_TEST


START_TEST (test_add_ent_existing_file_adds_file)
{
  uft_status * status = uft_tx_add_ent(g_tx, ".test_dir2/test_file1.txt", 0);
  ck_assert(uft_ll_count(g_tx->ents) == 1);
  uft_ent_state * ent_state = uft_ll_data(uft_ll_nth(g_tx->ents, 0));
  ck_assert((ent_state->flags & UFT_ES_FILE) == UFT_ES_FILE);
}
END_TEST


START_TEST (test_add_ent_existing_file_records_file_data)
{
  uft_status * status = uft_tx_add_ent(g_tx, ".test_dir2/test_file1.txt", 0);
  ck_assert(uft_ll_count(g_tx->ents) == 1);
  uft_ent_state * ent_state = uft_ll_data(uft_ll_nth(g_tx->ents, 0));
  ck_assert((ent_state->flags & UFT_ES_FILE) == UFT_ES_FILE);
  ck_assert(ent_state->data_len == 12);
  ck_assert(strncmp(ent_state->data, "foo\nbar\nbaz\n", ent_state->data_len) == 0);
}
END_TEST


START_TEST (test_add_ent_existing_symlink_succeeds)
{
  uft_status * status = uft_tx_add_ent(g_tx, ".test_dir2/test_symlink1.txt", 0);
  ck_assert(uft_status_success(status));
}
END_TEST


START_TEST (test_add_ent_existing_symlink_adds_symlink)
{
  uft_status * status = uft_tx_add_ent(g_tx, ".test_dir2/test_symlink1.txt", 0);
  ck_assert(uft_ll_count(g_tx->ents) == 1);
  uft_ent_state * ent_state = uft_ll_data(uft_ll_nth(g_tx->ents, 0));
  ck_assert((ent_state->flags & UFT_ES_SYMLINK) == UFT_ES_SYMLINK);
}
END_TEST


START_TEST (test_add_ent_existing_symlink_records_linkdest)
{
  uft_status * status = uft_tx_add_ent(g_tx, ".test_dir2/test_symlink1.txt", 0);
  ck_assert(uft_ll_count(g_tx->ents) == 1);
  uft_ent_state * ent_state = uft_ll_data(uft_ll_nth(g_tx->ents, 0));
  ck_assert((ent_state->flags & UFT_ES_SYMLINK) == UFT_ES_SYMLINK);
  ck_assert(ent_state->data_len == strlen("test_file1.txt"));
  ck_assert(strncmp(ent_state->data, "test_file1.txt", ent_state->data_len) == 0);
}
END_TEST


START_TEST (test_failure_rolls_back_changed_files)
{
  uft_tx * tx = uft_tx_begin(g_tx, tx_do_fail_with_file_edit);

  ck_assert(uft_tx_rollback_ok(tx));

  int fd = open(".test_dir2/test_file1.txt", O_RDONLY);
  ck_assert(fd >= 0);
  char buf[12];
  ck_assert_int_eq(read(fd, buf, 12), 12);
  close(fd);
  ck_assert(strncmp(buf, "foo\nbar\nbaz\n", 12) == 0);
}
END_TEST


START_TEST (test_failure_rolls_back_deleted_files)
{
  uft_tx * tx = uft_tx_begin(g_tx, tx_do_fail_with_file_rm);

  ck_assert(uft_tx_rollback_ok(tx));

  int fd = open(".test_dir2/test_file1.txt", O_RDONLY);
  ck_assert(fd >= 0);
  char buf[12];
  ck_assert_int_eq(read(fd, buf, 12), 12);
  close(fd);
  ck_assert(strncmp(buf, "foo\nbar\nbaz\n", 12) == 0);
}
END_TEST


START_TEST (test_failure_rolls_back_dir_replaced_files)
{
  uft_tx * tx = uft_tx_begin(g_tx, tx_do_fail_with_file_dir_replace);

  ck_assert(uft_tx_rollback_ok(tx));

  int fd = open(".test_dir2/test_file1.txt", O_RDONLY);
  ck_assert(fd >= 0);
  char buf[12];
  ck_assert_int_eq(read(fd, buf, 12), 12);
  close(fd);
  ck_assert(strncmp(buf, "foo\nbar\nbaz\n", 12) == 0);
}
END_TEST


START_TEST (test_failure_rolls_back_symlink_replaced_files)
{
  uft_tx * tx = uft_tx_begin(g_tx, tx_do_fail_with_file_symlink_replace);

  ck_assert(uft_tx_rollback_ok(tx));

  int fd = open(".test_dir2/test_file1.txt", O_RDONLY);
  ck_assert(fd >= 0);
  char buf[12];
  ck_assert_int_eq(read(fd, buf, 12), 12);
  close(fd);
  ck_assert(strncmp(buf, "foo\nbar\nbaz\n", 12) == 0);
}
END_TEST


START_TEST (test_failure_rolls_back_changed_symlinks)
{
  uft_tx * tx = uft_tx_begin(g_tx, tx_do_fail_with_symlink_edit);

  ck_assert(uft_tx_rollback_ok(tx));

  char buf[15];
  ck_assert(readlink(".test_dir2/test_symlink1.txt", buf, 15) == 14);
  buf[14] = '\0';
  ck_assert_str_eq(buf, "test_file1.txt");
}
END_TEST


START_TEST (test_failure_rolls_back_replaced_symlinks)
{
  uft_tx * tx = uft_tx_begin(g_tx, tx_do_fail_with_symlink_replacement);

  ck_assert(uft_tx_rollback_ok(tx));

  char buf[15];
  ck_assert(readlink(".test_dir2/test_symlink1.txt", buf, 15) == 14);
  buf[14] = '\0';
  ck_assert_str_eq(buf, "test_file1.txt");
}
END_TEST


START_TEST (test_failure_rolls_back_deleted_symlinks)
{
  uft_tx * tx = uft_tx_begin(g_tx, tx_do_fail_with_symlink_rm);

  ck_assert(uft_tx_rollback_ok(tx));

  char buf[15];
  ck_assert(readlink(".test_dir2/test_symlink1.txt", buf, 15) == 14);
  buf[14] = '\0';
  ck_assert_str_eq(buf, "test_file1.txt");
}
END_TEST


START_TEST (test_failure_rolls_back_noent)
{
  uft_tx * tx = uft_tx_begin(g_tx, tx_do_fail_with_new_file);

  ck_assert(uft_tx_rollback_ok(tx));

  struct stat statbuf;
  ck_assert(lstat(".test_dir2/test_file2.txt", &statbuf) != 0);
  ck_assert_int_eq(errno, ENOENT);
}
END_TEST


START_TEST (test_failure_rolls_back_noent_with_mkdir)
{
  uft_tx * tx = uft_tx_begin(g_tx, tx_do_fail_with_new_dir);

  ck_assert(uft_tx_rollback_ok(tx));

  struct stat statbuf;
  ck_assert(lstat(".no_test_dir2", &statbuf) != 0);
  ck_assert_int_eq(errno, ENOENT);
}
END_TEST


START_TEST (test_rollback_rolls_back_children)
{
  uft_tx * tx = uft_tx_begin(g_tx, tx_do_child_ok_parent_fail);

  ck_assert(uft_tx_rollback_ok(tx));
  uft_tx * child_tx = uft_tx_extra(tx);
  ck_assert(uft_tx_rollback_ok(child_tx));
}
END_TEST

START_TEST (test_error_msgs_returns_logged_errors)
{
  uft_tx_begin(g_tx, tx_do_fail_with_two_error_msgs);
  char ** errors = uft_tx_error_msgs(g_tx);
  ck_assert_str_eq(errors[0], "broke");
  ck_assert_str_eq(errors[1], "badly");
  ck_assert_ptr_eq(errors[2], NULL);
  free(errors);
}
END_TEST


void
setup_new (void)
{
  g_tx = uft_tx_new(NULL);
  fprintf(stderr, "transaction %d created\n", uft_tx_id(g_tx));
  g_txfp_called = 0;
}


void
teardown_new (void)
{
  uft_ll_node * lln;
  for (int i = 0; lln = uft_ll_nth(g_tx->errors, i++); lln != NULL)
    fprintf(stderr, "transaction %d error %d: %s\n", uft_tx_id(g_tx), i, uft_tx_error_msg(uft_ll_data(lln)));
  uft_tx_end(g_tx);
}


void
setup_test_files (void)
{
  if (mkdir(".test_dir1", 0755) != 0) {
    perror("mkdir .test_dir1");
  }
  if (mkdir(".test_dir2", 0755) != 0) {
    perror("mkdir .test_dir2");
  }

  int fd = open(".test_dir2/test_file1.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror("opening .test_dir2/test_file1.txt for write");
  }
  if (write(fd, "foo\nbar\nbaz\n", 12) != 12) {
    perror("writing .test_dir2/test_file1.txt");
  }
  close(fd);

  if (symlink("test_file1.txt", ".test_dir2/test_symlink1.txt") != 0) {
    perror("creating symlink .test_dir2/test_symlink1.txt to test_file1.txt");
  }
}


void
teardown_test_files (void)
{
  if (unlink(".test_dir2/test_symlink1.txt") != 0) {
    perror("unlinking .test_dir2/test_symlink1.txt");
  }
  if (unlink(".test_dir2/test_file1.txt") != 0) {
    perror("unlinking .test_dir2/test_file1.txt");
  }
  if (rmdir(".test_dir2") != 0) {
    perror("rmdir .test_dir2");
  }
  if (rmdir(".test_dir1") != 0) {
    perror("rmdir .test_dir1");
  }
  rmdir(".no_test_dir1");
  rmdir(".no_test_dir2");
}


Suite *
uft_tx_suite ()
{
  Suite * s = suite_create("transaction");

  TCase * tc_new_tx = tcase_create("new");

  tcase_add_test(tc_new_tx, test_new_tx_not_null);
  tcase_add_test(tc_new_tx, test_new_tx_stores_extra_data);

  suite_add_tcase(s, tc_new_tx);

  TCase * tc_extra_tx = tcase_create("extra");

  tcase_add_test(tc_extra_tx, test_extra_returns_extra_data);

  suite_add_tcase(s, tc_extra_tx);

  TCase * tc_begin_tx = tcase_create("begin");
  tcase_add_checked_fixture(tc_begin_tx, setup_new, teardown_new);

  tcase_add_test(tc_begin_tx, test_begin_tx_returns_tx);
  tcase_add_test(tc_begin_tx, test_begin_tx_calls_exec_fun);
  tcase_add_test(tc_begin_tx, test_begin_tx_rollback_when_no_update);
  tcase_add_test(tc_begin_tx, test_begin_tx_rollback_when_failed);
  tcase_add_test(tc_begin_tx, test_begin_tx_no_rollback_when_succeeded);
  tcase_add_test(tc_begin_tx, test_begin_tx_error_msg_recorded);
  tcase_add_test(tc_begin_tx, test_begin_tx_multiple_error_msgs_recorded);

  suite_add_tcase(s, tc_begin_tx);

  TCase * tc_tx_add_ent = tcase_create("add_ent");
  tcase_add_checked_fixture(tc_tx_add_ent, setup_new, teardown_new);
  tcase_add_checked_fixture(tc_tx_add_ent, setup_test_files, teardown_test_files);

  tcase_add_test(tc_tx_add_ent, test_add_ent_existing_dir_fails);
  tcase_add_test(tc_tx_add_ent, test_add_ent_nonexisting_fails_without_flag);
  tcase_add_test(tc_tx_add_ent, test_add_ent_nonexisting_succeeds);
  tcase_add_test(tc_tx_add_ent, test_add_ent_existing_file_succeeds);
  tcase_add_test(tc_tx_add_ent, test_add_ent_existing_file_adds_file);
  tcase_add_test(tc_tx_add_ent, test_add_ent_existing_file_records_file_data);
  tcase_add_test(tc_tx_add_ent, test_add_ent_existing_symlink_succeeds);
  tcase_add_test(tc_tx_add_ent, test_add_ent_existing_symlink_adds_symlink);
  tcase_add_test(tc_tx_add_ent, test_add_ent_existing_symlink_records_linkdest);

  suite_add_tcase(s, tc_tx_add_ent);

  TCase * tc_tx_failure = tcase_create("failure");
  tcase_add_checked_fixture(tc_tx_failure, setup_new, teardown_new);
  tcase_add_checked_fixture(tc_tx_failure, setup_test_files, teardown_test_files);

  tcase_add_test(tc_tx_failure, test_failure_rolls_back_changed_files);
  tcase_add_test(tc_tx_failure, test_failure_rolls_back_deleted_files);
  tcase_add_test(tc_tx_failure, test_failure_rolls_back_dir_replaced_files);
  tcase_add_test(tc_tx_failure, test_failure_rolls_back_symlink_replaced_files);
  tcase_add_test(tc_tx_failure, test_failure_rolls_back_replaced_symlinks);
  tcase_add_test(tc_tx_failure, test_failure_rolls_back_changed_symlinks);
  tcase_add_test(tc_tx_failure, test_failure_rolls_back_deleted_symlinks);
  tcase_add_test(tc_tx_failure, test_failure_rolls_back_noent);
  tcase_add_test(tc_tx_failure, test_failure_rolls_back_noent_with_mkdir);

  suite_add_tcase(s, tc_tx_failure);

  TCase * tc_tx_rollback = tcase_create("rollback");
  tcase_add_checked_fixture(tc_tx_rollback, setup_new, teardown_new);

  tcase_add_test(tc_tx_rollback, test_rollback_rolls_back_children);

  suite_add_tcase(s, tc_tx_rollback);

  TCase * tc_tx_error_msgs = tcase_create("error_msgs");
  tcase_add_checked_fixture(tc_tx_error_msgs, setup_new, teardown_new);

  tcase_add_test(tc_tx_error_msgs, test_error_msgs_returns_logged_errors);

  suite_add_tcase(s, tc_tx_error_msgs);

  return s;
}


int
main (int argc, char ** argv)
{
  int num_failed;
  Suite * s = uft_tx_suite();
  SRunner * sr = srunner_create(s);

  srunner_run_all(sr, UFT_CHECK_SRUNNER_FLAGS);
  num_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  exit(num_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
