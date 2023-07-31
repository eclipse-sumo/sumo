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
/// @file    GNETypeDistributionFrame.cpp
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

#include "GNETypeDistributionFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNETypeDistributionFrame::TypeDistributionEditor) typeDistributionEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATE,    GNETypeDistributionFrame::TypeDistributionEditor::onCmdCreateType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DELETE,    GNETypeDistributionFrame::TypeDistributionEditor::onCmdDeleteType),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_DELETE,    GNETypeDistributionFrame::TypeDistributionEditor::onUpdDeleteType),
};

FXDEFMAP(GNETypeDistributionFrame::TypeDistributionSelector) typeDistributionSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNETypeDistributionFrame::TypeDistributionSelector::onCmdSelectTypeDistribution),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SET_TYPE,   GNETypeDistributionFrame::TypeDistributionSelector::onCmdUpdateTypeDistribution)
};

FXDEFMAP(GNETypeDistributionFrame::TypeDistributionAttributesEditorRow) TypeDistributionAttributesEditorRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNETypeDistributionFrame::TypeDistributionAttributesEditorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_REMOVE,  GNETypeDistributionFrame::TypeDistributionAttributesEditorRow::onCmdRemoveRow)
};

FXDEFMAP(GNETypeDistributionFrame::TypeDistributionAttributesEditor) TypeDistributionAttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ADD,   GNETypeDistributionFrame::TypeDistributionAttributesEditor::onCmdAddRow)
};

