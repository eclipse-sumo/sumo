/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    FileHelpersTest.cpp
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @date    2009
///
// Tests FileHelpers class from <SUMO>/src/utils/common
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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


TEST(FileHelpers, test_method_fixRelative) {
    EXPECT_EQ("test.net.xml", FileHelpers::fixRelative("test.net.xml", "test.sumocfg", false, "/home/user")) << "no change for two filenames without paths.";
    EXPECT_EQ("test.net.xml", FileHelpers::fixRelative("test.net.xml", "test.sumocfg", true, "/home/user")) << "no change for two filenames without paths.";
    EXPECT_EQ("/home/user/test.net.xml", FileHelpers::fixRelative("/home/user/test.net.xml", "/home/user/test.sumocfg", false, "/home/user")) << "absolute path is kept without force.";
    EXPECT_EQ("test.net.xml", FileHelpers::fixRelative("/home/user/test.net.xml", "/home/user/test.sumocfg", true, "/home/user")) << "absolute path is removed with force.";
    EXPECT_EQ("test.net.xml", FileHelpers::fixRelative("user/test.net.xml", "user/test.sumocfg", false, "/home/user")) << "relative path is removed without force.";
    EXPECT_EQ("test.net.xml", FileHelpers::fixRelative("user/test.net.xml", "user/test.sumocfg", true, "/home/user")) << "relative path is removed with force.";
    EXPECT_EQ("subdir/test.net.xml", FileHelpers::fixRelative("subdir/test.net.xml", "test.sumocfg", false, "/home/user")) << "config without path means no change.";
    EXPECT_EQ("subdir/test.net.xml", FileHelpers::fixRelative("subdir/test.net.xml", "test.sumocfg", true, "/home/user")) << "config without path means no change.";
    EXPECT_EQ("user/subdir/test.net.xml", FileHelpers::fixRelative("subdir/test.net.xml", "../test.sumocfg", false, "/home/user")) << "config in parent dir makes path different.";
    EXPECT_EQ("user/subdir/test.net.xml", FileHelpers::fixRelative("subdir/test.net.xml", "../test.sumocfg", true, "/home/user")) << "config in parent dir makes path different.";
    EXPECT_EQ("/home/user/subdir/test.net.xml", FileHelpers::fixRelative("/home/user/subdir/test.net.xml", "../test.sumocfg", false, "/home/user")) << "config in parent dir does not change abs path without force.";
    EXPECT_EQ("user/subdir/test.net.xml", FileHelpers::fixRelative("/home/user/subdir/test.net.xml", "../test.sumocfg", true, "/home/user")) << "config in parent dir makes path different.";
    EXPECT_EQ("../../test.net.xml", FileHelpers::fixRelative("../test.net.xml", "subdir/test.sumocfg", false, "/home/user")) << "file in parent dir makes path different.";
    EXPECT_EQ("../../test.net.xml", FileHelpers::fixRelative("../test.net.xml", "subdir/test.sumocfg", true, "/home/user")) << "file in parent dir makes path different.";
    EXPECT_EQ("../../test.net.xml", FileHelpers::fixRelative("../test.net.xml", "/home/user/subdir/test.sumocfg", false, "/home/user")) << "file in parent dir makes path different.";
    EXPECT_EQ("../../test.net.xml", FileHelpers::fixRelative("../test.net.xml", "/home/user/subdir/test.sumocfg", true, "/home/user")) << "file in parent dir makes path different.";
    EXPECT_EQ("../../test.net.xml", FileHelpers::fixRelative("../../test.net.xml", "test.sumocfg", false, "/home/user")) << "file in parent dir with straight cfg should stay unchanged.";
    EXPECT_EQ("../../test.net.xml", FileHelpers::fixRelative("../../test.net.xml", "test.sumocfg", true, "/home/user")) << "file in parent dir with straight cfg should stay unchanged.";
    EXPECT_EQ("stdout", FileHelpers::fixRelative("stdout", "/home/sampledir", true, "/home")) << "check for reserved filenames.";
}


