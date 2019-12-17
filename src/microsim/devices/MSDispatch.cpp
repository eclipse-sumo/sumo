/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDispatch.cpp
/// @author  Jakob Erdmann
/// @date    16.12.2019
///
// An algorithm that performs dispatch for the taxi device
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <limits>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/transportables/MSTransportable.h>
#include "MSRoutingEngine.h"
#include "MSDispatch.h"

//#define DEBUG_RESERVATION
//#define DEBUG_DISPATCH
//#define DEBUG_COND2(obj) (obj->getID() == "p0")
#define DEBUG_COND2(obj) (true)

// ===========================================================================
// MSDispatch methods
// ===========================================================================

void
MSDispatch::addReservation(MSTransportable* person,
        SUMOTime reservationTime, 
        SUMOTime pickupTime,
        const MSEdge* from, double fromPos,
        const MSEdge* to, double toPos) 
{
    myReservations.push_back(Reservation(person, reservationTime, pickupTime, from, fromPos, to, toPos));
    myHasServableReservations = true;
#ifdef DEBUG_RESERVATION
    if (DEBUG_COND2(person)) std::cout << SIMTIME 
            << " addReservation p=" << person->getID() 
            << " rT=" << time2string(reservationTime)
            << " pT=" << time2string(pickupTime)
            << " from=" << from->getID() << " fromPos=" << fromPos
            << " to=" << to->getID() << " toPos=" << toPos
            << "\n";
#endif
}

// ===========================================================================
// MSDispatch_Greedy methods
// ===========================================================================

void
MSDispatch_Greedy::computeDispatch(SUMOTime now, const std::vector<MSDevice_Taxi*>& fleet) {
    int numDispatched = 0;
    int numPostponed = 0;
    // find available vehicles
    std::set<MSDevice_Taxi*> available;
    for (auto* taxi : fleet) {
        if (taxi->isEmpty()) {
            available.insert(taxi);
        }
    }
#ifdef DEBUG_DISPATCH
    std::cout << SIMTIME << " computeDispatch fleet=" << fleet.size() << " available=" << available.size() << "\n";
#endif
    // greedy assigned closest vehicle in reservation order
    SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = myRoutingMode == 1 ? MSRoutingEngine::getRouterTT(0) : MSNet::getInstance()->getRouterTT(0);
    std::sort(myReservations.begin(), myReservations.end(), time_sorter());
    for (auto it = myReservations.begin(); it != myReservations.end();) {
        if (available.size() == 0) {
            break;
        }
        Reservation& res = *it;
        if (res.recheck > now) {
            it++;
            numPostponed++;
            continue;
        }
        //Position pos = res.from->getLanes().front()->geometryPositionAtOffset(res.fromPos);
        MSDevice_Taxi* closest = nullptr;
        SUMOTime closestTime = SUMOTime_MAX;
        bool toEarly = false;
        for (auto* taxi : available) {
            ConstMSEdgeVector edges;
            router.compute(res.from, res.to, &taxi->getHolder(), now, edges);
            SUMOTime travelTime = TIME2STEPS(router.recomputeCosts(edges, &taxi->getHolder(), now));
            if (travelTime < closestTime) {
                closestTime = travelTime;
                closest = taxi;
                SUMOTime taxiWait = res.pickupTime - (now + closestTime);
                if (taxiWait > myMaximumWaitingTime) {
                    // no need to service this customer now
                    toEarly = true;
                    res.recheck += MAX2(now + myRecheckTime, res.pickupTime - closestTime - myRecheckSafety);
                    break;
                }
            }
        }
        if (toEarly) {
            it++;
            numPostponed++;
        } else {
            closest->dispatch(res);
            it = myReservations.erase(it);
            numDispatched++; 
#ifdef DEBUG_DISPATCH
            if (DEBUG_COND2(person)) std::cout << SIMTIME << " dispatch taxi=" << closest->getHolder().getID() << " person=" << res.person->getID() << "\n";
#endif
        }
    }
    // check if any taxis are able to service the remaining requests
    myHasServableReservations = myReservations.size() > 0 && available.size() < fleet.size() && numPostponed == 0 && numDispatched == 0;
}


/****************************************************************************/
