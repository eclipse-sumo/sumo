noinst_LIBRARIES = libqutils.a

libqutils_a_SOURCES = NewQMutex.cpp \
NewQMutex.h

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src $(all_includes)

libqutils_a_METASOURCES = AUTO

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(libqutils_a_METASOURCES)
