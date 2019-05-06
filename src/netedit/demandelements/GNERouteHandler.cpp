/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERouteHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Builds demand objects for netedit
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBVehicle.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/additionals/GNEBusStop.h>
#include <netedit/additionals/GNEChargingStation.h>
#include <netedit/additionals/GNEContainerStop.h>
#include <netedit/additionals/GNEParkingArea.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/router/DijkstraRouter.h>

#include "GNERouteHandler.h"
#include "GNERoute.h"
#include "GNEStop.h"
#include "GNEVehicle.h"
#include "GNEVehicleType.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteHandler::GNERouteHandler(const std::string& file, GNEViewNet* viewNet, bool undoDemandElements) :
    SUMORouteHandler(file, ""),
    myViewNet(viewNet),
    myUndoDemandElements(undoDemandElements) {
}


GNERouteHandler::~GNERouteHandler() {}


void
GNERouteHandler::buildVehicle(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter* vehicleParameters) {
    // first check if vehicle parameter was sucesfulyl created
    if (vehicleParameters) {
        // now check if exist another vehicle with the same ID
        if (viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VEHICLE, vehicleParameters->id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_VEHICLE) + " with the same ID='" + vehicleParameters->id + "'.");
        } else {
            // obtain routes and vtypes
            GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters->vtypeid, false);
            GNEDemandElement* route = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, vehicleParameters->routeid, false);
            if (vType == nullptr) {
                WRITE_WARNING("Invalid vehicle Type '" + vehicleParameters->vtypeid + "' used in vehicle '" + vehicleParameters->id + "'.");
            } else if (route == nullptr) {
                WRITE_WARNING("Invalid route '" + vehicleParameters->routeid + "' used in vehicle '" + vehicleParameters->id + "'.");
            } else {
                // create vehicle using vehicleParameters
                GNEVehicle* vehicle = new GNEVehicle(SUMO_TAG_VEHICLE, viewNet, *vehicleParameters, vType, route);
                if (undoDemandElements) {
                    viewNet->getUndoList()->p_begin("add " + vehicle->getTagStr());
                    viewNet->getUndoList()->add(new GNEChange_DemandElement(vehicle, true), true);
                    viewNet->getUndoList()->p_end();
                    // iterate over stops of vehicleParameters and create stops associated with it
                    for (const auto& i : vehicleParameters->stops) {
                        buildStop(viewNet, true, i, vehicle, false);
                    }
                } else {
                    viewNet->getNet()->insertDemandElement(vehicle);
                    vType->addDemandElementChild(vehicle);
                    vehicle->incRef("buildVehicle");
                    // iterate over stops of vehicleParameters and create stops associated with it
                    for (const auto& i : vehicleParameters->stops) {
                        buildStop(viewNet, false, i, vehicle, false);
                    }
                }
            }
        }
    }
}


void
GNERouteHandler::buildFlow(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter* flowParameters) {
    // first check if flow parameter was sucesfulyl created
    if (flowParameters) {
        // now check if exist another flow with the same ID
        if (viewNet->getNet()->retrieveDemandElement(SUMO_TAG_FLOW, flowParameters->id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_FLOW) + " with the same ID='" + flowParameters->id + "'.");
        } else {
            // obtain routes and vtypes
            GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, flowParameters->vtypeid, false);
            GNEDemandElement* route = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, flowParameters->routeid, false);
            if (vType == nullptr) {
                WRITE_WARNING("Invalid flow Type '" + flowParameters->vtypeid + "' used in flow '" + flowParameters->id + "'.");
            } else if (route == nullptr) {
                WRITE_WARNING("Invalid route '" + flowParameters->routeid + "' used in flow '" + flowParameters->id + "'.");
            } else {
                // create flow using flowParameters
                GNEVehicle* flow = new GNEVehicle(SUMO_TAG_FLOW, viewNet, *flowParameters, vType, route);
                if (undoDemandElements) {
                    viewNet->getUndoList()->p_begin("add " + flow->getTagStr());
                    viewNet->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                    viewNet->getUndoList()->p_end();
                } else {
                    viewNet->getNet()->insertDemandElement(flow);
                    vType->addDemandElementChild(flow);
                    flow->incRef("buildFlow");
                }
            }
        }
    }
}


