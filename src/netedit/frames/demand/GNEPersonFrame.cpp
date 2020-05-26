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

    // create PathCreator Modul
    myPathCreator = new GNEFrameModuls::PathCreator(this, 0);

    // limit path creator to pedestrians
    myPathCreator->setVClass(SVC_PEDESTRIAN);

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
    // update VClass of myPathCreator
    if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
        myPathCreator->setVClass(SVC_PASSENGER);
    } else {
        myPathCreator->setVClass(SVC_PEDESTRIAN);
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
        return myPathCreator->addAdditional(objectsUnderCursor.getAdditionalFront(), false, false);
    } else if (objectsUnderCursor.getEdgeFront()) {
        return myPathCreator->addEdge(objectsUnderCursor.getEdgeFront(), false, false);
    } else {
        return false;
    }
}


GNEFrameModuls::PathCreator*
GNEPersonFrame::getPathCreator() const {
    return myPathCreator;
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
                // check if myPathCreator has to be show
                if ((myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_WALK_ROUTE) ||
                        (myPersonPlanTagSelector->getCurrentTagProperties().isPersonStop())) {
                    // hide edge path creator modul
                    myPathCreator->hidePathCreatorModul();
                    // show Netedit attributes modul
                    myNeteditAttributes->showNeteditAttributesModul(myPersonPlanTagSelector->getCurrentTagProperties());
                } else {
                    // set edge path creator mode
                    myPathCreator->setPathCreatorMode(myPersonPlanTagSelector->getCurrentTagProperties().getTag(), false, false);
                    // update VClass of myPathCreator
                    if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                        myPathCreator->setVClass(SVC_PASSENGER);
                    } else {
                        myPathCreator->setVClass(SVC_PEDESTRIAN);
                    }
                    // show edge path creator modul
                    myPathCreator->showPathCreatorModul();
                    // hide Netedit attributes modul
                    myNeteditAttributes->hideNeteditAttributesModul();
                }
            } else {
                // hide modules
                myPersonPlanAttributes->hideAttributesCreatorModul();
                myPathCreator->hidePathCreatorModul();
            }
        } else {
            // hide modules
            myPersonPlanTagSelector->hideTagSelector();
            myPersonAttributes->hideAttributesCreatorModul();
            myPersonPlanAttributes->hideAttributesCreatorModul();
            myNeteditAttributes->hideNeteditAttributesModul();
            myPathCreator->hidePathCreatorModul();
        }
    } else {
        // hide all moduls if person isn't valid
        myPTypeSelector->hideDemandElementSelector();
        myPersonPlanTagSelector->hideTagSelector();
        myPersonAttributes->hideAttributesCreatorModul();
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myNeteditAttributes->hideNeteditAttributesModul();
        myPathCreator->hidePathCreatorModul();
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
            // check if myPathCreator has to be show
            if ((myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_WALK_ROUTE) ||
                    (myPersonPlanTagSelector->getCurrentTagProperties().isPersonStop())) {
                // hide edge path creator modul
                myPathCreator->hidePathCreatorModul();
                // show Netedit attributes modul
                myNeteditAttributes->showNeteditAttributesModul(myPersonPlanTagSelector->getCurrentTagProperties());
            } else {
                // set edge path creator mode
                myPathCreator->setPathCreatorMode(myPersonPlanTagSelector->getCurrentTagProperties().getTag(), false, false);
                // update VClass of myPathCreator
                if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                    myPathCreator->setVClass(SVC_PASSENGER);
                } else {
                    myPathCreator->setVClass(SVC_PEDESTRIAN);
                }
                // show edge path creator modul
                myPathCreator->showPathCreatorModul();
                // hide Netedit attributes modul
                myNeteditAttributes->hideNeteditAttributesModul();
            }
        } else {
            // hide modules
            myPersonPlanAttributes->hideAttributesCreatorModul();
            myNeteditAttributes->hideNeteditAttributesModul();
            myPathCreator->hidePathCreatorModul();
        }
    } else {
        // hide modules
        myPersonPlanTagSelector->hideTagSelector();
        myPersonAttributes->hideAttributesCreatorModul();
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myNeteditAttributes->hideNeteditAttributesModul();
        myPathCreator->hidePathCreatorModul();
    }
}


void
GNEPersonFrame::createPath() {
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
                if (myPathCreator->getSelectedEdges().size() > 1) {
                    GNERouteHandler::buildPersonTripFromTo(myViewNet->getNet(), true, createdPerson, myPathCreator->getSelectedEdges().front(), myPathCreator->getSelectedEdges().back(), arrivalPos, types, modes);
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
                if ((myPathCreator->getSelectedEdges().size() > 0) && myPathCreator->getToAdditional()) {
                    GNERouteHandler::buildPersonTripBusStop(myViewNet->getNet(), true, createdPerson, myPathCreator->getSelectedEdges().front(), myPathCreator->getToAdditional(), types, modes);
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
                GNERouteHandler::buildWalkEdges(myViewNet->getNet(), true, createdPerson, myPathCreator->getSelectedEdges(), arrivalPos);
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            case SUMO_TAG_WALK_FROMTO: {
                // obtain attributes
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildWalkFromTo(myViewNet->getNet(), true, createdPerson, myPathCreator->getSelectedEdges().front(), myPathCreator->getSelectedEdges().back(), arrivalPos);
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            case SUMO_TAG_WALK_BUSSTOP: {
                // obtain attributes
                GNERouteHandler::buildWalkBusStop(myViewNet->getNet(), true, createdPerson, myPathCreator->getSelectedEdges().front(), myPathCreator->getToAdditional());
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            case SUMO_TAG_RIDE_FROMTO: {
                // obtain attributes
                std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_LINES]);
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildRideFromTo(myViewNet->getNet(), true, createdPerson, myPathCreator->getSelectedEdges().front(), myPathCreator->getSelectedEdges().back(), lines, arrivalPos);
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            case SUMO_TAG_RIDE_BUSSTOP: {
                // obtain attributes
                std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_LINES]);
                GNERouteHandler::buildRideBusStop(myViewNet->getNet(), true, createdPerson, myPathCreator->getSelectedEdges().front(), myPathCreator->getToAdditional(), lines);
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            default:
                throw InvalidArgument("Invalid person plan tag");
        }
        // abort path creation
        myPathCreator->abortPathCreation();
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
            GNERouteHandler::buildWalkRoute(myViewNet->getNet(), true, buildPerson(), route, 0);
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
            GNERouteHandler::buildStop(myViewNet->getNet(), true, stopParameter, buildPerson());
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
        GNERouteHandler::buildPerson(myViewNet->getNet(), true, *personParameters);
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
        GNERouteHandler::buildPersonFlow(myViewNet->getNet(), true, *personFlowParameters);
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
