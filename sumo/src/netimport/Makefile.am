noinst_LIBRARIES = libnetimport.a

libnetimport_a_SOURCES = NILoader.cpp NILoader.h \
NIOptionsIO.cpp NIOptionsIO.h

CFRONT_FILES =

EXTRA_DIST = $(CFRONT_FILES)

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src \
-I$(XERCES_INCLUDES) -I$(XERCES_INCLUDES_PRIOR_1_7)

SUBDIRS = artemis arcview cell elmar elmar2 sumo tiger vissim visum xml

