if CHECK_MEMORY_LEAKS
MEM_LIBS  = ../foreign/nvwa/libnvwa.a
endif

bin_PROGRAMS = emissionsDrivingCycle emissionsMap

emissionsDrivingCycle_SOURCES = emissionsDrivingCycle_main.cpp \
TrajectoriesHandler.cpp TrajectoriesHandler.h

emissionsDrivingCycle_LDFLAGS = $(XERCES_LDFLAGS)

emissionsDrivingCycle_LDADD   = ../utils/options/liboptions.a \
../utils/xml/libxml.a \
../utils/geom/libgeom.a \
../utils/emissions/libemissions.a \
../utils/common/libcommon.a \
../utils/importio/libimportio.a \
../utils/iodevices/libiodevices.a \
../foreign/PHEMlight/cpp/libphemlight.a \
../foreign/tcpip/libtcpip.a \
$(MEM_LIBS) \
-l$(LIB_XERCES)


emissionsMap_SOURCES = emissionsMap_main.cpp 

emissionsMap_LDFLAGS = $(XERCES_LDFLAGS)

emissionsMap_LDADD   = $(emissionsDrivingCycle_LDADD)
