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
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/elements/demand/GNEVTypeDistribution.h>
#include <netedit/dialogs/GNEVehicleTypeDialog.h>
#include <netedit/dialogs/GNEVTypeDistributionsDialog.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEDistributionFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDistributionFrame::DistributionEditor) DistributionEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATE,    GNEDistributionFrame::DistributionEditor::onCmdCreateType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DELETE,    GNEDistributionFrame::DistributionEditor::onCmdDeleteType),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_DELETE,    GNEDistributionFrame::DistributionEditor::onUpdDeleteType),
};

FXDEFMAP(GNEDistributionFrame::DistributionSelector) DistributionSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNEDistributionFrame::DistributionSelector::onCmdSelectTypeDistribution),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SET_TYPE,   GNEDistributionFrame::DistributionSelector::onCmdUpdateTypeDistribution)
};

FXDEFMAP(GNEDistributionFrame::AttributeRow) AttributeRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEDistributionFrame::AttributeRow::onCmdSetAttribute),
};

FXDEFMAP(GNEDistributionFrame::DistributionRow) DistributionRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEDistributionFrame::DistributionRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_REMOVE,  GNEDistributionFrame::DistributionRow::onCmdRemoveRow)
};

FXDEFMAP(GNEDistributionFrame::AttributesEditor) AttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ADD,   GNEDistributionFrame::AttributesEditor::onCmdAddRow)
};

