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
/// @file    GNEPersonFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// The Widget for add Person elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/demand/GNEPerson.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNEPersonFrame.h"
#include "GNEStopFrame.h"

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
    // set text depending of selected person plan
    switch (myPersonFrameParent->myPersonPlanTagSelector->getCurrentTagProperties().getTag()) {
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
        case SUMO_TAG_PERSONSTOP_BUSSTOP:
            information
                    << "- Click over a busStop\n"
                    << "  to create a stop";
            break;
        case SUMO_TAG_PERSONSTOP_LANE:
            information
                    << "- Click over a lane\n"
                    << "  to create a stop";
            break;
        default:
            throw ProcessError("Invalid person plan tag");
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
    myPersonTagSelector = new GNEFrameModuls::TagSelector(this, GNETagProperties::TagType::PERSON);

    // create person types selector modul
    myPTypeSelector = new GNEFrameModuls::DemandElementSelector(this, SUMO_TAG_PTYPE);

    // create person attributes
    myPersonAttributes = new GNEFrameAttributesModuls::AttributesCreator(this);

    // create tag Selector modul for person plans
    myPersonPlanTagSelector = new GNEFrameModuls::TagSelector(this, GNETagProperties::TagType::PERSONPLAN);

    // create person plan attributes
    myPersonPlanAttributes = new GNEFrameAttributesModuls::AttributesCreator(this);

    // Create Netedit parameter
    myNeteditAttributes = new GNEFrameAttributesModuls::NeteditAttributes(this);

    // create EdgePathCreator Modul
    myEdgePathCreator = new GNEFrameModuls::EdgePathCreator(this, GNEFrameModuls::EdgePathCreator::Modes::FROM_TO_VIA);

    // create Help Creation Modul
    myHelpCreation = new HelpCreation(this);

    // limit path creator to pedestrians
    myEdgePathCreator->setVClass(SVC_PEDESTRIAN);

    // set Person as default vehicle
    myPersonTagSelector->setCurrentTag(SUMO_TAG_PERSON);
}


GNEPersonFrame::~GNEPersonFrame() {}


void
GNEPersonFrame::show() {
    // refresh item selector
    myPersonTagSelector->refreshTagProperties();
    myPTypeSelector->refreshDemandElementSelector();
    myPersonPlanTagSelector->refreshTagProperties();
    // update VClass of myEdgePathCreator
    if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
        myEdgePathCreator->setVClass(SVC_PASSENGER);
    } else {
        myEdgePathCreator->setVClass(SVC_PEDESTRIAN);
    }
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
    if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected person plan isn't valid.");
        return false;
    }
    // check if walk routes can be created
    if ((myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_WALK_ROUTE)) {
        return buildPersonOverRoute(objectsUnderCursor.getDemandElementFront());
    } else if ((myPersonPlanTagSelector->getCurrentTagProperties().isPersonStop())) {
        return buildPersonOverStop(objectsUnderCursor.getLaneFront(), objectsUnderCursor.getAdditionalFront());
    } else if (objectsUnderCursor.getAdditionalFront() && (objectsUnderCursor.getAdditionalFront()->getTagProperty().getTag() == SUMO_TAG_BUS_STOP)) {
        return myEdgePathCreator->addBusStop(objectsUnderCursor.getAdditionalFront());
    } else if (objectsUnderCursor.getEdgeFront()) {
        return myEdgePathCreator->addPathEdge(objectsUnderCursor.getEdgeFront());
    } else {
        return false;
    }
}


