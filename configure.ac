AC_INIT([libuft], 1.0.0)
AM_INIT_AUTOMAKE([foreign subdir-objects])
LT_INIT

AC_CONFIG_MACRO_DIRS([m4])

PKG_CHECK_MODULES([CHECK], [check], have_check=yes)

AC_PROG_CC
AC_PROG_CC_STDC

AC_DEFINE([UFT_MAX_MSG_LEN], [1024], [Maximum message length (applies to string buffers)])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(Makefile src/Makefile test/Makefile)

AC_OUTPUT
