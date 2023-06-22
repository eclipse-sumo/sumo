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

FXDEFMAP(GNETypeDistributionFrame::TypeDistributionEditor) typeEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATE,    GNETypeDistributionFrame::TypeDistributionEditor::onCmdCreateType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DELETE,    GNETypeDistributionFrame::TypeDistributionEditor::onCmdDeleteType),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_DELETE,    GNETypeDistributionFrame::TypeDistributionEditor::onUpdDeleteType),
};

FXDEFMAP(GNETypeDistributionFrame::TypeDistributionSelector) typeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNETypeDistributionFrame::TypeDistributionSelector::onCmdSelectTypeDistribution),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SET_TYPE,   GNETypeDistributionFrame::TypeDistributionSelector::onCmdUpdateTypeDistribution)
};

FXDEFMAP(GNETypeDistributionFrame::TypeAttributesEditorRow) TypeAttributesEditorRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNETypeDistributionFrame::TypeAttributesEditorRow::onCmdSetAttribute)
};

FXDEFMAP(GNETypeDistributionFrame::TypeAttributesEditor) TypeAttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNETypeDistributionFrame::TypeAttributesEditor::onCmdTypeAttributesEditorHelp)
};

// Object implementation
FXIMPLEMENT(GNETypeDistributionFrame::TypeDistributionEditor,   MFXGroupBoxModule,  typeEditorMap,              ARRAYNUMBER(typeEditorMap))
FXIMPLEMENT(GNETypeDistributionFrame::TypeDistributionSelector, MFXGroupBoxModule,  typeSelectorMap,            ARRAYNUMBER(typeSelectorMap))
FXIMPLEMENT(GNETypeDistributionFrame::TypeAttributesEditorRow,  FXHorizontalFrame,  TypeAttributesEditorRowMap, ARRAYNUMBER(TypeAttributesEditorRowMap))
FXIMPLEMENT(GNETypeDistributionFrame::TypeAttributesEditor,     MFXGroupBoxModule,  TypeAttributesEditorMap,    ARRAYNUMBER(TypeAttributesEditorMap))


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
        myTypeComboBox->appendItem(vType->getID().c_str(), vType->getACIcon());
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
        myTypeComboBox->appendItem(vTypeDistribution.first.c_str(), vTypeDistribution.second->getACIcon());
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
        myTypeDistributionFrameParent->myTypeTypeAttributesEditor->showAttributeEditorModule();
    } else {
        myCurrentTypeDistribution.clear();
        // set myCurrentType as inspected element
        myTypeDistributionFrameParent->getViewNet()->setInspectedAttributeCarriers({});
        // hide modules
        myTypeDistributionFrameParent->myTypeTypeAttributesEditor->hideTypeAttributesEditorModule();
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
            myTypeDistributionFrameParent->myTypeTypeAttributesEditor->showAttributeEditorModule();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeComboBox->getText() + "' in TypeDistributionSelector").text());
            // update viewNet
            viewNet->updateViewNet();
            return 1;
        }
    }
    myCurrentTypeDistribution.clear();
    // hide all modules if selected item isn't valid
    myTypeDistributionFrameParent->myTypeTypeAttributesEditor->hideTypeAttributesEditorModule();
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
// GNETypeDistributionFrame::TypeAttributesEditorRow - methods
// ---------------------------------------------------------------------------

