noinst_LIBRARIES = libglutils.a

libglutils_a_SOURCES = FontStorage.cpp FontStorage.h \
GLHelper.cpp GLHelper.h lfontrenderer.cpp lfontrenderer.h

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src $(all_includes)

libgui_a_METASOURCES = AUTO

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

# them while "make clean", use CLEANFILES
DISTCLEANFILES =
