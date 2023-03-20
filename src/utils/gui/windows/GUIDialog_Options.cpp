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

FXDEFMAP(GUIDialog_Options::InputString) InputStringMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GUIDialog_Options::InputString::onCmdSetOption),
};

FXDEFMAP(GUIDialog_Options::InputStringVector) InputStringVectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GUIDialog_Options::InputStringVector::onCmdSetOption),
};

FXDEFMAP(GUIDialog_Options::InputBool) InputBoolMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GUIDialog_Options::InputBool::onCmdSetOption),
};

FXDEFMAP(GUIDialog_Options::InputInt) InputIntMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GUIDialog_Options::InputInt::onCmdSetOption),
};

FXDEFMAP(GUIDialog_Options::InputIntVector) InputIntVectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GUIDialog_Options::InputIntVector::onCmdSetOption),
};

FXDEFMAP(GUIDialog_Options::InputFloat) InputFloatMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GUIDialog_Options::InputFloat::onCmdSetOption),
};

FXDEFMAP(GUIDialog_Options::InputFilename) InputFilenameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GUIDialog_Options::InputFilename::onCmdSetOption),
};

// Object implementation
FXIMPLEMENT(GUIDialog_Options,                      FXDialogBox,        GUIDialogOptionsMap,    ARRAYNUMBER(GUIDialogOptionsMap))
FXIMPLEMENT(GUIDialog_Options::InputString,         FXHorizontalFrame,  InputStringMap,         ARRAYNUMBER(InputStringMap))
FXIMPLEMENT(GUIDialog_Options::InputStringVector,   FXHorizontalFrame,  InputStringVectorMap,   ARRAYNUMBER(InputStringVectorMap))
FXIMPLEMENT(GUIDialog_Options::InputBool,           FXHorizontalFrame,  InputBoolMap,           ARRAYNUMBER(InputBoolMap))
FXIMPLEMENT(GUIDialog_Options::InputInt,            FXHorizontalFrame,  InputIntMap,            ARRAYNUMBER(InputIntMap))
FXIMPLEMENT(GUIDialog_Options::InputIntVector,      FXHorizontalFrame,  InputIntVectorMap,      ARRAYNUMBER(InputIntVectorMap))
FXIMPLEMENT(GUIDialog_Options::InputFloat,          FXHorizontalFrame,  InputFloatMap,          ARRAYNUMBER(InputFloatMap))
FXIMPLEMENT(GUIDialog_Options::InputFilename,       FXHorizontalFrame,  InputFilenameMap,       ARRAYNUMBER(InputFilenameMap))

// ===========================================================================
// method definitions
// ===========================================================================

int
GUIDialog_Options::Options(GUIMainWindow *windows, OptionsCont* optionsContainer, const char* titleName) {
    GUIDialog_Options* wizard = new GUIDialog_Options(windows, optionsContainer, titleName, false);
    return wizard->execute();
}


int
GUIDialog_Options::Run(GUIMainWindow *windows, OptionsCont* optionsContainer, const char* titleName) {
    GUIDialog_Options* wizard = new GUIDialog_Options(windows, optionsContainer, titleName, true);
    return wizard->execute();
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

GUIDialog_Options::InputString::InputString(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X),
    myGUIDialogOptions(GUIDialogOptions),
    myName(name) {
    new FXLabel(this, (name + "\t\t" + myGUIDialogOptions->myOptionsContainer->getDescription(name)).c_str());
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(myGUIDialogOptions->myOptionsContainer->getString(name).c_str());
}


long
GUIDialog_Options::InputString::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return 1;
}


GUIDialog_Options::InputStringVector::InputStringVector(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X),
    myGUIDialogOptions(GUIDialogOptions),
    myName(name) {
    new FXLabel(this, (name + "\t\t" + myGUIDialogOptions->myOptionsContainer->getDescription(name)).c_str());
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getStringVector(name)).c_str());
}


long
GUIDialog_Options::InputStringVector::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return 1;
}


