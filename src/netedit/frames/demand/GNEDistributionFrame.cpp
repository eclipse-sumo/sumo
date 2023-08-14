/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GNEDistributionFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2023
///
// The Widget for edit type distribution elements
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/elements/demand/GNEVTypeDistribution.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEDistributionFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDistributionFrame::DistributionEditor) DistributionEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATE,     GNEDistributionFrame::DistributionEditor::onCmdCreateType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DELETE,     GNEDistributionFrame::DistributionEditor::onCmdDeleteType),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_DELETE,     GNEDistributionFrame::DistributionEditor::onUpdDeleteType),
};

FXDEFMAP(GNEDistributionFrame::DistributionSelector) DistributionSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNEDistributionFrame::DistributionSelector::onCmdSelectTypeDistribution),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SET_TYPE,   GNEDistributionFrame::DistributionSelector::onCmdUpdateTypeDistribution)
};


FXDEFMAP(GNEDistributionFrame::DistributionRow) DistributionRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEDistributionFrame::DistributionRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_REMOVE,  GNEDistributionFrame::DistributionRow::onCmdRemoveRow)
};

FXDEFMAP(GNEDistributionFrame::DistributionValuesEditor) DistributionValuesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ADD,   GNEDistributionFrame::DistributionValuesEditor::onCmdAddRow)
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

GNEDistributionFrame::DistributionEditor::DistributionEditor(GNEFrame* frameParent, GUIIcon icon) :
    MFXGroupBoxModule(frameParent, TL("Distribution Editor")),
    myFrameParent(frameParent) {
    // Create new vehicle type
    myCreateTypeButton = new FXButton(getCollapsableFrame(), TL("New"), GUIIconSubSys::getIcon(icon), this, MID_GNE_CREATE, GUIDesignButton);
    
    // A�adir TOOLTIP

    // Create delete/reset vehicle type
    myDeleteTypeButton = new FXButton(getCollapsableFrame(), TL("Delete"), GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), this, MID_GNE_DELETE, GUIDesignButton);
    // show type editor
    show();
}


GNEDistributionFrame::DistributionEditor::~DistributionEditor() {}


long
GNEDistributionFrame::DistributionEditor::onCmdCreateType(FXObject*, FXSelector, void*) {
    auto undoList = myFrameParent->getViewNet()->getUndoList();
    // obtain a new valid Type ID
    const std::string typeDistributionID = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->generateDemandElementID(myDistributionSelector->getDistributionTag());
    // create new vehicle type
    GNEDemandElement* type = new GNEVTypeDistribution(myFrameParent->getViewNet()->getNet(), typeDistributionID);
    // add it using undoList (to allow undo-redo)
    undoList->begin(type->getTagProperty().getGUIIcon(), "create vehicle type distribution");
    undoList->add(new GNEChange_DemandElement(type, true), true);
    undoList->end();
    // refresh type distribution
    myDistributionSelector->refreshDistributionSelector();
    return 1;
}


long
GNEDistributionFrame::DistributionEditor::onCmdDeleteType(FXObject*, FXSelector, void*) {
    auto undoList = myFrameParent->getViewNet()->getUndoList();
    auto currentDistribution = myDistributionSelector->getCurrentDistribution();
    if (currentDistribution) {
        // begin undo list operation
        undoList->begin(currentDistribution->getTagProperty().getGUIIcon(), "delete " + currentDistribution->getTagProperty().getTagStr() + " distribution");
        // remove vehicle type (and all of their children)
        myFrameParent->getViewNet()->getNet()->deleteDemandElement(myDistributionSelector->getCurrentDistribution(), undoList);
        // end undo list operation
        undoList->end();
        // refresh type distribution
        myDistributionSelector->refreshDistributionSelector();
            }
    return 1;
}


