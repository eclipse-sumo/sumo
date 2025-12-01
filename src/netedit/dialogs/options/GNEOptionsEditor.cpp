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
/// @file    GNEOptionsEditor.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// A Dialog for setting options (see OptionsCont)
/****************************************************************************/
#include <config.h>

#include <fstream>

#include <netedit/dialogs/GNEDialog.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <utils/foxtools/MFXCheckButtonTooltip.h>
#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/foxtools/MFXStaticToolTip.h>
#include <utils/foxtools/MFXTextFieldSearch.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/options/OptionsLoader.h>
#include <xercesc/parsers/SAXParser.hpp>

#include "GNEOptionsEditor.h"

// ===========================================================================
// Defines
// ===========================================================================

#define TREELISTWIDTH 200

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEOptionsEditor) GNEOptionsEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,                 GNEOptionsEditor::onCmdSelectTopic),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SEARCH_USEDESCRIPTION,  GNEOptionsEditor::onCmdSearch),
    FXMAPFUNC(SEL_COMMAND,  MID_MTEXTFIELDSEARCH_UPDATED,   GNEOptionsEditor::onCmdSearch),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWTOOLTIPS_MENU,          GNEOptionsEditor::onCmdShowToolTipsMenu),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,               GNEOptionsEditor::onCmdSaveOptions),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,               GNEOptionsEditor::onCmdLoadOptions),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_DEFAULT,         GNEOptionsEditor::onCmdResetDefault),
};

