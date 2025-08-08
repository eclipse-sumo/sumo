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
/// @file    GNEOptionsEditorRow.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// Row used in GNEOptionsEditor to edit options
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/foxtools/MFXLabelTooltip.h>

#include "GNEOptionsEditorRow.h"
#include "GNEOptionsDialog.h"

// ===========================================================================
// Defines
// ===========================================================================

#define MARGIN 4
#define MINNAMEWIDTH 200

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEOptionsEditorRow::OptionRow) OptionRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEOptionsEditorRow::OptionRow::onCmdSetOption),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RESET,          GNEOptionsEditorRow::OptionRow::onCmdResetOption),
};

FXDEFMAP(GNEOptionsEditorRow::OptionFilename) OptionFilenameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,  GNEOptionsEditorRow::OptionFilename::onCmdOpenDialog),
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNEOptionsEditorRow::OptionRow,        FXHorizontalFrame,              OptionRowMap,       ARRAYNUMBER(OptionRowMap))
FXIMPLEMENT_ABSTRACT(GNEOptionsEditorRow::OptionFilename,   GNEOptionsEditorRow::OptionRow, OptionFilenameMap,  ARRAYNUMBER(OptionFilenameMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEOptionsEditorRow::OptionRow - methods
// ---------------------------------------------------------------------------

GNEOptionsEditorRow::OptionRow::OptionRow(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
        const std::string& name, const std::string& description, const std::string& defaultValue) :
    FXHorizontalFrame(parent, GUIDesignAuxiliarHorizontalFrame),
    myGUIDialogOptions(GUIDialogOptions),
    myTopic(topic),
    myName(name),
    myDescription(description),
    myDefaultValue(defaultValue) {
    // build label with name (default width 150)
    myNameLabel = new MFXLabelTooltip(this, myGUIDialogOptions->getApplicationWindow()->getStaticTooltipMenu(), name.c_str(), nullptr, GUIDesignLabelThickedFixed(MINNAMEWIDTH));
    // set description as tooltip
    myNameLabel->setTipText(description.c_str());
    // create content frame
    myContentFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // Create reset button
    myResetButton = GUIDesigns::buildFXButton(this, "", "", TL("Reset value"), GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_RESET, GUIDesignButtonIcon);
}


void
GNEOptionsEditorRow::OptionRow::adjustNameSize() {
    const int nameWidth = myNameLabel->getFont()->getTextWidth(myNameLabel->getText().text(), myNameLabel->getText().length() + MARGIN);
    if (nameWidth > MINNAMEWIDTH) {
        myNameLabel->setWidth(nameWidth);
    }
}


const std::string&
GNEOptionsEditorRow::OptionRow::getTopic() const {
    return myTopic;
}


const std::string
GNEOptionsEditorRow::OptionRow::getNameLower() const {
    return StringUtils::to_lower_case(myName);
}


const std::string
GNEOptionsEditorRow::OptionRow::getDescriptionLower() const {
    return StringUtils::to_lower_case(myDescription);
}


void
GNEOptionsEditorRow::OptionRow::updateResetButton() {
    if (getValue() != myDefaultValue) {
        myResetButton->enable();
    } else {
        myResetButton->disable();
    }
}

// ---------------------------------------------------------------------------
// GNEOptionsEditorRow::OptionString - methods
// ---------------------------------------------------------------------------

GNEOptionsEditorRow::OptionString::OptionString(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description, const std::string& defaultValue) :
    OptionRow(GUIDialogOptions, parent, topic, name, description, defaultValue) {
    myStringTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myStringTextField->setText(myGUIDialogOptions->myOptionsContainer.getString(name).c_str());
    updateOption();
}


void
GNEOptionsEditorRow::OptionString::updateOption() {
    myStringTextField->setText(myGUIDialogOptions->myOptionsContainer.getString(myName).c_str());
}


void
GNEOptionsEditorRow::OptionString::restoreOption() {
    myStringTextField->setText(myGUIDialogOptions->myOriginalOptionsContainer.getString(myName).c_str());
}


long
GNEOptionsEditorRow::OptionString::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer.resetWritable();
    myGUIDialogOptions->myOptionsContainer.set(myName, myStringTextField->getText().text());
    myGUIDialogOptions->myOptionsModified = true;
    updateResetButton();
    return 1;
}


long
GNEOptionsEditorRow::OptionString::onCmdResetOption(FXObject*, FXSelector, void*) {
    myStringTextField->setText(myDefaultValue.c_str());
    updateResetButton();
    return 1;
}


std::string
GNEOptionsEditorRow::OptionString::getValue() const {
    return myStringTextField->getText().text();
}


GNEOptionsEditorRow::OptionStringVector::OptionStringVector(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description, const std::string& defaultValue) :
    OptionRow(GUIDialogOptions, parent, topic, name, description, defaultValue) {
    myStringVectorTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    updateOption();
}


void
GNEOptionsEditorRow::OptionStringVector::updateOption() {
    myStringVectorTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getStringVector(myName)).c_str());
}


