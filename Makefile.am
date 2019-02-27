SUBDIRS = src test

ACLOCAL_AMFLAGS = -I m4

cov_report:
	mkdir -p coverage
	gcovr -s --html --html-details -o coverage/index.html -e test/check_
