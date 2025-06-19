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
/// @file    InternalTestStep.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Single operation used in InternalTests
/****************************************************************************/
#include <config.h>

#include <fxkeys.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>

#include "InternalTestStep.h"
#include "InternalTest.h"


// ===========================================================================
// static member definitions
// ===========================================================================

const std::string InternalTestStep::ModalArguments::colorValue = "139,131,120";
// this offsets corresponds to the offset of the test magenta square
constexpr int MOUSE_OFFSET_X = 24;
constexpr int MOUSE_OFFSET_Y = 25;
constexpr int MOUSE_REFERENCE_X = 304;
constexpr int MOUSE_REFERENCE_Y = 168;

// ===========================================================================
// member method definitions
// ===========================================================================

InternalTestStep::InternalTestStep(InternalTest* testSystem, const std::string& step) :
    myTestSystem(testSystem) {
    // add this testStep to test system
    testSystem->addTestSteps(this);
    // get overlapped tabs
    const int overlappedTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.editElements.overlapped");
    // parse step
    const auto function = parseStep(step);
    // continue depending of function
    if (function == "setupAndStart") {
        setupAndStart();
    } else if (function == "leftClick") {
        leftClick("");
    } else if (function == "leftClickControl") {
        leftClick("control");
    } else if (function == "leftClickShift") {
        leftClick("shift");
    } else if (function == "typeKey") {
        typeKey();
    } else if (function == "contextualMenuOperation") {
        contextualMenuOperation();
    } else if (function == "modifyAttribute") {
        modifyAttribute(0);
    } else if (function == "modifyAttributeOverlapped") {
        modifyAttribute(overlappedTabs);
    } else if (function == "modifyBoolAttribute") {
        modifyBoolAttribute(0);
    } else if (function == "modifyBoolAttributeOverlapped") {
        modifyBoolAttribute(overlappedTabs);
    } else if (function == "modifyColorAttribute") {
        modifyColorAttribute(0);
    } else if (function == "modifyColorAttributeOverlapped") {
        modifyColorAttribute(overlappedTabs);
    } else if (function == "modifyVClassDialog_NoDisallowAll") {
        modifyVClassDialog_NoDisallowAll(0);
    } else if (function == "modifyVClassDialogOverlapped_NoDisallowAll") {
        modifyVClassDialog_NoDisallowAll(overlappedTabs);
    } else if (function == "modifyVClassDialog_DisallowAll") {
        modifyVClassDialog_DisallowAll(0);
    } else if (function == "modifyVClassDialogOvelapped_DisallowAll") {
        modifyVClassDialog_DisallowAll(overlappedTabs);
    } else if (function == "modifyVClassDialog_Cancel") {
        modifyVClassDialog_Cancel(0);
    } else if (function == "modifyVClassDialogOverlapped_Cancel") {
        modifyVClassDialog_Cancel(overlappedTabs);
    } else if (function == "modifyVClassDialog_Reset") {
        modifyVClassDialog_Reset(0);
    } else if (function == "modifyVClassDialogOverlapped_Reset") {
        modifyVClassDialog_Reset(overlappedTabs);
    } else if (function == "changeEditMode") {
        changeEditMode();
    } else if (function == "changeSupermode") {
        changeSupermode();
    } else if (function == "changeMode") {
        changeMode();
    } else if (function == "changeElement") {
        changeElement();
    } else if (function == "changePlan") {
        changePlan();
    } else if (function == "computeJunctions") {
        computeJunctions();
    } else if (function == "computeJunctionsVolatileOptions") {
        computeJunctionsVolatileOptions();
    } else if (function == "saveExistentShortcut") {
        saveExistentShortcut();
    } else if (function == "checkUndoRedo") {
        checkUndoRedo();
    } else if (function == "delete") {
        deleteFunction();
    } else if (function == "selection") {
        selection();
    } else if (function == "undo") {
        undo();
    } else if (function == "redo") {
        redo();
    } else if (function == "quit") {
        quit();
    } else if (function.size() > 0) {
        std::cout << function << std::endl;
        throw ProcessError("Function " + function + " not implemented in InternalTestStep");
    }
}


InternalTestStep::InternalTestStep(InternalTest* testSystem, FXSelector messageType,
                                   FXSelector messageID, Category category) :
    myTestSystem(testSystem),
    myMessageType(messageType),
    myMessageID(messageID),
    myCategory(category) {
    // add this testStep to test system
    testSystem->addTestSteps(this);
}


InternalTestStep::InternalTestStep(InternalTest* testSystem, FXSelector messageType,
                                   Category category, FXEvent* event, const bool updateView) :
    myTestSystem(testSystem),
    myMessageType(messageType),
    myCategory(category),
    myUpdateView(updateView),
    myEvent(event) {
    // add this testStep to test system
    testSystem->addTestSteps(this);
}


InternalTestStep::InternalTestStep(InternalTestStep* parent, FXSelector messageType, FXEvent* event) :
    myTestSystem(parent->myTestSystem),
    myMessageType(messageType),
    myEvent(event) {
    // add this testStep to parent modal dialgo testSteps
    parent->myModalDialogTestSteps.push_back(this);
}


InternalTestStep::~InternalTestStep() {
    if (myEvent) {
        delete myEvent;
    }
    if (myModalArguments) {
        delete myModalArguments;
    }
    // remove all key steps
    for (auto modalDialogTestStep : myModalDialogTestSteps) {
        delete modalDialogTestStep;
    }
    myModalDialogTestSteps.clear();
}


FXSelector
InternalTestStep::getMessageType() const {
    return myMessageType;
}


FXSelector
InternalTestStep::getMessageID() const {
    return myMessageID;
}


InternalTestStep::ModalArguments*
InternalTestStep::getModalArguments() const {
    return myModalArguments;
}


FXSelector
InternalTestStep::getSelector() const {
    return FXSEL(myMessageType, myMessageID);
}


bool
InternalTestStep::updateView() const {
    return myUpdateView;
}


InternalTestStep::Category
InternalTestStep::getCategory() const {
    return myCategory;
}


void*
InternalTestStep::getEvent() const {
    return myEvent;
}


const std::vector<const InternalTestStep*>&
InternalTestStep::getModalDialogTestSteps() const {
    return myModalDialogTestSteps;
}


