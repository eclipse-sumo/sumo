/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TplConvertTest.cpp
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Oct 2010
/// @version $Id$
///
// Tests the class TplConvert
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
#include <utils/common/TplConvert.h>


// ===========================================================================
// test definitions
// ===========================================================================
/* Test the method '_2int'.*/
TEST(TplConvert, test_2int) {
    EXPECT_EQ(0, TplConvert::_2int("0"));
    EXPECT_EQ(1, TplConvert::_2int("+1"));
    EXPECT_EQ(-1, TplConvert::_2int("-1"));
    EXPECT_THROW(TplConvert::_2int("1e0"), NumberFormatException);
    EXPECT_THROW(TplConvert::_2int("100000000000"), NumberFormatException);
    EXPECT_THROW(TplConvert::_2int(""), EmptyData);
}


/* Test the method '_2long'.*/
TEST(TplConvert, test_2long) {
    EXPECT_EQ(0, TplConvert::_2long("0"));
    EXPECT_EQ(1, TplConvert::_2long("+1"));
    EXPECT_EQ(-1, TplConvert::_2long("-1"));
    EXPECT_THROW(TplConvert::_2long("1e0"), NumberFormatException);
    EXPECT_EQ(100000000000, TplConvert::_2long("100000000000"));
    EXPECT_THROW(TplConvert::_2long(""), EmptyData);
}


/* Test the method '_2double'.*/
TEST(TplConvert, test_2double) {
    EXPECT_EQ(0, TplConvert::_2double("0"));
    EXPECT_EQ(1, TplConvert::_2double("+1"));
    EXPECT_EQ(-1, TplConvert::_2double("-1"));
    EXPECT_EQ(1, TplConvert::_2double("1e0"));
    EXPECT_EQ(10, TplConvert::_2double("1e1"));
    EXPECT_EQ(1, TplConvert::_2double("1."));
    EXPECT_EQ(1.1, TplConvert::_2double("1.1"));
    EXPECT_EQ(.1, TplConvert::_2double(".1"));
    EXPECT_EQ(1.1, TplConvert::_2double("1,1"));
    EXPECT_EQ(.1, TplConvert::_2double(",1"));
    EXPECT_EQ(100000000000, TplConvert::_2double("100000000000"));
    EXPECT_THROW(TplConvert::_2double(""), EmptyData);
    EXPECT_THROW(TplConvert::_2double("1e0x"), NumberFormatException);
    EXPECT_THROW(TplConvert::_2double("1x"), NumberFormatException);
}


/* Test the method '_2bool'.*/
TEST(TplConvert, test_2bool) {
    // according to gtest issue 322 EXPECT_EQ(false, ...) triggers a gcc bug
    EXPECT_EQ(true, TplConvert::_2bool("true"));
    EXPECT_FALSE(TplConvert::_2bool("false"));
    EXPECT_EQ(true, TplConvert::_2bool("True"));
    EXPECT_FALSE(TplConvert::_2bool("False"));
    EXPECT_EQ(true, TplConvert::_2bool("yes"));
    EXPECT_FALSE(TplConvert::_2bool("no"));
    EXPECT_EQ(true, TplConvert::_2bool("on"));
    EXPECT_FALSE(TplConvert::_2bool("off"));
    EXPECT_EQ(true, TplConvert::_2bool("1"));
    EXPECT_FALSE(TplConvert::_2bool("0"));
    EXPECT_EQ(true, TplConvert::_2bool("x"));
    EXPECT_FALSE(TplConvert::_2bool("-"));
    EXPECT_EQ(true, TplConvert::_2bool("ON"));
    EXPECT_THROW(TplConvert::_2bool(""), EmptyData);
    EXPECT_THROW(TplConvert::_2bool("1e0"), BoolFormatException);
    EXPECT_THROW(TplConvert::_2bool("Trari"), BoolFormatException);
    EXPECT_THROW(TplConvert::_2bool("yessir"), BoolFormatException);
}
