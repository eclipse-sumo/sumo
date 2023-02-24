/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEVehicle.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
///
// Representation of vehicles in netedit
/****************************************************************************/
#include <cmath>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_EnableAttribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/frames/demand/GNETypeFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIBaseVehicleHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIGlobalPostDrawing.h>

#include "GNEVehicle.h"
#include "GNERouteHandler.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEVehicle::GNESingleVehiclePopupMenu) GNESingleVehiclePopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLE_TRANSFORM,   GNEVehicle::GNESingleVehiclePopupMenu::onCmdTransform),
};

FXDEFMAP(GNEVehicle::GNESelectedVehiclesPopupMenu) GNESelectedVehiclesPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLE_TRANSFORM,   GNEVehicle::GNESelectedVehiclesPopupMenu::onCmdTransform),
};

// Object implementation
FXIMPLEMENT(GNEVehicle::GNESingleVehiclePopupMenu,      GUIGLObjectPopupMenu,   GNESingleVehiclePopupMenuMap,       ARRAYNUMBER(GNESingleVehiclePopupMenuMap))
FXIMPLEMENT(GNEVehicle::GNESelectedVehiclesPopupMenu,   GUIGLObjectPopupMenu,   GNESelectedVehiclesPopupMenuMap,    ARRAYNUMBER(GNESelectedVehiclesPopupMenuMap))

// ===========================================================================
// static definitions
// ===========================================================================
const double GNEVehicle::myArrivalPositionDiameter = SUMO_const_halfLaneWidth * 0.5;

// ===========================================================================
// GNEVehicle::GNESingleVehiclePopupMenu
// ===========================================================================

