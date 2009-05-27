if WITH_GTEST
bin_PROGRAMS = sumo-unittest

sumo_unittest_SOURCES = unittest_main.cpp

sumo_unittest_LDFLAGS = $(GTEST_LDFLAGS) 

sumo_unittest_LDADD   = ../../src/utils/common/libcommon.a \
../../src/utils/geom/libgeom.a \
./utils/common/StringUtilsTest.o \
./utils/common/StringTokenizerTest.o \
./utils/common/RGBColorTest.o \
./utils/geom/BoundaryTest.o \
-lgtest

SUBDIRS = utils
endif
