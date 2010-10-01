/****************************************************************************/
/// @file    RandHelperTest.cpp
/// @author  Michael Behrisch
/// @date    Oct 2010
/// @version $Id$
///
// Tests the class RandHelper
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/common/StdDefs.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// test definitions
// ===========================================================================
/* Test the method 'rand' without parameters.*/
TEST(RandHelper, test_rand_range) {
    for (int i = 0; i < 1000; i++) {
        const SUMOReal rand = RandHelper::rand();
        EXPECT_LT(rand, SUMOReal(1));
        EXPECT_LE(SUMOReal(0), rand);
    }
}

/* Test the method 'rand' with an upper float limit.*/
TEST(RandHelper, test_rand_range_float) {
    for (int i = 0; i < 1000; i++) {
        const SUMOReal rand = RandHelper::rand(SUMOReal(10));
        EXPECT_LT(rand, SUMOReal(10));
        EXPECT_LE(SUMOReal(0), rand);
    }
}

/* Test the method 'rand' with an upper int limit.*/
TEST(RandHelper, test_rand_range_int) {
    for (int i = 0; i < 1000; i++) {
        const SUMOReal rand = RandHelper::rand(100);
        EXPECT_LT(rand, 100);
        EXPECT_LE(0, rand);
    }
}

/* Test the method 'rand' with two float limits.*/
TEST(RandHelper, test_rand_range_two_float) {
    for (int i = 0; i < 1000; i++) {
        const SUMOReal rand = RandHelper::rand(SUMOReal(0.1), SUMOReal(0.5));
        EXPECT_LT(rand, SUMOReal(0.5));
        EXPECT_LE(SUMOReal(0.1), rand);
    }
}

/* Test the method 'rand' with two int limits.*/
TEST(RandHelper, test_rand_range_two_int) {
    for (int i = 0; i < 1000; i++) {
        const SUMOReal rand = RandHelper::rand(50, 100);
        EXPECT_LT(rand, 100);
        EXPECT_LE(50, rand);
    }
}

/* Test whether the 'rand' distribution is more or less uniform.*/
TEST(RandHelper, test_uniform) {
    int count[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 1000; i++) {
        const SUMOReal rand = RandHelper::rand(SUMOReal(0), SUMOReal(10));
        count[(int)rand]++;
    }
    for (int i = 0; i < 10; i++) {
        EXPECT_LE(50, count[i]) << "Testing interval " << i;
        EXPECT_LT(count[i], 150) << "Testing interval " << i;
    }
}

/* Test whether the 'randNorm' distribution is more or less gaussian.*/
TEST(RandHelper, test_norm) {
    int count[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 1000; i++) {
        const SUMOReal rand = RandHelper::randNorm(SUMOReal(5), SUMOReal(2));
        count[MIN2(MAX2((int)rand,0),9)]++;
    }
    EXPECT_LE(0, count[0]);
    EXPECT_LT(count[0], 100);
    EXPECT_LE(0, count[1]);
    EXPECT_LT(count[1], 100);
    EXPECT_LE(50, count[2]);
    EXPECT_LT(count[2], 150);
    EXPECT_LE(100, count[3]);
    EXPECT_LT(count[3], 200);
    EXPECT_LE(100, count[4]);
    EXPECT_LT(count[4], 250);
    EXPECT_LE(100, count[5]);
    EXPECT_LT(count[5], 250);
    EXPECT_LE(100, count[6]);
    EXPECT_LT(count[6], 200);
    EXPECT_LE(0, count[7]);
    EXPECT_LT(count[7], 100);
    EXPECT_LE(0, count[8]);
    EXPECT_LT(count[8], 100);
    EXPECT_LE(0, count[9]);
    EXPECT_LT(count[9], 100);
}

