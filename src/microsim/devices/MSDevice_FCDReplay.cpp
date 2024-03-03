/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2013-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_FCDReplay.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    11.06.2013
///
// A device which stands as an implementation FCD and which outputs movereminder calls
/****************************************************************************/
#include <config.h>

#include <utils/geom/Position.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/XMLSubSys.h>
#include <libsumo/Helper.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSRoute.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSInsertionControl.h>
#include "MSDevice_FCDReplay.h"


// ===========================================================================
// static member initializations
// ===========================================================================
MSDevice_FCDReplay::FCDHandler MSDevice_FCDReplay::myHandler;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_FCDReplay::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("FCD Replay Device");
    insertDefaultAssignmentOptions("fcd-replay", "FCD Replay Device", oc);

    oc.doRegister("device.fcd-replay.file", new Option_FileName());
    oc.addDescription("device.fcd.begin", "FCD Replay Device", TL("FCD file to read"));
}


void
MSDevice_FCDReplay::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "fcd-replay", v, oc.isSet("device.fcd-replay.file"))) {
        MSDevice_FCDReplay* device = new MSDevice_FCDReplay(v, "fcdReplay_" + v.getID());
        into.push_back(device);
    }
}


void
MSDevice_FCDReplay::init() {
    const OptionsCont& oc = OptionsCont::getOptions();
    if (oc.isSet("device.fcd-replay.file")) {
        if (!XMLSubSys::runParser(myHandler, oc.getString("device.fcd-replay.file"))) {
            throw ProcessError();
        }
        myHandler.addVehicles();
    }
}


// ---------------------------------------------------------------------------
// MSDevice_FCDReplay-methods
// ---------------------------------------------------------------------------
MSDevice_FCDReplay::MSDevice_FCDReplay(SUMOVehicle& holder, const std::string& id) :
    MSVehicleDevice(holder, id) {
}


MSDevice_FCDReplay::~MSDevice_FCDReplay() {
}


bool
MSDevice_FCDReplay::notifyMove(SUMOTrafficObject& veh,
                         double /*oldPos*/,
                         double /*newPos*/,
                         double /*newSpeed*/) {
    if (myTrajectory == nullptr || myTrajectory->empty()) {
        // TODO remove vehicle
        return false;
    }
    MSVehicle* v = dynamic_cast<MSVehicle*>(&veh);
    if (v == nullptr) {
        return false;
    }
    const auto& p = myTrajectory->front();
    MSLane* lane = nullptr;
    double lanePos;
    double lanePosLat = 0;
    double bestDistance = std::numeric_limits<double>::max();
    int routeOffset = 0;
    ConstMSEdgeVector edges;
    libsumo::Helper::moveToXYMap_matchingRoutePosition(std::get<0>(p), std::get<1>(p),
            v->getRoute().getEdges(), v->getRoutePosition(),
            v->getVehicleType().getVehicleClass(), true,
            bestDistance, &lane, lanePos, routeOffset);
    libsumo::Helper::setRemoteControlled(v, std::get<0>(p), lane, std::get<2>(p), lanePosLat, libsumo::INVALID_DOUBLE_VALUE, routeOffset, edges, SIMSTEP);
    v->setPreviousSpeed(std::get<3>(p), std::numeric_limits<double>::min());
    myTrajectory->erase(myTrajectory->begin());
    if (myTrajectory->empty()) {
        // TODO remove vehicle
        return false;
    }
    return true;
}


// ---------------------------------------------------------------------------
// MSDevice_FCDReplay::FCDHandler-methods
// ---------------------------------------------------------------------------
void
MSDevice_FCDReplay::FCDHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    bool ok = true;
    switch (element) {
        case SUMO_TAG_TIMESTEP:
            myTime = attrs.getSUMOTimeReporting(SUMO_ATTR_TIME, "", ok);
            return;
        case SUMO_TAG_VEHICLE:
        case SUMO_TAG_PERSON: {
            const std::string id = attrs.getString(SUMO_ATTR_ID);
            const double x = attrs.getOpt<double>(SUMO_ATTR_X, id.c_str(), ok, INVALID_DOUBLE);
            const double y = attrs.getOpt<double>(SUMO_ATTR_Y, id.c_str(), ok, INVALID_DOUBLE);
            const std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, "");
            const std::string lane = attrs.getOpt<std::string>(SUMO_ATTR_LANE, id.c_str(), ok, "");
            const double speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, id.c_str(), ok, INVALID_DOUBLE);
            const double pos = attrs.getOpt<double>(SUMO_ATTR_POSITION, id.c_str(), ok, INVALID_DOUBLE);
            myTrajectories[id].push_back({Position(x, y), lane, pos, speed});
            const MSEdge* const edge = MSEdge::dictionary(SUMOXMLDefinitions::getEdgeIDFromLane(lane));
            if (myRoutes.count(id) == 0) {
                myRoutes[id] = {myTime, type, {edge}};
            } else {
                auto& route = std::get<2>(myRoutes[id]);
                if (edge != route.back()) {
                    route.push_back(edge);
                }
            }
            return;
        }
        default:
            break;
    }
}


void
MSDevice_FCDReplay::FCDHandler::addVehicles() {
    for (const auto& vehDesc : myRoutes) {
        const std::string& id = vehDesc.first;
        const std::string dummyRouteID = "DUMMY_ROUTE_" + id;
        Trajectory& t = myTrajectories[id];
        const std::vector<SUMOVehicleParameter::Stop> stops;
        ConstMSRoutePtr route = std::make_shared<MSRoute>(dummyRouteID, std::get<2>(vehDesc.second), true, nullptr, stops);
        SUMOVehicleParameter* params = new SUMOVehicleParameter();
        params->id = id;
        params->depart = std::get<0>(vehDesc.second);
        params->departPos = std::get<2>(t.front());
        params->departSpeed = std::get<3>(t.front());
        if (!MSRoute::dictionary(dummyRouteID, route)) {
            throw ProcessError("Could not add route '" + dummyRouteID + "'.");
        }
        MSVehicleType* vehicleType = MSNet::getInstance()->getVehicleControl().getVType(std::get<1>(vehDesc.second));
        SUMOVehicle* vehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(params, route, vehicleType, false);
        MSNet::getInstance()->getVehicleControl().addVehicle(id, vehicle);
        MSNet::getInstance()->getInsertionControl().add(vehicle);
        MSDevice_FCDReplay* device = static_cast<MSDevice_FCDReplay*>(vehicle->getDevice(typeid(MSDevice_FCDReplay)));
        t.erase(t.begin());
        device->setTrajectory(&t);
    }
}


/****************************************************************************/