void
GNERouteHandler::buildTrip(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter* tripParameters, const std::vector<GNEEdge*>& edges) {
    // first check if trip parameter was sucesfulyl created
    if (tripParameters) {
        // now check if exist another trip with the same ID
        if (viewNet->getNet()->retrieveDemandElement(SUMO_TAG_TRIP, tripParameters->id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_TRIP) + " with the same ID='" + tripParameters->id + "'.");
        } else {
            // obtain  vtypes
            GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, tripParameters->vtypeid, false);
            if (vType == nullptr) {
                WRITE_WARNING("Invalid trip Type '" + tripParameters->vtypeid + "' used in trip '" + tripParameters->id + "'.");
            } else {
                // check if route exist
                std::vector<const NBEdge*> routeEdges;
                NBVehicle tmpVehicle("temporalNBVehicle", GNEAttributeCarrier::parse<SUMOVehicleClass>(vType->getAttribute(SUMO_ATTR_VCLASS)));
                SUMOAbstractRouter<NBEdge, NBVehicle>* route;
                // create Dijkstra route
                route = new DijkstraRouter<NBEdge, NBVehicle, SUMOAbstractRouter<NBEdge, NBVehicle> >(
                    viewNet->getNet()->getNetBuilder()->getEdgeCont().getAllEdges(), true, &NBEdge::getTravelTimeStatic, nullptr, true);
                // check if route is valid
                if (!route->compute(edges.front()->getNBEdge(), edges.back()->getNBEdge(), &tmpVehicle, 10, routeEdges)) {
                    WRITE_WARNING("There isn't a route bewteen edges '" + edges.front()->getID() + "' and '" + edges.back()->getID() + "'.");
                }
                // delete route
                delete route;
                // create trip using tripParameters
                GNEVehicle* trip = new GNEVehicle(viewNet, *tripParameters, vType, edges);
                if (undoDemandElements) {
                    viewNet->getUndoList()->p_begin("add " + trip->getTagStr());
                    viewNet->getUndoList()->add(new GNEChange_DemandElement(trip, true), true);
                    viewNet->getUndoList()->p_end();
                } else {
                    viewNet->getNet()->insertDemandElement(trip);
                    vType->addDemandElementChild(trip);
                    trip->incRef("buildTrip");
                }
            }
        }
    }
}


void
GNERouteHandler::buildStop(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter::Stop& stopParameters, GNEDemandElement* stopParent, bool friendlyPosition) {
    // declare pointers to stopping place  and lane and obtain it
    GNEAdditional* stoppingPlace = nullptr;
    GNELane* lane = nullptr;
    SumoXMLTag stopTagType = SUMO_TAG_NOTHING;
    if (stopParameters.busstop.size() > 0) {
        stoppingPlace = viewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, stopParameters.busstop, false);
        stopTagType = SUMO_TAG_STOP_BUSSTOP;
    } else if (stopParameters.containerstop.size() > 0) {
        stoppingPlace = viewNet->getNet()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, stopParameters.containerstop, false);
        stopTagType = SUMO_TAG_STOP_CONTAINERSTOP;
    } else if (stopParameters.chargingStation.size() > 0) {
        stoppingPlace = viewNet->getNet()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, stopParameters.chargingStation, false);
        stopTagType = SUMO_TAG_STOP_CHARGINGSTATION;
    } else if (stopParameters.parkingarea.size() > 0) {
        stoppingPlace = viewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, stopParameters.parkingarea, false);
        stopTagType = SUMO_TAG_STOP_PARKINGAREA;
    } else if (stopParameters.lane.size() > 0) {
        lane = viewNet->getNet()->retrieveLane(stopParameters.lane, false);
        stopTagType = SUMO_TAG_STOP_LANE;
    }
    // check if values are correct
    if (stoppingPlace && lane) {
        WRITE_WARNING("A stop must be defined either over a stoppingPlace or over a lane");
    } else if (!stoppingPlace && !lane) {
        WRITE_WARNING("A stop requires a stoppingPlace or a lane");
    } else if (stoppingPlace) {
        // create stop using stopParameters and stoppingPlace
        GNEStop* stop = new GNEStop(stopTagType, viewNet, stopParameters, stoppingPlace, stopParent);
        // add it depending of undoDemandElements
        if (undoDemandElements) {
            viewNet->getUndoList()->p_begin("add " + stop->getTagStr());
            viewNet->getUndoList()->add(new GNEChange_DemandElement(stop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertDemandElement(stop);
            stoppingPlace->addDemandElementChild(stop);
            stopParent->addDemandElementChild(stop);
            stop->incRef("buildStoppingPlaceStop");
        }
    } else {
        // create stop using stopParameters and lane
        GNEStop* stop = new GNEStop(viewNet, stopParameters, lane, friendlyPosition, stopParent);
        // add it depending of undoDemandElements
        if (undoDemandElements) {
            viewNet->getUndoList()->p_begin("add " + stop->getTagStr());
            viewNet->getUndoList()->add(new GNEChange_DemandElement(stop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertDemandElement(stop);
            lane->addDemandElementChild(stop);
            stopParent->addDemandElementChild(stop);
            stop->incRef("buildLaneStop");
        }
    }
}


void
GNERouteHandler::openVehicleTypeDistribution(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::closeVehicleTypeDistribution() {
    // currently unused
}


void
GNERouteHandler::openRoute(const SUMOSAXAttributes& attrs) {
    myAbort = false;
    // parse attribute of routes
    myRouteID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_ROUTE, SUMO_ATTR_ID, myAbort);
    myEdgeIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, myRouteID, SUMO_TAG_ROUTE, SUMO_ATTR_EDGES, myAbort);
    myRouteColor = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, myRouteID, SUMO_TAG_ROUTE, SUMO_ATTR_COLOR, myAbort);

}


void
GNERouteHandler::openTrip(const SUMOSAXAttributes& attrs) {
    myAbort = false;
    // parse attributes of Trips
    myFromID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_TRIP, SUMO_ATTR_FROM, myAbort);
    myToID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_TRIP, SUMO_ATTR_TO, myAbort);
    // attribute VIA is optional
    if (attrs.hasAttribute(SUMO_ATTR_VIA)) {
        myViaIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_TRIP, SUMO_ATTR_VIA, myAbort);
    } else {
        myViaIDs.clear();
    }
}


