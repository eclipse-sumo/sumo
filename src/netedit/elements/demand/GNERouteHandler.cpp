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
/// @file    GNERouteHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
///
// Builds demand objects for netedit
/****************************************************************************/
#include <config.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_DemandElement.h>

#include "GNEContainer.h"
#include "GNEStopContainer.h"
#include "GNEPerson.h"
#include "GNEStopPerson.h"
#include "GNEPersonTrip.h"
#include "GNERide.h"
#include "GNERoute.h"
#include "GNERouteHandler.h"
#include "GNEStop.h"
#include "GNETranship.h"
#include "GNETransport.h"
#include "GNEVehicle.h"
#include "GNEVehicleType.h"
#include "GNEWalk.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteHandler::GNERouteHandler(const std::string& file, GNENet* net, bool undoDemandElements) :
    RouteHandler(file),
    myNet(net),
    myUndoDemandElements(undoDemandElements) {
}


GNERouteHandler::~GNERouteHandler() {}


void 
GNERouteHandler::buildVType(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVTypeParameter& vTypeParameter,
    const std::map<std::string, std::string> &parameters) {
    // first check if we're creating a vType or a pType
    SumoXMLTag vTypeTag = (vTypeParameter.vehicleClass == SVC_PEDESTRIAN) ? SUMO_TAG_PTYPE : SUMO_TAG_VTYPE;
    // check if loaded vType/pType is a default vtype
    if ((vTypeParameter.id == DEFAULT_VTYPE_ID) || (vTypeParameter.id == DEFAULT_PEDTYPE_ID) || (vTypeParameter.id == DEFAULT_BIKETYPE_ID)) {
        // overwrite default vehicle type
        GNEVehicleType::overwriteVType(myNet->retrieveDemandElement(vTypeTag, vTypeParameter.id, false), vTypeParameter, myNet->getViewNet()->getUndoList());
    } else if (myNet->retrieveDemandElement(vTypeTag, vTypeParameter.id, false) != nullptr) {
        WRITE_ERROR("There is another " + toString(vTypeTag) + " with the same ID='" + vTypeParameter.id + "'.");
    } else {
        // create vType/pType using myCurrentVType
        GNEDemandElement* vType = new GNEVehicleType(myNet, vTypeParameter, vTypeTag);
        if (myUndoDemandElements) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + vType->getTagStr());
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(vType, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(vType);
            vType->incRef("buildVType");
        }
    }
}


void 
GNERouteHandler::buildVTypeDistribution(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string &/*id*/) {
    // unsuported
    WRITE_ERROR("NETEDIT doesn't support vType distributions");
}


void 
GNERouteHandler::buildRoute(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string &id, SUMOVehicleClass vClass, 
    const std::vector<std::string> &edgeIDs, const RGBColor &color, const int repeat, const SUMOTime cycleTime, 
    const std::map<std::string, std::string> &parameters) {
    // parse edges
    const auto edges = parseEdges(SUMO_TAG_ROUTE, edgeIDs);
    // check edges
    if (edges.size() > 0) {
        // create GNERoute
        GNEDemandElement* route = new GNERoute(myNet, id, vClass, edges, color, repeat, cycleTime, parameters);
        if (myUndoDemandElements) {
            myNet->getViewNet()->getUndoList()->p_begin("add " + route->getTagStr());
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(route);
            for (const auto& edge : edges) {
                edge->addChildElement(route);
            }
            route->incRef("buildRoute");
        }
    }
}


void 
GNERouteHandler::buildEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::vector<std::string> &edgeIDs, 
    const RGBColor &color, const int repeat, const SUMOTime cycleTime, const std::map<std::string, std::string> &parameters) {
    // first create vehicle/flow
    const SUMOVehicleParameter& vehicleParameters = sumoBaseObject->getParentSumoBaseObject()->getVehicleParameter();
    const SumoXMLTag vehicleTag = sumoBaseObject->getParentSumoBaseObject()->getTag() == SUMO_TAG_VEHICLE? GNE_TAG_VEHICLE_WITHROUTE : GNE_TAG_FLOW_WITHROUTE;
    // parse route edges
    const auto edges = parseEdges(SUMO_TAG_ROUTE, edgeIDs);
    // check if ID is duplicated
    if ((edges.size() > 0) && !isVehicleIdDuplicated(myNet, vehicleParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* vType = myNet->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN) && ((int)edges.front()->getLanes().size() < vehicleParameters.departLane)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTLANE) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTSPEED) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // create vehicle using vehicleParameters
            GNEDemandElement* vehicle = new GNEVehicle(vehicleTag, myNet, vType, vehicleParameters);
            // create embedded route
            GNEDemandElement* route = new GNERoute(myNet, vehicle, edges, color, repeat, cycleTime, parameters);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + route->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(vehicle, true), true);
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(vehicle);
                myNet->getAttributeCarriers()->insertDemandElement(route);
                vType->addChildElement(vehicle);
                vehicle->addChildElement(route);
                for (const auto& edge : edges) {
                    edge->addChildElement(route);
                }
                route->incRef("buildRoute");
            }

        }
    }
}


void 
GNERouteHandler::buildRouteDistribution(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string &/*id*/) {
    // unsuported
    WRITE_ERROR("NETEDIT doesn't support route distributions");
}


void 
GNERouteHandler::buildVehicleOverRoute(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& vehicleParameters,
    const std::map<std::string, std::string> &parameters) {
    // first check if ID is duplicated
    if (!isVehicleIdDuplicated(myNet, vehicleParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* vType = myNet->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        GNEDemandElement* route = myNet->retrieveDemandElement(SUMO_TAG_ROUTE, vehicleParameters.routeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (route == nullptr) {
            WRITE_ERROR("Invalid route '" + vehicleParameters.routeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN) && ((int)route->getParentEdges().front()->getLanes().size() < vehicleParameters.departLane)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTLANE) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTSPEED) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // create vehicle using vehicleParameters
            GNEDemandElement* vehicle = new GNEVehicle(SUMO_TAG_VEHICLE, myNet, vType, route, vehicleParameters);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + vehicle->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(vehicle, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(vehicle);
                // set vehicle as child of vType and Route
                vType->addChildElement(vehicle);
                route->addChildElement(vehicle);
                vehicle->incRef("buildVehicleOverRoute");
            }
            // center view after creation
            if (!myNet->getViewNet()->getVisibleBoundary().around(vehicle->getPositionInView())) {
                myNet->getViewNet()->centerTo(vehicle->getPositionInView(), false);
            }
        }
    }
}