long
GNEDistributionFrame::DistributionEditor::onUpdDeleteType(FXObject* sender, FXSelector, void*) {
    // first check if selected VType is valid
    if (myDistributionSelector->getCurrentDistribution()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}

// ---------------------------------------------------------------------------
// GNETypeFrame::DistributionSelector - methods
// ---------------------------------------------------------------------------

GNEDistributionFrame::DistributionSelector::DistributionSelector(GNEFrame* frameParent, SumoXMLTag distributionTag) :
    MFXGroupBoxModule(frameParent, TL("Current type dist.")),
    myFrameParent(frameParent),
    myDistributionTag(distributionTag) {
    // Create FXComboBox
    myTypeComboBox = new FXComboBox(getCollapsableFrame(), GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // add default Types (always first)
    for (const auto& vType : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(myDistributionTag)) {
        myTypeComboBox->appendItem(vType->getID().c_str(), vType->getFXIcon());
    }
    // Set visible items
    if (myTypeComboBox->getNumItems() <= 20) {
        myTypeComboBox->setNumVisible((int)myTypeComboBox->getNumItems());
    } else {
        myTypeComboBox->setNumVisible(20);
    }
    // DistributionSelector is always shown
    show();
}


GNEDistributionFrame::DistributionSelector::~DistributionSelector() {}


SumoXMLTag
GNEDistributionFrame::DistributionSelector::getDistributionTag() const {
    return myDistributionTag;
}


GNEDemandElement*
GNEDistributionFrame::DistributionSelector::getCurrentDistribution() const {
    return myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(myDistributionTag, myCurrentTypeDistribution, false);
}


void
GNEDistributionFrame::DistributionSelector::refreshDistributionSelector() {
    // get ACs
    const auto& ACs = myFrameParent->getViewNet()->getNet()->getAttributeCarriers();
    // clear items
    myTypeComboBox->clearItems();
    // fill myTypeMatchBox with list of type distributions sorted by ID
    std::map<std::string, GNEDemandElement*> typeDistributions;
    for (const auto& vTypeDistribution : ACs->getDemandElements().at(myDistributionTag)) {
        typeDistributions[vTypeDistribution->getID()] = vTypeDistribution;
    }
    for (const auto& vTypeDistribution : typeDistributions) {
        myTypeComboBox->appendItem(vTypeDistribution.first.c_str(), vTypeDistribution.second->getFXIcon());
    }
    // Set visible items
    if (myTypeComboBox->getNumItems() <= 20) {
        myTypeComboBox->setNumVisible((int)myTypeComboBox->getNumItems());
    } else {
        myTypeComboBox->setNumVisible(20);
    }
    // check current type
    bool validCurrentTypeDistribution = false;
    for (int i = 0; i < (int)myTypeComboBox->getNumItems(); i++) {
        if (myTypeComboBox->getItem(i).text() == myCurrentTypeDistribution) {
            myTypeComboBox->setCurrentItem(i);
            validCurrentTypeDistribution = true;
        }
    }
    // Check that give vType type is valid
    GNEDemandElement* vTypeDistribution = nullptr;
    if (validCurrentTypeDistribution) {
        vTypeDistribution = ACs->retrieveDemandElement(myDistributionTag, myCurrentTypeDistribution);
    } else {
        vTypeDistribution = ACs->retrieveFirstDemandElement(myDistributionTag);
    }
    // Check that give vType type is valid
    if (vTypeDistribution) {
        myCurrentTypeDistribution = vTypeDistribution->getID();
        // set myCurrentType as inspected element
        myFrameParent->getViewNet()->setInspectedAttributeCarriers({vTypeDistribution});
        // show modules
        myAttributesEditor->showAttributeEditorModule(true);
        myDistributionValuesEditor->showAttributeEditorModule();
    } else {
        myCurrentTypeDistribution.clear();
        // set myCurrentType as inspected element
        myFrameParent->getViewNet()->setInspectedAttributeCarriers({});
        // hide modules
        myAttributesEditor->hideAttributesEditorModule();
        myDistributionValuesEditor->hideDistributionValuesEditorModule();
    }
}


long
GNEDistributionFrame::DistributionSelector::onCmdSelectTypeDistribution(FXObject*, FXSelector, void*) {
    const auto viewNet = myFrameParent->getViewNet();
    const auto& vTypeDistributions = viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(myDistributionTag);
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& vTypeDistribution : vTypeDistributions) {
        if (vTypeDistribution->getID() == myTypeComboBox->getText().text()) {
            // set pointer
            myCurrentTypeDistribution = vTypeDistribution->getID();
            // set color of myTypeMatchBox to black (valid)
            myTypeComboBox->setTextColor(FXRGB(0, 0, 0));
            // set myCurrentType as inspected element
            viewNet->setInspectedAttributeCarriers({vTypeDistribution});
            // show modules
            myAttributesEditor->showAttributeEditorModule(true);
            myDistributionValuesEditor->showAttributeEditorModule();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeComboBox->getText() + "' in DistributionSelector").text());
            // update viewNet
            viewNet->updateViewNet();
            return 1;
        }
    }
    myCurrentTypeDistribution.clear();
    // hide modules
    myAttributesEditor->hideAttributesEditorModule();
    myDistributionValuesEditor->hideDistributionValuesEditorModule();
    // set color of myTypeMatchBox to red (invalid)
    myTypeComboBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in DistributionSelector");
    // update viewNet
    viewNet->updateViewNet();
    return 1;
}


