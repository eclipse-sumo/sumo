SUBDIRS = bin prj data docs src tests 

AUTOMAKE_OPTIONS = gnu dist-zip #1.6

EXTRA_DIST = NEWS README ChangeLog AUTHORS COPYING INSTALL TODO 

#$(top_srcdir)/acinclude.m4: $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in
#	@cd $(top_srcdir) && cat admin/acinclude.m4.in admin/libtool.m4.in > acinclude.m4

#$(top_srcdir)/configure.in: $(top_srcdir)/configure.files $(shell test -f $(top_srcdir)/configure.files && sed -e "s%^%\$(top_srcdir)/%" $(top_srcdir)/configure.files)
#	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.in

#$(top_srcdir)/subdirs:
#	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common subdirs

#$(top_srcdir)/configure.files:
#	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.files



#MAINTAINERCLEANFILES = subdirs configure.in acinclude.m4 configure.files 

#package-messages:
#	$(MAKE) -f admin/Makefile.common package-messages
#	$(MAKE) -C po merge

#dist-hook:
#	cd $(top_distdir) && perl admin/am_edit -padmin
#	cd $(top_distdir) && $(MAKE) -f admin/Makefile.common subdirs
