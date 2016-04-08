if CHECK_MEMORY_LEAKS
MEM_LIBS  = ../foreign/nvwa/libnvwa.a
endif

bin_PROGRAMS = duarouter

duarouter_SOURCES = duarouter_main.cpp \
RODUAEdgeBuilder.cpp RODUAEdgeBuilder.h \
RODUAFrame.cpp RODUAFrame.h

duarouter_LDFLAGS = $(XERCES_LDFLAGS)

duarouter_LDADD   = ../router/librouter.a \
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