void 
GNERouteHandler::buildFlowOverRoute(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& vehicleParameters,
    const std::map<std::string, std::string> &parameters) {
    // first check if ID is duplicated
    if (!isVehicleIdDuplicated(myNet, vehicleParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* vType = myNet->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        GNEDemandElement* route = myNet->retrieveDemandElement(SUMO_TAG_ROUTE, vehicleParameters.routeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (route == nullptr) {
            WRITE_ERROR("Invalid route '" + vehicleParameters.routeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN) && ((int)route->getParentEdges().front()->getLanes().size() < vehicleParameters.departLane)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTLANE) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTSPEED) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // create flow or trips using vehicleParameters
            GNEDemandElement* flow = new GNEVehicle(GNE_TAG_FLOW_ROUTE, myNet, vType, route, vehicleParameters);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + flow->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(flow);
                // set flow as child of vType and Route
                vType->addChildElement(flow);
                route->addChildElement(flow);
                flow->incRef("buildFlowOverRoute");
            }
            // center view after creation
            if (!myNet->getViewNet()->getVisibleBoundary().around(flow->getPositionInView())) {
                myNet->getViewNet()->centerTo(flow->getPositionInView(), false);
            }
        }
    }
}


void
GNERouteHandler::buildTrip(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& vehicleParameters, 
    const std::string &fromEdgeID, const std::string &toEdgeID, const std::vector<std::string>& viaIDs, const std::map<std::string, std::string> &parameters) {
    // parse edges
    const auto fromEdge = parseEdge(SUMO_TAG_TRIP, fromEdgeID);
    const auto toEdge = parseEdge(SUMO_TAG_TRIP, toEdgeID);
    const auto via = parseEdges(SUMO_TAG_TRIP, viaIDs);
    // check if exist another vehicle with the same ID (note: Vehicles, Flows and Trips share namespace)
    if (fromEdge && toEdge && !isVehicleIdDuplicated(myNet, vehicleParameters.id)) {
        // obtain  vtypes
        GNEDemandElement* vType = myNet->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && ((vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN)) && ((int)fromEdge->getLanes().size() < vehicleParameters.departLane)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTLANE) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTSPEED) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // add "via" edges in vehicleParameters
            for (const auto& viaEdge : via) {
                vehicleParameters.via.push_back(viaEdge->getID());
            }
            // create trip or flow using tripParameters
            GNEDemandElement* trip = new GNEVehicle(SUMO_TAG_TRIP, myNet, vType, fromEdge, toEdge, via, vehicleParameters);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + trip->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(trip, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(trip);
                // set vehicle as child of vType
                vType->addChildElement(trip);
                trip->incRef("buildTrip");
                // add reference in all edges
                fromEdge->addChildElement(trip);
                toEdge->addChildElement(trip);
                for (const auto& viaEdge : via) {
                    viaEdge->addChildElement(trip);
                }
            }
            // center view after creation
            if (!myNet->getViewNet()->getVisibleBoundary().around(trip->getPositionInView())) {
                myNet->getViewNet()->centerTo(trip->getPositionInView(), false);
            }
        }
    }
}


void 
GNERouteHandler::buildFlow(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& vehicleParameters, 
    const std::string &fromEdgeID, const std::string &toEdgeID, const std::vector<std::string>& viaIDs, const std::map<std::string, std::string> &parameters) {
    // parse edges
    const auto fromEdge = parseEdge(SUMO_TAG_TRIP, fromEdgeID);
    const auto toEdge = parseEdge(SUMO_TAG_TRIP, toEdgeID);
    const auto via = parseEdges(SUMO_TAG_TRIP, viaIDs);
    // check if exist another vehicle with the same ID (note: Vehicles, Flows and Trips share namespace)
    if (fromEdge && toEdge && !isVehicleIdDuplicated(myNet, vehicleParameters.id)) {
        // obtain  vtypes
        GNEDemandElement* vType = myNet->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN) && ((int)fromEdge->getLanes().size() < vehicleParameters.departLane)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTLANE) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTSPEED) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // add "via" edges in vehicleParameters
            for (const auto& viaEdge : via) {
                vehicleParameters.via.push_back(viaEdge->getID());
            }
            // create trip or flow using tripParameters
            GNEDemandElement* flow = new GNEVehicle(SUMO_TAG_FLOW, myNet, vType, fromEdge, toEdge, via, vehicleParameters);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + flow->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(flow);
                // set vehicle as child of vType
                vType->addChildElement(flow);
                flow->incRef("buildFlow");
                // add reference in all edges
                fromEdge->addChildElement(flow);
                toEdge->addChildElement(flow);
                for (const auto& viaEdge : via) {
                    viaEdge->addChildElement(flow);
                }
            }
            // center view after creation
            if (!myNet->getViewNet()->getVisibleBoundary().around(flow->getPositionInView())) {
                myNet->getViewNet()->centerTo(flow->getPositionInView(), false);
            }
        }
    }
}

