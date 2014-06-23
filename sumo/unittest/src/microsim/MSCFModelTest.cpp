/****************************************************************************/
/// @file    MSCFModelTest.cpp
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    2013-06-05
/// @version $Id$
///
// Tests the cfmodel functions 
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
#include <utils/common/SUMOVTypeParameter.h>
#include <microsim/MSVehicleType.h>
#include <microsim/cfmodels/MSCFModel.h>
#include <microsim/cfmodels/MSCFModel_Krauss.h>


class MSCFModelTest : public testing::Test {
	protected :
		MSVehicleType* type;
		MSCFModel* m;
        SUMOReal accel;
        SUMOReal decel;
        SUMOReal dawdle;
        SUMOReal tau; // headway time

		virtual void SetUp(){
            accel = 2;
            decel = 4;
            dawdle = 0;
            tau = 1;
            type = new MSVehicleType(SUMOVTypeParameter("0"));
            m = new MSCFModel_Krauss(type,
                    accel, decel, dawdle, tau);
		}

		virtual void TearDown(){
			delete m;
			delete type;
		}
};

/* Test the method 'brakeGap'.*/

TEST_F(MSCFModelTest, test_method_brakeGap) {	
    // discrete braking model. keep driving for 1 s
    const SUMOReal v = 3;
	EXPECT_DOUBLE_EQ(tau * v, m->brakeGap(v)); 
}

TEST_F(MSCFModelTest, test_method_static_brakeGap) {	
    const SUMOReal v = 3;
    const SUMOReal b = 4;
    const SUMOReal tau = 1;
	EXPECT_DOUBLE_EQ(3, MSCFModel::brakeGap(v, b, tau)); 
	EXPECT_DOUBLE_EQ(0, MSCFModel::brakeGap(v, b, 0)); 
	EXPECT_DOUBLE_EQ(3, MSCFModel::brakeGap(7, b, 0)); 
	EXPECT_DOUBLE_EQ(28.6, MSCFModel::brakeGap(18.4, 4.5, 0)); 
}

TEST_F(MSCFModelTest, test_method_static_freeSpeed) {	
    const SUMOReal b = 4;
    const SUMOReal v = 0;
    const SUMOReal g = 4;
	EXPECT_DOUBLE_EQ(4, MSCFModel::freeSpeed(b, g, v)); 
	EXPECT_DOUBLE_EQ(20, MSCFModel::freeSpeed(5, 30, 10)); 
	EXPECT_DOUBLE_EQ(18.4, MSCFModel::freeSpeed(4.5, 30, 13.9)); 
	EXPECT_DOUBLE_EQ(22.9, MSCFModel::freeSpeed(4.5, 40, 13.9)); 
}