GNEVehicle::GNESingleVehiclePopupMenu::GNESingleVehiclePopupMenu(GNEVehicle* vehicle, GUIMainWindow& app, GUISUMOAbstractView& parent) :
    GUIGLObjectPopupMenu(app, parent, *vehicle),
    myVehicle(vehicle),
    myTransformToVehicle(nullptr),
    myTransformToVehicleWithEmbeddedRoute(nullptr),
    myTransformToRouteFlow(nullptr),
    myTransformToRouteFlowWithEmbeddedRoute(nullptr),
    myTransformToTrip(nullptr),
    myTransformToFlow(nullptr) {
    // build header
    myVehicle->buildPopupHeader(this, app);
    // build menu command for center button and copy cursor position to clipboard
    myVehicle->buildCenterPopupEntry(this);
    myVehicle->buildPositionCopyEntry(this, app);
    // build menu commands for names
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + myVehicle->getTagStr() + " name to clipboard").c_str(), nullptr, this, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + myVehicle->getTagStr() + " typed name to clipboard").c_str(), nullptr, this, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(this);
    // build selection and show parameters menu
    myVehicle->getNet()->getViewNet()->buildSelectionACPopupEntry(this, myVehicle);
    myVehicle->buildShowParamsPopupEntry(this);
    // route length
    vehicle->buildMenuCommandRouteLength(this);
    // add transform functions only in demand mode
    if (myVehicle->getNet()->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        // Get icons
        FXIcon* vehicleIcon = GUIIconSubSys::getIcon(GUIIcon::VEHICLE);
        FXIcon* tripIcon = GUIIconSubSys::getIcon(GUIIcon::TRIP);
        FXIcon* routeFlowIcon = GUIIconSubSys::getIcon(GUIIcon::ROUTEFLOW);
        FXIcon* flowIcon = GUIIconSubSys::getIcon(GUIIcon::FLOW);
        // create menu pane for transform operations
        FXMenuPane* transformOperation = new FXMenuPane(this);
        this->insertMenuPaneChild(transformOperation);
        new FXMenuCascade(this, "transform to", nullptr, transformOperation);
        // Create menu commands for all transform
        myTransformToVehicle = GUIDesigns::buildFXMenuCommand(transformOperation, "Vehicle", vehicleIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToVehicleWithEmbeddedRoute = GUIDesigns::buildFXMenuCommand(transformOperation, "Vehicle (embedded route)", vehicleIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToRouteFlow = GUIDesigns::buildFXMenuCommand(transformOperation, "RouteFlow", routeFlowIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToRouteFlowWithEmbeddedRoute = GUIDesigns::buildFXMenuCommand(transformOperation, "RouteFlow (embedded route)", routeFlowIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToTrip = GUIDesigns::buildFXMenuCommand(transformOperation, "Trip", tripIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToFlow = GUIDesigns::buildFXMenuCommand(transformOperation, "Flow", flowIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        // check what menu command has to be disabled
        if (myVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) {
            myTransformToVehicle->disable();
        } else if (myVehicle->getTagProperty().getTag() == GNE_TAG_VEHICLE_WITHROUTE) {
            myTransformToVehicleWithEmbeddedRoute->disable();
        } else if (myVehicle->getTagProperty().getTag() == GNE_TAG_FLOW_ROUTE) {
            myTransformToRouteFlow->disable();
        } else if (myVehicle->getTagProperty().getTag() == GNE_TAG_FLOW_WITHROUTE) {
            myTransformToRouteFlowWithEmbeddedRoute->disable();
        } else if (myVehicle->getTagProperty().getTag() == SUMO_TAG_TRIP) {
            myTransformToTrip->disable();
        } else if (myVehicle->getTagProperty().getTag() == SUMO_TAG_FLOW) {
            myTransformToFlow->disable();
        }
    }
}


GNEVehicle::GNESingleVehiclePopupMenu::~GNESingleVehiclePopupMenu() {}


long
GNEVehicle::GNESingleVehiclePopupMenu::onCmdTransform(FXObject* obj, FXSelector, void*) {
    if (obj == myTransformToVehicle) {
        GNERouteHandler::transformToVehicle(myVehicle, false);
    } else if (obj == myTransformToVehicleWithEmbeddedRoute) {
        GNERouteHandler::transformToVehicle(myVehicle, true);
    } else if (obj == myTransformToRouteFlow) {
        GNERouteHandler::transformToRouteFlow(myVehicle, false);
    } else if (obj == myTransformToRouteFlowWithEmbeddedRoute) {
        GNERouteHandler::transformToRouteFlow(myVehicle, true);
    } else if (obj == myTransformToTrip) {
        GNERouteHandler::transformToTrip(myVehicle);
    } else if (obj == myTransformToFlow) {
        GNERouteHandler::transformToFlow(myVehicle);
    }
    return 1;
}

// ===========================================================================
// GNEVehicle::GNESelectedVehiclesPopupMenu
// ===========================================================================

GNEVehicle::GNESelectedVehiclesPopupMenu::GNESelectedVehiclesPopupMenu(GNEVehicle* vehicle, const std::vector<GNEVehicle*>& selectedVehicle, GUIMainWindow& app, GUISUMOAbstractView& parent) :
    GUIGLObjectPopupMenu(app, parent, *vehicle),
    mySelectedVehicles(selectedVehicle),
    myVehicleTag(vehicle->getTagProperty().getTag()),
    myTransformToVehicle(nullptr),
    myTransformToVehicleWithEmbeddedRoute(nullptr),
    myTransformToRouteFlow(nullptr),
    myTransformToRouteFlowWithEmbeddedRoute(nullptr),
    myTransformToTrip(nullptr),
    myTransformToFlow(nullptr),
    myTransformAllVehiclesToVehicle(nullptr),
    myTransformAllVehiclesToVehicleWithEmbeddedRoute(nullptr),
    myTransformAllVehiclesToRouteFlow(nullptr),
    myTransformAllVehiclesToRouteFlowWithEmbeddedRoute(nullptr),
    myTransformAllVehiclesToTrip(nullptr),
    myTransformAllVehiclesToFlow(nullptr) {
    // build header
    vehicle->buildPopupHeader(this, app);
    // build menu command for center button and copy cursor position to clipboard
    vehicle->buildCenterPopupEntry(this);
    vehicle->buildPositionCopyEntry(this, app);
    // build menu commands for names
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + vehicle->getTagStr() + " name to clipboard").c_str(), nullptr, this, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + vehicle->getTagStr() + " typed name to clipboard").c_str(), nullptr, this, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(this);
    // build selection and show parameters menu
    vehicle->getNet()->getViewNet()->buildSelectionACPopupEntry(this, vehicle);
    vehicle->buildShowParamsPopupEntry(this);
    // route length
    vehicle->buildMenuCommandRouteLength(this);
    // add transform functions only in demand mode
    if (vehicle->getNet()->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        // Get icons
        FXIcon* vehicleIcon = GUIIconSubSys::getIcon(GUIIcon::VEHICLE);
        FXIcon* tripIcon = GUIIconSubSys::getIcon(GUIIcon::TRIP);
        FXIcon* routeFlowIcon = GUIIconSubSys::getIcon(GUIIcon::ROUTEFLOW);
        FXIcon* flowIcon = GUIIconSubSys::getIcon(GUIIcon::FLOW);
        // create menu pane for transform operations
        FXMenuPane* transformOperation = new FXMenuPane(this);
        this->insertMenuPaneChild(transformOperation);
        new FXMenuCascade(this, "transform to", nullptr, transformOperation);
        // Create menu commands for all transform
        myTransformToVehicle = GUIDesigns::buildFXMenuCommand(transformOperation,
                               "Vehicles (Only " + vehicle->getTagStr() + ")", vehicleIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToVehicleWithEmbeddedRoute = GUIDesigns::buildFXMenuCommand(transformOperation,
                                                "Vehicles (embedded route, only " + vehicle->getTagStr() + ")", vehicleIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToRouteFlow = GUIDesigns::buildFXMenuCommand(transformOperation,
                                 "RouteFlows (Only " + vehicle->getTagStr() + ")", routeFlowIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToRouteFlowWithEmbeddedRoute = GUIDesigns::buildFXMenuCommand(transformOperation,
                "RouteFlows (embedded route, only " + vehicle->getTagStr() + ")", routeFlowIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToTrip = GUIDesigns::buildFXMenuCommand(transformOperation,
                            "Trips (Only " + vehicle->getTagStr() + ")", tripIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToFlow = GUIDesigns::buildFXMenuCommand(transformOperation,
                            "Flows (Only " + vehicle->getTagStr() + ")", flowIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        // create separator
        new FXMenuSeparator(transformOperation);
        // Create menu commands for all transform all vehicles
        myTransformAllVehiclesToVehicle = GUIDesigns::buildFXMenuCommand(transformOperation, "Vehicles", vehicleIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformAllVehiclesToVehicleWithEmbeddedRoute = GUIDesigns::buildFXMenuCommand(transformOperation, "Vehicles (embedded route)", vehicleIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformAllVehiclesToRouteFlow = GUIDesigns::buildFXMenuCommand(transformOperation, "RouteFlows", routeFlowIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformAllVehiclesToRouteFlowWithEmbeddedRoute = GUIDesigns::buildFXMenuCommand(transformOperation, "RouteFlows (embedded route)", routeFlowIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformAllVehiclesToTrip = GUIDesigns::buildFXMenuCommand(transformOperation, "Trips", tripIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformAllVehiclesToFlow = GUIDesigns::buildFXMenuCommand(transformOperation, "Flows", flowIcon, this, MID_GNE_VEHICLE_TRANSFORM);
    }
}


GNEVehicle::GNESelectedVehiclesPopupMenu::~GNESelectedVehiclesPopupMenu() {}


long
GNEVehicle::GNESelectedVehiclesPopupMenu::onCmdTransform(FXObject* obj, FXSelector, void*) {
    // iterate over all selected vehicles
    for (const auto& vehicle : mySelectedVehicles) {
        if ((obj == myTransformToVehicle) &&
                (vehicle->getTagProperty().getTag() == myVehicleTag)) {
            GNERouteHandler::transformToVehicle(vehicle, false);
        } else if ((obj == myTransformToVehicleWithEmbeddedRoute) &&
                   (vehicle->getTagProperty().getTag() == myVehicleTag)) {
            GNERouteHandler::transformToVehicle(vehicle, true);
        } else if ((obj == myTransformToRouteFlow) &&
                   (vehicle->getTagProperty().getTag() == myVehicleTag)) {
            GNERouteHandler::transformToRouteFlow(vehicle, false);
        } else if ((obj == myTransformToRouteFlowWithEmbeddedRoute) &&
                   (vehicle->getTagProperty().getTag() == myVehicleTag)) {
            GNERouteHandler::transformToRouteFlow(vehicle, true);
        } else if ((obj == myTransformToTrip) &&
                   (vehicle->getTagProperty().getTag() == myVehicleTag)) {
            GNERouteHandler::transformToTrip(vehicle);
        } else if ((obj == myTransformToFlow) &&
                   (vehicle->getTagProperty().getTag() == myVehicleTag)) {
            GNERouteHandler::transformToFlow(vehicle);
        } else if (obj == myTransformAllVehiclesToVehicle) {
            GNERouteHandler::transformToVehicle(vehicle, false);
        } else if (obj == myTransformAllVehiclesToVehicleWithEmbeddedRoute) {
            GNERouteHandler::transformToVehicle(vehicle, true);
        } else if (obj == myTransformAllVehiclesToRouteFlow) {
            GNERouteHandler::transformToRouteFlow(vehicle, false);
        } else if (obj == myTransformAllVehiclesToRouteFlowWithEmbeddedRoute) {
            GNERouteHandler::transformToRouteFlow(vehicle, true);
        } else if (obj == myTransformAllVehiclesToTrip) {
            GNERouteHandler::transformToTrip(vehicle);
        } else if (obj == myTransformAllVehiclesToFlow) {
            GNERouteHandler::transformToFlow(vehicle);
        }
    }
    return 1;
}

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVehicle::GNEVehicle(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_VEHICLE, tag, GUIIconSubSys::getIcon(GUIIcon::VEHICLE),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}),
SUMOVehicleParameter() {
    // reset default values
    resetDefaultValues();
    // set end and vehPerHours
    toggleAttribute(SUMO_ATTR_END, 1);
    toggleAttribute(SUMO_ATTR_VEHSPERHOUR, 1);
}


GNEVehicle::GNEVehicle(SumoXMLTag tag, GNENet* net, const std::string& vehicleID, GNEDemandElement* vehicleType, GNEDemandElement* route) :
    GNEDemandElement(vehicleID, net, (tag == GNE_TAG_FLOW_ROUTE) ? GLO_ROUTEFLOW : GLO_VEHICLE, tag,
                     (tag == GNE_TAG_FLOW_ROUTE) ? GUIIconSubSys::getIcon(GUIIcon::ROUTEFLOW) : GUIIconSubSys::getIcon(GUIIcon::VEHICLE),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {vehicleType, route}, {}),
SUMOVehicleParameter() {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleID;
    // set manually vtypeID (needed for saving)
    vtypeid = vehicleType->getID();
    // adjust default flow attributes
    adjustDefaultFlowAttributes(this);
}


GNEVehicle::GNEVehicle(SumoXMLTag tag, GNENet* net, GNEDemandElement* vehicleType, GNEDemandElement* route, const SUMOVehicleParameter& vehicleParameters) :
    GNEDemandElement(vehicleParameters.id, net, (tag == GNE_TAG_FLOW_ROUTE) ? GLO_ROUTEFLOW : GLO_VEHICLE, tag,
                     (tag == GNE_TAG_FLOW_ROUTE) ? GUIIconSubSys::getIcon(GUIIcon::ROUTEFLOW) : GUIIconSubSys::getIcon(GUIIcon::VEHICLE),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {vehicleType, route}, {}),
SUMOVehicleParameter(vehicleParameters) {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleParameters.id;
    // set manually vtypeID (needed for saving)
    vtypeid = vehicleType->getID();
    // adjust default flow attributes
    adjustDefaultFlowAttributes(this);
}


GNEVehicle::GNEVehicle(SumoXMLTag tag, GNENet* net, GNEDemandElement* vehicleType, const SUMOVehicleParameter& vehicleParameters) :
    GNEDemandElement(vehicleParameters.id, net, (tag == GNE_TAG_VEHICLE_WITHROUTE) ? GLO_VEHICLE : GLO_ROUTEFLOW, tag,
                     (tag == GNE_TAG_FLOW_ROUTE) ? GUIIconSubSys::getIcon(GUIIcon::ROUTEFLOW) : GUIIconSubSys::getIcon(GUIIcon::VEHICLE),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {vehicleType}, {}),
SUMOVehicleParameter(vehicleParameters) {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleParameters.id;
    // reset routeid
    routeid.clear();
    // set manually vtypeID (needed for saving)
    vtypeid = vehicleType->getID();
    // adjust default flow attributes
    adjustDefaultFlowAttributes(this);
}


GNEVehicle::GNEVehicle(SumoXMLTag tag, GNENet* net, const std::string& vehicleID, GNEDemandElement* vehicleType,
                       GNEEdge* fromEdge, GNEEdge* toEdge) :
    GNEDemandElement(vehicleID, net, (tag == SUMO_TAG_FLOW) ? GLO_FLOW : GLO_TRIP, tag,
                     (tag == SUMO_TAG_FLOW) ? GUIIconSubSys::getIcon(GUIIcon::FLOW) : GUIIconSubSys::getIcon(GUIIcon::TRIP),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {fromEdge, toEdge}, {}, {}, {vehicleType}, {}),
SUMOVehicleParameter() {
    // adjust default flow attributes
    adjustDefaultFlowAttributes(this);
}


GNEVehicle::GNEVehicle(SumoXMLTag tag, GNENet* net, GNEDemandElement* vehicleType, GNEEdge* fromEdge, GNEEdge* toEdge,
                       const SUMOVehicleParameter& vehicleParameters) :
    GNEDemandElement(vehicleParameters.id, net, (tag == SUMO_TAG_FLOW) ? GLO_FLOW : GLO_TRIP, tag,
                     (tag == SUMO_TAG_FLOW) ? GUIIconSubSys::getIcon(GUIIcon::FLOW) : GUIIconSubSys::getIcon(GUIIcon::TRIP),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {fromEdge, toEdge}, {}, {}, {vehicleType}, {}),
SUMOVehicleParameter(vehicleParameters) {
    // adjust default flow attributes
    adjustDefaultFlowAttributes(this);
}


GNEVehicle::GNEVehicle(SumoXMLTag tag, GNENet* net, const std::string& vehicleID, GNEDemandElement* vehicleType, GNEJunction* fromJunction, GNEJunction* toJunction) :
    GNEDemandElement(vehicleID, net, (tag == GNE_TAG_FLOW_JUNCTIONS) ? GLO_FLOW : GLO_TRIP, tag,
                     (tag == GNE_TAG_FLOW_JUNCTIONS) ? GUIIconSubSys::getIcon(GUIIcon::FLOW_JUNCTIONS) : GUIIconSubSys::getIcon(GUIIcon::TRIP_JUNCTIONS),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {
    fromJunction, toJunction
}, {}, {}, {}, {vehicleType}, {}),
SUMOVehicleParameter() {
    // adjust default flow attributes
    adjustDefaultFlowAttributes(this);
}


GNEVehicle::GNEVehicle(SumoXMLTag tag, GNENet* net, GNEDemandElement* vehicleType, GNEJunction* fromJunction, GNEJunction* toJunction, const SUMOVehicleParameter& vehicleParameters) :
    GNEDemandElement(vehicleParameters.id, net, (tag == GNE_TAG_FLOW_JUNCTIONS) ? GLO_FLOW : GLO_TRIP, tag,
                     (tag == GNE_TAG_FLOW_JUNCTIONS) ? GUIIconSubSys::getIcon(GUIIcon::FLOW_JUNCTIONS) : GUIIconSubSys::getIcon(GUIIcon::TRIP_JUNCTIONS),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {
    fromJunction, toJunction
}, {}, {}, {}, {vehicleType}, {}),
SUMOVehicleParameter(vehicleParameters) {
    // adjust default flow attributes
    adjustDefaultFlowAttributes(this);
}


GNEVehicle::~GNEVehicle() {}


GNEMoveOperation*
GNEVehicle::getMoveOperation() {
    // get first and last lanes
    const GNELane* firstLane = getFirstPathLane();
    const GNELane* lastLane = getLastPathLane();
    // get depart and arrival positions (doubles)
    const double departPosDouble = getAttributeDouble(SUMO_ATTR_DEPARTPOS);
    const double arrivalPosDouble = (getAttributeDouble(SUMO_ATTR_ARRIVALPOS) < 0) ? lastLane->getLaneShape().length2D() : getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
    // obtain diameter
    const double diameter = getAttributeDouble(SUMO_ATTR_WIDTH) > getAttributeDouble(SUMO_ATTR_LENGTH) ? getAttributeDouble(SUMO_ATTR_WIDTH) : getAttributeDouble(SUMO_ATTR_LENGTH);
    // return move operation depending if we're editing departPos or arrivalPos
    if (myNet->getViewNet()->getPositionInformation().distanceSquaredTo(getAttributePosition(SUMO_ATTR_DEPARTPOS)) < (diameter * diameter)) {
        return new GNEMoveOperation(this, firstLane, departPosDouble, lastLane, INVALID_DOUBLE,
                                    myNet->getViewNet()->getViewParent()->getMoveFrame()->getCommonModeOptions()->getAllowChangeLane(),
                                    GNEMoveOperation::OperationType::TWO_LANES_MOVEFIRST);
    } else if (myNet->getViewNet()->getPositionInformation().distanceSquaredTo(getAttributePosition(SUMO_ATTR_ARRIVALPOS)) < (myArrivalPositionDiameter * myArrivalPositionDiameter)) {
        return new GNEMoveOperation(this, firstLane, INVALID_DOUBLE, lastLane, arrivalPosDouble,
                                    myNet->getViewNet()->getViewParent()->getMoveFrame()->getCommonModeOptions()->getAllowChangeLane(),
                                    GNEMoveOperation::OperationType::TWO_LANES_MOVESECOND);
    } else {
        return nullptr;
    }
}


std::string
GNEVehicle::getBegin() const {
    // obtain depart
    std::string departStr = depart < 0 ? "0.00" : time2string(depart);
    // we need to handle depart as a tuple of 20 numbers (format: 000000...00<departTime>)
    departStr.reserve(20 - departStr.size());
    // add 0s at the beginning of departStr until we have 20 numbers
    for (int i = (int)departStr.size(); i < 20; i++) {
        departStr.insert(departStr.begin(), '0');
    }
    return departStr;
}


void
GNEVehicle::writeDemandElement(OutputDevice& device) const {
    // attribute VType must not be written if is DEFAULT_VTYPE_ID
    if (getParentDemandElements().at(0)->getID() == DEFAULT_VTYPE_ID) {
        // unset VType parameter
        parametersSet &= ~VEHPARS_VTYPE_SET;
        // write vehicle attributes (VType will not be written)
        write(device, OptionsCont::getOptions(), myTagProperty.getXMLTag());
        // set VType parameter again
        parametersSet |= VEHPARS_VTYPE_SET;
    } else {
        // write vehicle attributes, including VType
        write(device, OptionsCont::getOptions(), myTagProperty.getXMLTag(), getParentDemandElements().at(0)->getID());
    }
    // write specific attribute depending of tag property
    if (myTagProperty.getTag() == SUMO_TAG_VEHICLE || myTagProperty.getTag() == GNE_TAG_FLOW_ROUTE) {
        // write route
        device.writeAttr(SUMO_ATTR_ROUTE, getParentDemandElements().at(1)->getID());
    }
    // write from, to and edge vias
    if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        // write manually from/to edges (it correspond to front and back parent edges)
        device.writeAttr(SUMO_ATTR_FROM, getParentEdges().front()->getID());
        device.writeAttr(SUMO_ATTR_TO, getParentEdges().back()->getID());
        // only write via if there isn't empty
        if (via.size() > 0) {
            device.writeAttr(SUMO_ATTR_VIA, via);
        }
    }
    // write fromJunction and toJunction
    if ((myTagProperty.getTag() == GNE_TAG_TRIP_JUNCTIONS) || (myTagProperty.getTag() == GNE_TAG_FLOW_JUNCTIONS)) {
        // write manually from/to junctions (it correspond to front and back parent junctions)
        device.writeAttr(SUMO_ATTR_FROMJUNCTION, getParentJunctions().front()->getID());
        device.writeAttr(SUMO_ATTR_TOJUNCTION, getParentJunctions().back()->getID());
    }
    // write specific routeFlow/flow attributes
    if (myTagProperty.isFlow()) {
        // write routeFlow values depending if it was set
        if (isAttributeEnabled(SUMO_ATTR_END)) {
            device.writeAttr(SUMO_ATTR_END, time2string(repetitionEnd));
        }
        if (isAttributeEnabled(SUMO_ATTR_NUMBER)) {
            device.writeAttr(SUMO_ATTR_NUMBER, repetitionNumber);
        }
        if (isAttributeEnabled(SUMO_ATTR_VEHSPERHOUR)) {
            device.writeAttr(SUMO_ATTR_VEHSPERHOUR, 3600. / STEPS2TIME(repetitionOffset));
        }
        if (isAttributeEnabled(SUMO_ATTR_PERIOD)) {
            device.writeAttr(SUMO_ATTR_PERIOD, time2string(repetitionOffset));
        }
        if (isAttributeEnabled(GNE_ATTR_POISSON)) {
            device.writeAttr(SUMO_ATTR_PERIOD, "exp(" + toString(1.0 / STEPS2TIME(repetitionOffset)) + ")");
        }
        if (isAttributeEnabled(SUMO_ATTR_PROB)) {
            device.writeAttr(SUMO_ATTR_PROB, repetitionProbability);
        }
    }
    // write parameters
    writeParams(device);
    // write route elements associated to this vehicle (except for calibrator FLows)
    if ((getChildDemandElements().size() > 0) && !myTagProperty.isCalibrator()) {
        if (getChildDemandElements().front()->getTagProperty().getTag() == GNE_TAG_ROUTE_EMBEDDED) {
            // write embedded route
            getChildDemandElements().front()->writeDemandElement(device);
            // write stops
            for (const auto& demandElement : getChildDemandElements()) {
                if (demandElement->getTagProperty().isStop() || demandElement->getTagProperty().isWaypoint()) {
                    demandElement->writeDemandElement(device);
                }
            }
        } else {
            for (const auto& route : getChildDemandElements()) {
                route->writeDemandElement(device);
            }
        }
    }
    // close vehicle tag
    device.closeTag();
}


GNEDemandElement::Problem
GNEVehicle::isDemandElementValid() const {
    // only trips or flows can have problems
    if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW) ||
            (myTagProperty.getTag() == GNE_TAG_TRIP_JUNCTIONS) || (myTagProperty.getTag() == GNE_TAG_FLOW_JUNCTIONS)) {
        // check path
        if (myNet->getPathManager()->isPathValid(this)) {
            return Problem::OK;
        } else {
            return Problem::INVALID_PATH;
        }
    } else if (getParentDemandElements().size() == 2) {
        // check if exist a valid path using route parent edges
        if (myNet->getPathManager()->getPathCalculator()->calculateDijkstraPath(getParentDemandElements().at(0)->getVClass(), getParentDemandElements().at(1)->getParentEdges()).size() > 0) {
            return Problem::OK;
        } else {
            return Problem::INVALID_PATH;
        }
    } else if ((getChildDemandElements().size() > 0) && getChildDemandElements().front()->getTagProperty().isRoute()) {
        // check if exist a valid path using route child edges
        if (myNet->getPathManager()->getPathCalculator()->calculateDijkstraPath(getParentDemandElements().at(0)->getVClass(), getChildDemandElements().at(0)->getParentEdges()).size() > 0) {
            return Problem::OK;
        } else {
            return Problem::INVALID_PATH;
        }
    } else {
        return Problem::INVALID_ELEMENT;
    }
}


std::string
GNEVehicle::getDemandElementProblem() const {
    // only trips or flows can have problems
    if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)getParentEdges().size(); i++) {
            if (myNet->getPathManager()->getPathCalculator()->consecutiveEdgesConnected(getParentDemandElements().at(0)->getVClass(), getParentEdges().at((int)i - 1), getParentEdges().at(i)) == false) {
                return ("There is no valid path between edges '" + getParentEdges().at((int)i - 1)->getID() + "' and '" + getParentEdges().at(i)->getID() + "'");
            }
        }
        // if there are connections between all edges, then all is ok
        return "";
    } else if ((myTagProperty.getTag() == GNE_TAG_TRIP_JUNCTIONS) || (myTagProperty.getTag() == GNE_TAG_FLOW_JUNCTIONS)) {
        return ("No path between junction '" + getParentJunctions().front()->getID() + "' and '" + getParentJunctions().back()->getID() + "'");
    } else if (getParentDemandElements().size() == 2) {
        // get route parent edges
        const std::vector<GNEEdge*>& routeEdges = getParentDemandElements().at(1)->getParentEdges();
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)routeEdges.size(); i++) {
            if (myNet->getPathManager()->getPathCalculator()->consecutiveEdgesConnected(getParentDemandElements().at(0)->getVClass(), routeEdges.at((int)i - 1), routeEdges.at(i)) == false) {
                return ("There is no valid path between route edges '" + routeEdges.at((int)i - 1)->getID() + "' and '" + routeEdges.at(i)->getID() + "'");
            }
        }
        // if there are connections between all edges, then all is ok
        return "";
    } else if ((getChildDemandElements().size() > 0) && getChildDemandElements().front()->getTagProperty().isRoute()) {
        // get route parent edges
        const std::vector<GNEEdge*>& routeEdges = getChildDemandElements().at(0)->getParentEdges();
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)routeEdges.size(); i++) {
            if (myNet->getPathManager()->getPathCalculator()->consecutiveEdgesConnected(getParentDemandElements().at(0)->getVClass(), routeEdges.at((int)i - 1), routeEdges.at(i)) == false) {
                return ("There is no valid path between embedded route edges '" + routeEdges.at((int)i - 1)->getID() + "' and '" + routeEdges.at(i)->getID() + "'");
            }
        }
        // if there are connections between all edges, then all is ok
        return "";
    } else {
        return "";
    }
}


void
GNEVehicle::fixDemandElementProblem() {

}


SUMOVehicleClass
GNEVehicle::getVClass() const {
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNEVehicle::getColor() const {
    return color;
}


void
GNEVehicle::updateGeometry() {
    if (getParentJunctions().size() > 0) {
        // calculate rotation between both junctions
        const Position posA = getParentJunctions().front()->getPositionInView();
        const Position posB = getParentJunctions().back()->getPositionInView();
        const double rot = ((double)atan2((posB.x() - posA.x()), (posA.y() - posB.y())) * (double) -180.0 / (double)M_PI);
        // update Geometry
        myDemandElementGeometry.updateSinglePosGeometry(getParentJunctions().front()->getPositionInView(), rot);
    } else {
        // get first path lane
        const GNELane* firstPathLane = getFirstPathLane();
        // check path lane
        if (firstPathLane) {
            // declare departPos
            double posOverLane = 0;
            if (wasSet(VEHPARS_DEPARTPOS_SET) && (departPosProcedure == DepartPosDefinition::GIVEN)) {
                posOverLane = departPos;
            }
            // update Geometry
            myDemandElementGeometry.updateGeometry(firstPathLane->getLaneShape(), posOverLane, myMoveElementLateralOffset);
            // compute route embedded associated with this vehicle
            for (const auto& demandElement : getChildDemandElements()) {
                if (demandElement->getTagProperty().getTag() == GNE_TAG_ROUTE_EMBEDDED) {
                    demandElement->computePathElement();
                }
                demandElement->updateGeometry();
            }
        }
    }
}


Position
GNEVehicle::getPositionInView() const {
    return myDemandElementGeometry.getShape().front();
}


GUIGLObjectPopupMenu*
GNEVehicle::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    if (isAttributeCarrierSelected()) {
        // obtain all selected vehicles
        const auto selectedDemandElements = myNet->getAttributeCarriers()->getSelectedDemandElements();
        std::vector<GNEVehicle*> selectedVehicles;
        selectedVehicles.reserve(selectedDemandElements.size());
        for (const auto& selectedDemandElement : selectedDemandElements) {
            if (selectedDemandElement->getTagProperty().isVehicle()) {
                selectedVehicles.push_back(dynamic_cast<GNEVehicle*>(selectedDemandElement));
            }
        }
        // return a GNESelectedVehiclesPopupMenu
        return new GNESelectedVehiclesPopupMenu(this, selectedVehicles, app, parent);
    } else {
        // return a GNESingleVehiclePopupMenu
        return new GNESingleVehiclePopupMenu(this, app, parent);
    }
}


std::string
GNEVehicle::getParentName() const {
    if ((myTagProperty.getTag() == SUMO_TAG_VEHICLE) || (myTagProperty.getTag() == GNE_TAG_FLOW_ROUTE)) {
        return getParentDemandElements().at(1)->getID();
    } else if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        return getParentEdges().front()->getID();
    } else {
        throw ProcessError(TL("Invalid vehicle tag"));
    }
}


double
GNEVehicle::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.vehicleSize.getExaggeration(s, this);
}


Boundary
GNEVehicle::getCenteringBoundary() const {
    Boundary vehicleBoundary;
    vehicleBoundary.add(myDemandElementGeometry.getShape().front());
    vehicleBoundary.grow(20);
    return vehicleBoundary;
}


void
GNEVehicle::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEVehicle::drawGL(const GUIVisualizationSettings& s) const {
    // only drawn in super mode demand
    if (myNet->getViewNet()->getNetworkViewOptions().showDemandElements() &&
            myNet->getViewNet()->getDataViewOptions().showDemandElements() &&
            myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(this)) {
        // declare common attributes
        const bool drawSpreadVehicles = (myNet->getViewNet()->getNetworkViewOptions().drawSpreadVehicles() || myNet->getViewNet()->getDemandViewOptions().drawSpreadVehicles());
        const double exaggeration = getExaggeration(s);
        const double width = getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_WIDTH);
        const double length = getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_LENGTH);
        const double vehicleSizeSquared = (width * width) * (length * length) * (exaggeration * exaggeration);
        const auto vehicleColor = setColor(s);
        // obtain Position an rotation (depending of draw spread vehicles)
        if (drawSpreadVehicles && mySpreadGeometry.getShape().size() == 0) {
            return;
        }
        const Position vehiclePosition = drawSpreadVehicles ? mySpreadGeometry.getShape().front() : myDemandElementGeometry.getShape().front();
        const double vehicleRotation = drawSpreadVehicles ? mySpreadGeometry.getShapeRotations().front() : myDemandElementGeometry.getShapeRotations().front();
        // check that position is valid
        if ((vehicleColor.alpha() != 0) && (vehiclePosition != Position::INVALID)) {
            // first push name
            GLHelper::pushName(getGlID());
            // first check if if mouse is enough near to this vehicle to draw it
            if (s.drawForRectangleSelection && (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(vehiclePosition) >= (vehicleSizeSquared + 2))) {
                // push draw matrix
                GLHelper::pushMatrix();
                // Start with the drawing of the area translating matrix to origin
                myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
                // translate to drawing position
                glTranslated(vehiclePosition.x(), vehiclePosition.y(), 0);
                glRotated(vehicleRotation, 0, 0, -1);
                // extra translation needed to draw vehicle over edge (to avoid selecting problems)
                glTranslated(0, (-1) * length * exaggeration, 0);
                GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
                // Pop last matrix
                GLHelper::popMatrix();
            } else {
                SUMOVehicleShape shape = getVehicleShapeID(getParentDemandElements().at(0)->getAttribute(SUMO_ATTR_GUISHAPE));
                // push draw matrix
                GLHelper::pushMatrix();
                // Start with the drawing of the area translating matrix to origin
                myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
                // translate to drawing position
                glTranslated(vehiclePosition.x(), vehiclePosition.y(), 0);
                glRotated(vehicleRotation, 0, 0, -1);
                // extra translation needed to draw vehicle over edge (to avoid selecting problems)
                glTranslated(0, (-1) * length * exaggeration, 0);
                // set lane color
                GLHelper::setColor(vehicleColor);
                double upscaleLength = exaggeration;
                if ((exaggeration > 1) && (length > 5)) {
                    // reduce the length/width ratio because this is not useful at high zoom
                    upscaleLength = MAX2(1.0, upscaleLength * (5 + sqrt(length - 5)) / length);
                }
                glScaled(exaggeration, upscaleLength, 1);
                // check if we're drawing in selecting mode
                if (s.drawForRectangleSelection) {
                    // draw vehicle as a box and don't draw the rest of details
                    GUIBaseVehicleHelper::drawAction_drawVehicleAsBoxPlus(width, length);
                } else {
                    // draw the vehicle depending of detail level
                    if (s.drawDetail(s.detailSettings.vehicleShapes, exaggeration)) {
                        GUIBaseVehicleHelper::drawAction_drawVehicleAsPoly(s, shape, width, length);
                    } else if (s.drawDetail(s.detailSettings.vehicleBoxes, exaggeration)) {
                        GUIBaseVehicleHelper::drawAction_drawVehicleAsBoxPlus(width, length);
                    } else if (s.drawDetail(s.detailSettings.vehicleTriangles, exaggeration)) {
                        GUIBaseVehicleHelper::drawAction_drawVehicleAsTrianglePlus(width, length);
                    }
                    // check if min gap has to be drawn
                    if (s.drawMinGap) {
                        const double minGap = -1 * getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_MINGAP);
                        glColor3d(0., 1., 0.);
                        glBegin(GL_LINES);
                        glVertex2d(0., 0);
                        glVertex2d(0., minGap);
                        glVertex2d(-.5, minGap);
                        glVertex2d(.5, minGap);
                        glEnd();
                    }
                    // drawing name at GLO_MAX fails unless translating z
                    glTranslated(0, MIN2(length / 2, double(5)), -getType());
                    glScaled(1 / exaggeration, 1 / upscaleLength, 1);
                    glRotated(vehicleRotation, 0, 0, -1);
                    drawName(Position(0, 0), s.scale, getParentDemandElements().at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "pedestrian" ? s.personName : s.vehicleName, s.angle);
                    // draw line
                    if (s.vehicleName.show(this) && line != "") {
                        glTranslated(0, 0.6 * s.vehicleName.scaledSize(s.scale), 0);
                        GLHelper::drawTextSettings(s.vehicleName, "line:" + line, Position(0, 0), s.scale, s.angle);
                    }
                }
                // pop draw matrix
                GLHelper::popMatrix();
                // draw line between junctions if path isn't valid
                if ((getParentJunctions().size() > 0) && !myNet->getPathManager()->isPathValid(this)) {
                    drawJunctionLine(this);
                }
                // draw stack label
                if ((myStackedLabelNumber > 0) && !drawSpreadVehicles) {
                    drawStackLabel("Vehicle", vehiclePosition, vehicleRotation, width, length, exaggeration);
                }
                // draw flow label
                if (myTagProperty.isFlow()) {
                    drawFlowLabel(vehiclePosition, vehicleRotation, width, length, exaggeration);
                }
                // draw lock icon
                GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), vehiclePosition, exaggeration);
                // check if mouse is over element
                mouseWithinGeometry(vehiclePosition, length * 0.5, width * 0.5, length * -0.5, 0, vehicleRotation);
                // inspect contour
                if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                    // draw using drawDottedContourClosedShape
                    GUIDottedGeometry::drawDottedSquaredShape(s, GUIDottedGeometry::DottedContourType::INSPECT, vehiclePosition, length * 0.5, width * 0.5, length * -0.5, 0, vehicleRotation, exaggeration);
                }
                // front contour
                if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                    // draw using drawDottedContourClosedShape
                    GUIDottedGeometry::drawDottedSquaredShape(s, GUIDottedGeometry::DottedContourType::FRONT, vehiclePosition, length * 0.5, width * 0.5, length * -0.5, 0, vehicleRotation, exaggeration);
                }
                // delete contour
                if (myNet->getViewNet()->drawDeleteContour(this, this)) {
                    // draw using drawDottedContourClosedShape
                    GUIDottedGeometry::drawDottedSquaredShape(s, GUIDottedGeometry::DottedContourType::REMOVE, vehiclePosition, length * 0.5, width * 0.5, length * -0.5, 0, vehicleRotation, exaggeration);
                }
                // select contour
                if (myNet->getViewNet()->drawSelectContour(this, this)) {
                    // draw using drawDottedContourClosedShape
                    GUIDottedGeometry::drawDottedSquaredShape(s, GUIDottedGeometry::DottedContourType::SELECT, vehiclePosition, length * 0.5, width * 0.5, length * -0.5, 0, vehicleRotation, exaggeration);
                }
                if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand() &&
                        (myNet->getViewNet()->getEditModes().demandEditMode == DemandEditMode::DEMAND_TYPE) &&
                        (myNet->getViewNet()->getViewParent()->getTypeFrame()->getTypeSelector()->getCurrentType() == getParentDemandElements().front())) {
                    // draw using drawDottedContourClosedShape
                    GUIDottedGeometry::drawDottedSquaredShape(s, GUIDottedGeometry::DottedContourType::ORANGE, vehiclePosition, length * 0.5, width * 0.5, length * -0.5, 0, vehicleRotation, exaggeration);
                }
            }
            // pop name
            GLHelper::popName();
        }
    }
}


