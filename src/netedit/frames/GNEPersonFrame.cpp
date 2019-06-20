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

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/demandelements/GNEPerson.h>
#include <netedit/demandelements/GNERouteHandler.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

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

     // create person attributes
    myPersonAttributes = new AttributesCreator(this);

    // create tag Selector modul for person plans
    myPersonPlanTagSelector = new TagSelector(this, GNEAttributeCarrier::TagType::TAGTYPE_PERSONPLAN);

    // create person plan attribuets
    myPersonPlanAttributes = new AttributesCreator(this);

    // create EdgePathCreator Modul
    myEdgePathCreator = new EdgePathCreator(this, EdgePathCreator::EdgePathCreatorModes::GNE_EDGEPATHCREATOR_FROM_TO_VIA);

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
    // add clicked edge in EdgePathCreator
    if (objectsUnderCursor.getAdditionalFront() && (objectsUnderCursor.getAdditionalFront()->getTagProperty().getTag() == SUMO_TAG_BUS_STOP)) {
        return myEdgePathCreator->addBusStop(objectsUnderCursor.getAdditionalFront());
    } else if (objectsUnderCursor.getEdgeFront()) {
        return myEdgePathCreator->addEdge(objectsUnderCursor.getEdgeFront());
    } else {
        return false;
    }
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
            // show person attributes
            myPersonAttributes->showAttributesCreatorModul(myPersonTagSelector->getCurrentTagProperties());
            // show person plan tag selector
            myPersonPlanTagSelector->showTagSelector();
            // now check if person plan selected is valid
            if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
                // show person plan attributes
                myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties());
                // set edge path creator name
                if (myPersonPlanTagSelector->getCurrentTagProperties().isPersonTrip()) {
                    myEdgePathCreator->edgePathCreatorName("person trip");
                } else if (myPersonPlanTagSelector->getCurrentTagProperties().isWalk()) {
                    myEdgePathCreator->edgePathCreatorName("walk");
                } else if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                    myEdgePathCreator->edgePathCreatorName("ride");
                }
                myEdgePathCreator->showEdgePathCreator();
                myHelpCreation->showHelpCreation();
            } else {
                myPersonPlanAttributes->hideAttributesCreatorModul();
                myEdgePathCreator->hideEdgePathCreator();
                myHelpCreation->hideHelpCreation();
            }
        } else {
            myPersonPlanTagSelector->hideTagSelector();
            myPersonAttributes->hideAttributesCreatorModul();
            myPersonPlanAttributes->hideAttributesCreatorModul();
            myEdgePathCreator->hideEdgePathCreator();
            myHelpCreation->hideHelpCreation();
        }
    } else {
        // hide all moduls if person isn't valid
        myPTypeSelector->hideDemandElementSelector();
        myPersonPlanTagSelector->hideTagSelector();
        myPersonAttributes->hideAttributesCreatorModul();
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myEdgePathCreator->hideEdgePathCreator();
        myHelpCreation->hideHelpCreation();
    }
}


