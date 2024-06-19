/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEOptionsDialogElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// A Dialog for setting options (see OptionsCont)
/****************************************************************************/
#include <config.h>

#include <netedit/GNEApplicationWindow.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/options/OptionsCont.h>

#include "GNEOptionsDialogElements.h"
#include "GNEOptionsDialog.h"


#define MARGIN 4
#define MINNAMEWIDTH 200

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEOptionsDialogElements::InputOption) InputOptionMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEOptionsDialogElements::InputOption::onCmdSetOption),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RESET,          GNEOptionsDialogElements::InputOption::onCmdResetOption),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_RESET,          GNEOptionsDialogElements::InputOption::onUpdResetOption),
};

FXDEFMAP(GNEOptionsDialogElements::InputFilename) InputFilenameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,  GNEOptionsDialogElements::InputFilename::onCmdOpenDialog),
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNEOptionsDialogElements::InputOption,     FXHorizontalFrame,                      InputOptionMap,     ARRAYNUMBER(InputOptionMap))
FXIMPLEMENT_ABSTRACT(GNEOptionsDialogElements::InputFilename,   GNEOptionsDialogElements::InputOption,  InputFilenameMap,   ARRAYNUMBER(InputFilenameMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEOptionsDialogElements::InputOption - methods
// ---------------------------------------------------------------------------

GNEOptionsDialogElements::InputOption::InputOption(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
        const std::string& name, const std::string& description, const std::string& defaultValue) :
    FXHorizontalFrame(parent, GUIDesignAuxiliarHorizontalFrame),
    myGUIDialogOptions(GUIDialogOptions),
    myTopic(topic),
    myName(name),
    myDescription(description),
    myDefaultValue(defaultValue) {
    // build label with name (default width 150)
    myNameLabel = new MFXLabelTooltip(this, myGUIDialogOptions->myGNEApp->getStaticTooltipMenu(), name.c_str(), nullptr, GUIDesignLabelThickedFixed(MINNAMEWIDTH));
    // set description as tooltip
    myNameLabel->setTipText(description.c_str());
    // create content frame
    myContentFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // Create reset button
    myResetButton = GUIDesigns::buildFXButton(this, "", "", TL("Reset value"), GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_RESET, GUIDesignButtonIcon);
}


void
GNEOptionsDialogElements::InputOption::adjustNameSize() {
    const int nameWidth = myNameLabel->getFont()->getTextWidth(myNameLabel->getText().text(), myNameLabel->getText().length() + MARGIN);
    if (nameWidth > MINNAMEWIDTH) {
        myNameLabel->setWidth(nameWidth);
    }
}


const std::string&
GNEOptionsDialogElements::InputOption::getTopic() const {
    return myTopic;
}


const std::string
GNEOptionsDialogElements::InputOption::getNameLower() const {
    return StringUtils::to_lower_case(myName);
}


const std::string
GNEOptionsDialogElements::InputOption::getDescriptionLower() const {
    return StringUtils::to_lower_case(myDescription);
}


long
GNEOptionsDialogElements::InputOption::onUpdResetOption(FXObject*, FXSelector, void*) {
    if (getValue() != myDefaultValue) {
        myResetButton->enable();
    } else {
        myResetButton->disable();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEOptionsDialogElements::InputString - methods
// ---------------------------------------------------------------------------

GNEOptionsDialogElements::InputString::InputString(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description, const std::string& defaultValue) :
    InputOption(GUIDialogOptions, parent, topic, name, description, defaultValue) {
    myStringTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myStringTextField->setText(myGUIDialogOptions->myOptionsContainer.getString(name).c_str());
    updateOption();
}


void
GNEOptionsDialogElements::InputString::updateOption() {
    myStringTextField->setText(myGUIDialogOptions->myOptionsContainer.getString(myName).c_str());
}


void
GNEOptionsDialogElements::InputString::restoreOption() {
    myStringTextField->setText(myGUIDialogOptions->myOriginalOptionsContainer.getString(myName).c_str());
}


long
GNEOptionsDialogElements::InputString::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer.resetWritable();
    myGUIDialogOptions->myOptionsContainer.set(myName, myStringTextField->getText().text());
    myGUIDialogOptions->myOptionsModified = true;
    return 1;
}


long
GNEOptionsDialogElements::InputString::onCmdResetOption(FXObject*, FXSelector, void*) {
    myStringTextField->setText(myDefaultValue.c_str());
    return 1;
}


std::string
GNEOptionsDialogElements::InputString::getValue() const {
    return myStringTextField->getText().text();
}


GNEOptionsDialogElements::InputStringVector::InputStringVector(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description, const std::string& defaultValue) :
    InputOption(GUIDialogOptions, parent, topic, name, description, defaultValue) {
    myStringVectorTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    updateOption();
}


void
GNEOptionsDialogElements::InputStringVector::updateOption() {
    myStringVectorTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getStringVector(myName)).c_str());
}


void
GNEOptionsDialogElements::InputStringVector::restoreOption() {
    myStringVectorTextField->setText(toString(myGUIDialogOptions->myOriginalOptionsContainer.getStringVector(myName)).c_str());
}


long
GNEOptionsDialogElements::InputStringVector::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer.resetWritable();
    myGUIDialogOptions->myOptionsContainer.set(myName, myStringVectorTextField->getText().text());
    myGUIDialogOptions->myOptionsModified = true;
    return 1;
}