void
GNEVehicle::computePathElement() {
    // calculate path (only for flows and trips)
    if (getParentJunctions().size() > 0) {
        // calculate path
        myNet->getPathManager()->calculatePathJunctions(this, getVClass(), getParentJunctions());
    } else if ((myTagProperty.getTag() == SUMO_TAG_FLOW) || (myTagProperty.getTag() == SUMO_TAG_TRIP)) {
        // declare lane stops
        std::vector<GNELane*> laneStopWaypoints;
        // iterate over child demand elements
        for (const auto& demandElement : getChildDemandElements()) {
            // extract lanes
            if (demandElement->getTagProperty().isStop() || demandElement->getTagProperty().isWaypoint()) {
                if (demandElement->getParentAdditionals().size() > 0) {
                    laneStopWaypoints.push_back(demandElement->getParentAdditionals().front()->getParentLanes().front());
                } else {
                    laneStopWaypoints.push_back(demandElement->getParentLanes().front());
                }
            }
        }
        // declare lane vector
        std::vector<GNELane*> lanes;
        // get first and last lanes
        const auto firstLane = getFirstPathLane();
        const auto lastLane = getLastPathLane();
        // check first and last lanes
        if (firstLane && lastLane) {
            // add first lane
            lanes.push_back(getFirstPathLane());
            // now check if there are lane Stops
            if (laneStopWaypoints.size() > 0) {
                // add stop lanes
                for (const auto& laneStop : laneStopWaypoints) {
                    lanes.push_back(laneStop);
                }
            } else {
                // add via lanes
                for (const auto& edgeID : via) {
                    const auto edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
                    if (edge) {
                        lanes.push_back(edge->getLaneByAllowedVClass(getVClass()));
                    }
                }
            }
            // add last lane
            lanes.push_back(getLastPathLane());
            // calculate path
            myNet->getPathManager()->calculatePathLanes(this, getVClass(), lanes);
        }
    }
    // update geometry
    updateGeometry();
}


