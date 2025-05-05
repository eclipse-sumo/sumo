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
/// @file    GNETestSystem.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Thread used for testing netedit
/****************************************************************************/
#pragma once
#include <config.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================

class GNETestSystem {

public:
    /// @brief constructor
    GNETestSystem(GNEApplicationWindow* applicationWindow);

    /// @brief destructor
    ~GNETestSystem();

    /// @brief init test
    void initTests();

    /// @brief run all tests
    void runAllTests();

protected:
    /// @brief process test file
    void processTestFile();

private:
    enum class TestStepType {
        // basic
        CLICK,
        // go to supermode
        SUPERMODE_NETWORK,
        SUPERMODE_DEMAND,
        SUPERMODE_DATA,
        // network modes
        NETWORKMODE_INSPECT,
        NETWORKMODE_DELETE,
        NETWORKMODE_SELECT,
        NETWORKMODE_MOVE,
        NETWORKMODE_EDGE,
        NETWORKMODE_TRAFFICLIGHT,
        NETWORKMODE_CONNECTION,
        NETWORKMODE_PROHIBITION,
        NETWORKMODE_CROSSING,
        NETWORKMODE_ADDITIONAL,
        NETWORKMODE_WIRE,
        NETWORKMODE_TAZ,
        NETWORKMODE_SHAPE,
        NETWORKMODE_DECAL,
        // select elements in frames
        SELECT_ADDITIONAL,
        // processing
        PROCESSING,
        // saving
        SAVE_NETEDITCONFIG,
        //other
        QUIT
    };

    /// @brief test step
    struct TestStep {
        /// @brief parameter constructor
        TestStep(const std::string &row);

        /// @brief destructor
        ~TestStep();

        /// @brief return step type
        TestStepType getStepType() const;

        /// @brief get tag
        SumoXMLTag getTag() const;

        /// @brief get list of consecutive event
        const std::vector<FXEvent*> &getEvents() const;

    protected:
        /// @brief build mouse move event
        FXEvent* buildMouseMoveEvent(const int posX, const int posY) const;

        /// @brief build mouse left click press event
        FXEvent* buildMouseLeftClickPressEvent(const int posX, const int posY) const;

        /// @brief build mouse left click release event
        FXEvent* buildMouseLeftClickReleaseEvent(const int posX, const int posY) const;

    private:
        /// @brief step type
        TestStepType myStepType;
        
        /// @brief function
        std::string myFunction;

        /// @brief arguments
        std::vector<std::string> myArguments;

        /// @brief tag (used in certain tests)
        SumoXMLTag myTag = SUMO_TAG_NOTHING;

        /// @brief list of events associated with this step
        std::vector<FXEvent*> myEvents;

        /// @brief parse function and arguments
        void parseFunctionAndArguments(const std::string &row);

        /// @brief invalidate default constructor
        TestStep() = delete;
    };

    /// @brief netedit application windows
    GNEApplicationWindow* myApplicationWindow;

    /// @brief test steps
    std::vector<TestStep*> myTestSteps;

    /// @brief current step
    int myCurrentStep = 0;

    /// @brief inited
    bool myInitedTest = false;
};
