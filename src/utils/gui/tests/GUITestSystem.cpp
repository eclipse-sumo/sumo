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

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/options/OptionsCont.h>
#include <fstream>

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
GUITestSystem::startTests(GUISUMOAbstractView* view, GUIMainWindow* mainWindow) {
    // run rest only once
    if (myInitedTest == false) {
        myInitedTest = true;
        // set common abstract view
        myAbstractView = view;
        // set specific parameter in children
        setSpecificMainWindow(mainWindow);
        // check if run test thread
        if (OptionsCont::getOptions().getString("test-file").size() > 0) {
            processTestFile();
        }
        // start thread if we have more than one test
        if (myTestSteps.size() > 0) {
            start();
        }
    }
}


void
GUITestSystem::nextTest(FXObject* sender, FXSelector sel) {
    // only continue if the signal was send by the test system
    if ((sender == this) && (sel == myCurrentSelector)) {
        myContinue = true;
    }
}


void
GUITestSystem::writeSignalInfo(FXObject* sender, FXSelector sel) const {
    // ignore update
    if (FXSELTYPE(sel) == SEL_UPDATE) {
        return;
    }
    std::string signalType;
    switch (FXSELTYPE(sel)) {
        case SEL_KEYPRESS:
            signalType = "Key pressed";
            break;
        case SEL_KEYRELEASE:
            signalType = "Key released";
            break;
        case SEL_LEFTBUTTONPRESS:
            signalType = "Left mouse button pressed";
            break;
        case SEL_LEFTBUTTONRELEASE:
            signalType = "Left mouse button released";
            break;
        case SEL_MIDDLEBUTTONPRESS:
            signalType = "Middle mouse button pressed";
            break;
        case SEL_MIDDLEBUTTONRELEASE:
            signalType = "Middle mouse button released";
            break;
        case SEL_RIGHTBUTTONPRESS:
            signalType = "Right mouse button pressed";
            break;
        case SEL_RIGHTBUTTONRELEASE:
            signalType = "Right mouse button released";
            break;
        case SEL_MOTION:
            signalType = "Mouse motion";
            break;
        case SEL_ENTER:
            signalType = "Mouse entered window";
            break;
        case SEL_LEAVE:
            signalType = "Mouse left window";
            break;
        case SEL_FOCUSIN:
            signalType = "Focus into window";
            break;
        case SEL_FOCUSOUT:
            signalType = "Focus out of window";
            break;
        case SEL_KEYMAP:
            signalType = "Key map";
            break;
        case SEL_UNGRABBED:
            signalType = "Lost the grab (Windows)";
            break;
        case SEL_PAINT:
            signalType = "Must repaint window";
            break;
        case SEL_CREATE:
            signalType = "Create";
            break;
        case SEL_DESTROY:
            signalType = "Destroy";
            break;
        case SEL_UNMAP:
            signalType = "Window was hidden";
            break;
        case SEL_MAP:
            signalType = "Window was shown";
            break;
        case SEL_CONFIGURE:
            signalType = "Configure (Resize)";
            break;
        case SEL_SELECTION_LOST:
            signalType = "Widget lost selection";
            break;
        case SEL_SELECTION_GAINED:
            signalType = "Widget gained selection";
            break;
        case SEL_SELECTION_REQUEST:
            signalType = "Inquire selection data";
            break;
        case SEL_RAISED:
            signalType = "Window to top of stack";
            break;
        case SEL_LOWERED:
            signalType = "Window to bottom of stack";
            break;
        case SEL_CLOSE:
            signalType = "Close window";
            break;
        case SEL_DELETE:
            signalType = "Delete window";
            break;
        case SEL_MINIMIZE:
            signalType = "Iconified";
            break;
        case SEL_RESTORE:
            signalType = "No longer iconified or maximized";
            break;
        case SEL_MAXIMIZE:
            signalType = "Maximized";
            break;
        case SEL_UPDATE:
            signalType = "GUI update";
            break;
        case SEL_COMMAND:
            signalType = "GUI command";
            break;
        case SEL_CLICKED:
            signalType = "Clicked";
            break;
        case SEL_DOUBLECLICKED:
            signalType = "Double-clicked";
            break;
        case SEL_TRIPLECLICKED:
            signalType = "Triple-clicked";
            break;
        case SEL_MOUSEWHEEL:
            signalType = "Mouse wheel";
            break;
        case SEL_CHANGED:
            signalType = "GUI has changed";
            break;
        case SEL_VERIFY:
            signalType = "Verify change";
            break;
        case SEL_DESELECTED:
            signalType = "Deselected";
            break;
        case SEL_SELECTED:
            signalType = "Selected";
            break;
        case SEL_INSERTED:
            signalType = "Inserted";
            break;
        case SEL_REPLACED:
            signalType = "Replaced";
            break;
        case SEL_DELETED:
            signalType = "Deleted";
            break;
        case SEL_OPENED:
            signalType = "Opened";
            break;
        case SEL_CLOSED:
            signalType = "Closed";
            break;
        case SEL_EXPANDED:
            signalType = "Expanded";
            break;
        case SEL_COLLAPSED:
            signalType = "Collapsed";
            break;
        case SEL_BEGINDRAG:
            signalType = "Start a drag";
            break;
        case SEL_ENDDRAG:
            signalType = "End a drag";
            break;
        case SEL_DRAGGED:
            signalType = "Dragged";
            break;
        case SEL_LASSOED:
            signalType = "Lassoed";
            break;
        case SEL_TIMEOUT:
            signalType = "Timeout occurred";
            break;
        case SEL_SIGNAL:
            signalType = "Signal received";
            break;
        case SEL_CLIPBOARD_LOST:
            signalType = "Widget lost clipboard";
            break;
        case SEL_CLIPBOARD_GAINED:
            signalType = "Widget gained clipboard";
            break;
        case SEL_CLIPBOARD_REQUEST:
            signalType = "Inquire clipboard data";
            break;
        case SEL_CHORE:
            signalType = "Background chore";
            break;
        case SEL_FOCUS_SELF:
            signalType = "Focus on widget itself";
            break;
        case SEL_FOCUS_RIGHT:
            signalType = "Focus moved right";
            break;
        case SEL_FOCUS_LEFT:
            signalType = "Focus moved left";
            break;
        case SEL_FOCUS_DOWN:
            signalType = "Focus moved down";
            break;
        case SEL_FOCUS_UP:
            signalType = "Focus moved up";
            break;
        case SEL_FOCUS_NEXT:
            signalType = "Focus moved to next widget";
            break;
        case SEL_FOCUS_PREV:
            signalType = "Focus moved to previous widget";
            break;
        case SEL_DND_ENTER:
            signalType = "Drag action entering potential drop target";
            break;
        case SEL_DND_LEAVE:
            signalType = "Drag action leaving potential drop target";
            break;
        case SEL_DND_DROP:
            signalType = "Drop on drop target";
            break;
        case SEL_DND_MOTION:
            signalType = "Drag position changed over potential drop target";
            break;
        case SEL_DND_REQUEST:
            signalType = "Inquire drag and drop data";
            break;
        case SEL_IO_READ:
            signalType = "Read activity on a pipe";
            break;
        case SEL_IO_WRITE:
            signalType = "Write activity on a pipe";
            break;
        case SEL_IO_EXCEPT:
            signalType = "Except activity on a pipe";
            break;
        case SEL_PICKED:
            signalType = "Picked some location";
            break;
        case SEL_QUERY_TIP:
            signalType = "Message inquiring about tooltip";
            break;
        case SEL_QUERY_HELP:
            signalType = "Message inquiring about statusline help";
            break;
        case SEL_DOCKED:
            signalType = "Toolbar docked";
            break;
        case SEL_FLOATED:
            signalType = "Toolbar floated";
            break;
        case SEL_SESSION_NOTIFY:
            signalType = "Session is about to close";
            break;
        case SEL_SESSION_CLOSED:
            signalType = "Session is closed";
            break;
        default:
            signalType = "Undefined";
            break;
    }
    std::cout << sender->getClassName() << " sent a signal of type '" << signalType << "'" << std::endl;
}


