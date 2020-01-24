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
#include "MSDispatch_GreedyShared.h"

//#define DEBUG_RESERVATION
//#define DEBUG_TRAVELTIME
//#define DEBUG_DISPATCH
//#define DEBUG_SERVABLE
//#define DEBUG_COND2(obj) (obj->getID() == "p0")
#define DEBUG_COND2(obj) (true)


// ===========================================================================
// MSDispatch_GreedyShared methods
// ===========================================================================

int
MSDispatch_GreedyShared::dispatch(MSDevice_Taxi* taxi, Reservation* res, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router, std::vector<Reservation*>& reservations) {
#ifdef DEBUG_DISPATCH
    if (DEBUG_COND2(person)) std::cout << SIMTIME << " dispatch taxi=" << taxi->getHolder().getID() << " person=" << toString(res->persons) << "\n";
#endif
    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    auto it = std::find(reservations.begin(), reservations.end(), res);
    // check whether the ride can be shared
    bool share = false;
    Reservation* res2 = nullptr;
    double absLoss = 0;
    double relLoss = 0;
    double absLoss2 = 0;
    double relLoss2 = 0;
    double directTime = -1; // only computed once for res
    double directTime2 = -1;
    for (auto it2 = it + 1; it2 != reservations.end(); it2++) {
        res2 = *it2;
        // res picks up res2 on the way
        directTime2 = -1; // reset for new candidate
        const double detourTime = computeDetourTime(res->pickupTime, res2->pickupTime, taxi,
                res->from, res->fromPos, res2->from, res2->fromPos, res->to, res->toPos, router, directTime);
        const double absLossPickup = detourTime - directTime;
        const double relLossPickup = absLoss / directTime;

        if (absLossPickup < myAbsoluteLossThreshold && relLossPickup < myRelativeLossThreshold) {
            double directTimeTmp = -1; // direct time from picking up res2 to dropping of res
            // case 1: res2 is dropped of before res (more detour for res)
            double detourTime2 = computeDetourTime(res2->pickupTime, now, taxi, 
                    res2->from, res2->fromPos, res2->to, res2->toPos, res->to, res->toPos, router, directTimeTmp);
            const double absLoss_c1 = absLossPickup + (detourTime2 - directTimeTmp);
            const double relLoss_c1 = absLoss_c1 / directTime;

            // case 2: res2 is dropped of after res (detour for res2)
            double detourTime3 = computeDetourTime(res2->pickupTime, now, taxi, 
                    res2->from, res2->fromPos, res->to, res->toPos, res2->to, res2->toPos, router, directTime2);
            const double absLoss_c2 = detourTime3 - directTime2;
            const double relLoss_c2 = absLoss_c2 / directTime2;

            if (absLoss_c2 <= absLoss_c1 &&
                    absLoss_c2 < myAbsoluteLossThreshold && relLoss_c2 < myRelativeLossThreshold) {
                // use case 2
                taxi->dispatchShared({res, res2, res, res2});
                share = true;
                absLoss = absLossPickup;
                relLoss = relLossPickup;
                absLoss2 = absLoss_c2;
                relLoss2 = relLoss_c2;
            } else if (absLoss < myAbsoluteLossThreshold && relLoss < myRelativeLossThreshold) {
                // use case 1
                taxi->dispatchShared({res, res2, res2, res});
                share = true;
                absLoss = absLoss_c1;
                relLoss = relLoss_c1;
                absLoss2 = 0;
                relLoss2 = 0;
            }
            if (share) {
                reservations.erase(it2); // it (before i2) stays valid
                break;
            }
        }
    }
    if (share) {
#ifdef DEBUG_DISPATCH
        if (DEBUG_COND2(person)) std::cout << "  sharing ride with " << toString(res2->persons) 
            << " absLoss=" << absLoss << " relLoss=" << relLoss
                << " absLoss2=" << absLoss2 << " relLoss2=" << relLoss2
                << "\n";
#endif
        servedReservation(res2); // deleting res2
    } else {
        taxi->dispatch(*res);
    }
    servedReservation(res); // deleting res
    return share ? 2 : 1;
}


/****************************************************************************/
