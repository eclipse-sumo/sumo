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

#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/options/OptionsCont.h>

#include "GNEOptionsDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEOptionsDialog) GUIDialogOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RUNNETGENERATE, GNEOptionsDialog::onCmdRunNetgenerate),
};

// Object implementation
FXIMPLEMENT(GNEOptionsDialog,   FXDialogBox,    GUIDialogOptionsMap,    ARRAYNUMBER(GUIDialogOptionsMap))

// ===========================================================================
// method definitions
// ===========================================================================

std::pair<int, bool>
GNEOptionsDialog::Options(GUIMainWindow* windows, GUIIcon icon, OptionsCont* optionsContainer, const char* titleName) {
    GNEOptionsDialog* optionsDialog = new GNEOptionsDialog(windows, icon, optionsContainer, titleName, false);
    return std::make_pair(optionsDialog->execute(), optionsDialog->myModified);
}


std::pair<int, bool>
GNEOptionsDialog::Run(GUIMainWindow* windows, GUIIcon icon, OptionsCont* optionsContainer, const char* titleName) {
    GNEOptionsDialog* optionsDialog = new GNEOptionsDialog(windows, icon, optionsContainer, titleName, true);
    return std::make_pair(optionsDialog->execute(), optionsDialog->myModified);
}


GNEOptionsDialog::~GNEOptionsDialog() { }


long
GNEOptionsDialog::onCmdRunNetgenerate(FXObject*, FXSelector, void*) {
    // close dialog accepting changes
    handle(this, FXSEL(SEL_COMMAND, ID_ACCEPT), nullptr);
    // run tool in mainWindow
    return myMainWindowParent->handle(this, FXSEL(SEL_COMMAND, MID_GNE_RUNNETGENERATE), nullptr);
}


GNEOptionsDialog::GNEOptionsDialog(GUIMainWindow* parent, GUIIcon icon, OptionsCont* optionsContainer, const char* titleName, const bool runDialog) :
    FXDialogBox(parent, titleName, GUIDesignDialogBoxResizable, 0, 0, 800, 600),
    myMainWindowParent(parent),
    myOptionsContainer(optionsContainer) {
    // set icon
    setIcon(GUIIconSubSys::getIcon(icon));
    // create content frame
    FXVerticalFrame* contentFrame = new FXVerticalFrame(this, GUIDesignContentsFrame);
    // create elements frame
    FXHorizontalFrame* elementsFrame = new FXHorizontalFrame(contentFrame, GUIDesignAuxiliarFrame);
    FXVerticalFrame* elementsFrameTree = new FXVerticalFrame(elementsFrame, GUIDesignAuxiliarVerticalFrame);
    FXVerticalFrame* elementsFrameValues = new FXVerticalFrame(elementsFrame, GUIDesignAuxiliarFrame);
    // Create GroupBox modules
    MFXGroupBoxModule* groupBoxTree = new MFXGroupBoxModule(elementsFrameTree, TL("Topics"));
    MFXGroupBoxModule* groupBoxOptions = new MFXGroupBoxModule(elementsFrameValues, TL("Options"));
    // create FXTreeList
    myTopicsTreeList = new FXTreeList(groupBoxTree->getCollapsableFrame(), this, MID_GNE_SELECT, GUIDesignTreeListFixedWidth);
    myTopicsTreeList->setWidth(200);
    // create scroll
    FXScrollWindow* scrollTabEntries = new FXScrollWindow(groupBoxOptions->getCollapsableFrame(), LAYOUT_FILL_X | LAYOUT_FILL_Y);
    // create vertical frame for entries
    FXVerticalFrame* entriesFrame = new FXVerticalFrame(scrollTabEntries, GUIDesignAuxiliarFrame);
    // iterate over all topics
    for (const auto& topic : myOptionsContainer->getSubTopics()) {
        // check if we have to ignore this topic
        if (myIgnoredTopics.count(topic) == 0) {
            // iterate over entries
            const std::vector<std::string> entries = myOptionsContainer->getSubTopicsEntries(topic);
            for (const auto& entry : entries) {
                // check if we have to ignore this entry
                if (myIgnoredEntries.count(entry) == 0) {
                    const std::string type = myOptionsContainer->getTypeName(entry);
                    // continue depending of type
                    if (type == "STR") {
                        myInputOptionEntries.push_back(InputOptionEntry(topic, entry, new GNEOptionsDialogElements::InputString(this, entriesFrame, entry)));
                    } else if ((type == "FILE") || (type == "NETWORK") || (type == "ADDITIONAL") || (type == "ROUTE") || (type == "DATA")) {
                        myInputOptionEntries.push_back(InputOptionEntry(topic, entry, new GNEOptionsDialogElements::InputFilename(this, entriesFrame, entry)));
                    } else if (type == "BOOL") {
                        myInputOptionEntries.push_back(InputOptionEntry(topic, entry, new GNEOptionsDialogElements::InputBool(this, entriesFrame, entry)));
                    } else if (type == "INT") {
                        myInputOptionEntries.push_back(InputOptionEntry(topic, entry, new GNEOptionsDialogElements::InputInt(this, entriesFrame, entry)));
                    } else if (type == "FLOAT") {
                        myInputOptionEntries.push_back(InputOptionEntry(topic, entry, new GNEOptionsDialogElements::InputFloat(this, entriesFrame, entry)));
                    } else if (type == "INT[]") {
                        myInputOptionEntries.push_back(InputOptionEntry(topic, entry, new GNEOptionsDialogElements::InputIntVector(this, entriesFrame, entry)));
                    } else if (type == "STR[]") {
                        myInputOptionEntries.push_back(InputOptionEntry(topic, entry, new GNEOptionsDialogElements::InputStringVector(this, entriesFrame, entry)));
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