void
GNEVehicle::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // get flags
    const bool dottedElement = myNet->getViewNet()->isAttributeCarrierInspected(this) ||
                               (myNet->getViewNet()->getFrontAttributeCarrier() == this) ||
                               myNet->getViewNet()->drawDeleteContour(this, this) ||
                               myNet->getViewNet()->drawSelectContour(this, this);
    const bool drawNetworkMode = myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
                                 myNet->getViewNet()->getNetworkViewOptions().showDemandElements() &&
                                 myNet->getViewNet()->getDemandViewOptions().showAllTrips();
    const bool drawDemandMode = myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand() &&
                                myNet->getViewNet()->getDemandViewOptions().showAllTrips();
    // check conditions
    if (!s.drawForRectangleSelection &&
            (drawNetworkMode || drawDemandMode || dottedElement || isAttributeCarrierSelected()) &&
            myNet->getPathManager()->getPathDraw()->drawPathGeometry(dottedElement, lane, myTagProperty.getTag())) {
        // calculate width
        const double width = s.vehicleSize.getExaggeration(s, lane) * s.widthSettings.tripWidth;
        // calculate startPos
        const double geometryDepartPos = (getParentJunctions().size() > 0) ? 0 : getAttributeDouble(SUMO_ATTR_DEPARTPOS) + getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_LENGTH);
        // get endPos
        const double geometryEndPos = (getParentJunctions().size() > 0) ? lane->getLaneGeometry().getShape().length2D() : getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
        // declare path geometry
        GUIGeometry vehicleGeometry;
        // update pathGeometry depending of first and last segment
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            vehicleGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                           geometryDepartPos, geometryEndPos,      // extreme positions
                                           Position::INVALID, Position::INVALID);  // extra positions
        } else if (segment->isFirstSegment()) {
            vehicleGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                           geometryDepartPos, -1,                  // extreme positions
                                           Position::INVALID, Position::INVALID);  // extra positions
        } else if (segment->isLastSegment()) {
            vehicleGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                           -1, geometryEndPos,                     // extreme positions
                                           Position::INVALID, Position::INVALID);  // extra positions
        } else {
            vehicleGeometry = lane->getLaneGeometry();
        }
        // obtain color
        const RGBColor pathColor = drawUsingSelectColor() ? s.colorSettings.selectedVehicleColor : s.colorSettings.vehicleTripColor;
        // Start drawing adding an gl identifier
        GLHelper::pushName(getGlID());
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area translating matrix to origin
        glTranslated(0, 0, getType() + offsetFront);
        // Set color
        GLHelper::setColor(pathColor);
        // draw geometry
        GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), vehicleGeometry, width);
        // Pop last matrix
        GLHelper::popMatrix();
        // Draw name if isn't being drawn for selecting
        if (!s.drawForRectangleSelection) {
            drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
        }
        // check if we have to draw a red line to the next segment
        if (segment->getNextSegment()) {
            // push draw matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area translating matrix to origin
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
            // Set red color
            GLHelper::setColor(RGBColor::RED);
            // get firstPosition (last position of current lane shape)
            const Position& firstPosition = lane->getLaneShape().back();
            // get lastPosition (first position of next lane shape)
            const Position& arrivalPosition = segment->getNextSegment()->getPathElement()->getPathElementArrivalPos();
            // draw box line
            GLHelper::drawBoxLine(arrivalPosition,
                                  RAD2DEG(firstPosition.angleTo2D(arrivalPosition)) - 90,
                                  firstPosition.distanceTo2D(arrivalPosition), .05);
            // pop draw matrix
            GLHelper::popMatrix();
        }
        // check if this is the last segment
        if (segment->isLastSegment() && (getParentJunctions().size() == 0)) {
            // get geometryEndPos
            const Position geometryEndPosition = getPathElementArrivalPos();
            // check if endPos can be drawn
            if (!s.drawForRectangleSelection || (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(geometryEndPosition) <= ((myArrivalPositionDiameter * myArrivalPositionDiameter) + 2))) {
                // push draw matrix
                GLHelper::pushMatrix();
                // Start with the drawing of the area translating matrix to origin
                myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
                // translate to geometryEndPos
                glTranslated(geometryEndPosition.x(), geometryEndPosition.y(), 0);
                // Set person plan color
                GLHelper::setColor(pathColor);
                // resolution of drawn circle depending of the zoom (To improve smoothness)
                GLHelper::drawFilledCircle(myArrivalPositionDiameter, s.getCircleResolution());
                // pop draw matrix
                GLHelper::popMatrix();
            }
        }
        // Pop name
        GLHelper::popName();
        // check if shape dotted contour has to be drawn
        if (dottedElement) {
            // declare trim geometry to draw
            const auto shape = (segment->isFirstSegment() || segment->isLastSegment() ? vehicleGeometry.getShape() : lane->getLaneShape());
            // check if mouse is over element
            mouseWithinGeometry(shape, width);
            // inspect contour
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT_SMALL, shape, width, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
            // front contour
            if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT_SMALL, shape, width, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
            // delete contour
            if (myNet->getViewNet()->drawDeleteContour(this, this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, width, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
            // select contour
            if (myNet->getViewNet()->drawSelectContour(this, this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, shape, width, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
        }
    }
}


void
GNEVehicle::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* /*segment*/, const double offsetFront) const {
    // get flags
    const bool dottedElement = myNet->getViewNet()->isAttributeCarrierInspected(this) ||
                               (myNet->getViewNet()->getFrontAttributeCarrier() == this) ||
                               myNet->getViewNet()->drawDeleteContour(this, this) ||
                               myNet->getViewNet()->drawSelectContour(this, this);
    const bool drawNetworkMode = myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
                                 myNet->getViewNet()->getNetworkViewOptions().showDemandElements() &&
                                 myNet->getViewNet()->getDemandViewOptions().showAllTrips();
    const bool drawDemandMode = myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand() &&
                                myNet->getViewNet()->getDemandViewOptions().showAllTrips();
    // check conditions
    if (!s.drawForRectangleSelection &&
            fromLane->getLane2laneConnections().exist(toLane) &&
            (drawNetworkMode || drawDemandMode || dottedElement || isAttributeCarrierSelected()) &&
            myNet->getPathManager()->getPathDraw()->drawPathGeometry(dottedElement, fromLane, toLane, myTagProperty.getTag())) {
        // Start drawing adding an gl identifier
        GLHelper::pushName(getGlID());
        // obtain lane2lane geometry
        const GUIGeometry& lane2laneGeometry = fromLane->getLane2laneConnections().getLane2laneGeometry(toLane);
        // calculate width
        const double width = s.vehicleSize.getExaggeration(s, fromLane) * s.widthSettings.tripWidth;
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area translating matrix to origin
        glTranslated(0, 0, getType() + offsetFront);
        // Set color of the base
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectedVehicleColor);
        } else {
            GLHelper::setColor(s.colorSettings.vehicleTripColor);
        }
        // draw lane2lane
        GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), lane2laneGeometry, width);
        // Pop last matrix
        GLHelper::popMatrix();
        // check if shape dotted contour has to be drawn
        if (dottedElement) {
            // check if exist lane2lane connection
            if (fromLane->getLane2laneConnections().exist(toLane)) {
                // draw inspected dotted contour
                if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                    GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT_SMALL,
                            fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                            width, 1, false, false);
                }
                // draw front dotted contour
                if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                    GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT_SMALL,
                            fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                            width, 1, false, false);
                }
                // delete contour
                if (myNet->getViewNet()->drawDeleteContour(this, this)) {
                    GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE,
                            fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                            width, 1, false, false);
                }
                // select contour
                if (myNet->getViewNet()->drawSelectContour(this, this)) {
                    GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT,
                            fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                            width, 1, false, false);
                }
            }
        }
        // Pop name
        GLHelper::popName();
    }
}