// Object implementation
FXIMPLEMENT(GNEOptionsEditor, FXVerticalFrame, GNEOptionsEditorMap, ARRAYNUMBER(GNEOptionsEditorMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEOptionsEditor::GNEOptionsEditor(GNEDialog* dialog, const std::string& titleName, OptionsCont& optionsContainer,
                                   const OptionsCont& originalOptionsContainer) :
    FXVerticalFrame(dialog->getContentFrame(), GUIDesignAuxiliarFrame),
    myDialog(dialog),
    myOptionsContainer(optionsContainer),
    myCopyOfOptionsContainer(optionsContainer.clone()),
    myOriginalOptionsContainer(originalOptionsContainer) {
    // get staticTooltipMenu
    auto staticTooltipMenu = dialog->getApplicationWindow()->getStaticTooltipMenu();
    // add buttons frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrameNoPadding);
    myShowToolTipsMenu = new MFXCheckableButton(false, buttonsFrame, staticTooltipMenu,
            (std::string("\t") + TL("Toggle Menu Tooltips") + std::string("\t") + TL("Toggles whether tooltips in the menu shall be shown.")).c_str(),
            GUIIconSubSys::getIcon(GUIIcon::SHOWTOOLTIPS_MENU), this, MID_SHOWTOOLTIPS_MENU, GUIDesignMFXCheckableButtonSquare);
    auto saveFile = new MFXButtonTooltip(buttonsFrame, staticTooltipMenu, TL("Save options"),
                                         GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_CHOOSEN_SAVE, GUIDesignButtonConfiguration);
    saveFile->setTipText(TL("Save configuration file"));
    auto loadFile = new MFXButtonTooltip(buttonsFrame, staticTooltipMenu, TL("Load options"),
                                         GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_CHOOSEN_LOAD, GUIDesignButtonConfiguration);
    loadFile->setTipText(TL("Load configuration file"));
    auto resetDefault = new MFXButtonTooltip(buttonsFrame, staticTooltipMenu, TL("Default options"),
            GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_BUTTON_DEFAULT, GUIDesignButtonConfiguration);
    resetDefault->setTipText(TL("Reset all options to default"));
    // add separator
    new FXSeparator(this);
    // create elements frame
    FXHorizontalFrame* elementsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarFrame);
    FXVerticalFrame* elementsFrameTree = new FXVerticalFrame(elementsFrame, GUIDesignAuxiliarVerticalFrame);
    FXVerticalFrame* elementsFrameValues = new FXVerticalFrame(elementsFrame, GUIDesignAuxiliarFrame);
    // Create GroupBox modules
    MFXGroupBoxModule* groupBoxTree = new MFXGroupBoxModule(elementsFrameTree, TL("Topics"));
    MFXGroupBoxModule* groupBoxOptions = new MFXGroupBoxModule(elementsFrameValues, TL("Options"));
    // create FXTreeList
    myTopicsTreeList = new FXTreeList(groupBoxTree->getCollapsableFrame(), this, MID_GNE_SELECT, GUIDesignTreeListFixedWidth);
    myTopicsTreeList->setWidth(TREELISTWIDTH);
    // add root item
    myRootItem = myTopicsTreeList->appendItem(nullptr, titleName.c_str());
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
                    // check if is editable
                    const bool editable = myOptionsContainer.isEditable(entry);
                    // continue depending of type
                    if (type == "STR") {
                        myOptionRowEntries.push_back(new GNEOptionsEditorRow::OptionString(this, myEntriesFrame, topic, entry, description, defaultValue, editable));
                    } else if (type == "TIME") {
                        myOptionRowEntries.push_back(new GNEOptionsEditorRow::OptionTime(this, myEntriesFrame, topic, entry, description, defaultValue, editable));
                    } else if ((type == "FILE") || (type == "NETWORK") || (type == "ADDITIONAL") || (type == "ROUTE") || (type == "DATA")) {
                        myOptionRowEntries.push_back(new GNEOptionsEditorRow::OptionFilename(this, myEntriesFrame, topic, entry, description, defaultValue, editable));
                    } else if (type == "BOOL") {
                        myOptionRowEntries.push_back(new GNEOptionsEditorRow::OptionBool(this, myEntriesFrame, topic, entry, description, defaultValue, editable));
                    } else if (type == "INT") {
                        myOptionRowEntries.push_back(new GNEOptionsEditorRow::OptionInt(this, myEntriesFrame, topic, entry, description, defaultValue, editable));
                    } else if (type == "FLOAT") {
                        myOptionRowEntries.push_back(new GNEOptionsEditorRow::OptionFloat(this, myEntriesFrame, topic, entry, description, defaultValue, editable));
                    } else if (type == "INT[]") {
                        myOptionRowEntries.push_back(new GNEOptionsEditorRow::OptionIntVector(this, myEntriesFrame, topic, entry, description, defaultValue, editable));
                    } else if (type == "STR[]") {
                        myOptionRowEntries.push_back(new GNEOptionsEditorRow::OptionStringVector(this, myEntriesFrame, topic, entry, description, defaultValue, editable));
                    }
                }
            }
        }
    }
    // create search elements
    FXHorizontalFrame* searchFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(searchFrame, TL("Search"), nullptr, GUIDesignLabelThickedFixed(TREELISTWIDTH - GUIDesignHeight + 14));
    myDescriptionSearchCheckButton = new MFXCheckButtonTooltip(searchFrame, staticTooltipMenu, "", this, MID_GNE_SEARCH_USEDESCRIPTION, GUIDesignCheckButtonThick);
    myDescriptionSearchCheckButton->setToolTipText(TL("Include description in search"));
    mySearchButton = new MFXTextFieldSearch(searchFrame, staticTooltipMenu, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // after creation, adjust entries name sizes
    for (const auto& entry : myOptionRowEntries) {
        entry->adjustNameSize();
    }
    // set myShowToolTipsMenu
    myShowToolTipsMenu->setChecked(getApp()->reg().readIntEntry("gui", "menuToolTips", 0) != 1);
}


GNEOptionsEditor::~GNEOptionsEditor() {
    delete myCopyOfOptionsContainer;
}


void
GNEOptionsEditor::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // not finished yet
}


bool
GNEOptionsEditor::isOptionModified() const {
    return myOptionsModified;
}

void
GNEOptionsEditor::resetAllOptions() {
    for (const auto& entry : myOptionRowEntries) {
        entry->onCmdResetOption(nullptr, 0, nullptr);
    }
    myOptionsModified = false;
}


long
GNEOptionsEditor::onCmdSelectTopic(FXObject*, FXSelector, void*) {
    if (mySearchButton->getText().count() == 0) {
        updateVisibleEntriesByTopic();
    }
    return 1;
}