FXEvent*
InternalTestStep::buildMouseMoveEvent(const int posX, const int posY) const {
    FXEvent* moveEvent = new FXEvent();
    // common values
    moveEvent->synthetic = true;
    // set event values
    moveEvent->type = SEL_MOTION;
    moveEvent->win_x = posX + MOUSE_OFFSET_X;
    moveEvent->win_y = posY + MOUSE_OFFSET_Y;
    moveEvent->moved = true;
    moveEvent->rect = FXRectangle(0, 0, 0, 0);
    return moveEvent;
}


FXEvent*
InternalTestStep::buildMouseLeftClickPressEvent(const int posX, const int posY) const {
    FXEvent* leftClickPressEvent = new FXEvent();
    // common values
    leftClickPressEvent->synthetic = true;
    // set event values
    leftClickPressEvent->win_x = posX + MOUSE_OFFSET_X;
    leftClickPressEvent->win_y = posY + MOUSE_OFFSET_Y;
    leftClickPressEvent->click_x = posX + MOUSE_OFFSET_X;
    leftClickPressEvent->click_y = posY + MOUSE_OFFSET_Y;
    leftClickPressEvent->type = SEL_LEFTBUTTONPRESS;
    leftClickPressEvent->state = 256;
    leftClickPressEvent->code = 1;
    leftClickPressEvent->click_button = 1;
    leftClickPressEvent->click_count = 1;
    leftClickPressEvent->moved = false;
    return leftClickPressEvent;
}


FXEvent*
InternalTestStep::buildMouseLeftClickReleaseEvent(const int posX, const int posY) const {
    FXEvent* leftClickReleaseEvent = new FXEvent();
    // common values
    leftClickReleaseEvent->synthetic = true;
    // set event values
    leftClickReleaseEvent->win_x = posX + MOUSE_OFFSET_X;
    leftClickReleaseEvent->win_y = posY + MOUSE_OFFSET_Y;
    leftClickReleaseEvent->click_x = posX + MOUSE_OFFSET_X;
    leftClickReleaseEvent->click_y = posY + MOUSE_OFFSET_Y;
    leftClickReleaseEvent->type = SEL_LEFTBUTTONRELEASE;
    leftClickReleaseEvent->state = 256;
    leftClickReleaseEvent->code = 1;
    leftClickReleaseEvent->click_button = 1;
    leftClickReleaseEvent->click_count = 1;
    leftClickReleaseEvent->moved = false;
    return leftClickReleaseEvent;
}


std::string
InternalTestStep::parseStep(const std::string& rowText) {
    // first check if this is the netedit.setupAndStart function
    if (rowText.find("netedit.setupAndStart") != std::string::npos) {
        return "setupAndStart";
    } else if (rowText.compare(0, 8, "netedit.") != 0) {
        // proces only lines that start with "netedit."
        return "";
    } else {
        std::string functionName;
        // make a copy to help editing row
        std::string rowStr = rowText;
        // every function has the format <function>(<argument1>, <argument2>,....,)
        if (rowText.empty() || (rowText.front() == '(') || (rowText.back() != ')')) {
            writeError("parseStep", 0, "function(arguments)");
            return "";
        }
        // first extract function
        while (rowStr.size() > 0) {
            if (rowStr.front() == '(') {
                break;
            } else {
                functionName.push_back(rowStr.front());
                rowStr.erase(rowStr.begin());
            }
        }
        // remove prefix "netedit." (size 8) from function
        functionName = functionName.substr(8);
        // check if there are at least two characters (to avoid cases like 'function)')
        if (rowStr.size() < 2) {
            writeError("parseStep", 0, "function(arguments)");
            return functionName;
        }
        // remove both pharentesis
        rowStr.erase(rowStr.begin());
        rowStr.pop_back();
        // now parse arguments
        parseArguments(rowStr);
        // remove "netedit." from frunction
        return functionName;
    }
}


void
InternalTestStep::parseArguments(const std::string& arguments) {
    std::string current;
    bool inQuotes = false;
    for (size_t i = 0; i < arguments.length(); ++i) {
        char c = arguments[i];
        if (c == '\"' || c == '\'') {
            // Toggle quote state
            inQuotes = !inQuotes;
            current.push_back(c);
        } else if (c == ',' && !inQuotes) {
            // End of argument
            if (!current.empty()) {
                // Trim leading/trailing whitespace
                size_t start = current.find_first_not_of(" \t");
                size_t end = current.find_last_not_of(" \t");
                myArguments.push_back(current.substr(start, end - start + 1));
                current.clear();
            }
        } else {
            current += c;
        }
    }
    // Add the last argument
    if (!current.empty()) {
        size_t start = current.find_first_not_of(" \t");
        size_t end = current.find_last_not_of(" \t");
        myArguments.push_back(current.substr(start, end - start + 1));
    }
    // inQuotes MUST be false, in other case we have a case like < "argument1", argument2, "argument3 >
    if (inQuotes) {
        writeError("parseArguments", 0, "<\"argument\", \"argument\">");
        myArguments.clear();
    }
}


void
InternalTestStep::setupAndStart() {
    myCategory = Category::INIT;
    // print in console the following lines
    std::cout << "TestFunctions: Netedit opened successfully" << std::endl;
    std::cout << "Finding reference" << std::endl;
    std::cout << "TestFunctions: 'reference.png' found. Position: " <<
              toString(MOUSE_REFERENCE_X) << " - " <<
              toString(MOUSE_REFERENCE_Y) << std::endl;
}


