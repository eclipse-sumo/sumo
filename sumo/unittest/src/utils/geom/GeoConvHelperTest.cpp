/****************************************************************************/
/// @file    GeoConvHelperTest.cpp
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @date    2011-09-23
/// @version $Id$
///
// Tests the class RandHelper
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
#include <utils/geom/GeoConvHelper.h>

using namespace std;

/*
Tests the class GeoConvHelper
*/


/* Test the method 'move2side' */
TEST(GeoConvHelper, test_method_x2cartesian) {
    GeoConvHelper gch(
            "+proj=utm +zone=33 +ellps=WGS84 +datum=WGS84 +units=m +no_defs",
            Position(), Boundary(), Boundary(), 0, false);

    Position pos(13.5326994, 52.428098100000007);
    gch.x2cartesian(pos);
    
	EXPECT_FLOAT_EQ(400235.50494557252, pos.x());
	EXPECT_FLOAT_EQ(5809666.826070101, pos.y());
}
