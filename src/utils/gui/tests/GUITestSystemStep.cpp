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

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>

#include "GUITestSystemStep.h"
#include "GUITestSystem.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GUITestSystemStep::GUITestSystemStep(GUITestSystem* testSystem, const std::string &rowText) {
    // first split between functions and arguments
    const auto function = parseStep(rowText);
    // continue depending of function
    if (function == "click") {
        myCategory = Category::VIRTUAL;
        if (myArguments.size() == 2) {
            if (!StringUtils::isInt(myArguments[0])) {
                throw ProcessError("First click position cannot be parsed to int");
            } else if (!StringUtils::isInt(myArguments[1])) {
                throw ProcessError("Second click position cannot be parsed to int");
            } else {
                const int posX = StringUtils::toInt(myArguments[0]);
                const int posY = StringUtils::toInt(myArguments[1]);
                // add move, left button press and left button release
                testSystem->addTestStep(new GUITestSystemStep(SEL_MOTION, 0, Category::VIEW, buildMouseMoveEvent(posX, posY)));
                testSystem->addTestStep(new GUITestSystemStep(SEL_LEFTBUTTONPRESS, 0, Category::VIEW, buildMouseLeftClickPressEvent(posX, posY)));
                testSystem->addTestStep(new GUITestSystemStep(SEL_LEFTBUTTONRELEASE, 0, Category::VIEW, buildMouseLeftClickReleaseEvent(posX, posY)));
            }
        } else {
            throw ProcessError("Invalid number of arguments for function " + function);
        }
    } else if (function == "supermode") {
        myCategory = Category::MAINWINDOW;
        if (myArguments.size() == 1) {
            if (myArguments[0] == "network") {
                myMessageID = MID_HOTKEY_F2_SUPERMODE_NETWORK;
            } else if (myArguments[0] == "demand") {
                myMessageID = MID_HOTKEY_F3_SUPERMODE_DEMAND;
            } else if (myArguments[0] == "data") {
                myMessageID = MID_HOTKEY_F4_SUPERMODE_DATA;
            } else {
                throw ProcessError("Invalid supermode");
            }
        } else {
            throw ProcessError("Invalid number of arguments for function " + function);
        }
    // network modes
    } else if (function == "networkMode") {
        myCategory = Category::MAINWINDOW;
        if (myArguments.size() == 1) {
            if (myArguments[0] == "inspect") {
                myMessageID = MID_HOTKEY_I_MODE_INSPECT;
            } else if (myArguments[0] == "delete") {
                myMessageID = MID_HOTKEY_D_MODE_SINGLESIMULATIONSTEP_DELETE;
            } else if (myArguments[0] == "select") {
                myMessageID = MID_HOTKEY_S_MODE_STOPSIMULATION_SELECT;
            } else if (myArguments[0] == "move") {
                myMessageID = MID_HOTKEY_M_MODE_MOVE_MEANDATA;
            } else if (myArguments[0] == "edge") {
                myMessageID = MID_HOTKEY_E_MODE_EDGE_EDGEDATA;
            } else if (myArguments[0] == "trafficLight") {
                myMessageID = MID_HOTKEY_T_MODE_TLS_TYPE;
            } else if (myArguments[0] == "connection") {
                myMessageID = MID_HOTKEY_C_MODE_CONNECT_CONTAINER;
            } else if (myArguments[0] == "prohibition") {
                myMessageID = MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN;
            } else if (myArguments[0] == "crossing") {
                myMessageID = MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA;
            } else if (myArguments[0] == "additional") {
                myMessageID = MID_HOTKEY_A_MODE_STARTSIMULATION_ADDITIONALS_STOPS;
            } else if (myArguments[0] == "wire") {
                myMessageID = MID_HOTKEY_W_MODE_WIRE_ROUTEDISTRIBUTION;
            } else if (myArguments[0] == "taz") {
                myMessageID = MID_HOTKEY_Z_MODE_TAZ_TAZREL;
            } else if (myArguments[0] == "shape") {
                myMessageID = MID_HOTKEY_P_MODE_POLYGON_PERSON;
            } else if (myArguments[0] == "decal") {
                myMessageID = MID_HOTKEY_U_MODE_DECAL_TYPEDISTRIBUTION;
            } else {
                throw ProcessError("Invalid network mode");
            }
        } else {
            throw ProcessError("Invalid number of arguments for function " + function);
        }
    // select additional
    } else if (function == "selectAdditional") {
        myCategory = Category::FRAME_ADDITIONAL_TAGSELECTOR;
        if (myArguments.size() == 1) {
            myMessageID = MID_GNE_TAG_SELECTED;
            myText = new FXString(myArguments[0].c_str());
        } else {
            throw ProcessError("Invalid number of arguments for function " + function);
        }
    // other
    } else if (function == "processing") {
        myCategory = Category::MAINWINDOW;
        if (myArguments.empty()) {
            myMessageID = MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND;
        } else {
            throw ProcessError("Invalid number of arguments for function " + function);
        }
    } else if (function == "save") {
        myCategory = Category::MAINWINDOW;
        if (myArguments.size() == 1) {
            if (myArguments.front() == "neteditConfig") {
                myMessageID = MID_HOTKEY_CTRL_SHIFT_E_SAVENETEDITCONFIG;
            } else {
                throw ProcessError("Invalid number of arguments for function " + function);
            }
        } else {
            throw ProcessError("Invalid number of arguments for function " + function);
        }
    } else if (function == "quit") {
        myCategory = Category::MAINWINDOW;
        if (myArguments.empty()) {
            myMessageID = MID_HOTKEY_CTRL_Q_CLOSE;
        } else {
            throw ProcessError("Invalid number of arguments for function " + function);
        }
    }
}


