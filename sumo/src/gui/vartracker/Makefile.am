noinst_LIBRARIES = libguivartracker.a


EXTRA_DIST =

libguivartracker_a_SOURCES = GUIParameterTracker.cpp GUIParameterTracker.h \
TrackerValueDesc.cpp TrackerValueDesc.h GUIParameterTracker.moc

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src $(all_includes) \
-I$(XERCESINCLUDE) -I$(XERCESINCLUDE_PRIOR_1_7)

SUBDIRS =

libguivartracker_a_METASOURCES = GUIParameterTracker.moc

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(libguivartracker_a_METASOURCES)