GNEFrameModuls::EdgePathCreator*
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
        // check if current person type selected is valid
        if (myPTypeSelector->getCurrentDemandElement()) {
            // show person attributes depending of myPersonPlanTagSelector
            if (myPersonPlanTagSelector->getCurrentTagProperties().isPersonStop()) {
                myPersonAttributes->showAttributesCreatorModul(myPersonTagSelector->getCurrentTagProperties(), {SUMO_ATTR_DEPARTPOS});
            } else {
                myPersonAttributes->showAttributesCreatorModul(myPersonTagSelector->getCurrentTagProperties(), {});
            }
            // show person plan tag selector
            myPersonPlanTagSelector->showTagSelector();
            // now check if person plan selected is valid
            if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
                // check if person plan attributes has to be shown
                if ((myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_WALK_BUSSTOP)) {
                    // hide person plan attributes
                    myPersonPlanAttributes->hideAttributesCreatorModul();
                } else {
                    // show person plan attributes
                    myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties(), {});
                }
                // check if myEdgePathCreator has to be show
                if ((myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_WALK_ROUTE) ||
                        (myPersonPlanTagSelector->getCurrentTagProperties().isPersonStop())) {
                    // hide edge path creator modul
                    myEdgePathCreator->hideEdgePathCreator();
                    // show Netedit attributes modul
                    myNeteditAttributes->showNeteditAttributesModul(myPersonPlanTagSelector->getCurrentTagProperties());
                } else {
                    // set edge path creator name
                    if (myPersonPlanTagSelector->getCurrentTagProperties().isPersonTrip()) {
                        myEdgePathCreator->edgePathCreatorName("person trip");
                    } else if (myPersonPlanTagSelector->getCurrentTagProperties().isWalk()) {
                        myEdgePathCreator->edgePathCreatorName("walk");
                    } else if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                        myEdgePathCreator->edgePathCreatorName("ride");
                    }
                    // update VClass of myEdgePathCreator
                    if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                        myEdgePathCreator->setVClass(SVC_PASSENGER);
                    } else {
                        myEdgePathCreator->setVClass(SVC_PEDESTRIAN);
                    }
                    // show edge path creator modul
                    myEdgePathCreator->showEdgePathCreator();
                    // hide Netedit attributes modul
                    myNeteditAttributes->hideNeteditAttributesModul();
                }
                // show help modul
                myHelpCreation->showHelpCreation();
            } else {
                // hide modules
                myPersonPlanAttributes->hideAttributesCreatorModul();
                myEdgePathCreator->hideEdgePathCreator();
                myHelpCreation->hideHelpCreation();
            }
        } else {
            // hide modules
            myPersonPlanTagSelector->hideTagSelector();
            myPersonAttributes->hideAttributesCreatorModul();
            myPersonPlanAttributes->hideAttributesCreatorModul();
            myNeteditAttributes->hideNeteditAttributesModul();
            myEdgePathCreator->hideEdgePathCreator();
            myHelpCreation->hideHelpCreation();
        }
    } else {
        // hide all moduls if person isn't valid
        myPTypeSelector->hideDemandElementSelector();
        myPersonPlanTagSelector->hideTagSelector();
        myPersonAttributes->hideAttributesCreatorModul();
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myNeteditAttributes->hideNeteditAttributesModul();
        myEdgePathCreator->hideEdgePathCreator();
        myHelpCreation->hideHelpCreation();
    }
}