long
GNEDistributionFrame::DistributionSelector::onCmdUpdateTypeDistribution(FXObject* sender, FXSelector, void*) {
    const auto& demandElements = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements();
    if (demandElements.at(myDistributionTag).size() > 0) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
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
    // create and hide color editor
    myDeleteRowButton = new MFXButtonTooltip(this, staticTooltipMenu,
        "", GUIIconSubSys::getIcon(GUIIcon::REMOVE), this, MID_GNE_BUTTON_REMOVE, GUIDesignButtonIcon);
    // Create and hide MFXTextFieldTooltip for string attributes
    myComboBoxKeys = new MFXComboBoxIcon(this, GUIDesignComboBoxNCol, true, this, MID_GNE_SET_TYPE, GUIDesignComboBoxAttribute);
    // Create and hide MFXTextFieldTooltip for string attributes
    myProbabilityTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldFixedRestricted(50, TEXTFIELD_REAL));
    // only create if parent was created
    if (getParent()->id() && attributeEditorParent->myDistributionSelector->getCurrentDistribution()) {
        // create DistributionRow
        FXHorizontalFrame::create();
        // fill comboBox with all possible keys
        const auto possibleKeys = attributeEditorParent->myDistributionSelector->getCurrentDistribution()->getPossibleDistributionKeys(myDistributionValuesEditorParent->myDistributionValueTag);
        for (int i = 0; i < (int)possibleKeys.size(); i++) {
            myComboBoxKeys->appendIconItem(possibleKeys[i]->getID().c_str(), possibleKeys[i]->getFXIcon());
            if (possibleKeys[i] == myKey) {
                myComboBoxKeys->setCurrentItem(i);
            }
        }
        // adjust combo Box
        myComboBoxKeys->setNumVisible(myComboBoxKeys->getNumItems() <= 10? myComboBoxKeys->getNumItems() : 10);
        myComboBoxKeys->setTextColor(FXRGB(0, 0, 0));
        myComboBoxKeys->killFocus();
        // set probability
        myProbabilityTextField->setText(toString(myProbability).c_str());
        myProbabilityTextField->setTextColor(FXRGB(0, 0, 0));
        myProbabilityTextField->killFocus();
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
GNEDistributionFrame::DistributionRow::refreshDistributionRow(const GNEDemandElement* key, const double value) {
    // set key
    myComboBoxKeys->setText(key->getID().c_str());
    // set probability
    myProbabilityTextField->setText(toString(value).c_str());
    myProbabilityTextField->setTextColor(FXRGB(0, 0, 0));
    myProbabilityTextField->killFocus();
}


bool
GNEDistributionFrame::DistributionRow::isDistributionRowValid() const {
    return (myComboBoxKeys->getTextColor() == FXRGB(0, 0, 0));
}


long
GNEDistributionFrame::DistributionRow::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // get Undo list
    GNEUndoList* undoList = myDistributionValuesEditorParent->getFrameParent()->getViewNet()->getUndoList();
    // get current distribution
    auto currentDistribution = myDistributionValuesEditorParent->myDistributionSelector->getCurrentDistribution();
    // get ACs
    const auto &ACs = myDistributionValuesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers();
    // continue if we have a distribution to edit
    if (currentDistribution == nullptr) {
        return 1;
    }
    // continue depending of calle dobj
    if (obj == myComboBoxKeys) {
        if (isValidKey()) {
            myComboBoxKeys->setTextColor(FXRGB(0, 0, 0));
            // get new key
            const auto newKey = ACs->retrieveDemandElement(myDistributionValuesEditorParent->myDistributionValueTag, myComboBoxKeys->getText().text());
            // only change if is different of current key
            if (myKey != newKey) {
                // change distribution key removing and adding it
                undoList->begin(myKey, "edit distribution key");
                currentDistribution->removeDistributionKey(myKey, undoList);
                myKey = ACs->retrieveDemandElement(myDistributionValuesEditorParent->myDistributionValueTag, myComboBoxKeys->getText().text());
                currentDistribution->addDistributionKey(myKey, myProbability, undoList);
                undoList->end();
            }
            // refresh rows
            myDistributionValuesEditorParent->refreshAttributeEditor();
        } else {
            myComboBoxKeys->setBackColor(FXRGB(255, 255, 255));
            myComboBoxKeys->killFocus();
        }
    } else if (obj == myProbabilityTextField) {
        // get probability
        const std::string probabilityStr = myProbabilityTextField->getText().text();
        const double probability = GNEAttributeCarrier::canParse<double>(probabilityStr)? GNEAttributeCarrier::parse<double>(probabilityStr) : -1;
        // Check if set new probability
        if (probability >= 0) {
            myProbabilityTextField->setTextColor(FXRGB(0, 0, 0));
            currentDistribution->editDistributionValue(myKey, probability, undoList);
        } else {
            myProbabilityTextField->setBackColor(FXRGB(255, 255, 255));
            myProbabilityTextField->killFocus();
        }
    }
    return 1;
}


