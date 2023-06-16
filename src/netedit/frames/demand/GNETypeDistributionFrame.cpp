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

// Object implementation
FXIMPLEMENT(GNETypeDistributionFrame::TypeDistributionEditor,   MFXGroupBoxModule,  typeEditorMap,      ARRAYNUMBER(typeEditorMap))
FXIMPLEMENT(GNETypeDistributionFrame::TypeDistributionSelector, MFXGroupBoxModule,  typeSelectorMap,    ARRAYNUMBER(typeSelectorMap))


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
    const auto &ACs = myTypeDistributionFrameParent->getViewNet()->getNet()->getAttributeCarriers();
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
    GNEDemandElement *vTypeDistribution = nullptr;
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
        myTypeDistributionFrameParent->myTypeAttributesEditor->showAttributeEditorModule(true);
    } else {
        myCurrentTypeDistribution.clear();
        // set myCurrentType as inspected element
        myTypeDistributionFrameParent->getViewNet()->setInspectedAttributeCarriers({});
        // hide modules
        myTypeDistributionFrameParent->myTypeAttributesEditor->hideAttributesEditorModule();
    }
}


long
GNETypeDistributionFrame::TypeDistributionSelector::onCmdSelectTypeDistribution(FXObject*, FXSelector, void*) {
    const auto viewNet = myTypeDistributionFrameParent->getViewNet();
    const auto &vTypeDistributions = viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE_DISTRIBUTION);
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
            myTypeDistributionFrameParent->myTypeAttributesEditor->showAttributeEditorModule(true);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeComboBox->getText() + "' in TypeDistributionSelector").text());
            // update viewNet
            viewNet->updateViewNet();
            return 1;
        }
    }
    myCurrentTypeDistribution.clear();
    // hide all modules if selected item isn't valid
    myTypeDistributionFrameParent->myTypeAttributesEditor->hideAttributesEditorModule();
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
    const auto &demandElements = myTypeDistributionFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements();
    if (demandElements.at(SUMO_TAG_VTYPE_DISTRIBUTION).size() > 0) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
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
    myTypeAttributesEditor = new GNEFrameAttributeModules::AttributesEditor(this);
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