void
GNEPersonFrame::demandElementSelected() {
    if (myPTypeSelector->getCurrentDemandElement()) {
        // show person attributes depending of myPersonPlanTagSelector
        if (myPersonPlanTagSelector->getCurrentTagProperties().isPersonStop()) {
            myPersonAttributes->showAttributesCreatorModul(myPersonTagSelector->getCurrentTagProperties(), {SUMO_ATTR_DEPARTPOS});
        } else {
            myPersonAttributes->showAttributesCreatorModul(myPersonTagSelector->getCurrentTagProperties(), {});
        }
        // show person plan tag selector
        myPersonPlanTagSelector->showTagSelector();
        // now check if person plan selected is valid
        if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
            // check if person plan attributes has to be shown
            if ((myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_WALK_BUSSTOP)) {
                // hide person plan attributes
                myPersonPlanAttributes->hideAttributesCreatorModul();
            } else {
                // show person plan attributes
                myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties(), {});
            }
            // check if myEdgePathCreator has to be show
            if ((myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_WALK_ROUTE) ||
                    (myPersonPlanTagSelector->getCurrentTagProperties().isPersonStop())) {
                // hide edge path creator modul
                myEdgePathCreator->hideEdgePathCreator();
                // show Netedit attributes modul
                myNeteditAttributes->showNeteditAttributesModul(myPersonPlanTagSelector->getCurrentTagProperties());
            } else {
                // set edge path creator name
                if (myPersonPlanTagSelector->getCurrentTagProperties().isPersonTrip()) {
                    myEdgePathCreator->edgePathCreatorName("person trip");
                    // set mode in path creator
                    if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_PERSONTRIP_BUSSTOP) {
                        myEdgePathCreator->setModes(GNEFrameModuls::EdgePathCreator::Modes::TO_BUSSTOP);
                    } else {
                        myEdgePathCreator->setModes(GNEFrameModuls::EdgePathCreator::Modes::FROM_TO_VIA);
                    }
                } else if (myPersonPlanTagSelector->getCurrentTagProperties().isWalk()) {
                    myEdgePathCreator->edgePathCreatorName("walk");
                    // set mode in path creator
                    if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_WALK_BUSSTOP) {
                        myEdgePathCreator->setModes(GNEFrameModuls::EdgePathCreator::Modes::TO_BUSSTOP);
                    } else if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_WALK_EDGES) {
                        myEdgePathCreator->setModes(GNEFrameModuls::EdgePathCreator::Modes::CONSECUTIVE);
                    } else {
                        myEdgePathCreator->setModes(GNEFrameModuls::EdgePathCreator::Modes::FROM_TO_VIA);
                    }
                } else if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                    myEdgePathCreator->edgePathCreatorName("ride");
                    // set mode in path creator
                    if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_RIDE_BUSSTOP) {
                        myEdgePathCreator->setModes(GNEFrameModuls::EdgePathCreator::Modes::TO_BUSSTOP);
                    } else {
                        myEdgePathCreator->setModes(GNEFrameModuls::EdgePathCreator::Modes::FROM_TO_VIA);
                    }
                }
                // update VClass of myEdgePathCreator
                if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                    myEdgePathCreator->setVClass(SVC_PASSENGER);
                } else {
                    myEdgePathCreator->setVClass(SVC_PEDESTRIAN);
                }
                // show edge path creator modul
                myEdgePathCreator->showEdgePathCreator();
                // hide Netedit attributes modul
                myNeteditAttributes->hideNeteditAttributesModul();
            }
            // show help modul
            myHelpCreation->showHelpCreation();
        } else {
            // hide modules
            myPersonPlanAttributes->hideAttributesCreatorModul();
            myNeteditAttributes->hideNeteditAttributesModul();
            myEdgePathCreator->hideEdgePathCreator();
            myHelpCreation->hideHelpCreation();
        }
    } else {
        // hide modules
        myPersonPlanTagSelector->hideTagSelector();
        myPersonAttributes->hideAttributesCreatorModul();
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myNeteditAttributes->hideNeteditAttributesModul();
        myEdgePathCreator->hideEdgePathCreator();
        myHelpCreation->hideHelpCreation();
    }
}


