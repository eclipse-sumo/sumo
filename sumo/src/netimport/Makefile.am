noinst_LIBRARIES = libnetimport.a

libnetimport_a_SOURCES = NILoader.cpp NILoader.h \
NIOptionsIO.cpp NIOptionsIO.h

CFRONT_FILES =

EXTRA_DIST = $(CFRONT_FILES)

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src \
-I$(XERCESINCLUDE) -I$(XERCESINCLUDE)/xercesc

SUBDIRS = artemis \
arcview \
cell \
sumo \
vissim \
visum \
xml

