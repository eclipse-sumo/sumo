SUBDIRS = activities city

XERCES_LIBS = -l$(LIB_XERCES)

if CHECK_MEMORY_LEAKS
MEM_LIBS  = ../foreign/nvwa/libnvwa.a
endif

COMMON_LIBS = ../utils/options/liboptions.a \
../utils/common/libcommon.a \
../utils/xml/libxml.a \
../utils/importio/libimportio.a \
../utils/iodevices/libiodevices.a \
../foreign/tcpip/libtcpip.a \
$(MEM_LIBS) \
$(XERCES_LIBS)

bin_PROGRAMS = activitygen

activitygen_SOURCES = AGMain.cpp \
AGActivityGen.cpp AGActivityGen.h \
AGActivityGenHandler.cpp AGActivityGenHandler.h \
AGActivityTripWriter.cpp AGActivityTripWriter.h

activitygen_LDFLAGS = $(XERCES_LDFLAGS)
activitygen_LDADD = ./activities/libagactivities.a \
./city/libagcity.a \
../duarouter/RODUAEdgeBuilder.o \
../duarouter/RODUAFrame.o \
../router/librouter.a \
$(COMMON_LIBS)
