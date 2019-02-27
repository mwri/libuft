all: uft_hello uft_shortcuts uft_nested

uft_nested: Makefile uft_nested.c
	gcc uft_nested.c -Wall -g -O0 -I../src -L../src/.libs -Wl,-rpath ../src/.libs -o uft_nested -luft

uft_hello: Makefile uft_hello.c
	gcc uft_hello.c -Wall -g -O0 -I../src -L../src/.libs -Wl,-rpath ../src/.libs -o uft_hello -luft

uft_shortcuts: Makefile uft_shortcuts.c
	gcc uft_shortcuts.c -Wall -g -O0 -I../src -L../src/.libs -Wl,-rpath ../src/.libs -o uft_shortcuts -luft

clean:
	rm -f uft_nested uft_hello uft_shortcuts