void
GNEPersonFrame::edgePathCreated() {
    // first check that all attributes are valid
    if (!myPersonAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid person parameters.");
    } else if (!myPersonPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myPersonPlanTagSelector->getCurrentTagProperties().getTagStr() + " parameters.");
    } else {
        // build person
        GNEDemandElement* createdPerson = buildPerson();
        // Declare map to keep attributes from myPersonPlanAttributes
        std::map<SumoXMLAttr, std::string> valuesMap = myPersonPlanAttributes->getAttributesAndValues(true);
        // check what PersonPlan we're creating
        switch (myPersonPlanTagSelector->getCurrentTagProperties().getTag()) {
            case SUMO_TAG_PERSONTRIP_FROMTO: {
                // obtain attributes
                std::vector<std::string> types = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_VTYPES]);
                std::vector<std::string> modes = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_MODES]);
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                // now check that number of selected edges are correct
                if (myEdgePathCreator->getClickedEdges().size() > 1) {
                    GNERouteHandler::buildPersonTripFromTo(myViewNet, true, createdPerson, myEdgePathCreator->getClickedEdges().front(), myEdgePathCreator->getClickedEdges().back(), arrivalPos, types, modes);
                    // end undo-redo operation
                    myViewNet->getUndoList()->p_end();
                } else {
                    myViewNet->setStatusBarText("A person trip with from-to attributes needs at least one edge.");
                    // abort person creation
                    myViewNet->getUndoList()->p_abort();
                }
                break;
            }
            case SUMO_TAG_PERSONTRIP_BUSSTOP: {
                // obtain attributes
                std::vector<std::string> types = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_VTYPES]);
                std::vector<std::string> modes = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_MODES]);
                // now check that number of selected edges are correct
                if ((myEdgePathCreator->getClickedEdges().size() > 0) && myEdgePathCreator->getClickedBusStop()) {
                    GNERouteHandler::buildPersonTripBusStop(myViewNet, true, createdPerson, myEdgePathCreator->getClickedEdges().front(), myEdgePathCreator->getClickedBusStop(), types, modes);
                    // end undo-redo operation
                    myViewNet->getUndoList()->p_end();
                } else {
                    myViewNet->setStatusBarText("A person trip with from-to attributes needs at least one edge.");
                    // abort person creation
                    myViewNet->getUndoList()->p_abort();
                }
                break;
            }
            case SUMO_TAG_WALK_EDGES: {
                // obtain attributes
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildWalkEdges(myViewNet, true, createdPerson, myEdgePathCreator->getClickedEdges(), arrivalPos);
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            case SUMO_TAG_WALK_FROMTO: {
                // obtain attributes
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildWalkFromTo(myViewNet, true, createdPerson, myEdgePathCreator->getClickedEdges().front(), myEdgePathCreator->getClickedEdges().back(), arrivalPos);
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            case SUMO_TAG_WALK_BUSSTOP: {
                // obtain attributes
                GNERouteHandler::buildWalkBusStop(myViewNet, true, createdPerson, myEdgePathCreator->getClickedEdges().front(), myEdgePathCreator->getClickedBusStop());
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            case SUMO_TAG_RIDE_FROMTO: {
                // obtain attributes
                std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_LINES]);
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildRideFromTo(myViewNet, true, createdPerson, myEdgePathCreator->getClickedEdges().front(), myEdgePathCreator->getClickedEdges().back(), lines, arrivalPos);
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            case SUMO_TAG_RIDE_BUSSTOP: {
                // obtain attributes
                std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_LINES]);
                GNERouteHandler::buildRideBusStop(myViewNet, true, createdPerson, myEdgePathCreator->getClickedEdges().front(), myEdgePathCreator->getClickedBusStop(), lines);
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            default:
                throw InvalidArgument("Invalid person plan tag");
        }
        // refresh person and personPlan attributes
        myPersonAttributes->refreshRows();
        myPersonPlanAttributes->refreshRows();
    }
}

// ---------------------------------------------------------------------------
// GNEPersonFrame - private methods
// ---------------------------------------------------------------------------


bool
GNEPersonFrame::buildPersonOverRoute(GNEDemandElement* route) {
    if (route && (route->getTagProperty().getTag() == SUMO_TAG_ROUTE)) {
        // first check that all attributes are valid
        if (!myPersonAttributes->areValuesValid()) {
            myViewNet->setStatusBarText("Invalid person parameters.");
        } else if (!myPersonPlanAttributes->areValuesValid()) {
            myViewNet->setStatusBarText("Invalid " + myPersonPlanTagSelector->getCurrentTagProperties().getTagStr() + " parameters.");
        } else {
            // build person and walk over route
            GNERouteHandler::buildWalkRoute(myViewNet, true, buildPerson(), route, 0);
            // end undo-redo operation
            myViewNet->getUndoList()->p_end();
            return true;
        }
        return false;
    } else {
        myViewNet->setStatusBarText("Click over a " + toString(SUMO_TAG_ROUTE) + " to create a " + myPersonTagSelector->getCurrentTagProperties().getTagStr());
        return false;
    }
}