void
InternalTestStep::leftClick(const std::string& modifier) const {
    if ((myArguments.size() != 2) || (myTestSystem->getViewPositions().count(myArguments[1]) == 0)) {
        writeError("leftClick", 0, "<reference, position>");
    } else {
        // parse arguments
        const int posX = myTestSystem->getViewPositions().at(myArguments[1]).first;
        const int posY = myTestSystem->getViewPositions().at(myArguments[1]).second;
        // check if add key modifier
        if (modifier == "control") {
            new InternalTestStep(myTestSystem, SEL_KEYPRESS, Category::APP, buildKeyPressEvent(modifier), false);
            // print info
            std::cout << "TestFunctions: Clicked with Control key pressed over position " <<
                      toString(posX + MOUSE_REFERENCE_X) << " - " <<
                      toString(posY + MOUSE_REFERENCE_Y) << std::endl;
        } else if (modifier == "shift") {
            new InternalTestStep(myTestSystem, SEL_KEYPRESS, Category::APP, buildKeyPressEvent(modifier), false);
            // print info
            std::cout << "TestFunctions: Clicked with Shift key pressed over position " <<
                      toString(posX + MOUSE_REFERENCE_X) << " - " <<
                      toString(posY + MOUSE_REFERENCE_Y) << std::endl;
        } else {
            // print info
            std::cout << "TestFunctions: Clicked over position " <<
                      toString(posX + MOUSE_REFERENCE_X) << " - " <<
                      toString(posY + MOUSE_REFERENCE_Y) << std::endl;
        }
        // add move, left button press and left button release
        new InternalTestStep(myTestSystem, SEL_MOTION, Category::VIEW, buildMouseMoveEvent(posX, posY), true);
        new InternalTestStep(myTestSystem, SEL_LEFTBUTTONPRESS, Category::VIEW, buildMouseLeftClickPressEvent(posX, posY), true);
        new InternalTestStep(myTestSystem, SEL_LEFTBUTTONRELEASE, Category::VIEW, buildMouseLeftClickReleaseEvent(posX, posY), true);
        // check if add key modifier
        if (!modifier.empty()) {
            new InternalTestStep(myTestSystem, SEL_KEYRELEASE, Category::APP, buildKeyReleaseEvent(modifier), true);
        }
    }
}


void
InternalTestStep::typeKey() const {
    if (myArguments.size() != 1) {
        writeError("typeKey", 0, "<key>");
    } else {
        buildPressKeyEvent(getStringArgument(myArguments[0]), true);
    }
}


void
InternalTestStep::contextualMenuOperation() const {
    if ((myArguments.size() != 3) || (myTestSystem->getViewPositions().count(myArguments[1]) == 0) ||
            !checkIntArgument(myArguments[2])) {
        writeError("contextualMenuOperation", 0, "<reference, position, int/contextualMenuOperations>");
    } else {
        // parse arguments
        const int posX = myTestSystem->getViewPositions().at(myArguments[1]).first;
        const int posY = myTestSystem->getViewPositions().at(myArguments[1]).second;
        //const int attribute = getIntArgument(myArguments[0]);
    }
}


void
InternalTestStep::modifyAttribute(const int overlappedTabs) const {
    if ((myArguments.size() != 2) || !checkIntArgument(myArguments[0]) ||
            !checkStringArgument(myArguments[1])) {
        writeError("modifyAttribute", overlappedTabs, "<int/attributeEnum, \"string\">");
    } else {
        const int attribute = getIntArgument(myArguments[0]);
        const std::string value = getStringArgument(myArguments[1]);
        // print info
        std::cout << value << std::endl;
        // focus frame
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
        // jump to the element
        for (int i = 0; i < (attribute + overlappedTabs); i++) {
            buildPressKeyEvent("tab", false);
        }
        // write attribute character by character
        if (value.empty()) {
            buildPressKeyEvent("delete", false);
        } else {
            for (const char c : value) {
                buildPressKeyEvent(c, false);
            }
        }
        // press enter to confirm changes (updating view)
        buildPressKeyEvent("enter", true);
    }
}


void
InternalTestStep::modifyBoolAttribute(const int overlappedTabs) const {
    if ((myArguments.size() != 1) || !checkIntArgument(myArguments[0])) {
        writeError("modifyBoolAttribute", overlappedTabs, "<int/attributeEnum>");
    } else {
        const int attribute = getIntArgument(myArguments[0]);
        // focus frame
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
        // jump to the element
        for (int i = 0; i < (attribute + overlappedTabs); i++) {
            buildPressKeyEvent("tab", false);
        }
        // toogle attribute
        buildPressKeyEvent("space", true);
    }
}


void
InternalTestStep::modifyColorAttribute(const int overlappedTabs) const {
    if ((myArguments.size() != 1) || !checkIntArgument(myArguments[0])) {
        writeError("modifyColorAttribute", overlappedTabs, "<int/attributeEnum>");
    } else {
        const int attribute = getIntArgument(myArguments[0]);
        // focus frame
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
        // jump to the element
        for (int i = 0; i < (attribute + overlappedTabs); i++) {
            buildPressKeyEvent("tab", false);
        }
        // open dialog
        auto spaceEvent = buildPressKeyEvent("space", false);
        // go to the list of colors
        for (int i = 0; i < 2; i++) {
            buildTwoPressKeyEvent(spaceEvent, "shift", "tab");
        }
        // select color
        for (int i = 0; i < 6; i++) {
            buildPressKeyEvent(spaceEvent, "down");
        }
        // go to button
        buildPressKeyEvent(spaceEvent, "tab");
        // press button
        buildPressKeyEvent(spaceEvent, "space");
    }
}


void
InternalTestStep::modifyVClassDialog_NoDisallowAll(const int overlappedTabs) const {
    if ((myArguments.size() != 2) || !checkIntArgument(myArguments[0]) ||
            !checkIntArgument(myArguments[1])) {
        writeError("modifyVClassDialog_NoDisallowAll", overlappedTabs, "<int/attributeEnum, int/attributeEnum>");
    } else {
        // parse input
        const int attribute = getIntArgument(myArguments[0]);
        const int vClass = getIntArgument(myArguments[1]);
        // focus frame
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
        // jump to open dialog button
        for (int i = 0; i < (attribute + overlappedTabs); i++) {
            buildPressKeyEvent("tab", false);
        }
        // open dialog
        auto openDialogEvent = buildPressKeyEvent("space", true);
        // jump to vClass
        for (int i = 0; i < vClass; i++) {
            buildPressKeyEvent(openDialogEvent, "tab");
        }
        // select vclass
        buildPressKeyEvent(openDialogEvent, "space");
        // go to accept button
        for (int i = 0; i < (myTestSystem->getAttributesEnum().at("netedit.attrs.dialog.allowVClass.accept") - vClass); i++) {
            buildPressKeyEvent(openDialogEvent, "tab");
        }
        // press accept
        buildPressKeyEvent(openDialogEvent, "space");
    }
}


