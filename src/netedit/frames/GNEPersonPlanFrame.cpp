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

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/demandelements/GNEPerson.h>
#include <netedit/demandelements/GNERouteHandler.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEPersonPlanFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEPersonPlanFrame::EdgePathCreator) EdgePathCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VEHICLEFRAME_ABORT,          GNEPersonPlanFrame::EdgePathCreator::onCmdAbortRouteCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VEHICLEFRAME_FINISHCREATION, GNEPersonPlanFrame::EdgePathCreator::onCmdFinishRouteCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VEHICLEFRAME_REMOVELASTEDGE, GNEPersonPlanFrame::EdgePathCreator::onCmdRemoveLastRouteEdge)
};

// Object implementation
FXIMPLEMENT(GNEPersonPlanFrame::EdgePathCreator,  FXGroupBox, EdgePathCreatorMap,    ARRAYNUMBER(EdgePathCreatorMap))

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
    // set text depending of selected vehicle type
    switch (myPersonPlanFrameParent->myTagSelector->getCurrentTagProperties().getTag()) {
        case SUMO_TAG_VEHICLE:
            information
                    << "- Click over a route to\n"
                    << "  create a vehicle.";
            break;
        case SUMO_TAG_TRIP:
            information
                    << "- Select two edges to\n"
                    << "  create a Trip.";
            break;
        case SUMO_TAG_ROUTEFLOW:
            information
                    << "- Click over a route to\n"
                    << "  create a routeFlow.";
            break;
        case SUMO_TAG_FLOW:
            information
                    << "- Select two edges to\n"
                    << "  create a flow.";
            break;
        default:
            break;
    }
    // set information label
    myInformationLabel->setText(information.str().c_str());
}

// ---------------------------------------------------------------------------
// GNEPersonPlanFrame::EdgePathCreator - methods
// ---------------------------------------------------------------------------

