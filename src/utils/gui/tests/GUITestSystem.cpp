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
/// @file    GUITestSystem.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Thread used for testing netedit
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/frames/GNETagSelector.h>
#include <netedit/frames/network/GNEAdditionalFrame.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>

#include <thread>
#include <chrono>

#include "GUITestSystem.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GUITestSystem::GUITestSystem() {}


GUITestSystem::~GUITestSystem() {
    for (auto testStep : myTestSteps) {
        delete testStep;
    }
}


void
GUITestSystem::startTests(GNEApplicationWindow* neteditApplicationWindow) {
    // run rest only once
    if (myInitedTest == false) {
        myNeteditApplicationWindow = neteditApplicationWindow;
        // check if run test thread
        if (OptionsCont::getOptions().getString("test-file").size() > 0) {
            processTestFile();
        }
        // start thread
        start();
        myInitedTest = true;
    }
}


void
GUITestSystem::checkLock(FXObject* sender, FXSelector sel) {
    if (sender == this) {
        std::cout << "lock" << std::endl;
        myContinue = false;
    }
}


void
GUITestSystem::checkUnlock(FXObject* sender, FXSelector sel) {
    if (sender == this) {
        std::cout << "unlock" << std::endl;
        myContinue = true;
    }
}


int
GUITestSystem::run() {
    for (const auto &testStep : myTestSteps) {
        // continue depending of step type
        switch (testStep->getStepType()) {
            // basic
            case TestStepType::CLICK:
                myNeteditApplicationWindow->getViewNet()->handle(this, FXSEL(SEL_MOTION, 0), (void*)testStep->getEvents().at(0));
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                myNeteditApplicationWindow->getViewNet()->handle(this, FXSEL(SEL_LEFTBUTTONPRESS, 0), (void*)testStep->getEvents().at(1));
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                myNeteditApplicationWindow->getViewNet()->handle(this, FXSEL(SEL_LEFTBUTTONRELEASE, 0), (void*)testStep->getEvents().at(2));
                break;
            // supermodes
            case TestStepType::SUPERMODE_NETWORK:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_F2_SUPERMODE_NETWORK), nullptr);
                break;
            case TestStepType::SUPERMODE_DEMAND:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_F3_SUPERMODE_DEMAND), nullptr);
                break;
            case TestStepType::SUPERMODE_DATA:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_F4_SUPERMODE_DATA), nullptr);
                break;
            // network mode
            case TestStepType::NETWORKMODE_INSPECT:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_I_MODE_INSPECT), nullptr);
                break;
            case TestStepType::NETWORKMODE_DELETE:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_D_MODE_SINGLESIMULATIONSTEP_DELETE), nullptr);
                break;
            case TestStepType::NETWORKMODE_SELECT:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_S_MODE_STOPSIMULATION_SELECT), nullptr);
                break;
            case TestStepType::NETWORKMODE_MOVE:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_M_MODE_MOVE_MEANDATA), nullptr);
                break;
            case TestStepType::NETWORKMODE_EDGE:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_E_MODE_EDGE_EDGEDATA), nullptr);
                break;
            case TestStepType::NETWORKMODE_TRAFFICLIGHT:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_T_MODE_TLS_TYPE), nullptr);
                break;
            case TestStepType::NETWORKMODE_CONNECTION:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_C_MODE_CONNECT_CONTAINER), nullptr);
                break;
            case TestStepType::NETWORKMODE_PROHIBITION:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN), nullptr);
                break;
            case TestStepType::NETWORKMODE_CROSSING:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA), nullptr);
                break;
            case TestStepType::NETWORKMODE_ADDITIONAL:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_A_MODE_STARTSIMULATION_ADDITIONALS_STOPS), nullptr);
                break;
            case TestStepType::NETWORKMODE_WIRE:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_W_MODE_WIRE_ROUTEDISTRIBUTION), nullptr);
                break;
            case TestStepType::NETWORKMODE_TAZ:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_Z_MODE_TAZ_TAZREL), nullptr);
                break;
            case TestStepType::NETWORKMODE_SHAPE:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_P_MODE_POLYGON_PERSON), nullptr);
                break;
            case TestStepType::NETWORKMODE_DECAL:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_U_MODE_DECAL_TYPEDISTRIBUTION), nullptr);
                break;
            // set additional
            case TestStepType::SELECT_ADDITIONAL:
                myNeteditApplicationWindow->getViewNet()->getViewParent()->getAdditionalFrame()->getAdditionalTagSelector()->handle(this, FXSEL(SEL_COMMAND, MID_GNE_TAG_SELECTED), (void*)testStep->getText());
                break;
            // other
            case TestStepType::PROCESSING:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND), nullptr);
                break;
            case TestStepType::SAVE_NETEDITCONFIG:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_SHIFT_E_SAVENETEDITCONFIG), nullptr);
                break;
            case TestStepType::QUIT:
                myNeteditApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_Q_CLOSE), nullptr);
                break;
            default:
                break;
        }
        while (!myContinue) {
            FXThread::sleep(10);  
        }
    }
    return 1;
}


