/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/geom/GeomHelper.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/TplConvert.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/xml/SUMOVehicleParserHelper.h>
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
#include <libsumo/TraCIDefs.h>
#include <traci-server/TraCIConstants.h>
#include "Helper.h"
#include "Vehicle.h"

#define FAR_AWAY 1000.0

//#define DEBUG_MOVEXY
//#define DEBUG_MOVEXY_ANGLE


namespace libsumo {
    // ===========================================================================
    // member definitions
    // ===========================================================================
    std::map<std::string, std::vector<MSLane*> > Vehicle::gVTDMap;

    // ===========================================================================
    // member definitions
    // ===========================================================================
    MSVehicle*
        Vehicle::getVehicle(const std::string& id) {
        SUMOVehicle* sumoVehicle = MSNet::getInstance()->getVehicleControl().getVehicle(id);
        if (sumoVehicle == 0) {
            throw TraCIException("Vehicle '" + id + "' is not known");
        }
        MSVehicle* v = dynamic_cast<MSVehicle*>(sumoVehicle);
        if (v == 0) {
            throw TraCIException("Vehicle '" + id + "' is not a micro-simulation vehicle");
        }
        return v;
    }


    bool
        Vehicle::isVisible(const MSVehicle* veh) {
        return veh->isOnRoad() || veh->isParking() || veh->wasRemoteControlled();
    }


    bool
        Vehicle::isOnInit(const std::string& vehicleID) {
        SUMOVehicle* sumoVehicle = MSNet::getInstance()->getVehicleControl().getVehicle(vehicleID);
        return sumoVehicle == 0 || sumoVehicle->getLane() == 0;
    }

