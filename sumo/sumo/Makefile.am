bin_PROGRAMS = sumo
sumo_SOURCES = main.cpp 

EXTRA_DIST = help.h

sumo_LDFLAGS = -L$(XERCESLIBPATH) 
sumo_LDADD   = ./netload/libnetload.a ./microsim/libmicrosim.a ./utils/libutils.a -l$(XERCESLIB)

AM_CPPFLAGS = -Imicrosim -Inetload -Iutils -I$(XERCESINCLUDE)

SUBDIRS = microsim netload utils data helpers



