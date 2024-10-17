/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2014-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSPModel_Interacting.cpp
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 13 Jan 2014
///
// The pedestrian following model (prototype)
/****************************************************************************/
#include <config.h>

#include <microsim/MSGlobals.h>
#include <microsim/MSLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSNet.h>
#include "MSPModel_Interacting.h"

// #define DEBUG_INTERACTING


// ===========================================================================
// static members
// ===========================================================================
MSPModel_Interacting::Pedestrians MSPModel_Interacting::noPedestrians;


// ===========================================================================
// MSPModel_Interacting method definitions
// ===========================================================================
MSPModel_Interacting::~MSPModel_Interacting() {
    clearState();
}


void
MSPModel_Interacting::clearState() {
    myActiveLanes.clear();
    myNumActivePedestrians = 0;
    myAmActive = false;
}


void
MSPModel_Interacting::remove(MSTransportableStateAdapter* state) {
    Pedestrians& pedestrians = myActiveLanes[state->getLane()];
    MSPModel_InteractingState* const p = static_cast<MSPModel_InteractingState*>(state);
    const auto& it = std::find(pedestrians.begin(), pedestrians.end(), p);
    if (it != pedestrians.end()) {
        if (p->getNextCrossing() != nullptr) {
            unregisterCrossingApproach(*p, p->getNextCrossing());
        }
        pedestrians.erase(it);
        myNumActivePedestrians--;
    }
}


bool
MSPModel_Interacting::blockedAtDist(const SUMOTrafficObject* ego, const MSLane* lane, double vehSide, double vehWidth,
                                    double oncomingGap, std::vector<const MSPerson*>* collectBlockers) {
    for (const MSPModel_InteractingState* ped : getPedestrians(lane)) {
        const double leaderFrontDist = (ped->getDirection() == FORWARD ? vehSide - ped->getEdgePos(0) : ped->getEdgePos(0) - vehSide);
        const double leaderBackDist = leaderFrontDist + ped->getPerson()->getVehicleType().getLength();
#ifdef DEBUG_INTERACTING
        if DEBUGCOND(ped) {
            std::cout << SIMTIME << " lane=" << lane->getID() << " dir=" << ped->getDirection() << " pX=" << ped->getEdgePos(0) << " pL=" << ped.getLength()
                      << " vehSide=" << vehSide
                      << " vehWidth=" << vehWidth
                      << " lBD=" << leaderBackDist
                      << " lFD=" << leaderFrontDist
                      << "\n";
        }
#endif
        if (leaderBackDist >= -vehWidth
                && (leaderFrontDist < 0
                    // give right of way to (close) approaching pedestrians unless they are standing
                    || (leaderFrontDist <= oncomingGap && ped->getWaitingTime() < TIME2STEPS(2.0)))) {
            if (MSLink::ignoreFoe(ego, ped->getPerson())) {
                continue;
            }
            // found one pedestrian that is not completely past the crossing point
            //std::cout << SIMTIME << " blocking pedestrian foeLane=" << lane->getID() << " ped=" << ped->getPerson()->getID() << " dir=" << ped->getDirection() << " pX=" << ped->getEdgePos(0) << " pL=" << ped.getLength() << " fDTC=" << distToCrossing << " lBD=" << leaderBackDist << "\n";
            if (collectBlockers == nullptr) {
                return true;
            }
            collectBlockers->push_back(ped->getPerson());
        }
    }
    if (collectBlockers == nullptr) {
        return false;
    }
    return collectBlockers->size() > 0;
}


bool
MSPModel_Interacting::hasPedestrians(const MSLane* lane) {
    return getPedestrians(lane).size() > 0;
}


bool
MSPModel_Interacting::usingInternalLanes() {
    return usingInternalLanesStatic();
}


bool
MSPModel_Interacting::usingInternalLanesStatic() {
    return MSGlobals::gUsingInternalLanes && MSNet::getInstance()->hasInternalLinks() && MSNet::getInstance()->hasPedestrianNetwork();
}


PersonDist
MSPModel_Interacting::nextBlocking(const MSLane* lane, double minPos, double minRight, double maxLeft, double stopTime, bool bidi) {
    PersonDist result((const MSPerson*)nullptr, std::numeric_limits<double>::max());
    for (const MSPModel_InteractingState* ped : getPedestrians(lane)) {
        // account for distance covered by oncoming pedestrians
        double relX2 = ped->getEdgePos(0) - (ped->getDirection() == FORWARD ? 0 : stopTime * ped->getPerson()->getMaxSpeed());
        double dist = ((relX2 - minPos) * (bidi ? -1 : 1)
                       - (ped->getDirection() == FORWARD ? ped->getPerson()->getVehicleType().getLength() : 0));
        const bool aheadOfVehicle = bidi ? ped->getEdgePos(0) < minPos : ped->getEdgePos(0) > minPos;
        if (aheadOfVehicle && dist < result.second) {
            const double center = ped->getLatOffset() + 0.5 * lane->getWidth();
            const double halfWidth = 0.5 * ped->getPerson()->getVehicleType().getWidth();
            const bool overlap = (center + halfWidth > minRight && center - halfWidth < maxLeft);
#ifdef DEBUG_INTERACTING
            if DEBUGCOND(ped) {
                std::cout << "  nextBlocking lane=" << lane->getID() << " bidi=" << bidi
                          << " minPos=" << minPos << " minRight=" << minRight << " maxLeft=" << maxLeft
                          << " stopTime=" << stopTime
                          << " pedY=" << ped->getPosLat()
                          << " pedX=" << ped->getEdgePos(0)
                          << " relX2=" << relX2
                          << " center=" << center
                          << " pedLeft=" << center + halfWidth
                          << " pedRight=" << center - halfWidth
                          << " overlap=" << overlap
                          << "\n";
            }
#endif
            if (overlap) {
                result.first = ped->getPerson();
                result.second = dist;
            }
        }
    }
    return result;
}


MSPModel_Interacting::Pedestrians&
MSPModel_Interacting::getPedestrians(const MSLane* lane) {
    ActiveLanes::iterator it = myActiveLanes.find(lane);
    if (it != myActiveLanes.end()) {
        //std::cout << " found lane=" << lane->getID() << " n=" << it->second.size() << "\n";
        return (it->second);
    }
    return noPedestrians;
}


void
MSPModel_Interacting::unregisterCrossingApproach(const MSPModel_InteractingState& ped, const MSLane* crossing) {
    // person has entered the crossing
    crossing->getIncomingLanes()[0].viaLink->removeApproachingPerson(ped.getPerson());
#ifdef DEBUG_INTERACTING
    if DEBUGCOND(ped) {
        std::cout << SIMTIME << " unregister " << ped->getPerson()->getID() << " at crossing " << crossing->getID() << "\n";
    }
#endif
}


/****************************************************************************/
