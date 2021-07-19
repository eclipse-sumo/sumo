/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
// Representation of vehicles in NETEDIT
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
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIBaseVehicleHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>

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
// static defintions
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
    myVehicle->buildPositionCopyEntry(this, false);
    // buld menu commands for names
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + myVehicle->getTagStr() + " name to clipboard").c_str(), nullptr, this, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + myVehicle->getTagStr() + " typed name to clipboard").c_str(), nullptr, this, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(this);
    // build selection and show parameters menu
    myVehicle->getNet()->getViewNet()->buildSelectionACPopupEntry(this, myVehicle);
    myVehicle->buildShowParamsPopupEntry(this);
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
        // Create menu comands for all transform
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
    vehicle->buildPositionCopyEntry(this, false);
    // buld menu commands for names
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + vehicle->getTagStr() + " name to clipboard").c_str(), nullptr, this, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(this, ("Copy " + vehicle->getTagStr() + " typed name to clipboard").c_str(), nullptr, this, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(this);
    // build selection and show parameters menu
    vehicle->getNet()->getViewNet()->buildSelectionACPopupEntry(this, vehicle);
    vehicle->buildShowParamsPopupEntry(this);
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
        // Create menu comands for all transform
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
        // Create menu comands for all transform all vehicles
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

GNEVehicle::GNEVehicle(SumoXMLTag tag, GNENet* net, const std::string& vehicleID, GNEDemandElement* vehicleType, GNEDemandElement* route) :
    GNEDemandElement(vehicleID, net, (tag == GNE_TAG_FLOW_ROUTE) ? GLO_ROUTEFLOW : GLO_VEHICLE, tag,
{}, {}, {}, {}, {}, {}, {vehicleType, route}, {}),
SUMOVehicleParameter() {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleID;
    // set manually vtypeID (needed for saving)
    vtypeid = vehicleType->getID();
}


GNEVehicle::GNEVehicle(GNENet* net, GNEDemandElement* vehicleType, GNEDemandElement* route, const SUMOVehicleParameter& vehicleParameters) :
    GNEDemandElement(vehicleParameters.id, net, (vehicleParameters.tag == GNE_TAG_FLOW_ROUTE) ? GLO_ROUTEFLOW : GLO_VEHICLE, vehicleParameters.tag,
{}, {}, {}, {}, {}, {}, {vehicleType, route}, {}),
SUMOVehicleParameter(vehicleParameters) {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleParameters.id;
    // set manually vtypeID (needed for saving)
    vtypeid = vehicleType->getID();
}


GNEVehicle::GNEVehicle(GNENet* net, GNEDemandElement* vehicleType, const SUMOVehicleParameter& vehicleParameters) :
    GNEDemandElement(vehicleParameters.id, net, (vehicleParameters.tag == GNE_TAG_VEHICLE_WITHROUTE) ? GLO_VEHICLE : GLO_ROUTEFLOW, vehicleParameters.tag,
{}, {}, {}, {}, {}, {}, {vehicleType}, {}),
SUMOVehicleParameter(vehicleParameters) {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleParameters.id;
    // reset routeid
    routeid.clear();
    // set manually vtypeID (needed for saving)
    vtypeid = vehicleType->getID();
}


GNEVehicle::GNEVehicle(SumoXMLTag tag, GNENet* net, const std::string& vehicleID, GNEDemandElement* vehicleType, GNEEdge* fromEdge, GNEEdge* toEdge,
                       const std::vector<GNEEdge*>& via) :
    GNEDemandElement(vehicleID, net, (tag == SUMO_TAG_FLOW) ? GLO_FLOW : GLO_TRIP, tag,
{}, {fromEdge, toEdge}, {}, {}, {}, {}, {vehicleType}, {}),
SUMOVehicleParameter() {
    // set via parameter without updating references
    replaceMiddleParentEdges(toString(via), false);
}


GNEVehicle::GNEVehicle(GNENet* net, GNEDemandElement* vehicleType, GNEEdge* fromEdge, GNEEdge* toEdge, const std::vector<GNEEdge*>& via,
                       const SUMOVehicleParameter& vehicleParameters) :
    GNEDemandElement(vehicleParameters.id, net, (vehicleParameters.tag == SUMO_TAG_FLOW) ? GLO_FLOW : GLO_TRIP, vehicleParameters.tag,
{}, {fromEdge, toEdge}, {}, {}, {}, {}, {vehicleType}, {}),
SUMOVehicleParameter(vehicleParameters) {
    // set via parameter without updating references
    replaceMiddleParentEdges(toString(via), false);
}


GNEVehicle::~GNEVehicle() {}


GNEMoveOperation*
GNEVehicle::getMoveOperation(const double /*shapeOffset*/) {
    // get first and last lanes
    const GNELane* firstLane = getFirstPathLane();
    const GNELane* lastLane = getLastPathLane();
    // get depart and arrival positions (doubles)
    const double departPosDouble = getAttributeDouble(SUMO_ATTR_DEPARTPOS);
    const double arrivalPosDouble = (getAttributeDouble(SUMO_ATTR_ARRIVALPOS) < 0)? lastLane->getLaneShape().length2D() : getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
    // obtain diameter
    const double diameter = getAttributeDouble(SUMO_ATTR_WIDTH) > getAttributeDouble(SUMO_ATTR_LENGTH)? getAttributeDouble(SUMO_ATTR_WIDTH) : getAttributeDouble(SUMO_ATTR_LENGTH);
    // return move operation depending if we're editing departPos or arrivalPos
    if (myNet->getViewNet()->getPositionInformation().distanceSquaredTo(getAttributePosition(SUMO_ATTR_DEPARTPOS)) < (diameter * diameter)) {
        return new GNEMoveOperation(this, firstLane, departPosDouble, lastLane, INVALID_DOUBLE,
                                    myNet->getViewNet()->getViewParent()->getMoveFrame()->getCommonModeOptions()->getAllowChangeLane());
    } else if (myNet->getViewNet()->getPositionInformation().distanceSquaredTo(getAttributePosition(SUMO_ATTR_ARRIVALPOS)) < (myArrivalPositionDiameter * myArrivalPositionDiameter)) {
        return new GNEMoveOperation(this, firstLane, INVALID_DOUBLE, lastLane, arrivalPosDouble,
                                    myNet->getViewNet()->getViewParent()->getMoveFrame()->getCommonModeOptions()->getAllowChangeLane());
    } else {
        return nullptr;
    }
}


std::string
GNEVehicle::getBegin() const {
    // obtain depart
    std::string departStr = time2string(depart);
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
    // attribute VType musn't be written if is DEFAULT_VTYPE_ID
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
    // write specific attribute depeding of tag property
    if (myTagProperty.getTag() == SUMO_TAG_VEHICLE || myTagProperty.getTag() == GNE_TAG_FLOW_ROUTE) {
        // write route
        device.writeAttr(SUMO_ATTR_ROUTE, getParentDemandElements().at(1)->getID());
    }
    // write from, to and edge vias
    if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        // write manually from/to edges (it correspond to fron and back parent edges)
        device.writeAttr(SUMO_ATTR_FROM, getParentEdges().front()->getID());
        device.writeAttr(SUMO_ATTR_TO, getParentEdges().back()->getID());
        // only write via if there isn't empty
        if (via.size() > 0) {
            device.writeAttr(SUMO_ATTR_VIA, via);
        }
    }
    // write specific routeFlow/flow attributes
    if ((myTagProperty.getTag() == GNE_TAG_FLOW_ROUTE) || (myTagProperty.getTag() == GNE_TAG_FLOW_WITHROUTE) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        // write routeFlow values depending if it was set
        if (isAttributeEnabled(SUMO_ATTR_END)) {
            device.writeAttr(SUMO_ATTR_END,  time2string(repetitionEnd));
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
        if (isAttributeEnabled(SUMO_ATTR_PROB)) {
            device.writeAttr(SUMO_ATTR_PROB, repetitionProbability);
        }
    }
    // write parameters
    writeParams(device);
    // write child demand elements associated to this vehicle
    for (const auto& i : getChildDemandElements()) {
        i->writeDemandElement(device);
    }
    // close vehicle tag
    device.closeTag();
}


bool
GNEVehicle::isDemandElementValid() const {
    // only trips or flows can have problems
    if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        // check path
        return myNet->getPathManager()->isPathValid(this);
    } else if (getParentDemandElements().size() == 2) {
        // check if exist a valid path using route parent edges
        if (myNet->getPathManager()->getPathCalculator()->calculateDijkstraPath(getParentDemandElements().at(0)->getVClass(), getParentDemandElements().at(1)->getParentEdges()).size() > 0) {
            return true;
        } else {
            return false;
        }
    } else if (getChildDemandElements().size() > 0 && (getChildDemandElements().front()->getTagProperty().getTag() == GNE_TAG_ROUTE_EMBEDDED)) {
        // check if exist a valid path using embebbed route edges
        if (myNet->getPathManager()->getPathCalculator()->calculateDijkstraPath(getParentDemandElements().at(0)->getVClass(), getChildDemandElements().front()->getParentEdges()).size() > 0) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
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
        // there is connections bewteen all edges, then all ok
        return "";
    } else if (getParentDemandElements().size() == 2) {
        // get route parent edges
        const std::vector<GNEEdge*>& routeEdges = getParentDemandElements().at(1)->getParentEdges();
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)routeEdges.size(); i++) {
            if (myNet->getPathManager()->getPathCalculator()->consecutiveEdgesConnected(getParentDemandElements().at(0)->getVClass(), routeEdges.at((int)i - 1), routeEdges.at(i)) == false) {
                return ("There is no valid path between route edges '" + routeEdges.at((int)i - 1)->getID() + "' and '" + routeEdges.at(i)->getID() + "'");
            }
        }
        // there is connections bewteen all edges, then all ok
        return "";
    } else if (getChildDemandElements().size() > 0 && (getChildDemandElements().front()->getTagProperty().getTag() == GNE_TAG_ROUTE_EMBEDDED)) {
        // get embebbed route edges
        const std::vector<GNEEdge*>& routeEdges = getChildDemandElements().front()->getParentEdges();
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)routeEdges.size(); i++) {
            if (myNet->getPathManager()->getPathCalculator()->consecutiveEdgesConnected(getParentDemandElements().at(0)->getVClass(), routeEdges.at((int)i - 1), routeEdges.at(i)) == false) {
                return ("There is no valid path between embebbed route edges '" + routeEdges.at((int)i - 1)->getID() + "' and '" + routeEdges.at(i)->getID() + "'");
            }
        }
        // there is connections bewteen all edges, then all ok
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
    // get first path lane
    const GNELane* firstPathLane = getFirstPathLane();
    // check path lane
    if (firstPathLane) {
        // declare departPos
        double posOverLane = 0;
        if (canParse<double>(getDepartPos())) {
            posOverLane = parse<double>(getDepartPos());
        }
        // update Geometry
        myDemandElementGeometry.updateGeometry(firstPathLane->getLaneShape(), posOverLane, myMoveElementLateralOffset);
        // compute route embedded vinculated with this vehicle
        for (const auto& demandElement : getChildDemandElements()) {
            if (demandElement->getTagProperty().getTag() == GNE_TAG_ROUTE_EMBEDDED) {
                demandElement->computePathElement();
            }
            demandElement->updateGeometry();
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
        std::vector<GNEDemandElement*> selectedDemandElements = myNet->retrieveDemandElements(true);
        std::vector<GNEVehicle*> selectedVehicles;
        selectedVehicles.reserve(selectedDemandElements.size());
        for (const auto& i : selectedDemandElements) {
            if (i->getTagProperty().isVehicle()) {
                selectedVehicles.push_back(dynamic_cast<GNEVehicle*>(i));
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
        throw ProcessError("Invalid vehicle tag");
    }
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
        const double exaggeration = s.vehicleSize.getExaggeration(s, this);
        const double width = getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_WIDTH);
        const double length = getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_LENGTH);
        const double vehicleSizeSquared = (width * width) * (length * length) * (exaggeration * exaggeration);
        // obtain Position an rotation (depending of draw spread vehicles)
        if (drawSpreadVehicles && mySpreadGeometry.getShape().size() == 0) {
            return;
        }
        const Position vehiclePosition = drawSpreadVehicles ? mySpreadGeometry.getShape().front() : myDemandElementGeometry.getShape().front();
        const double vehicleRotation = drawSpreadVehicles ? mySpreadGeometry.getShapeRotations().front() : myDemandElementGeometry.getShapeRotations().front();
        // check that position is valid
        if (vehiclePosition != Position::INVALID) {
            // first push name
            GLHelper::pushName(getGlID());
            // first check if if mouse is enought near to this vehicle to draw it
            if (s.drawForRectangleSelection && (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(vehiclePosition) >= (vehicleSizeSquared + 2))) {
                // push draw matrix
                GLHelper::pushMatrix();
                // Start with the drawing of the area traslating matrix to origin
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
                // Start with the drawing of the area traslating matrix to origin
                myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
                // translate to drawing position
                glTranslated(vehiclePosition.x(), vehiclePosition.y(), 0);
                glRotated(vehicleRotation, 0, 0, -1);
                // extra translation needed to draw vehicle over edge (to avoid selecting problems)
                glTranslated(0, (-1) * length * exaggeration, 0);
                // set lane color
                setColor(s);
                double upscaleLength = exaggeration;
                if ((exaggeration > 1) && (length > 5)) {
                    // reduce the length/width ratio because this is not usefull at high zoom
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

                    /*
                    switch (s.vehicleQuality) {
                        case 0:
                            GUIBaseVehicleHelper::drawAction_drawVehicleAsTrianglePlus(width, length);
                            break;
                        case 1:
                            GUIBaseVehicleHelper::drawAction_drawVehicleAsBoxPlus(width, length);
                            break;
                        default:
                            GUIBaseVehicleHelper::drawAction_drawVehicleAsPoly(s, shape, width, length);
                            break;
                    }
                    */
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
                    if (s.vehicleName.show && line != "") {
                        glTranslated(0, 0.6 * s.vehicleName.scaledSize(s.scale), 0);
                        GLHelper::drawTextSettings(s.vehicleName, "line:" + line, Position(0, 0), s.scale, s.angle);
                    }
                }
                // pop draw matrix
                GLHelper::popMatrix();
                // draw stack label
                if ((myStackedLabelNumber > 0) && !drawSpreadVehicles) {
                    drawStackLabel(vehiclePosition, vehicleRotation, width, length, exaggeration);
                }
                // draw flow label
                if ((myTagProperty.getTag() == SUMO_TAG_FLOW) || (myTagProperty.getTag() == GNE_TAG_FLOW_ROUTE) || (myTagProperty.getTag() == GNE_TAG_FLOW_WITHROUTE)) {
                    drawFlowLabel(vehiclePosition, vehicleRotation, width, length, exaggeration);
                }
                // check if dotted contours has to be drawn
                if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                    // draw using drawDottedContourClosedShape
                    GNEGeometry::drawDottedSquaredShape(GNEGeometry::DottedContourType::INSPECT, s, vehiclePosition, length * 0.5, width * 0.5, length * -0.5, 0, vehicleRotation, exaggeration);
                }
                if (s.drawDottedContour() || myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                    // draw using drawDottedContourClosedShape
                    GNEGeometry::drawDottedSquaredShape(GNEGeometry::DottedContourType::FRONT, s, vehiclePosition, length * 0.5, width * 0.5, length * -0.5, 0, vehicleRotation, exaggeration);
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
    if ((myTagProperty.getTag() == SUMO_TAG_FLOW) || (myTagProperty.getTag() == SUMO_TAG_TRIP)) {
        // declare lane stops
        std::vector<GNELane*> laneStops;
        // iterate over child demand elements
        for (const auto& demandElement : getChildDemandElements()) {
            // extract lanes
            if (demandElement->getTagProperty().getTag() == SUMO_TAG_STOP_LANE) {
                laneStops.push_back(demandElement->getParentLanes().front());
            } else if (demandElement->getTagProperty().getTag() == SUMO_TAG_STOP_BUSSTOP) {
                laneStops.push_back(demandElement->getParentAdditionals().front()->getParentLanes().front());
            }
        }
        // declare lane vector
        std::vector<GNELane*> lanes;
        // get first and last lanes
        GNELane* firstLane = getFirstPathLane();
        GNELane* lastLane = getLastPathLane();
        // check first and last lanes
        if (firstLane && lastLane) {
            // add first lane
            lanes.push_back(getFirstPathLane());
            // noch check if there are lane Stops
            if (laneStops.size() > 0) {
                // add stop lanes
                for (const auto& laneStop : laneStops) {
                    lanes.push_back(laneStop);
                }
            } else {
                // add via lanes
                for (int i = 1; i < ((int)getParentEdges().size() - 1); i++) {
                    lanes.push_back(getParentEdges().at(i)->getLaneByAllowedVClass(getVClass()));
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
    const bool dottedElement = myNet->getViewNet()->isAttributeCarrierInspected(this) || (myNet->getViewNet()->getFrontAttributeCarrier() == this);
    const bool drawNetworkMode = myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() && 
                                 myNet->getViewNet()->getNetworkViewOptions().showDemandElements() && 
                                 myNet->getViewNet()->getDemandViewOptions().showAllTrips();
    const bool drawDemandMode = myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand() && 
                                myNet->getViewNet()->getDemandViewOptions().showAllTrips();
    // check conditions
    if (!s.drawForRectangleSelection &&
            (drawNetworkMode || drawDemandMode || s.drawDottedContour() || dottedElement || isAttributeCarrierSelected()) &&
            myNet->getPathManager()->getPathDraw()->drawPathGeometry(dottedElement, lane, myTagProperty.getTag())) {
        // calculate width
        const double width = s.vehicleSize.getExaggeration(s, lane) * s.widthSettings.trip;
        // calculate startPos
        const double geometryDepartPos = getAttributeDouble(SUMO_ATTR_DEPARTPOS) + getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_LENGTH);
        // get endPos
        const double geometryEndPos = getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
        // declare path geometry
        GNEGeometry::Geometry vehicleGeometry;
        // check if segment is valid
        if (segment->isValid()) {
            // update pathGeometry depending of first and last segment
            if (segment->isFirstSegment() && segment->isLastSegment()) {
                vehicleGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                               geometryDepartPos, geometryEndPos,      // extrem positions
                                               Position::INVALID, Position::INVALID);  // extra positions
            } else if (segment->isFirstSegment()) {
                vehicleGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                               geometryDepartPos, -1,                  // extrem positions
                                               Position::INVALID, Position::INVALID);  // extra positions
            } else if (segment->isLastSegment()) {
                vehicleGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                               -1, geometryEndPos,                     // extrem positions
                                               Position::INVALID, Position::INVALID);  // extra positions
            } else {
                vehicleGeometry = lane->getLaneGeometry();
            }
        }
        // obtain color
        const RGBColor pathColor = drawUsingSelectColor() ? s.colorSettings.selectedVehicleColor : s.colorSettings.vehicleTrips;
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType() + offsetFront);
        // Set color
        GLHelper::setColor(pathColor);
        // draw geometry
        GNEGeometry::drawGeometry(myNet->getViewNet(), vehicleGeometry, width);
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
            // Start with the drawing of the area traslating matrix to origin
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
        if (segment->isLastSegment()) {
            // get geometryEndPos
            const Position geometryEndPosition = getPathElementArrivalPos();
            // check if endPos can be drawn
            if (!s.drawForRectangleSelection || (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(geometryEndPosition) <= ((myArrivalPositionDiameter * myArrivalPositionDiameter) + 2))) {
                // push draw matrix
                GLHelper::pushMatrix();
                // Start with the drawing of the area traslating matrix to origin
                myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
                // translate to geometryEndPos
                glTranslated(geometryEndPosition.x(), geometryEndPosition.y(), 0);
                // Set person plan color
                GLHelper::setColor(pathColor);
                // resolution of drawn circle depending of the zoom (To improve smothness)
                GLHelper::drawFilledCircle(myArrivalPositionDiameter, s.getCircleResolution());
                // pop draw matrix
                GLHelper::popMatrix();
            }
        }
        // Pop name
        GLHelper::popName();
        // check if shape dotted contour has to be drawn
        if (s.drawDottedContour() || dottedElement) {
            // declare trim geometry to draw
            const GNEGeometry::DottedGeometry pathDottedGeometry((segment->isFirstSegment() || segment->isLastSegment()) ? GNEGeometry::DottedGeometry(s, vehicleGeometry.getShape(), false) : lane->getDottedLaneGeometry());
            // draw inspected dotted contour
            if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::INSPECT, s, pathDottedGeometry, width, segment->isFirstSegment(), segment->isLastSegment());
            }
            // draw front dotted contour
            if (s.drawDottedContour() || (myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::FRONT, s, pathDottedGeometry, width, segment->isFirstSegment(), segment->isLastSegment());
            }
        }
    }
}


