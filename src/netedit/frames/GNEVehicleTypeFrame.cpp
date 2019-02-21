/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVehicleTypeFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
/// @version $Id$
///
// The Widget for edit Vehicle Type elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/demandelements/GNEVehicle.h>
#include <netedit/demandelements/GNERouteHandler.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEVehicleTypeFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEVehicleTypeFrame::vehicleTypeSelector) vehicleTypeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNEVehicleTypeFrame::vehicleTypeSelector::onCmdSelectItem),
};

// Object implementation
FXIMPLEMENT(GNEVehicleTypeFrame::vehicleTypeSelector, FXGroupBox, vehicleTypeSelectorMap,    ARRAYNUMBER(vehicleTypeSelectorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEVehicleTypeFrame::vehicleTypeSelector - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeFrame::vehicleTypeSelector::vehicleTypeSelector(GNEVehicleTypeFrame* vehicleTypeFrameParent) :
    FXGroupBox(vehicleTypeFrameParent->myContentFrame, "Current Vehicle Type", GUIDesignGroupBoxFrame),
    myVehicleTypeFrameParent(vehicleTypeFrameParent) {
    // Create FXComboBox
    myTypeMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // fill myTypeMatchBox with list of VTypes IDs
    for (const auto& i : myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE)) {
        myTypeMatchBox->appendItem(i.first.c_str());
    }
    // Set visible items
    myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
    // vehicleTypeSelector is always shown
    show();
}


GNEVehicleTypeFrame::vehicleTypeSelector::~vehicleTypeSelector() {}


GNEDemandElement*
GNEVehicleTypeFrame::vehicleTypeSelector::getCurrentVType() const {
    // obtain current VType ID (To improve code legibly)
    std::string vTypeID = myTypeMatchBox->getItem(myTypeMatchBox->getCurrentItem()).text();
    // check if ID of myTypeMatchBox is a valid ID
    if (myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE).count(vTypeID) == 1) {
        return myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE).at(vTypeID);
    } else {
        return nullptr;
    }
}


void
GNEVehicleTypeFrame::vehicleTypeSelector::setCurrentVType(GNEDemandElement *vType) {
    bool valid = false;
    // make sure that tag is in myTypeMatchBox
    for (int i = 0; i < (int)myTypeMatchBox->getNumItems(); i++) {
        if (myTypeMatchBox->getItem(i).text() == vType->getID()) {
            myTypeMatchBox->setCurrentItem(i);
            valid = true;
        }
    }
    // Check that give vType type is valid
    if (valid) {
        // show moduls if selected item is valid
        myVehicleTypeFrameParent->enableModuls(vType);
    } else {
        // hide all moduls if selected item isn't valid
        myVehicleTypeFrameParent->disableModuls();
    }
}


long
GNEVehicleTypeFrame::vehicleTypeSelector::onCmdSelectItem(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE)) {
        if (i.first == myTypeMatchBox->getText().text()) {
            // set color of myTypeMatchBox to black (valid)
            myTypeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // show moduls if selected item is valid
            myVehicleTypeFrameParent->enableModuls(i.second);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeMatchBox->getText() + "' in vehicleTypeSelector").text());
            return 1;
        }
    }
    // hide all moduls if selected item isn't valid
    myVehicleTypeFrameParent->disableModuls();
    // set color of myTypeMatchBox to red (invalid)
    myTypeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in vehicleTypeSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeFrame - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeFrame::GNEVehicleTypeFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Vehicle Types") {

    // create vehicle type selector
    myvehicleTypeSelector = new vehicleTypeSelector(this);

    // Create vehicle parameters
    myVehicleTypeAttributes = new ACAttributes(this);

    // set "VTYPE_DEFAULT" as default vehicle Type
    myvehicleTypeSelector->setCurrentVType(myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID));
}


GNEVehicleTypeFrame::~GNEVehicleTypeFrame() {}


void
GNEVehicleTypeFrame::show() {
    // simply show frame
    GNEFrame::show();
}


void
GNEVehicleTypeFrame::enableModuls(GNEDemandElement *vType) {
    // show vehicle type attributes moduls (Except the extended attributes)
    myVehicleTypeAttributes->showACAttributesModul(vType->getTagProperty(), false);
}


void
GNEVehicleTypeFrame::disableModuls() {
    // hide all moduls if vehicle isn't valid
    myVehicleTypeAttributes->hideACAttributesModul();
}


/****************************************************************************/