// Object implementation
FXIMPLEMENT(GNEDistributionFrame::DistributionEditor,   MFXGroupBoxModule,  DistributionEditorMap,      ARRAYNUMBER(DistributionEditorMap))
FXIMPLEMENT(GNEDistributionFrame::DistributionSelector, MFXGroupBoxModule,  DistributionSelectorMap,    ARRAYNUMBER(DistributionSelectorMap))
FXIMPLEMENT(GNEDistributionFrame::AttributeRow,         FXHorizontalFrame,  AttributeRowMap,            ARRAYNUMBER(AttributeRowMap))
FXIMPLEMENT(GNEDistributionFrame::DistributionRow,      FXHorizontalFrame,  DistributionRowMap,         ARRAYNUMBER(DistributionRowMap))
FXIMPLEMENT(GNEDistributionFrame::AttributesEditor,     MFXGroupBoxModule,  AttributesEditorMap,        ARRAYNUMBER(AttributesEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDistributionFrame::DistributionEditor - methods
// ---------------------------------------------------------------------------

GNEDistributionFrame::DistributionEditor::DistributionEditor(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Distribution Editor")),
    myFrameParent(frameParent) {
    // Create new vehicle type
    myCreateTypeButton = new FXButton(getCollapsableFrame(), TL("New"), GUIIconSubSys::getIcon(GUIIcon::VTYPEDISTRIBUTION), this, MID_GNE_CREATE, GUIDesignButton);
    
    // Añadir TOOLTIP

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
    const std::string typeDistributionID = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->generateDemandElementID(SUMO_TAG_VTYPE_DISTRIBUTION);
    // create new vehicle type
    GNEDemandElement* type = new GNEVTypeDistribution(myFrameParent->getViewNet()->getNet(), typeDistributionID);
    // add it using undoList (to allow undo-redo)
    undoList->begin(GUIIcon::VTYPEDISTRIBUTION, "create vehicle type distribution");
    undoList->add(new GNEChange_DemandElement(type, true), true);
    undoList->end();
    // refresh type distribution
    myFrameParent->myDistributionSelector->refreshDistributionSelector();
    return 1;
}


long
GNEDistributionFrame::DistributionEditor::onCmdDeleteType(FXObject*, FXSelector, void*) {
    auto undoList = myFrameParent->getViewNet()->getUndoList();
    // begin undo list operation
    undoList->begin(GUIIcon::VTYPE, "delete vehicle type distribution");
    // remove vehicle type (and all of their children)
    myFrameParent->getViewNet()->getNet()->deleteDemandElement(myFrameParent->myDistributionEditor->getDistribution(), undoList);
    // end undo list operation
    undoList->end();
    // refresh type distribution
    myFrameParent->myDistributionSelector->refreshDistributionSelector();
    return 1;
}


long
GNEDistributionFrame::DistributionEditor::onUpdDeleteType(FXObject* sender, FXSelector, void*) {
    // first check if selected VType is valid
    if (myFrameParent->myDistributionEditor->getDistribution()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}

// ---------------------------------------------------------------------------
// GNETypeFrame::DistributionSelector - methods
// ---------------------------------------------------------------------------

GNEDistributionFrame::DistributionSelector::DistributionSelector(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Current type dist.")),
    myFrameParent(frameParent) {
    // Create FXComboBox
    myTypeComboBox = new FXComboBox(getCollapsableFrame(), GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // add default Types (always first)
    for (const auto& vType : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE_DISTRIBUTION)) {
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


void
GNEDistributionFrame::DistributionSelector::refreshDistributionSelector() {
    // get ACs
    const auto& ACs = myFrameParent->getViewNet()->getNet()->getAttributeCarriers();
    // clear items
    myTypeComboBox->clearItems();
    // fill myTypeMatchBox with list of type distributions sorted by ID
    std::map<std::string, GNEDemandElement*> typeDistributions;
    for (const auto& vTypeDistribution : ACs->getDemandElements().at(SUMO_TAG_VTYPE_DISTRIBUTION)) {
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
        vTypeDistribution = ACs->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, myCurrentTypeDistribution);
    } else {
        vTypeDistribution = ACs->retrieveFirstDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION);
    }
    // Check that give vType type is valid
    if (vTypeDistribution) {
        myCurrentTypeDistribution = vTypeDistribution->getID();
        myFrameParent->myDistributionEditor->setDistribution(vTypeDistribution);
        // set myCurrentType as inspected element
        myFrameParent->getViewNet()->setInspectedAttributeCarriers({vTypeDistribution});
        // show modules
        myFrameParent->myDistributionEditor->showAttributeEditorModule();
    } else {
        myCurrentTypeDistribution.clear();
        myFrameParent->myDistributionEditor->setDistribution(nullptr);
        // set myCurrentType as inspected element
        myFrameParent->getViewNet()->setInspectedAttributeCarriers({});
        // hide modules
        myFrameParent->myDistributionEditor->hideAttributesEditorModule();
    }
}


long
GNEDistributionFrame::DistributionSelector::onCmdSelectTypeDistribution(FXObject*, FXSelector, void*) {
    const auto viewNet = myFrameParent->getViewNet();
    const auto& vTypeDistributions = viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE_DISTRIBUTION);
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& vTypeDistribution : vTypeDistributions) {
        if (vTypeDistribution->getID() == myTypeComboBox->getText().text()) {
            // set pointer
            myCurrentTypeDistribution = vTypeDistribution->getID();
            myFrameParent->myDistributionEditor->setDistribution(vTypeDistribution);
            // set color of myTypeMatchBox to black (valid)
            myTypeComboBox->setTextColor(FXRGB(0, 0, 0));
            // set myCurrentType as inspected element
            viewNet->setInspectedAttributeCarriers({vTypeDistribution});
            // show modules if selected item is valid
            myFrameParent->myDistributionEditor->showAttributeEditorModule();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeComboBox->getText() + "' in DistributionSelector").text());
            // update viewNet
            viewNet->updateViewNet();
            return 1;
        }
    }
    myCurrentTypeDistribution.clear();
    myFrameParent->myDistributionEditor->setDistribution(nullptr);
    // hide all modules if selected item isn't valid
    myFrameParent->myDistributionEditor->hideAttributesEditorModule();
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
    if (demandElements.at(SUMO_TAG_VTYPE_DISTRIBUTION).size() > 0) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}

// ---------------------------------------------------------------------------
// GNEDistributionFrame::AttributeRow - methods
// ---------------------------------------------------------------------------

GNEDistributionFrame::AttributeRow::AttributeRow(
        GNEDistributionFrame::AttributesEditor* attributeEditorParent,
        const GNEAttributeProperties& ACAttr, const std::string& attribute) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent),
    myACAttr(ACAttr) {
    // get staticTooltip menu
    auto staticTooltipMenu = attributeEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // Create attribute label (usually used only for ID)
    myAttributeLabel = new MFXLabelTooltip(this, staticTooltipMenu,
        ACAttr.getAttrStr().c_str(), nullptr, GUIDesignLabelThickedFixed(GUIDesignHeight));
    // Create and hide MFXTextFieldTooltip for string attributes
    myValueTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // only create if parent was created
    if (getParent()->id()) {
        // create AttributeRow
        FXHorizontalFrame::create();
        // Show attribute ACAttr.getAttrStr().c_str());
        myAttributeLabel->setTipText(ACAttr.getDefinition().c_str());
        // show value
        myValueTextField->setText(attribute.c_str());
        myValueTextField->setTextColor(FXRGB(0, 0, 0));
        myValueTextField->killFocus();
        // Show AttributeRow
        show();
    }
}


void
GNEDistributionFrame::AttributeRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


void
GNEDistributionFrame::AttributeRow::refreshAttributeRow(const std::string& value) {
    // set last valid value and restore color if onlyValid is disabled
    myValueTextField->setText(value.c_str());
    // set blue color if is an computed value
    myValueTextField->setTextColor(FXRGB(0, 0, 0));
    myValueTextField->killFocus();
}


bool
GNEDistributionFrame::AttributeRow::isAttributeRowValid() const {
    return (myValueTextField->getTextColor() == FXRGB(0, 0, 0));
}