void
GNEVehicle::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* /*segment*/, const double offsetFront) const {
    // get flags
    const bool dottedElement = myNet->getViewNet()->isAttributeCarrierInspected(this) || (myNet->getViewNet()->getFrontAttributeCarrier() == this);
    const bool drawNetworkMode = myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() && 
                                 myNet->getViewNet()->getNetworkViewOptions().showDemandElements() && 
                                 myNet->getViewNet()->getDemandViewOptions().showAllTrips();
    const bool drawDemandMode = myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand() && 
                                myNet->getViewNet()->getDemandViewOptions().showAllTrips();
    // check conditions
    if (!s.drawForRectangleSelection &&
            fromLane->getLane2laneConnections().exist(toLane) &&
            (drawNetworkMode || drawDemandMode || s.drawDottedContour() || dottedElement || isAttributeCarrierSelected()) &&
            myNet->getPathManager()->getPathDraw()->drawPathGeometry(dottedElement, fromLane, toLane, myTagProperty.getTag())) {
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // obtain lane2lane geometry
        const GNEGeometry::Geometry& lane2laneGeometry = fromLane->getLane2laneConnections().getLane2laneGeometry(toLane);
        // calculate width
        const double width = s.vehicleSize.getExaggeration(s, fromLane) * s.widthSettings.trip;
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType() + offsetFront);
        // Set color of the base
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectedVehicleColor);
        } else {
            GLHelper::setColor(s.colorSettings.vehicleTrips);
        }
        // draw lane2lane
        GNEGeometry::drawGeometry(myNet->getViewNet(), lane2laneGeometry, width);
        // Pop last matrix
        GLHelper::popMatrix();
        // check if shape dotted contour has to be drawn
        if (s.drawDottedContour() || dottedElement) {
            // check if exist lane2lane connection
            if (fromLane->getLane2laneConnections().exist(toLane)) {
                // draw inspected dotted contour
                if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                    GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::INSPECT, s, fromLane->getLane2laneConnections().getLane2laneDottedGeometry(toLane), width, false, false);
                }
                // draw front dotted contour
                if (s.drawDottedContour() || (myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                    GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::FRONT, s, fromLane->getLane2laneConnections().getLane2laneDottedGeometry(toLane), width, false, false);
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
    } else if ((myTagProperty.getTag() == GNE_TAG_VEHICLE_WITHROUTE) || (myTagProperty.getTag() == GNE_TAG_FLOW_WITHROUTE)) {
        // check if embebbed route exist (due during loading embedded route doesn't exist
        if (getChildDemandElements().empty()) {
            return nullptr;
        }
        // check departEdge
        if ((departEdge > 0) && (departEdge < (int)getChildDemandElements().front()->getParentEdges().size())) {
            // use depart edge
            firstEdge = getChildDemandElements().front()->getParentEdges().at(departEdge);
        } else {
            // use first embedded route edge
            firstEdge = getChildDemandElements().front()->getParentEdges().front();
        }
    } else {
        // use first parent edge
        firstEdge = getParentEdges().front();
    }
    // get departLane index
    const int departLaneIndex = canParse<int>(getAttribute(SUMO_ATTR_DEPARTLANE)) ? parse<int>(getAttribute(SUMO_ATTR_DEPARTLANE)) : -1;
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
    } else if ((myTagProperty.getTag() == GNE_TAG_VEHICLE_WITHROUTE) || (myTagProperty.getTag() == GNE_TAG_FLOW_WITHROUTE)) {
        // check if embebbed route exist (due during loading embedded route doesn't exist)
        if (getChildDemandElements().empty()) {
            return nullptr;
        }
        // check arrivalEdge
        if ((arrivalEdge > 0) && (arrivalEdge < (int)getChildDemandElements().front()->getParentEdges().size())) {
            // use arrival edge
            lastEdge = getChildDemandElements().front()->getParentEdges().at(arrivalEdge);
        } else {
            // use last route edge
            lastEdge = getChildDemandElements().front()->getParentEdges().back();
        }
    } else {
        // use last parent edge
        lastEdge = getParentEdges().back();
    }
    // get arrivalLane index
    const int arrivalLaneIndex = canParse<int>(getAttribute(SUMO_ATTR_ARRIVALLANE)) ? parse<int>(getAttribute(SUMO_ATTR_ARRIVALLANE)) : -1;
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
            return getID();
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
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
            return time2string(depart);
        case SUMO_ATTR_ROUTE:
            if (getParentDemandElements().size() == 2) {
                return getParentDemandElements().at(1)->getID();
            } else {
                return "";
            }
        // Specific of Trips
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
        // Specific of routeFlows
        case SUMO_ATTR_BEGIN:
            return time2string(depart);
        case SUMO_ATTR_END:
            return time2string(repetitionEnd);
        case SUMO_ATTR_VEHSPERHOUR:
            return toString(3600 / STEPS2TIME(repetitionOffset));
        case SUMO_ATTR_PERIOD:
            return time2string(repetitionOffset);
        case SUMO_ATTR_PROB:
            return toString(repetitionProbability);
        case SUMO_ATTR_NUMBER:
            return toString(repetitionNumber);
        // other
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
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
        case SUMO_ATTR_DEPARTPOS:
            // only return departPos it if is given
            if (departPosProcedure == DepartPosDefinition::GIVEN) {
                return departPos;
            } else {
                return 0;
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
            const PositionVector &laneShape = getFirstPathLane()->getLaneShape();
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
            const PositionVector &laneShape = getLastPathLane()->getLaneShape();
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
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_ROUTE:
        // Specific of Trips
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case SUMO_ATTR_VIA:
        case SUMO_ATTR_DEPARTEDGE:
        case SUMO_ATTR_ARRIVALEDGE:
        // Specific of routeFlows
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_PROB:
        // other
        case GNE_ATTR_PARAMETERS:
        case GNE_ATTR_SELECTED:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
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
                    (myNet->retrieveDemandElement(SUMO_TAG_VEHICLE, value, false) == nullptr) &&
                    (myNet->retrieveDemandElement(SUMO_TAG_TRIP, value, false) == nullptr) &&
                    (myNet->retrieveDemandElement(GNE_TAG_FLOW_ROUTE, value, false) == nullptr) &&
                    (myNet->retrieveDemandElement(SUMO_TAG_FLOW, value, false) == nullptr)) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_TYPE:
            return SUMOXMLDefinitions::isValidTypeID(value) && (myNet->retrieveDemandElement(SUMO_TAG_VTYPE, value, false) != nullptr);
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
            return error.empty();
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
        // Specific of vehicles
        case SUMO_ATTR_DEPART: {
            SUMOTime dummyDepart;
            DepartDefinition dummyDepartProcedure;
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, dummyDepart, dummyDepartProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ROUTE:
            if (getParentDemandElements().size() == 2) {
                return SUMOXMLDefinitions::isValidVehicleID(value) && (myNet->retrieveDemandElement(SUMO_TAG_ROUTE, value, false) != nullptr);
            } else {
                return true;
            }
        // Specific of Trips
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->retrieveEdge(value, false) != nullptr);
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
        // Specific of routeFlows
        case SUMO_ATTR_BEGIN:
            if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_END:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_VEHSPERHOUR:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) > 0);
            } else {
                return false;
            }
        case SUMO_ATTR_PERIOD:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) > 0);
            } else {
                return false;
            }
        case SUMO_ATTR_PROB:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
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
    // obtain a copy of parameter sets
    int newParametersSet = parametersSet;
    // modify newParametersSet
    GNERouteHandler::setFlowParameters(key, newParametersSet);
    // add GNEChange_EnableAttribute
    undoList->add(new GNEChange_EnableAttribute(this, parametersSet, newParametersSet), true);
}