void
GNERouteHandler::closeRoute(const bool /* mayBeDisconnected */) {
    // obtain edges (And show warnings if isn't valid)
    std::vector<GNEEdge*> edges;
    if (GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), myEdgeIDs, true)) {
        edges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(myViewNet->getNet(), myEdgeIDs);
    }
    // check that all elements are valid
    if (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, myRouteID, false) != nullptr) {
        WRITE_WARNING("There is another " + toString(SUMO_TAG_ROUTE) + " with the same ID='" + myRouteID + "'.");
    } else if (edges.size() == 0) {
        WRITE_WARNING("Routes needs at least one edge.");
    } else {
        // creaste GNERoute
        GNERoute* route = new GNERoute(myViewNet, myRouteID, edges, myRouteColor, SVC_PASSENGER);
        if (myUndoDemandElements) {
            myViewNet->getUndoList()->p_begin("add " + route->getTagStr());
            myViewNet->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
            myViewNet->getUndoList()->p_end();
        } else {
            myViewNet->getNet()->insertDemandElement(route);
            for (const auto& i : edges) {
                i->addDemandElementChild(route);
            }
            route->incRef("buildRoute");
        }
    }
}


void
GNERouteHandler::openRouteDistribution(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::closeRouteDistribution() {
    // currently unused
}


void
GNERouteHandler::closeVehicle() {
    // build vehicle
    buildVehicle(myViewNet, myUndoDemandElements, myVehicleParameter);
}


void
GNERouteHandler::closeVType() {
    // first check that VType was sucesfully created
    if (myCurrentVType) {
        // first check if loaded VType is a default vtype
        if ((myCurrentVType->id == DEFAULT_VTYPE_ID) || (myCurrentVType->id == DEFAULT_PEDTYPE_ID) || (myCurrentVType->id == DEFAULT_BIKETYPE_ID)) {
            // overwrite default vehicle type
            GNEVehicleType::overwriteVType(myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, myCurrentVType->id, false), myCurrentVType, myViewNet->getUndoList());
        } else if (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, myCurrentVType->id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_VTYPE) + " with the same ID='" + myCurrentVType->id + "'.");
        } else {
            // create VType using myCurrentVType
            GNEVehicleType* vType = new GNEVehicleType(myViewNet, *myCurrentVType);
            if (myUndoDemandElements) {
                myViewNet->getUndoList()->p_begin("add " + vType->getTagStr());
                myViewNet->getUndoList()->add(new GNEChange_DemandElement(vType, true), true);
                myViewNet->getUndoList()->p_end();
            } else {
                myViewNet->getNet()->insertDemandElement(vType);
                vType->incRef("buildVType");
            }
        }
    }
}


void
GNERouteHandler::closePerson() {
    // currently unused
}

void
GNERouteHandler::closePersonFlow() {
    // currently unused
}

void
GNERouteHandler::closeContainer() {
    // currently unused
}


void
GNERouteHandler::closeFlow() {
    // build flow
    buildFlow(myViewNet, myUndoDemandElements, myVehicleParameter);
}


