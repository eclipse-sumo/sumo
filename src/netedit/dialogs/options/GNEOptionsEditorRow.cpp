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
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>
#include <netedit/GNEApplicationWindow.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEOptionsEditorRow.h"
#include "GNEOptionsEditor.h"

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

GNEOptionsEditorRow::OptionRow::OptionRow(GNEOptionsEditor* optionsEditor, FXComposite* parent, const std::string& topic,
        const std::string& name, const std::string& description, const std::string& defaultValue, const bool editable) :
    FXHorizontalFrame(parent, GUIDesignAuxiliarHorizontalFrame),
    myOptionsEditor(optionsEditor),
    myTopic(topic),
    myName(name),
    myDescription(description),
    myDefaultValue(defaultValue),
    myEditable(editable) {
    // build label with name (default width 150)
    myNameLabel = new MFXLabelTooltip(this, myOptionsEditor->myDialog->getApplicationWindow()->getStaticTooltipMenu(),
                                      name.c_str(), nullptr, GUIDesignLabelThickedFixed(MINNAMEWIDTH));
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

GNEOptionsEditorRow::OptionString::OptionString(GNEOptionsEditor* optionsEditor, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description,
        const std::string& defaultValue, const bool editable) :
    OptionRow(optionsEditor, parent, topic, name, description, defaultValue, editable) {
    myStringTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myStringTextField->setText(myOptionsEditor->myOptionsContainer.getString(name).c_str());
    if (!editable) {
        myStringTextField->disable();
    }
    updateOption();
}


void
GNEOptionsEditorRow::OptionString::updateOption() {
    myStringTextField->setText(myOptionsEditor->myOptionsContainer.getString(myName).c_str());
    updateResetButton();
}


void
GNEOptionsEditorRow::OptionString::restoreOption() {
    myStringTextField->setText(myOptionsEditor->myOriginalOptionsContainer.getString(myName).c_str());
    onCmdSetOption(nullptr, 0, nullptr);
}


long
GNEOptionsEditorRow::OptionString::onCmdSetOption(FXObject*, FXSelector, void*) {
    myOptionsEditor->myOptionsContainer.resetWritable();
    myOptionsEditor->myOptionsContainer.set(myName, myStringTextField->getText().text());
    myOptionsEditor->myOptionsModified = true;
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


GNEOptionsEditorRow::OptionStringVector::OptionStringVector(GNEOptionsEditor* optionsEditor, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description,
        const std::string& defaultValue, const bool editable) :
    OptionRow(optionsEditor, parent, topic, name, description, defaultValue, editable) {
    myStringVectorTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    if (!editable) {
        myStringVectorTextField->disable();
    }
    updateOption();
}


void
GNEOptionsEditorRow::OptionStringVector::updateOption() {
    myStringVectorTextField->setText(toString(myOptionsEditor->myOptionsContainer.getStringVector(myName)).c_str());
    updateResetButton();
}


void
GNEOptionsEditorRow::OptionStringVector::restoreOption() {
    myStringVectorTextField->setText(toString(myOptionsEditor->myOriginalOptionsContainer.getStringVector(myName)).c_str());
    onCmdSetOption(nullptr, 0, nullptr);
}


long
GNEOptionsEditorRow::OptionStringVector::onCmdSetOption(FXObject*, FXSelector, void*) {
    myOptionsEditor->myOptionsContainer.resetWritable();
    myOptionsEditor->myOptionsContainer.set(myName, myStringVectorTextField->getText().text());
    myOptionsEditor->myOptionsModified = true;
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

GNEOptionsEditorRow::OptionBool::OptionBool(GNEOptionsEditor* optionsEditor, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description,
        const std::string& defaultValue, const bool editable) :
    OptionRow(optionsEditor, parent, topic, name, description, defaultValue, editable) {
    myCheckButton = new FXCheckButton(myContentFrame, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    if (!editable) {
        myCheckButton->disable();
    }
    updateOption();
}


void
GNEOptionsEditorRow::OptionBool::updateOption() {
    if (myOptionsEditor->myOptionsContainer.getBool(myName)) {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
    updateResetButton();
}


void
GNEOptionsEditorRow::OptionBool::restoreOption() {
    if (myOptionsEditor->myOriginalOptionsContainer.getBool(myName)) {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
    onCmdSetOption(nullptr, 0, nullptr);
}


long
GNEOptionsEditorRow::OptionBool::onCmdSetOption(FXObject*, FXSelector, void*) {
    myOptionsEditor->myOptionsContainer.resetWritable();
    if (myCheckButton->getCheck()) {
        myOptionsEditor->myOptionsContainer.set(myName, "true");
        myCheckButton->setText(TL("true"));
    } else {
        myOptionsEditor->myOptionsContainer.set(myName, "false");
        myCheckButton->setText(TL("false"));
    }
    myOptionsEditor->myOptionsModified = true;
    // special checks for Debug flags
    if ((myName == "gui-testing-debug") && myOptionsEditor->myOptionsContainer.isSet("gui-testing-debug")) {
        MsgHandler::enableDebugMessages(myOptionsEditor->myOptionsContainer.getBool("gui-testing-debug"));
    }
    if ((myName == "gui-testing-debug-gl") && myOptionsEditor->myOptionsContainer.isSet("gui-testing-debug-gl")) {
        MsgHandler::enableDebugGLMessages(myOptionsEditor->myOptionsContainer.getBool("gui-testing-debug-gl"));
    }
    updateResetButton();
    return 1;
}


long
GNEOptionsEditorRow::OptionBool::onCmdResetOption(FXObject*, FXSelector, void*) {
    if (myDefaultValue.empty()) {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    } else if (StringUtils::toBool(myDefaultValue)) {
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

GNEOptionsEditorRow::OptionInt::OptionInt(GNEOptionsEditor* optionsEditor, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description,
        const std::string& defaultValue, const bool editable) :
    OptionRow(optionsEditor, parent, topic, name, description, defaultValue, editable) {
    myIntTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_INTEGER));
    if (!editable) {
        myIntTextField->disable();
    }
    updateOption();
}


void
GNEOptionsEditorRow::OptionInt::updateOption() {
    myIntTextField->setText(toString(myOptionsEditor->myOptionsContainer.getInt(myName)).c_str());
    updateResetButton();
}


void
GNEOptionsEditorRow::OptionInt::restoreOption() {
    myIntTextField->setText(toString(myOptionsEditor->myOriginalOptionsContainer.getInt(myName)).c_str());
    onCmdSetOption(nullptr, 0, nullptr);
}


long
GNEOptionsEditorRow::OptionInt::onCmdSetOption(FXObject*, FXSelector, void*) {
    if (myIntTextField->getText().empty()) {
        myIntTextField->setText(myDefaultValue.c_str());
    } else {
        myOptionsEditor->myOptionsContainer.resetWritable();
        myOptionsEditor->myOptionsContainer.set(myName, myIntTextField->getText().text());
        myOptionsEditor->myOptionsModified = true;
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

GNEOptionsEditorRow::OptionIntVector::OptionIntVector(GNEOptionsEditor* optionsEditor, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description,
        const std::string& defaultValue, const bool editable) :
    OptionRow(optionsEditor, parent, topic, name, description, defaultValue, editable) {
    myIntVectorTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myIntVectorTextField->setText(toString(myOptionsEditor->myOptionsContainer.getIntVector(name)).c_str());
    if (!editable) {
        myIntVectorTextField->disable();
    }
    updateOption();
}


void
GNEOptionsEditorRow::OptionIntVector::updateOption() {
    myIntVectorTextField->setText(toString(myOptionsEditor->myOptionsContainer.getIntVector(myName)).c_str());
    updateResetButton();
}


void
GNEOptionsEditorRow::OptionIntVector::restoreOption() {
    myIntVectorTextField->setText(toString(myOptionsEditor->myOriginalOptionsContainer.getIntVector(myName)).c_str());
    onCmdSetOption(nullptr, 0, nullptr);
}


long
GNEOptionsEditorRow::OptionIntVector::onCmdSetOption(FXObject*, FXSelector, void*) {
    try {
        // check that int vector can be parsed
        const auto intVector = StringTokenizer(myIntVectorTextField->getText().text()).getVector();
        for (const auto& intValue : intVector) {
            StringUtils::toInt(intValue);
        }
        myOptionsEditor->myOptionsContainer.resetWritable();
        myOptionsEditor->myOptionsContainer.set(myName, myIntVectorTextField->getText().text());
        myIntVectorTextField->setTextColor(GUIDesignTextColorBlack);
        myOptionsEditor->myOptionsModified = true;
    } catch (...) {
        myIntVectorTextField->setTextColor(GUIDesignTextColorRed);
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

GNEOptionsEditorRow::OptionFloat::OptionFloat(GNEOptionsEditor* optionsEditor, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description,
        const std::string& defaultValue, const bool editable) :
    OptionRow(optionsEditor, parent, topic, name, description, parseFloat(defaultValue), editable) {
    myFloatTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myFloatTextField->setText(toString(myOptionsEditor->myOptionsContainer.getFloat(name)).c_str());
    if (!editable) {
        myFloatTextField->disable();
    }
    updateOption();
}


void
GNEOptionsEditorRow::OptionFloat::updateOption() {
    myFloatTextField->setText(toString(myOptionsEditor->myOptionsContainer.getFloat(myName)).c_str());
    updateResetButton();
}


void
GNEOptionsEditorRow::OptionFloat::restoreOption() {
    myFloatTextField->setText(toString(myOptionsEditor->myOriginalOptionsContainer.getFloat(myName)).c_str());
    onCmdSetOption(nullptr, 0, nullptr);
}


long
GNEOptionsEditorRow::OptionFloat::onCmdSetOption(FXObject*, FXSelector, void*) {
    // avoid empty values
    if (myFloatTextField->getText().empty()) {
        myFloatTextField->setText(myDefaultValue.c_str());
    } else {
        myOptionsEditor->myOptionsContainer.resetWritable();
        myOptionsEditor->myOptionsContainer.set(myName, myFloatTextField->getText().text());
        myOptionsEditor->myOptionsModified = true;
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

GNEOptionsEditorRow::OptionTime::OptionTime(GNEOptionsEditor* optionsEditor, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description,
        const std::string& defaultValue, const bool editable) :
    OptionRow(optionsEditor, parent, topic, name, description, parseTime(defaultValue), editable) {
    myTimeTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTimeTextField->setText(toString(myOptionsEditor->myOptionsContainer.getString(name)).c_str());
    if (!editable) {
        myTimeTextField->disable();
    }
    updateOption();
}


void
GNEOptionsEditorRow::OptionTime::updateOption() {
    myTimeTextField->setText(toString(myOptionsEditor->myOptionsContainer.getString(myName)).c_str());
    updateResetButton();
}


void
GNEOptionsEditorRow::OptionTime::restoreOption() {
    myTimeTextField->setText(toString(myOptionsEditor->myOriginalOptionsContainer.getString(myName)).c_str());
    onCmdSetOption(nullptr, 0, nullptr);
}


long
GNEOptionsEditorRow::OptionTime::onCmdSetOption(FXObject*, FXSelector, void*) {
    // avoid empty values
    if (myTimeTextField->getText().empty()) {
        myTimeTextField->setText(myDefaultValue.c_str());
    } else {
        myOptionsEditor->myOptionsContainer.resetWritable();
        myOptionsEditor->myOptionsContainer.set(myName, myTimeTextField->getText().text());
        myOptionsEditor->myOptionsModified = true;
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

GNEOptionsEditorRow::OptionFilename::OptionFilename(GNEOptionsEditor* optionsEditor, FXComposite* parent, const std::string& topic,
        const std::string& name, const std::string& description, const std::string& defaultValue, const bool editable) :
    OptionRow(optionsEditor, parent, topic, name, description, defaultValue, editable) {
    myOpenFilenameButton = GUIDesigns::buildFXButton(myContentFrame, "", "", TL("Select filename"),
                           GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonIcon);
    myFilenameTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    if (!editable) {
        myOpenFilenameButton->disable();
        myFilenameTextField->disable();
    }
    updateOption();
}


void
GNEOptionsEditorRow::OptionFilename::updateOption() {
    myFilenameTextField->setText(myOptionsEditor->myOptionsContainer.getString(myName).c_str());
    updateResetButton();
}


void
GNEOptionsEditorRow::OptionFilename::restoreOption() {
    myFilenameTextField->setText(myOptionsEditor->myOriginalOptionsContainer.getString(myName).c_str());
    onCmdSetOption(nullptr, 0, nullptr);
}


long
GNEOptionsEditorRow::OptionFilename::onCmdOpenDialog(FXObject*, FXSelector, void*) {
    // get open mode
    GNEFileDialog::OpenMode openMode = (myName.find("output") != std::string::npos) ? GNEFileDialog::OpenMode::SAVE : GNEFileDialog::OpenMode::LOAD_SINGLE;
    // open dialog
    const GNEFileDialog XMLFileDialog(myOptionsEditor->myDialog->getApplicationWindow(), this,
                                      TL("XML file"),
                                      SUMOXMLDefinitions::XMLFileExtensions.getStrings(), openMode,
                                      GNEFileDialog::ConfigType::NETEDIT);
    // check that file is valid
    if (XMLFileDialog.getResult() == GNEDialog::Result::ACCEPT) {
        myFilenameTextField->setText(XMLFileDialog.getFilename().c_str(), TRUE);
    }
    updateResetButton();
    return 1;
}


long
GNEOptionsEditorRow::OptionFilename::onCmdSetOption(FXObject*, FXSelector, void*) {
    if (SUMOXMLDefinitions::isValidFilename(myFilenameTextField->getText().text())) {
        myOptionsEditor->myOptionsContainer.resetWritable();
        myOptionsEditor->myOptionsContainer.set(myName, myFilenameTextField->getText().text());
        myFilenameTextField->setTextColor(GUIDesignTextColorBlack);
        myOptionsEditor->myOptionsModified = true;
    } else {
        myFilenameTextField->setTextColor(GUIDesignTextColorRed);
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