/*
void
GNERouteHandler::buildStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter::Stop& stopParameters) {
    // get stop parent
    GNEDemandElement *stopParent = myNet->retrieveDemandElement(sumoBaseObject->getParentSumoBaseObject()->getTag(), sumoBaseObject->getStringAttribute(SUMO_ATTR_ID));
    // declare pointers to parent elements
    GNEAdditional* stoppingPlace = nullptr;
    GNELane* lane = nullptr;
    // GNEEdge* edge = nullptr;
    SumoXMLTag stopTagType = SUMO_TAG_NOTHING;
    bool validParentDemandElement = true;
    if (stopParameters.busstop.size() > 0) {
        stoppingPlace = myNet->retrieveAdditional(SUMO_TAG_BUS_STOP, stopParameters.busstop, false);
        // distinguish between stop for vehicles and stops for persons
        if (stopParent->getTagProperty().isPerson()) {
            stopTagType = GNE_TAG_STOPPERSON_BUSSTOP;
        } else {
            stopTagType = SUMO_TAG_STOP_BUSSTOP;
        }
    } else if (stopParameters.containerstop.size() > 0) {
        stoppingPlace = myNet->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, stopParameters.containerstop, false);
        // distinguish between stop for vehicles and stops for persons
        if (stopParent->getTagProperty().isPerson()) {
            WRITE_ERROR("Persons don't support stops at container stops");
            validParentDemandElement = false;
        } else {
            stopTagType = SUMO_TAG_STOP_CONTAINERSTOP;
        }
    } else if (stopParameters.chargingStation.size() > 0) {
        stoppingPlace = myNet->retrieveAdditional(SUMO_TAG_CHARGING_STATION, stopParameters.chargingStation, false);
        // distinguish between stop for vehicles and stops for persons
        if (stopParent->getTagProperty().isPerson()) {
            WRITE_ERROR("Persons don't support stops at charging stations");
            validParentDemandElement = false;
        } else {
            stopTagType = SUMO_TAG_STOP_CHARGINGSTATION;
        }
    } else if (stopParameters.parkingarea.size() > 0) {
        stoppingPlace = myNet->retrieveAdditional(SUMO_TAG_PARKING_AREA, stopParameters.parkingarea, false);
        // distinguish between stop for vehicles and stops for persons
        if (stopParent->getTagProperty().isPerson()) {
            WRITE_ERROR("Persons don't support stops at parking areas");
            validParentDemandElement = false;
        } else {
            stopTagType = SUMO_TAG_STOP_PARKINGAREA;
        }
    } else if (stopParameters.lane.size() > 0) {
        lane = myNet->retrieveLane(stopParameters.lane, false);
        stopTagType = SUMO_TAG_STOP_LANE;
    }
    // first check that parent is valid
    if (validParentDemandElement) {
        // check if values are correct
        if (stoppingPlace && lane) {
            WRITE_ERROR("A stop must be defined either over a stoppingPlace or over a lane");
        } else if (!stoppingPlace && !lane) {
            WRITE_ERROR("A stop requires a stoppingPlace or a lane");
        } else if (stoppingPlace) {
            // create stop using stopParameters and stoppingPlace
            GNEDemandElement* stop = new GNEStop(stopTagType, myNet, stopParameters, stoppingPlace, stopParent);
            // add it depending of undoDemandElements
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + stop->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(stop, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(stop);
                stoppingPlace->addChildElement(stop);
                stopParent->addChildElement(stop);
                stop->incRef("buildStoppingPlaceStop");
            }
        } else {
            // create stop using stopParameters and lane
            GNEDemandElement* stop = new GNEStop(myNet, stopParameters, lane, stopParent);
            // add it depending of undoDemandElements
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + stop->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(stop, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(stop);
                lane->addChildElement(stop);
                stopParent->addChildElement(stop);
                stop->incRef("buildLaneStop");
            }
        }
    }
}
*/