void
InternalTestStep::modifyVClassDialog_DisallowAll(const int overlappedTabs) const {
    if ((myArguments.size() != 2) || !checkIntArgument(myArguments[0]) ||
            !checkIntArgument(myArguments[1])) {
        writeError("modifyVClassDialog_DisallowAll", overlappedTabs, "<int/attributeEnum, int/attributeEnum>");
    } else {
        // parse input
        const int attribute = getIntArgument(myArguments[0]);
        const int vClass = getIntArgument(myArguments[1]);
        // focus frame
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
        // jump to open dialog button
        for (int i = 0; i < (attribute + overlappedTabs); i++) {
            buildPressKeyEvent("tab", false);
        }
        // open dialog
        auto openDialogEvent = buildPressKeyEvent("space", true);
        // go to disallow all vehicles
        for (int i = 0; i < myTestSystem->getAttributesEnum().at("netedit.attrs.dialog.allowVClass.disallowAll"); i++) {
            buildPressKeyEvent(openDialogEvent, "tab");
        }
        // disallow all vehicles
        buildPressKeyEvent(openDialogEvent, "space");
        // go to vClass
        for (int i = 0; i < (vClass - myTestSystem->getAttributesEnum().at("netedit.attrs.dialog.allowVClass.disallowAll")); i++) {
            buildPressKeyEvent(openDialogEvent, "tab");
        }
        // select vClass
        buildPressKeyEvent(openDialogEvent, "space");
        // go to accept button
        for (int i = 0; i < (myTestSystem->getAttributesEnum().at("netedit.attrs.dialog.allowVClass.accept") - vClass); i++) {
            buildPressKeyEvent(openDialogEvent, "tab");
        }
        // press accept
        buildPressKeyEvent(openDialogEvent, "space");
    }
}


void
InternalTestStep::modifyVClassDialog_Cancel(const int overlappedTabs) const {
    if ((myArguments.size() != 2) ||
            !checkIntArgument(myArguments[0]) ||
            !checkIntArgument(myArguments[1])) {
        writeError("modifyVClassDialog_Cancel", overlappedTabs, "<int/attributeEnum, int/attributeEnum>");
    } else {
        // parse input
        const int attribute = getIntArgument(myArguments[0]);
        const int vClass = getIntArgument(myArguments[1]);
        // focus frame
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
        // jump to open dialog button
        for (int i = 0; i < (attribute + overlappedTabs); i++) {
            buildPressKeyEvent("tab", false);
        }
        // open dialog
        auto openDialogEvent = buildPressKeyEvent("space", true);
        // go to disallow all vehicles
        for (int i = 0; i < myTestSystem->getAttributesEnum().at("netedit.attrs.dialog.allowVClass.disallowAll"); i++) {
            buildPressKeyEvent(openDialogEvent, "tab");
        }
        // disallow all vehicles
        buildPressKeyEvent(openDialogEvent, "space");
        // go to vClass
        for (int i = 0; i < (vClass - myTestSystem->getAttributesEnum().at("netedit.attrs.dialog.allowVClass.disallowAll")); i++) {
            buildPressKeyEvent(openDialogEvent, "tab");
        }
        // select vClass
        buildPressKeyEvent(openDialogEvent, "space");
        // go to cancel button
        for (int i = 0; i < (myTestSystem->getAttributesEnum().at("netedit.attrs.dialog.allowVClass.cancel") - vClass); i++) {
            buildPressKeyEvent(openDialogEvent, "tab");
        }
        // press cancel
        buildPressKeyEvent(openDialogEvent, "space");
    }
}


void
InternalTestStep::modifyVClassDialog_Reset(const int overlappedTabs) const {
    if ((myArguments.size() != 2) || !checkIntArgument(myArguments[0]) ||
            !checkIntArgument(myArguments[1])) {
        writeError("modifyVClassDialog_Reset", overlappedTabs, "<int/attributeEnum, int/attributeEnum>");
    } else {
        // parse input
        const int attribute = getIntArgument(myArguments[0]);
        const int vClass = getIntArgument(myArguments[1]);
        // focus frame
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
        // jump to open dialog button
        for (int i = 0; i < (attribute + overlappedTabs); i++) {
            buildPressKeyEvent("tab", false);
        }
        // open dialog
        auto openDialogEvent = buildPressKeyEvent("space", true);
        // go to disallow all vehicles
        for (int i = 0; i < myTestSystem->getAttributesEnum().at("netedit.attrs.dialog.allowVClass.disallowAll"); i++) {
            buildPressKeyEvent(openDialogEvent, "tab");
        }
        // disallow all vehicles
        buildPressKeyEvent(openDialogEvent, "space");
        // go to vClass
        for (int i = 0; i < (vClass - myTestSystem->getAttributesEnum().at("netedit.attrs.dialog.allowVClass.disallowAll")); i++) {
            buildPressKeyEvent(openDialogEvent, "tab");
        }
        // select vClass
        buildPressKeyEvent(openDialogEvent, "space");
        // go to reset button
        for (int i = 0; i < (myTestSystem->getAttributesEnum().at("netedit.attrs.dialog.allowVClass.reset") - vClass); i++) {
            buildPressKeyEvent(openDialogEvent, "tab");
        }
        // press reset
        buildPressKeyEvent(openDialogEvent, "space");
        // go to accept button
        for (int i = 0; i < 2; i++) {
            buildTwoPressKeyEvent(openDialogEvent, "shift", "tab");
        }
        // press accept
        buildPressKeyEvent(openDialogEvent, "space");
    }
}


