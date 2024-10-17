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
/// @author  Michael Behrisch
/// @date    01.03.2024
///
// A device which replays recorded floating car data
/****************************************************************************/
#include <config.h>

#include <utils/common/StaticCommand.h>
#include <utils/geom/Position.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/SUMOSAXReader.h>
#include <utils/xml/XMLSubSys.h>
#include <libsumo/Vehicle.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSRoute.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/transportables/MSStageDriving.h>
#include <microsim/transportables/MSStageWaiting.h>
#include <microsim/transportables/MSStageWalking.h>
#include "MSTransportableDevice_FCDReplay.h"
#include "MSDevice_FCDReplay.h"


// ===========================================================================
// static member initializations
// ===========================================================================
MSDevice_FCDReplay::FCDHandler* MSDevice_FCDReplay::myHandler = nullptr;
SUMOSAXReader* MSDevice_FCDReplay::myParser = nullptr;


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
    oc.addDescription("device.fcd-replay.file", "FCD Replay Device", TL("FCD file to read"));
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
    delete myHandler;
    myHandler = nullptr;
    const OptionsCont& oc = OptionsCont::getOptions();
    if (oc.isSet("device.fcd-replay.file")) {
        const std::string& filename = oc.getString("device.fcd-replay.file");
        myHandler = new MSDevice_FCDReplay::FCDHandler(filename);
        myParser = XMLSubSys::getSAXReader(*myHandler);
        if (!myParser->parseFirst(filename)) {
            throw ProcessError(TLF("Can not read XML-file '%'.", filename));
        }
        const SUMOTime inc = parseNext(SIMSTEP);
        MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(new MoveVehicles(), SIMSTEP + DELTA_T);
        if (inc > 0) {
            MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(new StaticCommand<MSDevice_FCDReplay>(&MSDevice_FCDReplay::parseNext),
                    SIMSTEP + inc);
        }
    }
}


SUMOTime
MSDevice_FCDReplay::parseNext(SUMOTime t) {
    SUMOTime inc = string2time(OptionsCont::getOptions().getString("route-steps"));
    // make sure that we have always at least inc time steps buffered, so at time 200 we will parse 400 to 600
    const SUMOTime start = myHandler->getTime();
    while (myHandler->getTime() < t + 2 * inc) {
        if (!myParser->parseNext()) {
            inc = 0;
            break;
        }
    }
    myHandler->updateTrafficObjects(start);
    return inc;
}


// ---------------------------------------------------------------------------
// MSDevice_FCDReplay-methods
// ---------------------------------------------------------------------------
MSDevice_FCDReplay::MSDevice_FCDReplay(SUMOVehicle& holder, const std::string& id) :
    MSVehicleDevice(holder, id) {
}


MSDevice_FCDReplay::~MSDevice_FCDReplay() {
}


void
MSDevice_FCDReplay::move(SUMOTime currentTime) {
    if (myTrajectory == nullptr || myTrajectoryIndex == (int)myTrajectory->size()) {
        // removal happens via the usual MSVehicle::hasArrived mechanism
        // TODO we may need to set an arrivalPos
        return;
    }
    MSVehicle* v = dynamic_cast<MSVehicle*>(&myHolder);
    const TrajectoryEntry& te = myTrajectory->at(myTrajectoryIndex);
    if (v == nullptr || te.time > currentTime) {
        return;
    }
    if (te.edgeOrLane != "") {
        const std::string& edgeID = SUMOXMLDefinitions::getEdgeIDFromLane(te.edgeOrLane);
        const int laneIdx = SUMOXMLDefinitions::getIndexFromLane(te.edgeOrLane);
        libsumo::Vehicle::moveToXY(myHolder.getID(), edgeID, laneIdx, te.pos.x(), te.pos.y(), te.angle, 7);
    } else {
        libsumo::Vehicle::moveToXY(myHolder.getID(), "", -1, te.pos.x(), te.pos.y(), te.angle, 0);
    }
    libsumo::Vehicle::setSpeed(myHolder.getID(), te.speed);
    // libsumo::Vehicle::changeLane(myHolder.getID(), laneIdx, TS);
    myTrajectoryIndex++;
}


