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
/// @file    GNEDistributionFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2023
///
// The Widget for edit distribution elements
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/elements/demand/GNEVTypeDistribution.h>
#include <netedit/elements/demand/GNERouteDistribution.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEDistributionFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDistributionFrame::DistributionEditor) DistributionEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATE,     GNEDistributionFrame::DistributionEditor::onCmdCreateDistribution),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DELETE,     GNEDistributionFrame::DistributionEditor::onCmdDeleteDistribution),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_DELETE,     GNEDistributionFrame::DistributionEditor::onUpdDeleteDistribution),
};

FXDEFMAP(GNEDistributionFrame::DistributionSelector) DistributionSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNEDistributionFrame::DistributionSelector::onCmdSelectDistribution),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SET_TYPE,   GNEDistributionFrame::DistributionSelector::onCmdUpdateDistribution)
};


FXDEFMAP(GNEDistributionFrame::DistributionRow) DistributionRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,       GNEDistributionFrame::DistributionRow::onCmdSetKey),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEDistributionFrame::DistributionRow::onCmdSetProbability),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_REMOVE,  GNEDistributionFrame::DistributionRow::onCmdRemoveRow)
};

FXDEFMAP(GNEDistributionFrame::DistributionValuesEditor) DistributionValuesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ADD,   GNEDistributionFrame::DistributionValuesEditor::onCmdAddRow),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_BUTTON_ADD,   GNEDistributionFrame::DistributionValuesEditor::onUpdAddRow)
};

