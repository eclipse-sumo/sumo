CFRONT_FILES = Command.h SimpleCommand.h SimpleCommand.cpp SimpleCommand.icc Counter.h Counter.cpp Counter.icc 

if CFRONT
noinst_LIBRARIES = libhelpers.a
libhelpers_a_SOURCES = $(CFRONT_FILES)
AM_CPPFLAGS = -I../microsim
else !CFRONT
EXTRA_DIST = $(CFRONT_FILES)
endif !CFRONT




