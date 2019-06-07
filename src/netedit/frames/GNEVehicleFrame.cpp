/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVehicleFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2018
/// @version $Id$
///
// The Widget for add Vehicles/Flows/Trips/etc. elements
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
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEUndoList.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBVehicle.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEVehicleFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEVehicleFrame::HelpCreation - methods
// ---------------------------------------------------------------------------

GNEVehicleFrame::HelpCreation::HelpCreation(GNEVehicleFrame* vehicleFrameParent) :
    FXGroupBox(vehicleFrameParent->myContentFrame, "Help", GUIDesignGroupBoxFrame),
    myVehicleFrameParent(vehicleFrameParent) {
    myInformationLabel = new FXLabel(this, "", 0, GUIDesignLabelFrameInformation);
}


GNEVehicleFrame::HelpCreation::~HelpCreation() {}


void
GNEVehicleFrame::HelpCreation::showHelpCreation() {
    // first update help cration
    updateHelpCreation();
    // show modul
    show();
}


void
GNEVehicleFrame::HelpCreation::hideHelpCreation() {
    hide();
}

void
GNEVehicleFrame::HelpCreation::updateHelpCreation() {
    // create information label
    std::ostringstream information;
    // set text depending of selected vehicle type
    switch (myVehicleFrameParent->myVehicleTagSelector->getCurrentTagProperties().getTag()) {
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
// GNEVehicleFrame - methods
// ---------------------------------------------------------------------------

GNEVehicleFrame::GNEVehicleFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Vehicles") {

    // Create item Selector modul for vehicles
    myVehicleTagSelector = new TagSelector(this, GNEAttributeCarrier::TagType::TAGTYPE_VEHICLE);

    // Create vehicle type selector
    myVTypeSelector = new VTypeSelector(this);

    // Create vehicle parameters
    myVehicleAttributes = new AttributesCreator(this);

    // create EdgePathCreator Modul
    myEdgePathCreator = new EdgePathCreator(this);

    // Create Help Creation Modul
    myHelpCreation = new HelpCreation(this);

    // set Vehicle as default vehicle
    myVehicleTagSelector->setCurrentTypeTag(SUMO_TAG_VEHICLE);
}


GNEVehicleFrame::~GNEVehicleFrame() {}


void
GNEVehicleFrame::show() {
    // refresh item selector
    myVehicleTagSelector->refreshTagProperties();
    // show frame
    GNEFrame::show();
}


bool
GNEVehicleFrame::addVehicle(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // obtain tag (only for improve code legibility)
    SumoXMLTag vehicleTag = myVehicleTagSelector->getCurrentTagProperties().getTag();
    // first check that current selected vehicle is valid
    if (vehicleTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected vehicle isn't valid.");
        return false;
    }
    // now check if VType is valid
    if (myVTypeSelector->getCurrentVType() == nullptr) {
        myViewNet->setStatusBarText("Current selected vehicle type isn't valid.");
        return false;
    }
    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap = myVehicleAttributes->getAttributesAndValues(false);
    // add ID parameter
    valuesMap[SUMO_ATTR_ID] = myViewNet->getNet()->generateDemandElementID("", vehicleTag);
    // add VType
    valuesMap[SUMO_ATTR_TYPE] = myVTypeSelector->getCurrentVType()->getID();
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


GNEVehicleFrame::EdgePathCreator*
GNEVehicleFrame::getEdgePathCreator() const {
    return myEdgePathCreator;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEVehicleFrame::enableModuls(const GNEAttributeCarrier::TagProperties& tagProperties) {
    // show vehicle type selector modul
    myVTypeSelector->showVTypeSelector(tagProperties);
    // show AutoRute creator if we're editing a trip
    if ((myVehicleTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_TRIP) || (myVehicleTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_FLOW)) {
        myEdgePathCreator->showEdgePathCreator();
    } else {
        myEdgePathCreator->hideEdgePathCreator();
    }
}


void
GNEVehicleFrame::disableModuls() {
    // hide all moduls if vehicle isn't valid
    myVTypeSelector->hideVTypeSelector();
    myVehicleAttributes->hideAttributesCreatorModul();
    myHelpCreation->hideHelpCreation();
}


void 
GNEVehicleFrame::selectedVType(bool validVType) {
    if (validVType) {
        // show vehicle attributes modul
        myVehicleAttributes->showAttributesCreatorModul(myVehicleTagSelector->getCurrentTagProperties());
        // set current VTypeClass in TripCreator
        myEdgePathCreator->setVClass(myVTypeSelector->getCurrentVType()->getVClass());
        // show help creation
        myHelpCreation->showHelpCreation();
    } else {
        // hide all moduls if selected item isn't valid
        myVehicleAttributes->hideAttributesCreatorModul();
        // hide help creation
        myHelpCreation->hideHelpCreation();

    }
}


void
GNEVehicleFrame::finishEdgePathCreation() {
    // obtain tag (only for improve code legibility)
    SumoXMLTag vehicleTag = myVehicleTagSelector->getCurrentTagProperties().getTag();
    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap = myVehicleAttributes->getAttributesAndValues(false);
    // add ID parameter
    valuesMap[SUMO_ATTR_ID] = myViewNet->getNet()->generateDemandElementID("", vehicleTag);
    // check if we're creating a trip or flow
    if (vehicleTag == SUMO_TAG_TRIP) {
        // Add parameter departure
        if (valuesMap[SUMO_ATTR_DEPART].empty()) {
            valuesMap[SUMO_ATTR_DEPART] = "0";
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(vehicleTag));
        // obtain trip parameters
        SUMOVehicleParameter* tripParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMOSAXAttrs);
        // build trip in GNERouteHandler
        GNERouteHandler::buildTrip(myViewNet, true, *tripParameters, myEdgePathCreator->getSelectedEdges());
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
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(vehicleTag));
        // obtain flow parameters
        SUMOVehicleParameter* flowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMOSAXAttrs, 0, SUMOTime_MAX);
        // build flow in GNERouteHandler
        GNERouteHandler::buildFlow(myViewNet, true, *flowParameters, myEdgePathCreator->getSelectedEdges());
        // delete flowParameters
        delete flowParameters;
    }
}
/****************************************************************************/