GNEPersonPlanFrame::EdgePathCreator::EdgePathCreator(GNEPersonPlanFrame* vehicleFrameParent) :
    FXGroupBox(vehicleFrameParent->myContentFrame, "Route creator", GUIDesignGroupBoxFrame),
    myPersonPlanFrameParent(vehicleFrameParent) {

    // create button for create GEO POIs
    myFinishCreationButton = new FXButton(this, "Finish route creation", nullptr, this, MID_GNE_VEHICLEFRAME_FINISHCREATION, GUIDesignButton);
    myFinishCreationButton->disable();

    // create button for create GEO POIs
    myAbortCreationButton = new FXButton(this, "Abort route creation", nullptr, this, MID_GNE_VEHICLEFRAME_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();

    // create button for create GEO POIs
    myRemoveLastInsertedEdge = new FXButton(this, "Remove last inserted edge", nullptr, this, MID_GNE_VEHICLEFRAME_REMOVELASTEDGE, GUIDesignButton);
    myRemoveLastInsertedEdge->disable();
}


GNEPersonPlanFrame::EdgePathCreator::~EdgePathCreator() {
}


void
GNEPersonPlanFrame::EdgePathCreator::showEdgePathCreator() {
    // disable buttons
    myFinishCreationButton->disable();
    myAbortCreationButton->disable();
    myRemoveLastInsertedEdge->disable();
    show();
}


void
GNEPersonPlanFrame::EdgePathCreator::hideEdgePathCreator() {
    hide();
}


std::vector<GNEEdge*>
GNEPersonPlanFrame::EdgePathCreator::getSelectedEdges() const {
    return mySelectedEdges;
}


void
GNEPersonPlanFrame::EdgePathCreator::addEdge(GNEEdge* edge) {
    if (mySelectedEdges.empty() || ((mySelectedEdges.size() > 0) && (mySelectedEdges.back() != edge))) {
        mySelectedEdges.push_back(edge);
        // enable abort route button
        myAbortCreationButton->enable();
        // disable undo/redo
        myPersonPlanFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->disableUndoRedo("trip creation");
        // set special color
        for (auto i : edge->getLanes()) {
            i->setSpecialColor(&myPersonPlanFrameParent->getEdgeCandidateSelectedColor());
        }
        // calculate route if there is more than two edges
        if (mySelectedEdges.size() > 1) {
            // enable remove last edge button
            myRemoveLastInsertedEdge->enable();
            // enable finish button
            myFinishCreationButton->enable();
            // calculate temporal route
            myTemporalRoute = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(SVC_PASSENGER, mySelectedEdges);
        }
    }
}


void
GNEPersonPlanFrame::EdgePathCreator::clearEdges() {
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
    myPersonPlanFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedo();
}


void
GNEPersonPlanFrame::EdgePathCreator::drawTemporalRoute() const {
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


bool
GNEPersonPlanFrame::EdgePathCreator::isValid(SUMOVehicleClass /* vehicleClass */) const {
    return mySelectedEdges.size() > 0;
}


long
GNEPersonPlanFrame::EdgePathCreator::onCmdAbortRouteCreation(FXObject*, FXSelector, void*) {
    clearEdges();
    // disable buttons
    myFinishCreationButton->disable();
    myAbortCreationButton->disable();
    myRemoveLastInsertedEdge->disable();
    return 1;
}


long
GNEPersonPlanFrame::EdgePathCreator::onCmdFinishRouteCreation(FXObject*, FXSelector, void*) {
    // only create route if there is more than two edges
    if (mySelectedEdges.size() > 1) {
        // obtain tag (only for improve code legibility)
        SumoXMLTag vehicleTag = myPersonPlanFrameParent->myTagSelector->getCurrentTagProperties().getTag();
        // Declare map to keep attributes from Frames from Frame
        std::map<SumoXMLAttr, std::string> valuesMap = myPersonPlanFrameParent->myPersonPlanAttributes->getAttributesAndValues(false);
        // add ID parameter
        valuesMap[SUMO_ATTR_ID] = myPersonPlanFrameParent->myViewNet->getNet()->generateDemandElementID("", vehicleTag);
        // check if we're creating a trip or flow
        if (vehicleTag == SUMO_TAG_TRIP) {
            // Add parameter departure
            if (valuesMap[SUMO_ATTR_DEPART].empty()) {
                valuesMap[SUMO_ATTR_DEPART] = "0";
            }
            // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
            SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, myPersonPlanFrameParent->getPredefinedTagsMML(), toString(vehicleTag));
            // obtain trip parameters
            SUMOVehicleParameter* tripParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMOSAXAttrs);
            // build trip in GNERouteHandler
            GNERouteHandler::buildTrip(myPersonPlanFrameParent->myViewNet, true, *tripParameters, mySelectedEdges);
            // delete tripParameters
            delete tripParameters;
        } else {
            // set begin and end attributes
            if (valuesMap[SUMO_ATTR_BEGIN].empty()) {
                valuesMap[SUMO_ATTR_BEGIN] = "0";
            }
            if (valuesMap[SUMO_ATTR_END].empty()) {
                valuesMap[SUMO_ATTR_END] = "3600";
            }
            // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
            SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, myPersonPlanFrameParent->getPredefinedTagsMML(), toString(vehicleTag));
            // obtain flow parameters
            SUMOVehicleParameter* flowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMOSAXAttrs, 0, SUMOTime_MAX);
            // build flow in GNERouteHandler
            GNERouteHandler::buildFlow(myPersonPlanFrameParent->myViewNet, true, *flowParameters, mySelectedEdges);
            // delete flowParameters
            delete flowParameters;
        }
        // clear edges
        clearEdges();
        // disable buttons
        myFinishCreationButton->disable();
        myAbortCreationButton->disable();
        myRemoveLastInsertedEdge->disable();
    }
    return 1;
}


