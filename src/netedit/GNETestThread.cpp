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

#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>

#include <thread>
#include <chrono>

#include "GNEApplicationWindow.h"
#include "GNETestThread.h"
#include "GNELoadThread.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNETestThread::GNETestThread(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


GNETestThread::~GNETestThread() {}


FXint
GNETestThread::run() {
    // wait 1 sec
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // first process test file
    processTestFile();
    // execute every operation
    for (const auto &testStep : myTestSteps) {
        // continue depending of step type
        switch (testStep.getStepType()) {
            case TestStepType::SUPERMODE_NETWORK:
                myApplicationWindow->onCmdSetSuperMode(myApplicationWindow, MID_HOTKEY_F2_SUPERMODE_NETWORK, nullptr);
                break;
            case TestStepType::SUPERMODE_DEMAND:
                myApplicationWindow->onCmdSetSuperMode(myApplicationWindow, MID_HOTKEY_F3_SUPERMODE_DEMAND, nullptr);
                break;
            case TestStepType::SUPERMODE_DATA:
                myApplicationWindow->onCmdSetSuperMode(myApplicationWindow, MID_HOTKEY_F4_SUPERMODE_DATA, nullptr);
                break;
            case TestStepType::PROCESSING:
                myApplicationWindow->onCmdProcessButton(myApplicationWindow, MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND, nullptr);
                break;
            case TestStepType::SAVE_NETEDITCONFIG:
                myApplicationWindow->onCmdSaveNeteditConfig(myApplicationWindow, MID_HOTKEY_CTRL_SHIFT_E_SAVENETEDITCONFIG, nullptr);
                break;
            case TestStepType::QUIT:
                myApplicationWindow->onCmdQuit(myApplicationWindow, MID_HOTKEY_CTRL_Q_CLOSE, nullptr); 
                break;
            default:
                break;
        }
        // wait 1 sec
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    // execute every step
    return 0;
}


void
GNETestThread::startTest() {
    // start thread
    start();
}


void
GNETestThread::processTestFile() {
    const std::string testFile = OptionsCont::getOptions().getString("test-file");
    // open file
    std::ifstream strm(testFile);
    // check if file can be opened
    if (!strm.good()) {
        WRITE_ERRORF(TL("Could not open test file '%'."), testFile);
    } else {
        // continue while stream exist
        while (strm.good()) {
            std::string line;
            strm >> line;
            // check if line isn't empty
            if ((line.size() > 0) && line[0] != '#') {
                myTestSteps.push_back(TestStep(line));
            }
        }
    }
}


GNETestThread::TestStep::TestStep(const std::string &row) {
    // first split between functions and arguments
    parseFunctionAndArguments(row);
    // continue depending of function
    if (myFunction == "supermode") {
        if (myArguments.size() == 1) {
            if (myArguments[0] == "network") {
                myStepType = TestStepType::SUPERMODE_NETWORK;
            } else if (myArguments[0] == "demand") {
                myStepType = TestStepType::SUPERMODE_DEMAND;
            } else if (myArguments[0] == "data") {
                myStepType = TestStepType::SUPERMODE_DATA;
            } else {
                throw ProcessError("Invalid supermode");
            }
        } else {
            throw ProcessError("Invalid number of arguments for function " + myFunction);
        }
    } else if (myFunction == "processing") {
        if (myArguments.empty()) {
            myStepType = TestStepType::PROCESSING;
        } else {
            throw ProcessError("Invalid number of arguments for function " + myFunction);
        }
    } else if (myFunction == "save") {
        if (myArguments.size() == 1) {
            if (myArguments.front() == "neteditConfig") {
                myStepType = TestStepType::SAVE_NETEDITCONFIG;
            } else {
                throw ProcessError("Invalid number of arguments for function " + myFunction);
            }
        } else {
            throw ProcessError("Invalid number of arguments for function " + myFunction);
        }
    } else if (myFunction == "quit") {
        if (myArguments.empty()) {
            myStepType = TestStepType::QUIT;
        } else {
            throw ProcessError("Invalid number of arguments for function " + myFunction);
        }
    }
}

GNETestThread::TestStepType
GNETestThread::TestStep::getStepType() const {
    return myStepType;
}


void
GNETestThread::TestStep::parseFunctionAndArguments(const std::string &row) {
    // make a copy to help editing row
    std::string editedRow = row;
    // every function has the format <function>(<argument1>, <argument2>,....,)
    if ((row.size() < 3) || (row.front() == '(') || (row.back() != ')')) {
        throw ProcessError("Invalid testStep row '" + row + "' check function(arguments) format");
    }
    // first extract function
    while (editedRow.size() > 0) {
        if (editedRow.front() == '(') {
            break;
        } else {
            myFunction.push_back(editedRow.front());
            editedRow.erase(editedRow.begin());
        }
    }
    // check format
    if (editedRow.size() < 2) {
        throw ProcessError("Invalid testStep row '" + row + "'. Check <function>(<arguments>) format");
    }
    // remove both pharentersis
    editedRow.erase(editedRow.begin());
    editedRow.pop_back();
    // continue if there are arguments
    if (editedRow.size() > 0) {
        // now extract arguments
        bool inString = false;
        std::string argument;
        while (editedRow.size() > 0) {
            if ((editedRow.front() == ',') && !inString) {
                myArguments.push_back(argument);
                argument.clear();
            } else if ((editedRow.front() == '"')) {
                inString = !inString;
            } else {
                argument.push_back(editedRow.front());
            }
            editedRow.erase(editedRow.begin());
        }
        myArguments.push_back(argument);
        if (inString) {
            throw ProcessError("Invalid testStep row '" + row + "'. Check \" in arguments ");
        }
    }
}

/****************************************************************************/