GNELane*
GNEVehicle::getFirstPathLane() const {
    // declare first edge
    GNEEdge* firstEdge = nullptr;
    // continue depending of tags
    if ((myTagProperty.getTag() == SUMO_TAG_VEHICLE) || (myTagProperty.getTag() == GNE_TAG_FLOW_ROUTE)) {
        // check departEdge
        if ((departEdge > 0) && (departEdge < (int)getParentDemandElements().at(1)->getParentEdges().size())) {
            // use departEdge
            firstEdge = getParentDemandElements().at(1)->getParentEdges().at(departEdge);
        } else {
            // use first route edge
            firstEdge = getParentDemandElements().at(1)->getParentEdges().front();
        }
    } else if (myTagProperty.hasEmbeddedRoute()) {
        // check if embedded route exist (due during loading embedded route doesn't exist
        if (getChildDemandElements().empty()) {
            return nullptr;
        }
        // check departEdge
        if ((departEdge > 0) && (departEdge < (int)getChildDemandElements().front()->getParentEdges().size())) {
            // use depart edge
            firstEdge = getChildDemandElements().front()->getParentEdges().at(departEdge);
        } else if (getChildDemandElements().front()->getParentEdges().size() > 0) {
            firstEdge = getChildDemandElements().front()->getParentEdges().front();
        } else if (getChildDemandElements().front()->getParentLanes().size() > 0) {
            firstEdge = getChildDemandElements().front()->getParentLanes().front()->getParentEdge();
        } else {
            return nullptr;
        }
    } else {
        // use first parent edge
        firstEdge = getParentEdges().front();
    }
    // get departLane index
    const int departLaneIndex = (int)getAttributeDouble(SUMO_ATTR_DEPARTLANE);
    // check departLane index
    if ((departLaneIndex >= 0) && (departLaneIndex < (int)firstEdge->getLanes().size())) {
        return firstEdge->getLanes().at(departLaneIndex);
    } else {
        // get first allowed VClass
        return firstEdge->getLaneByAllowedVClass(getVClass());
    }
}


GNELane*
GNEVehicle::getLastPathLane() const {
    // declare last edge
    GNEEdge* lastEdge = nullptr;
    // continue depending of tags
    if ((myTagProperty.getTag() == SUMO_TAG_VEHICLE) || (myTagProperty.getTag() == GNE_TAG_FLOW_ROUTE)) {
        // check arrivalEdge
        if ((arrivalEdge > 0) && (arrivalEdge < (int)getParentDemandElements().at(1)->getParentEdges().size())) {
            // use arrival edge
            lastEdge = getParentDemandElements().at(1)->getParentEdges().at(arrivalEdge);
        } else {
            // use last route edge
            lastEdge = getParentDemandElements().at(1)->getParentEdges().back();
        }
    } else if (myTagProperty.hasEmbeddedRoute()) {
        // check if embedded route exist (due during loading embedded route doesn't exist)
        if (getChildDemandElements().empty()) {
            return nullptr;
        }
        // check arrivalEdge
        if ((arrivalEdge > 0) && (arrivalEdge < (int)getChildDemandElements().front()->getParentEdges().size())) {
            // use arrival edge
            lastEdge = getChildDemandElements().front()->getParentEdges().at(arrivalEdge);
        } else if (getChildDemandElements().front()->getParentEdges().size() > 0) {
            // use last route edge
            lastEdge = getChildDemandElements().front()->getParentEdges().back();
        } else if (getChildDemandElements().front()->getParentLanes().size() > 0) {
            // use lane
            lastEdge = getChildDemandElements().front()->getParentLanes().back()->getParentEdge();
        } else {
            return nullptr;
        }
    } else {
        // use last parent edge
        lastEdge = getParentEdges().back();
    }
    // get arrivalLane index
    const int arrivalLaneIndex = (int)getAttributeDouble(SUMO_ATTR_ARRIVALLANE);
    // check arrivalLane index
    if ((arrivalLaneIndex >= 0) && (arrivalLaneIndex < (int)lastEdge->getLanes().size())) {
        return lastEdge->getLanes().at(arrivalLaneIndex);
    } else {
        // get last allowed VClass
        return lastEdge->getLaneByAllowedVClass(getVClass());
    }
}


