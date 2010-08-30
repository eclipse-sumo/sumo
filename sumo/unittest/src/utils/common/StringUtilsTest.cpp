/****************************************************************************/
/// @file    StringUtilsTests.cpp
/// @author  Matthias Heppner
/// @date    2009
/// @version $Id: MESegment.cpp 3081 2010-08-28 06:52:54Z behr_mi $
///
// Tests StringUtils class from <SUMO>/src/utils/common
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
#include <gtest/gtest.h>
#include <utils/common/StringUtils.h>


// ===========================================================================
// test definitions
// ===========================================================================
/* Tests the method prune. Cut the blanks at the beginning and at the end of a string*/
TEST(StringUtils, test_method_prune) {
	EXPECT_EQ("result", StringUtils::prune("  result ")) << "Blanks at the beginning and at the end of a string must be removed.";
}

/* Tests the method to_lower_case.*/
TEST(StringUtils, test_method_to_lower_case) {
	EXPECT_EQ("hello", StringUtils::to_lower_case("HELLO"))<< "String should be converted into small letter.";
	EXPECT_EQ("world", StringUtils::to_lower_case("World"))<< "String should be converted into small letter.";
	std::string str;
	EXPECT_EQ("", StringUtils::to_lower_case(str));
}

/* Tests the method convertUmlaute.*/
TEST(StringUtils, test_method_convertUmlaute) {
	EXPECT_EQ("ae", StringUtils::convertUmlaute("ä"));
	EXPECT_EQ("Ae", StringUtils::convertUmlaute("Ä"));
	EXPECT_EQ("oe", StringUtils::convertUmlaute("ö"));
	EXPECT_EQ("Oe", StringUtils::convertUmlaute("Ö"));
	EXPECT_EQ("ue", StringUtils::convertUmlaute("ü"));
	EXPECT_EQ("Ue", StringUtils::convertUmlaute("Ü"));
	EXPECT_EQ("ss", StringUtils::convertUmlaute("ß"));
	EXPECT_EQ("E", StringUtils::convertUmlaute("É"));
	EXPECT_EQ("e", StringUtils::convertUmlaute("é"));
	EXPECT_EQ("E", StringUtils::convertUmlaute("È"));
	EXPECT_EQ("e", StringUtils::convertUmlaute("è"));
}

/* Tests the method replace. */
TEST(StringUtils, test_method_replace) {	
	EXPECT_EQ("helt", StringUtils::replace("hello","lo","t"));
	EXPECT_EQ("heststo", StringUtils::replace("hello","l","st"));
	EXPECT_EQ("", StringUtils::replace("","l","st"));
}

/* Tests the method replace with empty string. */
TEST(StringUtils, test_method_replace_empty_string) {		
	EXPECT_EQ("", StringUtils::replace("","l","st"));
}

/* Tests the method replace with empty second_argument */
TEST(StringUtils, test_method_replace_empty_second_argument ) {
    EXPECT_EQ("hello", StringUtils::replace("hello","","a"));
}

/* Tests the method replace with empty third_argument */
TEST(StringUtils, test_method_replace_empty_third_argument ) {
	EXPECT_EQ("hello", StringUtils::replace("hello","a",""));
}


/* Tests the method upper. */
TEST(StringUtils, test_method_upper) {
	std::string toUpper = "halloWelt123-?";
	StringUtils::upper(toUpper);
	EXPECT_EQ("HALLOWELT123-?", toUpper) << "It should consist only of capital letters.";
	std::string str;
	StringUtils::upper(str);
	EXPECT_EQ("",str );
}

/* Tests the method toTimeString. */
TEST(StringUtils, test_method_toTimeString) {	
	EXPECT_EQ("-00:00:01", StringUtils::toTimeString(-1));
	EXPECT_EQ("00:00:00", StringUtils::toTimeString(0));
	EXPECT_EQ("01:00:00", StringUtils::toTimeString(3600));
	EXPECT_EQ("00:00:01", StringUtils::toTimeString(1));
	EXPECT_EQ("49:40:00", StringUtils::toTimeString(178800));
	EXPECT_EQ("30883:00:01", StringUtils::toTimeString(111178801));
}

/* Tests the method escapeXML. */
TEST(StringUtils, test_method_escapeXML) {	
	std::string str;
	EXPECT_EQ("", StringUtils::escapeXML(str));
	EXPECT_EQ("test", StringUtils::escapeXML("test"))<< "nothing to be replaced.";
	EXPECT_EQ("test&apos;s", StringUtils::escapeXML("test's"))<< "' must be replaced.";
	EXPECT_EQ("1&lt;2", StringUtils::escapeXML("1<2"))<< "< must be replaced.";
	EXPECT_EQ("2&gt;1", StringUtils::escapeXML("2>1"))<< "> must be replaced.";
	EXPECT_EQ("M&amp;M", StringUtils::escapeXML("M&M"))<< "& must be replaced.";
	EXPECT_EQ("&quot;test&quot;", StringUtils::escapeXML("\"test\""))<< "\" must be replaced.";
}