int
GUITestSystem::run() {
    for (const auto &testStep : myTestSteps) {
        // stop thread until nextTest() is called in FXIMPLEMENT_TESTING
        myContinue = false;
        myCurrentSelector = testStep->getSelector();
        // continue depending of step type
        if (testStep->getCategory() == "abstractView") {
            myAbstractView->handle(this, testStep->getSelector(), (void*)testStep->getEvent());
        } else {
            runSpecificTest(testStep);
        }
        // if this is the quit order, stop thread. In other case, wait until nextTest() is called
        if (testStep->getFunction() == "quit") {
            return 1;
        } else {
            waitForContinue();
        }
    }
    return 1;
}


GUITestSystem::TestStep::TestStep(GUITestSystem* testSystem, const std::string &row) {
    // first split between functions and arguments
    parseFunctionAndArguments(row);
    // continue depending of function
    if (myFunction == "click") {
        myCategory = "virtual";
        if (myArguments.size() == 2) {
            if (!StringUtils::isInt(myArguments[0])) {
                throw ProcessError("First click position cannot be parsed to int");
            } else if (!StringUtils::isInt(myArguments[1])) {
                throw ProcessError("Second click position cannot be parsed to int");
            } else {
                const int posX = StringUtils::toInt(myArguments[0]);
                const int posY = StringUtils::toInt(myArguments[1]);
                // we need to add three steps
                testSystem->myTestSteps.push_back(new TestStep(SEL_MOTION, 0, "abstractView", "move", {}, nullptr, buildMouseMoveEvent(posX, posY)));
                testSystem->myTestSteps.push_back(new TestStep(SEL_LEFTBUTTONPRESS, 0, "abstractView", "leftButtonPress", {}, nullptr, buildMouseLeftClickPressEvent(posX, posY)));
                testSystem->myTestSteps.push_back(new TestStep(SEL_LEFTBUTTONRELEASE, 0, "abstractView", "leftButtonRelease", {}, nullptr, buildMouseLeftClickReleaseEvent(posX, posY)));
            }
        } else {
            throw ProcessError("Invalid number of arguments for function " + myFunction);
        }
    } else if (myFunction == "supermode") {
        myCategory = "applicationWindow";
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
            throw ProcessError("Invalid number of arguments for function " + myFunction);
        }
    // network modes
    } else if (myFunction == "networkMode") {
        myCategory = "applicationWindow";
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
            throw ProcessError("Invalid number of arguments for function " + myFunction);
        }
    // select additional
    } else if (myFunction == "selectAdditional") {
        myCategory = "additionalFrame";
        if (myArguments.size() == 1) {
            myMessageID = MID_GNE_TAG_SELECTED;
            myText = new FXString(myArguments[0].c_str());
        } else {
            throw ProcessError("Invalid number of arguments for function " + myFunction);
        }
    // other
    } else if (myFunction == "processing") {
        myCategory = "applicationWindow";
        if (myArguments.empty()) {
            myMessageID = MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND;
        } else {
            throw ProcessError("Invalid number of arguments for function " + myFunction);
        }
    } else if (myFunction == "save") {
        myCategory = "applicationWindow";
        if (myArguments.size() == 1) {
            if (myArguments.front() == "neteditConfig") {
                myMessageID = MID_HOTKEY_CTRL_SHIFT_E_SAVENETEDITCONFIG;
            } else {
                throw ProcessError("Invalid number of arguments for function " + myFunction);
            }
        } else {
            throw ProcessError("Invalid number of arguments for function " + myFunction);
        }
    } else if (myFunction == "quit") {
        myCategory = "applicationWindow";
        if (myArguments.empty()) {
            myMessageID = MID_HOTKEY_CTRL_Q_CLOSE;
        } else {
            throw ProcessError("Invalid number of arguments for function " + myFunction);
        }
    }
}


