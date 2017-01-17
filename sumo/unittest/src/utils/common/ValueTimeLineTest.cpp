/****************************************************************************/
/// @file    ValueTimeLineTest.cpp
/// @author  Daniel Krajzewicz
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @date    Sept 2009
/// @version $Id$
///
// Tests ValueTimeLine class from <SUMO>/src/utils/common
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <gtest/gtest.h>
#include <utils/common/ValueTimeLine.h>


// ===========================================================================
// test definitions
// ===========================================================================
/* Tests what happens if one tries to get a value from an empty ValueTimeLine. */
/*
TEST(ValueTimeLine, test_get_from_empty) {
    ValueTimeLine<int> vtl;
    EXPECT_EQ(1, vtl.getValue(0)) << "Something should happen if nothing was stored.";
}
*/


// --------------------------------
// plain retrieval / overwriting tests
// --------------------------------

/* Tests what happens if one tries to get a stored value (one value stored, fillGaps not called). */
TEST(ValueTimeLine, test_get_single_nocollect) {
    ValueTimeLine<int> vtl;
    vtl.add(0, 100, 2);
    EXPECT_EQ(2, vtl.getValue(0)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(99)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(50)) << "The stored number should be returned when asking within the interval.";
}

/* Tests what happens if one tries to get a stored value (three values stored, fillGaps not called). */
TEST(ValueTimeLine, test_get_multi_nocollect) {
    ValueTimeLine<int> vtl;
    vtl.add(0, 100, 1);
    vtl.add(100, 200, 2);
    vtl.add(200, 300, 3);
    EXPECT_EQ(1, vtl.getValue(0)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(1, vtl.getValue(99)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(1, vtl.getValue(50)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(100)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(199)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(150)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(200)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(299)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(250)) << "The stored number should be returned when asking within the interval.";
}

/* Tests what happens if one tries to get a stored value (one value stored, fillGaps called). */
TEST(ValueTimeLine, test_get_single_collect) {
    ValueTimeLine<int> vtl;
    vtl.add(0, 100, 2);
    vtl.fillGaps(0);
    EXPECT_EQ(2, vtl.getValue(0)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(99)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(50)) << "The stored number should be returned when asking within the interval.";
}

/* Tests what happens if one tries to get a stored value (three values stored, fillGaps called). */
TEST(ValueTimeLine, test_get_multi_collect) {
    ValueTimeLine<int> vtl;
    vtl.add(0, 100, 1);
    vtl.add(100, 200, 2);
    vtl.add(200, 300, 3);
    vtl.fillGaps(0);
    EXPECT_EQ(1, vtl.getValue(0)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(1, vtl.getValue(99)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(1, vtl.getValue(50)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(100)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(199)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(150)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(200)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(299)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(250)) << "The stored number should be returned when asking within the interval.";
}

// --------------------------------
// overwriting filling tests
// --------------------------------

/* Tests what happens if one overwrites a value (three values stored, fillGaps not called). */
TEST(ValueTimeLine, test_overwrite_nocollect) {
    ValueTimeLine<int> vtl;
    vtl.add(0, 100, 1);
    vtl.add(200, 300, 3);
    vtl.add(50, 250, 2);
    EXPECT_EQ(1, vtl.getValue(0)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(99)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(50)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(100)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(199)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(150)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(200)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(299)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(250)) << "The stored number should be returned when asking within the interval.";
}

/* Tests what happens if one overwrites a value (three values stored, fillGaps called). */
TEST(ValueTimeLine, test_overwrite_collect) {
    ValueTimeLine<int> vtl;
    vtl.add(0, 100, 1);
    vtl.add(200, 300, 3);
    vtl.add(50, 250, 2);
    vtl.fillGaps(0);
    EXPECT_EQ(1, vtl.getValue(0)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(99)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(50)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(100)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(199)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(150)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(200)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(299)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(250)) << "The stored number should be returned when asking within the interval.";
}

/* Tests what happens if one overwrites a value (three values stored, fillGaps not called, order changed). */
TEST(ValueTimeLine, test_overwrite_nocollect2) {
    ValueTimeLine<int> vtl;
    vtl.add(50, 250, 2);
    vtl.add(0, 100, 1);
    vtl.add(200, 300, 3);
    EXPECT_EQ(1, vtl.getValue(0)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(1, vtl.getValue(99)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(1, vtl.getValue(50)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(100)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(199)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(150)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(200)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(299)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(250)) << "The stored number should be returned when asking within the interval.";
}

/* Tests what happens if one overwrites a value (three values stored, fillGaps called, order changed). */
TEST(ValueTimeLine, test_overwrite_collect2) {
    ValueTimeLine<int> vtl;
    vtl.add(50, 250, 2);
    vtl.add(0, 100, 1);
    vtl.add(200, 300, 3);
    vtl.fillGaps(0);
    EXPECT_EQ(1, vtl.getValue(0)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(1, vtl.getValue(99)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(1, vtl.getValue(50)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(100)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(199)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(150)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(200)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(299)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(3, vtl.getValue(250)) << "The stored number should be returned when asking within the interval.";
}


// --------------------------------
// gap filling tests
// --------------------------------

/* Tests what happens if one overwrites a value (three values stored, fillGaps called). */
TEST(ValueTimeLine, test_fill_gaps_withbounds) {
    ValueTimeLine<int> vtl;
    vtl.add(50, 250, 2);
    vtl.fillGaps(4, true);
    EXPECT_EQ(2, vtl.getValue(0)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(99)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(50)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(100)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(199)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(150)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(200)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(299)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(250)) << "The stored number should be returned when asking within the interval.";
}


/* Tests what happens if one overwrites a value (three values stored, fillGaps called). */
TEST(ValueTimeLine, test_fill_gaps_nobounds) {
    ValueTimeLine<int> vtl;
    vtl.add(50, 250, 2);
    vtl.fillGaps(4, false);
    EXPECT_EQ(4, vtl.getValue(0)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(99)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(50)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(100)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(199)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(150)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(2, vtl.getValue(200)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(4, vtl.getValue(299)) << "The stored number should be returned when asking within the interval.";
    EXPECT_EQ(4, vtl.getValue(250)) << "The stored number should be returned when asking within the interval.";
}