void 
GNERouteHandler::buildPerson(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& personParameters,
    const std::map<std::string, std::string> &parameters) {
    // first check if ID is duplicated
    if (!isPersonIdDuplicated(myNet, personParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* pType = myNet->retrieveDemandElement(SUMO_TAG_PTYPE, personParameters.vtypeid, false);
        if (pType == nullptr) {
            WRITE_ERROR("Invalid person type '" + personParameters.vtypeid + "' used in " + toString(personParameters.tag) + " '" + personParameters.id + "'.");
        } else {
            // create person using personParameters
            GNEDemandElement* person = new GNEPerson(SUMO_TAG_PERSON, myNet, pType, personParameters);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + person->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(person, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(person);
                // set person as child of pType and Route
                pType->addChildElement(person);
                person->incRef("buildPerson");
            }
        }
    }
}


void
GNERouteHandler::buildPersonFlow(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& personFlowParameters,
    const std::map<std::string, std::string> &parameters) {
    // first check if ID is duplicated
    if (!isPersonIdDuplicated(myNet, personFlowParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* pType = myNet->retrieveDemandElement(SUMO_TAG_PTYPE, personFlowParameters.vtypeid, false);
        if (pType == nullptr) {
            WRITE_ERROR("Invalid personFlow type '" + personFlowParameters.vtypeid + "' used in " + toString(personFlowParameters.tag) + " '" + personFlowParameters.id + "'.");
        } else {
            // create personFlow using personFlowParameters
            GNEDemandElement* personFlow = new GNEPerson(SUMO_TAG_PERSONFLOW, myNet, pType, personFlowParameters);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + personFlow->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(personFlow, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(personFlow);
                // set personFlow as child of pType and Route
                pType->addChildElement(personFlow);
                personFlow->incRef("buildPersonFlow");
            }
        }
    }
}


void
GNERouteHandler::buildPersonTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdgeID, const std::string &toEdgeID,
    const std::string &toBusStopID, double arrivalPos, const std::vector<std::string>& types, const std::vector<std::string>& modes) {
    // first parse parents
    GNEDemandElement* personParent = getPersonParent(sumoBaseObject);
    GNEEdge* fromEdge = fromEdgeID.empty()? getPreviousPersonPlanEdge(sumoBaseObject) : myNet->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->retrieveEdge(toEdgeID, false);
    GNEAdditional* toBusStop = myNet->retrieveAdditional(SUMO_TAG_BUS_STOP, toBusStopID, false);
    // check conditions
    if (personParent && fromEdge) {
        if (toEdge) {
            // create personTrip from->to
            GNEDemandElement *personTrip = new GNEPersonTrip(myNet, personParent, fromEdge, toEdge, arrivalPos, types, modes);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + personTrip->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(personTrip, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(personTrip);
                // set child references
                personParent->addChildElement(personTrip);
                fromEdge->addChildElement(personTrip);
                toEdge->addChildElement(personTrip);
                personTrip->incRef("buildPersonTripFromTo");
            }
        } else if (toBusStop) {
            // create personTrip from->busStop
            GNEDemandElement *personTrip = new GNEPersonTrip(myNet, personParent, fromEdge, toBusStop, arrivalPos, types, modes);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + personTrip->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(personTrip, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(personTrip);
                // set child references
                personParent->addChildElement(personTrip);
                fromEdge->addChildElement(personTrip);
                toBusStop->addChildElement(personTrip);
                personTrip->incRef("buildPersonTripFromBusStop");
            }
        }
    }
}


void
GNERouteHandler::buildWalk(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdgeID, const std::string &toEdgeID,
    const std::string &toBusStopID, const std::vector<std::string>& edgeIDs, const std::string &routeID, double arrivalPos) {
    // first parse parents
    GNEDemandElement* personParent = getPersonParent(sumoBaseObject);
    GNEEdge* fromEdge = fromEdgeID.empty()? getPreviousPersonPlanEdge(sumoBaseObject) : myNet->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->retrieveEdge(toEdgeID, false);
    GNEAdditional* toBusStop = myNet->retrieveAdditional(SUMO_TAG_BUS_STOP, toBusStopID, false);
    GNEDemandElement* route = myNet->retrieveDemandElement(SUMO_TAG_ROUTE, routeID, false);
    std::vector<GNEEdge*> edges = parseEdges(SUMO_TAG_WALK, edgeIDs);
    // check conditions
    if (personParent && fromEdge) {
        if (edges.size() > 0) {
            // create walk edges
            GNEDemandElement *walk = new GNEWalk(myNet, personParent, edges, arrivalPos);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + walk->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(walk, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(walk);
                // set child references
                personParent->addChildElement(walk);
                for (const auto &edge : edges) {
                    edge->addChildElement(walk);
                }
                walk->incRef("buildWalkEdges");
            }
        } else if (route) {
            // create walk over route
            GNEDemandElement *walk = new GNEWalk(myNet, personParent, route, arrivalPos);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + walk->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(walk, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(walk);
                // set child references
                personParent->addChildElement(walk);
                route->addChildElement(walk);
                walk->incRef("buildWalkRoute");
            }
        } else if (toEdge) {
            // create walk from->to
            GNEDemandElement *walk = new GNEWalk(myNet, personParent, fromEdge, toEdge, arrivalPos);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + walk->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(walk, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(walk);
                // set child references
                personParent->addChildElement(walk);
                fromEdge->addChildElement(walk);
                toEdge->addChildElement(walk);
                walk->incRef("buildWalkFromTo");
            }
        } else if (toBusStop) {
            // create walk from->busStop
            GNEDemandElement *walk = new GNEWalk(myNet, personParent, fromEdge, toBusStop, arrivalPos);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + walk->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(walk, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(walk);
                // set child references
                personParent->addChildElement(walk);
                fromEdge->addChildElement(walk);
                toBusStop->addChildElement(walk);
                walk->incRef("buildWalkFromBusStop");
            }
        }
    }
}


void
GNERouteHandler::buildRide(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdgeID, const std::string &toEdgeID, 
    const std::string &toBusStopID, double arrivalPos, const std::vector<std::string>& lines) {
    // first parse parents
    GNEDemandElement* personParent = getPersonParent(sumoBaseObject);
    GNEEdge* fromEdge = fromEdgeID.empty()? getPreviousPersonPlanEdge(sumoBaseObject) : myNet->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->retrieveEdge(toEdgeID, false);
    GNEAdditional* toBusStop = myNet->retrieveAdditional(SUMO_TAG_BUS_STOP, toBusStopID, false);
    // check conditions
    if (personParent && fromEdge) {
        if (toEdge) {
            // create ride from->to
            GNEDemandElement *ride = new GNERide(myNet, personParent, fromEdge, toEdge, arrivalPos, lines);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + ride->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(ride, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(ride);
                // set child references
                personParent->addChildElement(ride);
                fromEdge->addChildElement(ride);
                toEdge->addChildElement(ride);
                ride->incRef("buildRideFromTo");
            }
        } else if (toBusStop) {
            // create ride from->busStop
            GNEDemandElement *ride = new GNERide(myNet, personParent, fromEdge, toBusStop, arrivalPos, lines);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + ride->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(ride, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(ride);
                // set child references
                personParent->addChildElement(ride);
                fromEdge->addChildElement(ride);
                toBusStop->addChildElement(ride);
                ride->incRef("buildRideFromBusStop");
            }
        }
    }
}


void
GNERouteHandler::buildContainer(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& containerParameters,
    const std::map<std::string, std::string> &parameters) {
    // first check if ID is duplicated
    if (!isContainerIdDuplicated(myNet, containerParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* pType = myNet->retrieveDemandElement(SUMO_TAG_PTYPE, containerParameters.vtypeid, false);
        if (pType == nullptr) {
            WRITE_ERROR("Invalid container type '" + containerParameters.vtypeid + "' used in " + toString(containerParameters.tag) + " '" + containerParameters.id + "'.");
        } else {
            // create container using containerParameters
            GNEDemandElement* container = new GNEContainer(SUMO_TAG_CONTAINER, myNet, pType, containerParameters);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + container->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(container, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(container);
                // set container as child of pType and Route
                pType->addChildElement(container);
                container->incRef("buildContainer");
            }
        }
    }
}


void
GNERouteHandler::buildContainerFlow(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& containerFlowParameters,
    const std::map<std::string, std::string> &parameters) {
    // first check if ID is duplicated
    if (!isContainerIdDuplicated(myNet, containerFlowParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* pType = myNet->retrieveDemandElement(SUMO_TAG_PTYPE, containerFlowParameters.vtypeid, false);
        if (pType == nullptr) {
            WRITE_ERROR("Invalid containerFlow type '" + containerFlowParameters.vtypeid + "' used in " + toString(containerFlowParameters.tag) + " '" + containerFlowParameters.id + "'.");
        } else {
            // create containerFlow using containerFlowParameters
            GNEDemandElement* containerFlow = new GNEContainer(SUMO_TAG_CONTAINERFLOW, myNet, pType, containerFlowParameters);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + containerFlow->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(containerFlow, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(containerFlow);
                // set containerFlow as child of pType and Route
                pType->addChildElement(containerFlow);
                containerFlow->incRef("buildContainerFlow");
            }
        }
    }
}


void 
GNERouteHandler::buildTransport(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdgeID, const std::string &toEdgeID,
    const std::string &toContainerStopID, const std::vector<std::string>& lines, const double arrivalPos) {
    // first parse parents
    GNEDemandElement* containerParent = getContainerParent(sumoBaseObject);
    GNEEdge* fromEdge = myNet->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->retrieveEdge(toEdgeID, false);
    GNEAdditional* toContainerStop = myNet->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, toContainerStopID, false);
    // check conditions
    if (containerParent && fromEdge) {
        if (toEdge) {
            // create transport from->to
            GNEDemandElement *transport = new GNETransport(myNet, containerParent, fromEdge, toEdge, lines, arrivalPos);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + transport->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(transport, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(transport);
                // set child references
                containerParent->addChildElement(transport);
                fromEdge->addChildElement(transport);
                toEdge->addChildElement(transport);
                transport->incRef("buildTransportFromTo");
            }
        } else if (toContainerStop) {
            // create transport from->containerStop
            GNEDemandElement *transport = new GNETransport(myNet, containerParent, fromEdge, toContainerStop, lines, arrivalPos);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + transport->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(transport, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(transport);
                // set child references
                containerParent->addChildElement(transport);
                fromEdge->addChildElement(transport);
                toContainerStop->addChildElement(transport);
                transport->incRef("buildTransportFromContainerStop");
            }
        }
    }
}


void
GNERouteHandler::buildTranship(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdgeID, const std::string &toEdgeID,
    const std::string &toContainerStopID, const std::vector<std::string>& edgeIDs, const double speed, const double departPosition, const double arrivalPosition) {
    // first parse parents
    GNEDemandElement* containerParent = getContainerParent(sumoBaseObject);
    GNEEdge* fromEdge = myNet->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->retrieveEdge(toEdgeID, false);
    GNEAdditional* toContainerStop = myNet->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, toContainerStopID, false);
    std::vector<GNEEdge*> edges = parseEdges(SUMO_TAG_WALK, edgeIDs);
    // check conditions
    if (containerParent && fromEdge) {
        if (edges.size() > 0) {
            // create tranship edges
            GNEDemandElement *tranship = new GNETranship(myNet, containerParent, edges, speed, departPosition, arrivalPosition);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + tranship->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(tranship, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(tranship);
                // set child references
                containerParent->addChildElement(tranship);
                for (const auto &edge : edges) {
                    edge->addChildElement(tranship);
                }
                tranship->incRef("buildTranshipEdges");
            }
        } else if (toEdge) {
            // create tranship from->to
            GNEDemandElement *tranship = new GNETranship(myNet, containerParent, fromEdge, toEdge, speed, departPosition, arrivalPosition);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + tranship->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(tranship, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(tranship);
                // set child references
                containerParent->addChildElement(tranship);
                fromEdge->addChildElement(tranship);
                toEdge->addChildElement(tranship);
                tranship->incRef("buildTranshipFromTo");
            }
        } else if (toContainerStop) {
            // create tranship from->containerStop
            GNEDemandElement *tranship = new GNETranship(myNet, containerParent, fromEdge, toContainerStop, speed, departPosition, arrivalPosition);
            if (myUndoDemandElements) {
                myNet->getViewNet()->getUndoList()->p_begin("add " + tranship->getTagStr());
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(tranship, true), true);
                myNet->getViewNet()->getUndoList()->p_end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(tranship);
                // set child references
                containerParent->addChildElement(tranship);
                fromEdge->addChildElement(tranship);
                toContainerStop->addChildElement(tranship);
                tranship->incRef("buildTranshipFromContainerStop");
            }
        }
    }
}


