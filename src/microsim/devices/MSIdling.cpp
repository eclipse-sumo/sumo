/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2023 German Aerospace Center (DLR) and others.
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
#include <microsim/transportables/MSTransportable.h>
#include <mesosim/MELoop.h>
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
            brakeGap = veh.getCarFollowModel().brakeGap(veh.getSpeed());
            stopPos = veh.getLanePosAfterDist(brakeGap);
        }
        if (stopPos.first != nullptr) {
            SUMOVehicleParameter::Stop stop;
            if (MSGlobals::gUseMesoSim) {
                stop.edge = stopPos.first->getEdge().getID();
            } else {
                stop.lane = stopPos.first->getID();
            }
            stop.startPos = stopPos.second;
            stop.endPos = stopPos.second + POSITION_EPS;
            if (taxi->getHolder().getVehicleType().getContainerCapacity() > 0) {
                stop.containerTriggered = true;
            } else {
                stop.triggered = true;
            }
            stop.actType = "idling";
            stop.parking = ParkingType::OFFROAD;
            taxi->getHolder().addTraciStop(stop, errorOut);
            if (errorOut != "") {
                WRITE_WARNING(errorOut);
            }
        } else {
            WRITE_WARNINGF(TL("Idle taxi '%' could not stop within %m"), taxi->getHolder().getID(), toString(brakeGap));
        }
    } else {
        MSStop& stop = taxi->getHolder().getNextStop();
#ifdef DEBUG_IDLING
        if (DEBUG_COND(taxi)) {
            std::cout << SIMTIME << " taxi=" << taxi->getHolder().getID() << " MSIdling_Stop reusing stop with duration " << time2string(stop.duration) << "\n";
        }
#endif
        if (taxi->getHolder().getVehicleType().getContainerCapacity() > 0) {
            stop.containerTriggered = true;
        } else {
            stop.triggered = true;
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


/****************************************************************************/
