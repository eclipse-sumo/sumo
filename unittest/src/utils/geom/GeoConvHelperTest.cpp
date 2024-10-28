/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GeoConvHelperTest.cpp
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @date    2011-09-23
///
// Tests the class GeoConvHelper
/****************************************************************************/
#include <config.h>

#include <gtest/gtest.h>
#include <utils/geom/GeoConvHelper.h>

/*
Tests the class GeoConvHelper
*/


/* Test the method 'x2cartesian' */
TEST(GeoConvHelper, test_method_x2cartesian) {
    GeoConvHelper gch(
        "+proj=utm +zone=33 +ellps=WGS84 +datum=WGS84 +units=m +no_defs",
        Position(), Boundary(), Boundary(), 1, false);

    Position pos(13.5326994, 52.428098100000007);
    gch.x2cartesian(pos);

    EXPECT_NEAR(400235.50494557252, pos.x(), 1e-5);
    EXPECT_NEAR(5809666.826070101, pos.y(), 1e-5);
}

/* Test the method 'cartesian2geo' */
TEST(GeoConvHelper, test_method_cartesian2geo) {
    GeoConvHelper gch(
        "+proj=utm +zone=33 +ellps=WGS84 +datum=WGS84 +units=m +no_defs",
        Position(), Boundary(), Boundary(), 1, false);

    // Use the outputs of the `x2cartesian` test - hence making the combination of tests circular: geo -> cartesian -> geo
    Position cartesian(400235.50494557252, 5809666.826070101);
    gch.cartesian2geo(cartesian);

    EXPECT_NEAR(13.5326994, cartesian.x(), 1e-5);
    EXPECT_NEAR(52.428098100000007, cartesian.y(), 1e-5);
}
