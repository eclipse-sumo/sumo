noinst_LIBRARIES = libguipopup.a


EXTRA_DIST =

libguipopup_a_SOURCES = QGLObjectPopupMenu.cpp QGLObjectPopupMenu.h \
QGLObjectPopupMenuItem.cpp QGLObjectPopupMenuItem.h \
QGLObjectPopupMenu.moc

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src $(all_includes) \
-I$(XERCESINCLUDE) -I$(XERCESINCLUDE_PRIOR_1_7)

SUBDIRS =

libguipopup_a_METASOURCES = QGLObjectPopupMenu.moc

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(libgui_a_METASOURCES)




