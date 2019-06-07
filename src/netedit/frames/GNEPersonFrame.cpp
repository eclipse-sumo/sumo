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

    // Create item Selector modul for persons
    myPersonTagSelector = new TagSelector(this, GNEAttributeCarrier::TagType::TAGTYPE_PERSON);

    // Create vehicle parameters
    myPersonAttributes = new AttributesCreator(this);

    // create EdgePathCreator Modul
    myEdgePathCreator = new EdgePathCreator(this);

    // Create Help Creation Modul
    myHelpCreation = new HelpCreation(this);

    // set Person as default vehicle
    myPersonTagSelector->setCurrentTypeTag(SUMO_TAG_PERSON);
}


GNEPersonFrame::~GNEPersonFrame() {}


void
GNEPersonFrame::show() {
    // refresh item selector
    myPersonTagSelector->refreshTagProperties();
    // show frame
    GNEFrame::show();
}


bool
GNEPersonFrame::addPerson(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // obtain tag (only for improve code legibility)
    SumoXMLTag vehicleTag = myPersonTagSelector->getCurrentTagProperties().getTag();
    // first check that current selected vehicle is valid
    if (vehicleTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected vehicle isn't valid.");
        return false;
    }
    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap = myPersonAttributes->getAttributesAndValues(false);
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


GNEPersonFrame::EdgePathCreator*
GNEPersonFrame::getEdgePathCreator() const {
    return myEdgePathCreator;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEPersonFrame::enableModuls(const GNEAttributeCarrier::TagProperties& tagProperties) {
    // show AutoRute creator if we're editing a trip
    if ((myPersonTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_TRIP) || (myPersonTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_FLOW)) {
        myEdgePathCreator->showEdgePathCreator();
    } else {
        myEdgePathCreator->hideEdgePathCreator();
    }
}


void
GNEPersonFrame::disableModuls() {
    // hide all moduls if vehicle isn't valid
    myPersonAttributes->hideAttributesCreatorModul();
    myHelpCreation->hideHelpCreation();
}


void
GNEPersonFrame::finishEdgePathCreation() {

}

/****************************************************************************/
