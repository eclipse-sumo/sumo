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
#include <netedit/GNEUndoList.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/router/DijkstraRouter.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBVehicle.h>

#include "GNEVehicleFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEVehicleFrame::VTypeSelector) VTypeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNEVehicleFrame::VTypeSelector::onCmdSelectVType),
};

// Object implementation
FXIMPLEMENT(GNEVehicleFrame::VTypeSelector, FXGroupBox, VTypeSelectorMap,   ARRAYNUMBER(VTypeSelectorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEVehicleFrame::VTypeSelector - methods
// ---------------------------------------------------------------------------

GNEVehicleFrame::VTypeSelector::VTypeSelector(GNEVehicleFrame* vehicleFrameParent) :
    FXGroupBox(vehicleFrameParent->myContentFrame, "Vehicle Type", GUIDesignGroupBoxFrame),
    myVehicleFrameParent(vehicleFrameParent),
    myCurrentVehicleType(nullptr) {
    // Create FXComboBox
    myTypeMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // refresh TypeMatchBox
    refreshVTypeSelector();
    // VTypeSelector is always shown
    show();
}


GNEVehicleFrame::VTypeSelector::~VTypeSelector() {}


const GNEDemandElement*
GNEVehicleFrame::VTypeSelector::getCurrentVehicleType() const {
    return myCurrentVehicleType;
}


void 
GNEVehicleFrame::VTypeSelector::showVTypeSelector(const GNEAttributeCarrier::TagProperties& tagProperties) {
    // if current selected item isn't valid, set DEFAULT_VEHTYPE
    if (myCurrentVehicleType) {
        // show vehicle attributes modul
        myVehicleFrameParent->myVehicleAttributes->showAttributesCreatorModul(tagProperties);
        // show help creation
        myVehicleFrameParent->myHelpCreation->showHelpCreation();
    } else {
        // set DEFAULT_VTYPE as current VType
        myTypeMatchBox->setText(DEFAULT_VTYPE_ID.c_str());
        // call manually onCmdSelectVType to update comboBox
        onCmdSelectVType(nullptr, 0, nullptr);
    }
    // show VType selector
    show();
}


void 
GNEVehicleFrame::VTypeSelector::hideVTypeSelector() {
    hide();
}


void 
GNEVehicleFrame::VTypeSelector::refreshVTypeSelector() {
    // clear comboBox
    myTypeMatchBox->clearItems();
    // get list of VTypes
    const auto &vTypes = myVehicleFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE);
    // fill myTypeMatchBox with list of tags
    for (const auto& i : vTypes) {
        myTypeMatchBox->appendItem(i.first.c_str());
    }
    // Set visible items
    myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
}


long
GNEVehicleFrame::VTypeSelector::onCmdSelectVType(FXObject*, FXSelector, void*) {
    // get list of VTypes
    const auto &vTypes = myVehicleFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE);
    // Check if value of myTypeMatchBox correspond to a VType
    for (const auto& i : vTypes) {
        if (i.first == myTypeMatchBox->getText().text()) {
            // set color of myTypeMatchBox to black (valid)
            myTypeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current VType
            myCurrentVehicleType = i.second;
            // show vehicle attributes modul
            myVehicleFrameParent->myVehicleAttributes->showAttributesCreatorModul(myVehicleFrameParent->myItemSelector->getCurrentTagProperties());
            // show help creation
            myVehicleFrameParent->myHelpCreation->showHelpCreation();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeMatchBox->getText() + "' in VTypeSelector").text());
            return 1;
        }
    }
    // if VType selecte is invalid, select
    myCurrentVehicleType = nullptr;
    // hide all moduls if selected item isn't valid
    myVehicleFrameParent->myVehicleAttributes->hideAttributesCreatorModul();
    // hide help creation
    myVehicleFrameParent->myHelpCreation->hideHelpCreation();
    // set color of myTypeMatchBox to red (invalid)
    myTypeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in VTypeSelector");
    return 1;
}

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
    switch (myVehicleFrameParent->myItemSelector->getCurrentTagProperties().getTag()) {
        case SUMO_TAG_VEHICLE:
            information
                << "- Click over a route to\n"
                << "  create a vehicle.";
            break;
        case SUMO_TAG_FLOW:
            information
                << "- Click over a route to\n"
                << "  create a flow.";
            break;
        case SUMO_TAG_TRIP:
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
// GNEVehicleFrame::AutoRoute - methods
// ---------------------------------------------------------------------------

GNEVehicleFrame::AutoRoute::AutoRoute(GNEVehicleFrame* vehicleFrameParent) :
    myVehicleFrameParent(vehicleFrameParent),
    myFrom(nullptr),
    myTo(nullptr) {
    myRouter = new DijkstraRouter<NBEdge, NBVehicle, SUMOAbstractRouter<NBEdge, NBVehicle> >(
        myVehicleFrameParent->myViewNet->getNet()->getNetBuilder()->getEdgeCont().getAllEdges(), 
        true, &NBEdge::getTravelTimeStatic, nullptr, true);
}


GNEVehicleFrame::AutoRoute::~AutoRoute() {
    delete myRouter;
}


GNEEdge* 
GNEVehicleFrame::AutoRoute::getFrom() const {
    return myFrom;
}
        

GNEEdge* 
GNEVehicleFrame::AutoRoute::getTo() const {
    return myTo;
}


void 
GNEVehicleFrame::AutoRoute::setFrom(GNEEdge* from) {
    myFrom = from;
    // set special color
    for (auto i : myFrom->getLanes()) {
        i->setSpecialColor(&myVehicleFrameParent->getEdgeCandidateSelectedColor());
    }
}
        

void 
GNEVehicleFrame::AutoRoute::setTo(GNEEdge* to) {
    myTo = to;
    // set special color
    for (auto i : myTo->getLanes()) {
        i->setSpecialColor(&myVehicleFrameParent->getEdgeCandidateSelectedColor());
    }
}


void 
GNEVehicleFrame::AutoRoute::clearEdges() {
    // restore colors
    for (auto i : myFrom->getLanes()) {
        i->setSpecialColor(nullptr);
    }
    for (auto i : myTo->getLanes()) {
        i->setSpecialColor(nullptr);
    }
    // reset pointers
    myFrom = nullptr;
    myTo = nullptr;
}


void 
GNEVehicleFrame::AutoRoute::updateAbstractRouterEdges() {
    // simply delete and create myRouter again
    delete myRouter;
    myRouter = new DijkstraRouter<NBEdge, NBVehicle, SUMOAbstractRouter<NBEdge, NBVehicle> >(
        myVehicleFrameParent->myViewNet->getNet()->getNetBuilder()->getEdgeCont().getAllEdges(), 
        true, &NBEdge::getTravelTimeStatic, nullptr, true);
}


void 
GNEVehicleFrame::AutoRoute::drawRoute() {
    if (myFrom) {

    }
}


bool 
GNEVehicleFrame::AutoRoute::isValid(SUMOVehicleClass vehicleClass) const {
    if (myFrom && myTo) {
        std::vector<const NBEdge*> into;
        NBVehicle tmpVehicle("temporalNBVehicle", vehicleClass);
        return myRouter->compute(myFrom->getNBEdge(), myTo->getNBEdge(), &tmpVehicle, 10, into);
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEVehicleFrame - methods
// ---------------------------------------------------------------------------

GNEVehicleFrame::GNEVehicleFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Vehicles"),
    myAutoRoute(this) {

    // Create item Selector modul for vehicles
    myItemSelector = new ItemSelector(this, GNEAttributeCarrier::TagType::TAGTYPE_VEHICLE);

    // Create vehicle type selector
    myVTypeSelector = new VTypeSelector(this);

    // Create vehicle parameters
    myVehicleAttributes = new AttributesCreator(this);

    // Create Help Creation Modul
    myHelpCreation = new HelpCreation(this);

    // set Vehicle as default vehicle
    myItemSelector->setCurrentTypeTag(SUMO_TAG_VEHICLE);
}


GNEVehicleFrame::~GNEVehicleFrame() {}


void
GNEVehicleFrame::show() {
    // refresh item selector
    myItemSelector->refreshTagProperties();
    // show frame
    GNEFrame::show();
}


bool
GNEVehicleFrame::addVehicle(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // obtain tag (only for improve code legibility)
    SumoXMLTag vehicleTag = myItemSelector->getCurrentTagProperties().getTag();

    // first check that current selected vehicle is valid
    if (vehicleTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected vehicle isn't valid.");
        return false;
    }

    // now check if VType is valid
    if (myVTypeSelector->getCurrentVehicleType() == nullptr) {
        myViewNet->setStatusBarText("Current selected vehicle type isn't valid.");
        return false;
    }

    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap = myVehicleAttributes->getAttributesAndValues(false);

    // add ID
    valuesMap[SUMO_ATTR_ID] = myViewNet->getNet()->generateDemandElementID(vehicleTag);

    // add VType
    valuesMap[SUMO_ATTR_TYPE] = myVTypeSelector->getCurrentVehicleType()->getID();

    // set route or edges depending of vehicle type
    if ((vehicleTag == SUMO_TAG_VEHICLE || vehicleTag == SUMO_TAG_FLOW)) {
        if (objectsUnderCursor.getDemandElementFront() && 
           (objectsUnderCursor.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE)) {
            // obtain route
            valuesMap[SUMO_ATTR_ROUTE] = objectsUnderCursor.getDemandElementFront()->getID();
            if(vehicleTag == SUMO_TAG_VEHICLE) {
                // Add parameter departure
                if(valuesMap.count(SUMO_ATTR_DEPART) == 0) {
                    valuesMap[SUMO_ATTR_DEPART] = "0";
                }
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(vehicleTag));
                // obtain vehicle parameters in vehicleParameters
                SUMOVehicleParameter* vehicleParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMOSAXAttrs);
                // create it in RouteFrame
                GNERouteHandler::buildVehicle(myViewNet, true, vehicleParameters);
                // delete vehicleParameters
                delete vehicleParameters;
            } else {
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(vehicleTag));
                // obtain flow parameters in flowParameters
                SUMOVehicleParameter* flowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMOSAXAttrs, 0, SUMOTime_MAX);
                // create it in RouteFrame
                GNERouteHandler::buildFlow(myViewNet, true, flowParameters);
                // delete vehicleParameters
                delete flowParameters;
            }
            // all ok, then return true;
            return true;
        } else {
            myViewNet->setStatusBarText(toString(vehicleTag) + " has to be placed within a route.");
            return false;
        }
    } else if (vehicleTag == SUMO_TAG_TRIP) {
        // set first edge
        if (myAutoRoute.getFrom() == nullptr) {
            // set from edge
            if (objectsUnderCursor.getEdgeFront()) {
                myAutoRoute.setFrom(objectsUnderCursor.getEdgeFront());
                // update showHelpCreation
                myHelpCreation->updateHelpCreation();
                return true;
            } else {
                myViewNet->setStatusBarText(toString(vehicleTag) + " has to be placed in two edges.");
                return false;
            }
        }
        // set second edge
        if (myAutoRoute.getTo() == nullptr) {
            if (objectsUnderCursor.getEdgeFront()) {
                // set to edge
                myAutoRoute.setTo(objectsUnderCursor.getEdgeFront());
                // check if route is valid
                myAutoRoute.isValid(GNEAttributeCarrier::parse<SUMOVehicleClass>(myVTypeSelector->getCurrentVehicleType()->getAttribute(SUMO_ATTR_VCLASS)));
                // update showHelpCreation
                myHelpCreation->updateHelpCreation();
                // Add parameter departure
                if(valuesMap.count(SUMO_ATTR_DEPART) == 0) {
                    valuesMap[SUMO_ATTR_DEPART] = "0";
                }
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(vehicleTag));
                // obtain trip parameters in tripParameters
                SUMOVehicleParameter* tripParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMOSAXAttrs);
                // create it in RouteFrame
                GNERouteHandler::buildTrip(myViewNet, true, tripParameters, myAutoRoute.getFrom(), myAutoRoute.getTo());
                // delete tripParameters
                delete tripParameters;
                // clear edges
                myAutoRoute.clearEdges();
                // all ok, then return true;
                return true;
            } else {
                myViewNet->setStatusBarText(toString(vehicleTag) + " has to be placed in two edges.");
                return false;
            }
        }
    }
    // nothing crated
    return false;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEVehicleFrame::enableModuls(const GNEAttributeCarrier::TagProperties& tagProperties) {
    // show vehicle type selector modul
    myVTypeSelector->showVTypeSelector(tagProperties);
}


void
GNEVehicleFrame::disableModuls() {
    // hide all moduls if vehicle isn't valid
    myVTypeSelector->hideVTypeSelector();
    myVehicleAttributes->hideAttributesCreatorModul();
    myHelpCreation->hideHelpCreation();
}

/****************************************************************************/