long
GNEPersonPlanFrame::EdgePathCreator::onCmdRemoveLastRouteEdge(FXObject*, FXSelector, void*) {
    if (mySelectedEdges.size() > 1) {
        // remove last edge
        mySelectedEdges.pop_back();
        // calculate temporal route
        myTemporalRoute = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(SVC_PASSENGER, mySelectedEdges);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEPersonPlanFrame - methods
// ---------------------------------------------------------------------------

GNEPersonPlanFrame::GNEPersonPlanFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "PersonPlans") {

    // Create tag selector for person plan
    myTagSelector = new TagSelector(this, GNEAttributeCarrier::TagType::TAGTYPE_PERSONPLAN);

    // Create person parameters
    myPersonPlanAttributes = new AttributesCreator(this);

    // create EdgePathCreator Modul
    myEdgePathCreator = new EdgePathCreator(this);

    // Create Help Creation Modul
    myHelpCreation = new HelpCreation(this);

    // set PersonPlan as default vehicle
    myTagSelector->setCurrentTypeTag(SUMO_TAG_VEHICLE);
}


GNEPersonPlanFrame::~GNEPersonPlanFrame() {}


void
GNEPersonPlanFrame::show() {
    // refresh item selector
    myTagSelector->refreshTagProperties();
    // show frame
    GNEFrame::show();
}


bool
GNEPersonPlanFrame::addPersonPlan(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // obtain tag (only for improve code legibility)
    SumoXMLTag vehicleTag = myTagSelector->getCurrentTagProperties().getTag();
    // first check that current selected vehicle is valid
    if (vehicleTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected vehicle isn't valid.");
        return false;
    }
    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap = myPersonPlanAttributes->getAttributesAndValues(false);
    // add ID parameter
    valuesMap[SUMO_ATTR_ID] = myViewNet->getNet()->generateDemandElementID("", vehicleTag);
    // set route or edges depending of vehicle type
    if ((vehicleTag == SUMO_TAG_VEHICLE) || (vehicleTag == SUMO_TAG_ROUTEFLOW)) {
        if (objectsUnderCursor.getDemandElementFront() && (objectsUnderCursor.getDemandElementFront()->getTagProperty().isRoute())) {
            // obtain route
            valuesMap[SUMO_ATTR_ROUTE] = (objectsUnderCursor.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE)? objectsUnderCursor.getDemandElementFront()->getID() : "embedded";
            // check if we're creating a vehicle or a flow
            if (vehicleTag == SUMO_TAG_VEHICLE) {
                // Add parameter departure
                if (valuesMap[SUMO_ATTR_DEPART].empty()) {
                    valuesMap[SUMO_ATTR_DEPART] = "0";
                }
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(vehicleTag));
                // obtain vehicle parameters in vehicleParameters
                SUMOVehicleParameter* vehicleParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMOSAXAttrs);
                // check if we're creating a vehicle over a existent route or over a embedded route
                if (objectsUnderCursor.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE) {
                    GNERouteHandler::buildVehicleOverRoute(myViewNet, true, *vehicleParameters);
                } else {
                    GNERouteHandler::buildVehicleWithEmbeddedRoute(myViewNet, true, *vehicleParameters, objectsUnderCursor.getDemandElementFront());
                }
                // delete vehicleParameters
                delete vehicleParameters;
            } else {
                // set begin and end attributes
                if (valuesMap[SUMO_ATTR_BEGIN].empty()) {
                    valuesMap[SUMO_ATTR_BEGIN] = "0";
                }
                if (valuesMap[SUMO_ATTR_END].empty()) {
                    valuesMap[SUMO_ATTR_END] = "3600";
                }
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(vehicleTag));
                // obtain routeFlow parameters in routeFlowParameters
                SUMOVehicleParameter* routeFlowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMOSAXAttrs, 0, SUMOTime_MAX);
                // check if we're creating a vehicle over a existent route or over a embedded route
                if (objectsUnderCursor.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE) {
                    GNERouteHandler::buildFlowOverRoute(myViewNet, true, *routeFlowParameters);
                } else {
                    GNERouteHandler::buildFlowWithEmbeddedRoute(myViewNet, true, *routeFlowParameters, objectsUnderCursor.getDemandElementFront());
                }
                // delete routeFlowParameters
                delete routeFlowParameters;
            }
            // all ok, then return true;
            return true;
        } else {
            myViewNet->setStatusBarText(toString(vehicleTag) + " has to be placed within a route.");
            return false;
        }
    } else if (((vehicleTag == SUMO_TAG_TRIP) || (vehicleTag == SUMO_TAG_FLOW)) && objectsUnderCursor.getEdgeFront()) {
        // add clicked edge in EdgePathCreator
        myEdgePathCreator->addEdge(objectsUnderCursor.getEdgeFront());
    }
    // nothing crated
    return false;
}


GNEPersonPlanFrame::EdgePathCreator*
GNEPersonPlanFrame::getEdgePathCreator() const {
    return myEdgePathCreator;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEPersonPlanFrame::enableModuls(const GNEAttributeCarrier::TagProperties& tagProperties) {
    // show AutoRute creator if we're editing a trip
    if ((myTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_TRIP) || (myTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_FLOW)) {
        myEdgePathCreator->showEdgePathCreator();
    } else {
        myEdgePathCreator->hideEdgePathCreator();
    }
}


void
GNEPersonPlanFrame::disableModuls() {
    // hide all moduls if vehicle isn't valid
    myPersonPlanAttributes->hideAttributesCreatorModul();
    myHelpCreation->hideHelpCreation();
}

/****************************************************************************/
