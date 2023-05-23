/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEOptionsDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// A Dialog for setting options (see OptionsCont)
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/options/OptionsCont.h>

#include "GNEOptionsDialogElements.h"
#include "GNEOptionsDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEOptionsDialogElements::InputOption) InputOptionMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GNEOptionsDialogElements::InputOption::onCmdSetOption),
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNEOptionsDialogElements::InputOption, FXHorizontalFrame,  InputOptionMap, ARRAYNUMBER(InputOptionMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEOptionsDialogElements::InputOption::InputOption(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X),
    myGUIDialogOptions(GUIDialogOptions),
    myName(name) {
    // build label with name
    new FXLabel(this, (name + "\t\t" + myGUIDialogOptions->myOptionsContainer->getDescription(name)).c_str());
}


long
GNEOptionsDialogElements::InputOption::onCmdSetOption(FXObject*, FXSelector, void*) {
    // try to set option and mark as modified if was sucessfully
    if (setOption()) {
        myGUIDialogOptions->myModified = true;
    }
    return 1;
}


GNEOptionsDialogElements::InputString::InputString(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(myGUIDialogOptions->myOptionsContainer->getString(name).c_str());
}


bool
GNEOptionsDialogElements::InputString::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return true;
}


GNEOptionsDialogElements::InputStringVector::InputStringVector(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getStringVector(name)).c_str());
}


bool
GNEOptionsDialogElements::InputStringVector::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return true;
}


GNEOptionsDialogElements::InputBool::InputBool(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myCheck = new FXMenuCheck(this, "", this, MID_GNE_SET_ATTRIBUTE);
    myCheck->setCheck(myGUIDialogOptions->myOptionsContainer->getBool(name));
}


bool
GNEOptionsDialogElements::InputBool::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myCheck->getCheck() ? "true" : "false");
    // special checks for Debug flags
    if ((myName == "gui-testing-debug") && myGUIDialogOptions->myOptionsContainer->isSet("gui-testing-debug")) {
        MsgHandler::enableDebugMessages(myGUIDialogOptions->myOptionsContainer->getBool("gui-testing-debug"));
    }
    if ((myName == "gui-testing-debug-gl") && myGUIDialogOptions->myOptionsContainer->isSet("gui-testing-debug-gl")) {
        MsgHandler::enableDebugGLMessages(myGUIDialogOptions->myOptionsContainer->getBool("gui-testing-debug-gl"));
    }
    return true;
}


GNEOptionsDialogElements::InputInt::InputInt(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_INTEGER | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getInt(name)).c_str());
}


bool
GNEOptionsDialogElements::InputInt::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return true;
}


GNEOptionsDialogElements::InputIntVector::InputIntVector(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getIntVector(name)).c_str());
}


bool
GNEOptionsDialogElements::InputIntVector::setOption() {
    try {
        // check that int vector can be parsed
        const auto intVector = StringTokenizer(myTextField->getText().text()).getVector();
        for (const auto& intValue : intVector) {
            StringUtils::toInt(intValue);
        }
        myGUIDialogOptions->myOptionsContainer->resetWritable();
        myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
        myTextField->setTextColor(FXRGB(0, 0, 0));
        return true;
    } catch (...) {
        myTextField->setTextColor(FXRGB(255, 0, 0));
    }
    return false;
}


GNEOptionsDialogElements::InputFloat::InputFloat(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_REAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getFloat(name)).c_str());
}


bool
GNEOptionsDialogElements::InputFloat::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return true;
}


GNEOptionsDialogElements::InputFilename::InputFilename(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(myGUIDialogOptions->myOptionsContainer->getString(name).c_str());
}


bool
GNEOptionsDialogElements::InputFilename::setOption() {
    if (SUMOXMLDefinitions::isValidFilename(myTextField->getText().text())) {
        myGUIDialogOptions->myOptionsContainer->resetWritable();
        myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
        myTextField->setTextColor(FXRGB(0, 0, 0));
        return true;
    } else {
        myTextField->setTextColor(FXRGB(255, 0, 0));
        return false;
    }
}

/****************************************************************************/
