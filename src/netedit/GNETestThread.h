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
/// @file    GNETestThread.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Thread used for testing netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXSingleEventThread.h>
#include <utils/foxtools/MFXSynchQue.h>
#include <utils/foxtools/MFXInterThreadEventClient.h>


// ===========================================================================
// class declarations
// ===========================================================================

class GNENet;
class GUIEvent;
class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================

class GNETestThread : protected MFXSingleEventThread {

public:
    /// @brief constructor
    GNETestThread(GNEApplicationWindow* applicationWindow);

    /// @brief destructor
    virtual ~GNETestThread();

    /// @brief starts the thread. The thread ends after the net has been loaded
    FXint run();

    /// @brief run test
    void startTest();

protected:
    /// @brief process test file
    void processTestFile();

private:
    enum class TestStepType {
        // go to supermode
        SUPERMODE_NETWORK,
        SUPERMODE_DEMAND,
        SUPERMODE_DATA,

        //other
        QUIT
    };

    /// @brief test step
    struct TestStep {
        /// @brief parameter constructor
        TestStep(const std::string &row);

        /// @brief return step type
        TestStepType getStepType() const;

    private:
        /// @brief step type
        TestStepType myStepType;
        
        /// @brief function
        std::string myFunction;

        /// @brief arguments
        std::vector<std::string> myArguments;

        /// @brief parse function and arguments
        void parseFunctionAndArguments(const std::string &row);

        /// @brief invalidate default constructor
        TestStep() = delete;
    };

    /// @brief netedit application windows
    GNEApplicationWindow* myApplicationWindow;

    /// @brief test steps
    std::vector<TestStep> myTestStep;
};
