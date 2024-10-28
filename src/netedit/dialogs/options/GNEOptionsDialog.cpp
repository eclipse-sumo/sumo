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
/// @file    GNEOptionsDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// A Dialog for setting options (see OptionsCont)
/****************************************************************************/
#include <config.h>

#include <algorithm>
#include <fstream>
#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/foxtools/MFXButtonTooltip.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsLoader.h>
#include <xercesc/parsers/SAXParser.hpp>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>

#include "GNEOptionsDialog.h"

#define TREELISTWIDTH 200

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEOptionsDialog) GUIDialogOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,                     GNEOptionsDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RESET,                  GNEOptionsDialog::onCmdReset),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RUNNETGENERATE,         GNEOptionsDialog::onCmdRunNetgenerate),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,                 GNEOptionsDialog::onCmdSelectTopic),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SEARCH_USEDESCRIPTION,  GNEOptionsDialog::onCmdSearch),
    FXMAPFUNC(SEL_COMMAND,  MID_MTEXTFIELDSEARCH_UPDATED,   GNEOptionsDialog::onCmdSearch),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWTOOLTIPS_MENU,          GNEOptionsDialog::onCmdShowToolTipsMenu),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,               GNEOptionsDialog::onCmdSaveOptions),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,               GNEOptionsDialog::onCmdLoadOptions),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_DEFAULT,         GNEOptionsDialog::onCmdResetDefault),
};

// Object implementation
FXIMPLEMENT(GNEOptionsDialog,   FXDialogBox,    GUIDialogOptionsMap,    ARRAYNUMBER(GUIDialogOptionsMap))

// ===========================================================================
// method definitions
// ===========================================================================

std::pair<int, bool>
GNEOptionsDialog::Options(GNEApplicationWindow* GNEApp, GUIIcon icon, OptionsCont& optionsContainer, const OptionsCont& originalOptionsContainer, const char* titleName) {
    GNEOptionsDialog* optionsDialog = new GNEOptionsDialog(GNEApp, icon, optionsContainer, originalOptionsContainer, titleName, false);
    auto result = std::make_pair(optionsDialog->execute(), optionsDialog->myOptionsModified);
    delete optionsDialog;
    return result;
}


std::pair<int, bool>
GNEOptionsDialog::Run(GNEApplicationWindow* GNEApp, GUIIcon icon, OptionsCont& optionsContainer, const OptionsCont& originalOptionsContainer, const char* titleName) {
    GNEOptionsDialog* optionsDialog = new GNEOptionsDialog(GNEApp, icon, optionsContainer, originalOptionsContainer, titleName, true);
    auto result = std::make_pair(optionsDialog->execute(), optionsDialog->myOptionsModified);
    delete optionsDialog;
    return result;
}


GNEOptionsDialog::~GNEOptionsDialog() { }


long
GNEOptionsDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // reset entries
    for (const auto& entry : myInputOptionEntries) {
        entry->onCmdResetOption(nullptr, 0, nullptr);
    }
    // close dialog canceling changes
    return handle(this, FXSEL(SEL_COMMAND, ID_CANCEL), nullptr);
}


long
GNEOptionsDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset entries
    for (const auto& entry : myInputOptionEntries) {
        entry->onCmdResetOption(nullptr, 0, nullptr);
    }
    return 1;
}


long
GNEOptionsDialog::onCmdRunNetgenerate(FXObject*, FXSelector, void*) {
    // close dialog accepting changes
    handle(this, FXSEL(SEL_COMMAND, ID_ACCEPT), nullptr);
    // run tool in mainWindow
    return myGNEApp->handle(this, FXSEL(SEL_COMMAND, MID_GNE_RUNNETGENERATE), nullptr);
}


