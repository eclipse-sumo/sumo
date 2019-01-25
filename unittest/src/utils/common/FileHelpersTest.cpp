/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    FileHelpersTest.cpp
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @date    2009
/// @version $Id$
///
// Tests FileHelpers class from <SUMO>/src/utils/common
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <gtest/gtest.h>
#include <utils/common/FileHelpers.h>


// ===========================================================================
// test definitions
// ===========================================================================
/* Tests the method checkForRelativity. Unify special filenames and make standard file paths relative to the config. */
TEST(FileHelpers, test_method_checkForRelativity) {
    EXPECT_EQ("stdout", FileHelpers::checkForRelativity("-", "")) << "Special filename '-' should be treated as stdout";
    EXPECT_EQ("/home/user/test.net.xml", FileHelpers::checkForRelativity("test.net.xml", "/home/user/test.sumocfg")) << "configuration path should be used.";
    EXPECT_EQ("/test.net.xml", FileHelpers::checkForRelativity("test.net.xml", "/test.sumocfg")) << "configuration path should be used.";
}


TEST(FileHelpers, test_method_getConfigurationRelative) {
    EXPECT_EQ("/home/user/test.net.xml", FileHelpers::getConfigurationRelative("/home/user/test.sumocfg", "test.net.xml")) << "configuration path should be used.";
    EXPECT_EQ("/test.net.xml", FileHelpers::getConfigurationRelative("/test.sumocfg", "test.net.xml")) << "configuration path should be used.";
}


TEST(FileHelpers, test_method_getFilePath) {
    EXPECT_EQ("/home/user/", FileHelpers::getFilePath("/home/user/test.sumocfg")) << "configuration path should be used.";
    EXPECT_EQ("/", FileHelpers::getFilePath("/test.sumocfg")) << "configuration path should be used.";
}