bool
GNERouteHandler::isVehicleIdDuplicated(GNENet* net, const std::string& id) {
    // declare vehicle tags vector
    std::vector<SumoXMLTag> vehicleTags = {SUMO_TAG_VEHICLE, GNE_TAG_VEHICLE_WITHROUTE, SUMO_TAG_TRIP, GNE_TAG_FLOW_ROUTE, GNE_TAG_FLOW_WITHROUTE, SUMO_TAG_FLOW};
    for (const auto& vehicleTag : vehicleTags) {
        if (net->retrieveDemandElement(vehicleTag, id, false) != nullptr) {
            WRITE_ERROR("There is another " + toString(vehicleTag) + " with the same ID='" + id + "'.");
            return true;
        }
    }
    return false;
}


bool
GNERouteHandler::isPersonIdDuplicated(GNENet* net, const std::string& id) {
    for (SumoXMLTag personTag : std::vector<SumoXMLTag>({SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW})) {
        if (net->retrieveDemandElement(personTag, id, false) != nullptr) {
            WRITE_ERROR("There is another " + toString(personTag) + " with the same ID='" + id + "'.");
            return true;
        }
    }
    return false;
}


bool
GNERouteHandler::isContainerIdDuplicated(GNENet* net, const std::string& id) {
    for (SumoXMLTag containerTag : std::vector<SumoXMLTag>({SUMO_TAG_CONTAINER, SUMO_TAG_CONTAINERFLOW})) {
        if (net->retrieveDemandElement(containerTag, id, false) != nullptr) {
            WRITE_ERROR("There is another " + toString(containerTag) + " with the same ID='" + id + "'.");
            return true;
        }
    }
    return false;
}


void
GNERouteHandler::transformToVehicle(GNEVehicle* originalVehicle, bool createEmbeddedRoute) {
    // get original vehicle tag
    SumoXMLTag tag = originalVehicle->getTagProperty().getTag();
    // get pointer to net
    GNENet* net = originalVehicle->getNet();
    // obtain vehicle parameters
    SUMOVehicleParameter vehicleParameters = *originalVehicle;
    // set "yellow" as original route color
    RGBColor routeColor = RGBColor::YELLOW;
    // declare edges
    std::vector<GNEEdge*> routeEdges;
    // obtain edges depending of tag
    if ((tag == GNE_TAG_FLOW_ROUTE) || (tag == SUMO_TAG_VEHICLE)) {
        // get route edges
        routeEdges = originalVehicle->getParentDemandElements().back()->getParentEdges();
        // get original route color
        routeColor = originalVehicle->getParentDemandElements().back()->getColor();
    } else if ((tag == GNE_TAG_VEHICLE_WITHROUTE) || (tag == GNE_TAG_FLOW_WITHROUTE)) {
        // get embedded route edges
        routeEdges = originalVehicle->getChildDemandElements().front()->getParentEdges();
    } else if ((tag == SUMO_TAG_TRIP) || (tag == SUMO_TAG_FLOW)) {
        // calculate path using from-via-to edges
        routeEdges = originalVehicle->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(originalVehicle->getVClass(), originalVehicle->getParentEdges());
    }
    // only continue if edges are valid
    if (routeEdges.empty()) {
        // declare header
        const std::string header = "Problem transforming to vehicle";
        // declare message
        const std::string message = "Vehicle cannot be transformed. Invalid number of edges";
        // write warning
        WRITE_DEBUG("Opened FXMessageBox " + header);
        // open message box
        FXMessageBox::warning(originalVehicle->getNet()->getViewNet()->getApp(), MBOX_OK, header.c_str(), "%s", message.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox " + header);
    } else {
        // begin undo-redo operation
        net->getViewNet()->getUndoList()->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_VEHICLE));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // check if new vehicle must have an embedded route
        if (createEmbeddedRoute) {
            // change tag in vehicle parameters
            vehicleParameters.tag = GNE_TAG_VEHICLE_WITHROUTE;
            // create a vehicle with embebbed routes
/*
            buildVehicleEmbeddedRoute(net, true, vehicleParameters, routeParameters.edges);
*/
        } else {
            // change tag in vehicle parameters
            vehicleParameters.tag = SUMO_TAG_VEHICLE;
            // generate a new route id
            const std::string routeID = net->generateDemandElementID(SUMO_TAG_ROUTE);
            // build route
/*
            buildRoute(net, true, routeParameters, {});
*/
            // set route ID in vehicle parameters
            vehicleParameters.routeid = routeID;
            // create vehicle
/*
            buildVehicleOverRoute(net, true, vehicleParameters);
*/
        }
        // end undo-redo operation
        net->getViewNet()->getUndoList()->p_end();
    }
}