void
GNEOptionsEditorRow::OptionStringVector::restoreOption() {
    myStringVectorTextField->setText(toString(myGUIDialogOptions->myOriginalOptionsContainer.getStringVector(myName)).c_str());
}


long
GNEOptionsEditorRow::OptionStringVector::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer.resetWritable();
    myGUIDialogOptions->myOptionsContainer.set(myName, myStringVectorTextField->getText().text());
    myGUIDialogOptions->myOptionsModified = true;
    updateResetButton();
    return 1;
}


long
GNEOptionsEditorRow::OptionStringVector::onCmdResetOption(FXObject*, FXSelector, void*) {
    myStringVectorTextField->setText(myDefaultValue.c_str());
    updateResetButton();
    return 1;
}


std::string
GNEOptionsEditorRow::OptionStringVector::getValue() const {
    return myStringVectorTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEOptionsEditorRow::OptionBool - methods
// ---------------------------------------------------------------------------

GNEOptionsEditorRow::OptionBool::OptionBool(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description, const std::string& defaultValue) :
    OptionRow(GUIDialogOptions, parent, topic, name, description, defaultValue) {
    myCheckButton = new FXCheckButton(myContentFrame, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    updateOption();
}


void
GNEOptionsEditorRow::OptionBool::updateOption() {
    if (myGUIDialogOptions->myOptionsContainer.getBool(myName)) {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
}


void
GNEOptionsEditorRow::OptionBool::restoreOption() {
    if (myGUIDialogOptions->myOriginalOptionsContainer.getBool(myName)) {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
}


long
GNEOptionsEditorRow::OptionBool::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer.resetWritable();
    if (myCheckButton->getCheck()) {
        myGUIDialogOptions->myOptionsContainer.set(myName, "true");
        myCheckButton->setText(TL("true"));
    } else {
        myGUIDialogOptions->myOptionsContainer.set(myName, "false");
        myCheckButton->setText(TL("false"));
    }
    myGUIDialogOptions->myOptionsModified = true;
    // special checks for Debug flags
    if ((myName == "gui-testing-debug") && myGUIDialogOptions->myOptionsContainer.isSet("gui-testing-debug")) {
        MsgHandler::enableDebugMessages(myGUIDialogOptions->myOptionsContainer.getBool("gui-testing-debug"));
    }
    if ((myName == "gui-testing-debug-gl") && myGUIDialogOptions->myOptionsContainer.isSet("gui-testing-debug-gl")) {
        MsgHandler::enableDebugGLMessages(myGUIDialogOptions->myOptionsContainer.getBool("gui-testing-debug-gl"));
    }
    updateResetButton();
    return 1;
}


long
GNEOptionsEditorRow::OptionBool::onCmdResetOption(FXObject*, FXSelector, void*) {
    if (StringUtils::toBool(myDefaultValue)) {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
    updateResetButton();
    return 1;
}


std::string
GNEOptionsEditorRow::OptionBool::getValue() const {
    return myCheckButton->getCheck() ? "true" : "false";
}

// ---------------------------------------------------------------------------
// GNEOptionsEditorRow::OptionInt - methods
// ---------------------------------------------------------------------------

GNEOptionsEditorRow::OptionInt::OptionInt(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description, const std::string& defaultValue) :
    OptionRow(GUIDialogOptions, parent, topic, name, description, defaultValue) {
    myIntTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_INTEGER));
    updateOption();
}


void
GNEOptionsEditorRow::OptionInt::updateOption() {
    myIntTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getInt(myName)).c_str());
}


void
GNEOptionsEditorRow::OptionInt::restoreOption() {
    myIntTextField->setText(toString(myGUIDialogOptions->myOriginalOptionsContainer.getInt(myName)).c_str());
}


