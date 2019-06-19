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
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEPersonPlanFrame::PersonPlanCreator) PersonPlanCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_ABORT,          GNEPersonPlanFrame::PersonPlanCreator::onCmdAbortRouteCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_FINISH,         GNEPersonPlanFrame::PersonPlanCreator::onCmdFinishRouteCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_REMOVELASTEDGE, GNEPersonPlanFrame::PersonPlanCreator::onCmdRemoveLastRouteEdge)
};

// Object implementation
FXIMPLEMENT(GNEPersonPlanFrame::PersonPlanCreator,  FXGroupBox, PersonPlanCreatorMap, ARRAYNUMBER(PersonPlanCreatorMap))


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
// GNEPersonPlanFrame::PersonPlanCreator - methods
// ---------------------------------------------------------------------------

GNEPersonPlanFrame::PersonPlanCreator::PersonPlanCreator(GNEPersonPlanFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Route creator", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {

    // create button for create GEO POIs
    myFinishCreationButton = new FXButton(this, "Finish route creation", nullptr, this, MID_GNE_EDGEPATH_FINISH, GUIDesignButton);
    myFinishCreationButton->disable();

    // create button for create GEO POIs
    myAbortCreationButton = new FXButton(this, "Abort route creation", nullptr, this, MID_GNE_EDGEPATH_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();

    // create button for create GEO POIs
    myRemoveLastInsertedEdge = new FXButton(this, "Remove last inserted edge", nullptr, this, MID_GNE_EDGEPATH_REMOVELASTEDGE, GUIDesignButton);
    myRemoveLastInsertedEdge->disable();
}


GNEPersonPlanFrame::PersonPlanCreator::~PersonPlanCreator() {}


void 
GNEPersonPlanFrame::PersonPlanCreator::edgePathCreatorName(const std::string &name) {
    // header needs the first capitalized letter
    std::string nameWithFirstCapitalizedLetter = name;
    nameWithFirstCapitalizedLetter[0] = (char)toupper(nameWithFirstCapitalizedLetter.at(0));
    setText((nameWithFirstCapitalizedLetter + " creator").c_str());
    myFinishCreationButton->setText(("Finish " + name + " creation").c_str());
    myAbortCreationButton->setText(("Abort " + name + " creation").c_str());
}


void
GNEPersonPlanFrame::PersonPlanCreator::showPersonPlanCreator() {
    // disable buttons
    myFinishCreationButton->disable();
    myAbortCreationButton->disable();
    myRemoveLastInsertedEdge->disable();
    show();
}


void
GNEPersonPlanFrame::PersonPlanCreator::hidePersonPlanCreator() {
    hide();
}


std::vector<GNEEdge*>
GNEPersonPlanFrame::PersonPlanCreator::getSelectedEdges() const {
    return mySelectedEdges;
}


bool
GNEPersonPlanFrame::PersonPlanCreator::addEdge(GNEEdge* edge) {
    if (mySelectedEdges.empty() || ((mySelectedEdges.size() > 0) && (mySelectedEdges.back() != edge))) {
        mySelectedEdges.push_back(edge);
        // enable abort route button
        myAbortCreationButton->enable();
        // disable undo/redo
        myFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->disableUndoRedo("trip creation");
        // set special color
        for (auto i : edge->getLanes()) {
            i->setSpecialColor(&myFrameParent->getEdgeCandidateSelectedColor());
        }
        // calculate route if there is more than two edges
        if (mySelectedEdges.size() > 1) {
            // enable remove last edge button
            myRemoveLastInsertedEdge->enable();
            // enable finish button
            myFinishCreationButton->enable();
            // calculate temporal route
            myTemporalRoute = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(SVC_PEDESTRIAN, mySelectedEdges);
        }
        return true;
    } else {
        return false;
    }
}


bool 
GNEPersonPlanFrame::PersonPlanCreator::addBusStop(GNEAdditional* /*busStop*/) {
    return false;
}


void
GNEPersonPlanFrame::PersonPlanCreator::clearEdges() {
    // restore colors
    for (const auto& i : mySelectedEdges) {
        for (const auto& j : i->getLanes()) {
            j->setSpecialColor(nullptr);
        }
    }
    // clear edges
    mySelectedEdges.clear();
    myTemporalRoute.clear();
    // enable undo/redo
    myFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedo();
}


void
GNEPersonPlanFrame::PersonPlanCreator::drawTemporalRoute() const {
    // only draw if there is at least two edges
    if (myTemporalRoute.size() > 1) {
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, GLO_MAX);
        // set orange color
        GLHelper::setColor(RGBColor::ORANGE);
        // set line width
        glLineWidth(5);
        // draw first line
        GLHelper::drawLine(myTemporalRoute.at(0)->getNBEdge()->getLanes().front().shape.front(),
                           myTemporalRoute.at(0)->getNBEdge()->getLanes().front().shape.back());
        // draw rest of lines
        for (int i = 1; i < (int)myTemporalRoute.size(); i++) {
            GLHelper::drawLine(myTemporalRoute.at(i - 1)->getNBEdge()->getLanes().front().shape.back(),
                               myTemporalRoute.at(i)->getNBEdge()->getLanes().front().shape.front());
            GLHelper::drawLine(myTemporalRoute.at(i)->getNBEdge()->getLanes().front().shape.front(),
                               myTemporalRoute.at(i)->getNBEdge()->getLanes().front().shape.back());
        }
        // Pop last matrix
        glPopMatrix();
    }
}


void 
GNEPersonPlanFrame::PersonPlanCreator::abortEdgePathCreation() {
    if (myAbortCreationButton->isEnabled()) {
        onCmdAbortRouteCreation(nullptr, 0, nullptr);
    }
}


