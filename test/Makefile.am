TESTS = check_uft_tx
check_PROGRAMS = $(TESTS)

clean-local:
	rm -f *.gcda *.gcno *.gcov

check_uft_tx_SOURCES = check_uft_tx.c \
	../src/uft_ll.c \
	../src/uft_status.c \
	../src/uft_tx.c
check_uft_tx_CFLAGS = @CHECK_CFLAGS@ -I../src --coverage
check_uft_tx_LDFLAGS =
check_uft_tx_LDADD = @CHECK_LIBS@
