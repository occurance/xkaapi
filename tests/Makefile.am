##
##  xkaapi
##
##  Created by VD on 30/11/12.
##  Copyright 2012 INRIA.
##

DIST_SUBDIRS=api-kaapic api-kaapixx kaapi

# The `:;' works around a Bash 3.2 bug when the output is not writable.
$(srcdir)/package.m4: $(top_srcdir)/configure.ac
	:;{ \
	  echo '# Signature of the current package.' && \
	  echo 'm4_define([AT_PACKAGE_NAME],' && \
	  echo '  [$(PACKAGE_NAME)])' && \
	  echo 'm4_define([AT_PACKAGE_TARNAME],' && \
	  echo '  [$(PACKAGE_TARNAME)])' && \
	  echo 'm4_define([AT_PACKAGE_VERSION],' && \
	  echo '  [$(PACKAGE_VERSION)])' && \
	  echo 'm4_define([AT_PACKAGE_STRING],' && \
	  echo '  [$(PACKAGE_STRING)])' && \
	  echo 'm4_define([AT_PACKAGE_BUGREPORT],' && \
	  echo '  [$(PACKAGE_BUGREPORT)])'; \
	  echo 'm4_define([AT_PACKAGE_URL],' && \
	  echo '  [$(PACKAGE_URL)])'; \
	} >'$(srcdir)/package.m4'

TESTSUITES_SRC = testsuite.at testsuite-api-kaapic.at testsuite-api-kaapixx.at \
	testsuite-examples.at testsuite-examples-inc.at \
	atenv.m4 check-headers/check-headers.m4
EXTRA_DIST = $(TESTSUITES_SRC) $(srcdir)/package.m4 $(TESTSUITE) atlocal.in
EXTRA_DIST += check-headers/main.c
TESTSUITE = $(srcdir)/testsuite
DISTCLEANFILES = atconfig

check-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

installcheck-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' AUTOTEST_PATH='$(bindir):..' \
	$(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || \
	$(SHELL) '$(TESTSUITE)' --clean

AUTOM4TE = $(SHELL) $(top_srcdir)/build-aux/missing --run autom4te
AUTOTEST = $(AUTOM4TE) --language=autotest
$(TESTSUITE): $(TESTSUITES_SRC) $(srcdir)/package.m4 $(srcdir)/atenv.m4
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

testsuite-examples-inc.at: Makefile.am
	for e in $$( (cd $(top_srcdir)/examples ; grep -h  _PRG $$(ls *.am | grep -v Makefile)) \
		| grep -v examples_PRGS | grep -v '^#\|all-' \
		| sed -e 's/^[A-Z0-9_]*+\?=//'); do \
	    echo "KAT_EX_CHECK_PROG([$$e])" ; \
	done > $(srcdir)/testsuite-examples-inc.at