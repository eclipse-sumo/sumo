/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    OptionsContTest.cpp
/// @author  Mark Colley
/// @date    2026-11-05
///
/****************************************************************************/
#include <config.h>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>

TEST(OptionsContClone, IsolatesAliasesAndWritableState) {
    OptionsCont original;
    original.doRegister("name", 'n', new Option_String("default"));
    original.addSynonyme("name", "legacy-name");
    ASSERT_TRUE(original.set("name", "original"));
    ASSERT_FALSE(original.isWriteable("name"));

    std::unique_ptr<OptionsCont> clone(original.clone());
    EXPECT_FALSE(original.isWriteable("name"));
    EXPECT_TRUE(clone->isWriteable("name"));
    EXPECT_FALSE(clone->isDefault("name"));
    ASSERT_TRUE(clone->set("legacy-name", "changed"));
    EXPECT_EQ("changed", clone->getString("name"));
    EXPECT_EQ("changed", clone->getString("n"));
    EXPECT_EQ("changed", clone->begin()->second->getString());
    EXPECT_FALSE(clone->isWriteable("n"));
    EXPECT_EQ("original", original.getString("name"));
    EXPECT_EQ("original", original.getString("legacy-name"));
    EXPECT_FALSE(original.isWriteable("name"));
}

TEST(OptionsContClone, IsolatesDefaultsAndSerialization) {
    OptionsCont original;
    original.setApplicationName("testoptions", "testoptions");
    original.addOptionSubTopic("Input");
    original.doRegister("file", new Option_FileName({"default.xml"}));
    original.addDescription("file", "Input", "Input file");
    ASSERT_TRUE(original.set("file", "source.xml"));
    std::unique_ptr<OptionsCont> clone(original.clone());
    ASSERT_TRUE(clone->setDefault("file", "generated.xml"));
    EXPECT_TRUE(clone->isDefault("file"));
    EXPECT_FALSE(original.isDefault("file"));
    EXPECT_EQ("source.xml", original.getString("file"));
    clone->resetWritable();
    ASSERT_TRUE(clone->set("file", "clone.xml"));
    std::ostringstream sourceXML;
    std::ostringstream cloneXML;
    original.writeConfiguration(sourceXML, true, false, false, "", false, true);
    clone->writeConfiguration(cloneXML, true, false, false, "", false, true);
    EXPECT_NE(std::string::npos, sourceXML.str().find("source.xml"));
    EXPECT_EQ(std::string::npos, sourceXML.str().find("clone.xml"));
    EXPECT_NE(std::string::npos, cloneXML.str().find("clone.xml"));
    EXPECT_EQ(std::string::npos, cloneXML.str().find("source.xml"));
}

TEST(OptionsContClone, ClonesCanBeClonedIndependently) {
    OptionsCont original;
    original.doRegister("value", 'v', new Option_Integer(4));
    std::unique_ptr<OptionsCont> first(original.clone());
    ASSERT_TRUE(first->set("v", "8"));
    std::unique_ptr<OptionsCont> second(first->clone());
    ASSERT_TRUE(second->set("value", "12"));
    EXPECT_EQ(4, original.getInt("value"));
    EXPECT_EQ(8, first->getInt("value"));
    EXPECT_EQ(12, second->getInt("v"));
}

TEST(OptionsContClone, OutlivesSource) {
    std::unique_ptr<OptionsCont> clone;
    {
        OptionsCont original;
        original.doRegister("value", 'v', new Option_String("retained"));
        clone.reset(original.clone());
    }
    EXPECT_EQ("retained", clone->getString("v"));
    EXPECT_TRUE(clone->set("value", "updated"));
    EXPECT_EQ("updated", clone->getString("v"));
}