// Object implementation
FXIMPLEMENT(GNEDistributionFrame::DistributionEditor,       MFXGroupBoxModule,  DistributionEditorMap,          ARRAYNUMBER(DistributionEditorMap))
FXIMPLEMENT(GNEDistributionFrame::DistributionSelector,     MFXGroupBoxModule,  DistributionSelectorMap,        ARRAYNUMBER(DistributionSelectorMap))
FXIMPLEMENT(GNEDistributionFrame::DistributionRow,          FXHorizontalFrame,  DistributionRowMap,             ARRAYNUMBER(DistributionRowMap))
FXIMPLEMENT(GNEDistributionFrame::DistributionValuesEditor, MFXGroupBoxModule,  DistributionValuesEditorMap,    ARRAYNUMBER(DistributionValuesEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDistributionFrame::DistributionEditor - methods
// ---------------------------------------------------------------------------

GNEDistributionFrame::DistributionEditor::DistributionEditor(GNEFrame* frameParent, SumoXMLTag distributionTag, GUIIcon icon) :
    MFXGroupBoxModule(frameParent, TL("Distribution Editor")),
    myFrameParent(frameParent),
    myDistributionTag(distributionTag) {
    // get staticTooltip menu
    auto staticTooltipMenu = myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // Create new distribution
    myCreateDistributionButton = new MFXButtonTooltip(getCollapsableFrame(), staticTooltipMenu, TL("New"),
            GUIIconSubSys::getIcon(icon), this, MID_GNE_CREATE, GUIDesignButton);
    myCreateDistributionButton->setTipText(TLF("Create new %", toString(myDistributionTag)).c_str()),
                               // Delete distribution
                               myDeleteDistributionButton = new MFXButtonTooltip(getCollapsableFrame(), staticTooltipMenu, TL("Delete"),
                                       GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), this, MID_GNE_DELETE, GUIDesignButton);
    myDeleteDistributionButton->setTipText(TLF("Delete current edited %", toString(myDistributionTag)).c_str()),
                               // show editor
                               show();
}


GNEDistributionFrame::DistributionEditor::~DistributionEditor() {}


SumoXMLTag
GNEDistributionFrame::DistributionEditor::getDistributionTag() const {
    return myDistributionTag;
}


long
GNEDistributionFrame::DistributionEditor::onCmdCreateDistribution(FXObject*, FXSelector, void*) {
    auto undoList = myFrameParent->getViewNet()->getUndoList();
    // obtain a new valid ID
    const auto distributionID = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->generateDemandElementID(myDistributionTag);
    // create new distribution
    GNEDemandElement* distribution = nullptr;
    if (myDistributionTag == SUMO_TAG_VTYPE_DISTRIBUTION) {
        distribution = new GNEVTypeDistribution(myFrameParent->getViewNet()->getNet(), distributionID, -1);
    } else if (myDistributionTag == SUMO_TAG_ROUTE_DISTRIBUTION) {
        distribution = new GNERouteDistribution(myFrameParent->getViewNet()->getNet(), distributionID);
    } else {
        throw ProcessError("Invalid distribution");
    }
    // add it using undoList (to allow undo-redo)
    undoList->begin(distribution->getTagProperty().getGUIIcon(), "create distribution");
    undoList->add(new GNEChange_DemandElement(distribution, true), true);
    undoList->end();
    // refresh selector using created distribution
    myDistributionSelector->setDistribution(distribution);
    return 1;
}


long
GNEDistributionFrame::DistributionEditor::onCmdDeleteDistribution(FXObject*, FXSelector, void*) {
    auto undoList = myFrameParent->getViewNet()->getUndoList();
    auto currentDistribution = myDistributionSelector->getCurrentDistribution();
    if (currentDistribution) {
        // begin undo list operation
        undoList->begin(currentDistribution->getTagProperty().getGUIIcon(), "delete " + currentDistribution->getTagProperty().getTagStr() + " distribution");
        // remove distribution
        myFrameParent->getViewNet()->getNet()->deleteDemandElement(myDistributionSelector->getCurrentDistribution(), undoList);
        // end undo list operation
        undoList->end();
        // refresh selector
        myDistributionSelector->refreshDistributionSelector();
    }
    return 1;
}


long
GNEDistributionFrame::DistributionEditor::onUpdDeleteDistribution(FXObject* sender, FXSelector, void*) {
    // check if we have a selected distribution
    if (myDistributionSelector->getCurrentDistribution()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}

// ---------------------------------------------------------------------------
// GNETypeFrame::DistributionSelector - methods
// ---------------------------------------------------------------------------

GNEDistributionFrame::DistributionSelector::DistributionSelector(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Distribution selector")),
    myFrameParent(frameParent) {
    // Create MFXComboBoxIcon
    myDistributionsComboBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItemsMedium,
            this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // DistributionSelector is always shown
    show();
}


GNEDistributionFrame::DistributionSelector::~DistributionSelector() {}


void
GNEDistributionFrame::DistributionSelector::setDistribution(GNEDemandElement* distribution) {
    myCurrentDistribution = distribution;
    refreshDistributionSelector();
}


GNEDemandElement*
GNEDistributionFrame::DistributionSelector::getCurrentDistribution() const {
    return myCurrentDistribution;

}


void
GNEDistributionFrame::DistributionSelector::refreshDistributionIDs() {
    // fill distributions
    fillDistributionComboBox();
    // set current item
    for (int i = 0; i < (int)myDistributionsComboBox->getNumItems(); i++) {
        if (myDistributionsComboBox->getItemText(i) == myCurrentDistribution->getID()) {
            myDistributionsComboBox->setCurrentItem(i);
        }
    }
}


void
GNEDistributionFrame::DistributionSelector::refreshDistributionSelector() {
    // fill distributions
    const auto distributions = fillDistributionComboBox();
    // update current distribution (used if myCurrentDistribution was deleted during undo-redo)
    myCurrentDistribution = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(myCurrentDistribution, false);
    // update comboBox
    if (myCurrentDistribution) {
        for (int i = 0; i < (int)myDistributionsComboBox->getNumItems(); i++) {
            if (myDistributionsComboBox->getItemText(i) == myCurrentDistribution->getID()) {
                myDistributionsComboBox->setCurrentItem(i);
            }
        }
    } else if (distributions.size() > 0) {
        // set first distribution
        myCurrentDistribution = distributions.begin()->second;
    }
    // continue depending of myCurrentDistribution
    if (myCurrentDistribution) {
        // set distribtution as inspected element
        myFrameParent->getViewNet()->setInspectedAttributeCarriers({myCurrentDistribution});
        // show modules
        myAttributesEditor->showAttributeEditorModule(true);
        myDistributionValuesEditor->showDistributionValuesEditor();
    } else {
        // remove inspected elements
        myFrameParent->getViewNet()->setInspectedAttributeCarriers({});
        // hide modules
        myAttributesEditor->hideAttributesEditorModule();
        myDistributionValuesEditor->hideDistributionValuesEditor();
    }
}


long
GNEDistributionFrame::DistributionSelector::onCmdSelectDistribution(FXObject*, FXSelector, void*) {
    const auto viewNet = myFrameParent->getViewNet();
    const auto& distributions = viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(myDistributionEditor->getDistributionTag());
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& distribution : distributions) {
        if (distribution.second->getID() == myDistributionsComboBox->getText().text()) {
            // set pointer
            myCurrentDistribution = distribution.second;
            // set color of myTypeMatchBox to black (valid)
            myDistributionsComboBox->setTextColor(FXRGB(0, 0, 0));
            // set myCurrentType as inspected element
            viewNet->setInspectedAttributeCarriers({distribution.second});
            // show modules
            myAttributesEditor->showAttributeEditorModule(true);
            myDistributionValuesEditor->showDistributionValuesEditor();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myDistributionsComboBox->getText() + "' in DistributionSelector").text());
            // update viewNet
            viewNet->updateViewNet();
            return 1;
        }
    }
    // not found, then reset myCurrentDistribution
    myCurrentDistribution = nullptr;
    // hide modules
    myAttributesEditor->hideAttributesEditorModule();
    myDistributionValuesEditor->hideDistributionValuesEditor();
    // set color of myTypeMatchBox to red (invalid)
    myDistributionsComboBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in DistributionSelector");
    // update viewNet
    viewNet->updateViewNet();
    return 1;
}


