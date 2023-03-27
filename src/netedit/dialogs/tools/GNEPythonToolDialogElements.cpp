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
/// @file    GNEPythonToolDialogElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Elements used in GNEPythonToolDialog
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEPythonToolDialogElements.h"
#include "GNEPythonToolDialog.h"


// ===========================================================================
// static members
// ===========================================================================

const std::string GNEPythonToolDialogElements::IntArgument::INVALID_INT_STR = toString(INVALID_INT);
const std::string GNEPythonToolDialogElements::FloatArgument::INVALID_DOUBLE_STR = toString(INVALID_DOUBLE);

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEPythonToolDialogElements::Argument) ArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEPythonToolDialogElements::Argument::onCmdSetValue),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RESET,          GNEPythonToolDialogElements::Argument::onCmdResetValue),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_RESET,          GNEPythonToolDialogElements::Argument::onUpdResetValue),
};

FXDEFMAP(GNEPythonToolDialogElements::FileNameArgument) FileNameArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,  GNEPythonToolDialogElements::FileNameArgument::onCmdOpenFilename),
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNEPythonToolDialogElements::Argument,   FXHorizontalFrame,                  ArgumentMap,            ARRAYNUMBER(ArgumentMap))
FXIMPLEMENT(GNEPythonToolDialogElements::FileNameArgument,    GNEPythonToolDialogElements::Argument,    FileNameArgumentMap,    ARRAYNUMBER(FileNameArgumentMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::Category - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::Category::Category(GNEPythonToolDialog* toolDialogParent, const std::string &category) :
    FXHorizontalFrame(toolDialogParent->getRowFrame(), GUIDesignAuxiliarHorizontalFrame) {
    // create category label
    new FXLabel(this, category.c_str(), nullptr, GUIDesignLabel(JUSTIFY_NORMAL));
    // create category
    create();
}


GNEPythonToolDialogElements::Category::~Category() {}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::Argument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::Argument::Argument(GNEPythonToolDialog* toolDialogParent, const std::string &parameter, Option* option) :
    FXHorizontalFrame(toolDialogParent->getRowFrame(), GUIDesignAuxiliarHorizontalFrame),
    myOption(option),
    myDefaultValue((option->getValueString() == "None")? "" : option->getValueString()) {
    // create parameter label
    myParameterLabel = new MFXLabelTooltip(this, toolDialogParent->myGNEApp->getStaticTooltipMenu(), parameter.c_str(), nullptr, GUIDesignLabelThickedFixed(0));
    myParameterLabel->setTipText((option->getTypeName() + ": " + option->getDescription()).c_str());
    // create horizontal frame for textField
    myAuxiliarTextFieldFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarFrameFixedWidth(250));
    // Create reset button
    myResetButton = new FXButton(this, (std::string("\t\t") + TL("Reset value")).c_str(), GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_RESET, GUIDesignButtonIcon);
    // create argument
    create();
}


GNEPythonToolDialogElements::Argument::~Argument() {}


MFXLabelTooltip*
GNEPythonToolDialogElements::Argument::getParameterLabel() const {
    return myParameterLabel;
}


const std::string
GNEPythonToolDialogElements::Argument::getArgument() const {
    if (getValue() != myDefaultValue) {
        return ("-" + std::string(myParameterLabel->getText().text()) + " " + getValue() + " ");
    } else {
        return "";
    }
}


long
GNEPythonToolDialogElements::Argument::onCmdResetValue(FXObject*, FXSelector, void*) {
    // just reset value
    reset();
    return 1;
}


long
GNEPythonToolDialogElements::Argument::onUpdResetValue(FXObject*, FXSelector, void*) {
    if (getValue() != myDefaultValue) {
        return myResetButton->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return myResetButton->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}


GNEPythonToolDialogElements::Argument::Argument() {}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::FileNameArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::FileNameArgument::FileNameArgument(GNEPythonToolDialog* toolDialogParent, 
        const std::string name, Option* option) :
    Argument(toolDialogParent, name, option) {
    // Create Open button
    myFilenameButton = new FXButton(myAuxiliarTextFieldFrame, (std::string("\t\t") + TL("Select filename")).c_str(), 
        GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, MID_GNE_SELECT, GUIDesignButtonIcon);
    myFilenameButton->create();
    // create text field for filename
    myFilenameTextField = new FXTextField(myAuxiliarTextFieldFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myFilenameTextField->create();
    // reset after creation
    reset();
}


void
GNEPythonToolDialogElements::FileNameArgument::reset() {
    myFilenameTextField->setText(myDefaultValue.c_str());
    myOption->set(myDefaultValue, myDefaultValue, false);
    myOption->resetDefault();
}


long
GNEPythonToolDialogElements::FileNameArgument::onCmdOpenFilename(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEPythonToolDialogElements::FileNameArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    myOption->resetWritable();
    if (myFilenameTextField->getText().empty()) {
        reset();
    } else {
        myOption->set(myFilenameTextField->getText().text(), myFilenameTextField->getText().text(), false);
    }
    return 1;
}


GNEPythonToolDialogElements::FileNameArgument::FileNameArgument() {}


const std::string
GNEPythonToolDialogElements::FileNameArgument::getValue() const {
    return myFilenameTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::StringArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::StringArgument::StringArgument(GNEPythonToolDialog* toolDialogParent, const std::string name, Option* option) :
    Argument(toolDialogParent, name, option) {
    // create text field for string
    myStringTextField = new FXTextField(myAuxiliarTextFieldFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myStringTextField->create();
    // reset after creation
    reset();
}


void
GNEPythonToolDialogElements::StringArgument::reset() {
    myStringTextField->setText(myDefaultValue.c_str());
    myOption->set(myDefaultValue, myDefaultValue, false);
    myOption->resetDefault();
}


long
GNEPythonToolDialogElements::StringArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    myOption->resetWritable();
    if (myStringTextField->getText().empty()) {
        reset();
    } else {
        myOption->set(myStringTextField->getText().text(), myStringTextField->getText().text(), false);
    }
    return 1;
}


const std::string
GNEPythonToolDialogElements::StringArgument::getValue() const {
    return myStringTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::IntArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::IntArgument::IntArgument(GNEPythonToolDialog* toolDialogParent, const std::string name, Option* option) :
    Argument(toolDialogParent, name, option) {
    // create text field for int
    myIntTextField = new FXTextField(myAuxiliarTextFieldFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_INTEGER));
    myIntTextField->create();
    // reset after creation
    reset();
}


void
GNEPythonToolDialogElements::IntArgument::reset() {
    myIntTextField->setText(myDefaultValue.c_str());
    if (myDefaultValue.empty()) {
        myOption->set(INVALID_INT_STR, INVALID_INT_STR, false);
    } else {
        myOption->set(myDefaultValue, myDefaultValue, false);
    }
    myOption->resetDefault();
}


long
GNEPythonToolDialogElements::IntArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    myOption->resetWritable();
    if (myIntTextField->getText().empty()) {
        reset();
    } else {
        myOption->set(myIntTextField->getText().text(), myIntTextField->getText().text(), false);
    }
    return 1;
}


const std::string
GNEPythonToolDialogElements::IntArgument::getValue() const {
    return myIntTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::FloatArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::FloatArgument::FloatArgument(GNEPythonToolDialog* toolDialogParent, const std::string name, Option* option) :
    Argument(toolDialogParent, name, option) {
    // create text field for float
    myFloatTextField = new FXTextField(myAuxiliarTextFieldFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myFloatTextField->create();
    // reset after creation
    reset();
}


void
GNEPythonToolDialogElements::FloatArgument::reset() {
    myFloatTextField->setText(myDefaultValue.c_str());
    if (myDefaultValue.empty()) {
        myOption->set(INVALID_DOUBLE_STR, INVALID_DOUBLE_STR, false);
    } else {
        myOption->set(myDefaultValue, myDefaultValue, false);
    }
    myOption->resetDefault();
}


long
GNEPythonToolDialogElements::FloatArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    myOption->resetWritable();
    if (myFloatTextField->getText().empty()) {
        reset();
    } else {
        myOption->set(myFloatTextField->getText().text(), myFloatTextField->getText().text(), false);
    }
    return 1;
}


const std::string
GNEPythonToolDialogElements::FloatArgument::getValue() const {
    return myFloatTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::BoolArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::BoolArgument::BoolArgument(GNEPythonToolDialog* toolDialogParent, const std::string name, Option* option) :
    Argument(toolDialogParent, name, option) {
    // create check button
    myCheckButton = new FXCheckButton(myAuxiliarTextFieldFrame, "" , this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myCheckButton->create();
    // reset after creation
    reset();
}


void
GNEPythonToolDialogElements::BoolArgument::reset() {
    if (GNEAttributeCarrier::parse<bool>(myDefaultValue)) {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
    myOption->set(myDefaultValue, myDefaultValue, false);
    myOption->resetDefault();
}


long
GNEPythonToolDialogElements::BoolArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    myOption->resetWritable();
    if (myCheckButton->getCheck() == TRUE) {
        myCheckButton->setText(TL("true"));
        myOption->set("true", "true", false);
    } else {
        myCheckButton->setText(TL("false"));
        myOption->set("false", "false", false);
    }
    return 1;
}


const std::string
GNEPythonToolDialogElements::BoolArgument::getValue() const {
    if (myCheckButton->getCheck() == TRUE) {
        return "true";
    } else {
        return "false";
    }
}

/****************************************************************************/