void
GNERouteHandler::transformToRouteFlow(GNEVehicle* originalVehicle, bool createEmbeddedRoute) {
    // get original vehicle tag
    SumoXMLTag tag = originalVehicle->getTagProperty().getTag();
    // get pointer to net
    GNENet* net = originalVehicle->getNet();
    // obtain vehicle parameters
    SUMOVehicleParameter vehicleParameters = *originalVehicle;
    // set "yellow" as original route color
    RGBColor routeColor = RGBColor::YELLOW;
    // declare edges
    std::vector<GNEEdge*> routeEdges;
    // obtain edges depending of tag
    if ((tag == GNE_TAG_FLOW_ROUTE) || (tag == SUMO_TAG_VEHICLE)) {
        // get route edges
        routeEdges = originalVehicle->getParentDemandElements().back()->getParentEdges();
        // get original route color
        routeColor = originalVehicle->getParentDemandElements().back()->getColor();
    } else if ((tag == GNE_TAG_VEHICLE_WITHROUTE) || (tag == GNE_TAG_FLOW_WITHROUTE)) {
        // get embedded route edges
        routeEdges = originalVehicle->getChildDemandElements().front()->getParentEdges();
    } else if ((tag == SUMO_TAG_TRIP) || (tag == SUMO_TAG_FLOW)) {
        // calculate path using from-via-to edges
        routeEdges = originalVehicle->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(originalVehicle->getVClass(), originalVehicle->getParentEdges());
    }
    // only continue if edges are valid
    if (routeEdges.empty()) {
        // declare header
        const std::string header = "Problem transforming to vehicle";
        // declare message
        const std::string message = "Vehicle cannot be transformed. Invalid number of edges";
        // write warning
        WRITE_DEBUG("Opened FXMessageBox " + header);
        // open message box
        FXMessageBox::warning(originalVehicle->getNet()->getViewNet()->getApp(), MBOX_OK, header.c_str(), "%s", message.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox " + header);
    } else {
        // begin undo-redo operation
        net->getViewNet()->getUndoList()->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_VEHICLE));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // change depart
        if ((vehicleParameters.tag == SUMO_TAG_TRIP) || (vehicleParameters.tag == SUMO_TAG_VEHICLE) || (vehicleParameters.tag == GNE_TAG_VEHICLE_WITHROUTE)) {
            // set end
            vehicleParameters.repetitionEnd = vehicleParameters.depart + 3600;
            // set number
            vehicleParameters.repetitionNumber = 1800;
            vehicleParameters.parametersSet |= VEHPARS_NUMBER_SET;
            // unset parameters
            vehicleParameters.parametersSet &= ~VEHPARS_END_SET;
            vehicleParameters.parametersSet &= ~VEHPARS_VPH_SET;
            vehicleParameters.parametersSet &= ~VEHPARS_PERIOD_SET;
            vehicleParameters.parametersSet &= ~VEHPARS_PROB_SET;
        }
        // check if new vehicle must have an embedded route
        if (createEmbeddedRoute) {
            // change tag in vehicle parameters
            vehicleParameters.tag = GNE_TAG_FLOW_WITHROUTE;
            // create a flow with embebbed routes
/*
            buildFlowEmbeddedRoute(net, true, vehicleParameters, routeParameters.edges);
*/
        } else {
            // change tag in vehicle parameters
            vehicleParameters.tag = GNE_TAG_FLOW_ROUTE;
            // generate a new route id
            const std::string routeID = net->generateDemandElementID(SUMO_TAG_ROUTE);
            // build route
/*
            buildRoute(net, true, routeParameters, {});
*/
            // set route ID in vehicle parameters
            vehicleParameters.routeid = routeID;
            // create flow
/*
            buildFlowOverRoute(net, true, vehicleParameters);
*/
        }
        // end undo-redo operation
        net->getViewNet()->getUndoList()->p_end();
    }
}


void
GNERouteHandler::transformToTrip(GNEVehicle* originalVehicle) {
    // get original vehicle tag
    SumoXMLTag tag = originalVehicle->getTagProperty().getTag();
    // get pointer to net
    GNENet* net = originalVehicle->getNet();
    // obtain vehicle parameters
    SUMOVehicleParameter vehicleParameters = *originalVehicle;
    // get route
    GNEDemandElement* route = nullptr;
    // declare edges
    std::vector<GNEEdge*> edges;
    // obtain edges depending of tag
    if ((tag == SUMO_TAG_VEHICLE) || (tag == GNE_TAG_FLOW_ROUTE)) {
        // set route
        route = originalVehicle->getParentDemandElements().back();
        // get route edges
        edges = route->getParentEdges();
    } else if ((tag == GNE_TAG_VEHICLE_WITHROUTE) || (tag == GNE_TAG_FLOW_WITHROUTE)) {
        // get embedded route edges
        edges = originalVehicle->getChildDemandElements().front()->getParentEdges();
    } else if ((tag == SUMO_TAG_TRIP) || (tag == SUMO_TAG_FLOW)) {
        // just take parent edges (from and to)
        edges = originalVehicle->getParentEdges();
    }
    // only continue if edges are valid
    if (edges.size() < 2) {
        // declare header
        const std::string header = "Problem transforming to vehicle";
        // declare message
        const std::string message = "Vehicle cannot be transformed. Invalid number of edges";
        // write warning
        WRITE_DEBUG("Opened FXMessageBox " + header);
        // open message box
        FXMessageBox::warning(originalVehicle->getNet()->getViewNet()->getApp(), MBOX_OK, header.c_str(), "%s", message.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox " + header);
    } else {
        // begin undo-redo operation
        net->getViewNet()->getUndoList()->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_TRIP));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // check if route has to be deleted
        if (route && route->getChildDemandElements().empty()) {
            net->deleteDemandElement(route, net->getViewNet()->getUndoList());
        }
        // change tag in vehicle parameters
        vehicleParameters.tag = SUMO_TAG_TRIP;
        // create trip
/*
        buildTrip(net, true, vehicleParameters, edges.front(), edges.back(), {});
*/
        // end undo-redo operation
        net->getViewNet()->getUndoList()->p_end();
    }
}


