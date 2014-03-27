XERCES_LIBS = -l$(LIB_XERCES)

if CHECK_MEMORY_LEAKS
MEM_LIBS  = ../foreign/nvwa/libnvwa.a
endif

COMMON_LIBS = ../utils/options/liboptions.a \
../utils/xml/libxml.a \
../utils/common/libcommon.a \
../utils/importio/libimportio.a \
../utils/iodevices/libiodevices.a \
../foreign/tcpip/libtcpip.a \
$(MEM_LIBS) \
$(XERCES_LIBS)

bin_PROGRAMS = od2trips

od2trips_SOURCES = od2trips_main.cpp \
ODDistrict.cpp ODDistrict.h \
ODDistrictCont.cpp ODDistrictCont.h \
ODAmitranHandler.cpp ODAmitranHandler.h \
ODDistrictHandler.cpp ODDistrictHandler.h \
ODCell.h ODMatrix.h ODMatrix.cpp

od2trips_LDFLAGS = $(XERCES_LDFLAGS)

od2trips_LDADD   = ../utils/options/liboptions.a \
../utils/distribution/libdistribution.a \
../utils/common/libcommon.a \
../utils/geom/libgeom.a \
$(COMMON_LIBS)
