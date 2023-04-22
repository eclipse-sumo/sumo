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
/// @file    GUIDialog_Options.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// The "About" - dialog for netedit, (adapted from GUIDialog_AboutSUMO)
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/foxtools/MFXLinkLabel.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/options/OptionsCont.h>

#include "GUIDialog_Options.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GUIDialog_Options) GUIDialogOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RUNNETGENERATE, GUIDialog_Options::onCmdRunNetgenerate),
};

FXDEFMAP(GUIDialog_Options::InputOption) InputOptionMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GUIDialog_Options::InputOption::onCmdSetOption),
};

// Object implementation
FXIMPLEMENT(GUIDialog_Options,                          FXDialogBox,        GUIDialogOptionsMap,    ARRAYNUMBER(GUIDialogOptionsMap))
FXIMPLEMENT_ABSTRACT(GUIDialog_Options::InputOption,    FXHorizontalFrame,  InputOptionMap,         ARRAYNUMBER(InputOptionMap))

// ===========================================================================
// method definitions
// ===========================================================================

std::pair<int, bool>
GUIDialog_Options::Options(GUIMainWindow* windows, OptionsCont* optionsContainer, const char* titleName) {
    GUIDialog_Options* optionsDialog = new GUIDialog_Options(windows, optionsContainer, titleName, false);
    return std::make_pair(optionsDialog->execute(), optionsDialog->myModified);
}


std::pair<int, bool>
GUIDialog_Options::Run(GUIMainWindow* windows, OptionsCont* optionsContainer, const char* titleName) {
    GUIDialog_Options* optionsDialog = new GUIDialog_Options(windows, optionsContainer, titleName, true);
    return std::make_pair(optionsDialog->execute(), optionsDialog->myModified);
}


GUIDialog_Options::~GUIDialog_Options() { }


long
GUIDialog_Options::onCmdRunNetgenerate(FXObject*, FXSelector, void*) {
    // close dialog accepting changes
    handle(this, FXSEL(SEL_COMMAND, ID_ACCEPT), nullptr);
    // run tool in mainWindow
    return myMainWindowParent->handle(this, FXSEL(SEL_COMMAND, MID_GNE_RUNNETGENERATE), nullptr);
}

// ===========================================================================
// Option input classes method definitions
// ===========================================================================

GUIDialog_Options::InputOption::InputOption(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X),
    myGUIDialogOptions(GUIDialogOptions),
    myName(name) {
    // build label with name
    new FXLabel(this, (name + "\t\t" + myGUIDialogOptions->myOptionsContainer->getDescription(name)).c_str());
}


long
GUIDialog_Options::InputOption::onCmdSetOption(FXObject*, FXSelector, void*) {
    // try to set option and mark as modified if was sucessfully
    if (setOption()) {
        myGUIDialogOptions->myModified = true;
    }
    return 1;
}


GUIDialog_Options::InputString::InputString(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(myGUIDialogOptions->myOptionsContainer->getString(name).c_str());
}


bool
GUIDialog_Options::InputString::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return true;
}


GUIDialog_Options::InputStringVector::InputStringVector(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getStringVector(name)).c_str());
}


bool
GUIDialog_Options::InputStringVector::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return true;
}


GUIDialog_Options::InputBool::InputBool(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myCheck = new FXMenuCheck(this, "", this, MID_GNE_SET_ATTRIBUTE);
    myCheck->setCheck(myGUIDialogOptions->myOptionsContainer->getBool(name));
}


bool
GUIDialog_Options::InputBool::setOption() {
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


GUIDialog_Options::InputInt::InputInt(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_INTEGER | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getInt(name)).c_str());
}


bool
GUIDialog_Options::InputInt::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return true;
}


GUIDialog_Options::InputIntVector::InputIntVector(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getIntVector(name)).c_str());
}


bool
GUIDialog_Options::InputIntVector::setOption() {
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


GUIDialog_Options::InputFloat::InputFloat(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_REAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getFloat(name)).c_str());
}


bool
GUIDialog_Options::InputFloat::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return true;
}


GUIDialog_Options::InputFilename::InputFilename(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    InputOption(GUIDialogOptions, parent, name) {
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(myGUIDialogOptions->myOptionsContainer->getString(name).c_str());
}


bool
GUIDialog_Options::InputFilename::setOption() {
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


GUIDialog_Options::GUIDialog_Options(GUIMainWindow* parent, OptionsCont* optionsContainer, const char* titleName, const bool runDialog) :
    FXDialogBox(parent, titleName, GUIDesignDialogBoxResizable, 0, 0, parent->getWidth(), parent->getHeight()),
    myMainWindowParent(parent),
    myOptionsContainer(optionsContainer) {
    new FXStatusBar(this, GUIDesignStatusBar);
    FXVerticalFrame* contentFrame = new FXVerticalFrame(this, GUIDesignContentsFrame);
    // create tabbook
    FXTabBook* tabbook = new FXTabBook(contentFrame, nullptr, 0, GUIDesignTabBook);
    // iterate over all topics
    for (const auto& topic : myOptionsContainer->getSubTopics()) {
        // ignore configuration
        if (topic != "Configuration") {
            new FXTabItem(tabbook, topic.c_str(), nullptr, TAB_LEFT_NORMAL);
            FXScrollWindow* scrollTab = new FXScrollWindow(tabbook, LAYOUT_FILL_X | LAYOUT_FILL_Y);
            FXVerticalFrame* tabContent = new FXVerticalFrame(scrollTab, FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_X | LAYOUT_FILL_Y);
            const std::vector<std::string> entries = myOptionsContainer->getSubTopicsEntries(topic);
            for (const auto& entry : entries) {
                if (entry != "geometry.remove" && entry != "edges.join" && entry != "geometry.split" && entry != "ramps.guess" && entry != "ramps.set") {
                    const std::string type = myOptionsContainer->getTypeName(entry);
                    if (type == "STR") {
                        new InputString(this, tabContent, entry);
                    } else if ((type == "FILE") || (type == "NETWORK") || (type == "ADDITIONAL") || (type == "ROUTE") || (type == "DATA")) {
                        new InputFilename(this, tabContent, entry);
                    } else if (type == "BOOL") {
                        new InputBool(this, tabContent, entry);
                    } else if (type == "INT") {
                        new InputInt(this, tabContent, entry);
                    } else if (type == "FLOAT") {
                        new InputFloat(this, tabContent, entry);
                    } else if (type == "INT[]") {
                        new InputIntVector(this, tabContent, entry);
                    } else if (type == "STR[]") {
                        new InputStringVector(this, tabContent, entry);
                    }
                }
            }
        }
    }
    // create buttons frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(contentFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // continue depending of dialog type
    if (runDialog) {
        new FXButton(buttonsFrame, (TL("Run") + std::string("\t\t") + TL("Run tool")).c_str(), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_RUNNETGENERATE, GUIDesignButtonOK);
        new FXButton(buttonsFrame, (TL("Cancel") + std::string("\t\t") + TL("Cancel tool")).c_str(), GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, ID_CANCEL, GUIDesignButtonOK);
    } else {
        new FXButton(buttonsFrame, (TL("OK") + std::string("\t\t") + TL("Accept settings")).c_str(), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, ID_ACCEPT, GUIDesignButtonOK);
    }
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
}

/****************************************************************************/