// Object implementation
FXIMPLEMENT(GNETypeDistributionFrame::TypeDistributionEditor,               MFXGroupBoxModule,  typeDistributionEditorMap,              ARRAYNUMBER(typeDistributionEditorMap))
FXIMPLEMENT(GNETypeDistributionFrame::TypeDistributionSelector,             MFXGroupBoxModule,  typeDistributionSelectorMap,            ARRAYNUMBER(typeDistributionSelectorMap))
FXIMPLEMENT(GNETypeDistributionFrame::TypeDistributionAttributesEditorRow,  FXHorizontalFrame,  TypeDistributionAttributesEditorRowMap, ARRAYNUMBER(TypeDistributionAttributesEditorRowMap))
FXIMPLEMENT(GNETypeDistributionFrame::TypeDistributionAttributesEditor,     MFXGroupBoxModule,  TypeDistributionAttributesEditorMap,    ARRAYNUMBER(TypeDistributionAttributesEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNETypeFrame::TypeDistributionEditor - methods
// ---------------------------------------------------------------------------

GNETypeDistributionFrame::TypeDistributionEditor::TypeDistributionEditor(GNETypeDistributionFrame* typeDistributionFrameParent) :
    MFXGroupBoxModule(typeDistributionFrameParent, TL("Type Editor")),
    myTypeDistributionFrameParent(typeDistributionFrameParent) {
    // Create new vehicle type
    myCreateTypeButton = new FXButton(getCollapsableFrame(), TL("Create type distribution"), GUIIconSubSys::getIcon(GUIIcon::VTYPEDISTRIBUTION), this, MID_GNE_CREATE, GUIDesignButton);
    // Create delete/reset vehicle type
    myDeleteTypeButton = new FXButton(getCollapsableFrame(), TL("Delete type distribution"), GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), this, MID_GNE_DELETE, GUIDesignButton);
    // show type editor
    show();
}


GNETypeDistributionFrame::TypeDistributionEditor::~TypeDistributionEditor() {}


long
GNETypeDistributionFrame::TypeDistributionEditor::onCmdCreateType(FXObject*, FXSelector, void*) {
    auto viewNet = myTypeDistributionFrameParent->myViewNet;
    // obtain a new valid Type ID
    const std::string typeDistributionID = viewNet->getNet()->getAttributeCarriers()->generateDemandElementID(SUMO_TAG_VTYPE_DISTRIBUTION);
    // create new vehicle type
    GNEDemandElement* type = new GNEVTypeDistribution(viewNet->getNet(), typeDistributionID);
    // add it using undoList (to allow undo-redo)
    viewNet->getUndoList()->begin(GUIIcon::VTYPEDISTRIBUTION, "create vehicle type distribution");
    viewNet->getUndoList()->add(new GNEChange_DemandElement(type, true), true);
    viewNet->getUndoList()->end();
    return 1;
}


long
GNETypeDistributionFrame::TypeDistributionEditor::onCmdDeleteType(FXObject*, FXSelector, void*) {
    auto viewNet = myTypeDistributionFrameParent->myViewNet;
    // begin undo list operation
    viewNet->getUndoList()->begin(GUIIcon::VTYPE, "delete vehicle type distribution");
    // remove vehicle type (and all of their children)
    viewNet->getNet()->deleteDemandElement(myTypeDistributionFrameParent->myTypeDistributionSelector->getCurrentTypeDistribution(), viewNet->getUndoList());
    // end undo list operation
    viewNet->getUndoList()->end();
    return 1;
}


long
GNETypeDistributionFrame::TypeDistributionEditor::onUpdDeleteType(FXObject* sender, FXSelector, void*) {
    // first check if selected VType is valid
    if (myTypeDistributionFrameParent->myTypeDistributionSelector->getCurrentTypeDistribution()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}

// ---------------------------------------------------------------------------
// GNETypeFrame::TypeDistributionSelector - methods
// ---------------------------------------------------------------------------

GNETypeDistributionFrame::TypeDistributionSelector::TypeDistributionSelector(GNETypeDistributionFrame* typeFrameParent) :
    MFXGroupBoxModule(typeFrameParent, TL("Current Type")),
    myTypeDistributionFrameParent(typeFrameParent) {
    // Create FXComboBox
    myTypeComboBox = new FXComboBox(getCollapsableFrame(), GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // add default Types (always first)
    for (const auto& vType : myTypeDistributionFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE_DISTRIBUTION)) {
        myTypeComboBox->appendItem(vType->getID().c_str(), vType->getFXIcon());
    }
    // Set visible items
    if (myTypeComboBox->getNumItems() <= 20) {
        myTypeComboBox->setNumVisible((int)myTypeComboBox->getNumItems());
    } else {
        myTypeComboBox->setNumVisible(20);
    }
    // TypeDistributionSelector is always shown
    show();
}


GNETypeDistributionFrame::TypeDistributionSelector::~TypeDistributionSelector() {}


GNEDemandElement*
GNETypeDistributionFrame::TypeDistributionSelector::getCurrentTypeDistribution() const {
    return myTypeDistributionFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, myCurrentTypeDistribution, false);
}


void
GNETypeDistributionFrame::TypeDistributionSelector::setCurrentTypeDistribution(const GNEDemandElement* vTypeDistribution) {
    myCurrentTypeDistribution = vTypeDistribution->getID();
    refreshTypeDistributionSelector();
}


void
GNETypeDistributionFrame::TypeDistributionSelector::refreshTypeDistributionSelector() {
    // get ACs
    const auto& ACs = myTypeDistributionFrameParent->getViewNet()->getNet()->getAttributeCarriers();
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
        // set myCurrentType as inspected element
        myTypeDistributionFrameParent->getViewNet()->setInspectedAttributeCarriers({vTypeDistribution});
        // show modules
        myTypeDistributionFrameParent->myTypeDistributionAttributesEditor->showAttributeEditorModule();
    } else {
        myCurrentTypeDistribution.clear();
        // set myCurrentType as inspected element
        myTypeDistributionFrameParent->getViewNet()->setInspectedAttributeCarriers({});
        // hide modules
        myTypeDistributionFrameParent->myTypeDistributionAttributesEditor->hideTypeDistributionAttributesEditorModule();
    }
}


