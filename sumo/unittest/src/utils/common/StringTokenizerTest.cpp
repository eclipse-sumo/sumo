/****************************************************************************/
/// @file    StringTokenizerTest.cpp
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @date    2009-03-24
/// @version $Id$
///
// 
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>

/*
Tests StringTokenizer class from <SUMO>/src/utils/common
*/

/* Tests the behaviour with a StringTokenizer::WHITECHAR for splitting a string.*/
TEST(StringTokenizer, test_split_with_whitechar) {
    StringTokenizer strTok("Hello  World", StringTokenizer::WHITECHARS);
    EXPECT_TRUE(strTok.hasNext()) << "There must be more tokens available.";
    EXPECT_EQ("Hello",strTok.next());
    EXPECT_EQ("World",strTok.next());
    EXPECT_FALSE(strTok.hasNext()) << "No tokens should be available.";
}

/* Tests the behaviour with a StringTokenizer::NEWLINE for splitting a string.*/
TEST(StringTokenizer, test_split_with_newline) {
    StringTokenizer strTok("Hello\nWorld", StringTokenizer::NEWLINE);
    EXPECT_TRUE(strTok.hasNext()) << "There must be more tokens available.";
    EXPECT_EQ("Hello",strTok.next());
    EXPECT_EQ("World",strTok.next());
    EXPECT_FALSE(strTok.hasNext()) << "No tokens should be available.";
}

/* Tests the behaviour with any tokens for splitting a string.*/
TEST(StringTokenizer, test_split_with_x) {
    StringTokenizer strTok("HelloxxWorld", "x");
    EXPECT_TRUE(strTok.hasNext()) << "There must be more tokens available.";
    EXPECT_EQ("Hello",strTok.next());
    EXPECT_EQ("",strTok.next());
    EXPECT_EQ("World",strTok.next());
    EXPECT_FALSE(strTok.hasNext()) << "No tokens should be available.";
}

/* Tests the behaviour with any tokens for splitting a string with the option splitAtAllChars=true*/
TEST(StringTokenizer, test_split_any_char) {
    StringTokenizer strTok("HelloxWyorld", "xy", true);
    EXPECT_TRUE(strTok.hasNext()) << "There must be more tokens available.";
    EXPECT_EQ("Hello",strTok.next());
    EXPECT_EQ("W",strTok.next());
    EXPECT_EQ("orld",strTok.next());
    EXPECT_FALSE(strTok.hasNext()) << "No tokens should be available.";
}

/* Tests the method reinit*/
TEST(StringTokenizer, test_method_reinit) {
    StringTokenizer strTok("Hello");
    strTok.next();
    EXPECT_FALSE(strTok.hasNext()) << "No tokens should be available.";
    strTok.reinit();
    EXPECT_TRUE(strTok.hasNext()) << "There must be more tokens available.";
}

/* Tests the method size*/
TEST(StringTokenizer, test_method_size) {
    StringTokenizer strTok("Hello little World");
    EXPECT_EQ(3,strTok.size()) << "The number of the token is not right.";
    StringTokenizer strTok2("");
    EXPECT_EQ(0,strTok2.size()) << "The number of the token is not right.";
}

/* Tests the method front*/
TEST(StringTokenizer, test_method_front) {
    StringTokenizer strTok("Hello World");
    EXPECT_EQ("Hello",strTok.front()) << "The first token is not right.";
    strTok.next();
    EXPECT_EQ("Hello",strTok.front()) << "The first token is not right.";
}

/* Tests the method get*/
TEST(StringTokenizer, test_method_get) {
    StringTokenizer strTok("Hello World");
    EXPECT_EQ("Hello",strTok.get(0)) << "The first token is not right.";
    EXPECT_EQ("World",strTok.get(1)) << "The second token is not right.";
    ASSERT_THROW(strTok.get(2),OutOfBoundsException) << "Expect an OutOfBoundsException exception.";
}

/* Tests the method get with empty data*/
TEST(StringTokenizer, test_method_get_with_empty_data) {
    StringTokenizer strTok;
    ASSERT_THROW(strTok.get(0),OutOfBoundsException) << "Expect an OutOfBoundsException exception.";
}

/* Tests the method getVector*/
TEST(StringTokenizer, test_method_getVector) {
    StringTokenizer strTok("Hello World");
    std::vector<std::string> strVek = strTok.getVector();
    EXPECT_EQ("World",strVek.back());
    EXPECT_EQ("Hello",strVek.front());
}
