/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEPersonPlanFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
///
// The Widget for add PersonPlan elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/additional/GNEBusStop.h>
#include <netedit/elements/demand/GNEPerson.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEPersonPlanFrame.h"


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPersonPlanFrame - methods
// ---------------------------------------------------------------------------

GNEPersonPlanFrame::GNEPersonPlanFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "PersonPlans") {

    // create person types selector modul
    myPersonSelector = new GNEFrameModuls::DemandElementSelector(this, {GNETagProperties::TagType::PERSON});

    // Create tag selector for person plan
    myPersonPlanTagSelector = new GNEFrameModuls::TagSelector(this, GNETagProperties::TagType::PERSONPLAN);

    // Create person parameters
    myPersonPlanAttributes = new GNEFrameAttributesModuls::AttributesCreator(this);

    // create myPathCreator Modul
    myPathCreator = new GNEFrameModuls::PathCreator(this, GNEFrameModuls::PathCreator::Mode::NONCONSECUTIVE_EDGES);

    // Create AttributeCarrierHierarchy modul
    myPersonHierarchy = new GNEFrameModuls::AttributeCarrierHierarchy(this);

    // set PersonPlan tag type in tag selector
    myPersonPlanTagSelector->setCurrentTagType(GNETagProperties::TagType::PERSONPLAN);
}


GNEPersonPlanFrame::~GNEPersonPlanFrame() {}


void
GNEPersonPlanFrame::show() {
    // get persons maps
    const std::map<std::string, GNEDemandElement*>& persons = myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSON);
    const std::map<std::string, GNEDemandElement*>& personFlows = myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSONFLOW);
    // Only show moduls if there is at least one person
    if ((persons.size() > 0) || (personFlows.size() > 0)) {
        // refresh demand element selector
        myPersonSelector->refreshDemandElementSelector();
        // refresh item selector
        myPersonPlanTagSelector->refreshTagProperties();
        // set first person as demand element (this will call demandElementSelected() function)
        if (persons.size() > 0) {
            myPersonSelector->setDemandElement(persons.begin()->second);
        } else {
            myPersonSelector->setDemandElement(personFlows.begin()->second);
        }
    } else {
        // hide all moduls except helpCreation
        myPersonSelector->hideDemandElementSelector();
        myPersonPlanTagSelector->hideTagSelector();
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myPathCreator->hidePathCreatorModul();
        myPersonHierarchy->hideAttributeCarrierHierarchy();
    }
    // show frame
    GNEFrame::show();
}


void
GNEPersonPlanFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


bool
GNEPersonPlanFrame::handlePersonPlanClick(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor,
        const bool shiftKeyPressed, const bool controlKeyPressed) {
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
    // Obtain current person plan tag (only for improve code legibility)
    SumoXMLTag personPlanTag = myPersonPlanTagSelector->getCurrentTagProperties().getTag();
    // declare flags to check required elements
    /*SUMO_TAG_PERSONSTOP_LANE;*/

    bool requireRoute = (personPlanTag == SUMO_TAG_WALK_ROUTE);
    bool requireBusStop = ((personPlanTag == SUMO_TAG_PERSONTRIP_BUSSTOP) || (personPlanTag == SUMO_TAG_WALK_BUSSTOP) ||
                           (personPlanTag == SUMO_TAG_RIDE_BUSSTOP) || (personPlanTag == SUMO_TAG_PERSONSTOP_BUSSTOP));
    bool requireEdge = ((personPlanTag == SUMO_TAG_PERSONTRIP_FROMTO) || (personPlanTag == SUMO_TAG_WALK_EDGES) ||
                        (personPlanTag == SUMO_TAG_WALK_FROMTO) || (personPlanTag == SUMO_TAG_RIDE_FROMTO));
    // process personPlanCreated(...) depending of required element
    if (requireBusStop && objectsUnderCursor.getAdditionalFront() && (objectsUnderCursor.getAdditionalFront()->getTagProperty().getTag() == SUMO_TAG_BUS_STOP)) {
        return myPathCreator->addAdditional(objectsUnderCursor.getAdditionalFront(), true, true);
/*
    } else if (requireRoute && objectsUnderCursor.getDemandElementFront() && (objectsUnderCursor.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE)) {
        return personPlanCreated(nullptr, objectsUnderCursor.getDemandElementFront());
*/
    } else if (requireEdge && objectsUnderCursor.getEdgeFront()) {
        return myPathCreator->addEdge(objectsUnderCursor.getEdgeFront(), true, true);
    } else {
        return false;
    }
}


