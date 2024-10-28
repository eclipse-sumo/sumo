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
/// @file    StringUtilsTest.cpp
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @date    2009
///
// Tests StringUtils class from <SUMO>/src/utils/common
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <gtest/gtest.h>
#include <utils/common/StringUtils.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// test definitions
// ===========================================================================
/* Tests the method prune. Cut the blanks at the beginning and at the end of a string*/
TEST(StringUtils, test_method_prune) {
    EXPECT_EQ("result", StringUtils::prune("  result ")) << "Blanks at the beginning and at the end of a string must be removed.";
    EXPECT_EQ("", StringUtils::prune("  ")) << "Blanks at the beginning and at the end of a string must be removed.";
}

/* Tests the method to_lower_case.*/
TEST(StringUtils, test_method_to_lower_case) {
    EXPECT_EQ("hello", StringUtils::to_lower_case("HELLO")) << "String should be converted into small letter.";
    EXPECT_EQ("world", StringUtils::to_lower_case("World")) << "String should be converted into small letter.";
    std::string str;
    EXPECT_EQ("", StringUtils::to_lower_case(str));
}

/* Tests the method to_lower_case.*/
TEST(StringUtils, test_method_latin1_to_utf8) {
    EXPECT_EQ("\xC3\xA4", StringUtils::latin1_to_utf8("\xE4"));
    EXPECT_EQ("\xC3\xB6", StringUtils::latin1_to_utf8("\xF6"));
    std::string str;
    EXPECT_EQ("", StringUtils::latin1_to_utf8(str));
}

/* Tests the method convertUmlaute.*/
TEST(StringUtils, test_method_convertUmlaute) {
    EXPECT_EQ("ae", StringUtils::convertUmlaute("\xE4"));
    EXPECT_EQ("Ae", StringUtils::convertUmlaute("\xC4"));
    EXPECT_EQ("oe", StringUtils::convertUmlaute("\xF6"));
    EXPECT_EQ("Oe", StringUtils::convertUmlaute("\xD6"));
    EXPECT_EQ("ue", StringUtils::convertUmlaute("\xFC"));
    EXPECT_EQ("Ue", StringUtils::convertUmlaute("\xDC"));
    EXPECT_EQ("ss", StringUtils::convertUmlaute("\xDF"));
    EXPECT_EQ("E", StringUtils::convertUmlaute("\xC9"));
    EXPECT_EQ("e", StringUtils::convertUmlaute("\xE9"));
    EXPECT_EQ("E", StringUtils::convertUmlaute("\xC8"));
    EXPECT_EQ("e", StringUtils::convertUmlaute("\xE8"));
    EXPECT_EQ("normal_string_no_umlaute", StringUtils::convertUmlaute("normal_string_no_umlaute"));
}

/* Tests the method replace. */
TEST(StringUtils, test_method_replace) {
    EXPECT_EQ("helt", StringUtils::replace("hello", "lo", "t"));
    EXPECT_EQ("heststo", StringUtils::replace("hello", "l", "st"));
    EXPECT_EQ("", StringUtils::replace("", "l", "st"));
}

/* Tests the method replace with empty string. */
TEST(StringUtils, test_method_replace_empty_string) {
    EXPECT_EQ("", StringUtils::replace("", "l", "st"));
}

/* Tests the method replace with empty second_argument */
TEST(StringUtils, test_method_replace_empty_second_argument) {
    EXPECT_EQ("hello", StringUtils::replace("hello", "", "a"));
}

/* Tests the method replace with empty third_argument */
TEST(StringUtils, test_method_replace_empty_third_argument) {
    EXPECT_EQ("hello", StringUtils::replace("hello", "a", ""));
    EXPECT_EQ("heo", StringUtils::replace("hello", "l", ""));
    EXPECT_EQ("he", StringUtils::replace("hell", "l", ""));
    EXPECT_EQ("test", StringUtils::replace("ltestl", "l", ""));
}

/* Tests the method escapeXML. */
TEST(StringUtils, test_method_escapeXML) {
    std::string str;
    EXPECT_EQ("", StringUtils::escapeXML(str));
    EXPECT_EQ("test", StringUtils::escapeXML("test")) << "nothing to be replaced.";
    EXPECT_EQ("test&apos;s", StringUtils::escapeXML("test's")) << "' must be replaced.";
    EXPECT_EQ("1&lt;2", StringUtils::escapeXML("1<2")) << "< must be replaced.";
    EXPECT_EQ("2&gt;1", StringUtils::escapeXML("2>1")) << "> must be replaced.";
    EXPECT_EQ("M&amp;M", StringUtils::escapeXML("M&M")) << "& must be replaced.";
    EXPECT_EQ("&quot;test&quot;", StringUtils::escapeXML("\"test\"")) << "\" must be replaced.";
    EXPECT_EQ("test", StringUtils::escapeXML("\01test\01"));
}