void
InternalTestStep::changeEditMode() {
    if ((myArguments.size() != 1) || (myTestSystem->getAttributesEnum().count(myArguments[0]) == 0)) {
        writeError("changeEditMode", 0, "<int/attributeEnum>");
    } else {
        myCategory = Category::APP;
        // network
        if (myArguments[0] == "netedit.attrs.modes.network.grid") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_TOGGLEGRID;
        } else if (myArguments[0] == "netedit.attrs.modes.network.junctionShape") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE;
        } else if (myArguments[0] == "netedit.attrs.modes.network.spreadVehicle") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_DRAWSPREADVEHICLES;
        } else if (myArguments[0] == "netedit.attrs.modes.network.showDemandElements") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS;
        } else if (myArguments[0] == "netedit.attrs.modes.network.selectLane") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES;
        } else if (myArguments[0] == "netedit.attrs.modes.network.showConnections") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS;
        } else if (myArguments[0] == "netedit.attrs.modes.network.hideConnetions") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS;
        } else if (myArguments[0] == "netedit.attrs.modes.network.showSubAdditionals") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_SHOWSUBADDITIONALS;
        } else if (myArguments[0] == "netedit.attrs.modes.network.showTAZElements") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_SHOWTAZELEMENTS;
        } else if (myArguments[0] == "netedit.attrs.modes.network.automaticSelectJunctions") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION;
        } else if (myArguments[0] == "netedit.attrs.modes.network.applyAllPhases") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES;
        } else if (myArguments[0] == "netedit.attrs.modes.network.mergingJunction") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_MERGEAUTOMATICALLY;
        } else if (myArguments[0] == "netedit.attrs.modes.network.showBubbles") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES;
        } else if (myArguments[0] == "netedit.attrs.modes.network.moveElevation") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION;
        } else if (myArguments[0] == "netedit.attrs.modes.network.chainMode") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES;
        } else if (myArguments[0] == "netedit.attrs.modes.network.twoWayMode") {
            myMessageID = MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES;
            // demand
        } else if (myArguments[0] == "netedit.attrs.modes.demand.grid") {
            myMessageID = MID_GNE_DEMANDVIEWOPTIONS_SHOWGRID;
        } else if (myArguments[0] == "netedit.attrs.modes.demand.junctionShape") {
            myMessageID = MID_GNE_DEMANDVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE;
        } else if (myArguments[0] == "netedit.attrs.modes.demand.spreadVehicle") {
            myMessageID = MID_GNE_DEMANDVIEWOPTIONS_DRAWSPREADVEHICLES;
        } else if (myArguments[0] == "netedit.attrs.modes.demand.showNonInspected") {
            myMessageID = MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED;
        } else if (myArguments[0] == "netedit.attrs.modes.demand.showShapes") {
            myMessageID = MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES;
        } else if (myArguments[0] == "netedit.attrs.modes.demand.showAllTrips") {
            myMessageID = MID_GNE_DEMANDVIEWOPTIONS_SHOWTRIPS;
        } else if (myArguments[0] == "netedit.attrs.modes.demand.showPersonPlans") {
            myMessageID = MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS;
        } else if (myArguments[0] == "netedit.attrs.modes.demand.lockPerson") {
            myMessageID = MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON;
        } else if (myArguments[0] == "netedit.attrs.modes.demand.showContainerPlans") {
            myMessageID = MID_GNE_DEMANDVIEWOPTIONS_SHOWALLCONTAINERPLANS;
        } else if (myArguments[0] == "netedit.attrs.modes.demand.lockContainer") {
            myMessageID = MID_GNE_DEMANDVIEWOPTIONS_LOCKCONTAINER;
        } else if (myArguments[0] == "netedit.attrs.modes.demand.showOverlappedRoutes") {
            myMessageID = MID_GNE_DEMANDVIEWOPTIONS_SHOWOVERLAPPEDROUTES;
            // data
        } else if (myArguments[0] == "netedit.attrs.modes.data.junctionShape") {
            myMessageID = MID_GNE_DATAVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE;
        } else if (myArguments[0] == "netedit.attrs.modes.data.showAdditionals") {
            myMessageID = MID_GNE_DATAVIEWOPTIONS_SHOWADDITIONALS;
        } else if (myArguments[0] == "netedit.attrs.modes.data.showShapes") {
            myMessageID = MID_GNE_DATAVIEWOPTIONS_SHOWSHAPES;
        } else if (myArguments[0] == "netedit.attrs.modes.data.showDemandElements") {
            myMessageID = MID_GNE_DATAVIEWOPTIONS_SHOWDEMANDELEMENTS;
        } else if (myArguments[0] == "netedit.attrs.modes.data.TAZRelDrawingMode") {
            myMessageID = MID_GNE_DATAVIEWOPTIONS_TAZRELDRAWING;
        } else if (myArguments[0] == "netedit.attrs.modes.data.TAZFill") {
            myMessageID = MID_GNE_DATAVIEWOPTIONS_TAZDRAWFILL;
        } else if (myArguments[0] == "netedit.attrs.modes.data.TAZRelOnlyFrom") {
            myMessageID = MID_GNE_DATAVIEWOPTIONS_TAZRELONLYFROM;
        } else if (myArguments[0] == "netedit.attrs.modes.data.TAZRelOnlyTo") {
            myMessageID = MID_GNE_DATAVIEWOPTIONS_TAZRELONLYTO;
        } else {
            writeError("changeEditMode", 0, "<enum>");
        }
    }
}


void
InternalTestStep::saveExistentShortcut() {
    if ((myArguments.size() != 1) ||
            !checkStringArgument(myArguments[0])) {
        writeError("save", 0, "<\"string\">");
    } else {
        myCategory = Category::APP;
        const auto savingType = getStringArgument(myArguments[0]);
        if (savingType == "neteditConfig") {
            myMessageID = MID_HOTKEY_CTRL_SHIFT_E_SAVENETEDITCONFIG;
        } else {
            writeError("save", 0, "<neteditConfig>");
        }
    }
}