long
GNEDistributionFrame::DistributionSelector::onCmdUpdateDistribution(FXObject* sender, FXSelector, void*) {
    const auto& demandElements = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements();
    if (demandElements.at(myDistributionEditor->getDistributionTag()).size() > 0) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}


std::map<std::string, GNEDemandElement*>
GNEDistributionFrame::DistributionSelector::fillDistributionComboBox() {
    // get ACs
    const auto& ACs = myFrameParent->getViewNet()->getNet()->getAttributeCarriers();
    // clear items
    myDistributionsComboBox->clearItems();
    // fill with distributions sorted by ID
    std::map<std::string, GNEDemandElement*> distributions;
    for (const auto& distribution : ACs->getDemandElements().at(myDistributionEditor->getDistributionTag())) {
        distributions[distribution.second->getID()] = distribution.second;
    }
    for (const auto& distribution : distributions) {
        myDistributionsComboBox->appendIconItem(distribution.first.c_str(), distribution.second->getACIcon());
    }
    // return distributions sorted by ID
    return distributions;
}

// ---------------------------------------------------------------------------
// GNEDistributionFrame::DistributionRow - methods
// ---------------------------------------------------------------------------

GNEDistributionFrame::DistributionRow::DistributionRow(DistributionValuesEditor* attributeEditorParent, const GNEDemandElement* key, const double probability) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myDistributionValuesEditorParent(attributeEditorParent),
    myKey(key),
    myProbability(probability) {
    // get staticTooltip menu
    auto staticTooltipMenu = attributeEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // create label
    myIconLabel = new FXLabel(this, "", key->getACIcon(), GUIDesignLabelIconThick);
    // Create and hide MFXTextFieldTooltip for string attributes
    myComboBoxKeys = new MFXComboBoxIcon(this, GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItemsMedium,
                                         this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // Create and hide MFXTextFieldTooltip for string attributes
    myProbabilityTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
            GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldFixedRestricted(50, TEXTFIELD_REAL));
    // create delete buton
    myDeleteRowButton = new MFXButtonTooltip(this, staticTooltipMenu,
            "", GUIIconSubSys::getIcon(GUIIcon::REMOVE), this, MID_GNE_BUTTON_REMOVE, GUIDesignButtonIcon);
    myDeleteRowButton->setTipText(TL("Delete distribution value"));
    // only create if parent was created
    if (getParent()->id() && attributeEditorParent->myDistributionSelector->getCurrentDistribution()) {
        // create DistributionRow
        FXHorizontalFrame::create();
        // refresh row
        refreshRow();
        // Show DistributionRow
        show();
    }
}


void
GNEDistributionFrame::DistributionRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


void
GNEDistributionFrame::DistributionRow::refreshRow() {
    // get distribution selector
    const auto currentDistribution = myDistributionValuesEditorParent->myDistributionSelector->getCurrentDistribution();
    // get possible keys
    const auto possibleKeys = currentDistribution->getPossibleDistributionKeys(myDistributionValuesEditorParent->myDistributionValueTag);
    // refill combo Box with possible values
    myComboBoxKeys->clearItems();
    myComboBoxKeys->appendIconItem(myKey->getID().c_str());
    for (const auto& possibleKey : possibleKeys) {
        myComboBoxKeys->appendIconItem(possibleKey.first.c_str());
    }
    myComboBoxKeys->setCurrentItem(0);
    // adjust combo Box
    myComboBoxKeys->setTextColor(FXRGB(0, 0, 0));
    myComboBoxKeys->killFocus();
    // set probability
    myProbabilityTextField->setText(toString(myProbability).c_str());
    myProbabilityTextField->setTextColor(FXRGB(0, 0, 0));
    myProbabilityTextField->killFocus();
}