void
GUITestSystem::processTestFile() {
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
                myTestSteps.push_back(new TestStep(line));
            }
        }
    }
}


GUITestSystem::TestStep::TestStep(const std::string &row) {
    // first split between functions and arguments
    parseFunctionAndArguments(row);
    // continue depending of function
    if (myFunction == "click") {
        if (myArguments.size() == 2) {
            if (!GNEAttributeCarrier::canParse<int>(myArguments[0])) {
                throw ProcessError("First click cannot be parsed to int");
            } else if (!GNEAttributeCarrier::canParse<int>(myArguments[1])) {
                throw ProcessError("Second click cannot be parsed to int");
            } else {
                const int posX = GNEAttributeCarrier::parse<int>(myArguments[0]);
                const int posY = GNEAttributeCarrier::parse<int>(myArguments[1]);
                // set event of moving, click presss and click release
                myEvents.push_back(buildMouseMoveEvent(posX, posY));
                myEvents.push_back(buildMouseLeftClickPressEvent(posX, posY));
                myEvents.push_back(buildMouseLeftClickReleaseEvent(posX, posY));
            }
            myStepType = TestStepType::CLICK;
        } else {
            throw ProcessError("Invalid number of arguments for function " + myFunction);
        }
    } else if (myFunction == "supermode") {
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
    // network modes
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
    // select additional
    } else if (myFunction == "selectAdditional") {
        if (myArguments.size() == 1) {
            myStepType = TestStepType::SELECT_ADDITIONAL;
            myText = new FXString(myArguments[0].c_str());
        } else {
            throw ProcessError("Invalid number of arguments for function " + myFunction);
        }
    // other
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


GUITestSystem::TestStep::~TestStep() {
    for (auto event : myEvents) {
        delete event;
    }
    if (myText) {
        delete myText;
    }
}


GUITestSystem::TestStepType
GUITestSystem::TestStep::getStepType() const {
    return myStepType;
}


FXString* 
GUITestSystem::TestStep::getText() const {
    return myText;
}


const std::vector<FXEvent*>&
GUITestSystem::TestStep::getEvents() const {
    return myEvents;
}

/*
    moveEvent->type = 9;
    moveEvent->time = 0;
    moveEvent->win_x = posX;
    moveEvent->win_y = posY;
    moveEvent->root_x = 0;
    moveEvent->root_y = 0;
    moveEvent->state = 0;
    moveEvent->code = 0;
    moveEvent->text = "";
    moveEvent->last_x = 0;
    moveEvent->last_y = 0;
    moveEvent->click_x = 0;
    moveEvent->click_y = 0;
    moveEvent->rootclick_x = 0;
    moveEvent->rootclick_y = 0;
    moveEvent->click_time = 0;
    moveEvent->click_button = 0;
    moveEvent->click_count = 0;
    moveEvent->moved = true;
    moveEvent->rect = FXRectangle(0, 0, 0, 0);
    moveEvent->synthetic = true;
    moveEvent->target = 0;
*/


FXEvent*
GUITestSystem::TestStep::buildMouseMoveEvent(const int posX, const int posY) const {
    FXEvent* moveEvent = new FXEvent();
    moveEvent->type = 9;
    moveEvent->win_x = posX;
    moveEvent->win_y = posY;
    moveEvent->moved = true;
    moveEvent->rect = FXRectangle(0, 0, 0, 0);
    moveEvent->synthetic = true;
    return moveEvent;
}


FXEvent*
GUITestSystem::TestStep::buildMouseLeftClickPressEvent(const int posX, const int posY) const {
    FXEvent* leftClickPressEvent = new FXEvent();
    // set fxevent
    leftClickPressEvent->win_x = posX;
    leftClickPressEvent->win_y = posY;
    leftClickPressEvent->click_x = posX;
    leftClickPressEvent->click_y = posY;
    leftClickPressEvent->type = 3;
    leftClickPressEvent->state = 256;
    leftClickPressEvent->code = 1;
    leftClickPressEvent->click_button = 1;
    leftClickPressEvent->click_count = 1;
    leftClickPressEvent->moved = false;
    leftClickPressEvent->synthetic = true;
    return leftClickPressEvent;
}


FXEvent*
GUITestSystem::TestStep::buildMouseLeftClickReleaseEvent(const int posX, const int posY) const {
    FXEvent* leftClickPressEvent = new FXEvent();
    // set fxevent
    leftClickPressEvent->win_x = posX;
    leftClickPressEvent->win_y = posY;
    leftClickPressEvent->click_x = posX;
    leftClickPressEvent->click_y = posY;
    leftClickPressEvent->type = 4;
    leftClickPressEvent->state = 256;
    leftClickPressEvent->code = 1;
    leftClickPressEvent->click_button = 1;
    leftClickPressEvent->click_count = 1;
    leftClickPressEvent->moved = false;
    leftClickPressEvent->synthetic = true;
    return leftClickPressEvent;
}


void
GUITestSystem::TestStep::parseFunctionAndArguments(const std::string &row) {
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