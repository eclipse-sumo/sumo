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

#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>

#include <thread>
#include <chrono>

#include "GNEApplicationWindow.h"
#include "GNETestSystem.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNETestSystem::GNETestSystem(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


GNETestSystem::~GNETestSystem() {}


void
GNETestSystem::runTest() {
    // first process test file
    processTestFile();
    // execute every operation
    for (const auto &testStep : myTestSteps) {
        // continue depending of step type
        switch (testStep.getStepType()) {
            // supermodes
            case TestStepType::SUPERMODE_NETWORK:
                myApplicationWindow->onCmdSetSuperMode(myApplicationWindow, MID_HOTKEY_F2_SUPERMODE_NETWORK, nullptr);
                break;
            case TestStepType::SUPERMODE_DEMAND:
                myApplicationWindow->onCmdSetSuperMode(myApplicationWindow, MID_HOTKEY_F3_SUPERMODE_DEMAND, nullptr);
                break;
            case TestStepType::SUPERMODE_DATA:
                myApplicationWindow->onCmdSetSuperMode(myApplicationWindow, MID_HOTKEY_F4_SUPERMODE_DATA, nullptr);
                break;
            // network mode
            case TestStepType::NETWORKMODE_INSPECT:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_I_MODE_INSPECT, nullptr);
                break;
            case TestStepType::NETWORKMODE_DELETE:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_D_MODE_SINGLESIMULATIONSTEP_DELETE, nullptr);
                break;
            case TestStepType::NETWORKMODE_SELECT:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_S_MODE_STOPSIMULATION_SELECT, nullptr);
                break;
            case TestStepType::NETWORKMODE_MOVE:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_M_MODE_MOVE_MEANDATA, nullptr);
                break;
            case TestStepType::NETWORKMODE_EDGE:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_E_MODE_EDGE_EDGEDATA, nullptr);
                break;
            case TestStepType::NETWORKMODE_TRAFFICLIGHT:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_T_MODE_TLS_TYPE, nullptr);
                break;
            case TestStepType::NETWORKMODE_CONNECTION:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_C_MODE_CONNECT_CONTAINER, nullptr);
                break;
            case TestStepType::NETWORKMODE_PROHIBITION:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN, nullptr);
                break;
            case TestStepType::NETWORKMODE_CROSSING:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA, nullptr);
                break;
            case TestStepType::NETWORKMODE_ADDITIONAL:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_A_MODE_STARTSIMULATION_ADDITIONALS_STOPS, nullptr);
                break;
            case TestStepType::NETWORKMODE_WIRE:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_W_MODE_WIRE_ROUTEDISTRIBUTION, nullptr);
                break;
            case TestStepType::NETWORKMODE_TAZ:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_Z_MODE_TAZ_TAZREL, nullptr);
                break;
            case TestStepType::NETWORKMODE_SHAPE:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_P_MODE_POLYGON_PERSON, nullptr);
                break;
            case TestStepType::NETWORKMODE_DECAL:
                myApplicationWindow->onCmdSetMode(myApplicationWindow, MID_HOTKEY_U_MODE_DECAL_TYPEDISTRIBUTION, nullptr);
                break;
            // other
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
    }
}


void
GNETestSystem::processTestFile() {
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


GNETestSystem::TestStep::TestStep(const std::string &row) {
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
    } else if (myFunction == "networkMode") {
        if (myArguments.size() == 1) {
            if (myArguments[0] == "inspect") {
                myStepType = TestStepType::NETWORKMODE_INSPECT;
            } else if (myArguments[0] == "delete") {
                myStepType = TestStepType::NETWORKMODE_DELETE;
            } else if (myArguments[0] == "select") {
                myStepType = TestStepType::NETWORKMODE_SELECT;
            } else if (myArguments[0] == "move") {
                myStepType = TestStepType::NETWORKMODE_MOVE;
            } else if (myArguments[0] == "edge") {
                myStepType = TestStepType::NETWORKMODE_EDGE;
            } else if (myArguments[0] == "trafficLight") {
                myStepType = TestStepType::NETWORKMODE_TRAFFICLIGHT;
            } else if (myArguments[0] == "connection") {
                myStepType = TestStepType::NETWORKMODE_CONNECTION;
            } else if (myArguments[0] == "prohibition") {
                myStepType = TestStepType::NETWORKMODE_PROHIBITION;
            } else if (myArguments[0] == "crossing") {
                myStepType = TestStepType::NETWORKMODE_CROSSING;
            } else if (myArguments[0] == "additional") {
                myStepType = TestStepType::NETWORKMODE_ADDITIONAL;
            } else if (myArguments[0] == "wire") {
                myStepType = TestStepType::NETWORKMODE_WIRE;
            } else if (myArguments[0] == "taz") {
                myStepType = TestStepType::NETWORKMODE_TAZ;
            } else if (myArguments[0] == "shape") {
                myStepType = TestStepType::NETWORKMODE_SHAPE;
            } else if (myArguments[0] == "decal") {
                myStepType = TestStepType::NETWORKMODE_DECAL;
            } else {
                throw ProcessError("Invalid network mode");
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

GNETestSystem::TestStepType
GNETestSystem::TestStep::getStepType() const {
    return myStepType;
}


void
GNETestSystem::TestStep::parseFunctionAndArguments(const std::string &row) {
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