long
GNEDistributionFrame::AttributeRow::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    // obtain value of myValueTextField
    newVal = myValueTextField->getText().text();
    // get current distribution
    auto currentDistribution = myAttributesEditorParent->getDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution) {
        // Check if attribute must be changed
        if (currentDistribution->isValid(myACAttr.getAttr(), newVal)) {
            // set attribute
            currentDistribution->setAttribute(myACAttr.getAttr(), newVal, myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList());
            // update text field
            myValueTextField->setTextColor(FXRGB(0, 0, 0));
            myValueTextField->setBackColor(FXRGB(255, 255, 255));
            myValueTextField->killFocus();
            // in this case, we need to refresh the other values (For example, allow/Disallow objects)
            myAttributesEditorParent->refreshAttributeEditor();
            // update frame parent after attribute successfully set
            myAttributesEditorParent->getFrameParent()->attributeUpdated(myACAttr.getAttr());
        } else {
            myValueTextField->setTextColor(FXRGB(255, 0, 0));
            if (newVal.empty()) {
                myValueTextField->setBackColor(FXRGBA(255, 213, 213, 255));
            }
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG("Value '" + newVal + "' for attribute " + myACAttr.getAttrStr() + " of Distribution isn't valid");
        }
    }
    return 1;
}


GNEDistributionFrame::AttributeRow::AttributeRow() :
    myAttributesEditorParent(nullptr) {
}

// ---------------------------------------------------------------------------
// GNEDistributionFrame::DistributionRow - methods
// ---------------------------------------------------------------------------

GNEDistributionFrame::DistributionRow::DistributionRow(
        AttributesEditor* attributeEditorParent, const GNEDemandElement* key, const double probability) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent),
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
    if (getParent()->id()) {
        // create DistributionRow
        FXHorizontalFrame::create();
        // fill comboBox with all possible keys
        const auto possibleKeys = attributeEditorParent->getDistribution()->getPossibleDistributionKeys(SUMO_TAG_VTYPE);
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
    GNEUndoList* undoList = myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList();
    // get current distribution
    auto currentDistribution = myAttributesEditorParent->getDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution == nullptr) {
        return 1;
    }
    // continue depending of calle dobj
    if (obj == myComboBoxKeys) {
        if (isValidKey()) {
            myComboBoxKeys->setTextColor(FXRGB(0, 0, 0));
            // get new key
            const auto newKey = myAttributesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, myComboBoxKeys->getText().text());
            // only change if is different of current key
            if (myKey != newKey) {
                // change distribution key removing and adding it
                undoList->begin(myKey, "edit distribution key");
                currentDistribution->removeDistributionKey(myKey, undoList);
                myKey = myAttributesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, myComboBoxKeys->getText().text());
                currentDistribution->addDistributionKey(myKey, myProbability, undoList);
                undoList->end();
            }
            // refresh rows
            myAttributesEditorParent->refreshAttributeEditor();
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


GNEDistributionFrame::DistributionRow::DistributionRow() :
    myAttributesEditorParent(nullptr) {
}


bool
GNEDistributionFrame::DistributionRow::isValidKey() const {
    // get element associated with key
    const auto element = myAttributesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, myComboBoxKeys->getText().text(), false);
    // first check if element exists
    if (element) {
        // avoid duplicated keys
        return !myKey->keyExists(element);
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEDistributionFrame::AttributesEditor - methods
// ---------------------------------------------------------------------------

GNEDistributionFrame::AttributesEditor::AttributesEditor(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Internal attributes")),
    myFrameParent(frameParent) {
    // resize myDistributionRows
    myDistributionRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // Create help button
    myAddButton = new FXButton(getCollapsableFrame(), "", GUIIconSubSys::getIcon(GUIIcon::ADD), this, MID_GNE_BUTTON_ADD, GUIDesignButtonIcon);
}


void
GNEDistributionFrame::AttributesEditor::showAttributeEditorModule() {
    // first remove all rows
    for (auto& row : myDistributionRows) {
        // destroy and delete all rows
        if (row != nullptr) {
            row->destroy();
            delete row;
            row = nullptr;
        }
    }
    // also destroy ID
    if (myIDAttributeRow) {
        myIDAttributeRow->destroy();
        delete myIDAttributeRow;
    }
    // continue if we have a distribution to edit
    if (myDistribution) {
        // create ID row
        myIDAttributeRow = new AttributeRow(this, myDistribution->getTagProperty().getAttributeProperties(SUMO_ATTR_ID), myDistribution->getAttribute(SUMO_ATTR_ID));
        // refresh attribute editor
        refreshAttributeEditor();
        // show AttributesEditor
        show();
    }
    // reparent help button (to place it at bottom)
    myAddButton->reparent(this);
}


void
GNEDistributionFrame::AttributesEditor::hideAttributesEditorModule() {
    // hide also AttributesEditor
    hide();
}


void
GNEDistributionFrame::AttributesEditor::refreshAttributeEditor() {
    // continue if we have a distribution to edit
    if (myDistribution) {
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
        for (const auto& keyValue : myDistribution->getDistributionKeyValues()) {
            // create distribution row
            auto distributionRow = new DistributionRow(this, keyValue.first, keyValue.second);
            // add into distribution rows
            myDistributionRows.push_back(distributionRow);
        }
    }
}


GNEFrame*
GNEDistributionFrame::AttributesEditor::getFrameParent() const {
    return myFrameParent;
}


GNEDemandElement*
GNEDistributionFrame::AttributesEditor::getDistribution() const {
    return myDistribution;
}


void
GNEDistributionFrame::AttributesEditor::setDistribution(GNEDemandElement* distribution) {
    myDistribution = distribution;
}


long
GNEDistributionFrame::AttributesEditor::onCmdAddRow(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