std::string
GNEVehicle::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_TYPE:
            return getParentDemandElements().at(0)->getID();
        case SUMO_ATTR_COLOR:
            if (wasSet(VEHPARS_COLOR_SET)) {
                return toString(color);
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_COLOR);
            }
        case SUMO_ATTR_DEPARTLANE:
            if (wasSet(VEHPARS_DEPARTLANE_SET)) {
                return getDepartLane();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_DEPARTLANE);
            }
        case SUMO_ATTR_DEPARTPOS:
            if (wasSet(VEHPARS_DEPARTPOS_SET)) {
                return getDepartPos();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_DEPARTPOS);
            }
        case SUMO_ATTR_DEPARTSPEED:
            if (wasSet(VEHPARS_DEPARTSPEED_SET)) {
                return getDepartSpeed();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_DEPARTSPEED);
            }
        case SUMO_ATTR_ARRIVALLANE:
            if (wasSet(VEHPARS_ARRIVALLANE_SET)) {
                return getArrivalLane();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_ARRIVALLANE);
            }
        case SUMO_ATTR_ARRIVALPOS:
            if (wasSet(VEHPARS_ARRIVALPOS_SET)) {
                return getArrivalPos();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_ARRIVALPOS);
            }
        case SUMO_ATTR_ARRIVALSPEED:
            if (wasSet(VEHPARS_ARRIVALSPEED_SET)) {
                return getArrivalSpeed();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_ARRIVALSPEED);
            }
        case SUMO_ATTR_LINE:
            if (wasSet(VEHPARS_LINE_SET)) {
                return line;
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_LINE);
            }
        case SUMO_ATTR_PERSON_NUMBER:
            if (wasSet(VEHPARS_PERSON_NUMBER_SET)) {
                return toString(personNumber);
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_PERSON_NUMBER);
            }
        case SUMO_ATTR_CONTAINER_NUMBER:
            if (wasSet(VEHPARS_CONTAINER_NUMBER_SET)) {
                return toString(containerNumber);
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_CONTAINER_NUMBER);
            }
        case SUMO_ATTR_REROUTE:
            if (wasSet(VEHPARS_CONTAINER_NUMBER_SET)) {
                return "true";
            } else {
                return "false";
            }
        case SUMO_ATTR_DEPARTPOS_LAT:
            if (wasSet(VEHPARS_DEPARTPOSLAT_SET)) {
                return getDepartPosLat();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_DEPARTPOS_LAT);
            }
        case SUMO_ATTR_ARRIVALPOS_LAT:
            if (wasSet(VEHPARS_ARRIVALPOSLAT_SET)) {
                return getArrivalPosLat();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_ARRIVALPOS_LAT);
            }
        case SUMO_ATTR_INSERTIONCHECKS:
            return getInsertionChecks();
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN:
            if (departProcedure == DepartDefinition::TRIGGERED) {
                return "triggered";
            } else if (departProcedure == DepartDefinition::CONTAINER_TRIGGERED) {
                return "containerTriggered";
            } else if (departProcedure == DepartDefinition::NOW) {
                return "now";
            } else if (departProcedure == DepartDefinition::SPLIT) {
                return "split";
            } else if (departProcedure == DepartDefinition::BEGIN) {
                return "begin";
            } else {
                return time2string(depart);
            }
        case SUMO_ATTR_ROUTE:
            if (getParentDemandElements().size() == 2) {
                return getParentDemandElements().at(1)->getID();
            } else {
                return "";
            }
        // Specific of from-to edge
        case SUMO_ATTR_FROM:
            return getParentEdges().front()->getID();
        case SUMO_ATTR_TO:
            return getParentEdges().back()->getID();
        case SUMO_ATTR_VIA:
            return toString(via);
        case SUMO_ATTR_DEPARTEDGE:
            if (departEdge == -1) {
                return "";
            } else {
                return toString(departEdge);
            }
        case SUMO_ATTR_ARRIVALEDGE:
            if (arrivalEdge == -1) {
                return "";
            } else {
                return toString(arrivalEdge);
            }
        // Specific of from-to junctions
        case SUMO_ATTR_FROMJUNCTION:
            return getParentJunctions().front()->getID();
        case SUMO_ATTR_TOJUNCTION:
            return getParentJunctions().back()->getID();
        // Specific of flows
        case SUMO_ATTR_END:
            return time2string(repetitionEnd);
        case SUMO_ATTR_VEHSPERHOUR:
            return toString(3600 / STEPS2TIME(repetitionOffset));
        case SUMO_ATTR_PERIOD:
            return time2string(repetitionOffset);
        case GNE_ATTR_POISSON:
            return toString(1 / STEPS2TIME(repetitionOffset));
        case SUMO_ATTR_PROB:
            return toString(repetitionProbability, 10);
        case SUMO_ATTR_NUMBER:
            return toString(repetitionNumber);
        // other
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        case GNE_ATTR_FLOWPARAMETERS:
            return toString(parametersSet);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEVehicle::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN:
            return STEPS2TIME(depart);
        case SUMO_ATTR_DEPARTLANE:
            if (wasSet(VEHPARS_DEPARTLANE_SET) && (departLaneProcedure == DepartLaneDefinition::GIVEN)) {
                return departLane;
            } else {
                return -1;
            }
        case SUMO_ATTR_DEPARTPOS:
            // only return departPos it if is given
            if (departPosProcedure == DepartPosDefinition::GIVEN) {
                return departPos;
            } else {
                return 0;
            }
        case SUMO_ATTR_ARRIVALLANE:
            if (wasSet(VEHPARS_ARRIVALLANE_SET) && (arrivalLaneProcedure == ArrivalLaneDefinition::GIVEN)) {
                return arrivalLane;
            } else {
                return -1;
            }
        case SUMO_ATTR_ARRIVALPOS:
            // only return departPos it if is given
            if (arrivalPosProcedure == ArrivalPosDefinition::GIVEN) {
                return arrivalPos;
            } else {
                return -1;
            }
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_MINGAP:
            return getParentDemandElements().at(0)->getAttributeDouble(key);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


