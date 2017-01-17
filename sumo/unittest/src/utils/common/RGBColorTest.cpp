/****************************************************************************/
/// @file    RGBColorTest.cpp
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2009
/// @version $Id$
///
// Tests the class RGBColor
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2009-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <gtest/gtest.h>
#include <utils/common/RGBColor.h>
#include <utils/common/UtilExceptions.h>
#include <iostream>
#include <fstream>
#include <sstream>


// ===========================================================================
// test definitions
// ===========================================================================
/* Test the method 'parseColor' with a wrong String. An exception must occur.*/
TEST(RGBColor, test_parseColor_with_wrong_string) {
	ASSERT_THROW(RGBColor::parseColor("a,b,y"),NumberFormatException);
}

/* Test the method 'parseColor'*/
TEST(RGBColor, test_parseColor) {
	RGBColor color = RGBColor::parseColor("0,1,255");
	EXPECT_EQ(0, color.red());
	EXPECT_EQ(1, color.green());
	EXPECT_EQ(255, color.blue());
}

/* Test the method 'parseColor' with thrown EmptyData Exception*/
TEST(RGBColor, test_parseColor_with_a_short_string) {	
	ASSERT_THROW(RGBColor::parseColor(""),EmptyData) << "Expect an EmptyData exception.";	
	ASSERT_THROW(RGBColor::parseColor("1,2"),EmptyData) << "Expect an EmptyData exception.";
	ASSERT_THROW(RGBColor::parseColor("test"),EmptyData) << "Expect an EmptyData exception.";
    ASSERT_THROW(RGBColor::parseColor("1,2,3,5,432test"), EmptyData) << "Expect an EmptyData exception.";
}

/* Test the method 'interpolate'*/
TEST(RGBColor, test_interpolate) {	
	RGBColor color1 = RGBColor(1,2,3);
	RGBColor color2 = RGBColor(3,4,1);
	RGBColor colorResult = RGBColor::interpolate(color1, color2, 0.5);
	EXPECT_EQ(2, colorResult.red());
	EXPECT_EQ(3, colorResult.green());
	EXPECT_EQ(2, colorResult.blue());
}

/* Test the method 'interpolate' with a weight of 1 and higher*/
TEST(RGBColor, test_interpolate_weight_1) {	
	RGBColor color1 = RGBColor(1,2,3);
	RGBColor color2 = RGBColor(3,4,1);
	RGBColor colorResult = RGBColor::interpolate(color1, color2, 0.5);
	RGBColor colorResult1 = RGBColor::interpolate(color1, color2, 0);
	RGBColor colorResult2 = RGBColor::interpolate(color1, color2, 1);
	EXPECT_TRUE(color1==colorResult1);
	EXPECT_TRUE(color2==colorResult2);
	EXPECT_EQ(2, colorResult.red());
	EXPECT_EQ(3, colorResult.green());
	EXPECT_EQ(2, colorResult.blue());
}

/* Test the method 'interpolate' with a weight of 0 and lower*/
TEST(RGBColor, test_interpolate_weight_0) {	
	RGBColor color1 = RGBColor(1,2,3);
	RGBColor color2 = RGBColor(2,4,2);
	RGBColor colorResult = RGBColor::interpolate(color1, color2, 0);
	RGBColor colorResult2 = RGBColor::interpolate(color1, color2, -1000);
	EXPECT_TRUE(colorResult==colorResult2);
	EXPECT_EQ(1, colorResult.red());
	EXPECT_EQ(2, colorResult.green());
	EXPECT_EQ(3, colorResult.blue());
}

/* Test the operator '=='. It has an tolerance of 1 */
TEST(RGBColor, test_operator_equal) {	
	RGBColor color1 = RGBColor(1,2,3);
	RGBColor color2 = RGBColor(1,2,3);	
	EXPECT_TRUE(color1==color2 );	
	color2 = RGBColor(1,2,4);	
	EXPECT_FALSE(color1==color2 );
}
