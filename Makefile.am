SUBDIRS = src test

ACLOCAL_AMFLAGS = -I m4

coverage:
	mkdir -p coverage
	gcovr --html --html-details -o coverage/index.html -e test/check_

.PHONY: coverage
