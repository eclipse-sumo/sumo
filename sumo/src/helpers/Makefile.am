noinst_LIBRARIES = libhelpers.a

CFRONT_FILES = Command.h \
SimpleCommand.h \
SimpleCommand.cpp \
Counter.h \
Counter.cpp \
Counter.icc \
Dictionary.h \
SingletonDictionary.h \
OneArgumentCommand.h

libhelpers_a_SOURCES = PreStartInitialised.cpp \
PreStartInitialised.h

EXTRA_DIST = $(CFRONT_FILES)

AM_CPPFLAGS = -I$(top_srcdir)  -I$(top_srcdir)/src -I$(top_srcdir)/src/microsim