long
GNEOptionsEditorRow::OptionInt::onCmdSetOption(FXObject*, FXSelector, void*) {
    if (myIntTextField->getText().empty()) {
        myIntTextField->setText(myDefaultValue.c_str());
    } else {
        myGUIDialogOptions->myOptionsContainer.resetWritable();
        myGUIDialogOptions->myOptionsContainer.set(myName, myIntTextField->getText().text());
        myGUIDialogOptions->myOptionsModified = true;
    }
    updateResetButton();
    return 1;
}


long
GNEOptionsEditorRow::OptionInt::onCmdResetOption(FXObject*, FXSelector, void*) {
    myIntTextField->setText(myDefaultValue.c_str());
    updateResetButton();
    return 1;
}


std::string
GNEOptionsEditorRow::OptionInt::getValue() const {
    return myIntTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEOptionsEditorRow::OptionIntVector - methods
// ---------------------------------------------------------------------------

GNEOptionsEditorRow::OptionIntVector::OptionIntVector(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description, const std::string& defaultValue) :
    OptionRow(GUIDialogOptions, parent, topic, name, description, defaultValue) {
    myIntVectorTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myIntVectorTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getIntVector(name)).c_str());
    updateOption();
}


void
GNEOptionsEditorRow::OptionIntVector::updateOption() {
    myIntVectorTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getIntVector(myName)).c_str());
}


void
GNEOptionsEditorRow::OptionIntVector::restoreOption() {
    myIntVectorTextField->setText(toString(myGUIDialogOptions->myOriginalOptionsContainer.getIntVector(myName)).c_str());
}


long
GNEOptionsEditorRow::OptionIntVector::onCmdSetOption(FXObject*, FXSelector, void*) {
    try {
        // check that int vector can be parsed
        const auto intVector = StringTokenizer(myIntVectorTextField->getText().text()).getVector();
        for (const auto& intValue : intVector) {
            StringUtils::toInt(intValue);
        }
        myGUIDialogOptions->myOptionsContainer.resetWritable();
        myGUIDialogOptions->myOptionsContainer.set(myName, myIntVectorTextField->getText().text());
        myIntVectorTextField->setTextColor(FXRGB(0, 0, 0));
        myGUIDialogOptions->myOptionsModified = true;
    } catch (...) {
        myIntVectorTextField->setTextColor(FXRGB(255, 0, 0));
    }
    updateResetButton();
    return 1;
}


long
GNEOptionsEditorRow::OptionIntVector::onCmdResetOption(FXObject*, FXSelector, void*) {
    myIntVectorTextField->setText(myDefaultValue.c_str());
    updateResetButton();
    return 1;
}


std::string
GNEOptionsEditorRow::OptionIntVector::getValue() const {
    return myIntVectorTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEOptionsEditorRow::OptionFloat - methods
// ---------------------------------------------------------------------------

GNEOptionsEditorRow::OptionFloat::OptionFloat(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
        const std::string& name, const std::string& description, const std::string& defaultValue) :
    OptionRow(GUIDialogOptions, parent, topic, name, description, parseFloat(defaultValue)) {
    myFloatTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myFloatTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getFloat(name)).c_str());
    updateOption();
}


void
GNEOptionsEditorRow::OptionFloat::updateOption() {
    myFloatTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getFloat(myName)).c_str());
}


void
GNEOptionsEditorRow::OptionFloat::restoreOption() {
    myFloatTextField->setText(toString(myGUIDialogOptions->myOriginalOptionsContainer.getFloat(myName)).c_str());
}


long
GNEOptionsEditorRow::OptionFloat::onCmdSetOption(FXObject*, FXSelector, void*) {
    // avoid empty values
    if (myFloatTextField->getText().empty()) {
        myFloatTextField->setText(myDefaultValue.c_str());
    } else {
        myGUIDialogOptions->myOptionsContainer.resetWritable();
        myGUIDialogOptions->myOptionsContainer.set(myName, myFloatTextField->getText().text());
        myGUIDialogOptions->myOptionsModified = true;
    }
    updateResetButton();
    return 1;
}


long
GNEOptionsEditorRow::OptionFloat::onCmdResetOption(FXObject*, FXSelector, void*) {
    myFloatTextField->setText(myDefaultValue.c_str());
    updateResetButton();
    return 1;
}


std::string
GNEOptionsEditorRow::OptionFloat::getValue() const {
    return myFloatTextField->getText().text();
}


std::string
GNEOptionsEditorRow::OptionFloat::parseFloat(const std::string& value) const {
    try {
        return toString(StringUtils::toDouble(value));
    } catch (...) {
        return value;
    }
}

