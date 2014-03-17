/****************************************************************************/
/// @file    GeomHelperTest.cpp
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    2011-12-12
/// @version $Id$
///
// Tests the class GeomHelper
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#include <gtest/gtest.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeomHelper.h>

using namespace std;


TEST(GeomHelper, test_method_intersects) {
    // on the same line but non-overlapping segments
	EXPECT_FALSE(GeomHelper::intersects(
                Position(0,0), Position(1,0),
                Position(2,0), Position(3,0)));

    // overlapping line segments
	EXPECT_TRUE(GeomHelper::intersects(
                Position(0,0), Position(2,0),
                Position(1,0), Position(3,0)));

    // identical line segments
	EXPECT_TRUE(GeomHelper::intersects(
                Position(1,0), Position(3,0),
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


TEST(GeomHelper, test_method_intersection_position2D) {
    Position expected(1,0);
    Position pos = GeomHelper::intersection_position2D(
            Position(0,0), Position(2,0),
            Position(1,3), Position(1,-1));
	EXPECT_FLOAT_EQ(expected.x(), pos.x());
	EXPECT_FLOAT_EQ(expected.y(), pos.y());
    // overlapping line segments
    Position expected2(1.5,0);
    Position pos2 = GeomHelper::intersection_position2D(
            Position(0,0), Position(2,0),
            Position(1,0), Position(3,0));
	EXPECT_FLOAT_EQ(expected2.x(), pos2.x());
	EXPECT_FLOAT_EQ(expected2.y(), pos2.y());
    // identical line segments
    Position expected3(1,0);
    Position pos3 = GeomHelper::intersection_position2D(
            Position(0,0), Position(2,0),
            Position(0,0), Position(2,0));
	EXPECT_FLOAT_EQ(expected3.x(), pos3.x());
	EXPECT_FLOAT_EQ(expected3.y(), pos3.y());
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
