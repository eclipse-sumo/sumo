noinst_LIBRARIES = libcommon.a

EXTRA_DIST = NamedObjectCont.cpp

libcommon_a_SOURCES = FileErrorReporter.cpp FileErrorReporter.h \
FileHelpers.cpp FileHelpers.h HelpPrinter.h \
MemDiff.cpp MemDiff.h Named.h NamedObjectCont.h \
SErrorHandler.cpp SErrorHandler.h SLogging.cpp SLogging.h \
StringTokenizer.cpp StringTokenizer.h \
UtilExceptions.cpp UtilExceptions.h

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src \
-I$(XERCESINCLUDE) -I$(XERCESINCLUDE_PRIOR_1_7)