GNEFrameModuls::PathCreator*
GNEPersonPlanFrame::getPathCreator() const {
    return myPathCreator;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEPersonPlanFrame::tagSelected() {
    // first check if person is valid
    if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
/*
        // set edge path creator name
        if (myPersonPlanTagSelector->getCurrentTagProperties().isPersonTrip()) {
            myPersonPlanCreator->edgePathCreatorName("person trip");
        } else if (myPersonPlanTagSelector->getCurrentTagProperties().isWalk()) {
            myPersonPlanCreator->edgePathCreatorName("walk");
        } else if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
            myPersonPlanCreator->edgePathCreatorName("ride");
        }
*/
        // show person attributes
        myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties(), {});
        // show edge path creator
        myPathCreator->showPathCreatorModul();
        // show person hierarchy
        myPersonHierarchy->showAttributeCarrierHierarchy(myPersonSelector->getCurrentDemandElement());
    } else {
        // hide moduls if tag selecte isn't valid
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myPathCreator->hidePathCreatorModul();
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
            // show person plan attributes
            myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties(), {});
            // show edge path creator
            myPathCreator->showPathCreatorModul();
            // Show the person's children
            myPersonHierarchy->showAttributeCarrierHierarchy(myPersonSelector->getCurrentDemandElement());
            // get last person plan
            const GNEDemandElement* personPlan = myPersonSelector->getCurrentDemandElement()->getChildDemandElements().back();
            if (personPlan->getTagProperty().getTag() == SUMO_TAG_WALK_ROUTE) {
                myPathCreator->addEdge(personPlan->getParentDemandElements().back()->getParentEdges().back(), false, false);
            } else if (personPlan->getParentAdditionals().size() > 0) {
                myPathCreator->addAdditional(personPlan->getParentAdditionals().front(), false, false);
            } else if (personPlan->getChildEdges().size() > 0) {
                myPathCreator->addEdge(personPlan->getChildEdges().front(), false, false);
            }
        } else {
            myPersonPlanAttributes->hideAttributesCreatorModul();
            myPathCreator->hidePathCreatorModul();
            myPersonHierarchy->hideAttributeCarrierHierarchy();
        }
    } else {
        // hide moduls if person selected isn't valid
        myPersonPlanTagSelector->hideTagSelector();
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myPathCreator->hidePathCreatorModul();
        myPersonHierarchy->hideAttributeCarrierHierarchy();
    }
}


