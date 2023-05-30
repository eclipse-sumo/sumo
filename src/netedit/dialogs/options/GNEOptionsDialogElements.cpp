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


#define MARGING 4
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
        const std::string& name, const std::string& description) :
    FXHorizontalFrame(parent, GUIDesignAuxiliarHorizontalFrame),
    myGUIDialogOptions(GUIDialogOptions),
    myTopic(topic),
    myName(name),
    myDescription(description) {
    // build label with name (default width 150)
    myNameLabel = new MFXLabelTooltip(this, myGUIDialogOptions->myMainWindowParent->getStaticTooltipMenu(), name.c_str(), nullptr, GUIDesignLabelThickedFixed(MINNAMEWIDTH));
    // set description as tooltip
    myNameLabel->setTipText(description.c_str());
    // create content frame
    myContentFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // Create reset button
    myResetButton = new FXButton(this, (std::string("\t\t") + TL("Reset value")).c_str(), GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_RESET, GUIDesignButtonIcon);
}


void
GNEOptionsDialogElements::InputOption::adjustNameSize() {
    const int nameWidth = myNameLabel->getFont()->getTextWidth(myNameLabel->getText().text(), myNameLabel->getText().length() + MARGING);
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
    std::string lowerName = myName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    return lowerName;
}


const std::string
GNEOptionsDialogElements::InputOption::getDescriptionLower() const {
    std::string lowerDescription = myDescription;
    std::transform(lowerDescription.begin(), lowerDescription.end(), lowerDescription.begin(), ::tolower);
    return lowerDescription;
}


long
GNEOptionsDialogElements::InputOption::onUpdResetOption(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNEOptionsDialogElements::InputString - methods
// ---------------------------------------------------------------------------

GNEOptionsDialogElements::InputString::InputString(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myStringTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myStringTextField->setText(myGUIDialogOptions->myOptionsContainer->getString(name).c_str());
}



long
GNEOptionsDialogElements::InputString::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myStringTextField->getText().text());
    myGUIDialogOptions->myModified = true;
    return 1;
}


long
GNEOptionsDialogElements::InputString::onCmdResetOption(FXObject*, FXSelector, void*) {
    return 1;
}


GNEOptionsDialogElements::InputStringVector::InputStringVector(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myStringVectorTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myStringVectorTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getStringVector(name)).c_str());
}


long
GNEOptionsDialogElements::InputStringVector::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myStringVectorTextField->getText().text());
    myGUIDialogOptions->myModified = true;
    return 1;
}


long
GNEOptionsDialogElements::InputStringVector::onCmdResetOption(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNEOptionsDialogElements::InputBool - methods
// ---------------------------------------------------------------------------

GNEOptionsDialogElements::InputBool::InputBool(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myCheckButton = new FXCheckButton(myContentFrame, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    if (myGUIDialogOptions->myOptionsContainer->getBool(name)) {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
}


long
GNEOptionsDialogElements::InputBool::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    if (myCheckButton->getCheck()) {
        myGUIDialogOptions->myOptionsContainer->set(myName, "true");
        myCheckButton->setText(TL("true"));
    } else {
        myGUIDialogOptions->myOptionsContainer->set(myName, "false");
        myCheckButton->setText(TL("false"));
    }
    myGUIDialogOptions->myModified = true;
    // special checks for Debug flags
    if ((myName == "gui-testing-debug") && myGUIDialogOptions->myOptionsContainer->isSet("gui-testing-debug")) {
        MsgHandler::enableDebugMessages(myGUIDialogOptions->myOptionsContainer->getBool("gui-testing-debug"));
    }
    if ((myName == "gui-testing-debug-gl") && myGUIDialogOptions->myOptionsContainer->isSet("gui-testing-debug-gl")) {
        MsgHandler::enableDebugGLMessages(myGUIDialogOptions->myOptionsContainer->getBool("gui-testing-debug-gl"));
    }
    return 1;
}


long
GNEOptionsDialogElements::InputBool::onCmdResetOption(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNEOptionsDialogElements::InputInt - methods
// ---------------------------------------------------------------------------

GNEOptionsDialogElements::InputInt::InputInt(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myIntTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_INTEGER));
    myIntTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getInt(name)).c_str());
}


long
GNEOptionsDialogElements::InputInt::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myIntTextField->getText().text());
    myGUIDialogOptions->myModified = true;
    return 1;
}


long
GNEOptionsDialogElements::InputInt::onCmdResetOption(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNEOptionsDialogElements::InputIntVector - methods
// ---------------------------------------------------------------------------

GNEOptionsDialogElements::InputIntVector::InputIntVector(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myIntVectorTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myIntVectorTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getIntVector(name)).c_str());
}


long
GNEOptionsDialogElements::InputIntVector::onCmdSetOption(FXObject*, FXSelector, void*) {
    try {
        // check that int vector can be parsed
        const auto intVector = StringTokenizer(myIntVectorTextField->getText().text()).getVector();
        for (const auto& intValue : intVector) {
            StringUtils::toInt(intValue);
        }
        myGUIDialogOptions->myOptionsContainer->resetWritable();
        myGUIDialogOptions->myOptionsContainer->set(myName, myIntVectorTextField->getText().text());
        myIntVectorTextField->setTextColor(FXRGB(0, 0, 0));
        myGUIDialogOptions->myModified = true;
    } catch (...) {
        myIntVectorTextField->setTextColor(FXRGB(255, 0, 0));
    }
    return 1;
}


long
GNEOptionsDialogElements::InputIntVector::onCmdResetOption(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNEOptionsDialogElements::InputFloat - methods
// ---------------------------------------------------------------------------

GNEOptionsDialogElements::InputFloat::InputFloat(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
        const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myFloatTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myFloatTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getFloat(name)).c_str());
}


long
GNEOptionsDialogElements::InputFloat::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myFloatTextField->getText().text());
    return 1;
}


long
GNEOptionsDialogElements::InputFloat::onCmdResetOption(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNEOptionsDialogElements::InputFilename - methods
// ---------------------------------------------------------------------------

GNEOptionsDialogElements::InputFilename::InputFilename(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
        const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myOpenFilenameButton = new FXButton(myContentFrame, (std::string("\t\t") + TL("Select filename")).c_str(),
        GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonIcon);
    myFilenameTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myFilenameTextField->setText(myGUIDialogOptions->myOptionsContainer->getString(name).c_str());
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
        myGUIDialogOptions->myOptionsContainer->resetWritable();
        myGUIDialogOptions->myOptionsContainer->set(myName, myFilenameTextField->getText().text());
        myFilenameTextField->setTextColor(FXRGB(0, 0, 0));
        myGUIDialogOptions->myModified = true;
    } else {
        myFilenameTextField->setTextColor(FXRGB(255, 0, 0));
    }
    return 1;
}


long
GNEOptionsDialogElements::InputFilename::onCmdResetOption(FXObject*, FXSelector, void*) {
    return 1;
}


GNEOptionsDialogElements::InputFilename::InputFilename() {}

/****************************************************************************/