GUIDialog_Options::InputBool::InputBool(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X),
    myGUIDialogOptions(GUIDialogOptions),
    myName(name) {
    new FXLabel(this, (name + "\t\t" + myGUIDialogOptions->myOptionsContainer->getDescription(name)).c_str());
    myCheck = new FXMenuCheck(this, "", this, MID_GNE_SET_ATTRIBUTE);
    myCheck->setCheck(myGUIDialogOptions->myOptionsContainer->getBool(name));
}


long
GUIDialog_Options::InputBool::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myCheck->getCheck() ? "true" : "false");
    // special checks for Debug flags
    if ((myName == "gui-testing-debug") && myGUIDialogOptions->myOptionsContainer->isSet("gui-testing-debug")) {
        MsgHandler::enableDebugMessages(myGUIDialogOptions->myOptionsContainer->getBool("gui-testing-debug"));
    }
    if ((myName == "gui-testing-debug-gl") && myGUIDialogOptions->myOptionsContainer->isSet("gui-testing-debug-gl")) {
        MsgHandler::enableDebugGLMessages(myGUIDialogOptions->myOptionsContainer->getBool("gui-testing-debug-gl"));
    }
    return 1;
}


GUIDialog_Options::InputInt::InputInt(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X),
    myGUIDialogOptions(GUIDialogOptions),
    myName(name) {
    new FXLabel(this, (name + "\t\t" + myGUIDialogOptions->myOptionsContainer->getDescription(name)).c_str());
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_INTEGER | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getInt(name)).c_str());
}


long
GUIDialog_Options::InputInt::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return 1;
}


GUIDialog_Options::InputIntVector::InputIntVector(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X),
    myGUIDialogOptions(GUIDialogOptions),
    myName(name) {
    new FXLabel(this, (name + "\t\t" + myGUIDialogOptions->myOptionsContainer->getDescription(name)).c_str());
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getIntVector(name)).c_str());
}


long
GUIDialog_Options::InputIntVector::onCmdSetOption(FXObject*, FXSelector, void*) {
    try {
        // check that int vector can be parsed
        const auto intVector = StringTokenizer(myTextField->getText().text()).getVector();
        for (const auto& intValue : intVector) {
            StringUtils::toInt(intValue);
        }
        myGUIDialogOptions->myOptionsContainer->resetWritable();
        myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
        myTextField->setTextColor(FXRGB(0, 0, 0));
    } catch (...) {
        myTextField->setTextColor(FXRGB(255, 0, 0));
    }
    return 1;
}


GUIDialog_Options::InputFloat::InputFloat(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X),
    myGUIDialogOptions(GUIDialogOptions),
    myName(name) {
    new FXLabel(this, (name + "\t\t" + myGUIDialogOptions->myOptionsContainer->getDescription(name)).c_str());
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_REAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getFloat(name)).c_str());
}


long
GUIDialog_Options::InputFloat::onCmdSetOption(FXObject*, FXSelector, void*) {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return 1;
}


GUIDialog_Options::InputFilename::InputFilename(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X),
    myGUIDialogOptions(GUIDialogOptions),
    myName(name) {
    new FXLabel(this, (name + "\t\t" + myGUIDialogOptions->myOptionsContainer->getDescription(name)).c_str());
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(myGUIDialogOptions->myOptionsContainer->getString(name).c_str());
}


long
GUIDialog_Options::InputFilename::onCmdSetOption(FXObject*, FXSelector, void*) {
    if (SUMOXMLDefinitions::isValidFilename(myTextField->getText().text())) {
        myGUIDialogOptions->myOptionsContainer->resetWritable();
        myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
        myTextField->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextField->setTextColor(FXRGB(255, 0, 0));
    }
    return 1;
}


GUIDialog_Options::GUIDialog_Options(GUIMainWindow* parent, OptionsCont* optionsContainer, const char* titleName, const bool runDialog) :
    FXDialogBox(parent, titleName, GUIDesignDialogBox, 0, 0, parent->getWidth(), parent->getHeight()),
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
                    } else if (type == "FILE") {
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