TEST(StringUtils, test_toInt) {
    EXPECT_EQ(0, StringUtils::toInt("0"));
    EXPECT_EQ(1, StringUtils::toInt("+1"));
    EXPECT_EQ(-1, StringUtils::toInt("-1"));
    EXPECT_THROW(StringUtils::toInt("1e0"), NumberFormatException);
    EXPECT_THROW(StringUtils::toInt("100000000000"), NumberFormatException);
    EXPECT_THROW(StringUtils::toInt(""), EmptyData);
}

TEST(StringUtils, test_toLong) {
    EXPECT_EQ(0, StringUtils::toLong("0"));
    EXPECT_EQ(1, StringUtils::toLong("+1"));
    EXPECT_EQ(-1, StringUtils::toLong("-1"));
    EXPECT_THROW(StringUtils::toLong("1e0"), NumberFormatException);
    EXPECT_EQ(100000000000, StringUtils::toLong("100000000000"));
    EXPECT_THROW(StringUtils::toLong(""), EmptyData);
}

TEST(StringUtils, test_toDouble) {
    EXPECT_EQ(0., StringUtils::toDouble("0"));
    EXPECT_EQ(1., StringUtils::toDouble("+1"));
    EXPECT_EQ(-1., StringUtils::toDouble("-1"));
    EXPECT_EQ(1., StringUtils::toDouble("1e0"));
    EXPECT_EQ(10., StringUtils::toDouble("1e1"));
    EXPECT_EQ(1., StringUtils::toDouble("1."));
    EXPECT_EQ(1.1, StringUtils::toDouble("1.1"));
    EXPECT_EQ(.1, StringUtils::toDouble(".1"));
    EXPECT_THROW(StringUtils::toDouble("1,1"), NumberFormatException);
    EXPECT_THROW(StringUtils::toDouble(",1"), NumberFormatException);
    EXPECT_EQ(100000000000., StringUtils::toDouble("100000000000"));
    EXPECT_THROW(StringUtils::toDouble(""), EmptyData);
    EXPECT_THROW(StringUtils::toDouble("1e0x"), NumberFormatException);
    EXPECT_THROW(StringUtils::toDouble("1x"), NumberFormatException);
}

TEST(StringUtils, test_toBool) {
    // according to gtest issue 322 EXPECT_EQ(false, ...) triggers a gcc bug
    EXPECT_EQ(true, StringUtils::toBool("true"));
    EXPECT_FALSE(StringUtils::toBool("false"));
    EXPECT_EQ(true, StringUtils::toBool("True"));
    EXPECT_FALSE(StringUtils::toBool("False"));
    EXPECT_EQ(true, StringUtils::toBool("yes"));
    EXPECT_FALSE(StringUtils::toBool("no"));
    EXPECT_EQ(true, StringUtils::toBool("on"));
    EXPECT_FALSE(StringUtils::toBool("off"));
    EXPECT_EQ(true, StringUtils::toBool("1"));
    EXPECT_FALSE(StringUtils::toBool("0"));
    EXPECT_EQ(true, StringUtils::toBool("x"));
    EXPECT_FALSE(StringUtils::toBool("-"));
    EXPECT_EQ(true, StringUtils::toBool("ON"));
    EXPECT_THROW(StringUtils::toBool(""), EmptyData);
    EXPECT_THROW(StringUtils::toBool("1e0"), BoolFormatException);
    EXPECT_THROW(StringUtils::toBool("Trari"), BoolFormatException);
    EXPECT_THROW(StringUtils::toBool("yessir"), BoolFormatException);
}

TEST(StringUtils, test_parseDist) {
    EXPECT_EQ(0., StringUtils::parseDist("0"));
    EXPECT_EQ(1., StringUtils::parseDist("1"));
    EXPECT_EQ(1., StringUtils::parseDist("1m"));
    EXPECT_EQ(1., StringUtils::parseDist("1 m"));
    EXPECT_EQ(1000., StringUtils::parseDist("1 km"));
    EXPECT_THROW(StringUtils::parseDist(""), EmptyData);
    EXPECT_THROW(StringUtils::parseDist("1xm"), NumberFormatException);
}

TEST(StringUtils, test_parseSpeed) {
    EXPECT_EQ(0., StringUtils::parseSpeed("0"));
    EXPECT_EQ(1., StringUtils::parseSpeed("3.6"));
    EXPECT_EQ(1., StringUtils::parseSpeed("1m/s"));
    EXPECT_EQ(1., StringUtils::parseSpeed("1 m/s"));
    EXPECT_EQ(1., StringUtils::parseSpeed("3.6 km/h"));
    EXPECT_EQ(1., StringUtils::parseSpeed("3.6 kmh"));
    EXPECT_EQ(1., StringUtils::parseSpeed("3.6 kmph"));
    EXPECT_DOUBLE_EQ(1.609344 / 3.6, StringUtils::parseSpeed("1 mph"));
    EXPECT_THROW(StringUtils::parseSpeed(""), EmptyData);
    EXPECT_THROW(StringUtils::parseSpeed("1xm"), NumberFormatException);
}
