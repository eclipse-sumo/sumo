/****************************************************************************/
/// @file    PositionVectorTest.cpp
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2009-10-24
/// @version $Id$
///
// Tests the class PositionVector
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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
#include <utils/geom/PositionVector.h>
#include <utils/geom/Boundary.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>


class PositionVectorTest : public testing::Test {
	protected :
		PositionVector *vectorPolygon;
		PositionVector *vectorLine;

		virtual void SetUp(){
			vectorPolygon = new PositionVector();
			vectorPolygon->push_back(Position(0,0));
			vectorPolygon->push_back(Position(0,2));
			vectorPolygon->push_back(Position(2,4));
			vectorPolygon->push_back(Position(4,2));
			vectorPolygon->push_back(Position(4,0));	

			vectorLine = new PositionVector();
			vectorLine->push_back(Position(0,0));
			vectorLine->push_back(Position(2,2));
		}

		virtual void TearDown(){
			delete vectorPolygon;
			delete vectorLine;
		}
};

/* Test the method 'around'*/
TEST_F(PositionVectorTest, test_method_around) {

	EXPECT_TRUE(vectorPolygon->around(Position(1,1)));
	EXPECT_TRUE(vectorPolygon->around(Position(1,2)));
	EXPECT_FALSE(vectorPolygon->around(Position(4,4)));
	EXPECT_FALSE(vectorPolygon->around(Position(0,0)));

	EXPECT_FALSE(vectorLine->around(Position(1,1)));
	EXPECT_FALSE(vectorLine->around(Position(0,2)));

    // with positive offset
	EXPECT_TRUE(vectorPolygon->around(Position(4,2), 1));
	EXPECT_FALSE(vectorPolygon->around(Position(5,2), 1));
    // what was true remains true
	EXPECT_TRUE(vectorPolygon->around(Position(1,1), POSITION_EPS));
	EXPECT_TRUE(vectorPolygon->around(Position(1,2), POSITION_EPS));

    // with negative offset
	EXPECT_FALSE(vectorPolygon->around(Position(4,2), -POSITION_EPS));
	EXPECT_TRUE(vectorPolygon->around(Position(1,1), -1));
	EXPECT_FALSE(vectorPolygon->around(Position(0.5,0.5), -1));
}

/* Test the method 'area'*/
TEST_F(PositionVectorTest, test_method_area) {
    PositionVector square;
    square.push_back(Position(0,0));
    square.push_back(Position(1,0));
    square.push_back(Position(1,1));
    square.push_back(Position(0,1)); // open
    EXPECT_DOUBLE_EQ(square.area(), 1); 
    square.push_back(Position(0,0)); // closed
    EXPECT_DOUBLE_EQ(square.area(), 1); 
}

/* Test the method 'scaleRelative'.*/
TEST_F(PositionVectorTest, test_method_scaleRelative) {
    PositionVector square;
    square.push_back(Position(0,0));
    square.push_back(Position(1,0));
    square.push_back(Position(1,1));
    square.push_back(Position(0,1));
    square.push_back(Position(0,0));
    EXPECT_DOUBLE_EQ(square.area(), 1);
    square.scaleRelative(3);
    EXPECT_DOUBLE_EQ(square.area(), 9);

    PositionVector expected;
    expected.push_back(Position(-1,-1));
    expected.push_back(Position(2,-1));
    expected.push_back(Position(2,2));
    expected.push_back(Position(-1,2));
    expected.push_back(Position(-1,-1));

    EXPECT_EQ(expected.getCentroid(), square.getCentroid());
    for (size_t i = 0; i < square.size(); i++) {
        EXPECT_DOUBLE_EQ(expected[i].x(), square[i].x());
        EXPECT_DOUBLE_EQ(expected[i].y(), square[i].y());
    }
}

/* Test the method 'getCentroid'.*/
TEST_F(PositionVectorTest, test_method_getCentroid) {
    PositionVector square;
    square.push_back(Position(0,0));
    square.push_back(Position(1,0));
    square.push_back(Position(1,1));
    square.push_back(Position(0,1));
    EXPECT_EQ(Position(0.5, 0.5), square.getCentroid());

	Position pos2 = vectorLine->getCentroid();
	EXPECT_DOUBLE_EQ(1, pos2.x());
	EXPECT_DOUBLE_EQ(1, pos2.y());
}

/* Test the method 'getPolygonCenter'.*/
TEST_F(PositionVectorTest, test_method_getPolygonCenter) {
       Position pos = vectorPolygon->getPolygonCenter();
       EXPECT_DOUBLE_EQ(2, pos.x());
       EXPECT_DOUBLE_EQ(1.6, pos.y());
       Position pos2 = vectorLine->getPolygonCenter();
}


/* Test the method 'getBoxBoundary'*/
TEST_F(PositionVectorTest, test_method_getBoxBoundary) {	
	Boundary bound = vectorPolygon->getBoxBoundary();
	EXPECT_DOUBLE_EQ(bound.xmax(), 4);
	EXPECT_DOUBLE_EQ(bound.xmin(), 0);
	EXPECT_DOUBLE_EQ(bound.ymax(), 4);
	EXPECT_DOUBLE_EQ(bound.ymin(), 0);
}

