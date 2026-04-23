/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2007-2026 German Aerospace Center (DLR) and others.
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
/// @file    MSIdling.cpp
/// @author  Jakob Erdmann
/// @author  Mirko Barthauer
/// @date    17.08.2020
///
// An algorithm that performs Idling for the taxi device
/****************************************************************************/
#include <config.h>

#include <limits>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSStop.h>
#include <microsim/MSParkingArea.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/trigger/MSTriggeredRerouter.h>
#include <mesosim/MELoop.h>
#include <mesosim/MEVehicle.h>
#include "MSRoutingEngine.h"
#include "MSIdling.h"

//#define DEBUG_IDLING
//#define DEBUG_COND(obj) (obj->getHolder().getID() == "p0")
//#define DEBUG_COND(obj) (obj->getHolder().isSelected())
#define DEBUG_COND(obj) (true)


// ===========================================================================
// MSIdling_stop methods
// ===========================================================================

void
MSIdling_Stop::idle(MSDevice_Taxi* taxi) {
    if (!taxi->getHolder().hasStops()) {
#ifdef DEBUG_IDLING
        if (DEBUG_COND(taxi)) {
            std::cout << SIMTIME << " taxi=" << taxi->getHolder().getID() << " MSIdling_Stop add stop\n";
        }
#endif
        std::string errorOut;
        double brakeGap = 0;
        std::pair<const MSLane*, double> stopPos;
        if (MSGlobals::gUseMesoSim) {
            // stops are only checked in MESegment::receive so we need to put this onto the next segment
            MSBaseVehicle& veh = dynamic_cast<MSBaseVehicle&>(taxi->getHolder());
            MSRouteIterator ri = veh.getCurrentRouteEdge();
            MESegment* curSeg = MSGlobals::gMesoNet->getSegmentForEdge(**ri, veh.getPositionOnLane());
            MESegment* stopSeg = curSeg->getNextSegment();
            if (stopSeg == nullptr) {
                if ((ri + 1) != veh.getRoute().end()) {
                    stopSeg = MSGlobals::gMesoNet->getSegmentForEdge(**(ri + 1), 0);
                } else {
                    WRITE_WARNINGF(TL("Idle taxi '%' has no next segment to stop. time=%."), taxi->getHolder().getID(), time2string(SIMSTEP));
                    return;
                }
            }
            // determine offset of stopSeg
            double stopOffset = 0;
            const MSEdge& stopEdge = stopSeg->getEdge();
            MESegment* seg = MSGlobals::gMesoNet->getSegmentForEdge(stopEdge);
            while (seg != stopSeg) {
                stopOffset += seg->getLength();
                seg = seg->getNextSegment();
            }
            stopPos = std::make_pair(stopEdge.getLanes()[0], stopOffset);
        } else {
            MSVehicle& veh = dynamic_cast<MSVehicle&>(taxi->getHolder());
            brakeGap = veh.getCarFollowModel().brakeGap(veh.getSpeed(), veh.getCarFollowModel().getMaxDecel(), 0.0);
            stopPos = veh.getLanePosAfterDist(brakeGap);
        }
        if (stopPos.first != nullptr) {
            SUMOVehicleParameter::Stop stop;
            if (MSGlobals::gUseMesoSim) {
                stop.edge = stopPos.first->getEdge().getID();
            } else {
                stop.lane = stopPos.first->getID();
            }
            stop.startPos = MAX2(0.0, stopPos.second - POSITION_EPS);
            stop.endPos = stopPos.second;
            if (MSGlobals::gUseMesoSim) {
                // meso needs the stop to be on the next segment
                stop.startPos += POSITION_EPS;
                stop.endPos += POSITION_EPS;
            }
            stop.actType = "idling";
            taxi->getHolder().addTraciStop(stop, errorOut);
            if (errorOut != "") {
                WRITE_WARNING(errorOut);
            }
        } else {
            WRITE_WARNINGF(TL("Idle taxi '%' could not stop within %m"), taxi->getHolder().getID(), toString(brakeGap));
        }
    } else {
#ifdef DEBUG_IDLING
        if (DEBUG_COND(taxi)) {
            std::cout << SIMTIME << " taxi=" << taxi->getHolder().getID() << " MSIdling_Stop reusing stop with duration " << time2string(taxi->getHolder().getNextStop().duration) << "\n";
        }
#endif
    }
    if (taxi->getHolder().hasStops()) {
        MSStop& stop = taxi->getHolder().getNextStopMutable();
        SUMOVehicleParameter::Stop& pars = const_cast<SUMOVehicleParameter::Stop&>(stop.pars);
        if (taxi->getHolder().getVehicleType().getContainerCapacity() > 0) {
            stop.containerTriggered = true;
            pars.containerTriggered = true;
        } else {
            stop.triggered = true;
            pars.triggered = true;
        }
        pars.parametersSet |= STOP_TRIGGER_SET | STOP_PARKING_SET | STOP_END_SET;
        pars.parking = ParkingType::OFFROAD;
        if (MSGlobals::gUseMesoSim) {
            MEVehicle& veh = dynamic_cast<MEVehicle&>(taxi->getHolder());
            // register triggered stop
            veh.mayProceed();
        }
    }
}


// ===========================================================================
// MSIdling_RandomCircling methods
// ===========================================================================

