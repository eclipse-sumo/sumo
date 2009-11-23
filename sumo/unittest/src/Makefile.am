if CHECK_MEMORY_LEAKS
MEM_LIBS = ../../src/foreign/nvwa/libnvwa.a
endif

if WITH_GTEST
bin_PROGRAMS = sumo-unittest

sumo_unittest_SOURCES = unittest_main.cpp

sumo_unittest_LDFLAGS = $(GTEST_LDFLAGS) 

sumo_unittest_LDADD   = ../../src/netbuild/libnetbuild.a \
../../src/microsim/libmicrosim.a \
../../src/microsim/output/libmicrosimoutput.a \
../../src/microsim/trigger/libmicrosimtrigger.a \
../../src/microsim/actions/libmsactions.a \
../../src/microsim/traffic_lights/libmicrosimtls.a \
../../src/utils/geom/libgeom.a \
../../src/utils/options/liboptions.a \
../../src/utils/common/libcommon.a \
../../src/utils/xml/libxml.a \
../../src/utils/iodevices/libiodevices.a \
../../src/foreign/tcpip/libtcpip.a \
../../src/traci-server/libtraciserver.a \
./utils/common/StringUtilsTest.o \
./utils/common/StringTokenizerTest.o \
./utils/common/RGBColorTest.o \
./utils/common/ValueTimeLineTest.o \
./utils/common/CommandMock.o \
./utils/geom/BoundaryTest.o \
./utils/geom/Position2DVectorTest.o \
./utils/geom/Line2DTest.o \
./utils/iodevices/OutputDeviceMock.o \
./microsim/MSEventControlTest.o \
./netbuild/NBTrafficLightLogicTest.o \
$(MEM_LIBS) \
-lgtest

SUBDIRS = utils microsim netbuild
endif