/* Test the method 'splitAt'*/
TEST_F(PositionVectorTest, test_method_splitAt) {	
    PositionVector vec;
    vec.push_back(Position(0,0));
    vec.push_back(Position(2,0));
    vec.push_back(Position(5,0));
    SUMOReal smallDiff = POSITION_EPS / 2;
    std::pair<PositionVector, PositionVector> result;
    // split in first segment
    result = vec.splitAt(1);
	EXPECT_DOUBLE_EQ(2, result.first.size());
	EXPECT_DOUBLE_EQ(0, result.first[0].x());
	EXPECT_DOUBLE_EQ(1, result.first[1].x());
	EXPECT_DOUBLE_EQ(3, result.second.size());
	EXPECT_DOUBLE_EQ(1, result.second[0].x());
	EXPECT_DOUBLE_EQ(2, result.second[1].x());
	EXPECT_DOUBLE_EQ(5, result.second[2].x());
    // split in second segment
    result = vec.splitAt(4);
	EXPECT_DOUBLE_EQ(3, result.first.size());
	EXPECT_DOUBLE_EQ(0, result.first[0].x());
	EXPECT_DOUBLE_EQ(2, result.first[1].x());
	EXPECT_DOUBLE_EQ(4, result.first[2].x());
	EXPECT_DOUBLE_EQ(2, result.second.size());
	EXPECT_DOUBLE_EQ(4, result.second[0].x());
	EXPECT_DOUBLE_EQ(5, result.second[1].x());
    // split close before inner point
    result = vec.splitAt(2 - smallDiff);
	EXPECT_DOUBLE_EQ(2, result.first.size());
	EXPECT_DOUBLE_EQ(0, result.first[0].x());
	EXPECT_DOUBLE_EQ(2, result.first[1].x());
	EXPECT_DOUBLE_EQ(2, result.second.size());
	EXPECT_DOUBLE_EQ(2, result.second[0].x());
	EXPECT_DOUBLE_EQ(5 ,result.second[1].x());
    // split close after inner point
    result = vec.splitAt(2 + smallDiff);
	EXPECT_DOUBLE_EQ(2, result.first.size());
	EXPECT_DOUBLE_EQ(0, result.first[0].x());
	EXPECT_DOUBLE_EQ(2, result.first[1].x());
	EXPECT_DOUBLE_EQ(2, result.second.size());
	EXPECT_DOUBLE_EQ(2, result.second[0].x());
	EXPECT_DOUBLE_EQ(5 ,result.second[1].x());

    // catch a bug
    vec.push_back(Position(6,0));
    vec.push_back(Position(8,0));
    // split at inner point
    result = vec.splitAt(5);
	EXPECT_DOUBLE_EQ(3, result.first.size());
	EXPECT_DOUBLE_EQ(0, result.first[0].x());
	EXPECT_DOUBLE_EQ(2, result.first[1].x());
	EXPECT_DOUBLE_EQ(5, result.first[2].x());
	EXPECT_DOUBLE_EQ(3, result.second.size());
	EXPECT_DOUBLE_EQ(5, result.second[0].x());
	EXPECT_DOUBLE_EQ(6 ,result.second[1].x());
	EXPECT_DOUBLE_EQ(8 ,result.second[2].x());

    // split short vector
    PositionVector vec2;
    vec2.push_back(Position(0,0));
    vec2.push_back(Position(2,0));
    result = vec2.splitAt(1);
	EXPECT_DOUBLE_EQ(2, result.first.size());
	EXPECT_DOUBLE_EQ(0, result.first[0].x());
	EXPECT_DOUBLE_EQ(1, result.first[1].x());
	EXPECT_DOUBLE_EQ(2, result.second.size());
	EXPECT_DOUBLE_EQ(1, result.second[0].x());
	EXPECT_DOUBLE_EQ(2 ,result.second[1].x());

    // split very short vector
    PositionVector vec3;
    vec3.push_back(Position(0,0));
    vec3.push_back(Position(POSITION_EPS,0));
    // supress expected warning
    MsgHandler::getWarningInstance()->removeRetriever(&OutputDevice::getDevice("stderr"));
    result = vec3.splitAt(smallDiff);
    MsgHandler::getWarningInstance()->addRetriever(&OutputDevice::getDevice("stderr"));

	EXPECT_DOUBLE_EQ(2, result.first.size());
	EXPECT_DOUBLE_EQ(0, result.first[0].x());
	EXPECT_DOUBLE_EQ(smallDiff, result.first[1].x());
	EXPECT_DOUBLE_EQ(2, result.second.size());
	EXPECT_DOUBLE_EQ(smallDiff, result.second[0].x());
	EXPECT_DOUBLE_EQ(POSITION_EPS ,result.second[1].x());
}


/* Test the method 'intersectsAtLengths2D'*/
TEST_F(PositionVectorTest, test_method_intersectsAtLenghts2D) {	
    PositionVector vec1;
    vec1.push_back(Position(0,0,42));
    vec1.push_back(Position(100,0,0));

    PositionVector vec2;
    vec2.push_back(Position(0,0,0));
    vec2.push_back(Position(3,1,0));
	EXPECT_DOUBLE_EQ(0, vec1.intersectsAtLengths2D(vec2)[0]);
}


/* Test the method 'nearest_offset_to_point2D'*/
TEST_F(PositionVectorTest, test_method_nearest_offset_to_point2D) {	
    PositionVector vec1;
    vec1.push_back(Position(0,1,0));
    vec1.push_back(Position(0,0,0));
    vec1.push_back(Position(1,0,0));

	EXPECT_DOUBLE_EQ(1, vec1.nearest_offset_to_point2D(Position(-1,-1), false));
	EXPECT_DOUBLE_EQ(1, vec1.nearest_offset_to_point2D(Position(-1,-1), true));
}
