noinst_LIBRARIES = libhelpers.a

CFRONT_FILES = Command.h \
SimpleCommand.h SimpleCommand.cpp DiscreteCommand.h \
Counter.h Counter.cpp \
Counter.icc \
Dictionary.h \
SingletonDictionary.h \
OneArgumentCommand.h \
gcc_NullType.h gcc_Typelist.h gcc_TypeManip.h gcc_TypeTraits.h \
msvc6_NullType.h msvc6_static_check.h msvc6_Typelist.h msvc6_TypeManip.h \
msvc6_TypeTraits.h

libhelpers_a_SOURCES = PreStartInitialised.cpp \
PreStartInitialised.h

EXTRA_DIST = $(CFRONT_FILES) ValueRetriever.h ValueSource.h

AM_CPPFLAGS = -I$(top_srcdir)  -I$(top_srcdir)/src -I$(top_srcdir)/src/microsim