long
GNEOptionsDialog::onCmdSelectTopic(FXObject*, FXSelector, void*) {
    if (mySearchButton->getText().count() == 0) {
        updateVisibleEntriesByTopic();
    }
    return 1;
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
GNEOptionsDialog::onCmdShowToolTipsMenu(FXObject*, FXSelector, void*) {
    // toggle check
    myShowToolTipsMenu->setChecked(!myShowToolTipsMenu->amChecked());
    if (myGNEApp->getViewNet()) {
        myGNEApp->getViewNet()->getViewParent()->getShowToolTipsMenu()->setChecked(myShowToolTipsMenu->amChecked());
        myGNEApp->getViewNet()->getViewParent()->getShowToolTipsMenu()->update();
    }
    // enable/disable static tooltip
    myGNEApp->getStaticTooltipMenu()->enableStaticToolTip(myShowToolTipsMenu->amChecked());
    // save in registry
    getApp()->reg().writeIntEntry("gui", "menuToolTips", myShowToolTipsMenu->amChecked() ? 0 : 1);
    update();

    return 1;
}


long
GNEOptionsDialog::onCmdSaveOptions(FXObject*, FXSelector, void*) {
    // open save dialog
    const std::string file = GNEApplicationWindowHelper::openOptionFileDialog(this, true);
    // check file
    if (file.size() > 0) {
        std::ofstream out(StringUtils::transcodeToLocal(file));
        myOptionsContainer.writeConfiguration(out, true, false, false, file, true);
        out.close();
    }
    return 1;
}


long
GNEOptionsDialog::onCmdLoadOptions(FXObject*, FXSelector, void*) {
    // open file dialog
    const std::string file = GNEApplicationWindowHelper::openOptionFileDialog(this, false);
    // check file
    if ((file.size() > 0) && loadConfiguration(file)) {
        // update entries
        for (const auto& entry : myInputOptionEntries) {
            entry->updateOption();
        }
    }
    return 1;
}


long
GNEOptionsDialog::onCmdResetDefault(FXObject*, FXSelector, void*) {
    // restore entries
    for (const auto& entry : myInputOptionEntries) {
        entry->restoreOption();
    }
    return 1;
}


GNEOptionsDialog::GNEOptionsDialog() :
    myGNEApp(nullptr),
    myOptionsContainer(OptionsCont::EMPTY_OPTIONS),
    myOriginalOptionsContainer(OptionsCont::EMPTY_OPTIONS) {
}


bool
GNEOptionsDialog::updateVisibleEntriesByTopic() {
    // iterate over tree elements and get the selected item
    for (const auto& treeItemTopic : myTreeItemTopics) {
        if (treeItemTopic.first->isSelected()) {
            // iterate over entries
            for (const auto& entry : myInputOptionEntries) {
                if (entry->getTopic() == treeItemTopic.second) {
                    entry->show();
                } else {
                    entry->hide();
                }
            }
            myEntriesFrame->recalc();
            myEntriesFrame->update();
            return true;
        }
    }
    // no topic selected, then show all
    for (const auto& entry : myInputOptionEntries) {
        entry->show();
    }
    myEntriesFrame->recalc();
    myEntriesFrame->update();
    return true;
}


void
GNEOptionsDialog::updateVisibleEntriesBySearch(std::string searchText) {
    // first lower case search text
    searchText = StringUtils::to_lower_case(searchText);
    // iterate over entries
    for (const auto& entry : myInputOptionEntries) {
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


bool
GNEOptionsDialog::loadConfiguration(const std::string& file) {
    // make all options writable
    myOptionsContainer.resetWritable();
    // build parser
    XERCES_CPP_NAMESPACE::SAXParser parser;
    parser.setValidationScheme(XERCES_CPP_NAMESPACE::SAXParser::Val_Never);
    parser.setDisableDefaultEntityResolution(true);
    // start the parsing
    OptionsLoader handler(myOptionsContainer);
    try {
        parser.setDocumentHandler(&handler);
        parser.setErrorHandler(&handler);
        parser.parse(StringUtils::transcodeToLocal(file).c_str());
        if (handler.errorOccurred()) {
            WRITE_ERROR(TL("Could not load configuration '") + file + "'.");
            return false;
        }
    } catch (const XERCES_CPP_NAMESPACE::XMLException& e) {
        WRITE_ERROR(TL("Could not load tool configuration '") + file + "':\n " + StringUtils::transcode(e.getMessage()));
        return false;
    }
    // write info
    WRITE_MESSAGE(TL("Loaded configuration."));
    return true;
}


GNEOptionsDialog::GNEOptionsDialog(GNEApplicationWindow* GNEApp, GUIIcon icon, OptionsCont& optionsContainer,
                                   const OptionsCont& originalOptionsContainer, const char* titleName, const bool runDialog) :
    FXDialogBox(GNEApp, titleName, GUIDesignDialogBoxExplicitStretchable(800, 600)),
    myGNEApp(GNEApp),
    myOptionsContainer(optionsContainer),
    myOriginalOptionsContainer(originalOptionsContainer) {
    // set icon
    setIcon(GUIIconSubSys::getIcon(icon));
    // create content frame
    FXVerticalFrame* contentFrame = new FXVerticalFrame(this, GUIDesignContentsFrame);
    // add buttons frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(contentFrame, GUIDesignHorizontalFrameNoPadding);
    myShowToolTipsMenu = new MFXCheckableButton(false, buttonsFrame,
            myGNEApp->getStaticTooltipMenu(), (std::string("\t") + TL("Toggle Menu Tooltips") + std::string("\t") + TL("Toggles whether tooltips in the menu shall be shown.")).c_str(),
            GUIIconSubSys::getIcon(GUIIcon::SHOWTOOLTIPS_MENU), this, MID_SHOWTOOLTIPS_MENU, GUIDesignMFXCheckableButtonSquare);
    auto saveFile = new MFXButtonTooltip(buttonsFrame, myGNEApp->getStaticTooltipMenu(), TL("Save options"),
                                         GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_CHOOSEN_SAVE, GUIDesignButtonConfiguration);
    saveFile->setTipText(TL("Save configuration file"));
    auto loadFile = new MFXButtonTooltip(buttonsFrame, myGNEApp->getStaticTooltipMenu(), TL("Load options"),
                                         GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_CHOOSEN_LOAD, GUIDesignButtonConfiguration);
    loadFile->setTipText(TL("Load configuration file"));
    auto resetDefault = new MFXButtonTooltip(buttonsFrame, myGNEApp->getStaticTooltipMenu(), TL("Default options"),
            GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_BUTTON_DEFAULT, GUIDesignButtonConfiguration);
    resetDefault->setTipText(TL("Reset all options to default"));
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
    // add root item
    myRootItem = myTopicsTreeList->appendItem(nullptr, titleName);
    myRootItem->setExpanded(TRUE);
    // create scroll
    FXScrollWindow* scrollTabEntries = new FXScrollWindow(groupBoxOptions->getCollapsableFrame(), LAYOUT_FILL_X | LAYOUT_FILL_Y);
    // create vertical frame for entries
    myEntriesFrame = new FXVerticalFrame(scrollTabEntries, GUIDesignAuxiliarFrame);
    // iterate over all topics
    for (const auto& topic : myOptionsContainer.getSubTopics()) {
        // check if we have to ignore this topic
        if (myIgnoredTopics.count(topic) == 0) {
            // add topic into myTreeItemTopics and tree
            myTreeItemTopics[myTopicsTreeList->appendItem(myRootItem, topic.c_str())] = topic;
            // iterate over entries
            const std::vector<std::string> entries = myOptionsContainer.getSubTopicsEntries(topic);
            for (const auto& entry : entries) {
                // check if we have to ignore this entry
                if (myIgnoredEntries.count(entry) == 0) {
                    // get type
                    const std::string type = myOptionsContainer.getTypeName(entry);
                    // get description
                    const std::string description = myOptionsContainer.getDescription(entry);
                    // get default value
                    const std::string defaultValue = myOptionsContainer.getValueString(entry);
                    // continue depending of type
                    if (type == "STR") {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputString(this, myEntriesFrame, topic, entry, description, defaultValue));
                    } else if (type == "TIME") {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputTime(this, myEntriesFrame, topic, entry, description, defaultValue));
                    } else if ((type == "FILE") || (type == "NETWORK") || (type == "ADDITIONAL") || (type == "ROUTE") || (type == "DATA")) {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputFilename(this, myEntriesFrame, topic, entry, description, defaultValue));
                    } else if (type == "BOOL") {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputBool(this, myEntriesFrame, topic, entry, description, defaultValue));
                    } else if (type == "INT") {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputInt(this, myEntriesFrame, topic, entry, description, defaultValue));
                    } else if (type == "FLOAT") {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputFloat(this, myEntriesFrame, topic, entry, description, defaultValue));
                    } else if (type == "INT[]") {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputIntVector(this, myEntriesFrame, topic, entry, description, defaultValue));
                    } else if (type == "STR[]") {
                        myInputOptionEntries.push_back(new GNEOptionsDialogElements::InputStringVector(this, myEntriesFrame, topic, entry, description, defaultValue));
                    }
                }
            }
        }
    }
    // create search elements
    FXHorizontalFrame* searchFrame = new FXHorizontalFrame(contentFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(searchFrame, TL("Search"), nullptr, GUIDesignLabelThickedFixed(TREELISTWIDTH - GUIDesignHeight + 14));
    myDescriptionSearchCheckButton = new MFXCheckButtonTooltip(searchFrame, myGNEApp->getStaticTooltipMenu(), "", this, MID_GNE_SEARCH_USEDESCRIPTION, GUIDesignCheckButtonThick);
    myDescriptionSearchCheckButton->setToolTipText(TL("Include description in search"));
    mySearchButton = new MFXTextFieldSearch(searchFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // add separator
    new FXSeparator(contentFrame);
    // create buttons frame
    buttonsFrame = new FXHorizontalFrame(contentFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // continue depending of dialog type
    if (runDialog) {
        GUIDesigns::buildFXButton(buttonsFrame, TL("Run"), "", "", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_RUNNETGENERATE, GUIDesignButtonOK);
    } else {
        GUIDesigns::buildFXButton(buttonsFrame, TL("OK"), "", "", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, ID_ACCEPT, GUIDesignButtonOK);
    }
    GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", "", GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_CANCEL, GUIDesignButtonOK);
    GUIDesigns::buildFXButton(buttonsFrame, TL("Reset"), "", "", GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_RESET, GUIDesignButtonOK);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // create dialog
    create();
    // after creation, adjust entries name sizes
    for (const auto& entry : myInputOptionEntries) {
        entry->adjustNameSize();
    }
    // set myShowToolTipsMenu
    myShowToolTipsMenu->setChecked(getApp()->reg().readIntEntry("gui", "menuToolTips", 0) != 1);
}

/****************************************************************************/
