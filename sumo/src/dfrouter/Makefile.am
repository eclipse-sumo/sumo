if CHECK_MEMORY_LEAKS
MEM_LIBS  = ../foreign/nvwa/libnvwa.a
endif

bin_PROGRAMS = dfrouter

dfrouter_SOURCES = dfrouter_main.cpp RODFDetFlowLoader.cpp RODFDetFlowLoader.h \
RODFDetector.cpp RODFDetector.h RODFDetectorFlow.cpp RODFDetectorFlow.h \
RODFDetectorHandler.cpp RODFDetectorHandler.h \
RODFNet.cpp RODFNet.h RODFRouteCont.cpp RODFRouteCont.h \
RODFRouteDesc.h RODFEdgeBuilder.cpp RODFEdgeBuilder.h \
RODFEdge.cpp RODFEdge.h \
RODFFrame.cpp RODFFrame.h

dfrouter_LDFLAGS = $(XERCES_LDFLAGS)

dfrouter_LDADD   = ../router/librouter.a \
../utils/options/liboptions.a \
../utils/xml/libxml.a \
../utils/vehicle/libvehicle.a \
../utils/common/libcommon.a \
../utils/importio/libimportio.a \
../utils/iodevices/libiodevices.a \
../utils/geom/libgeom.a \
../utils/emissions/libemissions.a \
../foreign/tcpip/libtcpip.a \
$(MEM_LIBS) \
-l$(LIB_XERCES) $(FOX_LDFLAGS)