void 
GNEPersonPlanFrame::PersonPlanCreator::finishEdgePathCreation() {
    if (myFinishCreationButton->isEnabled()) {
        onCmdFinishRouteCreation(nullptr, 0, nullptr);
    }
}


void 
GNEPersonPlanFrame::PersonPlanCreator::removeLastAddedRoute() {
    if (myRemoveLastInsertedEdge->isEnabled()) {
        onCmdRemoveLastRouteEdge(nullptr, 0, nullptr);
    }
}


long
GNEPersonPlanFrame::PersonPlanCreator::onCmdAbortRouteCreation(FXObject*, FXSelector, void*) {
    clearEdges();
    // disable buttons
    myAbortCreationButton->disable();
    myFinishCreationButton->disable();
    myRemoveLastInsertedEdge->disable();
    return 1;
}


long
GNEPersonPlanFrame::PersonPlanCreator::onCmdFinishRouteCreation(FXObject*, FXSelector, void*) {
    // only create route if there is more than two edges
    if (mySelectedEdges.size() > 1) {
        // call edgePathCreated
        myFrameParent->edgePathCreated();
        // update view
        myFrameParent->myViewNet->update();
        // clear edges after creation
        clearEdges();
        // disable buttons
        myFinishCreationButton->disable();
        myAbortCreationButton->disable();
        myRemoveLastInsertedEdge->disable();
    }
    return 1;
}


long
GNEPersonPlanFrame::PersonPlanCreator::onCmdRemoveLastRouteEdge(FXObject*, FXSelector, void*) {
    if (mySelectedEdges.size() > 1) {
        // remove last edge
        mySelectedEdges.pop_back();
        // calculate temporal route
        myTemporalRoute = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(SVC_PEDESTRIAN, mySelectedEdges);
    }
    return 1;
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

    // create PersonPlanCreator Modul
    myPersonPlanCreator = new PersonPlanCreator(this);

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
    // add clicked edge in PersonPlanCreator
    if (objectsUnderCursor.getEdgeFront()) {
        return myPersonPlanCreator->addEdge(objectsUnderCursor.getEdgeFront());
    } else {
        return false;
    }
}


GNEPersonPlanFrame::PersonPlanCreator*
GNEPersonPlanFrame::getPersonPlanCreator() const {
    return myPersonPlanCreator;
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
            myPersonPlanCreator->edgePathCreatorName("person trip");
        } else if (myPersonPlanTagSelector->getCurrentTagProperties().isWalk()) {
            myPersonPlanCreator->edgePathCreatorName("walk");
        } else if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
            myPersonPlanCreator->edgePathCreatorName("ride");
        }
        // show person attributes
        myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties());
        // show edge path creator
        myPersonPlanCreator->showPersonPlanCreator();
        // show help creation
        myHelpCreation->showHelpCreation();
        // show person hierarchy
        myPersonHierarchy->showAttributeCarrierHierarchy(myPersonSelector->getCurrentDemandElement());
    } else {
        // hide moduls if tag selecte isn't valid
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myPersonPlanCreator->hidePersonPlanCreator();
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
                myPersonPlanCreator->edgePathCreatorName("person trip");
            } else if (myPersonPlanTagSelector->getCurrentTagProperties().isWalk()) {
                myPersonPlanCreator->edgePathCreatorName("walk");
            } else if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                myPersonPlanCreator->edgePathCreatorName("ride");
            }
            // show person plan attributes
            myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties());
            // show edge path creator
            myPersonPlanCreator->showPersonPlanCreator();
            // show help creation
            myHelpCreation->showHelpCreation();
            // Show the person's children
            myPersonHierarchy->showAttributeCarrierHierarchy(myPersonSelector->getCurrentDemandElement());
        } else {
            myPersonPlanAttributes->hideAttributesCreatorModul();
            myPersonPlanCreator->hidePersonPlanCreator();
            myHelpCreation->hideHelpCreation();
            myPersonHierarchy->hideAttributeCarrierHierarchy();
        }
    } else {
        // hide moduls if person selected isn't valid
        myPersonPlanTagSelector->hideTagSelector();
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myPersonPlanCreator->hidePersonPlanCreator();
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
                GNERouteHandler::buildPersonTripFromTo(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getSelectedEdges(), types, modes, arrivalPos);
                break;
            }
            case SUMO_TAG_PERSONTRIP_BUSSTOP: {
                GNEAdditional *busStop = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, valuesMap[SUMO_ATTR_BUS_STOP]);
                std::vector<std::string> types = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_VTYPES]);
                std::vector<std::string> modes = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_MODES]);
                GNERouteHandler::buildPersonTripBusStop(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getSelectedEdges(), busStop, types, modes);
                break;
            }
            case SUMO_TAG_WALK_EDGES: {
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildWalkEdges(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getSelectedEdges(), arrivalPos);
                break;
            }
            case SUMO_TAG_WALK_FROMTO: {
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                GNERouteHandler::buildWalkFromTo(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getSelectedEdges(), arrivalPos);
                break;
            }
            case SUMO_TAG_WALK_BUSSTOP: {
                GNEAdditional *busStop = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, valuesMap[SUMO_ATTR_BUS_STOP]);
                GNERouteHandler::buildWalkBusStop(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getSelectedEdges(), busStop);
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
                GNERouteHandler::buildRideFromTo(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getSelectedEdges(), lines, arrivalPos);
                break;
            }
            case SUMO_TAG_RIDE_BUSSTOP: {
                GNEAdditional *busStop = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, valuesMap[SUMO_ATTR_BUS_STOP]);
                std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_LINES]);
                GNERouteHandler::buildRideBusStop(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getSelectedEdges(), busStop, lines);
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
