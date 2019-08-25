/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Vehicle.cpp
/// @author  Jakob Erdmann
/// @date    15.03.2017
/// @version $Id$
///
// C++ Vehicle API
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/geom/GeomHelper.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/devices/MSDevice.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSParkingArea.h>
#include <libsumo/TraCIDefs.h>
#include <libsumo/TraCIConstants.h>
#include "Helper.h"
#include "Route.h"
#include "Polygon.h"
#include "Vehicle.h"


// ===========================================================================
// debug defines
// ===========================================================================
//#define DEBUG_NEIGHBORS
//#define DEBUG_DYNAMIC_SHAPES
//#define DEBUG_MOVEXY
#define DEBUG_COND (veh->isSelected())



namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults Vehicle::mySubscriptionResults;
ContextSubscriptionResults Vehicle::myContextSubscriptionResults;


// ===========================================================================
// static member definitions
// ===========================================================================
bool
Vehicle::isVisible(const SUMOVehicle* veh) {
    return veh->isOnRoad() || veh->isParking() || veh->wasRemoteControlled();
}


bool
Vehicle::isOnInit(const std::string& vehicleID) {
    SUMOVehicle* sumoVehicle = MSNet::getInstance()->getVehicleControl().getVehicle(vehicleID);
    return sumoVehicle == nullptr || sumoVehicle->getLane() == nullptr;
}


std::vector<std::string>
Vehicle::getIDList() {
    std::vector<std::string> ids;
    MSVehicleControl& c = MSNet::getInstance()->getVehicleControl();
    for (MSVehicleControl::constVehIt i = c.loadedVehBegin(); i != c.loadedVehEnd(); ++i) {
        if (isVisible((*i).second)) {
            ids.push_back((*i).first);
        }
    }
    return ids;
}

int
Vehicle::getIDCount() {
    return (int)getIDList().size();
}


double
Vehicle::getSpeed(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? veh->getSpeed() : INVALID_DOUBLE_VALUE;
}

double
Vehicle::getLateralSpeed(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? veh->getLaneChangeModel().getSpeedLat() : INVALID_DOUBLE_VALUE;
}


double
Vehicle::getAcceleration(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? veh->getAcceleration() : INVALID_DOUBLE_VALUE;
}


double
Vehicle::getSpeedWithoutTraCI(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? veh->getSpeedWithoutTraciInfluence() : INVALID_DOUBLE_VALUE;
}


TraCIPosition
Vehicle::getPosition(const std::string& vehicleID, const bool includeZ) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (isVisible(veh)) {
        return Helper::makeTraCIPosition(veh->getPosition(), includeZ);
    }
    return TraCIPosition();
}


TraCIPosition
Vehicle::getPosition3D(const std::string& vehicleID) {
    return getPosition(vehicleID, true);
}


double
Vehicle::getAngle(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? GeomHelper::naviDegree(veh->getAngle()) : INVALID_DOUBLE_VALUE;
}


double
Vehicle::getSlope(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return veh->isOnRoad() ? veh->getSlope() : INVALID_DOUBLE_VALUE;
}


std::string
Vehicle::getRoadID(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? veh->getLane()->getEdge().getID() : "";
}


std::string
Vehicle::getLaneID(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return veh->isOnRoad() ? veh->getLane()->getID() : "";
}


int
Vehicle::getLaneIndex(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return veh->isOnRoad() ? veh->getLane()->getIndex() : INVALID_INT_VALUE;
}


std::string
Vehicle::getTypeID(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getID();
}


std::string
Vehicle::getRouteID(const std::string& vehicleID) {
    return Helper::getVehicle(vehicleID)->getRoute().getID();
}


int
Vehicle::getRouteIndex(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return veh->hasDeparted() ? veh->getRoutePosition() : INVALID_INT_VALUE;
}


TraCIColor
Vehicle::getColor(const std::string& vehicleID) {
    return Helper::makeTraCIColor(Helper::getVehicle(vehicleID)->getParameter().color);
}

double
Vehicle::getLanePosition(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return veh->isOnRoad() ? veh->getPositionOnLane() : INVALID_DOUBLE_VALUE;
}

double
Vehicle::getLateralLanePosition(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return veh->isOnRoad() ? veh->getLateralPositionOnLane() : INVALID_DOUBLE_VALUE;
}

double
Vehicle::getCO2Emission(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? veh->getCO2Emissions() : INVALID_DOUBLE_VALUE;
}

double
Vehicle::getCOEmission(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? veh->getCOEmissions() : INVALID_DOUBLE_VALUE;
}

double
Vehicle::getHCEmission(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? veh->getHCEmissions() : INVALID_DOUBLE_VALUE;
}

double
Vehicle::getPMxEmission(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? veh->getPMxEmissions() : INVALID_DOUBLE_VALUE;
}

double
Vehicle::getNOxEmission(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? veh->getNOxEmissions() : INVALID_DOUBLE_VALUE;
}

double
Vehicle::getFuelConsumption(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? veh->getFuelConsumption() : INVALID_DOUBLE_VALUE;
}

double
Vehicle::getNoiseEmission(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? veh->getHarmonoise_NoiseEmissions() : INVALID_DOUBLE_VALUE;
}

double
Vehicle::getElectricityConsumption(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    return isVisible(veh) ? veh->getElectricityConsumption() : INVALID_DOUBLE_VALUE;
}

int
Vehicle::getPersonNumber(const std::string& vehicleID) {
    return Helper::getVehicle(vehicleID)->getPersonNumber();
}

int
Vehicle::getPersonCapacity(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getPersonCapacity();
}

std::vector<std::string>
Vehicle::getPersonIDList(const std::string& vehicleID) {
    return Helper::getVehicle(vehicleID)->getPersonIDList();
}

std::pair<std::string, double>
Vehicle::getLeader(const std::string& vehicleID, double dist) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (veh->isOnRoad()) {
        std::pair<const MSVehicle* const, double> leaderInfo = veh->getLeader(dist);
        return std::make_pair(
                   leaderInfo.first != nullptr ? leaderInfo.first->getID() : "",
                   leaderInfo.second);
    } else {
        return std::make_pair("", -1);
    }
}


double
Vehicle::getWaitingTime(const std::string& vehicleID) {
    return Helper::getVehicle(vehicleID)->getWaitingSeconds();
}


double
Vehicle::getAccumulatedWaitingTime(const std::string& vehicleID) {
    return Helper::getVehicle(vehicleID)->getAccumulatedWaitingSeconds();
}


double
Vehicle::getAdaptedTraveltime(const std::string& vehicleID, double time, const std::string& edgeID) {
    MSEdge* edge = Helper::getEdge(edgeID);
    double value = INVALID_DOUBLE_VALUE;
    Helper::getVehicle(vehicleID)->getWeightsStorage().retrieveExistingTravelTime(edge, time, value);
    return value;
}


double
Vehicle::getEffort(const std::string& vehicleID, double time, const std::string& edgeID) {
    MSEdge* edge = Helper::getEdge(edgeID);
    double value = INVALID_DOUBLE_VALUE;
    Helper::getVehicle(vehicleID)->getWeightsStorage().retrieveExistingEffort(edge, time, value);
    return value;
}


bool
Vehicle::isRouteValid(const std::string& vehicleID) {
    std::string msg;
    return Helper::getVehicle(vehicleID)->hasValidRoute(msg);
}


std::vector<std::string>
Vehicle::getRoute(const std::string& vehicleID) {
    std::vector<std::string> result;
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    const MSRoute& r = veh->getRoute();
    for (MSRouteIterator i = r.begin(); i != r.end(); ++i) {
        result.push_back((*i)->getID());
    }
    return result;
}


int
Vehicle::getSignals(const std::string& vehicleID) {
    return Helper::getVehicle(vehicleID)->getSignals();
}


std::vector<TraCIBestLanesData>
Vehicle::getBestLanes(const std::string& vehicleID) {
    std::vector<TraCIBestLanesData> result;
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (veh->isOnRoad()) {
        const std::vector<MSVehicle::LaneQ>& bestLanes = veh->getBestLanes();
        for (std::vector<MSVehicle::LaneQ>::const_iterator i = bestLanes.begin(); i != bestLanes.end(); ++i) {
            TraCIBestLanesData bld;
            const MSVehicle::LaneQ& lq = *i;
            bld.laneID = lq.lane->getID();
            bld.length = lq.length;
            bld.occupation = lq.nextOccupation;
            bld.bestLaneOffset = lq.bestLaneOffset;
            bld.allowsContinuation = lq.allowsContinuation;
            for (std::vector<MSLane*>::const_iterator j = lq.bestContinuations.begin(); j != lq.bestContinuations.end(); ++j) {
                if ((*j) != nullptr) {
                    bld.continuationLanes.push_back((*j)->getID());
                }
            }
            result.push_back(bld);
        }
    }
    return result;
}