long
GNEOptionsEditor::onCmdSearch(FXObject*, FXSelector, void*) {
    if (mySearchButton->getText().count() > 0) {
        updateVisibleEntriesBySearch(mySearchButton->getText().text());
    } else {
        updateVisibleEntriesByTopic();
    }
    return 1;
}


long
GNEOptionsEditor::onCmdShowToolTipsMenu(FXObject*, FXSelector, void*) {
    // get staticTooltipMenu
    auto viewNet = myDialog->getApplicationWindow()->getViewNet();
    // toggle check
    myShowToolTipsMenu->setChecked(!myShowToolTipsMenu->amChecked());
    if (viewNet) {
        viewNet->getViewParent()->getShowToolTipsMenu()->setChecked(myShowToolTipsMenu->amChecked());
        viewNet->getViewParent()->getShowToolTipsMenu()->update();
    }
    // enable/disable static tooltip
    myDialog->getApplicationWindow()->getStaticTooltipMenu()->enableStaticToolTip(myShowToolTipsMenu->amChecked());
    // save in registry
    getApp()->reg().writeIntEntry("gui", "menuToolTips", myShowToolTipsMenu->amChecked() ? 0 : 1);
    update();

    return 1;
}


long
GNEOptionsEditor::onCmdSaveOptions(FXObject*, FXSelector, void*) {
    // open file dialog
    const GNEFileDialog optionsFileDialog(this, myDialog->getApplicationWindow(),
                                          TL("options file"),
                                          SUMOXMLDefinitions::XMLFileExtensions.getStrings(),
                                          GNEFileDialog::OpenMode::SAVE,
                                          GNEFileDialog::ConfigType::NETEDIT);
    // check file
    if (optionsFileDialog.getResult() == GNEDialog::Result::ACCEPT) {
        std::ofstream out(StringUtils::transcodeToLocal(optionsFileDialog.getFilename()));
        myOptionsContainer.writeConfiguration(out, true, false, false, optionsFileDialog.getFilename(), true);
        out.close();
    }
    return 1;
}


long
GNEOptionsEditor::onCmdLoadOptions(FXObject*, FXSelector, void*) {
    // open file dialog
    const GNEFileDialog optionsFileDialog(this, myDialog->getApplicationWindow(),
                                          TL("options file"),
                                          SUMOXMLDefinitions::XMLFileExtensions.getStrings(),
                                          GNEFileDialog::OpenMode::LOAD_SINGLE,
                                          GNEFileDialog::ConfigType::NETEDIT);
    // check file
    if ((optionsFileDialog.getResult() == GNEDialog::Result::ACCEPT) && loadConfiguration(optionsFileDialog.getFilename())) {
        // update entries
        for (const auto& entry : myOptionRowEntries) {
            entry->updateOption();
        }
    }
    return 1;
}


long
GNEOptionsEditor::onCmdResetDefault(FXObject*, FXSelector, void*) {
    // restore entries
    for (const auto& entry : myOptionRowEntries) {
        entry->restoreOption();
    }
    return 1;
}


GNEOptionsEditor::GNEOptionsEditor() :
    myOptionsContainer(OptionsCont::EMPTY_OPTIONS),
    myOriginalOptionsContainer(OptionsCont::EMPTY_OPTIONS) {
}


bool
GNEOptionsEditor::updateVisibleEntriesByTopic() {
    // iterate over tree elements and get the selected item
    for (const auto& treeItemTopic : myTreeItemTopics) {
        if (treeItemTopic.first->isSelected()) {
            // iterate over entries
            for (const auto& entry : myOptionRowEntries) {
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
    for (const auto& entry : myOptionRowEntries) {
        entry->show();
    }
    myEntriesFrame->recalc();
    myEntriesFrame->update();
    return true;
}


void
GNEOptionsEditor::updateVisibleEntriesBySearch(std::string searchText) {
    // first lower case search text
    searchText = StringUtils::to_lower_case(searchText);
    // iterate over entries
    for (const auto& entry : myOptionRowEntries) {
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
GNEOptionsEditor::loadConfiguration(const std::string& file) {
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

/****************************************************************************/
