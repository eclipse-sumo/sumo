bin_PROGRAMS = netgenerate

netgenerate_SOURCES = netgen_main.cpp NGEdge.cpp NGEdge.h NGFrame.cpp NGFrame.h \
NGNet.h NGNet.cpp NGNode.cpp NGNode.h \
NGRandomNetBuilder.h NGRandomNetBuilder.cpp

netgenerate_LDADD   = ../netbuild/libnetbuild.a \
../netwrite/libnetwrite.a \
../utils/geom/libgeom.a \
../utils/options/liboptions.a \
../utils/xml/libxml.a \
../utils/common/libcommon.a \
../utils/importio/libimportio.a \
../utils/iodevices/libiodevices.a \
../foreign/tcpip/libtcpip.a \
-l$(LIB_XERCES) \
$(XERCES_LDFLAGS) $(GDAL_LDFLAGS) $(PROJ_LDFLAGS)