std::vector<TraCINextTLSData>
Vehicle::getNextTLS(const std::string& vehicleID) {
    std::vector<TraCINextTLSData> result;
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (veh->isOnRoad()) {
        const MSLane* lane = veh->getLane();
        const std::vector<MSLane*>& bestLaneConts = veh->getBestLanesContinuation(lane);
        double seen = veh->getLane()->getLength() - veh->getPositionOnLane();
        int view = 1;
        MSLinkCont::const_iterator link = MSLane::succLinkSec(*veh, view, *lane, bestLaneConts);
        while (!lane->isLinkEnd(link)) {
            if (!lane->getEdge().isInternal()) {
                if ((*link)->isTLSControlled()) {
                    TraCINextTLSData ntd;
                    ntd.id = (*link)->getTLLogic()->getID();
                    ntd.tlIndex = (*link)->getTLIndex();
                    ntd.dist = seen;
                    ntd.state = (char)(*link)->getState();
                    result.push_back(ntd);
                }
            }
            lane = (*link)->getViaLaneOrLane();
            if (!lane->getEdge().isInternal()) {
                view++;
            }
            seen += lane->getLength();
            link = MSLane::succLinkSec(*veh, view, *lane, bestLaneConts);
        }
        // consider edges beyond bestLanes
        const int remainingEdges = (int)(veh->getRoute().end() - veh->getCurrentRouteEdge()) - view;
        //std::cout << SIMTIME << "remainingEdges=" << remainingEdges << " seen=" << seen << " view=" << view << " best=" << toString(bestLaneConts) << "\n";
        for (int i = 0; i < remainingEdges; i++) {
            const MSEdge* prev = *(veh->getCurrentRouteEdge() + view + i - 1);
            const MSEdge* next = *(veh->getCurrentRouteEdge() + view + i);
            const std::vector<MSLane*>* allowed = prev->allowedLanes(*next, veh->getVClass());
            if (allowed != nullptr && allowed->size() != 0) {
                for (MSLink* link : allowed->front()->getLinkCont()) {
                    if (&link->getLane()->getEdge() == next) {
                        if (link->isTLSControlled()) {
                            TraCINextTLSData ntd;
                            ntd.id = link->getTLLogic()->getID();
                            ntd.tlIndex = link->getTLIndex();
                            ntd.dist = seen;
                            ntd.state = (char)link->getState();
                            result.push_back(ntd);
                        }
                        seen += allowed->front()->getLength();
                    }
                }
            } else {
                // invalid route, cannot determine nextTLS
                break;
            }
        }
    }
    return result;
}


std::vector<TraCINextStopData>
Vehicle::getNextStops(const std::string& vehicleID) {
    std::vector<TraCINextStopData> result;
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    std::list<MSVehicle::Stop> stops = veh->getMyStops();
    for (std::list<MSVehicle::Stop>::iterator it = stops.begin(); it != stops.end(); ++it) {
        if (!it->collision) {
            TraCINextStopData nsd;
            nsd.lane = it->lane->getID();
            nsd.endPos = it->getEndPos(*veh);
            // all optionals, only one can be set
            if (it->busstop != nullptr) {
                nsd.stoppingPlaceID = it->busstop->getID();
            }
            if (it->containerstop != nullptr) {
                nsd.stoppingPlaceID = it->containerstop->getID();
            }
            if (it->parkingarea != nullptr) {
                nsd.stoppingPlaceID = it->parkingarea->getID();
            }
            if (it->chargingStation != nullptr) {
                nsd.stoppingPlaceID = it->chargingStation->getID();
            }
            nsd.stopFlags = ((it->reached ? 1 : 0) +
                             (it->pars.parking ? 2 : 0) +
                             (it->pars.triggered ? 4 : 0) +
                             (it->pars.containerTriggered ? 8 : 0) +
                             (it->busstop != nullptr ? 16 : 0) +
                             (it->containerstop != nullptr ? 32 : 0) +
                             (it->chargingStation != nullptr ? 64 : 0) +
                             (it->parkingarea != nullptr ? 128 : 0));
            nsd.duration = STEPS2TIME(it->reached ? it->duration : it->pars.duration);
            nsd.until = STEPS2TIME(it->pars.until);
            result.push_back(nsd);
        }
    }
    return result;
}


int
Vehicle::getStopState(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    int result = 0;
    if (veh->isStopped()) {
        const MSVehicle::Stop& stop = veh->getNextStop();
        result = ((stop.reached ? 1 : 0) +
                  (stop.pars.parking ? 2 : 0) +
                  (stop.pars.triggered ? 4 : 0) +
                  (stop.pars.containerTriggered ? 8 : 0) +
                  (stop.busstop != nullptr ? 16 : 0) +
                  (stop.containerstop != nullptr ? 32 : 0) +
                  (stop.chargingStation != nullptr ? 64 : 0) +
                  (stop.parkingarea != nullptr ? 128 : 0));
    }
    return result;
}


double
Vehicle::getDistance(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (veh->isOnRoad()) {
        double distance;
        if (veh->getLane()->isInternal()) {
            // route edge still points to the edge before the intersection
            const double normalEnd = (*veh->getCurrentRouteEdge())->getLength();
            distance = (veh->getRoute().getDistanceBetween(veh->getDepartPos(), normalEnd,
                        veh->getRoute().begin(), veh->getCurrentRouteEdge())
                        + veh->getRoute().getDistanceBetween(normalEnd, veh->getPositionOnLane(),
                                *veh->getCurrentRouteEdge(), &veh->getLane()->getEdge()));
        } else {
            distance = veh->getRoute().getDistanceBetween(veh->getDepartPos(), veh->getPositionOnLane(),
                       veh->getRoute().begin(), veh->getCurrentRouteEdge());
        }
        if (distance == std::numeric_limits<double>::max()) {
            return INVALID_DOUBLE_VALUE;
        } else {
            return distance;
        }
    } else {
        return INVALID_DOUBLE_VALUE;
    }
}


double
Vehicle::getDrivingDistance(const std::string& vehicleID, const std::string& edgeID, double position, int /* laneIndex */) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (veh->isOnRoad()) {
        double distance = veh->getRoute().getDistanceBetween(veh->getPositionOnLane(), position,
                          &veh->getLane()->getEdge(), Helper::getEdge(edgeID), true, veh->getRoutePosition());
        if (distance == std::numeric_limits<double>::max()) {
            return INVALID_DOUBLE_VALUE;
        }
        return distance;
    } else {
        return INVALID_DOUBLE_VALUE;
    }
}


double
Vehicle::getDrivingDistance2D(const std::string& vehicleID, double x, double y) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (veh->isOnRoad()) {
        std::pair<MSLane*, double> roadPos = Helper::convertCartesianToRoadMap(Position(x, y), veh->getVehicleType().getVehicleClass());
        double distance = veh->getRoute().getDistanceBetween(veh->getPositionOnLane(), roadPos.second,
                          veh->getEdge(), &roadPos.first->getEdge(), true, veh->getRoutePosition());
        if (distance == std::numeric_limits<double>::max()) {
            return INVALID_DOUBLE_VALUE;
        }
        return distance;
    } else {
        return INVALID_DOUBLE_VALUE;
    }
}


double
Vehicle::getAllowedSpeed(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (veh->isOnRoad()) {
        return veh->getLane()->getVehicleMaxSpeed(veh);
    } else {
        return INVALID_DOUBLE_VALUE;
    }
}


double
Vehicle::getSpeedFactor(const std::string& vehicleID) {
    return Helper::getVehicle(vehicleID)->getChosenSpeedFactor();
}


int
Vehicle::getSpeedMode(const std::string& vehicleID) {
    return Helper::getVehicle(vehicleID)->getInfluencer().getSpeedMode();
}


int
Vehicle::getLaneChangeMode(const std::string& vehicleID) {
    return Helper::getVehicle(vehicleID)->getInfluencer().getLaneChangeMode();
}


