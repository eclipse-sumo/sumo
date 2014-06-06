XERCES_LIBS = -l$(LIB_XERCES)

if CHECK_MEMORY_LEAKS
MEM_LIBS = ../../src/foreign/nvwa/libnvwa.a
endif

if INTERNAL
MESO_LIBS = ../../src/mesosim/libmesosim.a
INTERNAL_LIB = ../../src/internal/libinternal.a
INTERNAL_TESTS = ./internal/HeightMapperTest.o
endif


if WITH_GTEST
bin_PROGRAMS = sumo-unittest

sumo_unittest_SOURCES = unittest_main.cpp

sumo_unittest_LDFLAGS = $(GTEST_LDFLAGS) $(GDAL_LDFLAGS) $(PROJ_LDFLAGS)

sumo_unittest_LDADD   = \
./utils/common/StringUtilsTest.o \
./utils/common/StringTokenizerTest.o \
./utils/common/RGBColorTest.o \
./utils/common/TplConvertTest.o \
./utils/common/ValueTimeLineTest.o \
./utils/geom/BoundaryTest.o \
./utils/geom/PositionVectorTest.o \
./utils/geom/LineTest.o \
./utils/geom/GeomHelperTest.o \
./utils/geom/GeoConvHelperTest.o \
./netbuild/NBTrafficLightLogicTest.o \
$(INTERNAL_TESTS) \
../../src/netbuild/libnetbuild.a \
../../src/traci-server/libtraciserver.a \
../../src/microsim/libmicrosim.a \
../../src/traci-server/TraCIServer.o \
../../src/microsim/trigger/libmicrosimtrigger.a \
../../src/microsim/cfmodels/libmicrosimcfmodels.a \
../../src/microsim/devices/libmicrosimdevs.a \
../../src/microsim/output/libmicrosimoutput.a \
../../src/microsim/MSMoveReminder.o \
../../src/microsim/MSRouteHandler.o \
../../src/microsim/actions/libmsactions.a \
../../src/microsim/traffic_lights/libmicrosimtls.a \
$(MESO_LIBS) \
../../src/utils/geom/libgeom.a \
../../src/utils/shapes/libshapes.a \
../../src/utils/emissions/libemissions.a \
../../src/utils/xml/libxml.a \
../../src/utils/common/libcommon.a \
../../src/utils/options/liboptions.a \
../../src/utils/iodevices/libiodevices.a \
../../src/utils/common/FileHelpers.o \
../../src/foreign/tcpip/libtcpip.a \
$(INTERNAL_LIB) \
$(MEM_LIBS) \
$(PYTHON_LIBS) \
$(XERCES_LIBS) \
$(LIB_GTEST)

SUBDIRS = utils microsim netbuild $(UNITTEST_INTERNAL_DIRS)
endif
