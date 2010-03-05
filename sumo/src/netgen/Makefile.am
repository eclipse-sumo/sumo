XERCES_LIBS = -l$(LIB_XERCES)

if WITH_PROJ
PROJ_LIBS = -l$(LIB_PROJ)
endif

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

bin_PROGRAMS = netgen

netgen_SOURCES = netgen_main.cpp NGEdge.cpp NGEdge.h NGNet.h NGNet.cpp NGNode.cpp NGNode.h \
NGRandomNetBuilder.h NGRandomNetBuilder.cpp

netgen_LDFLAGS = $(XERCES_LDFLAGS) $(PROJ_LDFLAGS)

netgen_LDADD   = ../netbuild/libnetbuild.a \
../utils/geom/libgeom.a \
$(COMMON_LIBS) \
$(PROJ_LIBS)