int
Vehicle::getRoutingMode(const std::string& vehicleID) {
    return Helper::getVehicle(vehicleID)->getInfluencer().getRoutingMode();
}


std::string
Vehicle::getLine(const std::string& vehicleID) {
    return Helper::getVehicle(vehicleID)->getParameter().line;
}


std::vector<std::string>
Vehicle::getVia(const std::string& vehicleID) {
    return Helper::getVehicle(vehicleID)->getParameter().via;
}


std::pair<int, int>
Vehicle::getLaneChangeState(const std::string& vehicleID, int direction) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (veh->isOnRoad()) {
        return veh->getLaneChangeModel().getSavedState(direction);
    } else {
        return std::make_pair((int)LCA_UNKNOWN, (int)LCA_UNKNOWN);
    }
}


std::string
Vehicle::getParameter(const std::string& vehicleID, const std::string& key) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (StringUtils::startsWith(key, "device.")) {
        StringTokenizer tok(key, ".");
        if (tok.size() < 3) {
            throw TraCIException("Invalid device parameter '" + key + "' for vehicle '" + vehicleID + "'.");
        }
        try {
            return veh->getDeviceParameter(tok.get(1), key.substr(tok.get(0).size() + tok.get(1).size() + 2));
        } catch (InvalidArgument& e) {
            throw TraCIException("Vehicle '" + vehicleID + "' does not support device parameter '" + key + "' (" + e.what() + ").");
        }
    } else if (StringUtils::startsWith(key, "laneChangeModel.")) {
        const std::string attrName = key.substr(16);
        try {
            return veh->getLaneChangeModel().getParameter(attrName);
        } catch (InvalidArgument& e) {
            throw TraCIException("Vehicle '" + vehicleID + "' does not support laneChangeModel parameter '" + key + "' (" + e.what() + ").");
        }
    } else if (StringUtils::startsWith(key, "carFollowModel.")) {
        const std::string attrName = key.substr(15);
        try {
            return veh->getCarFollowModel().getParameter(veh, attrName);
        } catch (InvalidArgument& e) {
            throw TraCIException("Vehicle '" + vehicleID + "' does not support carFollowModel parameter '" + key + "' (" + e.what() + ").");
        }
    } else if (StringUtils::startsWith(key, "has.") && StringUtils::endsWith(key, ".device")) {
        StringTokenizer tok(key, ".");
        if (tok.size() != 3) {
            throw TraCIException("Invalid check for device. Expected format is 'has.DEVICENAME.device'.");
        }
        return veh->hasDevice(tok.get(1)) ? "true" : "false";
    } else {
        return veh->getParameter().getParameter(key, "");
    }
}


std::vector<std::pair<std::string, double> >
Vehicle::getNeighbors(const std::string& vehicleID, const int mode) {
    int dir = (1 & mode) != 0 ? -1 : 1;
    bool queryLeaders = (2 & mode) != 0;
    bool blockersOnly = (4 & mode) != 0;

    MSVehicle* veh = Helper::getVehicle(vehicleID);
    std::vector<std::pair<std::string, double> > neighs;
    auto& lcm = veh->getLaneChangeModel();

#ifdef DEBUG_NEIGHBORS
    if (DEBUG_COND) {
        std::cout << "getNeighbors() for veh '" << vehicleID << "': dir=" << dir
                  << ", queryLeaders=" << queryLeaders
                  << ", blockersOnly=" << blockersOnly << std::endl;
    }
#endif



    if (blockersOnly) {
        // Check if a blocking neigh exists in the given direction
        bool blocked = false;
        if (dir == -1) {
            if (queryLeaders) {
                blocked = (lcm.getOwnState() & LCA_BLOCKED_BY_RIGHT_LEADER) != 0;
            } else {
                blocked = (lcm.getOwnState() & LCA_BLOCKED_BY_RIGHT_FOLLOWER) != 0;
            }
        } else {
            if (queryLeaders) {
                blocked = (lcm.getOwnState() & LCA_BLOCKED_BY_LEFT_LEADER) != 0;
            } else {
                blocked = (lcm.getOwnState() & LCA_BLOCKED_BY_LEFT_FOLLOWER) != 0;
            }
        }

#ifdef DEBUG_NEIGHBORS
        if (DEBUG_COND) {
            std::cout << " blocked=" << blocked << std::endl;
        }
#endif

        if (!blocked) {
            // Not blocked => return empty vector
            return neighs;
        }
    }

    const std::shared_ptr<MSLeaderDistanceInfo> res = queryLeaders ? lcm.getLeaders(dir) : lcm.getFollowers(dir);
    if (res != nullptr && res->hasVehicles()) {
        auto distIt = begin(res->getDistances());
        auto vehIt = begin(res->getVehicles());
        while (distIt != end(res->getDistances())) {
            if (*vehIt != nullptr) {
                if (neighs.size() == 0 || neighs.back().first != (*vehIt)->getID()) {
                    neighs.push_back(std::make_pair((*vehIt)->getID(), *distIt));
                }
            }
            ++vehIt;
            ++distIt;
        }
    }
    return neighs;
}


std::string
Vehicle::getEmissionClass(const std::string& vehicleID) {
    return PollutantsInterface::getName(Helper::getVehicleType(vehicleID).getEmissionClass());
}

std::string
Vehicle::getShapeClass(const std::string& vehicleID) {
    return getVehicleShapeName(Helper::getVehicleType(vehicleID).getGuiShape());
}


double
Vehicle::getLength(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getLength();
}


double
Vehicle::getAccel(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getCarFollowModel().getMaxAccel();
}


double
Vehicle::getDecel(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getCarFollowModel().getMaxDecel();
}


double Vehicle::getEmergencyDecel(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getCarFollowModel().getEmergencyDecel();
}


double Vehicle::getApparentDecel(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getCarFollowModel().getApparentDecel();
}


double Vehicle::getActionStepLength(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getActionStepLengthSecs();
}


double Vehicle::getLastActionTime(const std::string& vehicleID) {
    return STEPS2TIME(Helper::getVehicle(vehicleID)->getLastActionTime());
}


double
Vehicle::getTau(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getCarFollowModel().getHeadwayTime();
}


double
Vehicle::getImperfection(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getCarFollowModel().getImperfection();
}


double
Vehicle::getSpeedDeviation(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getSpeedFactor().getParameter()[1];
}


std::string
Vehicle::getVehicleClass(const std::string& vehicleID) {
    return toString(Helper::getVehicleType(vehicleID).getVehicleClass());
}


double
Vehicle::getMinGap(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getMinGap();
}


double
Vehicle::getMinGapLat(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getMinGapLat();
}


double
Vehicle::getMaxSpeed(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getMaxSpeed();
}


double
Vehicle::getMaxSpeedLat(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getMaxSpeedLat();
}


std::string
Vehicle::getLateralAlignment(const std::string& vehicleID) {
    return toString(Helper::getVehicleType(vehicleID).getPreferredLateralAlignment());
}


double
Vehicle::getWidth(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getWidth();
}


double
Vehicle::getHeight(const std::string& vehicleID) {
    return Helper::getVehicleType(vehicleID).getHeight();
}


void
Vehicle::setStop(const std::string& vehicleID,
                 const std::string& edgeID,
                 double pos,
                 int laneIndex,
                 double duration,
                 int flags,
                 double startPos,
                 double until) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    // optional stop flags
    bool parking = false;
    bool triggered = false;
    bool containerTriggered = false;
    SumoXMLTag stoppingPlaceType = SUMO_TAG_NOTHING;

    parking = ((flags & 1) != 0);
    triggered = ((flags & 2) != 0);
    containerTriggered = ((flags & 4) != 0);
    if ((flags & 8) != 0) {
        stoppingPlaceType = SUMO_TAG_BUS_STOP;
    }
    if ((flags & 16) != 0) {
        stoppingPlaceType = SUMO_TAG_CONTAINER_STOP;
    }
    if ((flags & 32) != 0) {
        stoppingPlaceType = SUMO_TAG_CHARGING_STATION;
    }
    if ((flags & 64) != 0) {
        stoppingPlaceType = SUMO_TAG_PARKING_AREA;
    }
    const SUMOTime durationSteps = duration == INVALID_DOUBLE_VALUE ? SUMOTime_MAX : TIME2STEPS(duration);
    const SUMOTime untilStep = until == INVALID_DOUBLE_VALUE ? -1 : TIME2STEPS(until);

    std::string error;
    if (stoppingPlaceType != SUMO_TAG_NOTHING) {
        // Forward command to vehicle
        if (!veh->addTraciStopAtStoppingPlace(edgeID, durationSteps, untilStep, parking, triggered, containerTriggered, stoppingPlaceType, error)) {
            throw TraCIException(error);
        }
    } else {
        // check
        if (startPos == INVALID_DOUBLE_VALUE) {
            startPos = pos - POSITION_EPS;
        }
        if (startPos < 0.) {
            throw TraCIException("Position on lane must not be negative.");
        }
        if (pos < startPos) {
            throw TraCIException("End position on lane must be after start position.");
        }
        // get the actual lane that is referenced by laneIndex
        MSEdge* road = MSEdge::dictionary(edgeID);
        if (road == nullptr) {
            throw TraCIException("Edge '" + edgeID + "' is not known.");
        }
        const std::vector<MSLane*>& allLanes = road->getLanes();
        if ((laneIndex < 0) || laneIndex >= (int)(allLanes.size())) {
            throw TraCIException("No lane with index '" + toString(laneIndex) + "' on edge '" + edgeID + "'.");
        }
        // Forward command to vehicle
        if (!veh->addTraciStop(allLanes[laneIndex], startPos, pos, durationSteps, untilStep, parking, triggered, containerTriggered, error)) {
            throw TraCIException(error);
        }
    }
}