void
GNERouteHandler::transformToFlow(GNEVehicle* originalVehicle) {
    // get original vehicle tag
    SumoXMLTag tag = originalVehicle->getTagProperty().getTag();
    // get pointer to net
    GNENet* net = originalVehicle->getNet();
    // obtain vehicle parameters
    SUMOVehicleParameter vehicleParameters = *originalVehicle;
    // declare route
    GNEDemandElement* route = nullptr;
    // declare edges
    std::vector<GNEEdge*> edges;
    // obtain edges depending of tag
    if ((tag == SUMO_TAG_VEHICLE) || (tag == GNE_TAG_FLOW_ROUTE)) {
        // set route
        route = originalVehicle->getParentDemandElements().back();
        // get route edges
        edges = route->getParentEdges();
    } else if ((tag == GNE_TAG_VEHICLE_WITHROUTE) || (tag == GNE_TAG_FLOW_WITHROUTE)) {
        // get embedded route edges
        edges = originalVehicle->getChildDemandElements().front()->getParentEdges();
    } else if ((tag == SUMO_TAG_TRIP) || (tag == SUMO_TAG_FLOW)) {
        // just take parent edges (from and to)
        edges = originalVehicle->getParentEdges();
    }
    // only continue if edges are valid
    if (edges.empty()) {
        // declare header
        const std::string header = "Problem transforming to vehicle";
        // declare message
        const std::string message = "Vehicle cannot be transformed. Invalid number of edges";
        // write warning
        WRITE_DEBUG("Opened FXMessageBox " + header);
        // open message box
        FXMessageBox::warning(originalVehicle->getNet()->getViewNet()->getApp(), MBOX_OK, header.c_str(), "%s", message.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox " + header);
    } else {
        // begin undo-redo operation
        net->getViewNet()->getUndoList()->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_VEHICLE));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // check if route has to be deleted
        if (route && route->getChildDemandElements().empty()) {
            net->deleteDemandElement(route, net->getViewNet()->getUndoList());
        }
        // change depart
        if ((vehicleParameters.tag == SUMO_TAG_TRIP) || (vehicleParameters.tag == SUMO_TAG_VEHICLE) || (vehicleParameters.tag == GNE_TAG_VEHICLE_WITHROUTE)) {
            // set end
            vehicleParameters.repetitionEnd = vehicleParameters.depart + 3600;
            // set number
            vehicleParameters.repetitionNumber = 1800;
            vehicleParameters.parametersSet |= VEHPARS_NUMBER_SET;
            // unset parameters
            vehicleParameters.parametersSet &= ~VEHPARS_END_SET;
            vehicleParameters.parametersSet &= ~VEHPARS_VPH_SET;
            vehicleParameters.parametersSet &= ~VEHPARS_PERIOD_SET;
            vehicleParameters.parametersSet &= ~VEHPARS_PROB_SET;
        }
        // change tag in vehicle parameters
        vehicleParameters.tag = SUMO_TAG_FLOW;
        // create flow
/*
        buildFlow(net, true, vehicleParameters, edges.front(), edges.back(), {});
*/
        // end undo-redo operation
        net->getViewNet()->getUndoList()->p_end();
    }
}


void
GNERouteHandler::transformToPerson(GNEPerson* /*originalPerson*/) {
    //
}


void
GNERouteHandler::transformToPersonFlow(GNEPerson* /*originalPerson*/) {
    //
}


void
GNERouteHandler::transformToContainer(GNEContainer* /*originalContainer*/) {
    //
}


void
GNERouteHandler::transformToContainerFlow(GNEContainer* /*originalContainer*/) {
    //
}


void
GNERouteHandler::setFlowParameters(const SumoXMLAttr attribute, int& parameters) {
    // modify parametersSetCopy depending of given Flow attribute
    switch (attribute) {
        case SUMO_ATTR_END: {
            // give more priority to end
            parameters = VEHPARS_END_SET | VEHPARS_NUMBER_SET;
            break;
        }
        case SUMO_ATTR_NUMBER:
            parameters ^= VEHPARS_END_SET;
            parameters |= VEHPARS_NUMBER_SET;
            break;
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERSONSPERHOUR: {
            // give more priority to end
            if ((parameters & VEHPARS_END_SET) && (parameters & VEHPARS_NUMBER_SET)) {
                parameters = VEHPARS_END_SET;
            } else if (parameters & VEHPARS_END_SET) {
                parameters = VEHPARS_END_SET;
            } else if (parameters & VEHPARS_NUMBER_SET) {
                parameters = VEHPARS_NUMBER_SET;
            }
            // set VehsPerHour
            parameters |= VEHPARS_VPH_SET;
            break;
        }
        case SUMO_ATTR_PERIOD: {
            // give more priority to end
            if ((parameters & VEHPARS_END_SET) && (parameters & VEHPARS_NUMBER_SET)) {
                parameters = VEHPARS_END_SET;
            } else if (parameters & VEHPARS_END_SET) {
                parameters = VEHPARS_END_SET;
            } else if (parameters & VEHPARS_NUMBER_SET) {
                parameters = VEHPARS_NUMBER_SET;
            }
            // set period
            parameters |= VEHPARS_PERIOD_SET;
            break;
        }
        case SUMO_ATTR_PROB: {
            // give more priority to end
            if ((parameters & VEHPARS_END_SET) && (parameters & VEHPARS_NUMBER_SET)) {
                parameters = VEHPARS_END_SET;
            } else if (parameters & VEHPARS_END_SET) {
                parameters = VEHPARS_END_SET;
            } else if (parameters & VEHPARS_NUMBER_SET) {
                parameters = VEHPARS_NUMBER_SET;
            }
            // set probability
            parameters |= VEHPARS_PROB_SET;
            break;
        }
        default:
            break;
    }
}

// ===========================================================================
// protected
// ===========================================================================