long
GNEDistributionFrame::DistributionRow::onCmdRemoveRow(FXObject*, FXSelector, void*) {

    return 1;
}


bool
GNEDistributionFrame::DistributionRow::isValidKey() const {
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
    MFXGroupBoxModule(frameParent, TL("Internal attributes")),
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
    // resize myDistributionRows
    myDistributionRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // Create help button
    myAddButton = new FXButton(getCollapsableFrame(), "", GUIIconSubSys::getIcon(GUIIcon::ADD), this, MID_GNE_BUTTON_ADD, GUIDesignButtonIcon);
}


void
GNEDistributionFrame::DistributionValuesEditor::showAttributeEditorModule() {
    // first remove all rows
    for (auto& row : myDistributionRows) {
        // destroy and delete all rows
        if (row != nullptr) {
            row->destroy();
            delete row;
            row = nullptr;
        }
    }
    // continue if we have a distribution to edit
    if (myDistributionSelector->getCurrentDistribution()) {
        // refresh attribute editor
        refreshAttributeEditor();
        // show DistributionValuesEditor
        show();
    }
    // reparent help button (to place it at bottom)
    myAddButton->reparent(this);
}


void
GNEDistributionFrame::DistributionValuesEditor::hideDistributionValuesEditorModule() {
    // hide also DistributionValuesEditor
    hide();
}


void
GNEDistributionFrame::DistributionValuesEditor::refreshAttributeEditor() {
    // continue if we have a distribution to edit
    if (myDistributionSelector->getCurrentDistribution()) {
        // first remove all rows
        for (auto& row : myDistributionRows) {
            // destroy and delete all rows
            if (row != nullptr) {
                row->destroy();
                delete row;
                row = nullptr;
            }
        }
        // Iterate over distribution key-values
        for (const auto& keyValue : myDistributionSelector->getCurrentDistribution()->getDistributionKeyValues()) {
            // create distribution row
            auto distributionRow = new DistributionRow(this, keyValue.first, keyValue.second);
            // add into distribution rows
            myDistributionRows.push_back(distributionRow);
        }
    }
}


GNEFrame*
GNEDistributionFrame::DistributionValuesEditor::getFrameParent() const {
    return myFrameParent;
}


long
GNEDistributionFrame::DistributionValuesEditor::onCmdAddRow(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