long
GNETypeDistributionFrame::TypeDistributionSelector::onCmdSelectTypeDistribution(FXObject*, FXSelector, void*) {
    const auto viewNet = myTypeDistributionFrameParent->getViewNet();
    const auto& vTypeDistributions = viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE_DISTRIBUTION);
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& vTypeDistribution : vTypeDistributions) {
        if (vTypeDistribution->getID() == myTypeComboBox->getText().text()) {
            // set pointer
            myCurrentTypeDistribution = vTypeDistribution->getID();
            // set color of myTypeMatchBox to black (valid)
            myTypeComboBox->setTextColor(FXRGB(0, 0, 0));
            // set myCurrentType as inspected element
            viewNet->setInspectedAttributeCarriers({vTypeDistribution});
            // show modules if selected item is valid
            myTypeDistributionFrameParent->myTypeDistributionAttributesEditor->showAttributeEditorModule();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeComboBox->getText() + "' in TypeDistributionSelector").text());
            // update viewNet
            viewNet->updateViewNet();
            return 1;
        }
    }
    myCurrentTypeDistribution.clear();
    // hide all modules if selected item isn't valid
    myTypeDistributionFrameParent->myTypeDistributionAttributesEditor->hideTypeDistributionAttributesEditorModule();
    // set color of myTypeMatchBox to red (invalid)
    myTypeComboBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in TypeDistributionSelector");
    // update viewNet
    viewNet->updateViewNet();
    return 1;
}


long
GNETypeDistributionFrame::TypeDistributionSelector::onCmdUpdateTypeDistribution(FXObject* sender, FXSelector, void*) {
    const auto& demandElements = myTypeDistributionFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements();
    if (demandElements.at(SUMO_TAG_VTYPE_DISTRIBUTION).size() > 0) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}

// ---------------------------------------------------------------------------
// GNETypeDistributionFrame::TypeDistributionAttributesEditorRow - methods
// ---------------------------------------------------------------------------

GNETypeDistributionFrame::TypeDistributionAttributesEditorRow::TypeDistributionAttributesEditorRow(
        GNETypeDistributionFrame::TypeDistributionAttributesEditor* attributeEditorParent,
        const GNEAttributeProperties& ACAttr, const std::string& id) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myTypeDistributionAttributesEditorParent(attributeEditorParent) {
    // get staticTooltip menu
    auto staticTooltipMenu = attributeEditorParent->getTypeDistributionFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // Create attribute label (usually used only for ID)
    myIDLabel = new MFXLabelTooltip(this, staticTooltipMenu,
        ACAttr.getAttrStr().c_str(), nullptr, GUIDesignLabelThickedFixed(GUIDesignHeight));
    // Create and hide MFXTextFieldTooltip for string attributes
    myValueTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // only create if parent was created
    if (getParent()->id()) {
        // create TypeDistributionAttributesEditorRow
        FXHorizontalFrame::create();
        // Show attribute ACAttr.getAttrStr().c_str());
        myIDLabel->setTipText(ACAttr.getDefinition().c_str());
        // In any other case (String, list, etc.), show value as String
        myValueTextField->setText(id.c_str());
        myValueTextField->setTextColor(FXRGB(0, 0, 0));
        myValueTextField->killFocus();
        // Show TypeDistributionAttributesEditorRow
        show();
    }
}


GNETypeDistributionFrame::TypeDistributionAttributesEditorRow::TypeDistributionAttributesEditorRow(
        TypeDistributionAttributesEditor* attributeEditorParent, const std::string& type, const std::string& probability) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myTypeDistributionAttributesEditorParent(attributeEditorParent) {
    // get staticTooltip menu
    auto staticTooltipMenu = attributeEditorParent->getTypeDistributionFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // create and hide color editor
    myDeleteRowButton = new MFXButtonTooltip(this, staticTooltipMenu,
        "", GUIIconSubSys::getIcon(GUIIcon::REMOVE), this, MID_GNE_BUTTON_REMOVE, GUIDesignButtonIcon);
    // Create and hide MFXTextFieldTooltip for string attributes
    myValueTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // Create and hide MFXTextFieldTooltip for string attributes
    myProbabilityTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    // only create if parent was created
    if (getParent()->id()) {
        // create TypeDistributionAttributesEditorRow
        FXHorizontalFrame::create();
        // set type
        myValueTextField->setText(type.c_str());
        myValueTextField->setTextColor(FXRGB(0, 0, 0));
        myValueTextField->killFocus();
        // set probability
        myProbabilityTextField->setText(probability.c_str());
        myProbabilityTextField->setTextColor(FXRGB(0, 0, 0));
        myProbabilityTextField->killFocus();
        // Show TypeDistributionAttributesEditorRow
        show();
    }
}


