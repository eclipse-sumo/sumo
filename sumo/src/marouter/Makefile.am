if CHECK_MEMORY_LEAKS
MEM_LIBS  = ../foreign/nvwa/libnvwa.a
endif

bin_PROGRAMS = marouter

marouter_SOURCES = marouter_main.cpp \
ROMAAssignments.cpp ROMAAssignments.h \
ROMAEdge.cpp ROMAEdge.h \
ROMAEdgeBuilder.cpp ROMAEdgeBuilder.h \
ROMAFrame.cpp ROMAFrame.h \
ROMARouteHandler.cpp ROMARouteHandler.h

marouter_LDFLAGS = $(XERCES_LDFLAGS)

marouter_LDADD   = ../router/librouter.a \
../od/libod.a \
../utils/distribution/libdistribution.a \
../utils/options/liboptions.a \
../utils/xml/libxml.a \
../utils/vehicle/libvehicle.a \
../utils/common/libcommon.a \
../utils/importio/libimportio.a \
../utils/iodevices/libiodevices.a \
../utils/geom/libgeom.a \
../utils/emissions/libemissions.a \
../foreign/PHEMlight/cpp/libphemlight.a \
../foreign/tcpip/libtcpip.a \
$(MEM_LIBS) \
-l$(LIB_XERCES) $(FOX_LDFLAGS)