GNETypeDistributionFrame::TypeAttributesEditorRow::TypeAttributesEditorRow(GNETypeDistributionFrame::TypeAttributesEditor* attributeEditorParent,
        const GNEAttributeProperties& ACAttr, const std::string& value) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myTypeAttributesEditorParent(attributeEditorParent),
    myACAttr(ACAttr) {
    // Create and hide label
    myAttributeLabel = new MFXLabelTooltip(this,
        attributeEditorParent->getTypeDistributionFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
        "attributeLabel", nullptr, GUIDesignLabelThickedFixed(100));
    // Create and hide MFXTextFieldTooltip for string attributes
    myValueTextField = new MFXTextFieldTooltip(this,
        attributeEditorParent->getTypeDistributionFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // only create if parent was created
    if (getParent()->id()) {
        // create TypeAttributesEditorRow
        FXHorizontalFrame::create();
        // Show attribute Label
        myAttributeLabel->setText(myACAttr.getAttrStr().c_str());
        myAttributeLabel->setTipText(myACAttr.getDefinition().c_str());
        // In any other case (String, list, etc.), show value as String
        myValueTextField->setText(value.c_str());
        myValueTextField->setTextColor(FXRGB(0, 0, 0));
        myValueTextField->killFocus();
        // Show TypeAttributesEditorRow
        show();
    }
}


void
GNETypeDistributionFrame::TypeAttributesEditorRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


void
GNETypeDistributionFrame::TypeAttributesEditorRow::refreshTypeAttributesEditorRow(const std::string& value) {
    // set last valid value and restore color if onlyValid is disabled
    myValueTextField->setText(value.c_str());
    // set blue color if is an computed value
    myValueTextField->setTextColor(FXRGB(0, 0, 0));
    myValueTextField->killFocus();
}


bool
GNETypeDistributionFrame::TypeAttributesEditorRow::isTypeAttributesEditorRowValid() const {
    return (myValueTextField->getTextColor() == FXRGB(0, 0, 0));
}


long
GNETypeDistributionFrame::TypeAttributesEditorRow::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    // Check if default value must be set
    if (myValueTextField->getText().empty() && myACAttr.hasDefaultValue()) {
        newVal = myACAttr.getDefaultValue();
        myValueTextField->setText(newVal.c_str());
    } else {
        // obtain value of myValueTextField
        newVal = myValueTextField->getText().text();
    }
    // get current distribution
    auto currentDistribution = myTypeAttributesEditorParent->getTypeDistributionFrameParent()->getTypeDistributionSelector()->getCurrentTypeDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution) {
        // Check if attribute must be changed
        if (currentDistribution->isValid(myACAttr.getAttr(), newVal)) {
            // set attribute
            currentDistribution->setAttribute(myACAttr.getAttr(), myACAttr.getDefaultValue(), myTypeAttributesEditorParent->getTypeDistributionFrameParent()->getViewNet()->getUndoList());
            // update text field
            myValueTextField->setTextColor(FXRGB(0, 0, 0));
            myValueTextField->setBackColor(FXRGB(255, 255, 255));
            myValueTextField->killFocus();
            // in this case, we need to refresh the other values (For example, allow/Disallow objects)
            myTypeAttributesEditorParent->refreshAttributeEditor();
            // update frame parent after attribute successfully set
            myTypeAttributesEditorParent->getTypeDistributionFrameParent()->attributeUpdated(myACAttr.getAttr());
        } else {
            myValueTextField->setTextColor(FXRGB(255, 0, 0));
            if (newVal.empty()) {
                myValueTextField->setBackColor(FXRGBA(255, 213, 213, 255));
            }
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(TL("Value '") + newVal + TL("' for attribute ") + myACAttr.getAttrStr() + TL(" of ") + myACAttr.getTagPropertyParent().getTagStr() + TL(" isn't valid"));
        }
    }
    return 1;
}


GNETypeDistributionFrame::TypeAttributesEditorRow::TypeAttributesEditorRow() :
    myTypeAttributesEditorParent(nullptr) {
}

// ---------------------------------------------------------------------------
// GNETypeDistributionFrame::TypeAttributesEditor - methods
// ---------------------------------------------------------------------------

GNETypeDistributionFrame::TypeAttributesEditor::TypeAttributesEditor(GNETypeDistributionFrame* typeDistributionFrameParent) :
    MFXGroupBoxModule(typeDistributionFrameParent, TL("Internal attributes")),
    myTypeDistributionFrameParent(typeDistributionFrameParent) {
    // resize myTypeAttributesEditorRows
    myTypeAttributesEditorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // Create help button
    myHelpButton = new FXButton(getCollapsableFrame(), TL("Help"), nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


void
GNETypeDistributionFrame::TypeAttributesEditor::showAttributeEditorModule() {
    // first remove all rows
    for (auto& row : myTypeAttributesEditorRows) {
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
        // Iterate over attributes
        for (const auto& attrProperty : currentDistribution->getTagProperty()) {
            // create attribute editor row
            myTypeAttributesEditorRows[attrProperty.getPositionListed()] = new TypeAttributesEditorRow(this,
                attrProperty, currentDistribution->getAttribute(attrProperty.getAttr()));
        }
        // show TypeAttributesEditor
        show();
    }
    // reparent help button (to place it at bottom)
    myHelpButton->reparent(this);
}


void
GNETypeDistributionFrame::TypeAttributesEditor::hideTypeAttributesEditorModule() {
    // hide also TypeAttributesEditor
    hide();
}


void
GNETypeDistributionFrame::TypeAttributesEditor::refreshAttributeEditor() {
    // get current distribution
    auto currentDistribution = myTypeDistributionFrameParent->getTypeDistributionSelector()->getCurrentTypeDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution) {
        // Iterate over inspected attribute carriers
        for (const auto& attrProperty : currentDistribution->getTagProperty()) {
            // Refresh attributes
            myTypeAttributesEditorRows[attrProperty.getPositionListed()]->refreshTypeAttributesEditorRow(currentDistribution->getAttribute(attrProperty.getAttr()));
        }
    }
}


GNETypeDistributionFrame*
GNETypeDistributionFrame::TypeAttributesEditor::getTypeDistributionFrameParent() const {
    return myTypeDistributionFrameParent;
}


long
GNETypeDistributionFrame::TypeAttributesEditor::onCmdTypeAttributesEditorHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog
    myTypeDistributionFrameParent->openHelpAttributesDialog(myTypeDistributionFrameParent->getTypeDistributionSelector()->getCurrentTypeDistribution());
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

    // Create vehicle type attributes editor
    myTypeTypeAttributesEditor = new GNETypeDistributionFrame::TypeAttributesEditor(this);
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