void
GNEVehicle::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    // nothing to disable
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


const std::map<std::string, std::string>&
GNEVehicle::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEVehicle::setColor(const GUIVisualizationSettings& s) const {
    // change color
    if (drawUsingSelectColor()) {
        GLHelper::setColor(s.colorSettings.selectedVehicleColor);
    } else {
        // obtain vehicle color
        const GUIColorer& c = s.vehicleColorer;
        // set color depending of vehicle color active
        switch (c.getActive()) {
            case 0: {
                // test for emergency vehicle
                if (getParentDemandElements().at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "emergency") {
                    GLHelper::setColor(RGBColor::WHITE);
                    break;
                }
                // test for firebrigade
                if (getParentDemandElements().at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "firebrigade") {
                    GLHelper::setColor(RGBColor::RED);
                    break;
                }
                // test for police car
                if (getParentDemandElements().at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "police") {
                    GLHelper::setColor(RGBColor::BLUE);
                    break;
                }
                // check if color was set
                if (wasSet(VEHPARS_COLOR_SET)) {
                    GLHelper::setColor(color);
                    break;
                } else {
                    // take their parent's color)
                    GLHelper::setColor(getParentDemandElements().at(0)->getColor());
                    break;
                }
            }
            case 2: {
                if (wasSet(VEHPARS_COLOR_SET)) {
                    GLHelper::setColor(color);
                } else {
                    GLHelper::setColor(c.getScheme().getColor(0));
                }
                break;
            }
            case 3: {
                if (getParentDemandElements().at(0)->isAttributeEnabled(SUMO_ATTR_COLOR)) {
                    GLHelper::setColor(getParentDemandElements().at(0)->getColor());
                } else {
                    GLHelper::setColor(c.getScheme().getColor(0));
                }
                break;
            }
            case 4: {
                if (getParentDemandElements().at(1)->getColor() != RGBColor::DEFAULT_COLOR) {
                    GLHelper::setColor(getParentDemandElements().at(1)->getColor());
                } else {
                    GLHelper::setColor(c.getScheme().getColor(0));
                }
                break;
            }
            case 5: {
                Position p = getParentDemandElements().at(1)->getParentEdges().at(0)->getLanes().at(0)->getLaneShape()[0];
                const Boundary& b = myNet->getBoundary();
                Position center = b.getCenter();
                double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
                double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
                GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
                break;
            }
            case 6: {
                Position p = getParentDemandElements().at(1)->getParentEdges().back()->getLanes().at(0)->getLaneShape()[-1];
                const Boundary& b = myNet->getBoundary();
                Position center = b.getCenter();
                double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
                double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
                GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
                break;
            }
            case 7: {
                Position pb = getParentDemandElements().at(1)->getParentEdges().at(0)->getLanes().at(0)->getLaneShape()[0];
                Position pe = getParentDemandElements().at(1)->getParentEdges().back()->getLanes().at(0)->getLaneShape()[-1];
                const Boundary& b = myNet->getBoundary();
                double hue = 180. + atan2(pb.x() - pe.x(), pb.y() - pe.y()) * 180. / M_PI;
                Position minp(b.xmin(), b.ymin());
                Position maxp(b.xmax(), b.ymax());
                double sat = pb.distanceTo(pe) / minp.distanceTo(maxp);
                GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
                break;
            }
            case 29: { // color randomly (by pointer hash)
                std::hash<const GNEVehicle*> ptr_hash;
                const double hue = (double)(ptr_hash(this) % 360); // [0-360]
                const double sat = ((ptr_hash(this) / 360) % 67) / 100.0 + 0.33; // [0.33-1]
                GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
                break;
            }
            default: {
                GLHelper::setColor(c.getScheme().getColor(0));
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
            myNet->getAttributeCarriers()->updateID(this, value);
            // set manually vehicle ID (needed for saving)
            id = value;
            // Change IDs of all person plans children (stops, embedded routes...)
            for (const auto& childDemandElement : getChildDemandElements()) {
                childDemandElement->setMicrosimID(getID());
            }
            break;
        case SUMO_ATTR_TYPE:
            replaceDemandElementParent(SUMO_TAG_VTYPE, value, 0);
            // set manually vtypeID (needed for saving)
            vtypeid = value;
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
            updateGeometry();
            updateSpreadStackGeometry = true;
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
            updateGeometry();
            updateSpreadStackGeometry = true;
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
        // Specific of vehicles
        case SUMO_ATTR_DEPART: {
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
        // Specific of Trips and flow
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
            // update via
            replaceMiddleParentEdges(value, true);
            // compute vehicle
            computePathElement();
            updateSpreadStackGeometry = true;
            break;
        }
        case SUMO_ATTR_DEPARTEDGE: {
            // update depart edge
            if (value.empty()) {
                departEdge = -1;
            } else {
                departEdge = parse<int>(value);
            }
            // compute vehicle
            computePathElement();
            updateSpreadStackGeometry = true;
            break;
        }
        case SUMO_ATTR_ARRIVALEDGE: {
            // update arrival edge
            if (value.empty()) {
                arrivalEdge = 0;
            } else {
                arrivalEdge = parse<int>(value);
            }
            // compute vehicle
            computePathElement();
            updateSpreadStackGeometry = true;
            break;
        }
        // Specific of routeFlows
        case SUMO_ATTR_BEGIN: {
            depart = string2time(value);
            break;
        }
        case SUMO_ATTR_END:
            repetitionEnd = string2time(value);
            break;
        case SUMO_ATTR_VEHSPERHOUR:
            repetitionOffset = TIME2STEPS(3600 / parse<double>(value));
            break;
        case SUMO_ATTR_PERIOD:
            repetitionOffset = string2time(value);
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
GNEVehicle::setEnabledAttribute(const int enabledAttributes) {
    parametersSet = enabledAttributes;
}


void
GNEVehicle::setMoveShape(const GNEMoveResult& moveResult) {
    // check departPos
    if (moveResult.newFirstPos != INVALID_DOUBLE) {
        departPosProcedure = DepartPosDefinition::GIVEN;
        departPos = moveResult.newFirstPos;
    }
    // check arrivalPos
    if (moveResult.newSecondPos != INVALID_DOUBLE) {
        arrivalPosProcedure = ArrivalPosDefinition::GIVEN;
        arrivalPos = moveResult.newSecondPos;
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
        undoList->p_begin("departPos of " + getTagStr());
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
        undoList->p_begin("arrivalPos of " + getTagStr());
        // now set arrivalPos
        setAttribute(SUMO_ATTR_ARRIVALPOS, toString(moveResult.newSecondPos), undoList);
        // check if arrival lane has to be changed
        if (moveResult.newSecondLane) {
            // set new arrival lane
            setAttribute(SUMO_ATTR_ARRIVALLANE, toString(moveResult.newSecondLane->getIndex()), undoList);
        }
    }
    // end change attribute
    undoList->p_end();
}


void
GNEVehicle::drawStackLabel(const Position& vehiclePosition, const double vehicleRotation, const double width, const double length, const double exaggeration) const {
    // declare contour width
    const double contourWidth = (0.05 * exaggeration);
    // Push matrix
    GLHelper::pushMatrix();
    // Traslate to vehicle top
    glTranslated(vehiclePosition.x(), vehiclePosition.y(), GLO_ROUTE + getType() + 0.1 + GLO_PERSONFLOW);
    glRotated(vehicleRotation, 0, 0, -1);
    glTranslated((width * exaggeration * 0.5) + (0.35 * exaggeration), 0, 0);
    // draw external box
    GLHelper::setColor(RGBColor::GREY);
    GLHelper::drawBoxLine(Position(), 0, (length * exaggeration), 0.3 * exaggeration);
    // draw internal box
    glTranslated(0, 0, 0.1);
    GLHelper::setColor(RGBColor(0, 128, 0));
    GLHelper::drawBoxLine(Position(0, -contourWidth), Position(0, -contourWidth), 0, (length * exaggeration) - (contourWidth * 2), (0.3 * exaggeration) - contourWidth);
    // draw stack label
    GLHelper::drawText("vehicles stacked: " + toString(myStackedLabelNumber), Position(0, length * exaggeration * -0.5), (.1 * exaggeration), (0.6 * exaggeration), RGBColor::WHITE, 90, 0, -1);
    // pop draw matrix
    GLHelper::popMatrix();
}


void
GNEVehicle::drawFlowLabel(const Position& vehiclePosition, const double vehicleRotation, const double width, const double length, const double exaggeration) const {
    // declare contour width
    const double contourWidth = (0.05 * exaggeration);
    // Push matrix
    GLHelper::pushMatrix();
    // Traslate to vehicle bot
    glTranslated(vehiclePosition.x(), vehiclePosition.y(), GLO_ROUTE + getType() + 0.1 + GLO_PERSONFLOW);
    glRotated(vehicleRotation, 0, 0, -1);
    glTranslated(-1 * ((width * 0.5 * exaggeration) + (0.35 * exaggeration)), 0, 0);
    // draw external box
    GLHelper::setColor(RGBColor::GREY);
    GLHelper::drawBoxLine(Position(), Position(), 0, (length * exaggeration), 0.3 * exaggeration);
    // draw internal box
    glTranslated(0, 0, 0.1);
    GLHelper::setColor(RGBColor::CYAN);
    GLHelper::drawBoxLine(Position(0, -contourWidth), Position(0, -contourWidth), 0, (length * exaggeration) - (contourWidth * 2), (0.3 * exaggeration) - contourWidth);
    // draw stack label
    GLHelper::drawText("Flow", Position(0, length * exaggeration * -0.5), (.1 * exaggeration), (0.6 * exaggeration), RGBColor::BLACK, 90, 0, -1);
    // pop draw matrix
    GLHelper::popMatrix();
}


/****************************************************************************/
