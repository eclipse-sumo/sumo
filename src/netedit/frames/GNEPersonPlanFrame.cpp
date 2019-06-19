/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPersonPlanFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
/// @version $Id$
///
// The Widget for add PersonPlan elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/demandelements/GNEPerson.h>
#include <netedit/demandelements/GNERouteHandler.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNEPersonPlanFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPersonPlanFrame::HelpCreation - methods
// ---------------------------------------------------------------------------

GNEPersonPlanFrame::HelpCreation::HelpCreation(GNEPersonPlanFrame* vehicleFrameParent) :
    FXGroupBox(vehicleFrameParent->myContentFrame, "Help", GUIDesignGroupBoxFrame),
    myPersonPlanFrameParent(vehicleFrameParent) {
    myInformationLabel = new FXLabel(this, "", 0, GUIDesignLabelFrameInformation);
}


GNEPersonPlanFrame::HelpCreation::~HelpCreation() {}


void
GNEPersonPlanFrame::HelpCreation::showHelpCreation() {
    // first update help cration
    updateHelpCreation();
    // show modul
    show();
}


void
GNEPersonPlanFrame::HelpCreation::hideHelpCreation() {
    hide();
}

void
GNEPersonPlanFrame::HelpCreation::updateHelpCreation() {
    // create information label
    std::ostringstream information;
    // set text depending of selected person plan
    switch (myPersonPlanFrameParent->myPersonPlanTagSelector->getCurrentTagProperties().getTag()) {
        case SUMO_TAG_PERSONTRIP_FROMTO:
            information
                << "- Click over edges to\n"
                << "  create a trip.";
            break;
        case SUMO_TAG_PERSONTRIP_BUSSTOP:
            information
                << "- Click over an edge and\n"
                << "  a bus to create a trip.";
            break;
        case SUMO_TAG_WALK_EDGES:
            information
                << "- Click over a sequenz of\n"
                << "  consecutive edges to\n"
                << "  create a walk.";
            break;
        case SUMO_TAG_WALK_FROMTO:
            information
                << "- Click over edges to\n"
                << "  create a walk.";
            break;
        case SUMO_TAG_WALK_BUSSTOP:
            information
                << "- Click over an edge and\n"
                << "  a bus to create a walk.";
            break;
        case SUMO_TAG_WALK_ROUTE:
            information
                << "- Click over a route";
            break;
        case SUMO_TAG_RIDE_FROMTO:
            information
                << "- Click over edges to\n"
                << "  create a ride.";
            break;
        case SUMO_TAG_RIDE_BUSSTOP:
            information
                << "- Click over an edge and\n"
                << "  a bus to create a ride";
            break;
        default:
            break;
    }
    // set information label
    myInformationLabel->setText(information.str().c_str());
}

// ---------------------------------------------------------------------------
// GNEPersonPlanFrame - methods
// ---------------------------------------------------------------------------

GNEPersonPlanFrame::GNEPersonPlanFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "PersonPlans") {

    // create person types selector modul
    myPersonSelector = new DemandElementSelector(this, {GNEAttributeCarrier::TagType::TAGTYPE_PERSON});

    // Create tag selector for person plan
    myPersonPlanTagSelector = new TagSelector(this, GNEAttributeCarrier::TagType::TAGTYPE_PERSONPLAN);

    // Create person parameters
    myPersonPlanAttributes = new AttributesCreator(this);

    // create EdgePathCreator Modul
    myEdgePathCreator = new EdgePathCreator(this, EdgePathCreator::EdgePathCreatorModes::GNE_EDGEPATHCREATOR_FROM_TO_VIA);

    // Create Help Creation Modul
    myHelpCreation = new HelpCreation(this);

    // Create AttributeCarrierHierarchy modul
    myPersonHierarchy = new GNEFrame::AttributeCarrierHierarchy(this);

    // set PersonPlan tag type in tag selector
    myPersonPlanTagSelector->setCurrentTagType(GNEAttributeCarrier::TagType::TAGTYPE_PERSONPLAN);
}


GNEPersonPlanFrame::~GNEPersonPlanFrame() {}


void
GNEPersonPlanFrame::show() {
    // Only show moduls if there is at least one person
    if ((myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSON).size() > 0) ||
        (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONFLOW).size() > 0)) {
        // refresh demand element selector
        myPersonSelector->refreshDemandElementSelector();
        // refresh item selector
        myPersonPlanTagSelector->refreshTagProperties();
        // set first person as demand element
        if (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSON).size() > 0) {
            myPersonSelector->setDemandElement(myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSON).begin()->second);
        } else {
            myPersonSelector->setDemandElement(myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONFLOW).begin()->second);
        }
    } else {
        // hide all moduls
    }
    // show frame
    GNEFrame::show();
}


bool
GNEPersonPlanFrame::addPersonPlan(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // first check if person selected is valid
    if (myPersonSelector->getCurrentDemandElement() == nullptr) {
        myViewNet->setStatusBarText("Current selected person isn't valid.");
        return false;
    }
    // finally check that person plan selected is valid
    if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected person plan isn't valid.");
        return false;
    }
    // add clicked edge in EdgePathCreator
    if (objectsUnderCursor.getEdgeFront()) {
        return myEdgePathCreator->addEdge(objectsUnderCursor.getEdgeFront());
    } else {
        return false;
    }
}


GNEPersonPlanFrame::EdgePathCreator*
GNEPersonPlanFrame::getEdgePathCreator() const {
    return myEdgePathCreator;
}

// ===========================================================================
// protected
// ===========================================================================