void
InternalTestStep::checkUndoRedo() const {
    if (myArguments.size() != 1) {
        writeError("checkUndoRedo", 0, "<referencePosition>");
    } else {
        const int numUndoRedos = 9;
        // focus frame
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
        // go to inspect mode
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_I_MODE_INSPECT, Category::APP);
        // click over reference
        std::cout << "TestFunctions: Clicked over position " <<
                  toString(MOUSE_REFERENCE_X) << " - " <<
                  toString(MOUSE_REFERENCE_Y) << std::endl;
        // add move, left button press and left button release
        new InternalTestStep(myTestSystem, SEL_MOTION, Category::VIEW, buildMouseMoveEvent(0, 0), true);
        new InternalTestStep(myTestSystem, SEL_LEFTBUTTONPRESS, Category::VIEW, buildMouseLeftClickPressEvent(0, 0), true);
        new InternalTestStep(myTestSystem, SEL_LEFTBUTTONRELEASE, Category::VIEW, buildMouseLeftClickReleaseEvent(0, 0), true);
        // undo
        for (int i = 0; i < numUndoRedos; i++) {
            new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_CTRL_Z_UNDO, Category::APP);
        }
        // focus frame
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
        // go to inspect mode
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_I_MODE_INSPECT, Category::APP);
        // click over reference
        std::cout << "TestFunctions: Clicked over position " <<
                  toString(MOUSE_REFERENCE_X) << " - " <<
                  toString(MOUSE_REFERENCE_Y) << std::endl;
        // add move, left button press and left button release
        new InternalTestStep(myTestSystem, SEL_MOTION, Category::VIEW, buildMouseMoveEvent(0, 0), true);
        new InternalTestStep(myTestSystem, SEL_LEFTBUTTONPRESS, Category::VIEW, buildMouseLeftClickPressEvent(0, 0), true);
        new InternalTestStep(myTestSystem, SEL_LEFTBUTTONRELEASE, Category::VIEW, buildMouseLeftClickReleaseEvent(0, 0), true);
        // undo
        for (int i = 0; i < numUndoRedos; i++) {
            new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_CTRL_Y_REDO, Category::APP);
        }
    }
}


void
InternalTestStep::deleteFunction() const {
    if (myArguments.size() != 0) {
        writeError("delete", 0, "<>");
    } else {
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_DEL, Category::APP);
    }
}


void
InternalTestStep::selection() const {
    if (myArguments.size() != 1 || !checkStringArgument(myArguments[0])) {
        writeError("selection", 0, "<selection operation>");
    } else {
        const std::string selectionType = getStringArgument(myArguments[0]);
        // get number of tabls
        int numTabs = 0;
        if (selectionType == "default") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.selection.default");
        } else if (selectionType == "save") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.selection.save");
        } else if (selectionType == "load") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.selection.load");
        } else if (selectionType == "add") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.selection.add");
        } else if (selectionType == "remove") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.selection.remove");
        } else if (selectionType == "keep") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.selection.keep");
        } else if (selectionType == "replace") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.selection.replace");
        } else if (selectionType == "clear") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.selection.clear");
        } else if (selectionType == "invert") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.selection.invert");
        } else if (selectionType == "invertData") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.selection.invertData");
        } else if (selectionType == "delete") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.selection.delete");
        }
        // focus frame
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
        // jump to the element
        for (int i = 0; i < numTabs; i++) {
            buildPressKeyEvent("tab", false);
        }
        if (selectionType == "save") {
            buildPressKeyEvent("enter", false);
            // complete
        } else if (selectionType == "load") {
            buildPressKeyEvent("enter", false);
            // complete
        } else {
            buildPressKeyEvent("space", true);
        }
    }
}


void
InternalTestStep::undo() const {
    if ((myArguments.size() != 2) || !checkIntArgument(myArguments[1])) {
        writeError("undo", 0, "<referencePosition, int>");
    } else {
        const int numUndoRedos = getIntArgument(myArguments[1]);
        // focus frame
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
        // go to inspect mode
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_I_MODE_INSPECT, Category::APP);
        // click over reference
        std::cout << "TestFunctions: Clicked over position " <<
                  toString(MOUSE_REFERENCE_X) << " - " <<
                  toString(MOUSE_REFERENCE_Y) << std::endl;
        // add move, left button press and left button release
        new InternalTestStep(myTestSystem, SEL_MOTION, Category::VIEW, buildMouseMoveEvent(0, 0), true);
        new InternalTestStep(myTestSystem, SEL_LEFTBUTTONPRESS, Category::VIEW, buildMouseLeftClickPressEvent(0, 0), true);
        new InternalTestStep(myTestSystem, SEL_LEFTBUTTONRELEASE, Category::VIEW, buildMouseLeftClickReleaseEvent(0, 0), true);
        // undo
        for (int i = 0; i < numUndoRedos; i++) {
            new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_CTRL_Z_UNDO, Category::APP);
        }
    }
}


void
InternalTestStep::redo() const {
    if ((myArguments.size() != 2) || !checkIntArgument(myArguments[1])) {
        writeError("redo", 0, "<referencePosition, int>");
    } else {
        const int numUndoRedos = getIntArgument(myArguments[1]);
        // focus frame
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
        // go to inspect mode
        new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_I_MODE_INSPECT, Category::APP);
        // click over reference
        std::cout << "TestFunctions: Clicked over position " <<
                  toString(MOUSE_REFERENCE_X) << " - " <<
                  toString(MOUSE_REFERENCE_Y) << std::endl;
        // add move, left button press and left button release
        new InternalTestStep(myTestSystem, SEL_MOTION, Category::VIEW, buildMouseMoveEvent(0, 0), true);
        new InternalTestStep(myTestSystem, SEL_LEFTBUTTONPRESS, Category::VIEW, buildMouseLeftClickPressEvent(0, 0), true);
        new InternalTestStep(myTestSystem, SEL_LEFTBUTTONRELEASE, Category::VIEW, buildMouseLeftClickReleaseEvent(0, 0), true);
        // undo
        for (int i = 0; i < numUndoRedos; i++) {
            new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_CTRL_Y_REDO, Category::APP);
        }
    }
}


void
InternalTestStep::changeSupermode() {
    if ((myArguments.size() != 1) ||
            !checkStringArgument(myArguments[0])) {
        writeError("supermode", 0, "<\"string\">");
    } else {
        myCategory = Category::APP;
        const std::string supermode = getStringArgument(myArguments[0]);
        if (supermode == "network") {
            myMessageID = MID_HOTKEY_F2_SUPERMODE_NETWORK;
        } else if (supermode == "demand") {
            myMessageID = MID_HOTKEY_F3_SUPERMODE_DEMAND;
        } else if (supermode == "data") {
            myMessageID = MID_HOTKEY_F4_SUPERMODE_DATA;
        } else {
            writeError("supermode", 0, "<network/demand/data>");
        }
    }
}