void
Vehicle::rerouteParkingArea(const std::string& vehicleID, const std::string& parkingAreaID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    std::string error;
    // Forward command to vehicle
    if (!veh->rerouteParkingArea(parkingAreaID, error)) {
        throw TraCIException(error);
    }
}

void
Vehicle::resume(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (!veh->hasStops()) {
        throw TraCIException("Failed to resume vehicle '" + veh->getID() + "', it has no stops.");
    }
    if (!veh->resumeFromStopping()) {
        MSVehicle::Stop& sto = veh->getNextStop();
        std::ostringstream strs;
        strs << "reached: " << sto.reached;
        strs << ", duration:" << sto.duration;
        strs << ", edge:" << (*sto.edge)->getID();
        strs << ", startPos: " << sto.pars.startPos;
        std::string posStr = strs.str();
        throw TraCIException("Failed to resume from stopping for vehicle '" + veh->getID() + "', " + posStr);
    }
}


void
Vehicle::changeTarget(const std::string& vehicleID, const std::string& edgeID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    const MSEdge* destEdge = MSEdge::dictionary(edgeID);
    const bool onInit = isOnInit(vehicleID);
    if (destEdge == nullptr) {
        throw TraCIException("Destination edge '" + edgeID + "' is not known.");
    }
    // build a new route between the vehicle's current edge and destination edge
    ConstMSEdgeVector newRoute;
    const MSEdge* currentEdge = veh->getRerouteOrigin();
    veh->getInfluencer().getRouterTT().compute(
        currentEdge, destEdge, (const MSVehicle * const)veh, MSNet::getInstance()->getCurrentTimeStep(), newRoute);
    // replace the vehicle's route by the new one (cost is updated by call to reroute())
    if (!veh->replaceRouteEdges(newRoute, -1, 0, "traci:changeTarget", onInit)) {
        throw TraCIException("Route replacement failed for vehicle '" + veh->getID() + "'.");
    }
    // route again to ensure usage of via/stops
    try {
        veh->reroute(MSNet::getInstance()->getCurrentTimeStep(), "traci:changeTarget", veh->getInfluencer().getRouterTT(), onInit);
    } catch (ProcessError& e) {
        throw TraCIException(e.what());
    }
}


void
Vehicle::changeLane(const std::string& vehicleID, int laneIndex, double duration) {
    std::vector<std::pair<SUMOTime, int> > laneTimeLine;
    laneTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), laneIndex));
    laneTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(duration), laneIndex));
    Helper::getVehicle(vehicleID)->getInfluencer().setLaneTimeLine(laneTimeLine);
}

void
Vehicle::changeLaneRelative(const std::string& vehicleID, int indexOffset, double duration) {
    std::vector<std::pair<SUMOTime, int> > laneTimeLine;
    int laneIndex = Helper::getVehicle(vehicleID)->getLaneIndex() + indexOffset;
    laneTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), laneIndex));
    laneTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(duration), laneIndex));
    Helper::getVehicle(vehicleID)->getInfluencer().setLaneTimeLine(laneTimeLine);
}


void
Vehicle::changeSublane(const std::string& vehicleID, double latDist) {
    Helper::getVehicle(vehicleID)->getInfluencer().setSublaneChange(latDist);
}


void
Vehicle::add(const std::string& vehicleID,
             const std::string& routeID,
             const std::string& typeID,
             const std::string& depart,
             const std::string& departLane,
             const std::string& departPos,
             const std::string& departSpeed,
             const std::string& arrivalLane,
             const std::string& arrivalPos,
             const std::string& arrivalSpeed,
             const std::string& fromTaz,
             const std::string& toTaz,
             const std::string& line,
             int /*personCapacity*/,
             int personNumber) {
    SUMOVehicle* veh = MSNet::getInstance()->getVehicleControl().getVehicle(vehicleID);
    if (veh != nullptr) {
        throw TraCIException("The vehicle '" + vehicleID + "' to add already exists.");
    }

    SUMOVehicleParameter vehicleParams;
    vehicleParams.id = vehicleID;
    MSVehicleType* vehicleType = MSNet::getInstance()->getVehicleControl().getVType(typeID);
    if (!vehicleType) {
        throw TraCIException("Invalid type '" + typeID + "' for vehicle '" + vehicleID + "'.");
    }
    const MSRoute* route = MSRoute::dictionary(routeID);
    if (!route) {
        if (routeID == "") {
            // assume, route was intentionally left blank because the caller
            // intends to control the vehicle remotely
            SUMOVehicleClass vclass = vehicleType->getVehicleClass();
            const std::string dummyRouteID = "DUMMY_ROUTE_" + SumoVehicleClassStrings.getString(vclass);
            route = MSRoute::dictionary(dummyRouteID);
            if (route == nullptr) {
                for (MSEdge* e : MSEdge::getAllEdges()) {
                    if (e->getFunction() == EDGEFUNC_NORMAL && (e->getPermissions() & vclass) == vclass) {
                        std::vector<std::string>  edges;
                        edges.push_back(e->getID());
                        libsumo::Route::add(dummyRouteID, edges);
                        break;
                    }
                }
            }
            route = MSRoute::dictionary(dummyRouteID);
            if (!route) {
                throw TraCIException("Could not build dummy route for vehicle class: '" + SumoVehicleClassStrings.getString(vehicleType->getVehicleClass()) + "'");
            }
        } else {
            throw TraCIException("Invalid route '" + routeID + "' for vehicle '" + vehicleID + "'.");
        }
    }
    // check if the route implies a trip
    if (route->getEdges().size() == 2) {
        const MSEdgeVector& succ = route->getEdges().front()->getSuccessors();
        if (std::find(succ.begin(), succ.end(), route->getEdges().back()) == succ.end()) {
            vehicleParams.parametersSet |= VEHPARS_FORCE_REROUTE;
        }
    }
    if (fromTaz != "" || toTaz != "") {
        vehicleParams.parametersSet |= VEHPARS_FORCE_REROUTE;
    }
    std::string error;
    if (!SUMOVehicleParameter::parseDepart(depart, "vehicle", vehicleID, vehicleParams.depart, vehicleParams.departProcedure, error)) {
        throw TraCIException(error);
    }
    if (vehicleParams.departProcedure == DEPART_GIVEN && vehicleParams.depart < MSNet::getInstance()->getCurrentTimeStep()) {
        vehicleParams.depart = MSNet::getInstance()->getCurrentTimeStep();
        WRITE_WARNING("Departure time for vehicle '" + vehicleID + "' is in the past; using current time instead.");
    } else if (vehicleParams.departProcedure == DEPART_NOW) {
        vehicleParams.depart = MSNet::getInstance()->getCurrentTimeStep();
    }
    if (!SUMOVehicleParameter::parseDepartLane(departLane, "vehicle", vehicleID, vehicleParams.departLane, vehicleParams.departLaneProcedure, error)) {
        throw TraCIException(error);
    }
    if (!SUMOVehicleParameter::parseDepartPos(departPos, "vehicle", vehicleID, vehicleParams.departPos, vehicleParams.departPosProcedure, error)) {
        throw TraCIException(error);
    }
    if (!SUMOVehicleParameter::parseDepartSpeed(departSpeed, "vehicle", vehicleID, vehicleParams.departSpeed, vehicleParams.departSpeedProcedure, error)) {
        throw TraCIException(error);
    }
    if (!SUMOVehicleParameter::parseArrivalLane(arrivalLane, "vehicle", vehicleID, vehicleParams.arrivalLane, vehicleParams.arrivalLaneProcedure, error)) {
        throw TraCIException(error);
    }
    if (!SUMOVehicleParameter::parseArrivalPos(arrivalPos, "vehicle", vehicleID, vehicleParams.arrivalPos, vehicleParams.arrivalPosProcedure, error)) {
        throw TraCIException(error);
    }
    if (!SUMOVehicleParameter::parseArrivalSpeed(arrivalSpeed, "vehicle", vehicleID, vehicleParams.arrivalSpeed, vehicleParams.arrivalSpeedProcedure, error)) {
        throw TraCIException(error);
    }
    vehicleParams.fromTaz = fromTaz;
    vehicleParams.toTaz = toTaz;
    vehicleParams.line = line;
    //vehicleParams.personCapacity = personCapacity;
    vehicleParams.personNumber = personNumber;

    SUMOVehicleParameter* params = new SUMOVehicleParameter(vehicleParams);
    try {
        SUMOVehicle* vehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(params, route, vehicleType, true, false);
        MSNet::getInstance()->getVehicleControl().addVehicle(vehicleParams.id, vehicle);
        MSNet::getInstance()->getInsertionControl().add(vehicle);
    } catch (ProcessError& e) {
        throw TraCIException(e.what());
    }
}