GUITestSystem::TestStep::TestStep(FXSelector messageType, FXSelector messageID, const std::string &category,
    const std::string &function, const std::vector<std::string> &arguments, FXString* text, FXEvent* event) :
    myMessageType(messageType),
    myMessageID(messageID),
    myCategory(category),
    myFunction(function),
    myArguments(arguments),
    myText(text),
    myEvent(event) {
}


GUITestSystem::TestStep::~TestStep() {
    if (myEvent) {
        delete myEvent;
    }
    if (myText) {
        delete myText;
    }
}


FXSelector
GUITestSystem::TestStep::getMessageType() const {
    return myMessageType;
}
        

FXSelector
GUITestSystem::TestStep::getMessageID() const {
    return myMessageID;
}


FXSelector
GUITestSystem::TestStep::getSelector() const {
    return FXSEL(myMessageType, myMessageID);
}


const std::string&
GUITestSystem::TestStep::getCategory() const {
    return myCategory;
}


const std::string&
GUITestSystem::TestStep::getFunction() const {
    return myFunction;
}


FXString* 
GUITestSystem::TestStep::getText() const {
    return myText;
}


const FXEvent*
GUITestSystem::TestStep::getEvent() const {
    return myEvent;
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


void
GUITestSystem::waitForContinue() const {
    while (!myContinue) {
        FXThread::sleep(10);  
    }
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
                auto step = new TestStep(this, line);
                // only add to list if this is not a virtual attribute (for example, click)
                if (step->getCategory() != "virtual") {
                    myTestSteps.push_back(step);
                }
            }
        }
    }
}

/****************************************************************************/