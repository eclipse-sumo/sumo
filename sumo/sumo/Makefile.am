bin_PROGRAMS = sumo
sumo_SOURCES = main.cpp help.h

if CFRONT
CFRONT_LIB = ./helpers/libhelpers.a
endif CFRONT

sumo_LDFLAGS = -L$(XERCESLIBPATH) 
sumo_LDADD   = $(CFRONT_LIB) ./netload/libnetload.a ./microsim/libmicrosim.a ./utils/libutils.a -l$(XERCESLIB)

AM_CPPFLAGS = -Imicrosim -Inetload -Iutils -I$(XERCESINCLUDE)

SUBDIRS = microsim netload utils data helpers