Position
GNEVehicle::getAttributePosition(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPARTPOS: {
            // get first path lane shape
            const PositionVector& laneShape = getFirstPathLane()->getLaneShape();
            // check arrivalPosProcedure
            if (departPosProcedure == DepartPosDefinition::GIVEN) {
                if (departPos < 0) {
                    return laneShape.front();
                } else if (departPos > laneShape.length2D()) {
                    return laneShape.back();
                } else {
                    return laneShape.positionAtOffset2D(departPos);
                }
            } else {
                return laneShape.front();
            }
        }
        case SUMO_ATTR_ARRIVALPOS: {
            // get last path lane shape
            const PositionVector& laneShape = getLastPathLane()->getLaneShape();
            // check arrivalPosProcedure
            if (arrivalPosProcedure == ArrivalPosDefinition::GIVEN) {
                if (arrivalPos < 0) {
                    return laneShape.front();
                } else if (arrivalPos > laneShape.length2D()) {
                    return laneShape.back();
                } else {
                    return laneShape.positionAtOffset2D(arrivalPos);
                }
            } else {
                return laneShape.back();
            }
        }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


void
GNEVehicle::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_DEPARTLANE:
        case SUMO_ATTR_DEPARTPOS:
        case SUMO_ATTR_DEPARTSPEED:
        case SUMO_ATTR_ARRIVALLANE:
        case SUMO_ATTR_ARRIVALPOS:
        case SUMO_ATTR_ARRIVALSPEED:
        case SUMO_ATTR_LINE:
        case SUMO_ATTR_PERSON_NUMBER:
        case SUMO_ATTR_CONTAINER_NUMBER:
        case SUMO_ATTR_REROUTE:
        case SUMO_ATTR_DEPARTPOS_LAT:
        case SUMO_ATTR_ARRIVALPOS_LAT:
        case SUMO_ATTR_INSERTIONCHECKS:
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_ROUTE:
        // Specific of from-to edges
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case SUMO_ATTR_VIA:
        case SUMO_ATTR_DEPARTEDGE:
        case SUMO_ATTR_ARRIVALEDGE:
        // Specific of from-to junctions
        case SUMO_ATTR_FROMJUNCTION:
        case SUMO_ATTR_TOJUNCTION:
        // Specific of flows
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
        case SUMO_ATTR_PROB:
        // other
        case GNE_ATTR_PARAMETERS:
        case GNE_ATTR_SELECTED:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEVehicle::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            // Vehicles, Trips and Flows share namespace
            if (SUMOXMLDefinitions::isValidVehicleID(value) &&
                    (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VEHICLE, value, false) == nullptr) &&
                    (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_TRIP, value, false) == nullptr) &&
                    (myNet->getAttributeCarriers()->retrieveDemandElement(GNE_TAG_FLOW_ROUTE, value, false) == nullptr) &&
                    (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_FLOW, value, false) == nullptr)) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_TYPE:
            return SUMOXMLDefinitions::isValidTypeID(value) && (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, value, false) != nullptr);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_DEPARTLANE: {
            int dummyDepartLane;
            DepartLaneDefinition dummyDepartLaneProcedure;
            parseDepartLane(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartLane, dummyDepartLaneProcedure, error);
            // if error is empty, check if depart lane is correct
            if (error.empty()) {
                if (dummyDepartLaneProcedure != DepartLaneDefinition::GIVEN) {
                    return true;
                } else if (isTemplate()) {
                    return true;
                } else if (getParentJunctions().size() > 0) {
                    return (dummyDepartLane == 0);
                } else {
                    return dummyDepartLane < (int)getFirstPathLane()->getParentEdge()->getLanes().size();
                }
            } else {
                return false;
            }
        }
        case SUMO_ATTR_DEPARTPOS: {
            double dummyDepartPos;
            DepartPosDefinition dummyDepartPosProcedure;
            parseDepartPos(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartPos, dummyDepartPosProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_DEPARTSPEED: {
            double dummyDepartSpeed;
            DepartSpeedDefinition dummyDepartSpeedProcedure;
            parseDepartSpeed(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartSpeed, dummyDepartSpeedProcedure, error);
            // if error is empty, check if depart speed is correct
            if (error.empty()) {
                if (dummyDepartSpeedProcedure != DepartSpeedDefinition::GIVEN) {
                    return true;
                } else if (isTemplate()) {
                    return true;
                } else {
                    return (dummyDepartSpeed <= getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_MAXSPEED));
                }
            } else {
                return false;
            }
        }
        case SUMO_ATTR_ARRIVALLANE: {
            int dummyArrivalLane;
            ArrivalLaneDefinition dummyArrivalLaneProcedure;
            parseArrivalLane(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalLane, dummyArrivalLaneProcedure, error);
            // if error is empty, given value is valid
            if (error.empty()) {
                if (dummyArrivalLaneProcedure != ArrivalLaneDefinition::GIVEN) {
                    return true;
                } else if (isTemplate()) {
                    return true;
                } else if (getParentJunctions().size() > 0) {
                    return (dummyArrivalLane == 0);
                } else {
                    return dummyArrivalLane < (int)getLastPathLane()->getParentEdge()->getLanes().size();
                }
            } else {
                return false;
            }
        }
        case SUMO_ATTR_ARRIVALPOS: {
            double dummyArrivalPos;
            ArrivalPosDefinition dummyArrivalPosProcedure;
            parseArrivalPos(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalPos, dummyArrivalPosProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALSPEED: {
            double dummyArrivalSpeed;
            ArrivalSpeedDefinition dummyArrivalSpeedProcedure;
            parseArrivalSpeed(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalSpeed, dummyArrivalSpeedProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_LINE:
            return true;
        case SUMO_ATTR_PERSON_NUMBER:
            return canParse<int>(value) && parse<int>(value) >= 0;
        case SUMO_ATTR_CONTAINER_NUMBER:
            return canParse<int>(value) && parse<int>(value) >= 0;
        case SUMO_ATTR_REROUTE:
            return true;    // check
        case SUMO_ATTR_DEPARTPOS_LAT: {
            double dummyDepartPosLat;
            DepartPosLatDefinition dummyDepartPosLatProcedure;
            parseDepartPosLat(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartPosLat, dummyDepartPosLatProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALPOS_LAT: {
            double dummyArrivalPosLat;
            ArrivalPosLatDefinition dummyArrivalPosLatProcedure;
            parseArrivalPosLat(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalPosLat, dummyArrivalPosLatProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_INSERTIONCHECKS:
            return areInsertionChecksValid(value);
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN: {
            SUMOTime dummyDepart;
            DepartDefinition dummyDepartProcedure;
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, dummyDepart, dummyDepartProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ROUTE:
            if (getParentDemandElements().size() == 2) {
                return SUMOXMLDefinitions::isValidVehicleID(value) && (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, value, false) != nullptr);
            } else {
                return true;
            }
        // Specific of from-to edges
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->getAttributeCarriers()->retrieveEdge(value, false) != nullptr);
        case SUMO_ATTR_DEPARTEDGE:
        case SUMO_ATTR_ARRIVALEDGE: {
            if (value.empty()) {
                return true;
            } else if (canParse<int>(value)) {
                // get index
                const int index = parse<int>(value);
                // check conditions
                if (index < 0) {
                    return false;
                } else if (myTagProperty.getTag() == SUMO_TAG_VEHICLE || myTagProperty.getTag() == GNE_TAG_FLOW_ROUTE) {
                    // check parent route
                    return (index < (int)getParentDemandElements().at(1)->getParentEdges().size());
                } else {
                    // check embedded route
                    return (index < (int)getChildDemandElements().front()->getParentEdges().size());
                }
            } else {
                return false;
            }
        }
        case SUMO_ATTR_VIA:
            if (value.empty()) {
                return true;
            } else {
                return canParse<std::vector<GNEEdge*> >(myNet, value, false);
            }
        // Specific of from-to junctions
        case SUMO_ATTR_FROMJUNCTION:
        case SUMO_ATTR_TOJUNCTION:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->getAttributeCarriers()->retrieveJunction(value, false) != nullptr);
        // Specific of flows
        case SUMO_ATTR_END:
            if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_VEHSPERHOUR:
            if (canParse<double>(value)) {
                return (parse<double>(value) > 0);
            } else {
                return false;
            }
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
            if (canParse<double>(value)) {
                return (parse<double>(value) > 0);
            } else {
                return false;
            }
        case SUMO_ATTR_PROB:
            if (canParse<double>(value)) {
                const double prob = parse<double>(value);
                return ((prob >= 0) && (prob <= 1));
            } else {
                return false;
            }
        case SUMO_ATTR_NUMBER:
            if (canParse<int>(value)) {
                return (parse<int>(value) >= 0);
            } else {
                return false;
            }
        // other
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEVehicle::enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
        case SUMO_ATTR_PROB:
            undoList->add(new GNEChange_EnableAttribute(this, key, true, parametersSet), true);
            return;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEVehicle::disableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case GNE_ATTR_POISSON:
        case SUMO_ATTR_PROB:
            undoList->add(new GNEChange_EnableAttribute(this, key, false, parametersSet), true);
            return;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEVehicle::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_END:
            return (parametersSet & VEHPARS_END_SET) != 0;
        case SUMO_ATTR_NUMBER:
            return (parametersSet & VEHPARS_NUMBER_SET) != 0;
        case SUMO_ATTR_VEHSPERHOUR:
            return (parametersSet & VEHPARS_VPH_SET) != 0;
        case SUMO_ATTR_PERIOD:
            return (parametersSet & VEHPARS_PERIOD_SET) != 0;
        case GNE_ATTR_POISSON:
            return (parametersSet & VEHPARS_POISSON_SET) != 0;
        case SUMO_ATTR_PROB:
            return (parametersSet & VEHPARS_PROB_SET) != 0;
        default:
            return true;
    }
}


std::string
GNEVehicle::getPopUpID() const {
    return getTagStr();
}


std::string
GNEVehicle::getHierarchyName() const {
    // special case for Trips and flow
    if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        // check if we're inspecting a Edge
        if (!myNet->getViewNet()->getInspectedAttributeCarriers().empty() &&
                myNet->getViewNet()->getInspectedAttributeCarriers().front()->getTagProperty().getTag() == SUMO_TAG_EDGE) {
            // check if edge correspond to a "from", "to" or "via" edge
            if (myNet->getViewNet()->isAttributeCarrierInspected(getParentEdges().front())) {
                return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (from)";
            } else if (myNet->getViewNet()->isAttributeCarrierInspected(getParentEdges().front())) {
                return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (to)";
            } else {
                // iterate over via
                for (const auto& i : via) {
                    if (i == myNet->getViewNet()->getInspectedAttributeCarriers().front()->getID()) {
                        return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (via)";
                    }
                }
            }
        }
    }
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID);
}


const Parameterised::Map&
GNEVehicle::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// protected
// ===========================================================================

RGBColor
GNEVehicle::setColor(const GUIVisualizationSettings& s) const {
    // change color
    if (drawUsingSelectColor()) {
        return s.colorSettings.selectedVehicleColor;
    } else {
        // obtain vehicle color
        const GUIColorer& c = s.vehicleColorer;
        // set color depending of vehicle color active
        switch (c.getActive()) {
            case 0: {
                // test for emergency vehicle
                if (getParentDemandElements().at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "emergency") {
                    return RGBColor::WHITE;
                }
                // test for firebrigade
                if (getParentDemandElements().at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "firebrigade") {
                    return RGBColor::RED;
                }
                // test for police car
                if (getParentDemandElements().at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "police") {
                    return RGBColor::BLUE;
                }
                if (getParentDemandElements().at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "scooter") {
                    return RGBColor::WHITE;
                }
                // check if color was set
                if (wasSet(VEHPARS_COLOR_SET)) {
                    return color;
                } else {
                    // take their parent's color)
                    return getParentDemandElements().at(0)->getColor();
                }
            }
            case 2: {
                if (wasSet(VEHPARS_COLOR_SET)) {
                    return color;
                } else {
                    return c.getScheme().getColor(0);
                }
            }
            case 3: {
                if (getParentDemandElements().at(0)->isAttributeEnabled(SUMO_ATTR_COLOR)) {
                    return getParentDemandElements().at(0)->getColor();
                } else {
                    return c.getScheme().getColor(0);
                }
                break;
            }
            case 4: {
                if (getParentDemandElements().at(1)->getColor() != RGBColor::DEFAULT_COLOR) {
                    return getParentDemandElements().at(1)->getColor();
                } else {
                    return c.getScheme().getColor(0);
                }
            }
            case 5: {
                Position p = getParentDemandElements().at(1)->getParentEdges().at(0)->getLanes().at(0)->getLaneShape()[0];
                const Boundary& b = myNet->getBoundary();
                Position center = b.getCenter();
                double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
                double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
                return RGBColor::fromHSV(hue, sat, 1.);
            }
            case 6: {
                Position p = getParentDemandElements().at(1)->getParentEdges().back()->getLanes().at(0)->getLaneShape()[-1];
                const Boundary& b = myNet->getBoundary();
                Position center = b.getCenter();
                double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
                double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
                return RGBColor::fromHSV(hue, sat, 1.);
            }
            case 7: {
                Position pb = getParentDemandElements().at(1)->getParentEdges().at(0)->getLanes().at(0)->getLaneShape()[0];
                Position pe = getParentDemandElements().at(1)->getParentEdges().back()->getLanes().at(0)->getLaneShape()[-1];
                const Boundary& b = myNet->getBoundary();
                double hue = 180. + atan2(pb.x() - pe.x(), pb.y() - pe.y()) * 180. / M_PI;
                Position minp(b.xmin(), b.ymin());
                Position maxp(b.xmax(), b.ymax());
                double sat = pb.distanceTo(pe) / minp.distanceTo(maxp);
                return RGBColor::fromHSV(hue, sat, 1.);
            }
            case 29: { // color randomly (by pointer hash)
                std::hash<const GNEVehicle*> ptr_hash;
                const double hue = (double)(ptr_hash(this) % 360); // [0-360]
                const double sat = (double)((ptr_hash(this) / 360) % 67) / 100. + 0.33; // [0.33-1]
                return RGBColor::fromHSV(hue, sat, 1.);
            }
            default: {
                return c.getScheme().getColor(0);
            }
        }
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVehicle::setAttribute(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    // flag to upate stack label
    bool updateSpreadStackGeometry = false;
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setMicrosimID(value);
            // set manually vehicle ID (needed for saving)
            id = value;
            // Change IDs of all person plans children (stops, embedded routes...)
            for (const auto& childDemandElement : getChildDemandElements()) {
                childDemandElement->setMicrosimID(getID());
            }
            break;
        case SUMO_ATTR_TYPE:
            if (getID().size() > 0) {
                replaceDemandElementParent(SUMO_TAG_VTYPE, value, 0);
                // set manually vtypeID (needed for saving)
                vtypeid = value;
            }
            break;
        case SUMO_ATTR_COLOR:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                color = parse<RGBColor>(value);
                // mark parameter as set
                parametersSet |= VEHPARS_COLOR_SET;
            } else {
                // set default value
                color = parse<RGBColor>(myTagProperty.getDefaultValue(key));
                // unset parameter
                parametersSet &= ~VEHPARS_COLOR_SET;
            }
            break;
        case SUMO_ATTR_DEPARTLANE:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseDepartLane(value, toString(SUMO_TAG_VEHICLE), id, departLane, departLaneProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTLANE_SET;
            } else {
                // set default value
                parseDepartLane(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, departLane, departLaneProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTLANE_SET;
            }
            break;
        case SUMO_ATTR_DEPARTPOS:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseDepartPos(value, toString(SUMO_TAG_VEHICLE), id, departPos, departPosProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTPOS_SET;
            } else {
                // set default value
                parseDepartPos(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, departPos, departPosProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTPOS_SET;
            }
            if (getID().size() > 0) {
                updateGeometry();
                updateSpreadStackGeometry = true;
            }
            break;
        case SUMO_ATTR_DEPARTSPEED:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseDepartSpeed(value, toString(SUMO_TAG_VEHICLE), id, departSpeed, departSpeedProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTSPEED_SET;
            } else {
                // set default value
                parseDepartSpeed(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, departSpeed, departSpeedProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTSPEED_SET;
            }
            break;
        case SUMO_ATTR_ARRIVALLANE:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseArrivalLane(value, toString(SUMO_TAG_VEHICLE), id, arrivalLane, arrivalLaneProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_ARRIVALLANE_SET;
            } else {
                // set default value
                parseArrivalLane(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, arrivalLane, arrivalLaneProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_ARRIVALLANE_SET;
            }
            break;
        case SUMO_ATTR_ARRIVALPOS:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseArrivalPos(value, toString(SUMO_TAG_VEHICLE), id, arrivalPos, arrivalPosProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_ARRIVALPOS_SET;
            } else {
                // set default value
                parseArrivalPos(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, arrivalPos, arrivalPosProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_ARRIVALPOS_SET;
            }
            if (getID().size() > 0) {
                updateGeometry();
                updateSpreadStackGeometry = true;
            }
            break;
        case SUMO_ATTR_ARRIVALSPEED:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseArrivalSpeed(value, toString(SUMO_TAG_VEHICLE), id, arrivalSpeed, arrivalSpeedProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_ARRIVALSPEED_SET;
            } else {
                // set default value
                parseArrivalSpeed(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, arrivalSpeed, arrivalSpeedProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_ARRIVALSPEED_SET;
            }
            break;
        case SUMO_ATTR_LINE:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                line = value;
                // mark parameter as set
                parametersSet |= VEHPARS_LINE_SET;
            } else {
                // set default value
                line = myTagProperty.getDefaultValue(key);
                // unset parameter
                parametersSet &= ~VEHPARS_LINE_SET;
            }
            break;
        case SUMO_ATTR_PERSON_NUMBER:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                personNumber = parse<int>(value);
                // mark parameter as set
                parametersSet |= VEHPARS_PERSON_NUMBER_SET;
            } else {
                // set default value
                personNumber = parse<int>(myTagProperty.getDefaultValue(key));
                // unset parameter
                parametersSet &= ~VEHPARS_PERSON_NUMBER_SET;
            }
            break;
        case SUMO_ATTR_CONTAINER_NUMBER:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                containerNumber = parse<int>(value);
                // mark parameter as set
                parametersSet |= VEHPARS_CONTAINER_NUMBER_SET;
            } else {
                // set default value
                containerNumber = parse<int>(myTagProperty.getDefaultValue(key));
                // unset parameter
                parametersSet &= ~VEHPARS_CONTAINER_NUMBER_SET;
            }
            break;
        case SUMO_ATTR_REROUTE:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                // mark parameter as set
                parametersSet |= VEHPARS_ROUTE_SET;
            } else {
                // unset parameter
                parametersSet &= ~VEHPARS_ROUTE_SET;
            }
            break;
        case SUMO_ATTR_DEPARTPOS_LAT:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseDepartPosLat(value, toString(SUMO_TAG_VEHICLE), id, departPosLat, departPosLatProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTPOSLAT_SET;
            } else {
                // set default value
                parseDepartPosLat(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, departPosLat, departPosLatProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTPOSLAT_SET;
            }
            break;
        case SUMO_ATTR_ARRIVALPOS_LAT:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseArrivalPosLat(value, toString(SUMO_TAG_VEHICLE), id, arrivalPosLat, arrivalPosLatProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_ARRIVALPOSLAT_SET;
            } else {
                // set default value
                parseArrivalPosLat(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, arrivalPosLat, arrivalPosLatProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_ARRIVALPOSLAT_SET;
            }
            parseArrivalPosLat(value, toString(SUMO_TAG_VEHICLE), id, arrivalPosLat, arrivalPosLatProcedure, error);
            break;
        case SUMO_ATTR_INSERTIONCHECKS:
            parseInsertionChecks(value);
            break;
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN: {
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, depart, departProcedure, error);
            break;
        }
        case SUMO_ATTR_ROUTE:
            if (getParentDemandElements().size() == 2) {
                replaceDemandElementParent(SUMO_TAG_ROUTE, value, 1);
            }
            updateGeometry();
            updateSpreadStackGeometry = true;
            break;
        // Specific of from-to edges
        case SUMO_ATTR_FROM: {
            // change first edge
            replaceFirstParentEdge(value);
            // compute vehicle
            computePathElement();
            updateSpreadStackGeometry = true;
            break;
        }
        case SUMO_ATTR_TO: {
            // change last edge
            replaceLastParentEdge(value);
            // compute vehicle
            computePathElement();
            updateSpreadStackGeometry = true;
            break;
        }
        case SUMO_ATTR_VIA: {
            if (!value.empty()) {
                // set new via edges
                via = parse< std::vector<std::string> >(value);
                // mark parameter as set
                parametersSet |= VEHPARS_VIA_SET;
            } else {
                // clear via
                via.clear();
                // unset parameter
                parametersSet &= ~VEHPARS_VIA_SET;
            }
            // compute vehicle
            computePathElement();
            updateSpreadStackGeometry = true;
            break;
        }
        case SUMO_ATTR_DEPARTEDGE: {
            // update depart edge
            if (value.empty()) {
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTEDGE_SET;
                departEdge = -1;
                departEdgeProcedure = RouteIndexDefinition::DEFAULT;
            } else {
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTEDGE_SET;
                departEdge = parse<int>(value);
                departEdgeProcedure = RouteIndexDefinition::GIVEN;
            }
            // compute vehicle
            if (getID().size() > 0) {
                computePathElement();
                updateSpreadStackGeometry = true;
            }
            break;
        }
        case SUMO_ATTR_ARRIVALEDGE: {
            // update arrival edge
            if (value.empty()) {
                // unset parameter
                parametersSet &= ~VEHPARS_ARRIVALEDGE_SET;
                arrivalEdge = -1;
                arrivalEdgeProcedure = RouteIndexDefinition::DEFAULT;
            } else {
                // mark parameter as set
                parametersSet |= VEHPARS_ARRIVALEDGE_SET;
                arrivalEdge = parse<int>(value);
                arrivalEdgeProcedure = RouteIndexDefinition::GIVEN;
            }
            if (getID().size() > 0) {
                // compute vehicle
                computePathElement();
                updateSpreadStackGeometry = true;
            }
            break;
        }
        // Specific of from-to junctions
        case SUMO_ATTR_FROMJUNCTION: {
            // change first junction
            replaceFirstParentJunction(value);
            // compute vehicle
            computePathElement();
            updateSpreadStackGeometry = true;
            break;
        }
        case SUMO_ATTR_TOJUNCTION: {
            // change last junction
            replaceLastParentJunction(value);
            // compute vehicle
            computePathElement();
            updateSpreadStackGeometry = true;
            break;
        }
        // Specific of flows
        case SUMO_ATTR_END:
            repetitionEnd = string2time(value);
            break;
        case SUMO_ATTR_VEHSPERHOUR:
            repetitionOffset = TIME2STEPS(3600 / parse<double>(value));
            break;
        case SUMO_ATTR_PERIOD:
            repetitionOffset = string2time(value);
            break;
        case GNE_ATTR_POISSON:
            repetitionOffset = TIME2STEPS(1 / parse<double>(value));
            break;
        case SUMO_ATTR_PROB:
            repetitionProbability = parse<double>(value);
            break;
        case SUMO_ATTR_NUMBER:
            repetitionNumber = parse<int>(value);
            break;
        // other
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // check if stack label has to be updated
    if (updateSpreadStackGeometry) {
        if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
            getParentEdges().front()->updateVehicleStackLabels();
            getParentEdges().front()->updateVehicleSpreadGeometries();
        } else if (getParentDemandElements().size() == 2) {
            getParentDemandElements().at(1)->getParentEdges().front()->updateVehicleStackLabels();
            getParentDemandElements().at(1)->getParentEdges().front()->updateVehicleSpreadGeometries();
        } else if (getChildDemandElements().size() > 0) {
            getChildDemandElements().front()->getParentEdges().front()->updateVehicleStackLabels();
            getChildDemandElements().front()->getParentEdges().front()->updateVehicleSpreadGeometries();
        }
    }
}


void
GNEVehicle::toggleAttribute(SumoXMLAttr key, const bool value) {
    // set flow parameters
    setFlowParameters(this, key, value);
}


void
GNEVehicle::setMoveShape(const GNEMoveResult& moveResult) {
    if ((moveResult.newFirstPos != INVALID_DOUBLE) &&
            (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVEFIRST)) {
        // change depart
        departPosProcedure = DepartPosDefinition::GIVEN;
        departPos = moveResult.newFirstPos;
    }
    if ((moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND) ||
            (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVESECOND)) {
        // change arrival
        arrivalPosProcedure = ArrivalPosDefinition::GIVEN;
        arrivalPos = moveResult.newFirstPos;
    }
    // set lateral offset
    myMoveElementLateralOffset = moveResult.firstLaneOffset;
    // update geometry
    updateGeometry();
}


void
GNEVehicle::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // reset lateral offset
    myMoveElementLateralOffset = 0;
    // check departPos
    if (moveResult.newFirstPos != INVALID_DOUBLE) {
        // begin change attribute
        undoList->begin(myTagProperty.getGUIIcon(), "departPos of " + getTagStr());
        // now set departPos
        setAttribute(SUMO_ATTR_DEPARTPOS, toString(moveResult.newFirstPos), undoList);
        // check if depart lane has to be changed
        if (moveResult.newFirstLane) {
            // set new depart lane
            setAttribute(SUMO_ATTR_DEPARTLANE, toString(moveResult.newFirstLane->getIndex()), undoList);
        }
    }
    // check arrivalPos
    if (moveResult.newSecondPos != INVALID_DOUBLE) {
        // begin change attribute
        undoList->begin(myTagProperty.getGUIIcon(), "arrivalPos of " + getTagStr());
        // now set arrivalPos
        setAttribute(SUMO_ATTR_ARRIVALPOS, toString(moveResult.newSecondPos), undoList);
        // check if arrival lane has to be changed
        if (moveResult.newSecondLane) {
            // set new arrival lane
            setAttribute(SUMO_ATTR_ARRIVALLANE, toString(moveResult.newSecondLane->getIndex()), undoList);
        }
    }
    // end change attribute
    undoList->end();
}

/****************************************************************************/