void
GNEPersonPlanFrame::createPath() {
    // first check that all attributes are valid
    if (!myPersonPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myPersonPlanTagSelector->getCurrentTagProperties().getTagStr() + " parameters.");
    } else {
        // declare flag
        bool personPlanCreated = false;
        // get busStop
        GNEAdditional *busStop = myPathCreator->getSelectedAdditionals().empty()? nullptr : myPathCreator->getSelectedAdditionals().back();
        // Declare map to keep attributes from myPersonPlanAttributes
        std::map<SumoXMLAttr, std::string> valuesMap = myPersonPlanAttributes->getAttributesAndValues(true);
        // check what PersonPlan we're creating
        switch (myPersonPlanTagSelector->getCurrentTagProperties().getTag()) {
            case SUMO_TAG_PERSONTRIP_FROMTO: {
                std::vector<std::string> types = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_VTYPES]);
                std::vector<std::string> modes = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_MODES]);
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                // check if person trip fromto can be created
                if (myPathCreator->getPath().size() > 1) {
                    GNERouteHandler::buildPersonTripFromTo(myViewNet->getNet(), true, myPersonSelector->getCurrentDemandElement(), myPathCreator->getSelectedEdges().front(), myPathCreator->getSelectedEdges().back(), arrivalPos, types, modes);
                    personPlanCreated = true;
                } else {
                    myViewNet->setStatusBarText("A person trip with from-to attributes needs at least two edge.");
                }
                break;
            }
            case SUMO_TAG_PERSONTRIP_BUSSTOP: {
                std::vector<std::string> types = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_VTYPES]);
                std::vector<std::string> modes = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_MODES]);
                // check if person trip busstop can be created
                if (busStop == nullptr) {
                    myViewNet->setStatusBarText("A person trip with from and busStop attributes needs one edge and one busStop");
                } else {
                    // add busstop's edge to personPlan creator (To calculate a temporal route)
                    myPathCreator->addEdge(busStop->getParentLanes().front()->getParentEdge(), false, false);
                    if (myPathCreator->getSelectedEdges().size() > 0) {
                        GNERouteHandler::buildPersonTripBusStop(myViewNet->getNet(), true, myPersonSelector->getCurrentDemandElement(), myPathCreator->getSelectedEdges().front(), busStop, types, modes);
                        personPlanCreated = true;
                    } else {
                        myViewNet->setStatusBarText("A person trip with from and busStop attributes needs one edge and one busStop");
                    }
                }
                break;
            }
            case SUMO_TAG_WALK_EDGES: {
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                // check if walk edges can be created
                if (myPathCreator->getSelectedEdges().size() > 0) {
                    GNERouteHandler::buildWalkEdges(myViewNet->getNet(), true, myPersonSelector->getCurrentDemandElement(), myPathCreator->getSelectedEdges(), arrivalPos);
                } else if ((myPathCreator->getSelectedEdges().size() == 1) || (myPathCreator->getSelectedEdges().size() == 2)) {
                    GNERouteHandler::buildWalkEdges(myViewNet->getNet(), true, myPersonSelector->getCurrentDemandElement(), myPathCreator->getSelectedEdges(), arrivalPos);
                    personPlanCreated = true;
                } else {
                    myViewNet->setStatusBarText("A walk with edges attribute needs a list of edges");
                }
                break;
            }
            case SUMO_TAG_WALK_FROMTO: {
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                // check if walk fromto can be created
                if (myPathCreator->getSelectedEdges().size() > 1) {
                    GNERouteHandler::buildWalkFromTo(myViewNet->getNet(), true, myPersonSelector->getCurrentDemandElement(), myPathCreator->getSelectedEdges().front(), myPathCreator->getSelectedEdges().back(), arrivalPos);
                    personPlanCreated = true;
                } else {
                    myViewNet->setStatusBarText("A walk with from-to attributes needs at least two edges.");
                }
                break;
            }
            case SUMO_TAG_WALK_BUSSTOP: {
                // check if walk busstop can be created
                if (busStop == nullptr) {
                    myViewNet->setStatusBarText("A walk with from and busStop attributes needs one edge and one busStop");
                } else {
                    // add busstop's edge to personPlan creator (To calculate a temporal route)
                    myPathCreator->addEdge(busStop->getParentLanes().front()->getParentEdge(), false, false);
                    if (myPathCreator->getSelectedEdges().size() > 0) {
                        GNERouteHandler::buildWalkBusStop(myViewNet->getNet(), true, myPersonSelector->getCurrentDemandElement(), myPathCreator->getSelectedEdges().front(), busStop);
                        personPlanCreated = true;
                    } else {
                        myViewNet->setStatusBarText("A walk with from and busStop attributes needs one edge and one busStop");
                    }
                }
                break;
            }
            case SUMO_TAG_WALK_ROUTE: {
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
/*
                // check if walk route can be created
                if (route != nullptr) {
                    GNERouteHandler::buildWalkRoute(myViewNet->getNet(), true, myPersonSelector->getCurrentDemandElement(), route, arrivalPos);
                    personPlanCreated = true;
                } else {
                    myViewNet->setStatusBarText("A ride with route attributes needs one route");
                }
*/
                break;
            }
            case SUMO_TAG_RIDE_FROMTO: {
                std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_LINES]);
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                // check if ride fromto can be created
                if (myPathCreator->getSelectedEdges().size() > 2) {
                    GNERouteHandler::buildRideFromTo(myViewNet->getNet(), true, myPersonSelector->getCurrentDemandElement(), myPathCreator->getSelectedEdges().front(), myPathCreator->getSelectedEdges().back(), lines, arrivalPos);
                    personPlanCreated = true;
                } else {
                    myViewNet->setStatusBarText("A ride with from-to attributes needs at least two edge.");
                }
                break;
            }
            case SUMO_TAG_RIDE_BUSSTOP: {
                std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_LINES]);
                // check if ride busstop can be created
                if (busStop == nullptr) {
                    myViewNet->setStatusBarText("A ride with from and busStop attributes needs one edge and one busStop");
                } else {
                    // add busstop's edge to personPlan creator (To calculate a temporal route)
                    myPathCreator->addEdge(busStop->getParentLanes().front()->getParentEdge(), false, false);
                    if (myPathCreator->getSelectedEdges().size() > 0) {
                        GNERouteHandler::buildRideBusStop(myViewNet->getNet(), true, myPersonSelector->getCurrentDemandElement(), myPathCreator->getSelectedEdges().front(), busStop, lines);
                        personPlanCreated = true;
                    } else {
                        myViewNet->setStatusBarText("A ride with from and busStop attributes needs one edge and one busStop");
                    }
                }
                break;
            }
            default:
                throw InvalidArgument("Invalid person plan tag");
        }
        // refresh AttributeCarrierHierarchy
        myPersonHierarchy->refreshAttributeCarrierHierarchy();
        // refresh also Person Plan creator
/*
        myPathCreator->refreshPersonPlanCreator();
*/
        // refresh personPlan attributes
        myPersonPlanAttributes->refreshRows();
    }
}


/****************************************************************************/
