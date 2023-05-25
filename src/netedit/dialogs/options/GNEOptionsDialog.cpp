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

#include <algorithm>
#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/foxtools/MFXButtonTooltip.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/options/OptionsCont.h>

#include "GNEOptionsDialog.h"

#define TREELISTWIDTH 200

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEOptionsDialog) GUIDialogOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RUNNETGENERATE,         GNEOptionsDialog::onCmdRunNetgenerate),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SEARCH_USEDESCRIPTION,  GNEOptionsDialog::onCmdSearch),
    FXMAPFUNC(SEL_COMMAND,  MID_MTFS_UPDATED,               GNEOptionsDialog::onCmdSearch),
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


long
GNEOptionsDialog::onCmdSearch(FXObject*, FXSelector, void*) {
    if (mySearchButton->getText().count() > 0) {
        updateVisibleEntriesBySearch(mySearchButton->getText().text());
    } else {
        updateVisibleEntriesByTopic();
    }
    return 1;
}


long
GNEOptionsDialog::onCmdUseDescription(FXObject*, FXSelector, void*) {
    // update search if we're searching
    if (mySearchButton->getText().count() > 0) {
        updateVisibleEntriesBySearch(mySearchButton->getText().text());
    }
    return 1;
}


void
GNEOptionsDialog::updateVisibleEntriesByTopic() {
    // iterate over entries
    for (const auto &entry : myInputOptionEntries) {
        if (entry->getTopic() == "topic") {
            entry->show();
        } else {
            entry->hide();
        }
    }
}


void
GNEOptionsDialog::updateVisibleEntriesBySearch(std::string searchText) {
    // first tolow search text
    std::transform(searchText.begin(), searchText.end(), searchText.begin(), ::tolower);
    // iterate over entries
    for (const auto &entry : myInputOptionEntries) {
        if (searchText.empty()) {
            // show all entries if searchText is empty
            entry->show();
        } else if (entry->getNameLower().find(searchText) != std::string::npos) {
            entry->show();
        } else if ((myDescriptionSearchCheckButton->getCheck() == TRUE) &&
                   (entry->getDescriptionLower().find(searchText) != std::string::npos)) {
            entry->show();
        } else {
            entry->hide();
        }
    }
    myEntriesFrame->recalc();
    myEntriesFrame->update();
}


