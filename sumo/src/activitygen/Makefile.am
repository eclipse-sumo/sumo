SUBDIRS = activities city

XERCES_LIBS = -l$(LIB_XERCES)

COMMON_LIBS = ../utils/options/liboptions.a \
../utils/xml/libxml.a \
../utils/distribution/libdistribution.a \
../utils/vehicle/libvehicle.a \
../utils/common/libcommon.a \
../utils/importio/libimportio.a \
../utils/iodevices/libiodevices.a \
../utils/geom/libgeom.a \
../utils/emissions/libemissions.a \
../foreign/PHEMlight/cpp/libphemlight.a \
../foreign/tcpip/libtcpip.a \
$(XERCES_LIBS)

bin_PROGRAMS = activitygen

activitygen_SOURCES = activitygen_main.cpp \
AGActivityGen.cpp AGActivityGen.h \
AGActivityGenHandler.cpp AGActivityGenHandler.h \
AGActivityTripWriter.cpp AGActivityTripWriter.h \
AGFrame.cpp AGFrame.h

activitygen_LDFLAGS = $(XERCES_LDFLAGS)
activitygen_LDADD = ./activities/libagactivities.a \
./city/libagcity.a \
../duarouter/RODUAEdgeBuilder.o \
../duarouter/RODUAFrame.o \
../router/librouter.a \
$(COMMON_LIBS) $(FOX_LDFLAGS)