void
Vehicle::moveToXY(const std::string& vehicleID, const std::string& edgeID, const int laneIndex,
                  const double x, const double y, double angle, const int keepRoute) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    const bool doKeepRoute = (keepRoute & 1) != 0 && veh->getID() != "VTD_EGO";
    const bool mayLeaveNetwork = (keepRoute & 2) != 0;
    const bool ignorePermissions = (keepRoute & 4) != 0;
    SUMOVehicleClass vClass = ignorePermissions ? SVC_IGNORING : veh->getVClass();
    // process
    const std::string origID = edgeID + "_" + toString(laneIndex);
    // @todo add an interpretation layer for OSM derived origID values (without lane index)
    Position pos(x, y);
#ifdef DEBUG_MOVEXY
    const double origAngle = angle;
#endif
    // angle must be in [0,360] because it will be compared against those returned by naviDegree()
    // angle set to INVALID_DOUBLE_VALUE is ignored in the evaluated and later set to the angle of the matched lane
    if (angle != INVALID_DOUBLE_VALUE) {
        while (angle >= 360.) {
            angle -= 360.;
        }
        while (angle < 0.) {
            angle += 360.;
        }
    }

    Position vehPos = veh->getPosition();
#ifdef DEBUG_MOVEXY
    std::cout << std::endl << SIMTIME << " moveToXY veh=" << veh->getID() << " vehPos=" << vehPos << " lane=" << Named::getIDSecure(veh->getLane()) << std::endl;
    std::cout << " wantedPos=" << pos << " origID=" << origID << " laneIndex=" << laneIndex << " origAngle=" << origAngle << " angle=" << angle << " keepRoute=" << keepRoute << std::endl;
#endif

    ConstMSEdgeVector edges;
    MSLane* lane = nullptr;
    double lanePos;
    double lanePosLat = 0;
    double bestDistance = std::numeric_limits<double>::max();
    int routeOffset = 0;
    bool found;
    double maxRouteDistance = 100;
    /* EGO vehicle is known to have a fixed route. @todo make this into a parameter of the TraCI call */
    if (doKeepRoute) {
        // case a): vehicle is on its earlier route
        //  we additionally assume it is moving forward (SUMO-limit);
        //  note that the route ("edges") is not changed in this case

        found = Helper::moveToXYMap_matchingRoutePosition(pos, origID,
                veh->getRoute().getEdges(), (int)(veh->getCurrentRouteEdge() - veh->getRoute().begin()),
                vClass,
                bestDistance, &lane, lanePos, routeOffset);
        // @note silenty ignoring mapping failure
    } else {
        double speed = pos.distanceTo2D(veh->getPosition()); // !!!veh->getSpeed();
        found = Helper::moveToXYMap(pos, maxRouteDistance, mayLeaveNetwork, origID, angle,
                                    speed, veh->getRoute().getEdges(), veh->getRoutePosition(), veh->getLane(), veh->getPositionOnLane(), veh->isOnRoad(),
                                    vClass,
                                    bestDistance, &lane, lanePos, routeOffset, edges);
    }
    if ((found && bestDistance <= maxRouteDistance) || mayLeaveNetwork) {
        // optionally compute lateral offset
        pos.setz(veh->getPosition().z());
        if (found && (MSGlobals::gLateralResolution > 0 || mayLeaveNetwork)) {
            const double perpDist = lane->getShape().distance2D(pos, false);
            if (perpDist != GeomHelper::INVALID_OFFSET) {
                lanePosLat = perpDist;
                if (!mayLeaveNetwork) {
                    lanePosLat = MIN2(lanePosLat, 0.5 * (lane->getWidth() + veh->getVehicleType().getWidth() - MSGlobals::gLateralResolution));
                }
                // figure out whether the offset is to the left or to the right
                PositionVector tmp = lane->getShape();
                try {
                    tmp.move2side(-lanePosLat); // moved to left
                } catch (ProcessError&) {
                    WRITE_WARNING("Could not determine position on lane '" + lane->getID() + "' at lateral position " + toString(-lanePosLat) + ".");
                }
                //std::cout << " lane=" << lane->getID() << " posLat=" << lanePosLat << " shape=" << lane->getShape() << " tmp=" << tmp << " tmpDist=" << tmp.distance2D(pos) << "\n";
                if (tmp.distance2D(pos) > perpDist) {
                    lanePosLat = -lanePosLat;
                }
            }
            pos.setz(lane->geometryPositionAtOffset(lanePos).z());
        }
        if (found && !mayLeaveNetwork && MSGlobals::gLateralResolution < 0) {
            // mapped position may differ from pos
            pos = lane->geometryPositionAtOffset(lanePos, -lanePosLat);
        }
        assert((found && lane != 0) || (!found && lane == 0));
        if (angle == INVALID_DOUBLE_VALUE) {
            if (lane != nullptr) {
                angle = GeomHelper::naviDegree(lane->getShape().rotationAtOffset(lanePos));
            } else {
                // compute angle outside road network from old and new position
                angle = GeomHelper::naviDegree(veh->getPosition().angleTo2D(pos));
            }
        }
        // use the best we have
#ifdef DEBUG_MOVEXY
        std::cout << SIMTIME << " veh=" << vehicleID + " moveToXYResult lane='" << Named::getIDSecure(lane) << "' lanePos=" << lanePos << " lanePosLat=" << lanePosLat << "\n";
#endif
        Helper::setRemoteControlled(veh, pos, lane, lanePos, lanePosLat, angle, routeOffset, edges, MSNet::getInstance()->getCurrentTimeStep());
        if (!veh->isOnRoad()) {
            MSNet::getInstance()->getInsertionControl().alreadyDeparted(veh);
        }
    } else {
        if (lane == nullptr) {
            throw TraCIException("Could not map vehicle '" + vehicleID + "', no road found within " + toString(maxRouteDistance) + "m.");
        } else {
            throw TraCIException("Could not map vehicle '" + vehicleID + "', distance to road is " + toString(bestDistance) + ".");
        }
    }
}

void
Vehicle::slowDown(const std::string& vehicleID, double speed, double duration) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    std::vector<std::pair<SUMOTime, double> > speedTimeLine;
    speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), veh->getSpeed()));
    speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(duration), speed));
    veh->getInfluencer().setSpeedTimeLine(speedTimeLine);
}

void
Vehicle::openGap(const std::string& vehicleID, double newTimeHeadway, double newSpaceHeadway, double duration, double changeRate, double maxDecel, const std::string& referenceVehID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    MSVehicle* refVeh = nullptr;
    if (referenceVehID != "") {
        refVeh = Helper::getVehicle(referenceVehID);
    }
    const double originalTau = veh->getVehicleType().getCarFollowModel().getHeadwayTime();
    if (newTimeHeadway == -1) {
        newTimeHeadway = originalTau;
    }
    if (originalTau > newTimeHeadway) {
        WRITE_WARNING("Ignoring openGap(). New time headway must not be smaller than the original.");
        return;
    }
    veh->getInfluencer().activateGapController(originalTau, newTimeHeadway, newSpaceHeadway, duration, changeRate, maxDecel, refVeh);
}