GUITestSystemStep::GUITestSystemStep(FXSelector messageType, FXSelector messageID, Category category,
        const std::vector<std::string> &arguments, FXString* text, FXEvent* event) :
    myMessageType(messageType),
    myMessageID(messageID),
    myCategory(category),
    myArguments(arguments),
    myText(text),
    myEvent(event) {
}


GUITestSystemStep::GUITestSystemStep(FXSelector messageType, FXSelector messageID, Category category,
        FXEvent* event) :
    myMessageType(messageType),
    myMessageID(messageID),
    myCategory(category),
    myEvent(event) {
}


GUITestSystemStep::~GUITestSystemStep() {
    if (myEvent) {
        delete myEvent;
    }
    if (myText) {
        delete myText;
    }
}


FXSelector
GUITestSystemStep::getMessageType() const {
    return myMessageType;
}
        

FXSelector
GUITestSystemStep::getMessageID() const {
    return myMessageID;
}


FXSelector
GUITestSystemStep::getSelector() const {
    return FXSEL(myMessageType, myMessageID);
}


GUITestSystemStep::Category
GUITestSystemStep::getCategory() const {
    return myCategory;
}


FXString* 
GUITestSystemStep::getText() const {
    return myText;
}


void*
GUITestSystemStep::getEvent() const {
    return myEvent;
}


FXEvent*
GUITestSystemStep::buildMouseMoveEvent(const int posX, const int posY) const {
    FXEvent* moveEvent = new FXEvent();
    // set event values
    moveEvent->type = 9;
    moveEvent->win_x = posX;
    moveEvent->win_y = posY;
    moveEvent->moved = true;
    moveEvent->rect = FXRectangle(0, 0, 0, 0);
    moveEvent->synthetic = true;
    return moveEvent;
}


FXEvent*
GUITestSystemStep::buildMouseLeftClickPressEvent(const int posX, const int posY) const {
    FXEvent* leftClickPressEvent = new FXEvent();
    // set event values
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
GUITestSystemStep::buildMouseLeftClickReleaseEvent(const int posX, const int posY) const {
    FXEvent* leftClickPressEvent = new FXEvent();
    // set event values
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


std::string
GUITestSystemStep::parseStep(const std::string &rowText) {
    std::string functionName;
    // make a copy to help editing row
    std::string editedRow = rowText;
    // every function has the format <function>(<argument1>, <argument2>,....,)
    if ((rowText.size() < 3) || (rowText.front() == '(') || (rowText.back() != ')')) {
        throw ProcessError("Invalid testStep row '" + rowText + "' check function(arguments) format");
    }
    // first extract function
    while (editedRow.size() > 0) {
        if (editedRow.front() == '(') {
            break;
        } else {
            functionName.push_back(editedRow.front());
            editedRow.erase(editedRow.begin());
        }
    }
    // check format
    if (editedRow.size() < 2) {
        throw ProcessError("Invalid testStep row '" + rowText + "'. Check <function>(<arguments>) format");
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
            throw ProcessError("Invalid testStep row '" + rowText + "'. Check \" in arguments ");
        }
    }
    return functionName;
}

/****************************************************************************/