void 
GNEPersonPlanFrame::tagSelected() {
    // first check if person is valid
    if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
        // set edge path creator name
        if (myPersonPlanTagSelector->getCurrentTagProperties().isPersonTrip()) {
            myEdgePathCreator->edgePathCreatorName("person trip");
        } else if (myPersonPlanTagSelector->getCurrentTagProperties().isWalk()) {
            myEdgePathCreator->edgePathCreatorName("walk");
        } else if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
            myEdgePathCreator->edgePathCreatorName("ride");
        }
        // show person attributes
        myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties());
        // show edge path creator
        myEdgePathCreator->showEdgePathCreator();
        // show help creation
        myHelpCreation->showHelpCreation();
        // show person hierarchy
        myPersonHierarchy->showAttributeCarrierHierarchy(myPersonSelector->getCurrentDemandElement());
    } else {
        // hide moduls if tag selecte isn't valid
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myEdgePathCreator->hideEdgePathCreator();
        myHelpCreation->hideHelpCreation();
        myPersonHierarchy->hideAttributeCarrierHierarchy();
    }
}


void 
GNEPersonPlanFrame::demandElementSelected() {
    // check if a valid person was selected
    if (myPersonSelector->getCurrentDemandElement()) {
        // show person plan tag selector
        myPersonPlanTagSelector->showTagSelector();
        // now check if person plan selected is valid
        if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
            // set edge path creator name
            if (myPersonPlanTagSelector->getCurrentTagProperties().isPersonTrip()) {
                myEdgePathCreator->edgePathCreatorName("person trip");
            } else if (myPersonPlanTagSelector->getCurrentTagProperties().isWalk()) {
                myEdgePathCreator->edgePathCreatorName("walk");
            } else if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                myEdgePathCreator->edgePathCreatorName("ride");
            }
            // show person plan attributes
            myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties());
            // show edge path creator
            myEdgePathCreator->showEdgePathCreator();
            // show help creation
            myHelpCreation->showHelpCreation();
            // Show the person's children
            myPersonHierarchy->showAttributeCarrierHierarchy(myPersonSelector->getCurrentDemandElement());
        } else {
            myPersonPlanAttributes->hideAttributesCreatorModul();
            myEdgePathCreator->hideEdgePathCreator();
            myHelpCreation->hideHelpCreation();
            myPersonHierarchy->hideAttributeCarrierHierarchy();
        }
    } else {
        // hide moduls if person selected isn't valid
        myPersonPlanTagSelector->hideTagSelector();
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myEdgePathCreator->hideEdgePathCreator();
        myHelpCreation->hideHelpCreation();
        myPersonHierarchy->hideAttributeCarrierHierarchy();
    }
}


void
GNEPersonPlanFrame::edgePathCreated() {
    // first check that all attributes are valid
    if (!myPersonPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid person parameters.");
    } else if (!myPersonPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myPersonPlanTagSelector->getCurrentTagProperties().getTagStr() + " parameters.");
    } else {
        // begin undo-redo operation
        myViewNet->getUndoList()->p_begin("create and " + myPersonPlanTagSelector->getCurrentTagProperties().getTagStr());
        // Declare map to keep attributes from myPersonPlanAttributes
        std::map<SumoXMLAttr, std::string> valuesMap = myPersonPlanAttributes->getAttributesAndValues(true);
        // check what PersonPlan we're creating
        switch (myPersonPlanTagSelector->getCurrentTagProperties().getTag()) {
            case SUMO_TAG_PERSONTRIP_FROMTO: {
                std::vector<std::string> types = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_VTYPES]);
                std::vector<std::string> modes = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_MODES]);
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildPersonTripFromTo(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myEdgePathCreator->getSelectedEdges(), types, modes, arrivalPos);
                break;
            }
            case SUMO_TAG_PERSONTRIP_BUSSTOP: {
                GNEAdditional *busStop = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, valuesMap[SUMO_ATTR_BUS_STOP]);
                std::vector<std::string> types = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_VTYPES]);
                std::vector<std::string> modes = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_MODES]);
                GNERouteHandler::buildPersonTripBusStop(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myEdgePathCreator->getSelectedEdges(), busStop, types, modes);
                break;
            }
            case SUMO_TAG_WALK_EDGES: {
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildWalkEdges(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myEdgePathCreator->getSelectedEdges(), arrivalPos);
                break;
            }
            case SUMO_TAG_WALK_FROMTO: {
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildWalkFromTo(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myEdgePathCreator->getSelectedEdges(), arrivalPos);
                break;
            }
            case SUMO_TAG_WALK_BUSSTOP: {
                GNEAdditional *busStop = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, valuesMap[SUMO_ATTR_BUS_STOP]);
                GNERouteHandler::buildWalkBusStop(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myEdgePathCreator->getSelectedEdges(), busStop);
                break;
            }
            case SUMO_TAG_WALK_ROUTE: {
                GNEDemandElement *route = myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, valuesMap[SUMO_ATTR_ROUTE]);
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildWalkRoute(myViewNet, true, myPersonSelector->getCurrentDemandElement(), route, arrivalPos);
                break;
            }
            case SUMO_TAG_RIDE_FROMTO: {
                std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_LINES]);
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildRideFromTo(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myEdgePathCreator->getSelectedEdges(), lines, arrivalPos);
                break;
            }
            case SUMO_TAG_RIDE_BUSSTOP: {
                GNEAdditional *busStop = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, valuesMap[SUMO_ATTR_BUS_STOP]);
                std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_LINES]);
                GNERouteHandler::buildRideBusStop(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myEdgePathCreator->getSelectedEdges(), busStop, lines);
                break;
            }
            default:
                throw InvalidArgument("Invalid person plan tag");
        }
        // end undo-redo operation
        myViewNet->getUndoList()->p_end();
        // refresh AttributeCarrierHierarchy
        myPersonHierarchy->refreshAttributeCarrierHierarchy();
    }
}


/****************************************************************************/
