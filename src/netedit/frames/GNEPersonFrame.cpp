/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPersonFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
/// @version $Id$
///
// The Widget for add Person elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/div/GUIDesigns.h>
#include <netedit/demandelements/GNEPerson.h>
#include <netedit/demandelements/GNERouteHandler.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>

#include "GNEPersonFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPersonFrame::HelpCreation - methods
// ---------------------------------------------------------------------------

GNEPersonFrame::HelpCreation::HelpCreation(GNEPersonFrame* vehicleFrameParent) :
    FXGroupBox(vehicleFrameParent->myContentFrame, "Help", GUIDesignGroupBoxFrame),
    myPersonFrameParent(vehicleFrameParent) {
    myInformationLabel = new FXLabel(this, "", 0, GUIDesignLabelFrameInformation);
}


GNEPersonFrame::HelpCreation::~HelpCreation() {}


void
GNEPersonFrame::HelpCreation::showHelpCreation() {
    // first update help cration
    updateHelpCreation();
    // show modul
    show();
}


void
GNEPersonFrame::HelpCreation::hideHelpCreation() {
    hide();
}

void
GNEPersonFrame::HelpCreation::updateHelpCreation() {
    // create information label
    std::ostringstream information;
    // set text depending of selected vehicle type
    switch (myPersonFrameParent->myPersonTagSelector->getCurrentTagProperties().getTag()) {
        case SUMO_TAG_PERSON:
            information
                    << "- Click over a route to\n"
                    << "  create a vehicle.";
            break;
        case SUMO_TAG_PERSONFLOW:
            information
                    << "- Select two edges to\n"
                    << "  create a Trip.";
            break;
        default:
            break;
    }
    // set information label
    myInformationLabel->setText(information.str().c_str());
}

// ---------------------------------------------------------------------------
// GNEPersonFrame - methods
// ---------------------------------------------------------------------------

GNEPersonFrame::GNEPersonFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Persons") {

    // create tag Selector modul for persons
    myPersonTagSelector = new TagSelector(this, GNEAttributeCarrier::TagType::TAGTYPE_PERSON);

    // create person types selector modul
    myPTypeSelector = new DemandElementSelector(this, SUMO_TAG_PTYPE);

    // create tag Selector modul for person plans
    myPersonPlanSelector = new TagSelector(this, GNEAttributeCarrier::TagType::TAGTYPE_PERSONPLAN);

    // create vehicle parameters
    myPersonAttributes = new AttributesCreator(this);

    // create EdgePathCreator Modul
    myEdgePathCreator = new EdgePathCreator(this);

    // create Help Creation Modul
    myHelpCreation = new HelpCreation(this);

    // limit path creator to pedestrians
    myEdgePathCreator->setVClass(SVC_PEDESTRIAN);

    // set Person as default vehicle
    myPersonTagSelector->setCurrentTypeTag(SUMO_TAG_PERSON);
}


GNEPersonFrame::~GNEPersonFrame() {}


void
GNEPersonFrame::show() {
    // refresh item selector
    myPersonTagSelector->refreshTagProperties();
    myPTypeSelector->refreshDemandElementSelector();
    myPersonPlanSelector->refreshTagProperties();
    // show frame
    GNEFrame::show();
}


bool
GNEPersonFrame::addPerson(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // obtain tag (only for improve code legibility)
    SumoXMLTag personTag = myPersonTagSelector->getCurrentTagProperties().getTag();
    // first check that current selected person is valid
    if (personTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected person isn't valid.");
        return false;
    }
    // now check that pType is valid
    if (myPTypeSelector->getCurrentDemandElement() == nullptr) {
        myViewNet->setStatusBarText("Current selected person type isn't valid.");
        return false;
    }
    // finally check that person plan selected is valid
    if (myPersonPlanSelector->getCurrentTagProperties().getTag() == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected person plan isn't valid.");
        return false;
    }
    // add clicked edge in EdgePathCreator
    if (objectsUnderCursor.getEdgeFront()) {
        myEdgePathCreator->addEdge(objectsUnderCursor.getEdgeFront());
    }
    return true;
}


GNEPersonFrame::EdgePathCreator*
GNEPersonFrame::getEdgePathCreator() const {
    return myEdgePathCreator;
}

// ===========================================================================
// protected
// ===========================================================================

void 
GNEPersonFrame::tagSelected() {
    // first check if person is valid
    if (myPersonTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
        // show PType selector and person plan selector
        myPTypeSelector->showDemandElementSelector();
        if (myPTypeSelector->getCurrentDemandElement()) {
            myPersonPlanSelector->showTagSelector();
            // now check if person plan selected is valid
            if (myPersonPlanSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
                myPersonAttributes->showAttributesCreatorModul(myPersonTagSelector->getCurrentTagProperties());
                // set edge path creator name
                if (myPersonPlanSelector->getCurrentTagProperties().isPersonTrip()) {
                    myEdgePathCreator->edgePathCreatorName("person trip");
                } else if (myPersonPlanSelector->getCurrentTagProperties().isWalk()) {
                    myEdgePathCreator->edgePathCreatorName("walk");
                } else if (myPersonPlanSelector->getCurrentTagProperties().isRide()) {
                    myEdgePathCreator->edgePathCreatorName("ride");
                }
                myEdgePathCreator->showEdgePathCreator();
                myHelpCreation->showHelpCreation();
            } else {
                myPersonAttributes->hideAttributesCreatorModul();
                myEdgePathCreator->hideEdgePathCreator();
                myHelpCreation->hideHelpCreation();
            }
        } else {
            myPersonPlanSelector->hideTagSelector();
            myPersonAttributes->hideAttributesCreatorModul();
            myEdgePathCreator->hideEdgePathCreator();
            myHelpCreation->hideHelpCreation();
        }
    } else {
        // hide all moduls if person isn't valid
        myPTypeSelector->hideDemandElementSelector();
        myPersonPlanSelector->hideTagSelector();
        myPersonAttributes->hideAttributesCreatorModul();
        myEdgePathCreator->hideEdgePathCreator();
        myHelpCreation->hideHelpCreation();
    }
}


void 
GNEPersonFrame::demandElementSelected() {
    if (myPTypeSelector->getCurrentDemandElement()) {
        myPersonPlanSelector->showTagSelector();
        // now check if person plan selected is valid
        if (myPersonPlanSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
            myPersonAttributes->showAttributesCreatorModul(myPersonTagSelector->getCurrentTagProperties());
            // set edge path creator name
            if (myPersonPlanSelector->getCurrentTagProperties().isPersonTrip()) {
                myEdgePathCreator->edgePathCreatorName("person trip");
            } else if (myPersonPlanSelector->getCurrentTagProperties().isWalk()) {
                myEdgePathCreator->edgePathCreatorName("walk");
            } else if (myPersonPlanSelector->getCurrentTagProperties().isRide()) {
                myEdgePathCreator->edgePathCreatorName("ride");
            }
            myEdgePathCreator->showEdgePathCreator();
            myHelpCreation->showHelpCreation();
        } else {
            myPersonAttributes->hideAttributesCreatorModul();
            myEdgePathCreator->hideEdgePathCreator();
            myHelpCreation->hideHelpCreation();
        }
    } else {
        myPersonPlanSelector->hideTagSelector();
        myPersonAttributes->hideAttributesCreatorModul();
        myEdgePathCreator->hideEdgePathCreator();
        myHelpCreation->hideHelpCreation();
    }
}


void
GNEPersonFrame::edgePathCreated() {


}

/****************************************************************************/