GNEOptionsDialog::GNEOptionsDialog(GUIMainWindow* parent, GUIIcon icon, OptionsCont* optionsContainer, const char* titleName, const bool runDialog) :
    FXDialogBox(parent, titleName, GUIDesignDialogBoxExplicitStretchable(800, 600)),
    myMainWindowParent(parent),
    myOptionsContainer(optionsContainer) {
    // set icon
    setIcon(GUIIconSubSys::getIcon(icon));
    // create content frame
    FXVerticalFrame* contentFrame = new FXVerticalFrame(this, GUIDesignContentsFrame);
    // add buttons frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(contentFrame, GUIDesignHorizontalFrameNoPadding);
    myShowToolTipsMenu = new MFXCheckableButton(false, buttonsFrame,
        parent->getStaticTooltipMenu(), "\tToggle Menu Tooltips\tToggles whether tooltips in the menu shall be shown.",
        GUIIconSubSys::getIcon(GUIIcon::SHOWTOOLTIPS_MENU), this, MID_SHOWTOOLTIPS_MENU, GUIDesignMFXCheckableButtonSquare);
    auto saveFile = new MFXButtonTooltip(buttonsFrame, parent->getStaticTooltipMenu(), TL("Save options"),
        GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_CHOOSEN_SAVE, GUIDesignButtonConfiguration);
        saveFile->setTipText(TL("Save file with tool configuration"));
    auto loadFile = new MFXButtonTooltip(buttonsFrame, parent->getStaticTooltipMenu(), TL("Load options") ,
        GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_CHOOSEN_LOAD, GUIDesignButtonConfiguration);
        loadFile->setTipText(TL("Load file with tool configuration"));
    // add separator
    new FXSeparator(contentFrame);
    // create elements frame
    FXHorizontalFrame* elementsFrame = new FXHorizontalFrame(contentFrame, GUIDesignAuxiliarFrame);
    FXVerticalFrame* elementsFrameTree = new FXVerticalFrame(elementsFrame, GUIDesignAuxiliarVerticalFrame);
    FXVerticalFrame* elementsFrameValues = new FXVerticalFrame(elementsFrame, GUIDesignAuxiliarFrame);
    // Create GroupBox modules
    MFXGroupBoxModule* groupBoxTree = new MFXGroupBoxModule(elementsFrameTree, TL("Topics"));
    MFXGroupBoxModule* groupBoxOptions = new MFXGroupBoxModule(elementsFrameValues, TL("Options"));
    // create FXTreeList
    myTopicsTreeList = new FXTreeList(groupBoxTree->getCollapsableFrame(), this, MID_GNE_SELECT, GUIDesignTreeListFixedWidth);
    myTopicsTreeList->setWidth(TREELISTWIDTH);
    // add first item
    FXTreeItem* rootItem = myTopicsTreeList->appendItem(nullptr, titleName);
    rootItem->setExpanded(TRUE);
    // create scroll
    FXScrollWindow* scrollTabEntries = new FXScrollWindow(groupBoxOptions->getCollapsableFrame(), LAYOUT_FILL_X | LAYOUT_FILL_Y);
    // create vertical frame for entries
    myEntriesFrame = new FXVerticalFrame(scrollTabEntries, GUIDesignAuxiliarFrame);
    // iterate over all topics
    for (const auto& topic : myOptionsContainer->getSubTopics()) {
        // check if we have to ignore this topic
        if (myIgnoredTopics.count(topic) == 0) {
            // add topic into myTopicsTreeList
            myTopicsTreeList->appendItem(rootItem, topic.c_str());
            // iterate over entries
            const std::vector<std::string> entries = myOptionsContainer->getSubTopicsEntries(topic);
            for (const auto& entry : entries) {
                // check if we have to ignore this entry
                if (myIgnoredEntries.count(entry) == 0) {
                    // get type
                    const std::string type = myOptionsContainer->getTypeName(entry);
                    // get description
                    const std::string description = myOptionsContainer->getDescription(entry);
                    // continue depending of type
                    if (type == "STR") {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputString(this, myEntriesFrame, topic, entry, description));
                    } else if ((type == "FILE") || (type == "NETWORK") || (type == "ADDITIONAL") || (type == "ROUTE") || (type == "DATA")) {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputFilename(this, myEntriesFrame, topic, entry, description));
                    } else if (type == "BOOL") {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputBool(this, myEntriesFrame, topic, entry, description));
                    } else if (type == "INT") {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputInt(this, myEntriesFrame, topic, entry, description));
                    } else if (type == "FLOAT") {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputFloat(this, myEntriesFrame, topic, entry, description));
                    } else if (type == "INT[]") {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputIntVector(this, myEntriesFrame, topic, entry, description));
                    } else if (type == "STR[]") {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputStringVector(this, myEntriesFrame, topic, entry, description));
                    }
                }
            }
        }
    }
    // create search elements
    FXHorizontalFrame* searchFrame = new FXHorizontalFrame(contentFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(searchFrame, TL("Search"), nullptr, GUIDesignLabelThickedFixed(TREELISTWIDTH - GUIDesignHeight + 14));
    myDescriptionSearchCheckButton = new MFXCheckButtonTooltip(searchFrame, parent->getStaticTooltipMenu(), "", this, MID_GNE_SEARCH_USEDESCRIPTION, GUIDesignCheckButtonThick);
    myDescriptionSearchCheckButton->setToolTipText(TL("Include description in search"));
    mySearchButton = new MFXTextFieldSearch(searchFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // add separator
    new FXSeparator(contentFrame);
    // create buttons frame
    buttonsFrame = new FXHorizontalFrame(contentFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // continue depending of dialog type
    if (runDialog) {
        new FXButton(buttonsFrame, (TL("Run") + std::string("\t\t") + TL("Run tool")).c_str(), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_RUNNETGENERATE, GUIDesignButtonOK);
        new FXButton(buttonsFrame, (TL("Cancel") + std::string("\t\t") + TL("Cancel tool")).c_str(), GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, ID_CANCEL, GUIDesignButtonOK);
    } else {
        new FXButton(buttonsFrame, (TL("OK") + std::string("\t\t") + TL("Accept settings")).c_str(), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, ID_ACCEPT, GUIDesignButtonOK);
    }
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // create dialog
    create();
    // after creation, adjust entries name sizes
    for (const auto &entry : myInputOptionEntries) {
        entry->adjustNameSize();
    }
}

/****************************************************************************/