double
GNEDistributionFrame::DistributionRow::getProbability() const {
    return myProbability;
}


long
GNEDistributionFrame::DistributionRow::onCmdSetKey(FXObject*, FXSelector, void*) {
    // get Undo list
    GNEUndoList* undoList = myDistributionValuesEditorParent->getFrameParent()->getViewNet()->getUndoList();
    // get current distribution
    auto currentDistribution = myDistributionValuesEditorParent->myDistributionSelector->getCurrentDistribution();
    // get ACs
    const auto& ACs = myDistributionValuesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers();
    // continue if we have a distribution to edit
    if (currentDistribution == nullptr) {
        return 1;
    }
    // check if new key is valid
    if (isValidNewKey()) {
        myComboBoxKeys->setTextColor(FXRGB(0, 0, 0));
        // get new key
        const auto newKey = ACs->retrieveDemandElement(myDistributionValuesEditorParent->myDistributionValueTag, myComboBoxKeys->getText().text());
        // only change if is different of current key
        if (myKey != newKey) {
            // begin undo list
            undoList->begin(myKey, "edit distribution key");
            // remove distribution key
            currentDistribution->removeDistributionKey(myKey, undoList);
            // sert key and icon
            myKey = ACs->retrieveDemandElement(myDistributionValuesEditorParent->myDistributionValueTag, myComboBoxKeys->getText().text());
            myIconLabel->setIcon(myKey->getACIcon());
            // add distribution key (and probability)
            currentDistribution->addDistributionKey(myKey, myProbability, undoList);
            // end undo list
            undoList->end();
            // refresh all rows
            myDistributionValuesEditorParent->refreshRows();
        }
    } else {
        myComboBoxKeys->setTextColor(FXRGB(255, 0, 0));
        myComboBoxKeys->killFocus();
    }
    return 1;
}


long
GNEDistributionFrame::DistributionRow::onCmdSetProbability(FXObject*, FXSelector, void*) {
    // get current distribution
    auto currentDistribution = myDistributionValuesEditorParent->myDistributionSelector->getCurrentDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution == nullptr) {
        return 1;
    }
    // get probability
    const std::string probabilityStr = myProbabilityTextField->getText().text();
    const double probability = GNEAttributeCarrier::canParse<double>(probabilityStr) ? GNEAttributeCarrier::parse<double>(probabilityStr) : -1;
    // Check if set new probability
    if (probability >= 0) {
        // set new probability
        myProbability = probability;
        // edit distribution value
        currentDistribution->editDistributionValue(myKey, probability, myDistributionValuesEditorParent->getFrameParent()->getViewNet()->getUndoList());
        // reset color
        myProbabilityTextField->setTextColor(FXRGB(0, 0, 0));
        // update sum label
        myDistributionValuesEditorParent->updateSumLabel();
    } else {
        myProbabilityTextField->setTextColor(FXRGB(255, 0, 0));
        myProbabilityTextField->killFocus();
    }
    return 1;
}


long
GNEDistributionFrame::DistributionRow::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    // get current distribution
    auto currentDistribution = myDistributionValuesEditorParent->myDistributionSelector->getCurrentDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution == nullptr) {
        return 1;
    }
    // remove distribution key
    currentDistribution->removeDistributionKey(myKey, myDistributionValuesEditorParent->getFrameParent()->getViewNet()->getUndoList());
    // remake rows
    myDistributionValuesEditorParent->remakeRows();
    return 1;
}