void
Vehicle::deactivateGapControl(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (veh->hasInfluencer()) {
        veh->getInfluencer().deactivateGapController();
    }
}

void
Vehicle::requestToC(const std::string& vehID, double leadTime) {
    setParameter(vehID, "device.toc.requestToC", toString(leadTime));
}

void
Vehicle::setSpeed(const std::string& vehicleID, double speed) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    std::vector<std::pair<SUMOTime, double> > speedTimeLine;
    if (speed >= 0) {
        speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), speed));
        speedTimeLine.push_back(std::make_pair(SUMOTime_MAX - DELTA_T, speed));
    }
    veh->getInfluencer().setSpeedTimeLine(speedTimeLine);
}

void
Vehicle::setSpeedMode(const std::string& vehicleID, int speedMode) {
    Helper::getVehicle(vehicleID)->getInfluencer().setSpeedMode(speedMode);
}

void
Vehicle::setLaneChangeMode(const std::string& vehicleID, int laneChangeMode) {
    Helper::getVehicle(vehicleID)->getInfluencer().setLaneChangeMode(laneChangeMode);
}

void
Vehicle::setRoutingMode(const std::string& vehicleID, int routingMode) {
    Helper::getVehicle(vehicleID)->getInfluencer().setRoutingMode(routingMode);
}

void
Vehicle::setType(const std::string& vehicleID, const std::string& typeID) {
    MSVehicleType* vehicleType = MSNet::getInstance()->getVehicleControl().getVType(typeID);
    if (vehicleType == nullptr) {
        throw TraCIException("Vehicle type '" + typeID + "' is not known");
    }
    Helper::getVehicle(vehicleID)->replaceVehicleType(vehicleType);
}

void
Vehicle::setRouteID(const std::string& vehicleID, const std::string& routeID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    const MSRoute* r = MSRoute::dictionary(routeID);
    if (r == nullptr) {
        throw TraCIException("The route '" + routeID + "' is not known.");
    }
    std::string msg;
    if (!veh->hasValidRoute(msg, r)) {
        WRITE_WARNING("Invalid route replacement for vehicle '" + veh->getID() + "'. " + msg);
        if (MSGlobals::gCheckRoutes) {
            throw TraCIException("Route replacement failed for " + veh->getID());
        }
    }

    if (!veh->replaceRoute(r, "traci:setRouteID", veh->getLane() == nullptr)) {
        throw TraCIException("Route replacement failed for " + veh->getID());
    }
}

void
Vehicle::setRoute(const std::string& vehicleID, const std::vector<std::string>& edgeIDs) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    ConstMSEdgeVector edges;
    try {
        MSEdge::parseEdgesList(edgeIDs, edges, "<unknown>");
        if (edges.size() > 0 && edges.back()->isInternal()) {
            edges.push_back(edges.back()->getLanes()[0]->getNextNormal());
        }
    } catch (ProcessError& e) {
        throw TraCIException("Invalid edge list for vehicle '" + veh->getID() + "' (" + e.what() + ")");
    }
    if (!veh->replaceRouteEdges(edges, -1, 0, "traci:setRoute", veh->getLane() == nullptr, true)) {
        throw TraCIException("Route replacement failed for " + veh->getID());
    }
}

void
Vehicle::updateBestLanes(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    veh->updateBestLanes(true);
}


void
Vehicle::setAdaptedTraveltime(const std::string& vehicleID, const std::string& edgeID,
                              double time, double begSeconds, double endSeconds) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    MSEdge* edge = MSEdge::dictionary(edgeID);
    if (edge == nullptr) {
        throw TraCIException("Edge '" + edgeID + "' is not known.");
    }
    if (time != INVALID_DOUBLE_VALUE) {
        // add time
        if (begSeconds == 0 && endSeconds == std::numeric_limits<double>::max()) {
            // clean up old values before setting whole range
            while (veh->getWeightsStorage().knowsTravelTime(edge)) {
                veh->getWeightsStorage().removeTravelTime(edge);
            }
        }
        veh->getWeightsStorage().addTravelTime(edge, begSeconds, endSeconds, time);
    } else {
        // remove time
        while (veh->getWeightsStorage().knowsTravelTime(edge)) {
            veh->getWeightsStorage().removeTravelTime(edge);
        }
    }
}


void
Vehicle::setEffort(const std::string& vehicleID, const std::string& edgeID,
                   double effort, double begSeconds, double endSeconds) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    MSEdge* edge = MSEdge::dictionary(edgeID);
    if (edge == nullptr) {
        throw TraCIException("Edge '" + edgeID + "' is not known.");
    }
    if (effort != INVALID_DOUBLE_VALUE) {
        // add effort
        if (begSeconds == 0 && endSeconds == std::numeric_limits<double>::max()) {
            // clean up old values before setting whole range
            while (veh->getWeightsStorage().knowsEffort(edge)) {
                veh->getWeightsStorage().removeEffort(edge);
            }
        }
        veh->getWeightsStorage().addEffort(edge, begSeconds, endSeconds, effort);
    } else {
        // remove effort
        while (veh->getWeightsStorage().knowsEffort(edge)) {
            veh->getWeightsStorage().removeEffort(edge);
        }
    }
}


void
Vehicle::rerouteTraveltime(const std::string& vehicleID, const bool currentTravelTimes) {
    UNUSED_PARAMETER(currentTravelTimes); // !!! see #5943
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    veh->reroute(MSNet::getInstance()->getCurrentTimeStep(), "traci:rerouteTraveltime",
                 veh->getInfluencer().getRouterTT(), isOnInit(vehicleID));
}


void
Vehicle::rerouteEffort(const std::string& vehicleID) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    veh->reroute(MSNet::getInstance()->getCurrentTimeStep(), "traci:rerouteEffort",
                 MSNet::getInstance()->getRouterEffort(), isOnInit(vehicleID));
}


void
Vehicle::setSignals(const std::string& vehicleID, int signals) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    // set influencer to make the change persistent
    veh->getInfluencer().setSignals(signals);
    // set them now so that getSignals returns the correct value
    veh->switchOffSignal(0x0fffffff);
    if (signals >= 0) {
        veh->switchOnSignal(signals);
    }
}


void
Vehicle::moveTo(const std::string& vehicleID, const std::string& laneID, double position) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    MSLane* l = MSLane::dictionary(laneID);
    if (l == nullptr) {
        throw TraCIException("Unknown lane '" + laneID + "'.");
    }
    MSEdge* destinationEdge = &l->getEdge();
    const MSEdge* destinationRouteEdge = destinationEdge->getNormalBefore();
    // find edge in the remaining route
    MSRouteIterator it = std::find(veh->getCurrentRouteEdge(), veh->getRoute().end(), destinationRouteEdge);
    if (it == veh->getRoute().end()) {
        // find edge in the edges that were already passed
        it = std::find(veh->getRoute().begin(), veh->getRoute().end(), destinationRouteEdge);
    }
    if (it == veh->getRoute().end() ||
            // internal edge must continue the route
            (destinationEdge->isInternal() &&
             ((it + 1) == veh->getRoute().end()
              || l->getNextNormal() != *(it + 1)))) {
        throw TraCIException("Lane '" + laneID + "' is not on the route of vehicle '" + vehicleID + "'.");
    }
    veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_TELEPORT);
    if (veh->getLane() != nullptr) {
        veh->getLane()->removeVehicle(veh, MSMoveReminder::NOTIFICATION_TELEPORT);
    } else {
        veh->setTentativeLaneAndPosition(l, position);
    }
    const int newRouteIndex = (int)(it - veh->getRoute().begin());
    veh->resetRoutePosition(newRouteIndex, veh->getParameter().departLaneProcedure);
    if (!veh->isOnRoad()) {
        MSNet::getInstance()->getInsertionControl().alreadyDeparted(veh);

    }
    l->forceVehicleInsertion(veh, position,
                             veh->hasDeparted() ? MSMoveReminder::NOTIFICATION_TELEPORT : MSMoveReminder::NOTIFICATION_DEPARTED);
}


