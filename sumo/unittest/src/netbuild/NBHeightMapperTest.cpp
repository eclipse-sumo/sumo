/****************************************************************************/
/// @file    NBHeightMapperTest.cpp
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    2014-09-09
/// @version $Id$
///
// Tests the class NBHeightMapper
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <netbuild/NBHeightMapper.h>

class NBHeightMapperTest : public testing::Test {
protected :

    virtual void SetUp(){
        NBHeightMapper &hm = NBHeightMapper::Singleton;
        PositionVector t1;
        t1.push_back(Position(0,0,0));
        t1.push_back(Position(1,0,0));
        t1.push_back(Position(0,1,0));
        hm.addTriangle(t1);

        PositionVector t2;
        t2.push_back(Position(1,0,1));
        t2.push_back(Position(1,1,1));
        t2.push_back(Position(0,1,1));
        hm.addTriangle(t2);

        PositionVector t3;
        t3.push_back(Position(1,0,0));
        t3.push_back(Position(3,0,4));
        t3.push_back(Position(1,2,4));
        hm.addTriangle(t3);
    }

    virtual void TearDown(){
        NBHeightMapper &hm = NBHeightMapper::Singleton;
        hm.clearData();
    }
};

/* Test the method 'getZ'*/
TEST_F(NBHeightMapperTest, test_method_getZ) {
    const NBHeightMapper &hm = NBHeightMapper::get();
	EXPECT_TRUE(hm.ready());
	EXPECT_FLOAT_EQ(0, hm.getZ(Position(0.25,0.25)));
	EXPECT_FLOAT_EQ(1, hm.getZ(Position(0.75,0.75)));
	EXPECT_FLOAT_EQ(2, hm.getZ(Position(1.5,0.5)));
	//EXPECT_FLOAT_EQ(0.5, hm.getZ(Position(0.5,0.5, 100)));
}