void
GNERouteHandler::closeTrip() {
    // force reroute
    myVehicleParameter->parametersSet |= VEHPARS_FORCE_REROUTE;
    // obtain from and to edges
    GNEEdge* from = myViewNet->getNet()->retrieveEdge(myFromID, false);
    GNEEdge* to = myViewNet->getNet()->retrieveEdge(myToID, false);
    // check if edges are valid
    if (from == nullptr) {
        WRITE_WARNING("Invalid 'from' edge used in trip '" + myVehicleParameter->vtypeid + "'.");
    } else if (to == nullptr) {
        WRITE_WARNING("Invalid 'to' edge used in trip '" + myVehicleParameter->vtypeid + "'.");
    } else if (!GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), myEdgeIDs, false)) {
        WRITE_WARNING("Invalid 'via' edges used in trip '" + myVehicleParameter->vtypeid + "'.");
    } else {
        // obtain via
        std::vector<GNEEdge*> via = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(myViewNet->getNet(), myEdgeIDs);
        // build edges (from - via - to)
        std::vector<GNEEdge*> edges;
        edges.push_back(from);
        for (const auto& i : via) {
            edges.push_back(i);
        }
        // check that from and to edge are different
        if (from != to) {
            edges.push_back(to);
        }
        // build trips
        buildTrip(myViewNet, true, myVehicleParameter, edges);
    }
}


void
GNERouteHandler::addStop(const SUMOSAXAttributes& attrs) {
    std::string errorSuffix;
    if (myVehicleParameter != nullptr) {
        errorSuffix = " in vehicle '" + myVehicleParameter->id + "'.";
    } else {
        errorSuffix = " in route '" + myActiveRouteID + "'.";
    }
    SUMOVehicleParameter::Stop stop;
    bool ok = parseStop(stop, attrs, errorSuffix, MsgHandler::getErrorInstance());
    if (!ok) {
        return;
    }
    // try to parse the assigned bus stop
    if (stop.busstop != "") {
        // ok, we have a bus stop
        GNEBusStop* bs = dynamic_cast<GNEBusStop*>(myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, stop.busstop, false));
        if (bs == nullptr) {
            WRITE_ERROR("The busStop '" + stop.busstop + "' is not known" + errorSuffix);
            return;
        }
        // obtain lane
        stop.lane = bs->getAttribute(SUMO_ATTR_LANE);
    } //try to parse the assigned container stop
    else if (stop.containerstop != "") {
        // ok, we have obviously a container stop
        GNEAdditional* cs = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, stop.containerstop, false);
        if (cs == nullptr) {
            WRITE_ERROR("The containerStop '" + stop.containerstop + "' is not known" + errorSuffix);
            return;
        }
        stop.lane = cs->getAttribute(SUMO_ATTR_LANE);
    } //try to parse the assigned parking area
    else if (stop.parkingarea != "") {
        // ok, we have obviously a parking area
        GNEAdditional* pa = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, stop.parkingarea, false);
        if (pa == nullptr) {
            WRITE_ERROR("The parkingArea '" + stop.parkingarea + "' is not known" + errorSuffix);
            return;
        }
        stop.lane = pa->getAttribute(SUMO_ATTR_LANE);
    } else if (stop.chargingStation != "") {
        // ok, we have a charging station
        GNEAdditional* cs = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, stop.chargingStation, false);
        if (cs == nullptr) {
            WRITE_ERROR("The chargingStation '" + stop.chargingStation + "' is not known" + errorSuffix);
            return;
        }
        stop.lane = cs->getAttribute(SUMO_ATTR_LANE);
    } else {
        // no, the lane and the position should be given
        // get the lane
        stop.lane = attrs.getOpt<std::string>(SUMO_ATTR_LANE, nullptr, ok, "");
        GNELane* lane = myViewNet->getNet()->retrieveLane(stop.lane, false);
        // check if lane is valid
        if (ok && stop.lane != "") {
            if (lane == nullptr) {
                WRITE_ERROR("The lane '" + stop.lane + "' for a stop is not known" + errorSuffix);
                return;
            }
        } else {
            WRITE_ERROR("A stop must be placed on a busStop, a chargingStation, a containerStop a parkingArea or a lane" + errorSuffix);
            return;
        }
        // calculate start and end position
        stop.endPos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, nullptr, ok, lane->getLaneParametricLength());
        if (attrs.hasAttribute(SUMO_ATTR_POSITION)) {
            WRITE_WARNING("Deprecated attribute 'pos' in description of stop" + errorSuffix);
            stop.endPos = attrs.getOpt<double>(SUMO_ATTR_POSITION, nullptr, ok, stop.endPos);
        }
        stop.startPos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, nullptr, ok, MAX2(0., stop.endPos - 2 * POSITION_EPS));
        const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, nullptr, ok, false);
        if (!ok || !checkStopPos(stop.startPos, stop.endPos, lane->getLaneParametricLength(), POSITION_EPS, friendlyPos)) {
            WRITE_ERROR("Invalid start or end position for stop on lane '" + stop.lane + "'" + errorSuffix);
            return;
        }
    }
    if (myVehicleParameter != nullptr) {
        myVehicleParameter->stops.push_back(stop);
    } else {
        myActiveRouteStops.push_back(stop);
    }
}


void
GNERouteHandler::addPersonTrip(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::addWalk(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::addPerson(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::addContainer(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::addRide(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::addTransport(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::addTranship(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}

/****************************************************************************/
