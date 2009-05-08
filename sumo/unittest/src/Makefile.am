if WITH_GTEST
bin_PROGRAMS = sumo-unittest

sumo_unittest_SOURCES = unittest_main.cpp

sumo_unittest_LDFLAGS = $(GTEST_LDFLAGS)

sumo_unittest_LDADD   = ../../src/utils/common/libcommon.a \
./utils/common/libtestcommon.a \
-lgtest

SUBDIRS = utils
endif