long
GNEOptionsDialogElements::InputStringVector::onCmdResetOption(FXObject*, FXSelector, void*) {
    myStringVectorTextField->setText(myDefaultValue.c_str());
    return 1;
}


std::string
GNEOptionsDialogElements::InputStringVector::getValue() const {
    return myStringVectorTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEOptionsDialogElements::InputBool - methods
// ---------------------------------------------------------------------------

GNEOptionsDialogElements::InputBool::InputBool(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description, const std::string& defaultValue) :
    InputOption(GUIDialogOptions, parent, topic, name, description, defaultValue) {
    myCheckButton = new FXCheckButton(myContentFrame, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    updateOption();
}


void
GNEOptionsDialogElements::InputBool::updateOption() {
    if (myGUIDialogOptions->myOptionsContainer.getBool(myName)) {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
}


void
GNEOptionsDialogElements::InputBool::restoreOption() {
    if (myGUIDialogOptions->myOriginalOptionsContainer.getBool(myName)) {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
}


long
GNEOptionsDialogElements::InputBool::onCmdSetOption(FXObject*, FXSelector, void*) {
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
    return 1;
}


long
GNEOptionsDialogElements::InputBool::onCmdResetOption(FXObject*, FXSelector, void*) {
    if (StringUtils::toBool(myDefaultValue)) {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
    return 1;
}


std::string
GNEOptionsDialogElements::InputBool::getValue() const {
    return myCheckButton->getCheck() ? "true" : "false";
}

// ---------------------------------------------------------------------------
// GNEOptionsDialogElements::InputInt - methods
// ---------------------------------------------------------------------------

GNEOptionsDialogElements::InputInt::InputInt(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description, const std::string& defaultValue) :
    InputOption(GUIDialogOptions, parent, topic, name, description, defaultValue) {
    myIntTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_INTEGER));
    updateOption();
}


void
GNEOptionsDialogElements::InputInt::updateOption() {
    myIntTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getInt(myName)).c_str());
}


void
GNEOptionsDialogElements::InputInt::restoreOption() {
    myIntTextField->setText(toString(myGUIDialogOptions->myOriginalOptionsContainer.getInt(myName)).c_str());
}


long
GNEOptionsDialogElements::InputInt::onCmdSetOption(FXObject*, FXSelector, void*) {
    if (myIntTextField->getText().empty()) {
        myIntTextField->setText(myDefaultValue.c_str());
    } else {
        myGUIDialogOptions->myOptionsContainer.resetWritable();
        myGUIDialogOptions->myOptionsContainer.set(myName, myIntTextField->getText().text());
        myGUIDialogOptions->myOptionsModified = true;
    }
    return 1;
}


long
GNEOptionsDialogElements::InputInt::onCmdResetOption(FXObject*, FXSelector, void*) {
    myIntTextField->setText(myDefaultValue.c_str());
    return 1;
}


std::string
GNEOptionsDialogElements::InputInt::getValue() const {
    return myIntTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEOptionsDialogElements::InputIntVector - methods
// ---------------------------------------------------------------------------

GNEOptionsDialogElements::InputIntVector::InputIntVector(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description, const std::string& defaultValue) :
    InputOption(GUIDialogOptions, parent, topic, name, description, defaultValue) {
    myIntVectorTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myIntVectorTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getIntVector(name)).c_str());
    updateOption();
}


void
GNEOptionsDialogElements::InputIntVector::updateOption() {
    myIntVectorTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getIntVector(myName)).c_str());
}


void
GNEOptionsDialogElements::InputIntVector::restoreOption() {
    myIntVectorTextField->setText(toString(myGUIDialogOptions->myOriginalOptionsContainer.getIntVector(myName)).c_str());
}


long
GNEOptionsDialogElements::InputIntVector::onCmdSetOption(FXObject*, FXSelector, void*) {
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
    return 1;
}


