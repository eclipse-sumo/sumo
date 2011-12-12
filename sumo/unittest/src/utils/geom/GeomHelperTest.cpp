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


TEST(GeomHelper, test_method_closestDistancePointLine_basic) {
    Position expected(1,0);
    SUMOReal expectedDistance = 1;
    Position point(1,1);
    Position start(0,0);
    Position end(2,0);
    Position closestPoint;
    SUMOReal result = GeomHelper::closestDistancePointLine(point, start, end, closestPoint);
	EXPECT_FLOAT_EQ(expected.x(), closestPoint.x());
	EXPECT_FLOAT_EQ(expected.y(), closestPoint.y());
	EXPECT_FLOAT_EQ(expected.z(), closestPoint.z());
	EXPECT_FLOAT_EQ(expectedDistance, result);
}
TEST(GeomHelper, test_method_closestDistancePointLine_onLine) {
    Position expected(1,0);
    SUMOReal expectedDistance = 0;
    Position point(1,0);
    Position start(0,0);
    Position end(2,0);
    Position closestPoint;
    SUMOReal result = GeomHelper::closestDistancePointLine(point, start, end, closestPoint);
	EXPECT_FLOAT_EQ(expected.x(), closestPoint.x());
	EXPECT_FLOAT_EQ(expected.y(), closestPoint.y());
	EXPECT_FLOAT_EQ(expected.z(), closestPoint.z());
	EXPECT_FLOAT_EQ(expectedDistance, result);
}
TEST(GeomHelper, test_method_closestDistancePointLine_outside_after) {
    Position expected(2,0);
    SUMOReal expectedDistance = 5;
    Position point(5,4);
    Position start(0,0);
    Position end(2,0);
    Position closestPoint;
    SUMOReal result = GeomHelper::closestDistancePointLine(point, start, end, closestPoint);
	EXPECT_FLOAT_EQ(expected.x(), closestPoint.x());
	EXPECT_FLOAT_EQ(expected.y(), closestPoint.y());
	EXPECT_FLOAT_EQ(expected.z(), closestPoint.z());
	EXPECT_FLOAT_EQ(expectedDistance, result);
}
TEST(GeomHelper, test_method_closestDistancePointLine_outside_before) {
    Position expected(0,0);
    SUMOReal expectedDistance = 5;
    Position point(-3,4);
    Position start(0,0);
    Position end(2,0);
    Position closestPoint;
    SUMOReal result = GeomHelper::closestDistancePointLine(point, start, end, closestPoint);
	EXPECT_FLOAT_EQ(expected.x(), closestPoint.x());
	EXPECT_FLOAT_EQ(expected.y(), closestPoint.y());
	EXPECT_FLOAT_EQ(expected.z(), closestPoint.z());
	EXPECT_FLOAT_EQ(expectedDistance, result);
}