bool
GNEPersonFrame::buildPersonOverStop(GNELane* lane, GNEAdditional* busStop) {
    // first check that all attributes are valid
    if (!myPersonAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid person parameters.");
        return false;
    } else if (!myPersonPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myPersonPlanTagSelector->getCurrentTagProperties().getTagStr() + " parameters.");
        return false;
    } else {
        // declare stop parameters and friendly position
        SUMOVehicleParameter::Stop stopParameter;
        if (GNEStopFrame::getStopParameter(stopParameter, myPersonPlanTagSelector->getCurrentTagProperties().getTag(),
                                           myViewNet, myPersonPlanAttributes, myNeteditAttributes, lane, busStop)) {
            // create it in RouteFrame
            GNERouteHandler::buildStop(myViewNet, true, stopParameter, buildPerson());
            // end undo-redo operation
            myViewNet->getUndoList()->p_end();
            // stop sucesfully created, then return true
            return true;
        } else {
            return false;
        }
    }
}


GNEDemandElement*
GNEPersonFrame::buildPerson() {
    // obtain person tag (only for improve code legibility)
    SumoXMLTag personTag = myPersonTagSelector->getCurrentTagProperties().getTag();
    // obtain person plan tag (only for improve code legibility)
    SumoXMLTag personPlanTag = myPersonPlanTagSelector->getCurrentTagProperties().getTag();
    // begin undo-redo operation
    myViewNet->getUndoList()->p_begin("create " + toString(personTag) + " and " + toString(personPlanTag));
    // Declare map to keep attributes from myPersonAttributes
    std::map<SumoXMLAttr, std::string> valuesMap = myPersonAttributes->getAttributesAndValues(false);
    // Check if ID has to be generated
    if (valuesMap.count(SUMO_ATTR_ID) == 0) {
        valuesMap[SUMO_ATTR_ID] = myViewNet->getNet()->generateDemandElementID("", personTag);
    }
    // add pType parameter
    valuesMap[SUMO_ATTR_TYPE] = myPTypeSelector->getCurrentDemandElement()->getID();
    // check if we're creating a person or personFlow
    if (personTag == SUMO_TAG_PERSON) {
        // Add parameter departure
        if (valuesMap[SUMO_ATTR_DEPART].empty()) {
            valuesMap[SUMO_ATTR_DEPART] = "0";
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(personTag));
        // obtain person parameters
        SUMOVehicleParameter* personParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_PERSON, SUMOSAXAttrs, false, false, false);
        // build person in GNERouteHandler
        GNERouteHandler::buildPerson(myViewNet, true, *personParameters);
        // delete personParameters
        delete personParameters;
    } else {
        // set begin and end attributes
        if (valuesMap[SUMO_ATTR_BEGIN].empty()) {
            valuesMap[SUMO_ATTR_BEGIN] = "0";
        }
        if (valuesMap[SUMO_ATTR_END].empty()) {
            valuesMap[SUMO_ATTR_END] = "3600";
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(personTag));
        // obtain personFlow parameters
        SUMOVehicleParameter* personFlowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMOSAXAttrs, false, 0, SUMOTime_MAX, true);
        // build personFlow in GNERouteHandler
        GNERouteHandler::buildPersonFlow(myViewNet, true, *personFlowParameters);
        // delete personFlowParameters
        delete personFlowParameters;
    }
    // refresh person and personPlan attributes
    myPersonAttributes->refreshRows();
    myPersonPlanAttributes->refreshRows();
    // return created person
    return myViewNet->getNet()->retrieveDemandElement(personTag, valuesMap[SUMO_ATTR_ID]);
}


/****************************************************************************/