void
GNETypeDistributionFrame::TypeDistributionAttributesEditorRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


void
GNETypeDistributionFrame::TypeDistributionAttributesEditorRow::refreshTypeDistributionAttributesEditorRow(const std::string& value) {
    // set last valid value and restore color if onlyValid is disabled
    myValueTextField->setText(value.c_str());
    // set blue color if is an computed value
    myValueTextField->setTextColor(FXRGB(0, 0, 0));
    myValueTextField->killFocus();
}


bool
GNETypeDistributionFrame::TypeDistributionAttributesEditorRow::isTypeDistributionAttributesEditorRowValid() const {
    return (myValueTextField->getTextColor() == FXRGB(0, 0, 0));
}


long
GNETypeDistributionFrame::TypeDistributionAttributesEditorRow::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    // obtain value of myValueTextField
    newVal = myValueTextField->getText().text();
    // get current distribution
    auto currentDistribution = myTypeDistributionAttributesEditorParent->getTypeDistributionFrameParent()->getTypeDistributionSelector()->getCurrentTypeDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution) {
        // Check if attribute must be changed
        if (currentDistribution->isValid(SUMO_ATTR_ID, newVal)) {
            // set attribute
            currentDistribution->setAttribute(SUMO_ATTR_ID, newVal, myTypeDistributionAttributesEditorParent->getTypeDistributionFrameParent()->getViewNet()->getUndoList());
            // update text field
            myValueTextField->setTextColor(FXRGB(0, 0, 0));
            myValueTextField->setBackColor(FXRGB(255, 255, 255));
            myValueTextField->killFocus();
            // in this case, we need to refresh the other values (For example, allow/Disallow objects)
            myTypeDistributionAttributesEditorParent->refreshAttributeEditor();
            // update frame parent after attribute successfully set
            myTypeDistributionAttributesEditorParent->getTypeDistributionFrameParent()->attributeUpdated(SUMO_ATTR_ID);
        } else {
            myValueTextField->setTextColor(FXRGB(255, 0, 0));
            if (newVal.empty()) {
                myValueTextField->setBackColor(FXRGBA(255, 213, 213, 255));
            }
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(TL("Value '") + newVal + TL("' for attribute ID of Distribution isn't valid"));
        }
    }
    return 1;
}


long
GNETypeDistributionFrame::TypeDistributionAttributesEditorRow::onCmdRemoveRow(FXObject*, FXSelector, void*) {

    return 1;
}


GNETypeDistributionFrame::TypeDistributionAttributesEditorRow::TypeDistributionAttributesEditorRow() :
    myTypeDistributionAttributesEditorParent(nullptr) {
}

// ---------------------------------------------------------------------------
// GNETypeDistributionFrame::TypeDistributionAttributesEditor - methods
// ---------------------------------------------------------------------------

GNETypeDistributionFrame::TypeDistributionAttributesEditor::TypeDistributionAttributesEditor(GNETypeDistributionFrame* typeDistributionFrameParent) :
    MFXGroupBoxModule(typeDistributionFrameParent, TL("Internal attributes")),
    myTypeDistributionFrameParent(typeDistributionFrameParent) {
    // resize myTypeDistributionAttributesEditorRows
    myTypeDistributionAttributesEditorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // Create help button
    myAddButton = new FXButton(getCollapsableFrame(), "", GUIIconSubSys::getIcon(GUIIcon::ADD), this, MID_GNE_BUTTON_ADD, GUIDesignButtonIcon);
}