void 
GNEPersonFrame::demandElementSelected() {
    if (myPTypeSelector->getCurrentDemandElement()) {
        // show person attributes
        myPersonAttributes->showAttributesCreatorModul(myPersonTagSelector->getCurrentTagProperties());
        // show person plan tag selector
        myPersonPlanTagSelector->showTagSelector();
        // now check if person plan selected is valid
        if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
            // show person plan attributes
            myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties());
            // set edge path creator name
            if (myPersonPlanTagSelector->getCurrentTagProperties().isPersonTrip()) {
                myEdgePathCreator->edgePathCreatorName("person trip");
            } else if (myPersonPlanTagSelector->getCurrentTagProperties().isWalk()) {
                myEdgePathCreator->edgePathCreatorName("walk");
            } else if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                myEdgePathCreator->edgePathCreatorName("ride");
            }
            myEdgePathCreator->showEdgePathCreator();
            myHelpCreation->showHelpCreation();
        } else {
            myPersonPlanAttributes->hideAttributesCreatorModul();
            myEdgePathCreator->hideEdgePathCreator();
            myHelpCreation->hideHelpCreation();
        }
    } else {
        myPersonPlanTagSelector->hideTagSelector();
        myPersonAttributes->hideAttributesCreatorModul();
        myPersonPlanAttributes->hideAttributesCreatorModul();
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
        // obtain person tag (only for improve code legibility)
        SumoXMLTag personTag = myPersonTagSelector->getCurrentTagProperties().getTag();
        // obtain person plan tag (only for improve code legibility)
        SumoXMLTag personPlanTag = myPersonPlanTagSelector->getCurrentTagProperties().getTag();
        // begin undo-redo operation
        myViewNet->getUndoList()->p_begin("create " + toString(personTag) + " and " + toString(personPlanTag));
        // Declare map to keep attributes from myPersonAttributes
        std::map<SumoXMLAttr, std::string> valuesMap = myPersonAttributes->getAttributesAndValues(false);
        // add ID parameter
        valuesMap[SUMO_ATTR_ID] = myViewNet->getNet()->generateDemandElementID("", personTag);
        // check if we're creating a person or personFlow
        if (personTag == SUMO_TAG_PERSON) {
            // Add parameter departure
            if (valuesMap[SUMO_ATTR_DEPART].empty()) {
                valuesMap[SUMO_ATTR_DEPART] = "0";
            }
            // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
            SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(personTag));
            // obtain person parameters
            SUMOVehicleParameter* personParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMOSAXAttrs, false, false, true);
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
            SUMOVehicleParameter* personFlowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMOSAXAttrs, 0, SUMOTime_MAX, true);
            // build personFlow in GNERouteHandler
            GNERouteHandler::buildPersonFlow(myViewNet, true, *personFlowParameters);
            // delete personFlowParameters
            delete personFlowParameters;
        }
        // obtain created person
        GNEDemandElement *createdPerson = myViewNet->getNet()->retrieveDemandElement(personTag, valuesMap[SUMO_ATTR_ID]);
        // Declare map to keep attributes from myPersonPlanAttributes
        valuesMap = myPersonPlanAttributes->getAttributesAndValues(true);
        // check what PersonPlan we're creating
        switch (personPlanTag) {
            case SUMO_TAG_PERSONTRIP_FROMTO: {
                // obtain attributes
                std::vector<std::string> types = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_VTYPES]);
                std::vector<std::string> modes = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_MODES]);
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                // now check that number of selected edges are correct
                if (myEdgePathCreator->getClickedEdges().size() > 0) {
                    GNERouteHandler::buildPersonTripFromTo(myViewNet, true, createdPerson, myEdgePathCreator->getClickedEdges(), types, modes, arrivalPos);
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
                if ((myEdgePathCreator->getClickedEdges().size() == 1) && myEdgePathCreator->getClickedBusStop()) {
                    GNERouteHandler::buildPersonTripBusStop(myViewNet, true, createdPerson, myEdgePathCreator->getClickedEdges(), myEdgePathCreator->getClickedBusStop(), types, modes);
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
                GNERouteHandler::buildWalkFromTo(myViewNet, true, createdPerson, myEdgePathCreator->getClickedEdges(), arrivalPos);
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            case SUMO_TAG_WALK_BUSSTOP: {
                // obtain attributes
                GNERouteHandler::buildWalkBusStop(myViewNet, true, createdPerson, myEdgePathCreator->getClickedEdges(), myEdgePathCreator->getClickedBusStop());
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            case SUMO_TAG_WALK_ROUTE: {
                // obtain attributes
                GNEDemandElement *route = myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, valuesMap[SUMO_ATTR_ROUTE]);
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildWalkRoute(myViewNet, true, createdPerson, route, arrivalPos);
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            case SUMO_TAG_RIDE_FROMTO: {
                // obtain attributes
                std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_LINES]);
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildRideFromTo(myViewNet, true, createdPerson, myEdgePathCreator->getClickedEdges(), lines, arrivalPos);
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            case SUMO_TAG_RIDE_BUSSTOP: {
                // obtain attributes
                std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_LINES]);
                GNERouteHandler::buildRideBusStop(myViewNet, true, createdPerson, myEdgePathCreator->getClickedEdges(), myEdgePathCreator->getClickedBusStop(), lines);
                // end undo-redo operation
                myViewNet->getUndoList()->p_end();
                break;
            }
            default:
                throw InvalidArgument("Invalid person plan tag");
        }
    }
}

/****************************************************************************/