void
InternalTestStep::changeMode() {
    if ((myArguments.size() != 1) ||
            !checkStringArgument(myArguments[0])) {
        writeError("changeMode", 0, "<\"string\">");
    } else {
        myCategory = Category::APP;
        const std::string networkMode = getStringArgument(myArguments[0]);
        if (networkMode == "inspect") {
            myMessageID = MID_HOTKEY_I_MODE_INSPECT;
        } else if (networkMode == "delete") {
            myMessageID = MID_HOTKEY_D_MODE_SINGLESIMULATIONSTEP_DELETE;
        } else if (networkMode == "select") {
            myMessageID = MID_HOTKEY_S_MODE_STOPSIMULATION_SELECT;
        } else if (networkMode == "move") {
            myMessageID = MID_HOTKEY_M_MODE_MOVE_MEANDATA;
        } else if ((networkMode == "createEdge") || (networkMode == "edgeData")) {
            myMessageID = MID_HOTKEY_E_MODE_EDGE_EDGEDATA;
        } else if ((networkMode == "trafficLight") || (networkMode == "type")) {
            myMessageID = MID_HOTKEY_T_MODE_TLS_TYPE;
        } else if ((networkMode == "connection") || (networkMode == "container")) {
            myMessageID = MID_HOTKEY_C_MODE_CONNECT_CONTAINER;
        } else if ((networkMode == "prohibition") || (networkMode == "containerPlan")) {
            myMessageID = MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN;
        } else if ((networkMode == "crossing") || (networkMode == "edgeRelData")) {
            myMessageID = MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA;
        } else if ((networkMode == "additional") || (networkMode == "stop")) {
            myMessageID = MID_HOTKEY_A_MODE_STARTSIMULATION_ADDITIONALS_STOPS;
        } else if ((networkMode == "wire") || (networkMode == "routeDistribution")) {
            myMessageID = MID_HOTKEY_W_MODE_WIRE_ROUTEDISTRIBUTION;
        } else if ((networkMode == "taz") || (networkMode == "tazRel")) {
            myMessageID = MID_HOTKEY_Z_MODE_TAZ_TAZREL;
        } else if ((networkMode == "shape") || (networkMode == "person")) {
            myMessageID = MID_HOTKEY_P_MODE_POLYGON_PERSON;
        } else if ((networkMode == "decal") || (networkMode == "typeDistribution")) {
            myMessageID = MID_HOTKEY_U_MODE_DECAL_TYPEDISTRIBUTION;
        } else if (networkMode == "personPlan") {
            myMessageID = MID_HOTKEY_L_MODE_PERSONPLAN;
        } else if (networkMode == "vehicle") {
            myMessageID = MID_HOTKEY_V_MODE_VEHICLE;
        } else {
            writeError("changeMode", 0, "<inspect/delete/select/move...>");
        }
    }
}


void
InternalTestStep::changeElement() const {
    if ((myArguments.size() != 2) ||
            !checkStringArgument(myArguments[0])) {
        writeError("selectAdditional", 0, "<\"frame\", \"string\">");
    } else {
        const std::string frame = getStringArgument(myArguments[0]);
        const std::string element = getStringArgument(myArguments[1]);
        int numTabs = -1;
        // continue depending of frame
        if (frame == "additionalFrame") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.changeElement.additional");
        } else if (frame == "shapeFrame") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.changeElement.shape");
        } else if (frame == "vehicleFrame") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.changeElement.vehicle");
        } else if (frame == "routeFrame") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.changeElement.route");
        } else if (frame == "personFrame") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.changeElement.person");
        } else if (frame == "containerFrame") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.netedit.attrs.frames.changeElement.container");
        } else if (frame == "personPlanFrame") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.changeElement.personPlan");
        } else if (frame == "containerPlanFrame") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.netedit.attrs.frames.changeElement.containerPlan");
        } else if (frame == "stopFrame") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.changeElement.stop");
        } else if (frame == "meanDataFrame") {
            numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.changeElement.meanData");
        } else {
            WRITE_ERRORF("Invalid frame '%' used in function changeElement", frame);
        }
        if (numTabs >= 0) {
            // show info
            std::cout << element << std::endl;
            // focus frame
            new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
            // jump to select additional argument
            for (int i = 0; i < numTabs; i++) {
                buildPressKeyEvent("tab", false);
            }
            // write additional character by character
            for (const char c : element) {
                buildPressKeyEvent(c, false);
            }
            // press enter to confirm changes (updating view)
            buildPressKeyEvent("enter", true);
        }
    }
}


void
InternalTestStep::changePlan()  const {
    if ((myArguments.size() != 3) ||
            !checkStringArgument(myArguments[0]) ||
            !checkStringArgument(myArguments[1]) ||
            !checkBoolArgument(myArguments[2])) {
        writeError("changePlan", 0, "<\"type\", \"plan\", true/false>");
    } else {
        // get arguments
        const std::string type = getStringArgument(myArguments[0]);
        const std::string plan = getStringArgument(myArguments[1]);
        const bool flow = getBoolArgument(myArguments[2]);
        // check plan
        if ((type != "person") && (type != "container")) {
            WRITE_ERRORF("invalid plan type '%' used in changePlan()", type);
        } else {
            // calculate num tabs
            int numTabs = 0;
            if (flow) {
                numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.changePlan." + type + "Flow");
            } else {
                numTabs = myTestSystem->getAttributesEnum().at("netedit.attrs.frames.changePlan." + type);
            }
            // focus frame
            new InternalTestStep(myTestSystem, SEL_COMMAND, MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT, Category::APP);
            // jump to select additional argument
            for (int i = 0; i < numTabs; i++) {
                buildPressKeyEvent("tab", false);
            }
            // write additional character by character
            for (const char c : plan) {
                buildPressKeyEvent(c, false);
            }
            // print info
            std::cout << plan << std::endl;
            // press enter to confirm changes (updating view)
            buildPressKeyEvent("enter", true);
        }
    }
}


void
InternalTestStep::computeJunctions() {
    if (myArguments.size() > 0) {
        writeError("computeJunctions", 0, "<>");
    } else {
        myCategory = Category::APP;
        myMessageID = MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND;
    }
}


