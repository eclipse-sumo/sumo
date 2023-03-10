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
/// @file    GNEToolDialogElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Elements used in GNEToolDialog
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEToolDialogElements.h"
#include "GNEToolDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEToolDialogElements::Argument) ArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEToolDialogElements::Argument::XXXX),
};

FXDEFMAP(GNEToolDialogElements::FileNameArgument) FileNameArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEToolDialogElements::FileNameArgument::onCmdSetFilenameValue),
};

FXDEFMAP(GNEToolDialogElements::StringArgument) StringArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEToolDialogElements::StringArgument::onCmdSetStringValue),
};

FXDEFMAP(GNEToolDialogElements::IntArgument) IntArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEToolDialogElements::IntArgument::onCmdSetIntValue),
};

FXDEFMAP(GNEToolDialogElements::FloatArgument) FloatArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEToolDialogElements::FloatArgument::onCmdSetFloatValue),
};

FXDEFMAP(GNEToolDialogElements::BoolArgument) BoolArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEToolDialogElements::BoolArgument::onCmdSetBoolValue),
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNEToolDialogElements::Argument,   FXHorizontalFrame,                  ArgumentMap,            ARRAYNUMBER(ArgumentMap))
FXIMPLEMENT(GNEToolDialogElements::FileNameArgument,    GNEToolDialogElements::Argument,    FileNameArgumentMap,    ARRAYNUMBER(FileNameArgumentMap))
FXIMPLEMENT(GNEToolDialogElements::StringArgument,      GNEToolDialogElements::Argument,    StringArgumentMap,      ARRAYNUMBER(StringArgumentMap))
FXIMPLEMENT(GNEToolDialogElements::IntArgument,         GNEToolDialogElements::Argument,    IntArgumentMap,         ARRAYNUMBER(IntArgumentMap))
FXIMPLEMENT(GNEToolDialogElements::FloatArgument,       GNEToolDialogElements::Argument,    FloatArgumentMap,       ARRAYNUMBER(FloatArgumentMap))
FXIMPLEMENT(GNEToolDialogElements::BoolArgument,        GNEToolDialogElements::Argument,    BoolArgumentMap,        ARRAYNUMBER(BoolArgumentMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEToolDialogElements::Argument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::Argument::Argument(GNEToolDialog* toolDialogParent, const std::string &parameter, const std::string &defaultValue, const std::string &description) :
    FXHorizontalFrame(toolDialogParent->getContentFrame(), GUIDesignAuxiliarHorizontalFrame),
    myParameter(parameter),
    myDefaultValue(defaultValue) {
    // create parameter label
    myParameterLabel = new MFXLabelTooltip(this, toolDialogParent->getGNEApp()->getStaticTooltipMenu(), parameter.c_str(), nullptr, GUIDesignLabelThickedFixed(0));
    myParameterLabel->setTipText(description.c_str());
}


GNEToolDialogElements::Argument::~Argument() {}


MFXLabelTooltip*
GNEToolDialogElements::Argument::getParameterLabel() const {
    return myParameterLabel;
}


long
GNEToolDialogElements::Argument::XXXX(FXObject*, FXSelector, void*) {
    //
    return 1;
}


GNEToolDialogElements::Argument::Argument() {}

// ---------------------------------------------------------------------------
// GNEToolDialogElements::HeaderArgument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::HeaderArgument::HeaderArgument(GNEToolDialog* toolDialogParent) :
    Argument(toolDialogParent, "Parameter", "", "") {
    // create text field for filename
    new MFXLabelTooltip(this, toolDialogParent->getGNEApp()->getStaticTooltipMenu(), TL("Value"), nullptr, GUIDesignLabelThickedFixed(50));
}


void
GNEToolDialogElements::HeaderArgument::reset() {
    // nothing to reset
}


const std::string
GNEToolDialogElements::HeaderArgument::getValue() const {
    return "";
}

// ---------------------------------------------------------------------------
// GNEToolDialogElements::FileNameArgument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::FileNameArgument::FileNameArgument(GNEToolDialog* toolDialogParent, 
        const std::string name, const Option* option) :
    Argument(toolDialogParent, name, option->getValueString(), option->getDescription()) {
    // Create Open button
    myFilenameButton = new FXButton(this, (std::string("\t\t") + TL("Select filename")).c_str(), 
        GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, FXDialogBox::ID_ACCEPT, GUIDesignButtonIcon);
    // create text field for filename
    myFilenameTextField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldFixed(180));
    // reset after creation
    reset();
}


