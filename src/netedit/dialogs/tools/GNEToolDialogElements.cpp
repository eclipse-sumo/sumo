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
#include <netedit/elements/GNEAttributeCarrier.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEToolDialogElements.h"
#include "GNEToolDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEToolDialogElements::Argument) ArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEToolDialogElements::Argument::onCmdSetValue),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RESET,          GNEToolDialogElements::Argument::onCmdResetValue),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_RESET,          GNEToolDialogElements::Argument::onUpdResetValue),
};

FXDEFMAP(GNEToolDialogElements::FileNameArgument) FileNameArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,  GNEToolDialogElements::FileNameArgument::onCmdOpenFilename),
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNEToolDialogElements::Argument,   FXHorizontalFrame,                  ArgumentMap,            ARRAYNUMBER(ArgumentMap))
FXIMPLEMENT(GNEToolDialogElements::FileNameArgument,    GNEToolDialogElements::Argument,    FileNameArgumentMap,    ARRAYNUMBER(FileNameArgumentMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEToolDialogElements::Argument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::Argument::Argument(GNEToolDialog* toolDialogParent, const std::string &parameter, const Option* option) :
    FXHorizontalFrame(toolDialogParent->getContentFrame(), GUIDesignAuxiliarHorizontalFrame),
    myDefaultValue((option->getValueString() == "None")? "" : option->getValueString()) {
    // create parameter label
    myParameterLabel = new MFXLabelTooltip(this, toolDialogParent->getGNEApp()->getStaticTooltipMenu(), parameter.c_str(), nullptr, GUIDesignLabelThickedFixed(0));
    myParameterLabel->setTipText((option->getTypeName() + ": " + option->getDescription()).c_str());
    // create horizontal frame for textField
    myAuxiliarTextFieldFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarFrameFixedWidth(250));
    // Create reset button
    myResetButton = new FXButton(this, (std::string("\t\t") + TL("Reset value")).c_str(), GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_RESET, GUIDesignButtonIcon);
}


GNEToolDialogElements::Argument::~Argument() {}


MFXLabelTooltip*
GNEToolDialogElements::Argument::getParameterLabel() const {
    return myParameterLabel;
}


const std::string
GNEToolDialogElements::Argument::getArgument() const {
    if (getValue() != myDefaultValue) {
        return ("-" + std::string(myParameterLabel->getText().text()) + " " + getValue() + " ");
    } else {
        return "";
    }
}


long
GNEToolDialogElements::Argument::onCmdResetValue(FXObject*, FXSelector, void*) {
    // just reset value
    reset();
    return 1;
}


long
GNEToolDialogElements::Argument::onUpdResetValue(FXObject*, FXSelector, void*) {
    if (getValue() != myDefaultValue) {
        return myResetButton->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return myResetButton->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}


GNEToolDialogElements::Argument::Argument() {}

// ---------------------------------------------------------------------------
// GNEToolDialogElements::FileNameArgument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::FileNameArgument::FileNameArgument(GNEToolDialog* toolDialogParent, 
        const std::string name, const Option* option) :
    Argument(toolDialogParent, name, option) {
    // Create Open button
    myFilenameButton = new FXButton(myAuxiliarTextFieldFrame, (std::string("\t\t") + TL("Select filename")).c_str(), 
        GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, MID_GNE_SELECT, GUIDesignButtonIcon);
    // create text field for filename
    myFilenameTextField = new FXTextField(myAuxiliarTextFieldFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // reset after creation
    reset();
}


void
GNEToolDialogElements::FileNameArgument::reset() {
    myFilenameTextField->setText(myDefaultValue.c_str());
}


long
GNEToolDialogElements::FileNameArgument::onCmdOpenFilename(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEToolDialogElements::FileNameArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
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
    Argument(toolDialogParent, name, option) {
    // create text field for string
    myStringTextField = new FXTextField(myAuxiliarTextFieldFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // reset after creation
    reset();
}


void
GNEToolDialogElements::StringArgument::reset() {
    myStringTextField->setText(myDefaultValue.c_str());
}


long
GNEToolDialogElements::StringArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    return 1;
}


const std::string
GNEToolDialogElements::StringArgument::getValue() const {
    return myStringTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEToolDialogElements::IntArgument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::IntArgument::IntArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option) :
    Argument(toolDialogParent, name, option) {
    // create text field for int
    myIntTextField = new FXTextField(myAuxiliarTextFieldFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_INTEGER));
    // reset after creation
    reset();
}


void
GNEToolDialogElements::IntArgument::reset() {
    myIntTextField->setText(myDefaultValue.c_str());
}


long
GNEToolDialogElements::IntArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    return 1;
}


const std::string
GNEToolDialogElements::IntArgument::getValue() const {
    return myIntTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEToolDialogElements::FloatArgument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::FloatArgument::FloatArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option) :
    Argument(toolDialogParent, name, option) {
    // create text field for float
    myFloatTextField = new FXTextField(myAuxiliarTextFieldFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    // reset after creation
    reset();
}


void
GNEToolDialogElements::FloatArgument::reset() {
    myFloatTextField->setText(myDefaultValue.c_str());
}


long
GNEToolDialogElements::FloatArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    return 1;
}


const std::string
GNEToolDialogElements::FloatArgument::getValue() const {
    return myFloatTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEToolDialogElements::BoolArgument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::BoolArgument::BoolArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option) :
    Argument(toolDialogParent, name, option) {
    // create check button
    myCheckButton = new FXCheckButton(myAuxiliarTextFieldFrame, "" , this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    // reset after creation
    reset();
}


void
GNEToolDialogElements::BoolArgument::reset() {
    if (GNEAttributeCarrier::parse<bool>(myDefaultValue)) {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
}


long
GNEToolDialogElements::BoolArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    if (myCheckButton->getCheck() == TRUE) {
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setText(TL("false"));
    }
    return 1;
}


const std::string
GNEToolDialogElements::BoolArgument::getValue() const {
    if (myCheckButton->getCheck() == TRUE) {
        return "true";
    } else {
        return "false";
    }
}

/****************************************************************************/