    std::vector<std::string>
        Vehicle::getIDList() {
        std::vector<std::string> ids;
        MSVehicleControl& c = MSNet::getInstance()->getVehicleControl();
        for (MSVehicleControl::constVehIt i = c.loadedVehBegin(); i != c.loadedVehEnd(); ++i) {
            if ((*i).second->isOnRoad() || (*i).second->isParking()) {
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
        MSVehicle* veh = getVehicle(vehicleID);
        return isVisible(veh) ? veh->getSpeed() : INVALID_DOUBLE_VALUE;
    }


    double
        Vehicle::getSpeedWithoutTraCI(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return isVisible(veh) ? veh->getSpeedWithoutTraciInfluence() : INVALID_DOUBLE_VALUE;
    }


    TraCIPosition
        Vehicle::getPosition(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        if (isVisible(veh)) {
            return Helper::makeTraCIPosition(veh->getPosition());
        } else {
            TraCIPosition result;
            result.x = INVALID_DOUBLE_VALUE;
            result.y = INVALID_DOUBLE_VALUE;
            result.z = INVALID_DOUBLE_VALUE;
            return result;
        }
    }


    double
        Vehicle::getAngle(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return isVisible(veh) ? GeomHelper::naviDegree(veh->getAngle()) : INVALID_DOUBLE_VALUE;
    }


    double
        Vehicle::getSlope(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return veh->isOnRoad() ? veh->getSlope() : INVALID_DOUBLE_VALUE;
    }


    std::string
        Vehicle::getRoadID(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return isVisible(veh) ? veh->getLane()->getEdge().getID() : "";
    }


    std::string
        Vehicle::getLaneID(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return veh->isOnRoad() ? veh->getLane()->getID() : "";
    }


    int
        Vehicle::getLaneIndex(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return veh->isOnRoad() ? veh->getLane()->getIndex() : INVALID_INT_VALUE;
    }

    std::string
        Vehicle::getTypeID(const std::string& vehicleID) {
        return getVehicle(vehicleID)->getVehicleType().getID();
    }

    std::string
        Vehicle::getRouteID(const std::string& vehicleID) {
        return getVehicle(vehicleID)->getRoute().getID();
    }

    int
        Vehicle::getRouteIndex(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return veh->hasDeparted() ? veh->getRoutePosition() : INVALID_INT_VALUE;
    }

    TraCIColor
        Vehicle::getColor(const std::string& vehicleID) {
        return Helper::makeTraCIColor(getVehicle(vehicleID)->getParameter().color);
    }

    double
        Vehicle::getLanePosition(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return veh->isOnRoad() ? veh->getPositionOnLane() : INVALID_DOUBLE_VALUE;
    }

    double
        Vehicle::getLateralLanePosition(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return veh->isOnRoad() ? veh->getLateralPositionOnLane() : INVALID_DOUBLE_VALUE;
    }

    double
        Vehicle::getCO2Emission(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return isVisible(veh) ? veh->getCO2Emissions() : INVALID_DOUBLE_VALUE;
    }

    double
        Vehicle::getCOEmission(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return isVisible(veh) ? veh->getCOEmissions() : INVALID_DOUBLE_VALUE;
    }

    double
        Vehicle::getHCEmission(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return isVisible(veh) ? veh->getHCEmissions() : INVALID_DOUBLE_VALUE;
    }

    double
        Vehicle::getPMxEmission(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return isVisible(veh) ? veh->getPMxEmissions() : INVALID_DOUBLE_VALUE;
    }

    double
        Vehicle::getNOxEmission(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return isVisible(veh) ? veh->getNOxEmissions() : INVALID_DOUBLE_VALUE;
    }

    double
        Vehicle::getFuelConsumption(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return isVisible(veh) ? veh->getFuelConsumption() : INVALID_DOUBLE_VALUE;
    }

    double
        Vehicle::getNoiseEmission(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return isVisible(veh) ? veh->getHarmonoise_NoiseEmissions() : INVALID_DOUBLE_VALUE;
    }

    double
        Vehicle::getElectricityConsumption(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        return isVisible(veh) ? veh->getElectricityConsumption() : INVALID_DOUBLE_VALUE;
    }

    int
        Vehicle::getPersonNumber(const std::string& vehicleID) {
        return getVehicle(vehicleID)->getPersonNumber();
    }


    std::pair<std::string, double>
        Vehicle::getLeader(const std::string& vehicleID, double dist) {
        MSVehicle* veh = getVehicle(vehicleID);
        if (veh->isOnRoad()) {
            std::pair<const MSVehicle* const, double> leaderInfo = veh->getLeader(dist);
            return std::make_pair(
                leaderInfo.first != 0 ? leaderInfo.first->getID() : "",
                leaderInfo.second);
        } else {
            return std::make_pair("", -1);
        }
    }


    double
        Vehicle::getWaitingTime(const std::string& vehicleID) {
        return getVehicle(vehicleID)->getWaitingSeconds();
    }


    double
        Vehicle::getAccumulatedWaitingTime(const std::string& vehicleID) {
        return getVehicle(vehicleID)->getAccumulatedWaitingSeconds();
    }


    double
        Vehicle::getAdaptedTraveltime(const std::string& vehicleID, const std::string& edgeID, int time) {
        MSVehicle* veh = getVehicle(vehicleID);
        MSEdge* edge = Helper::getEdge(edgeID);
        double value = INVALID_DOUBLE_VALUE;;
        veh->getWeightsStorage().retrieveExistingTravelTime(edge, time, value);
        return value;
    }


    double
        Vehicle::getEffort(const std::string& vehicleID, const std::string& edgeID, int time) {
        MSVehicle* veh = getVehicle(vehicleID);
        MSEdge* edge = Helper::getEdge(edgeID);
        double value = INVALID_DOUBLE_VALUE;;
        veh->getWeightsStorage().retrieveExistingEffort(edge, time, value);
        return value;
    }


    bool
        Vehicle::isRouteValid(const std::string& vehicleID) {
        std::string msg;
        return getVehicle(vehicleID)->hasValidRoute(msg);
    }

    std::vector<std::string>
        Vehicle::getEdges(const std::string& vehicleID) {
        std::vector<std::string> result;
        MSVehicle* veh = getVehicle(vehicleID);
        const MSRoute& r = veh->getRoute();
        for (MSRouteIterator i = r.begin(); i != r.end(); ++i) {
            result.push_back((*i)->getID());
        }
        return result;
    }


    int
        Vehicle::getSignalStates(const std::string& vehicleID) {
        return getVehicle(vehicleID)->getSignals();
    }

    std::vector<TraCIBestLanesData>
        Vehicle::getBestLanes(const std::string& vehicleID) {
        std::vector<TraCIBestLanesData> result;
        MSVehicle* veh = getVehicle(vehicleID);
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
                    if ((*j) != 0) {
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
        MSVehicle* veh = getVehicle(vehicleID);
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
        }
        return result;
    }

    int
        Vehicle::getStopState(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        int result = 0;
        if (veh->isStopped()) {
            const MSVehicle::Stop& stop = veh->getNextStop();
            result = (1 + (stop.pars.parking ? 2 : 0) +
                (stop.pars.triggered ? 4 : 0) +
                (stop.pars.containerTriggered ? 8 : 0) +
                (stop.busstop != 0 ? 16 : 0) +
                (stop.containerstop != 0 ? 32 : 0) +
                (stop.chargingStation != 0 ? 64 : 0) +
                (stop.parkingarea != 0 ? 128 : 0));
        }
        return result;
    }

    double
        Vehicle::getDistance(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
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
        MSVehicle* veh = getVehicle(vehicleID);
        if (veh->isOnRoad()) {
            double distance = veh->getRoute().getDistanceBetween(veh->getPositionOnLane(), position,
                &veh->getLane()->getEdge(), Helper::getEdge(edgeID));
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
        MSVehicle* veh = getVehicle(vehicleID);
        if (veh->isOnRoad()) {
            std::pair<MSLane*, double> roadPos = Helper::convertCartesianToRoadMap(Position(x, y));
            double distance = veh->getRoute().getDistanceBetween(veh->getPositionOnLane(), roadPos.second,
                veh->getEdge(), &roadPos.first->getEdge());
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
        MSVehicle* veh = getVehicle(vehicleID);
        if (veh->isOnRoad()) {
            return veh->getLane()->getVehicleMaxSpeed(veh);
        } else {
            return INVALID_DOUBLE_VALUE;
        }
    }

    double
        Vehicle::getSpeedFactor(const std::string& vehicleID) {
        return getVehicle(vehicleID)->getChosenSpeedFactor();
    }


    int
        Vehicle::getSpeedMode(const std::string& vehicleID) {
        return getVehicle(vehicleID)->getInfluencer().getSpeedMode();
    }

    int
        Vehicle::getLanechangeMode(const std::string& vehicleID) {
        return getVehicle(vehicleID)->getInfluencer().getLanechangeMode();
    }

    int
        Vehicle::getRoutingMode(const std::string& vehicleID) {
        return getVehicle(vehicleID)->getInfluencer().getRoutingMode();
    }

    std::string
        Vehicle::getLine(const std::string& vehicleID) {
        return getVehicle(vehicleID)->getParameter().line;
    }

    std::vector<std::string>
        Vehicle::getVia(const std::string& vehicleID) {
        return getVehicle(vehicleID)->getParameter().via;
    }


    std::pair<int, int>
        Vehicle::getLaneChangeState(const std::string& vehicleID, int direction) {
        MSVehicle* veh = getVehicle(vehicleID);
        if (veh->isOnRoad()) {
            return veh->getLaneChangeModel().getSavedState(direction);
        } else {
            return std::make_pair((int)LCA_UNKNOWN, (int)LCA_UNKNOWN);
        }
    }


    std::string
        Vehicle::getParameter(const std::string& vehicleID, const std::string& key) {
        MSVehicle* veh = getVehicle(vehicleID);
        if (StringUtils::startsWith(key, "device.")) {
            StringTokenizer tok(key, ".");
            if (tok.size() < 3) {
                throw TraCIException("Invalid device parameter '" + key + "' for vehicle '" + vehicleID + "'");
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
        } else if (StringUtils::startsWith(key, "has.") && StringUtils::endsWith(key, ".device")) {
            StringTokenizer tok(key, ".");
            if (tok.size() != 3) {
                throw TraCIException("Invalid check for device. Expected format is 'has.DEVICENAME.device'");
            }
            return veh->hasDevice(tok.get(1)) ? "true" : "false";
        } else {
            return veh->getParameter().getParameter(key, "");
        }
    }


    const MSVehicleType&
        Vehicle::getVehicleType(const std::string& vehicleID) {
        return getVehicle(vehicleID)->getVehicleType();
    }


    std::string
        Vehicle::getEmissionClass(const std::string& vehicleID) {
        return PollutantsInterface::getName(getVehicleType(vehicleID).getEmissionClass());
    }

    std::string
        Vehicle::getShapeClass(const std::string& vehicleID) {
        return getVehicleShapeName(getVehicleType(vehicleID).getGuiShape());
    }


    double
        Vehicle::getLength(const std::string& vehicleID) {
        return getVehicleType(vehicleID).getLength();
    }


    double
        Vehicle::getAccel(const std::string& vehicleID) {
        return getVehicleType(vehicleID).getLength();
    }


    double
        Vehicle::getDecel(const std::string& vehicleID) {
        return getVehicleType(vehicleID).getCarFollowModel().getMaxDecel();
    }


    double Vehicle::getEmergencyDecel(const std::string& vehicleID) {
        return getVehicleType(vehicleID).getCarFollowModel().getEmergencyDecel();
    }
    double Vehicle::getApparentDecel(const std::string& vehicleID) {
        return getVehicleType(vehicleID).getCarFollowModel().getApparentDecel();
    }
    double Vehicle::getActionStepLength(const std::string& vehicleID) {
        return getVehicleType(vehicleID).getActionStepLengthSecs();
    }
    double Vehicle::getLastActionTime(const std::string& vehicleID) {
        return STEPS2TIME(getVehicle(vehicleID)->getLastActionTime());
    }

    double
        Vehicle::getTau(const std::string& vehicleID) {
        return getVehicleType(vehicleID).getCarFollowModel().getHeadwayTime();
    }


    double
        Vehicle::getImperfection(const std::string& vehicleID) {
        return getVehicleType(vehicleID).getCarFollowModel().getImperfection();
    }


    double
        Vehicle::getSpeedDeviation(const std::string& vehicleID) {
        return getVehicleType(vehicleID).getSpeedFactor().getParameter()[1];
    }


    std::string
        Vehicle::getVClass(const std::string& vehicleID) {
        return toString(getVehicleType(vehicleID).getVehicleClass());
    }


    double
        Vehicle::getMinGap(const std::string& vehicleID) {
        return getVehicleType(vehicleID).getMinGap();
    }


    double
        Vehicle::getMaxSpeed(const std::string& vehicleID) {
        return getVehicleType(vehicleID).getMaxSpeed();
    }


    double
        Vehicle::getWidth(const std::string& vehicleID) {
        return getVehicleType(vehicleID).getWidth();
    }


    void
        Vehicle::setStop(const std::string& vehicleID,
        const std::string& edgeID,
        double endPos,
        int laneIndex,
        SUMOTime duration,
        int flags,
        double startPos,
        SUMOTime until) {
        MSVehicle* veh = getVehicle(vehicleID);
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

        std::string error;
        if (stoppingPlaceType != SUMO_TAG_NOTHING) {
            // Forward command to vehicle
            if (!veh->addTraciStopAtStoppingPlace(edgeID, duration, until, parking, triggered, containerTriggered, stoppingPlaceType, error)) {
                throw TraCIException(error);
            }
        } else {
            // check
            if (startPos < 0) {
                throw TraCIException("Position on lane must not be negative.");
            }
            if (endPos < startPos) {
                throw TraCIException("End position on lane must be after start position.");
            }
            // get the actual lane that is referenced by laneIndex
            MSEdge* road = MSEdge::dictionary(edgeID);
            if (road == 0) {
                throw TraCIException("Unable to retrieve road with given id.");
            }
            const std::vector<MSLane*>& allLanes = road->getLanes();
            if ((laneIndex < 0) || laneIndex >= (int)(allLanes.size())) {
                throw TraCIException("No lane with index '" + toString(laneIndex) + "' on road '" + edgeID + "'.");
            }
            // Forward command to vehicle
            if (!veh->addTraciStop(allLanes[laneIndex], startPos, endPos, duration, until, parking, triggered, containerTriggered, error)) {
                throw TraCIException(error);
            }
        }
    }


    void
        Vehicle::resume(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
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
            throw TraCIException("Failed to resume from stoppingfor vehicle '" + veh->getID() + "', " + posStr);
        }
    }


    void
        Vehicle::changeTarget(const std::string& vehicleID, const std::string& edgeID) {
        MSVehicle* veh = getVehicle(vehicleID);
        const MSEdge* destEdge = MSEdge::dictionary(edgeID);
        const bool onInit = isOnInit(vehicleID);
        if (destEdge == 0) {
            throw TraCIException("Can not retrieve road with ID " + edgeID);
        }
        // build a new route between the vehicle's current edge and destination edge
        ConstMSEdgeVector newRoute;
        const MSEdge* currentEdge = veh->getRerouteOrigin();
        MSNet::getInstance()->getRouterTT().compute(
            currentEdge, destEdge, (const MSVehicle * const)veh, MSNet::getInstance()->getCurrentTimeStep(), newRoute);
        // replace the vehicle's route by the new one
        if (!veh->replaceRouteEdges(newRoute, onInit)) {
            throw TraCIException("Route replacement failed for " + veh->getID());
        }
        // route again to ensure usage of via/stops
        try {
            veh->reroute(MSNet::getInstance()->getCurrentTimeStep(), MSNet::getInstance()->getRouterTT(), onInit);
        } catch (ProcessError& e) {
            throw TraCIException(e.what());
        }
    }


    void
        Vehicle::changeLane(const std::string& vehicleID, int laneIndex, SUMOTime duration) {
        std::vector<std::pair<SUMOTime, int> > laneTimeLine;
        laneTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), laneIndex));
        laneTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep() + duration, laneIndex));
        getVehicle(vehicleID)->getInfluencer().setLaneTimeLine(laneTimeLine);
    }


    void
        Vehicle::changeSublane(const std::string& vehicleID, double latDist) {
        getVehicle(vehicleID)->getInfluencer().setSublaneChange(latDist);
    }


    void
        Vehicle::add(const std::string& vehicleID,
        const std::string& routeID,
        const std::string& typeID,
        std::string depart,
        const std::string& departLane,
        const std::string& departPos,
        const std::string& departSpeed,
        const std::string& arrivalLane,
        const std::string& arrivalPos,
        const std::string& arrivalSpeed,
        const std::string& fromTaz,
        const std::string& toTaz,
        const std::string& line,
        int personCapacity,
        int personNumber) {
        MSVehicle* veh = getVehicle(vehicleID);
        if (veh != 0) {
            throw TraCIException("The vehicle " + vehicleID + " to add already exists.");
        }
        UNUSED_PARAMETER(routeID);
        UNUSED_PARAMETER(typeID);
        UNUSED_PARAMETER(depart);
        UNUSED_PARAMETER(departLane);
        UNUSED_PARAMETER(departPos);
        UNUSED_PARAMETER(departSpeed);
        UNUSED_PARAMETER(arrivalLane);
        UNUSED_PARAMETER(arrivalPos);
        UNUSED_PARAMETER(arrivalSpeed);
        UNUSED_PARAMETER(fromTaz);
        UNUSED_PARAMETER(toTaz);
        UNUSED_PARAMETER(line);
        UNUSED_PARAMETER(personCapacity);
        UNUSED_PARAMETER(personNumber);
    }


    void
        Vehicle::moveToXY(const std::string& vehicleID, const std::string& edgeID, const int laneIndex, const double x, const double y, double angle, const int keepRouteFlag) {
        MSVehicle* veh = getVehicle(vehicleID);
        bool keepRoute = (keepRouteFlag == 1) && veh->getID() != "VTD_EGO";
        bool mayLeaveNetwork = (keepRouteFlag == 2);
        // process
        const std::string origID = edgeID + "_" + toString(laneIndex);
        // @todo add an interpretation layer for OSM derived origID values (without lane index)
        Position pos(x, y);
        double origAnge = angle;
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
        std::cout << std::endl << "begin vehicle " << veh->getID() << " vehPos:" << vehPos << " lane:" << Named::getIDSecure(veh->getLane()) << std::endl;
        std::cout << " want pos:" << pos << " origID:" << origID << " laneIndex:" << laneIndex << " origAngle:" << origAngle << " angle:" << angle << " keepRoute:" << keepRoute << std::endl;
#endif

        ConstMSEdgeVector edges;
        MSLane* lane = 0;
        double lanePos;
        double lanePosLat = 0;
        double bestDistance = std::numeric_limits<double>::max();
        int routeOffset = 0;
        bool found;
        double maxRouteDistance = 100;
        /* EGO vehicle is known to have a fixed route. @todo make this into a parameter of the TraCI call */
        if (keepRoute) {
            // case a): vehicle is on its earlier route
            //  we additionally assume it is moving forward (SUMO-limit);
            //  note that the route ("edges") is not changed in this case
            found = vtdMap_matchingRoutePosition(pos, origID, *veh, bestDistance, &lane, lanePos, routeOffset, edges);
            // @note silenty ignoring mapping failure
        } else {
            found = vtdMap(pos, maxRouteDistance, mayLeaveNetwork, origID, angle, *veh, bestDistance, &lane, lanePos, routeOffset, edges);
        }
        if ((found && bestDistance <= maxRouteDistance) || mayLeaveNetwork) {
            // optionally compute lateral offset
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
                        WRITE_WARNING("Could not determine position on lane '" + lane->getID() + " at lateral position " + toString(-lanePosLat) + ".");
                    }
                    //std::cout << " lane=" << lane->getID() << " posLat=" << lanePosLat << " shape=" << lane->getShape() << " tmp=" << tmp << " tmpDist=" << tmp.distance2D(pos) << "\n";
                    if (tmp.distance2D(pos) > perpDist) {
                        lanePosLat = -lanePosLat;
                    }
                }
            }
            if (found && !mayLeaveNetwork && MSGlobals::gLateralResolution < 0) {
                // mapped position may differ from pos
                pos = lane->geometryPositionAtOffset(lanePos, -lanePosLat);
            }
            assert((found && lane != 0) || (!found && lane == 0));
            if (angle == INVALID_DOUBLE_VALUE) {
                if (lane != 0) {
                    angle = GeomHelper::naviDegree(lane->getShape().rotationAtOffset(lanePos));
                } else {
                    // compute angle outside road network from old and new position
                    angle = GeomHelper::naviDegree(veh->getPosition().angleTo2D(pos));
                }
            }
            // use the best we have
            Helper::setVTDControlled(veh, pos, lane, lanePos, lanePosLat, angle, routeOffset, edges, MSNet::getInstance()->getCurrentTimeStep());
            if (!veh->isOnRoad()) {
                MSNet::getInstance()->getInsertionControl().alreadyDeparted(veh);

            }
        } else {
            if (lane == 0) {
                throw TraCIException("Could not map vehicle '" + vehicleID + "' no road found within " + toString(maxRouteDistance) + "m.");
            } else {
                throw TraCIException("Could not map vehicle '" + vehicleID + "' distance to road is " + toString(bestDistance) + ".");
            }
        }
    }

    void
        Vehicle::slowDown(const std::string& vehicleID, double speed, SUMOTime duration) {
        MSVehicle* veh = getVehicle(vehicleID);
        std::vector<std::pair<SUMOTime, double> > speedTimeLine;
        speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), veh->getSpeed()));
        speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep() + duration, speed));
        veh->getInfluencer().setSpeedTimeLine(speedTimeLine);
    }

    void
        Vehicle::setSpeed(const std::string& vehicleID, double speed) {
        getVehicle(vehicleID);
        UNUSED_PARAMETER(speed);
    }

    void
        Vehicle::setType(const std::string& vehicleID, const std::string& typeID) {
        MSVehicleType* vehicleType = MSNet::getInstance()->getVehicleControl().getVType(typeID);
        if (vehicleType == 0) {
            throw TraCIException("Vehicle type '" + vehicleID + "' is not known");
        }
        getVehicle(vehicleID)->replaceVehicleType(vehicleType);
    }

    void
        Vehicle::setRouteID(const std::string& vehicleID, const std::string& routeID) {
        MSVehicle* veh = getVehicle(vehicleID);
        const MSRoute* r = MSRoute::dictionary(routeID);
        if (r == 0) {
            throw TraCIException("The route '" + routeID + "' is not known.");
        }
        std::string msg;
        if (!veh->hasValidRoute(msg, r)) {
            WRITE_WARNING("Invalid route replacement for vehicle '" + veh->getID() + "'. " + msg);
            if (MSGlobals::gCheckRoutes) {
                throw TraCIException("Route replacement failed for " + veh->getID());
            }
        }

        if (!veh->replaceRoute(r, veh->getLane() == 0)) {
            throw TraCIException("Route replacement failed for " + veh->getID());
        }
    }


    void
        Vehicle::setRoute(const std::string& vehicleID, const std::vector<std::string>& edgeIDs) {
        MSVehicle* veh = getVehicle(vehicleID);
        ConstMSEdgeVector edges;
        try {
            MSEdge::parseEdgesList(edgeIDs, edges, "<unknown>");
        } catch (ProcessError& e) {
            throw TraCIException("Invalid edge list for vehicle '" + veh->getID() + "' (" + e.what() + ")");
        }
        if (!veh->replaceRouteEdges(edges, veh->getLane() == 0, true)) {
            throw TraCIException("Route replacement failed for " + veh->getID());
        }
    }

    void
        Vehicle::setAdaptedTraveltime(const std::string& vehicleID, const std::string& edgeID,
        double time, SUMOTime begTime, SUMOTime endTime) {
        MSVehicle* veh = getVehicle(vehicleID);
        MSEdge* edge = MSEdge::dictionary(edgeID);
        if (edge == 0) {
            throw TraCIException("Referended edge '" + edgeID + "' is not known.");
        }
        if (time != INVALID_DOUBLE_VALUE) {
            // add time
            if (begTime == 0 && endTime == SUMOTime_MAX) {
                // clean up old values before setting whole range
                while (veh->getWeightsStorage().knowsTravelTime(edge)) {
                    veh->getWeightsStorage().removeTravelTime(edge);
                }
            }
            veh->getWeightsStorage().addTravelTime(edge, STEPS2TIME(begTime), STEPS2TIME(endTime), time);
        } else {
            // remove time
            while (veh->getWeightsStorage().knowsTravelTime(edge)) {
                veh->getWeightsStorage().removeTravelTime(edge);
            }
        }
    }


    void
        Vehicle::setEffort(const std::string& vehicleID, const std::string& edgeID,
        double effort, SUMOTime begTime, SUMOTime endTime) {
        MSVehicle* veh = getVehicle(vehicleID);
        MSEdge* edge = MSEdge::dictionary(edgeID);
        if (edge == 0) {
            throw TraCIException("Referended edge '" + edgeID + "' is not known.");
        }
        if (effort != INVALID_DOUBLE_VALUE) {
            // add effort
            if (begTime == 0 && endTime == SUMOTime_MAX) {
                // clean up old values before setting whole range
                while (veh->getWeightsStorage().knowsEffort(edge)) {
                    veh->getWeightsStorage().removeEffort(edge);
                }
            }
            veh->getWeightsStorage().addEffort(edge, STEPS2TIME(begTime), STEPS2TIME(endTime), effort);
        } else {
            // remove effort
            while (veh->getWeightsStorage().knowsEffort(edge)) {
                veh->getWeightsStorage().removeEffort(edge);
            }
        }
    }


    void
        Vehicle::rerouteTraveltime(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        veh->reroute(MSNet::getInstance()->getCurrentTimeStep(), MSNet::getInstance()->getRouterTT(), isOnInit(vehicleID));
    }


    void
        Vehicle::rerouteEffort(const std::string& vehicleID) {
        MSVehicle* veh = getVehicle(vehicleID);
        veh->reroute(MSNet::getInstance()->getCurrentTimeStep(), MSNet::getInstance()->getRouterEffort(), isOnInit(vehicleID));
    }


    void
        Vehicle::setSignals(const std::string& vehicleID, int signals) {
        MSVehicle* veh = getVehicle(vehicleID);
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
        MSVehicle* veh = getVehicle(vehicleID);
        MSLane* l = MSLane::dictionary(laneID);
        if (l == 0) {
            throw TraCIException("Unknown lane '" + laneID + "'.");
        }
        MSEdge& destinationEdge = l->getEdge();
        if (!veh->willPass(&destinationEdge)) {
            throw TraCIException("Vehicle '" + laneID + "' may be set onto an edge to pass only.");
        }
        veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_TELEPORT);
        if (veh->getLane() != 0) {
            veh->getLane()->removeVehicle(veh, MSMoveReminder::NOTIFICATION_TELEPORT);
        } else {
            veh->setTentativeLaneAndPosition(l, position);
        }
        while (veh->getEdge() != &destinationEdge) {
            const MSEdge* nextEdge = veh->succEdge(1);
            // let the vehicle move to the next edge
            if (veh->enterLaneAtMove(nextEdge->getLanes()[0], true)) {
                MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
                continue;
            }
        }
        if (!veh->isOnRoad()) {
            MSNet::getInstance()->getInsertionControl().alreadyDeparted(veh);

        }
        l->forceVehicleInsertion(veh, position,
            veh->hasDeparted() ? MSMoveReminder::NOTIFICATION_TELEPORT : MSMoveReminder::NOTIFICATION_DEPARTED);
    }


    void
        Vehicle::setMaxSpeed(const std::string& vehicleID, double speed) {
        getVehicle(vehicleID)->getSingularType().setMaxSpeed(speed);
    }

    void
        Vehicle::setActionStepLength(const std::string& vehicleID, double actionStepLength, bool resetActionOffset) {
        if (actionStepLength < 0.0) {
            WRITE_ERROR("Invalid action step length (<0). Ignoring command setActionStepLength().");
            return;
        }
        MSVehicle* veh = getVehicle(vehicleID);
        if (actionStepLength == 0.) {
            veh->resetActionOffset();
            return;
        }
        SUMOTime actionStepLengthMillisecs = SUMOVehicleParserHelper::processActionStepLength(actionStepLength);
        SUMOTime previousActionStepLength = veh->getActionStepLength();
        veh->getSingularType().setActionStepLength(actionStepLengthMillisecs, resetActionOffset);
        if (resetActionOffset) {
            veh->resetActionOffset();
        } else {
            veh->updateActionOffset(previousActionStepLength, actionStepLengthMillisecs);
        }
    }

    void
        Vehicle::remove(const std::string& vehicleID, char reason) {
        getVehicle(vehicleID);
        UNUSED_PARAMETER(reason);
    }


    void
        Vehicle::setColor(const std::string& vehicleID, const TraCIColor& col) {
        const SUMOVehicleParameter& p = getVehicle(vehicleID)->getParameter();
        p.color.set(col.r, col.g, col.b, col.a);
        p.parametersSet |= VEHPARS_COLOR_SET;
    }


    void
        Vehicle::setLine(const std::string& vehicleID, const std::string& line) {
        getVehicle(vehicleID)->getParameter().line = line;
    }

    void
        Vehicle::setVia(const std::string& vehicleID, const std::vector<std::string>& via) {
        getVehicle(vehicleID);
        UNUSED_PARAMETER(via);
    }

    void
        Vehicle::setShapeClass(const std::string& vehicleID, const std::string& clazz) {
        getVehicle(vehicleID)->getSingularType().setShape(getVehicleShapeID(clazz));
    }

    void
        Vehicle::setEmissionClass(const std::string& vehicleID, const std::string& clazz) {
        getVehicle(vehicleID);
        UNUSED_PARAMETER(clazz);
    }


    void
        Vehicle::setParameter(const std::string& vehicleID, const std::string& key, const std::string& value) {
        MSVehicle* veh = getVehicle(vehicleID);
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
        } else if (StringUtils::startsWith(key, "has.") && StringUtils::endsWith(key, ".device")) {
            StringTokenizer tok(key, ".");
            if (tok.size() != 3) {
                throw TraCIException("Invalid request for device status change. Expected format is 'has.DEVICENAME.device'");
            }
            const std::string deviceName = tok.get(1);
            bool create;
            try {
                create = TplConvert::_2bool(value.c_str());
            } catch (BoolFormatException) {
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


    bool
        Vehicle::vtdMap(const Position& pos, double maxRouteDistance, bool mayLeaveNetwork, const std::string& origID, const double angle,  MSVehicle& veh,
                double& bestDistance, MSLane** lane, double& lanePos, int& routeOffset, ConstMSEdgeVector& edges) {
        // collect edges around the vehicle
        double speed = pos.distanceTo2D(veh.getPosition()); // !!!veh.getSpeed();
        std::set<std::string> into;
        PositionVector shape;
        shape.push_back(pos);
        Helper::collectObjectsInRange(CMD_GET_EDGE_VARIABLE, shape, maxRouteDistance, into);
        double maxDist = 0;
        std::map<MSLane*, LaneUtility> lane2utility;
        // compute utility for all candidate edges
        for (std::set<std::string>::const_iterator j = into.begin(); j != into.end(); ++j) {
            const MSEdge* const e = MSEdge::dictionary(*j);
            const MSEdge* prevEdge = 0;
            const MSEdge* nextEdge = 0;
            bool onRoute = false;
            // the next if/the clause sets "onRoute", "prevEdge", and "nextEdge", depending on
            //  whether the currently seen edge is an internal one or a normal one
            if (!e->isInternal()) {
#ifdef DEBUG_MOVEXY_ANGLE
                std::cout << "Ego on normal" << std::endl;
#endif
                // a normal edge
                //
                // check whether the currently seen edge is in the vehicle's route
                //  - either the one it's on or one of the next edges
                const ConstMSEdgeVector& ev = veh.getRoute().getEdges();
                int routePosition = veh.getRoutePosition();
                if (veh.isOnRoad() && veh.getLane()->getEdge().isInternal()) {
                    ++routePosition;
                }
                ConstMSEdgeVector::const_iterator edgePos = std::find(ev.begin() + routePosition, ev.end(), e);
                onRoute = edgePos != ev.end(); // no? -> onRoute is false
                if (edgePos == ev.end() - 1 && veh.getEdge() == e) {
                    // onRoute is false as well if the vehicle is beyond the edge
                    onRoute &= veh.getEdge()->getLanes()[0]->getLength() > veh.getPositionOnLane() + SPEED2DIST(speed);
                }
                // save prior and next edges
                prevEdge = e;
                nextEdge = !onRoute || edgePos == ev.end() - 1 ? 0 : *(edgePos + 1);
#ifdef DEBUG_MOVEXY_ANGLE
                std::cout << "normal:" << e->getID() << " prev:" << prevEdge->getID() << " next:";
                if (nextEdge != 0) {
                    std::cout << nextEdge->getID();
                }
                std::cout << std::endl;
#endif
            } else {
#ifdef DEBUG_MOVEXY_ANGLE
                std::cout << "Ego on internal" << std::endl;
#endif
                // an internal edge
                // get the previous edge
                prevEdge = e;
                while (prevEdge != 0 && prevEdge->isInternal()) {
                    MSLane* l = prevEdge->getLanes()[0];
                    l = l->getLogicalPredecessorLane();
                    prevEdge = l == 0 ? 0 : &l->getEdge();
                }
                // check whether the previous edge is on the route (was on the route)
                const ConstMSEdgeVector& ev = veh.getRoute().getEdges();
                ConstMSEdgeVector::const_iterator prevEdgePos = std::find(ev.begin() + veh.getRoutePosition(), ev.end(), prevEdge);
                nextEdge = e;
                while (nextEdge != 0 && nextEdge->isInternal()) {
                    nextEdge = nextEdge->getSuccessors()[0]; // should be only one for an internal edge
                }
                if (prevEdgePos != ev.end() && (prevEdgePos + 1) != ev.end()) {
                    onRoute = *(prevEdgePos + 1) == nextEdge;
                }
#ifdef DEBUG_MOVEXY_ANGLE
                std::cout << "internal:" << e->getID() << " prev:" << prevEdge->getID() << " next:" << nextEdge->getID() << std::endl;
#endif
            }


            // weight the lanes...
            const std::vector<MSLane*>& lanes = e->getLanes();
            const bool perpendicular = false;
            for (std::vector<MSLane*>::const_iterator k = lanes.begin(); k != lanes.end(); ++k) {
                MSLane* lane = *k;
                double langle = 180.;
                double dist = FAR_AWAY;
                double perpendicularDist = FAR_AWAY;
                double off = lane->getShape().nearest_offset_to_point2D(pos, true);
                if (off != GeomHelper::INVALID_OFFSET) {
                    perpendicularDist = lane->getShape().distance2D(pos, true);
                }
                off = lane->getShape().nearest_offset_to_point2D(pos, perpendicular);
                if (off != GeomHelper::INVALID_OFFSET) {
                    dist = lane->getShape().distance2D(pos, perpendicular);
                    langle = GeomHelper::naviDegree(lane->getShape().rotationAtOffset(off));
                }
                bool sameEdge = veh.isOnRoad() && &lane->getEdge() == &veh.getLane()->getEdge() && veh.getEdge()->getLanes()[0]->getLength() > veh.getPositionOnLane() + SPEED2DIST(speed);
                /*
                const MSEdge* rNextEdge = nextEdge;
                while(rNextEdge==0&&lane->getEdge().getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
                    MSLane* next = lane->getLinkCont()[0]->getLane();
                    rNextEdge = next == 0 ? 0 : &next->getEdge();
                }
                */
                double dist2 = dist;
                if (mayLeaveNetwork && dist != perpendicularDist) {
                    // ambiguous mapping. Don't trust this
                    dist2 = FAR_AWAY;
                }
                const double angleDiff = (angle == INVALID_DOUBLE_VALUE ? 0 : GeomHelper::getMinAngleDiff(angle, langle));
#ifdef DEBUG_MOVEXY_ANGLE
                std::cout << lane->getID() << " lAngle:" << langle << " lLength=" << lane->getLength()
                          << " angleDiff:" << angleDiff
                          << " off:" << off
                          << " pDist=" << perpendicularDist
                          << " dist=" << dist
                          << " dist2=" << dist2
                          << "\n";
                std::cout << lane->getID() << " param=" << lane->getParameter(SUMO_PARAM_ORIGID, lane->getID()) << " origID='" << origID << "\n";
#endif
                lane2utility[lane] = LaneUtility(
                                         dist2, perpendicularDist, off, angleDiff,
                                         lane->getParameter(SUMO_PARAM_ORIGID, lane->getID()) == origID,
                                         onRoute, sameEdge, prevEdge, nextEdge);
                // update scaling value
                maxDist = MAX2(maxDist, MIN2(dist, SUMO_const_laneWidth));

            }
        }

        // get the best lane given the previously computed values
        double bestValue = 0;
        MSLane* bestLane = 0;
        for (std::map<MSLane*, LaneUtility>::iterator i = lane2utility.begin(); i != lane2utility.end(); ++i) {
            MSLane* l = (*i).first;
            const LaneUtility& u = (*i).second;
            double distN = u.dist > 999 ? -10 : 1. - (u.dist / maxDist);
            double angleDiffN = 1. - (u.angleDiff / 180.);
            double idN = u.ID ? 1 : 0;
            double onRouteN = u.onRoute ? 1 : 0;
            double sameEdgeN = u.sameEdge ? MIN2(veh.getEdge()->getLength() / speed, (double)1.) : 0;
            double value = (distN * .5 // distance is more important than angle because the vehicle might be driving in the opposite direction
                            + angleDiffN * 0.35 /*.5 */
                            + idN * 1
                            + onRouteN * 0.1
                            + sameEdgeN * 0.1);
#ifdef DEBUG_MOVEXY
            std::cout << " x; l:" << l->getID() << " d:" << u.dist << " dN:" << distN << " aD:" << angleDiffN <<
                      " ID:" << idN << " oRN:" << onRouteN << " sEN:" << sameEdgeN << " value:" << value << std::endl;
#endif
            if (value > bestValue || bestLane == 0) {
                bestValue = value;
                if (u.dist == FAR_AWAY) {
                    bestLane = 0;
                } else {
                    bestLane = l;
                }
            }
        }
        // no best lane found, return
        if (bestLane == 0) {
            return false;
        }
        const LaneUtility& u = lane2utility.find(bestLane)->second;
        bestDistance = u.dist;
        *lane = bestLane;
        lanePos = bestLane->getShape().nearest_offset_to_point2D(pos, false);
        const MSEdge* prevEdge = u.prevEdge;
        if (u.onRoute) {
            const ConstMSEdgeVector& ev = veh.getRoute().getEdges();
            ConstMSEdgeVector::const_iterator prevEdgePos = std::find(ev.begin(), ev.end(), prevEdge);
            routeOffset = (int)std::distance(ev.begin(), prevEdgePos);
            //std::cout << SIMTIME << "vtdMap vehicle=" << veh.getID() << " currLane=" << veh.getLane()->getID() << " routeOffset=" << routeOffset << " edges=" << toString(ev) << " bestLane=" << bestLane->getID() << " prevEdge=" << prevEdge->getID() << "\n";
        } else {
            edges.push_back(u.prevEdge);
            /*
               if(bestLane->getEdge().getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
               edges.push_back(&bestLane->getEdge());
               }
            */
            if (u.nextEdge != 0) {
                edges.push_back(u.nextEdge);
            }
            routeOffset = 0;
#ifdef DEBUG_MOVEXY_ANGLE
            std::cout << "internal2: lane=" << bestLane->getID() << " prev=" << Named::getIDSecure(u.prevEdge) << " next=" << Named::getIDSecure(u.nextEdge) << "\n";;
#endif
        }
        return true;
    }


    bool
        Vehicle::findCloserLane(const MSEdge* edge, const Position& pos, double& bestDistance, MSLane** lane) {
        if (edge == 0) {
            return false;
        }
        const std::vector<MSLane*>& lanes = edge->getLanes();
        bool newBest = false;
        for (std::vector<MSLane*>::const_iterator k = lanes.begin(); k != lanes.end() && bestDistance > POSITION_EPS; ++k) {
            MSLane* candidateLane = *k;
            const double dist = candidateLane->getShape().distance2D(pos); // get distance
#ifdef DEBUG_MOVEXY
            std::cout << "   b at lane " << candidateLane->getID() << " dist:" << dist << " best:" << bestDistance << std::endl;
#endif
            if (dist < bestDistance) {
                // is the new distance the best one? keep then...
                bestDistance = dist;
                *lane = candidateLane;
                newBest = true;
            }
        }
        return newBest;
    }

    bool
        Vehicle::vtdMap_matchingRoutePosition(const Position& pos, const std::string& origID, MSVehicle& veh,
            double& bestDistance, MSLane** lane, double& lanePos, int& routeOffset, ConstMSEdgeVector& /*edges*/) {

        const ConstMSEdgeVector& edges = veh.getRoute().getEdges();
        routeOffset = 0;
        // routes may be looped which makes routeOffset ambiguous. We first try to
        // find the closest upcoming edge on the route and then look for closer passed edges

        // look forward along the route
        const MSEdge* prev = 0;
        UNUSED_PARAMETER(prev); // silence 'unused variable' warning when built without INTERNAL_LANES
        for (ConstMSEdgeVector::const_iterator i = veh.getCurrentRouteEdge(); i != edges.end(); ++i) {
            while (prev != 0) {
                // check internal edge(s)
                const MSEdge* internalCand = prev->getInternalFollowingEdge(*i);
                findCloserLane(internalCand, pos, bestDistance, lane);
                prev = internalCand;
            }
            if (findCloserLane(*i, pos, bestDistance, lane)) {
                routeOffset = (int)std::distance(edges.begin(), i);
            }
            prev = *i;
        }
        // look backward along the route
        const MSEdge* next = *veh.getCurrentRouteEdge();
        UNUSED_PARAMETER(next); // silence 'unused variable' warning when built without INTERNAL_LANES
        for (ConstMSEdgeVector::const_iterator i = veh.getCurrentRouteEdge(); i != edges.begin(); --i) {
            prev = *i;
            while (prev != 0) {
                // check internal edge(s)
                const MSEdge* internalCand = prev->getInternalFollowingEdge(next);
                findCloserLane(internalCand, pos, bestDistance, lane);
                prev = internalCand;
            }
            if (findCloserLane(*i, pos, bestDistance, lane)) {
                routeOffset = (int)std::distance(edges.begin(), i);
            }
            next = *i;
        }

        assert(lane != 0);
        // quit if no solution was found, reporting a failure
        if (lane == 0) {
#ifdef DEBUG_MOVEXY
            std::cout << "  b failed - no best route lane" << std::endl;
#endif
            return false;
        }


        // position may be inaccurate; let's checkt the given index, too
        // @note: this is enabled for non-internal lanes only, as otherwise the position information may ambiguous
        if (!(*lane)->getEdge().isInternal()) {
            const std::vector<MSLane*>& lanes = (*lane)->getEdge().getLanes();
            for (std::vector<MSLane*>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
                if ((*i)->getParameter(SUMO_PARAM_ORIGID, (*i)->getID()) == origID) {
                    *lane = *i;
                    break;
                }
            }
        }
        // check position, stuff, we should have the best lane along the route
        lanePos = MAX2(0., MIN2(double((*lane)->getLength() - POSITION_EPS),
                    (*lane)->interpolateGeometryPosToLanePos(
                        (*lane)->getShape().nearest_offset_to_point2D(pos, false))));
        //std::cout << SIMTIME << " vtdMap_matchingRoutePosition vehicle=" << veh.getID() << " currLane=" << veh.getLane()->getID() << " routeOffset=" << routeOffset << " edges=" << toString(edges) << " lane=" << (*lane)->getID() << "\n";
#ifdef DEBUG_MOVEXY
        std::cout << "  b ok lane " << (*lane)->getID() << " lanePos:" << lanePos << std::endl;
#endif
        return true;
    }


}

/****************************************************************************/