long
GNEOptionsDialogElements::InputIntVector::onCmdResetOption(FXObject*, FXSelector, void*) {
    myIntVectorTextField->setText(myDefaultValue.c_str());
    return 1;
}


std::string
GNEOptionsDialogElements::InputIntVector::getValue() const {
    return myIntVectorTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEOptionsDialogElements::InputFloat - methods
// ---------------------------------------------------------------------------

GNEOptionsDialogElements::InputFloat::InputFloat(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
        const std::string& name, const std::string& description, const std::string& defaultValue) :
    InputOption(GUIDialogOptions, parent, topic, name, description, parseFloat(defaultValue)) {
    myFloatTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myFloatTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getFloat(name)).c_str());
    updateOption();
}


void
GNEOptionsDialogElements::InputFloat::updateOption() {
    myFloatTextField->setText(toString(myGUIDialogOptions->myOptionsContainer.getFloat(myName)).c_str());
}


void
GNEOptionsDialogElements::InputFloat::restoreOption() {
    myFloatTextField->setText(toString(myGUIDialogOptions->myOriginalOptionsContainer.getFloat(myName)).c_str());
}


long
GNEOptionsDialogElements::InputFloat::onCmdSetOption(FXObject*, FXSelector, void*) {
    // avoid empty values
    if (myFloatTextField->getText().empty()) {
        myFloatTextField->setText(myDefaultValue.c_str());
    } else {
        myGUIDialogOptions->myOptionsContainer.resetWritable();
        myGUIDialogOptions->myOptionsContainer.set(myName, myFloatTextField->getText().text());
        myGUIDialogOptions->myOptionsModified = true;
    }
    return 1;
}


long
GNEOptionsDialogElements::InputFloat::onCmdResetOption(FXObject*, FXSelector, void*) {
    myFloatTextField->setText(myDefaultValue.c_str());
    return 1;
}


std::string
GNEOptionsDialogElements::InputFloat::getValue() const {
    return myFloatTextField->getText().text();
}


std::string
GNEOptionsDialogElements::InputFloat::parseFloat(const std::string& value) const {
    try {
        return toString(StringUtils::toDouble(value));
    } catch (...) {
        return value;
    }
}

// ---------------------------------------------------------------------------
// GNEOptionsDialogElements::InputFilename - methods
// ---------------------------------------------------------------------------

GNEOptionsDialogElements::InputFilename::InputFilename(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
        const std::string& name, const std::string& description, const std::string& defaultValue) :
    InputOption(GUIDialogOptions, parent, topic, name, description, defaultValue) {
    myOpenFilenameButton = GUIDesigns::buildFXButton(myContentFrame, "", "", TL("Select filename"),
                           GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonIcon);
    myFilenameTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    updateOption();
}


void
GNEOptionsDialogElements::InputFilename::updateOption() {
    myFilenameTextField->setText(myGUIDialogOptions->myOptionsContainer.getString(myName).c_str());
}


void
GNEOptionsDialogElements::InputFilename::restoreOption() {
    myFilenameTextField->setText(myGUIDialogOptions->myOriginalOptionsContainer.getString(myName).c_str());
}


long
GNEOptionsDialogElements::InputFilename::onCmdOpenDialog(FXObject*, FXSelector, void*) {
    // get file
    const auto file = GNEApplicationWindowHelper::openFileDialog(this, (myName.find("output") != std::string::npos), false);
    // check that file is valid
    if (file.size() > 0) {
        myFilenameTextField->setText(file.c_str(), TRUE);
    }
    return 1;
}


long
GNEOptionsDialogElements::InputFilename::onCmdSetOption(FXObject*, FXSelector, void*) {
    if (SUMOXMLDefinitions::isValidFilename(myFilenameTextField->getText().text())) {
        myGUIDialogOptions->myOptionsContainer.resetWritable();
        myGUIDialogOptions->myOptionsContainer.set(myName, myFilenameTextField->getText().text());
        myFilenameTextField->setTextColor(FXRGB(0, 0, 0));
        myGUIDialogOptions->myOptionsModified = true;
    } else {
        myFilenameTextField->setTextColor(FXRGB(255, 0, 0));
    }
    return 1;
}


long
GNEOptionsDialogElements::InputFilename::onCmdResetOption(FXObject*, FXSelector, void*) {
    myFilenameTextField->setText(myDefaultValue.c_str());
    return 1;
}


GNEOptionsDialogElements::InputFilename::InputFilename() {}


std::string
GNEOptionsDialogElements::InputFilename::getValue() const {
    return myFilenameTextField->getText().text();
}

/****************************************************************************/
