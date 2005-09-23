noinst_LIBRARIES = libcommon.a

EXTRA_DIST = DoubleVector.h StdDefs.h SUMOTime.h

libcommon_a_SOURCES = AbstractMutex.h \
BoolVector.h DevHelper.cpp DevHelper.h DoubleVector.cpp DoubleVector.h \
FileErrorReporter.cpp FileErrorReporter.h FileHelpers.cpp FileHelpers.h \
HelpPrinter.h IntVector.cpp IntVector.h InstancePool.h \
Named.h StringTokenizer.cpp StringTokenizer.h \
StringUtils.cpp StringUtils.h UtilExceptions.cpp UtilExceptions.h \
MsgHandler.h MsgHandler.cpp MsgRetriever.h \
RandHelper.h RandHelper.cpp \
SystemFrame.cpp SystemFrame.h SysUtils.h SysUtils.cpp \
LogFile.cpp LogFile.h \
MsgRetrievingFunction.h \
ToString.h TplConvert.h TplConvertSec.h \
XMLHelpers.cpp XMLHelpers.h

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src \
-I$(XERCES_INCLUDES) -I$(XERCES_INCLUDES_PRIOR_1_7)

