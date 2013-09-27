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

bin_PROGRAMS = polyconvert

polyconvert_SOURCES = polyconvert_main.cpp PCLoaderArcView.cpp PCLoaderArcView.h \
PCLoaderDlrNavteq.cpp PCLoaderDlrNavteq.h \
PCLoaderVisum.cpp PCLoaderVisum.h \
PCLoaderOSM.cpp PCLoaderOSM.h \
PCLoaderXML.cpp PCLoaderXML.h \
PCNetProjectionLoader.cpp PCNetProjectionLoader.h \
PCPolyContainer.cpp PCPolyContainer.h \
PCTypeDefHandler.cpp PCTypeDefHandler.h \
PCTypeMap.cpp PCTypeMap.h

polyconvert_LDADD   = ../utils/options/liboptions.a \
../utils/geom/libgeom.a \
../utils/shapes/libshapes.a \
$(COMMON_LIBS) \
$(XERCES_LDFLAGS) $(GDAL_LDFLAGS) $(PROJ_LDFLAGS)