SUMOTime
MSDevice_FCDReplay::MoveVehicles::execute(SUMOTime currentTime) {
    MSVehicleControl& c = MSNet::getInstance()->getVehicleControl();
    for (MSVehicleControl::constVehIt i = c.loadedVehBegin(); i != c.loadedVehEnd(); ++i) {
        MSDevice_FCDReplay* device = static_cast<MSDevice_FCDReplay*>(i->second->getDevice(typeid(MSDevice_FCDReplay)));
        if (device != nullptr && i->second->hasDeparted()) {
            device->move(currentTime);
        }
    }
    return DELTA_T;
}


// ---------------------------------------------------------------------------
// MSDevice_FCDReplay::FCDHandler-methods
// ---------------------------------------------------------------------------
MSDevice_FCDReplay::FCDHandler::FCDHandler(const std::string& file) :
    SUMOSAXHandler(file),
    MapMatcher(OptionsCont::getOptions().getBool("mapmatch.junctions"),
               OptionsCont::getOptions().getFloat("mapmatch.distance"),
               MsgHandler::getErrorInstance()) {}


void
MSDevice_FCDReplay::FCDHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    bool ok = true;
    switch (element) {
        case SUMO_TAG_TIMESTEP:
            myTime = attrs.getSUMOTimeReporting(SUMO_ATTR_TIME, "", ok);
            myPositions.clear();
            return;
        case SUMO_TAG_VEHICLE:
        case SUMO_TAG_PERSON: {
            if (myTime >= SIMSTEP) {
                const bool isPerson = element == SUMO_TAG_PERSON;
                const std::string id = attrs.getString(SUMO_ATTR_ID);
                const Position xy = Position(attrs.getOpt<double>(SUMO_ATTR_X, id.c_str(), ok, libsumo::INVALID_DOUBLE_VALUE),
                                             attrs.getOpt<double>(SUMO_ATTR_Y, id.c_str(), ok, libsumo::INVALID_DOUBLE_VALUE));
                const std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, "");
                const std::string edgeOrLane = attrs.getOpt<std::string>(isPerson ? SUMO_ATTR_EDGE : SUMO_ATTR_LANE, id.c_str(), ok, "");
                const double speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, id.c_str(), ok, libsumo::INVALID_DOUBLE_VALUE);
                const double pos = attrs.getOpt<double>(SUMO_ATTR_POSITION, id.c_str(), ok, libsumo::INVALID_DOUBLE_VALUE);
                const double angle = attrs.getOpt<double>(SUMO_ATTR_ANGLE, id.c_str(), ok, libsumo::INVALID_DOUBLE_VALUE);
                std::string vehicle = attrs.getOpt<std::string>(SUMO_ATTR_VEHICLE, id.c_str(), ok, "");
                if (isPerson) {
                    if (vehicle == "") {
                        const auto& veh = myPositions.find(xy);
                        if (veh != myPositions.end()) {
                            vehicle = veh->second;
                        }
                    }
                } else {
                    myPositions[xy] = id;
                }
                myTrajectories[id].push_back({myTime, xy, edgeOrLane, pos, speed, angle});
                const MSEdge* edge = MSEdge::dictionary(isPerson ? edgeOrLane : SUMOXMLDefinitions::getEdgeIDFromLane(edgeOrLane));
                if (edge == nullptr && edgeOrLane != "") {
                    WRITE_WARNINGF(isPerson ? TL("Unknown edge '%' in fcd replay file for person '%'.") : TL("Unknown lane '%' in fcd replay file for vehicle '%'."), edgeOrLane, id);
                }
                if (myRoutes.count(id) == 0) {
                    if (edgeOrLane == "") {
                        const MSLane* const lane = getClosestLane(xy, SVC_PASSENGER);
                        if (lane != nullptr) {
                            myTrajectories[id].back().edgeOrLane = lane->getID();
                            myTrajectories[id].back().lanePos = lane->interpolateGeometryPosToLanePos(
                                                                    lane->getShape().nearest_offset_to_point25D(xy, false));
                            edge = &lane->getEdge();
                            while (edge->isInternal()) {  // we cannot use an internal edge for a route
                                edge = edge->getSuccessors().front();
                            }
                        }
                    }
                    myRoutes[id] = std::make_tuple(myTime, type, isPerson, ConstMSEdgeVector{edge}, std::vector<StageStart>());
                } else {
                    ConstMSEdgeVector& route = std::get<3>(myRoutes[id]);
                    if (edge != nullptr && !edge->isInternal() && edge != route.back()) {
                        route.push_back(edge);
                    }
                }
                std::vector<StageStart>& vehicleUsage = std::get<4>(myRoutes[id]);
                if ((vehicleUsage.empty() && vehicle != "") || (!vehicleUsage.empty() && vehicle != vehicleUsage.back().vehicle)) {
                    vehicleUsage.push_back({vehicle, (int)myTrajectories[id].size() - 1, (int)std::get<3>(myRoutes[id]).size() - 1});
                }
            }
            return;
        }
        default:
            break;
    }
}


