noinst_LIBRARIES = libimportio.a

EXTRA_DIST = LineHandler.h

libimportio_a_SOURCES = LineReader.cpp \
NamedColumnsParser.cpp \
StringUtils.cpp \
LineReader.h \
NamedColumnsParser.h \
StringUtils.h

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src \
-I$(XERCESINCLUDE) -I$(XERCESINCLUDE_PRIOR_1_7)
