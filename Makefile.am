SUBDIRS = src test

ACLOCAL_AMFLAGS = -I m4

cov_report:
	mkdir -p coverage
	gcovr --html --html-details -o coverage/index.html -e test/check_
	lcov --base-directory src --directory . -c -o coverage/lcov.info
	lcov --remove coverage/lcov.info "*/check_*.c" -o coverage/lcov.info