void
GNEToolDialogElements::FileNameArgument::reset() {
    myFilenameTextField->setText(myDefaultValue.c_str());
}


long
GNEToolDialogElements::FileNameArgument::onCmdSetFilenameValue(FXObject*, FXSelector, void*) {
    return 1;
}


GNEToolDialogElements::FileNameArgument::FileNameArgument() {}


const std::string
GNEToolDialogElements::FileNameArgument::getValue() const {
    return myFilenameTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEToolDialogElements::StringArgument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::StringArgument::StringArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option) :
    Argument(toolDialogParent, name, option->getValueString(), option->getDescription()) {
    // create text field for filename
    myStringTextField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldFixedRestricted(180, 0));
    // reset after creation
    reset();
}


void
GNEToolDialogElements::StringArgument::reset() {
    myStringTextField->setText(myDefaultValue.c_str());
}


long
GNEToolDialogElements::StringArgument::onCmdSetStringValue(FXObject*, FXSelector, void*) {
    return 1;
}


GNEToolDialogElements::StringArgument::StringArgument() {}


const std::string
GNEToolDialogElements::StringArgument::getValue() const {
    return myStringTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEToolDialogElements::IntArgument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::IntArgument::IntArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option) :
    Argument(toolDialogParent, name, option->getValueString(), option->getDescription()) {
    // create text field for filename
    myIntTextField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldFixedRestricted(180, TEXTFIELD_INTEGER));
    // reset after creation
    reset();
}


void
GNEToolDialogElements::IntArgument::reset() {
    myIntTextField->setText(myDefaultValue.c_str());
}


long
GNEToolDialogElements::IntArgument::onCmdSetIntValue(FXObject*, FXSelector, void*) {
    return 1;
}


GNEToolDialogElements::IntArgument::IntArgument() {}


const std::string
GNEToolDialogElements::IntArgument::getValue() const {
    return myIntTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEToolDialogElements::FloatArgument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::FloatArgument::FloatArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option) :
    Argument(toolDialogParent, name, option->getValueString(), option->getDescription()) {
    // create text field for filename
    myFloatTextField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldFixedRestricted(180, TEXTFIELD_REAL));
    // reset after creation
    reset();
}


void
GNEToolDialogElements::FloatArgument::reset() {
    myFloatTextField->setText(myDefaultValue.c_str());
}


long
GNEToolDialogElements::FloatArgument::onCmdSetFloatValue(FXObject*, FXSelector, void*) {
    return 1;
}


GNEToolDialogElements::FloatArgument::FloatArgument() {}


const std::string
GNEToolDialogElements::FloatArgument::getValue() const {
    return myFloatTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEToolDialogElements::BoolArgument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::BoolArgument::BoolArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option) :
    Argument(toolDialogParent, name, option->getValueString(), option->getDescription()) {
    // create text field for filename
    myBoolTextField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldFixedRestricted(180, TEXTFIELD_REAL));
    // reset after creation
    reset();
}


void
GNEToolDialogElements::BoolArgument::reset() {
    myBoolTextField->setText(myDefaultValue.c_str());
}


long
GNEToolDialogElements::BoolArgument::onCmdSetBoolValue(FXObject*, FXSelector, void*) {
    return 1;
}


GNEToolDialogElements::BoolArgument::BoolArgument() {}


const std::string
GNEToolDialogElements::BoolArgument::getValue() const {
    return myBoolTextField->getText().text();
}

/****************************************************************************/
