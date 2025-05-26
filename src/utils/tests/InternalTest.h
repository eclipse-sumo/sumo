/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    InternalTest.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Class used for internal tests
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>

#include <utils/foxtools/fxheader.h>

// ===========================================================================
// class declaration
// ===========================================================================

class InternalTestStep;

// ===========================================================================
// class definitions
// ===========================================================================

class InternalTest : public FXObject {

    /// @brief declare InternalTestStep as friend
    friend class InternalTestStep;

public:
    /// @brief constructor
    InternalTest(const std::string& testFile);

    /// @brief destructor
    ~InternalTest();

    /// @brief check if test are finised
    bool isTestFinished() const;

protected:
    /// @brief vector with attributesEnum jump steps
    std::map<std::string, int> myAttributesEnum;

    /// @brief vector with contextual menu operation jump steps
    std::map<std::string, int> myContextualMenuOperations;

    /// @brief vector with view position pairs
    std::map<std::string, std::pair<int, int> > myViewPositions;

    /// @brief test steps
    std::vector<const InternalTestStep*> myTestSteps;

    /// @brief flag to check if test are started
    bool myTestStarted = false;

    /// @brief flag to check if test are finished
    bool myTestFinished = false;

    /// @brief parse test data int file
    std::map<std::string, int> parseIntTestDataFile(const std::string filePath) const;

    /// @brief parse test data position file
    std::map<std::string, std::pair<int, int> > parsePositionTestDataFile(const std::string filePath) const;

    /// @brief invalidate default constructor
    InternalTest() = delete;

    /// @brief Invalidated copy constructor.
    InternalTest(const InternalTest&) = delete;

    /// @brief Invalidated assignment operator
    InternalTest& operator=(const InternalTest& src) = delete;
};