void
Vehicle::setActionStepLength(const std::string& vehicleID, double actionStepLength, bool resetActionOffset) {
    if (actionStepLength < 0.0) {
        WRITE_ERROR("Invalid action step length (<0). Ignoring command setActionStepLength().");
        return;
    }
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (actionStepLength == 0.) {
        veh->resetActionOffset();
    } else {
        veh->setActionStepLength(actionStepLength, resetActionOffset);
    }
}


void
Vehicle::remove(const std::string& vehicleID, char reason) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    MSMoveReminder::Notification n = MSMoveReminder::NOTIFICATION_ARRIVED;
    switch (reason) {
        case REMOVE_TELEPORT:
            // XXX semantics unclear
            // n = MSMoveReminder::NOTIFICATION_TELEPORT;
            n = MSMoveReminder::NOTIFICATION_TELEPORT_ARRIVED;
            break;
        case REMOVE_PARKING:
            // XXX semantics unclear
            // n = MSMoveReminder::NOTIFICATION_PARKING;
            n = MSMoveReminder::NOTIFICATION_ARRIVED;
            break;
        case REMOVE_ARRIVED:
            n = MSMoveReminder::NOTIFICATION_ARRIVED;
            break;
        case REMOVE_VAPORIZED:
            n = MSMoveReminder::NOTIFICATION_VAPORIZED;
            break;
        case REMOVE_TELEPORT_ARRIVED:
            n = MSMoveReminder::NOTIFICATION_TELEPORT_ARRIVED;
            break;
        default:
            throw TraCIException("Unknown removal status.");
    }
    if (veh->hasDeparted()) {
        veh->onRemovalFromNet(n);
        if (veh->getLane() != nullptr) {
            veh->getLane()->removeVehicle(veh, n);
        }
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
    } else {
        MSNet::getInstance()->getInsertionControl().alreadyDeparted(veh);
        MSNet::getInstance()->getVehicleControl().deleteVehicle(veh, true);
    }
}


void
Vehicle::setColor(const std::string& vehicleID, const TraCIColor& col) {
    const SUMOVehicleParameter& p = Helper::getVehicle(vehicleID)->getParameter();
    p.color.set((unsigned char)col.r, (unsigned char)col.g, (unsigned char)col.b, (unsigned char)col.a);
    p.parametersSet |= VEHPARS_COLOR_SET;
}


void
Vehicle::setSpeedFactor(const std::string& vehicleID, double factor) {
    Helper::getVehicle(vehicleID)->setChosenSpeedFactor(factor);
}


void
Vehicle::setLine(const std::string& vehicleID, const std::string& line) {
    Helper::getVehicle(vehicleID)->getParameter().line = line;
}


void
Vehicle::setVia(const std::string& vehicleID, const std::vector<std::string>& via) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    try {
        // ensure edges exist
        ConstMSEdgeVector edges;
        MSEdge::parseEdgesList(via, edges, "<via-edges>");
    } catch (ProcessError& e) {
        throw TraCIException(e.what());
    }
    veh->getParameter().via = via;
}


void
Vehicle::setLength(const std::string& vehicleID, double length) {
    Helper::getVehicle(vehicleID)->getSingularType().setLength(length);
}


void
Vehicle::setMaxSpeed(const std::string& vehicleID, double speed) {
    Helper::getVehicle(vehicleID)->getSingularType().setMaxSpeed(speed);
}


void
Vehicle::setVehicleClass(const std::string& vehicleID, const std::string& clazz) {
    Helper::getVehicle(vehicleID)->getSingularType().setVClass(getVehicleClassID(clazz));
}


void
Vehicle::setShapeClass(const std::string& vehicleID, const std::string& clazz) {
    Helper::getVehicle(vehicleID)->getSingularType().setShape(getVehicleShapeID(clazz));
}


void
Vehicle::setEmissionClass(const std::string& vehicleID, const std::string& clazz) {
    Helper::getVehicle(vehicleID)->getSingularType().setEmissionClass(PollutantsInterface::getClassByName(clazz));
}


void
Vehicle::setWidth(const std::string& vehicleID, double width) {
    Helper::getVehicle(vehicleID)->getSingularType().setWidth(width);
}


void
Vehicle::setHeight(const std::string& vehicleID, double height) {
    Helper::getVehicle(vehicleID)->getSingularType().setHeight(height);
}


void
Vehicle::setMinGap(const std::string& vehicleID, double minGap) {
    Helper::getVehicle(vehicleID)->getSingularType().setMinGap(minGap);
}


void
Vehicle::setAccel(const std::string& vehicleID, double accel) {
    Helper::getVehicle(vehicleID)->getSingularType().setAccel(accel);
}


void
Vehicle::setDecel(const std::string& vehicleID, double decel) {
    VehicleType::setDecel(Helper::getVehicle(vehicleID)->getSingularType().getID(), decel);
}


void
Vehicle::setEmergencyDecel(const std::string& vehicleID, double decel) {
    VehicleType::setEmergencyDecel(Helper::getVehicle(vehicleID)->getSingularType().getID(), decel);
}


void
Vehicle::setApparentDecel(const std::string& vehicleID, double decel) {
    Helper::getVehicle(vehicleID)->getSingularType().setApparentDecel(decel);
}


void
Vehicle::setImperfection(const std::string& vehicleID, double imperfection) {
    Helper::getVehicle(vehicleID)->getSingularType().setImperfection(imperfection);
}


void
Vehicle::setTau(const std::string& vehicleID, double tau) {
    Helper::getVehicle(vehicleID)->getSingularType().setTau(tau);
}


void
Vehicle::setMinGapLat(const std::string& vehicleID, double minGapLat) {
    Helper::getVehicle(vehicleID)->getSingularType().setMinGapLat(minGapLat);
}


void
Vehicle::setMaxSpeedLat(const std::string& vehicleID, double speed) {
    Helper::getVehicle(vehicleID)->getSingularType().setMaxSpeedLat(speed);
}


void
Vehicle::setLateralAlignment(const std::string& vehicleID, const std::string& latAlignment) {
    Helper::getVehicle(vehicleID)->getSingularType().setPreferredLateralAlignment(SUMOXMLDefinitions::LateralAlignments.get(latAlignment));
}


void
Vehicle::setParameter(const std::string& vehicleID, const std::string& key, const std::string& value) {
    MSVehicle* veh = Helper::getVehicle(vehicleID);
    if (StringUtils::startsWith(key, "device.")) {
        StringTokenizer tok(key, ".");
        if (tok.size() < 3) {
            throw TraCIException("Invalid device parameter '" + key + "' for vehicle '" + vehicleID + "'");
        }
        try {
            veh->setDeviceParameter(tok.get(1), key.substr(tok.get(0).size() + tok.get(1).size() + 2), value);
        } catch (InvalidArgument& e) {
            throw TraCIException("Vehicle '" + vehicleID + "' does not support device parameter '" + key + "' (" + e.what() + ").");
        }
    } else if (StringUtils::startsWith(key, "laneChangeModel.")) {
        const std::string attrName = key.substr(16);
        try {
            veh->getLaneChangeModel().setParameter(attrName, value);
        } catch (InvalidArgument& e) {
            throw TraCIException("Vehicle '" + vehicleID + "' does not support laneChangeModel parameter '" + key + "' (" + e.what() + ").");
        }
    } else if (StringUtils::startsWith(key, "carFollowModel.")) {
        const std::string attrName = key.substr(15);
        try {
            veh->getCarFollowModel().setParameter(veh, attrName, value);
        } catch (InvalidArgument& e) {
            throw TraCIException("Vehicle '" + vehicleID + "' does not support carFollowModel parameter '" + key + "' (" + e.what() + ").");
        }
    } else if (StringUtils::startsWith(key, "has.") && StringUtils::endsWith(key, ".device")) {
        StringTokenizer tok(key, ".");
        if (tok.size() != 3) {
            throw TraCIException("Invalid request for device status change. Expected format is 'has.DEVICENAME.device'");
        }
        const std::string deviceName = tok.get(1);
        bool create;
        try {
            create = StringUtils::toBool(value);
        } catch (BoolFormatException&) {
            throw TraCIException("Changing device status requires a 'true' or 'false'");
        }
        if (!create) {
            throw TraCIException("Device removal is not supported for device of type '" + deviceName + "'");
        }
        try {
            veh->createDevice(deviceName);
        } catch (InvalidArgument& e) {
            throw TraCIException("Cannot create vehicle device (" + std::string(e.what()) + ").");
        }
    } else {
        ((SUMOVehicleParameter&)veh->getParameter()).setParameter(key, value);
    }
}