// ---------------------------------------------------------------------------
// GNEOptionsEditorRow::OptionTime - methods
// ---------------------------------------------------------------------------

GNEOptionsEditorRow::OptionTime::OptionTime(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
        const std::string& name, const std::string& description, const std::string& defaultValue) :
    OptionRow(GUIDialogOptions, parent, topic, name, description, parseTime(defaultValue)) {
    myTimeTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTimeTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getString(name)).c_str());
    updateOption();
}


void
GNEOptionsEditorRow::OptionTime::updateOption() {
    myTimeTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getString(myName)).c_str());
}


void
GNEOptionsEditorRow::OptionTime::restoreOption() {
    myTimeTextField->setText(toString(myGUIDialogOptions->myOriginalOptionsContainer.getString(myName)).c_str());
}


long
GNEOptionsEditorRow::OptionTime::onCmdSetOption(FXObject*, FXSelector, void*) {
    // avoid empty values
    if (myTimeTextField->getText().empty()) {
        myTimeTextField->setText(myDefaultValue.c_str());
    } else {
        myGUIDialogOptions->myOptionsContainer.resetWritable();
        myGUIDialogOptions->myOptionsContainer.set(myName, myTimeTextField->getText().text());
        myGUIDialogOptions->myOptionsModified = true;
    }
    updateResetButton();
    return 1;
}


long
GNEOptionsEditorRow::OptionTime::onCmdResetOption(FXObject*, FXSelector, void*) {
    myTimeTextField->setText(myDefaultValue.c_str());
    updateResetButton();
    return 1;
}


std::string
GNEOptionsEditorRow::OptionTime::getValue() const {
    return myTimeTextField->getText().text();
}


std::string
GNEOptionsEditorRow::OptionTime::parseTime(const std::string& value) const {
    try {
        return time2string(string2time(value));
    } catch (...) {
        return value;
    }
}

// ---------------------------------------------------------------------------
// GNEOptionsEditorRow::OptionFilename - methods
// ---------------------------------------------------------------------------

GNEOptionsEditorRow::OptionFilename::OptionFilename(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
        const std::string& name, const std::string& description, const std::string& defaultValue) :
    OptionRow(GUIDialogOptions, parent, topic, name, description, defaultValue) {
    myOpenFilenameButton = GUIDesigns::buildFXButton(myContentFrame, "", "", TL("Select filename"),
                           GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonIcon);
    myFilenameTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    updateOption();
}


void
GNEOptionsEditorRow::OptionFilename::updateOption() {
    myFilenameTextField->setText(myGUIDialogOptions->myOptionsContainer.getString(myName).c_str());
}


void
GNEOptionsEditorRow::OptionFilename::restoreOption() {
    myFilenameTextField->setText(myGUIDialogOptions->myOriginalOptionsContainer.getString(myName).c_str());
}


long
GNEOptionsEditorRow::OptionFilename::onCmdOpenDialog(FXObject*, FXSelector, void*) {
    // get file
    const auto file = GNEApplicationWindowHelper::openXMLFileDialog(this, (myName.find("output") != std::string::npos), false);
    // check that file is valid
    if (file.size() > 0) {
        myFilenameTextField->setText(file.c_str(), TRUE);
    }
    return 1;
}


long
GNEOptionsEditorRow::OptionFilename::onCmdSetOption(FXObject*, FXSelector, void*) {
    if (SUMOXMLDefinitions::isValidFilename(myFilenameTextField->getText().text())) {
        myGUIDialogOptions->myOptionsContainer.resetWritable();
        myGUIDialogOptions->myOptionsContainer.set(myName, myFilenameTextField->getText().text());
        myFilenameTextField->setTextColor(FXRGB(0, 0, 0));
        myGUIDialogOptions->myOptionsModified = true;
    } else {
        myFilenameTextField->setTextColor(FXRGB(255, 0, 0));
    }
    updateResetButton();
    return 1;
}


long
GNEOptionsEditorRow::OptionFilename::onCmdResetOption(FXObject*, FXSelector, void*) {
    myFilenameTextField->setText(myDefaultValue.c_str());
    updateResetButton();
    return 1;
}


GNEOptionsEditorRow::OptionFilename::OptionFilename() {}


std::string
GNEOptionsEditorRow::OptionFilename::getValue() const {
    return myFilenameTextField->getText().text();
}

/****************************************************************************/
