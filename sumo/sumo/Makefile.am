bin_PROGRAMS = sumo
sumo_SOURCES = main.cpp help.h

sumo_LDFLAGS = -L$(XERCESLIBPATH) 
sumo_LDADD   = ./netload/libnetload.a ./microsim/libmicrosim.a ./utils/libutils.a -l$(XERCESLIB)

AM_CPPFLAGS = -I$(top_srcdir)/sumo/microsim -I$(top_srcdir)/sumo/netload \
-I$(top_srcdir)/sumo/utils -I$(XERCESINCLUDE) -I$(XERCESINCLUDE_PRIOR_1_7)

SUBDIRS = microsim netload utils data helpers