void
MSIdling_RandomCircling::idle(MSDevice_Taxi* taxi) {
    SUMOVehicle& veh = taxi->getHolder();
    ConstMSEdgeVector edges = veh.getRoute().getEdges();
    ConstMSEdgeVector newEdges;
    double remainingDist = -veh.getPositionOnLane();
    int remainingEdges = 0;
    int routePos = veh.getRoutePosition();
    const int routeLength = (int)edges.size();
    while (routePos + 1 < routeLength && (remainingEdges < 2 || remainingDist < 200)) {
        const MSEdge* edge = edges[routePos];
        remainingDist += edge->getLength();
        remainingEdges++;
        routePos++;
        newEdges.push_back(edge);
    }
    const MSEdge* lastEdge = edges.back();
    newEdges.push_back(lastEdge);
    int added = 0;
    while (remainingEdges < 2 || remainingDist < 200) {
        remainingDist += lastEdge->getLength();
        remainingEdges++;
        MSEdgeVector successors = lastEdge->getSuccessors(veh.getVClass());
        for (auto it = successors.begin(); it != successors.end();) {
            if ((*it)->getFunction() == SumoXMLEdgeFunc::CONNECTOR) {
                it = successors.erase(it);
            } else {
                it++;
            }
        }
        if (successors.size() == 0) {
            WRITE_WARNINGF(TL("Vehicle '%' ends idling in a cul-de-sac"), veh.getID());
            break;
        } else {
            int nextIndex = RandHelper::rand((int)successors.size(), veh.getRNG());
            newEdges.push_back(successors[nextIndex]);
            lastEdge = newEdges.back();
            added++;
        }
    }
    if (added > 0) {
        //std::cout << SIMTIME << " circleVeh=" << veh.getID() << "  newEdges=" << toString(newEdges) << "\n";
        veh.replaceRouteEdges(newEdges, -1, 0, "taxi:idling:randomCircling", false, false, false);
    }
}

// ===========================================================================
// MSIdling_TaxiStand methods
// ===========================================================================

void
MSIdling_TaxiStand::idle(MSDevice_Taxi* taxi) {
    MSBaseVehicle& veh = dynamic_cast<MSBaseVehicle&>(taxi->getHolder());

    const MSTriggeredRerouter::RerouteInterval* rerouteDef = myRerouter->getCurrentReroute(SIMSTEP);
    if (rerouteDef == nullptr || rerouteDef->parkProbs.getVals().size() == 0) {
        if (!myHaveWarned) {
            WRITE_WARNINGF(TL("Could not determine taxi stand for vehicle '%' at time=%"), veh.getID(), time2string(SIMSTEP));
            myHaveWarned = true;
        }
        return;
    }
    MSStop* lastStop = nullptr;
    if (veh.hasStops()) {
        lastStop = &veh.getStop((int)veh.getStops().size() - 1);
    }
    if (lastStop == nullptr || lastStop->parkingarea == nullptr) {
        const MSParkingArea* pa = dynamic_cast<MSParkingArea*>(rerouteDef->parkProbs.getVals().front().first);
        SUMOVehicleParameter::Stop stop;
        stop.lane = pa->getLane().getID();
        stop.startPos = pa->getBeginLanePosition();
        stop.endPos = pa->getEndLanePosition();

        if (taxi->getHolder().getVehicleType().getContainerCapacity() > 0) {
            stop.containerTriggered = true;
        } else {
            stop.triggered = true;
        }
        stop.actType = "idling";
        stop.parkingarea = pa->getID();
        stop.parking = ParkingType::OFFROAD;
        const int nextStopIndex = (int)veh.getStops().size();
        std::string error;
        if (!veh.insertStop(nextStopIndex, stop, "taxi:taxistand", false, error)) {
            WRITE_WARNING("Stop insertion failed for idling taxi '" + veh.getID() + "' (" + error + ").");
        } else {
            //std::cout << SIMTIME << " taxistandsVeh=" << veh.getID() << "  driving to parkingArea " << pa->getID() << "\n";
            myRerouter->triggerRouting(veh, MSMoveReminder::NOTIFICATION_PARKING_REROUTE);
        }
    } else if (!MSGlobals::gUseMesoSim) {
        //std::cout << SIMTIME << " taxistandsVeh=" << veh.getID() << "  already driving to parkingArea\n";
        MSParkingArea* pa = lastStop->parkingarea;
        if (taxi->getHolder().isStoppedTriggered() && pa != nullptr && pa->mustAdvance(taxi->getHolder().getVClass())) {
            double vehPos = taxi->getHolder().getPositionOnLane();
            double targetPos = pa->getLastFreePos(taxi->getHolder(), vehPos);
            //std::cout << SIMTIME << " veh=" << taxi->getHolder().getID() << " vehPos=" << vehPos << " targetPos=" << targetPos << " cap=" << pa->getCapacity() << " occ=" << pa->getOccupancyIncludingBlocked() << "\n";
            if (targetPos > vehPos + POSITION_EPS) {
                taxi->getHolder().abortNextStop();
                idle(taxi);
            } else {
                auto follower = veh.getFollower();
                if (follower.first != nullptr && follower.first->getWaitingTime() > DELTA_T) {
                    // advance in queue to unblock follower
                    SUMOVehicleParameter::Stop stop = taxi->getHolder().getNextStop().pars;
                    taxi->getHolder().abortNextStop();
                    ConstMSEdgeVector loopedRoute;
                    SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = MSRoutingEngine::getRouterTT(veh.getRNGIndex(), veh.getVClass());
                    router.computeLooped(veh.getEdge(), veh.getEdge(), &veh, SIMSTEP, loopedRoute);
                    veh.replaceRouteEdges(loopedRoute, -1, 0, "taxi:idling_unblock", false, false, false);
                    stop.index = 1;
                    std::string error;
                    veh.addStop(stop, error);
                }
            }
        }
    }
}



/****************************************************************************/