void
Vehicle::highlight(const std::string& vehicleID, const TraCIColor& col, double size, const int alphaMax, const double duration, const int type) {
    // NOTE: Code is duplicated in large parts in POI.cpp
    MSVehicle* veh = Helper::getVehicle(vehicleID);

    // Center of the highlight circle
    Position center = veh->getPosition();
    const double l2 = veh->getLength() * 0.5;
    center.sub(cos(veh->getAngle())*l2, sin(veh->getAngle())*l2);
    // Size of the highlight circle
    if (size <= 0) {
        size = veh->getLength() * 0.7;
    }
    // Make polygon shape
    const unsigned int nPoints = 34;
    const PositionVector circlePV = GeomHelper::makeRing(size, size + 1., center, nPoints);
    TraCIPositionVector circle = Helper::makeTraCIPositionVector(circlePV);

#ifdef DEBUG_DYNAMIC_SHAPES
    std::cout << SIMTIME << " Vehicle::highlight() for vehicle '" << vehicleID << "'\n"
              << " circle: " << circlePV << std::endl;
#endif

    // Find a free polygon id
    int i = 0;
    std::string polyID = veh->getID() + "_hl" + toString(i);
    while (Polygon::exists(polyID)) {
        polyID = veh->getID() + "_hl" + toString(++i);
    }
    // Line width
    double lw = 0.;
    // Layer
    double lyr = 0.;
    if (MSNet::getInstance()->isGUINet()) {
        lyr = GLO_VEHICLE + 0.01;
        lyr += (type + 1) / 257.;
    }
    // Make Polygon
    Polygon::addHighlightPolygon(vehicleID, type, polyID, circle, col, true, "highlight", (int)lyr, lw);

    // Animation time line
    double maxAttack = 1.0; // maximal fade-in time
    std::vector<double> timeSpan;
    if (duration > 0.) {
        timeSpan = {0, MIN2(maxAttack, duration / 3.), 2.*duration / 3., duration};
    }
    // Alpha time line
    std::vector<double> alphaSpan;
    if (alphaMax > 0.) {
        alphaSpan = {0., (double) alphaMax, (double)(alphaMax) / 3., 0.};
    }
    // Attach dynamics
    Polygon::addDynamics(polyID, vehicleID, timeSpan, alphaSpan, false, true);
}


LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(Vehicle, VEHICLE)


void
Vehicle::subscribeLeader(const std::string& vehicleID, double /* dist */, double beginTime, double endTime) {
    // TODO handle dist correctly
    Vehicle::subscribe(vehicleID, std::vector<int>({libsumo::VAR_LEADER}), beginTime, endTime);
}


void
Vehicle::storeShape(const std::string& id, PositionVector& shape) {
    shape.push_back(Helper::getVehicle(id)->getPosition());
}


std::shared_ptr<VariableWrapper>
Vehicle::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
Vehicle::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper) {
    switch (variable) {
        case TRACI_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getIDList());
        case ID_COUNT:
            return wrapper->wrapInt(objID, variable, getIDCount());
        case VAR_POSITION:
            return wrapper->wrapPosition(objID, variable, getPosition(objID));
        case VAR_POSITION3D:
            return wrapper->wrapPosition(objID, variable, getPosition(objID, true));
        case VAR_ANGLE:
            return wrapper->wrapDouble(objID, variable, getAngle(objID));
        case VAR_SPEED:
            return wrapper->wrapDouble(objID, variable, getSpeed(objID));
        case VAR_SPEED_LAT:
            return wrapper->wrapDouble(objID, variable, getLateralSpeed(objID));
        case VAR_ROAD_ID:
            return wrapper->wrapString(objID, variable, getRoadID(objID));
        case VAR_SPEED_WITHOUT_TRACI:
            return wrapper->wrapDouble(objID, variable, getSpeedWithoutTraCI(objID));
        case VAR_SLOPE:
            return wrapper->wrapDouble(objID, variable, getSlope(objID));
        case VAR_LANE_ID:
            return wrapper->wrapString(objID, variable, getLaneID(objID));
        case VAR_LANE_INDEX:
            return wrapper->wrapInt(objID, variable, getLaneIndex(objID));
        case VAR_TYPE:
            return wrapper->wrapString(objID, variable, getTypeID(objID));
        case VAR_ROUTE_ID:
            return wrapper->wrapString(objID, variable, getRouteID(objID));
        case VAR_ROUTE_INDEX:
            return wrapper->wrapInt(objID, variable, getRouteIndex(objID));
        case VAR_COLOR:
            return wrapper->wrapColor(objID, variable, getColor(objID));
        case VAR_LANEPOSITION:
            return wrapper->wrapDouble(objID, variable, getLanePosition(objID));
        case VAR_LANEPOSITION_LAT:
            return wrapper->wrapDouble(objID, variable, getLateralLanePosition(objID));
        case VAR_CO2EMISSION:
            return wrapper->wrapDouble(objID, variable, getCO2Emission(objID));
        case VAR_COEMISSION:
            return wrapper->wrapDouble(objID, variable, getCOEmission(objID));
        case VAR_HCEMISSION:
            return wrapper->wrapDouble(objID, variable, getHCEmission(objID));
        case VAR_PMXEMISSION:
            return wrapper->wrapDouble(objID, variable, getPMxEmission(objID));
        case VAR_NOXEMISSION:
            return wrapper->wrapDouble(objID, variable, getNOxEmission(objID));
        case VAR_FUELCONSUMPTION:
            return wrapper->wrapDouble(objID, variable, getFuelConsumption(objID));
        case VAR_NOISEEMISSION:
            return wrapper->wrapDouble(objID, variable, getNoiseEmission(objID));
        case VAR_ELECTRICITYCONSUMPTION:
            return wrapper->wrapDouble(objID, variable, getElectricityConsumption(objID));
        case VAR_PERSON_NUMBER:
            return wrapper->wrapInt(objID, variable, getPersonNumber(objID));
        case VAR_PERSON_CAPACITY:
            return wrapper->wrapInt(objID, variable, getPersonCapacity(objID));
        case LAST_STEP_PERSON_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getPersonIDList(objID));
        case VAR_WAITING_TIME:
            return wrapper->wrapDouble(objID, variable, getWaitingTime(objID));
        case VAR_ACCUMULATED_WAITING_TIME:
            return wrapper->wrapDouble(objID, variable, getAccumulatedWaitingTime(objID));
        case VAR_ROUTE_VALID:
            return wrapper->wrapInt(objID, variable, isRouteValid(objID));
        case VAR_EDGES:
            return wrapper->wrapStringList(objID, variable, getRoute(objID));
        case VAR_SIGNALS:
            return wrapper->wrapInt(objID, variable, getSignals(objID));
        case VAR_STOPSTATE:
            return wrapper->wrapInt(objID, variable, getStopState(objID));
        case VAR_DISTANCE:
            return wrapper->wrapDouble(objID, variable, getDistance(objID));
        case VAR_ALLOWED_SPEED:
            return wrapper->wrapDouble(objID, variable, getAllowedSpeed(objID));
        case VAR_SPEED_FACTOR:
            return wrapper->wrapDouble(objID, variable, getSpeedFactor(objID));
        case VAR_SPEEDSETMODE:
            return wrapper->wrapInt(objID, variable, getSpeedMode(objID));
        case VAR_LANECHANGE_MODE:
            return wrapper->wrapInt(objID, variable, getLaneChangeMode(objID));
        case VAR_ROUTING_MODE:
            return wrapper->wrapInt(objID, variable, getRoutingMode(objID));
        case VAR_LINE:
            return wrapper->wrapString(objID, variable, getLine(objID));
        case VAR_VIA:
            return wrapper->wrapStringList(objID, variable, getVia(objID));
        case VAR_ACCELERATION:
            return wrapper->wrapDouble(objID, variable, getAcceleration(objID));
        case VAR_LASTACTIONTIME:
            return wrapper->wrapDouble(objID, variable, getLastActionTime(objID));
        case VAR_LEADER: {
            const auto& lead = getLeader(objID);
            TraCIRoadPosition rp;
            rp.edgeID = lead.first;
            rp.pos = lead.second;
            return wrapper->wrapRoadPosition(objID, variable, rp);
        }
        default:
            return false;
    }
}


}

/****************************************************************************/
