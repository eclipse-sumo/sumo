noinst_LIBRARIES = libcommon.a

EXTRA_DIST = NamedObjectCont.cpp DoubleVector.h

libcommon_a_SOURCES = BoolVector.h DoubleVector.cpp DoubleVector.h \
FileErrorReporter.cpp FileErrorReporter.h FileHelpers.cpp FileHelpers.h \
HelpPrinter.h IntVector.cpp IntVector.h Named.h \
NamedObjectCont.h SErrorHandler.cpp SErrorHandler.h \
SLogging.cpp SLogging.h StringTokenizer.cpp StringTokenizer.h \
StringUtils.cpp StringUtils.h UtilExceptions.cpp UtilExceptions.h

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src \
-I$(XERCESINCLUDE) -I$(XERCESINCLUDE_PRIOR_1_7)