void
InternalTestStep::computeJunctionsVolatileOptions() {
    if (myArguments.size() > 1) {
        writeError("computeJunctionsVolatileOptions", 0, "<True/False>");
    } else {
        FXuint result = ModalArguments::yes;
        if ((myArguments.size() == 1) && (myArguments[0] == "False")) {
            result = ModalArguments::no;
        }
        myCategory = Category::APP;
        myMessageID = MID_HOTKEY_SHIFT_F5_COMPUTEJUNCTIONS_VOLATILE;
        myModalArguments = new ModalArguments({result});
    }
}


void
InternalTestStep::quit() {
    if (myArguments.size() == 0) {
        writeError("quit", 0, "<neteditProcess>");
    } else {
        myCategory = Category::APP;
        myMessageID = MID_HOTKEY_CTRL_Q_CLOSE;
        //don't update view if we're closing to avoid problems with drawGL
        myUpdateView = false;
    }
}


bool
InternalTestStep::checkIntArgument(const std::string& argument) const {
    if (StringUtils::isInt(argument)) {
        return true;
    } else if (myTestSystem->getAttributesEnum().count(argument) > 0) {
        return true;
    } else {
        return false;
    }
}


int
InternalTestStep::getIntArgument(const std::string& argument) const {
    if (StringUtils::isInt(argument)) {
        return StringUtils::toInt(argument);
    } else {
        return myTestSystem->getAttributesEnum().at(argument);
    }
}


bool
InternalTestStep::checkBoolArgument(const std::string& argument) const {
    if (argument == "True") {
        return true;
    } else if (argument == "False") {
        return true;
    } else {
        return false;
    }
}


bool
InternalTestStep::getBoolArgument(const std::string& argument) const {
    if (argument == "True") {
        return true;
    } else {
        return false;
    }
}


bool
InternalTestStep::checkStringArgument(const std::string& argument) const {
    if (argument.size() < 2) {
        return false;
    } else if ((argument.front() != argument.back()) || ((argument.front() != '\'') && ((argument.front() != '\"')))) {
        return false;
    } else {
        return true;
    }
}


std::string
InternalTestStep::getStringArgument(const std::string& argument) const {
    std::string argumentParsed;
    for (int i = 1; i < ((int)argument.size() - 1); i++) {
        argumentParsed.push_back(argument[i]);
    }
    return argumentParsed;
}


std::string
InternalTestStep::stripSpaces(const std::string& str) const {
    auto start = std::find_if_not(str.begin(), str.end(), isspace);
    auto end = std::find_if_not(str.rbegin(), str.rend(), isspace).base();
    if (start < end) {
        return std::string(start, end);
    } else {
        return "";
    }
}


void
InternalTestStep::writeError(const std::string& function, const int overlapping, const std::string& expected) const {
    if (overlapping > 0) {
        WRITE_ERRORF("Invalid internal testStep function '%Ovelapped', requires '%' arguments ", function, expected);
    } else {
        WRITE_ERRORF("Invalid internal testStep function '%', requires '%' arguments ", function, expected);
    }
}


template <> std::pair<FXint, FXString>
InternalTestStep::translateKey(const std::string key) const {
    std::pair<FXint, FXString> solution;
    // continue depending of key
    if (key == "backspace") {
        solution.first = KEY_BackSpace;
        solution.second = "\b";
    } else if (key == "space") {
        solution.first = KEY_space;
    } else if (key == "tab") {
        solution.first = KEY_Tab;
        solution.second = "\t";
    } else if (key == "clear") {
        solution.first = KEY_Clear;
    } else if (key == "enter" || key == "return") {
        solution.first = KEY_Return;
        solution.second = "\n";
    } else if (key == "pause") {
        solution.first = KEY_Pause;
    } else if (key == "sys_req") {
        solution.first = KEY_Sys_Req;
    } else if (key == "esc" || key == "escape") {
        solution.first = KEY_Escape;
        solution.second = "\x1B";
    } else if (key == "delete") {
        solution.first = KEY_Delete;
        solution.second = "\x7F";
    } else if (key == "multi_key") {
        solution.first = KEY_Multi_key;
        // function
    } else if (key == "shift") {
        solution.first = KEY_Shift_L;
    } else if (key == "control") {
        solution.first = KEY_Control_L;
        // Cursor
    } else if (key == "home") {
        solution.first = KEY_Home;
    } else if (key == "left") {
        solution.first = KEY_Left;
    } else if (key == "up") {
        solution.first = KEY_Up;
    } else if (key == "right") {
        solution.first = KEY_Right;
    } else if (key == "down") {
        solution.first = KEY_Down;
    } else if (key == "prior" || key == "page_up") {
        solution.first = KEY_Page_Up;
    } else if (key == "next" || key == "page_down") {
        solution.first = KEY_Page_Down;
    } else if (key == "end") {
        solution.first = KEY_End;
    } else if (key == "begin") {
        solution.first = KEY_Begin;
        // Function keys
    } else if (key == "f1") {
        solution.first = KEY_F1;
    } else if (key == "f2") {
        solution.first = KEY_F2;
    } else if (key == "f3") {
        solution.first = KEY_F3;
    } else if (key == "f4") {
        solution.first = KEY_F4;
    } else if (key == "f5") {
        solution.first = KEY_F5;
    } else if (key == "f6") {
        solution.first = KEY_F6;
    } else if (key == "f7") {
        solution.first = KEY_F7;
    } else if (key == "f8") {
        solution.first = KEY_F8;
    } else if (key == "f9") {
        solution.first = KEY_F9;
    } else if (key == "f10") {
        solution.first = KEY_F10;
    } else if (key == "f11" || key == "l1") {
        solution.first = KEY_F11;
    } else if (key == "f12" || key == "l2") {
        solution.first = KEY_F12;
    } else {
        writeError("translateKey", 0, "<key>");
        solution.first = KEY_VoidSymbol;
    }
    return solution;
}


template <> std::pair<FXint, FXString>
InternalTestStep::translateKey(const char* key) const {
    return translateKey(std::string(key));
}


template <> std::pair<FXint, FXString>
InternalTestStep::translateKey(const char key) const {
    std::pair<FXint, FXString> solution;
    solution.first = FXint(key);
    solution.second.append(key);
    return solution;
}

/****************************************************************************/