void
GNETypeDistributionFrame::TypeDistributionAttributesEditor::showAttributeEditorModule() {
    // first remove all rows
    for (auto& row : myTypeDistributionAttributesEditorRows) {
        // destroy and delete all rows
        if (row != nullptr) {
            row->destroy();
            delete row;
            row = nullptr;
        }
    }
    // get current distribution
    auto currentDistribution = myTypeDistributionFrameParent->getTypeDistributionSelector()->getCurrentTypeDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution) {
        // get ID
        const auto distributionID = currentDistribution->getAttribute(SUMO_ATTR_ID);
        // first add ID
        myTypeDistributionAttributesEditorRows[0] = new TypeDistributionAttributesEditorRow(this,
                currentDistribution->getTagProperty().getAttributeProperties(SUMO_ATTR_ID), distributionID);
        // get all vTypes with the given current sorted by ID
        std::map<std::string, GNEDemandElement*> vTypes;
        for (const auto &vType : myTypeDistributionFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE)) {
            if (vType->getAttribute(GNE_ATTR_VTYPE_DISTRIBUTION) == distributionID) {
                vTypes[vType->getID()] = vType;
            }
        }
        int index = 1;
        for (const auto &vType : vTypes) {
            myTypeDistributionAttributesEditorRows[index] = new TypeDistributionAttributesEditorRow(this,
                vType.first, vType.second->getAttribute(GNE_ATTR_VTYPE_DISTRIBUTION_PROBABILITY));
            index++;
        }
        // show TypeDistributionAttributesEditor
        show();
    }
    // reparent help button (to place it at bottom)
    myAddButton->reparent(this);
}


void
GNETypeDistributionFrame::TypeDistributionAttributesEditor::hideTypeDistributionAttributesEditorModule() {
    // hide also TypeDistributionAttributesEditor
    hide();
}


void
GNETypeDistributionFrame::TypeDistributionAttributesEditor::refreshAttributeEditor() {
    // get current distribution
    auto currentDistribution = myTypeDistributionFrameParent->getTypeDistributionSelector()->getCurrentTypeDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution) {
        // Iterate over inspected attribute carriers
        for (const auto& attrProperty : currentDistribution->getTagProperty()) {
            // Refresh attributes
            myTypeDistributionAttributesEditorRows[attrProperty.getPositionListed()]->refreshTypeDistributionAttributesEditorRow(currentDistribution->getAttribute(attrProperty.getAttr()));
        }
    }
}


GNETypeDistributionFrame*
GNETypeDistributionFrame::TypeDistributionAttributesEditor::getTypeDistributionFrameParent() const {
    return myTypeDistributionFrameParent;
}


long
GNETypeDistributionFrame::TypeDistributionAttributesEditor::onCmdAddRow(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNETypeDistributionFrame - methods
// ---------------------------------------------------------------------------

GNETypeDistributionFrame::GNETypeDistributionFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, "Type Distributions") {

    // create type editor
    myTypeDistributionEditor = new TypeDistributionEditor(this);

    // create type selector
    myTypeDistributionSelector = new TypeDistributionSelector(this);

    // Create type distribution attributes editor
    myTypeDistributionAttributesEditor = new GNETypeDistributionFrame::TypeDistributionAttributesEditor(this);
}


GNETypeDistributionFrame::~GNETypeDistributionFrame() {}


void
GNETypeDistributionFrame::show() {
    // refresh type selector
    myTypeDistributionSelector->refreshTypeDistributionSelector();
    // show frame
    GNEFrame::show();
}


GNETypeDistributionFrame::TypeDistributionSelector*
GNETypeDistributionFrame::getTypeDistributionSelector() const {
    return myTypeDistributionSelector;
}


void
GNETypeDistributionFrame::attributeUpdated(SumoXMLAttr /*attribute*/) {

}

/****************************************************************************/