MSTransportable::MSTransportablePlan*
MSDevice_FCDReplay::FCDHandler::makePlan(const SUMOVehicleParameter& params, const ConstMSEdgeVector& route,
        const std::vector<StageStart>& stages, const Trajectory& t) {
    MSTransportable::MSTransportablePlan* plan = new MSTransportable::MSTransportablePlan();
    plan->push_back(new MSStageWaiting(route.front(), nullptr, 0, params.depart, params.departPos, "awaiting departure", true));
    int prevRouteOffset = 0;
    const MSEdge* start = route.front();
    std::string prevVeh;
    for (const auto& stageStart : stages) {
        if (stageStart.vehicle != prevVeh) {
            if (stageStart.trajectoryOffset != 0) {
                const MSEdge* prevEdge = MSEdge::dictionary(t[stageStart.trajectoryOffset - 1].edgeOrLane);
                if (prevVeh == "") {
                    MSStoppingPlace* finalStop = nullptr;
                    if (prevRouteOffset < (int)route.size() - 1 && (route[prevRouteOffset]->getPermissions() & SVC_PEDESTRIAN) == 0) {
                        prevRouteOffset++;  // skip the access
                    }
                    int offset = stageStart.routeOffset;
                    if (offset < (int)route.size() - 1 && (route[offset]->getPermissions() & SVC_PEDESTRIAN) == SVC_PEDESTRIAN) {
                        offset++;  // a bus stop or two consecutive walks so include the edge in both
                    } else {
                        // may have an access, let's find the stop
                        const std::string& stop = MSNet::getInstance()->getStoppingPlaceID(route[offset]->getLanes()[0], t[stageStart.trajectoryOffset].lanePos, SUMO_TAG_BUS_STOP);
                        if (stop != "") {
                            finalStop = MSNet::getInstance()->getStoppingPlace(stop, SUMO_TAG_BUS_STOP);
                        }
                    }
                    ConstMSEdgeVector subRoute = ConstMSEdgeVector(route.begin() + prevRouteOffset, route.begin() + offset);
                    plan->push_back(new MSStageWalking(params.id, subRoute, finalStop, -1, params.departSpeed, params.departPos, 0, 0));
                } else {
                    plan->push_back(new MSStageDriving(start, prevEdge, nullptr, -1, 0, {prevVeh}));
                }
                start = MSEdge::dictionary(t[stageStart.trajectoryOffset].edgeOrLane);
                prevVeh = stageStart.vehicle;
                prevRouteOffset = stageStart.routeOffset;
            }
        }
    }
    // final stage
    if (prevVeh == "") {
        if (prevRouteOffset < (int)route.size() - 1 && (route[prevRouteOffset]->getPermissions() & SVC_PEDESTRIAN) == 0) {
            prevRouteOffset++;
        }
        int offset = (int)route.size() - 1;
        if ((route[offset]->getPermissions() & SVC_PEDESTRIAN) == SVC_PEDESTRIAN) {
            offset++;
        }
        if (prevRouteOffset < offset) {
            // otherwise we may be still in a vehicle or in access, skip the stage for now
            ConstMSEdgeVector subRoute = ConstMSEdgeVector(route.begin() + prevRouteOffset, route.begin() + offset);
            plan->push_back(new MSStageWalking(params.id, subRoute, nullptr, -1, params.departSpeed, params.departPos, 0, 0));
        }
    } else {
        plan->push_back(new MSStageDriving(start, route.back(), nullptr, -1, 0, {prevVeh}));
    }
    return plan;
}


