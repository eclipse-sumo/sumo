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

FXDEFMAP(GNETypeDistributionFrame::TypeDistributionSelector) typeDistributionSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNETypeDistributionFrame::TypeDistributionSelector::onCmdSelectTypeDistribution),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SET_TYPE,   GNETypeDistributionFrame::TypeDistributionSelector::onCmdUpdateTypeDistribution)
};

// Object implementation
FXIMPLEMENT(GNETypeDistributionFrame::TypeDistributionSelector, MFXGroupBoxModule,  typeDistributionSelectorMap,    ARRAYNUMBER(typeDistributionSelectorMap))


// ===========================================================================
// method definitions
// ===========================================================================


// ---------------------------------------------------------------------------
// GNETypeFrame::TypeDistributionSelector - methods
// ---------------------------------------------------------------------------

GNETypeDistributionFrame::TypeDistributionSelector::TypeDistributionSelector(GNETypeDistributionFrame* typeFrameParent) :
    MFXGroupBoxModule(typeFrameParent, TL("Current type dist.")),
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
        myTypeDistributionFrameParent->myDistributionEditor->setDistribution(vTypeDistribution);
        // set myCurrentType as inspected element
        myTypeDistributionFrameParent->getViewNet()->setInspectedAttributeCarriers({vTypeDistribution});
        // show modules
        myTypeDistributionFrameParent->myDistributionEditor->showAttributeEditorModule();
    } else {
        myCurrentTypeDistribution.clear();
        myTypeDistributionFrameParent->myDistributionEditor->setDistribution(nullptr);
        // set myCurrentType as inspected element
        myTypeDistributionFrameParent->getViewNet()->setInspectedAttributeCarriers({});
        // hide modules
        myTypeDistributionFrameParent->myDistributionEditor->hideAttributesEditorModule();
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
            myTypeDistributionFrameParent->myDistributionEditor->setDistribution(vTypeDistribution);
            // set color of myTypeMatchBox to black (valid)
            myTypeComboBox->setTextColor(FXRGB(0, 0, 0));
            // set myCurrentType as inspected element
            viewNet->setInspectedAttributeCarriers({vTypeDistribution});
            // show modules if selected item is valid
            myTypeDistributionFrameParent->myDistributionEditor->showAttributeEditorModule();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeComboBox->getText() + "' in TypeDistributionSelector").text());
            // update viewNet
            viewNet->updateViewNet();
            return 1;
        }
    }
    myCurrentTypeDistribution.clear();
    myTypeDistributionFrameParent->myDistributionEditor->setDistribution(nullptr);
    // hide all modules if selected item isn't valid
    myTypeDistributionFrameParent->myDistributionEditor->hideAttributesEditorModule();
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
// GNETypeDistributionFrame - methods
// ---------------------------------------------------------------------------

GNETypeDistributionFrame::GNETypeDistributionFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, "Type Distributions") {

    /// @brief type editor
    myDistributionEditor = new GNEDistributionFrame::DistributionEditor(this);

    /// @brief type distribution selector
    myTypeDistributionSelector = new TypeDistributionSelector(this);

    /// @brief distribution attributes editor
    myAttributesEditor = new GNEFrameAttributeModules::AttributesEditor(this);

    // Create type distribution attributes editor
    myDistributionAttributesEditor = new GNEDistributionFrame::AttributesEditor(this);
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