bool
GNEDistributionFrame::DistributionRow::isValidNewKey() const {
    const auto ACs = myDistributionValuesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers();
    // get element associated with key
    const auto element = ACs->retrieveDemandElement(myDistributionValuesEditorParent->myDistributionValueTag, myComboBoxKeys->getText().text(), false);
    // first check if element exists
    if (element) {
        // avoid duplicated keys
        return !myKey->keyExists(element);
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEDistributionFrame::DistributionValuesEditor - methods
// ---------------------------------------------------------------------------

GNEDistributionFrame::DistributionValuesEditor::DistributionValuesEditor(GNEFrame* frameParent, DistributionEditor* distributionEditor,
        DistributionSelector* distributionSelector, GNEFrameAttributeModules::AttributesEditor* attributesEditor, SumoXMLTag distributionValueTag) :
    MFXGroupBoxModule(frameParent, TL("Distribution values")),
    myFrameParent(frameParent),
    myDistributionEditor(distributionEditor),
    myDistributionSelector(distributionSelector),
    myAttributesEditor(attributesEditor),
    myDistributionValueTag(distributionValueTag) {
    // set relations
    myDistributionEditor->myDistributionSelector = myDistributionSelector;
    myDistributionSelector->myDistributionEditor = myDistributionEditor;
    myDistributionSelector->myAttributesEditor = myAttributesEditor;
    myDistributionSelector->myDistributionValuesEditor = this;
    // get staticTooltip menu
    auto staticTooltipMenu = frameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // Create bot frame elements
    myBotFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    auto addButton = new MFXButtonTooltip(myBotFrame, staticTooltipMenu, "", GUIIconSubSys::getIcon(GUIIcon::ADD), this, MID_GNE_BUTTON_ADD, GUIDesignButtonIcon);
    addButton->setTipText(TL("Add new distribution value"));
    new FXHorizontalFrame(myBotFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myBotFrame, "", GUIIconSubSys::getIcon(GUIIcon::SUM), GUIDesignLabelIconThick);
    mySumLabel = new FXLabel(myBotFrame, "", nullptr, GUIDesignLabelThickedFixed(50));
    new FXLabel(myBotFrame, "", GUIIconSubSys::getIcon(GUIIcon::EMPTY), GUIDesignLabelFixed(GUIDesignHeight));
}


void
GNEDistributionFrame::DistributionValuesEditor::showDistributionValuesEditor() {
    // remake rows
    remakeRows();
    // show DistributionValuesEditor
    show();
}


void
GNEDistributionFrame::DistributionValuesEditor::hideDistributionValuesEditor() {
    // hide also DistributionValuesEditor
    hide();
}


void
GNEDistributionFrame::DistributionValuesEditor::remakeRows() {
    // first remove all rows
    for (auto& row : myDistributionRows) {
        // destroy and delete all rows
        if (row != nullptr) {
            row->destroy();
            delete row;
            row = nullptr;
        }
    }
    myDistributionRows.clear();
    // continue if we have a distribution to edit
    if (myDistributionSelector->getCurrentDistribution()) {
        // Iterate over distribution key-values
        for (const auto& keyValue : myDistributionSelector->getCurrentDistribution()->getDistributionKeyValues()) {
            // create distribution row
            auto distributionRow = new DistributionRow(this, keyValue.first, keyValue.second);
            // add into distribution rows
            myDistributionRows.push_back(distributionRow);
        }
    }
    // reparent bot frame button (to place it at bottom)
    myBotFrame->reparent(getCollapsableFrame());
}


void
GNEDistributionFrame::DistributionValuesEditor::refreshRows() {
    // refresh rows
    for (const auto& row : myDistributionRows) {
        row->refreshRow();
    }
}


GNEFrame*
GNEDistributionFrame::DistributionValuesEditor::getFrameParent() const {
    return myFrameParent;
}


void
GNEDistributionFrame::DistributionValuesEditor::updateSumLabel() {
    // update probability
    double sumProbability = 0;
    for (const auto& row : myDistributionRows) {
        sumProbability += row->getProbability();
    }
    mySumLabel->setText(toString(sumProbability).c_str());
}


long
GNEDistributionFrame::DistributionValuesEditor::onCmdAddRow(FXObject*, FXSelector, void*) {
    if (myDistributionSelector->getCurrentDistribution() == nullptr) {
        return 1;
    }
    // get next free key
    const auto possibleKeys = myDistributionSelector->getCurrentDistribution()->getPossibleDistributionKeys(myDistributionValueTag);
    if (possibleKeys.empty()) {
        return 1;
    }
    // add first possible key
    myDistributionSelector->getCurrentDistribution()->addDistributionKey(possibleKeys.begin()->second, 0.5, myFrameParent->getViewNet()->getUndoList());
    // remake rows
    remakeRows();
    return 1;
}


long
GNEDistributionFrame::DistributionValuesEditor::onUpdAddRow(FXObject* sender, FXSelector, void*) {
    if (myDistributionSelector->getCurrentDistribution() == nullptr) {
        mySumLabel->setText("");
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        // update sum label
        updateSumLabel();
        // enable or disable add button depending of existents distributions
        if (myDistributionSelector->getCurrentDistribution()->getPossibleDistributionKeys(myDistributionValueTag).size() > 0) {
            return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
        } else {
            return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
        }
    }
}

/****************************************************************************/
