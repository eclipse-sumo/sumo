#include <gtest/gtest.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeomHelper.h>

using namespace std;

/*
Tests the class GeomHelper
*/

TEST(GeomHelper, test_method_intersects) {
    // overlapping line segments
	EXPECT_FALSE(GeomHelper::intersects(
                Position(0,0), Position(2,0),
                Position(1,0), Position(3,0)));

    // parallel line segments
	EXPECT_FALSE(GeomHelper::intersects(
                Position(0,0), Position(1,0),
                Position(0,1), Position(1,1)));

    // intersection outside of segments
	EXPECT_FALSE(GeomHelper::intersects(
                Position(0,0), Position(2,0),
                Position(1,3), Position(1,1)));

    // intersection at (1,0)
	EXPECT_TRUE(GeomHelper::intersects(
                Position(0,0), Position(2,0),
                Position(1,3), Position(1,-1)));
}


TEST(GeomHelper, test_method_intersection_position) {
    Position expected(1,0);
    Position pos = GeomHelper::intersection_position(
            Position(0,0), Position(2,0),
            Position(1,3), Position(1,-1));
	EXPECT_FLOAT_EQ(expected.x(), pos.x());
	EXPECT_FLOAT_EQ(expected.y(), pos.y());
}