ConstMSEdgeVector
MSDevice_FCDReplay::FCDHandler::checkRoute(const ConstMSEdgeVector& edges, const SUMOVehicle* const vehicle) {
    ConstMSEdgeVector checkedRoute;
    for (const MSEdge* const e : edges) {
        if (checkedRoute.empty() || checkedRoute.back()->isConnectedTo(*e, vehicle->getVehicleType().getVehicleClass())) {
            checkedRoute.push_back(e);
        } else {
            const MSEdge* fromEdge = checkedRoute.back();
            checkedRoute.pop_back();
            if (!MSNet::getInstance()->getRouterTT(0).compute(fromEdge, e, vehicle, myTime, checkedRoute)) {
                // TODO maybe warn about disconnected route
                checkedRoute.push_back(fromEdge);
                checkedRoute.push_back(e);
            }
            // TODO check whether we introduced a big detour
        }
    }
    return checkedRoute;
}


void
MSDevice_FCDReplay::FCDHandler::updateTrafficObjects(const SUMOTime intervalStart) {
    for (const auto& desc : myRoutes) {
        const std::string& id = desc.first;
        const bool isPerson = std::get<2>(desc.second);
        Trajectory& t = myTrajectories[id];
        if (t.front().time >= intervalStart) {
            // new vehicle or person
            SUMOVehicleParameter* params = new SUMOVehicleParameter();
            params->id = id;
            params->depart = std::get<0>(desc.second);
            params->departPos = t.front().lanePos;
            params->departSpeed = t.front().speed;
            // params->arrivalPos = t.back().lanePos;
            std::string vType = std::get<1>(desc.second);
            if (vType == "") {
                vType = isPerson ? DEFAULT_PEDTYPE_ID : DEFAULT_VTYPE_ID;
            }
            MSVehicleType* vehicleType = MSNet::getInstance()->getVehicleControl().getVType(vType);
            if (vehicleType == nullptr) {
                throw ProcessError("Unknown vType '" + vType + "'.");
            }
            if (isPerson) {
                MSTransportable::MSTransportablePlan* plan = makePlan(*params, std::get<3>(desc.second), std::get<4>(desc.second), t);
                // plan completed, now build the person
                MSTransportable* person = MSNet::getInstance()->getPersonControl().buildPerson(params, vehicleType, plan, nullptr);
                person->getSingularType().setVClass(SVC_IGNORING);
                if (!MSNet::getInstance()->getPersonControl().add(person)) {
                    throw ProcessError("Duplicate person '" + id + "'.");
                }
                MSTransportableDevice_FCDReplay* device = static_cast<MSTransportableDevice_FCDReplay*>(person->getDevice(typeid(MSTransportableDevice_FCDReplay)));
                if (device == nullptr) {  // Person did not get a replay device
                    // TODO delete person
                    continue;
                }
                device->setTrajectory(&t);
            } else {
                const std::string dummyRouteID = "DUMMY_ROUTE_" + id;
                const std::vector<SUMOVehicleParameter::Stop> stops;
                const ConstMSEdgeVector& routeEdges = std::get<3>(desc.second);
                ConstMSRoutePtr route = std::make_shared<MSRoute>(dummyRouteID, routeEdges, true, nullptr, stops);
                if (!MSRoute::dictionary(dummyRouteID, route)) {
                    throw ProcessError("Could not add route '" + dummyRouteID + "'.");
                }
                if (t.front().edgeOrLane != "") {
                    params->departPosProcedure = DepartPosDefinition::GIVEN;
                    params->departLaneProcedure = DepartLaneDefinition::GIVEN;
                    params->departLane = SUMOXMLDefinitions::getIndexFromLane(t.front().edgeOrLane);
                }
                SUMOVehicle* vehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(params, route, vehicleType, false, MSVehicleControl::VehicleDefinitionSource::OTHER);
                if (!MSNet::getInstance()->getVehicleControl().addVehicle(id, vehicle)) {
                    throw ProcessError("Duplicate vehicle '" + id + "'.");
                }
                MSNet::getInstance()->getInsertionControl().add(vehicle);
                MSDevice_FCDReplay* device = static_cast<MSDevice_FCDReplay*>(vehicle->getDevice(typeid(MSDevice_FCDReplay)));
                if (device == nullptr) {  // Vehicle did not get a replay device
                    MSNet::getInstance()->getVehicleControl().deleteVehicle(vehicle, true);
                    continue;
                }
                device->setTrajectory(&t);
                static_cast<MSVehicle*>(vehicle)->getInfluencer().setSpeedMode(0);

                // repair the route, cannot do this on parsing because a vehicle is needed
                ConstMSEdgeVector checkedRoute = checkRoute(routeEdges, vehicle);
                if (checkedRoute.size() != routeEdges.size()) {
                    vehicle->replaceRouteEdges(checkedRoute, -1, 0, "FCDReplay", true);
                }
            }
        } else if (t.back().time >= intervalStart) {
            // new data for existing person / vehicle
            if (isPerson) {
                MSTransportable* person = MSNet::getInstance()->getPersonControl().get(id);
                if (person == nullptr) {
                    // TODO this should not happen
                    continue;
                }
                // TODO optimize: no need to regenerate the whole plan
                MSTransportable::MSTransportablePlan* plan = makePlan(person->getParameter(), std::get<3>(desc.second), std::get<4>(desc.second), t);
                const int stageIndex = person->getNumRemainingStages() - 1;
                MSStage* const final = person->getNextStage(stageIndex);
                bool append = false;
                for (MSStage* stage : *plan) {
                    if (stage->getStageType() == final->getStageType() && stage->getFromEdge() == final->getFromEdge()) {
                        // TODO: circular plans?
                        append = true;
                    }
                    if (append) {
                        person->appendStage(stage);
                    }
                }
                person->removeStage(stageIndex);
            } else {
                SUMOVehicle* vehicle = MSNet::getInstance()->getVehicleControl().getVehicle(id);
                const ConstMSEdgeVector& routeEdges = std::get<3>(desc.second);
                ConstMSEdgeVector checkedRoute = checkRoute(routeEdges, vehicle);
                if ((int)checkedRoute.size() != vehicle->getRoute().size()) {
                    vehicle->replaceRouteEdges(checkedRoute, -1, 0, "FCDReplay", true);
                }
            }
        }
    }
}


void
MSDevice_FCDReplay::FCDHandler::initLaneTree(NamedRTree* tree) {
    for (const auto& edge : MSEdge::getAllEdges()) {
        for (MSLane* lane : edge->getLanes()) {
            Boundary b = lane->getShape().getBoxBoundary();
            const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
            const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
            tree->Insert(cmin, cmax, lane);
        }
    }
}


MSEdge*
MSDevice_FCDReplay::FCDHandler::retrieveEdge(const std::string& id) {
    return MSEdge::dictionary(id);
}


/****************************************************************************/