GNEEdge*
GNERouteHandler::parseEdge(const SumoXMLTag tag, const std::string& edgeID) const {
    GNEEdge* edge = myNet->retrieveEdge(edgeID, false);
    // empty edges aren't allowed. If edge is empty, write error, clear edges and stop
    if (edge == nullptr) {
        WRITE_ERROR("Could not build " + toString(tag) + " in netedit; " +  toString(SUMO_TAG_EDGE) + " doesn't exist.");
    }
    return edge;
}


std::vector<GNEEdge*>
GNERouteHandler::parseEdges(const SumoXMLTag tag, const std::vector<std::string>& edgeIDs) const {
    std::vector<GNEEdge*> edges;
    for (const auto &edgeID : edgeIDs) {
        GNEEdge* edge = myNet->retrieveEdge(edgeID, false);
        // empty edges aren't allowed. If edge is empty, write error, clear edges and stop
        if (edge == nullptr) {
            WRITE_ERROR("Could not build " + toString(tag) + " in netedit; " +  toString(SUMO_TAG_EDGE) + " doesn't exist.");
            edges.clear();
            return edges;
        } else {
            edges.push_back(edge);
        }
    }
    return edges;
}


GNEDemandElement*
GNERouteHandler::getPersonParent(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const {
    // check that sumoBaseObject has parent
    if (sumoBaseObject->getParentSumoBaseObject() == nullptr) {
        return nullptr;
    }
    if ((sumoBaseObject->getParentSumoBaseObject()->getTag() != SUMO_TAG_PERSON) && 
        (sumoBaseObject->getParentSumoBaseObject()->getTag() != SUMO_TAG_PERSONFLOW)) {
        return nullptr;
    }
    // try it with person
    GNEDemandElement* personParent = myNet->retrieveDemandElement(SUMO_TAG_PERSON, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    // if empty, try it with personFlow
    if (personParent == nullptr) {
        return myNet->retrieveDemandElement(SUMO_TAG_PERSONFLOW, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    } else {
        return personParent;
    }
}


GNEDemandElement*
GNERouteHandler::getContainerParent(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const {
    // check that sumoBaseObject has parent
    if (sumoBaseObject->getParentSumoBaseObject() == nullptr) {
        return nullptr;
    }
    if ((sumoBaseObject->getParentSumoBaseObject()->getTag() != SUMO_TAG_CONTAINER) && 
        (sumoBaseObject->getParentSumoBaseObject()->getTag() != SUMO_TAG_CONTAINERFLOW)) {
        return nullptr;
    }
    // try it with container
    GNEDemandElement* containerParent = myNet->retrieveDemandElement(SUMO_TAG_CONTAINER, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    // if empty, try it with containerFlow
    if (containerParent == nullptr) {
        return myNet->retrieveDemandElement(SUMO_TAG_CONTAINERFLOW, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    } else {
        return containerParent;
    }
}


GNEEdge*
GNERouteHandler::getPreviousPersonPlanEdge(const CommonXMLStructure::SumoBaseObject* obj) const {
    if (obj->getParentSumoBaseObject() == nullptr) {
        // no parent defined
        return nullptr;
    }
    // get parent object
    const CommonXMLStructure::SumoBaseObject* parentObject = obj->getParentSumoBaseObject();
    // check conditions
    if ((parentObject->getTag() != SUMO_TAG_PERSON) && (parentObject->getTag() != SUMO_TAG_PERSONFLOW)) {
        // invalid parent
        return nullptr;
    }
    // search previous child
    const auto it = std::find(parentObject->getSumoBaseObjectChildren().begin(), parentObject->getSumoBaseObjectChildren().end(), obj);
    if (it == parentObject->getSumoBaseObjectChildren().begin()) {
        return nullptr;
    }
    // get last children
    const CommonXMLStructure::SumoBaseObject* previousPersonPlan = *(it - 1);
    // check conditions
    if ((previousPersonPlan->getTag() != SUMO_TAG_WALK) && (previousPersonPlan->getTag() != SUMO_TAG_RIDE) && 
        (previousPersonPlan->getTag() != SUMO_TAG_PERSONTRIP) && (previousPersonPlan->getTag() != SUMO_TAG_STOP)) {
        // invalid last child
        return nullptr;
    }
    // ends in an edge (only for stops)
    if (previousPersonPlan->hasStringAttribute(SUMO_ATTR_EDGE)) {
        return myNet->retrieveEdge(previousPersonPlan->getStringAttribute(SUMO_ATTR_EDGE), false);
    }
    // ends in a lane (only for stops)
    if (previousPersonPlan->hasStringAttribute(SUMO_ATTR_LANE)) {
        const auto lane = myNet->retrieveLane(previousPersonPlan->getStringAttribute(SUMO_ATTR_LANE), false);
        if (lane) {
            return lane->getParentEdge();
        } else {
            return nullptr;
        }
    }
    // ends in a route (walk)
    if (previousPersonPlan->hasStringAttribute(SUMO_ATTR_ROUTE) && 
        !previousPersonPlan->getStringAttribute(SUMO_ATTR_ROUTE).empty()) {
        const auto route = myNet->retrieveDemandElement(SUMO_TAG_ROUTE, previousPersonPlan->getStringAttribute(SUMO_ATTR_ROUTE), false);
        if (route) {
            return route->getParentEdges().back();
        } else {
            return nullptr;
        }
    }
    // ends in a list of edges (walk)
    if (previousPersonPlan->hasStringListAttribute(SUMO_ATTR_EDGES) && 
        !previousPersonPlan->getStringListAttribute(SUMO_ATTR_EDGES).empty()) {
        return myNet->retrieveEdge(previousPersonPlan->getStringListAttribute(SUMO_ATTR_EDGES).back(), false);
    }
    // ends in a "to" edge
    if (previousPersonPlan->hasStringAttribute(SUMO_ATTR_TO) &&
        !previousPersonPlan->getStringAttribute(SUMO_ATTR_TO).empty()) {
        return myNet->retrieveEdge(previousPersonPlan->getStringAttribute(SUMO_ATTR_TO), false);
    } 
    // ends in a "busStop"
    if (previousPersonPlan->hasStringAttribute(SUMO_ATTR_BUS_STOP) && 
        !previousPersonPlan->getStringAttribute(SUMO_ATTR_BUS_STOP).empty()) {
        const auto busStop = myNet->retrieveAdditional(SUMO_TAG_BUS_STOP, previousPersonPlan->getStringAttribute(SUMO_ATTR_BUS_STOP), false);
        if (busStop) {
            return busStop->getParentLanes().front()->getParentEdge();
        } else {
            return nullptr;
        }
    } 
    return nullptr;
}

/****************************************************************************/
