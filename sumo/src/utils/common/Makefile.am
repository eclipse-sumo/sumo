noinst_LIBRARIES = libcommon.a

EXTRA_DIST = NamedObjectCont.cpp DoubleVector.h StdDefs.h SUMOTime.h

libcommon_a_SOURCES = BoolVector.h DoubleVector.cpp DoubleVector.h \
FileErrorReporter.cpp FileErrorReporter.h FileHelpers.cpp FileHelpers.h \
HelpPrinter.h IntVector.cpp IntVector.h InstancePool.h \
Named.h NamedObjectCont.h StringTokenizer.cpp StringTokenizer.h \
StringUtils.cpp StringUtils.h UtilExceptions.cpp UtilExceptions.h \
MsgHandler.h MsgHandler.cpp MsgRetriever.h \
RandHelper.h RandHelper.cpp \
SystemFrame.cpp SystemFrame.h SysUtils.h SysUtils.cpp \
LogFile.cpp LogFile.h \
MsgRetrievingFunction.h \
XMLHelpers.cpp XMLHelpers.h \
NamedObjectContSingleton.h

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src \
-I$(XERCES_INCLUDES) -I$(XERCES_INCLUDES_PRIOR_1_7)

