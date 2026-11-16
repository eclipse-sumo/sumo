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
/// @file    MSDispatch_Greedy.cpp
/// @author  Jakob Erdmann
/// @date    16.12.2019
///
// An algorithm that performs dispatch for the taxi device
/****************************************************************************/
#include <config.h>

#include <limits>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/transportables/MSTransportable.h>
#include "MSRoutingEngine.h"
#include "MSDispatch_GreedyShared.h"

//#define DEBUG_DISPATCH
//#define DEBUG_SERVABLE
//#define DEBUG_TRAVELTIME
//#define DEBUG_COND2(res) (res->id == "")
#define DEBUG_COND2(obj) (true)

// ===========================================================================
// MSDispatch_Greedy methods
// ===========================================================================

void
MSDispatch_Greedy::computeDispatch(SUMOTime now, const std::vector<MSDevice_Taxi*>& fleet) {
    int numDispatched = 0;
    int numPostponed = 0;
    // find available vehicles
    std::set<MSDevice_Taxi*, MSVehicleDevice::ComparatorNumericalVehicleIdLess> available;
    for (auto* taxi : fleet) {
        if (taxi->isEmpty()) {
            available.insert(taxi);
        }
    }
    // greedy assign closest vehicle in reservation order
    SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = getRouter();
    std::vector<Reservation*> reservations = getReservations();
    std::sort(reservations.begin(), reservations.end(), time_sorter());
#ifdef DEBUG_DISPATCH
    std::cout << SIMTIME << " computeDispatch fleet=" << fleet.size() << " available=" << available.size() << " reservations=" << toString(reservations) << "\n";
#endif
    for (auto it = reservations.begin(); it != reservations.end();) {
        if (available.size() == 0) {
            break;
        }
        Reservation* res = *it;
        if (res->recheck > now) {
            it++;
            numPostponed++;
            continue;
        }
        //Position pos = res.from->getLanes().front()->geometryPositionAtOffset(res.fromPos);
        MSDevice_Taxi* closest = nullptr;
        SUMOTime closestTime = SUMOTime_MAX;
        bool tooEarly = false;
        bool unreachable = false;
        bool unreachableDest = false;
        for (auto* taxi : available) {
            if (remainingCapacity(taxi, res) < 0 || !taxi->compatibleLine(res)) {
                continue;
            }
            SUMOTime travelTime = computePickupTime(now, taxi, *res, router);
            const bool destConnected = isReachable(now, taxi, *res, router);
#ifdef DEBUG_TRAVELTIME
            if (DEBUG_COND2(res)) {
                std::cout << SIMTIME << " taxi=" << taxi->getHolder().getID() << " person=" << toString(res->persons) << " traveltime=" << time2string(travelTime) << " reachable=" << destConnected << "\n";
            }
#endif
            if (travelTime < closestTime && destConnected) {
                closestTime = travelTime;
                closest = taxi;
                SUMOTime taxiWait = res->pickupTime - (now + closestTime);
                if (taxiWait > myMaximumWaitingTime) {
                    // no need to service this customer now
                    tooEarly = true;
                    res->recheck += MAX2(now + myRecheckTime, res->pickupTime - closestTime - myRecheckSafety);
                    break;
                }
            } else if (travelTime == SUMOTime_MAX) {
                unreachable = true;
            } else if (!destConnected) {
                unreachableDest = true;
            }
        }
        if (tooEarly || closest == nullptr) {
            // too early or all taxis are too small
            SUMOTime resWait = now - res->pickupTime;
            if ((unreachable || unreachableDest) && resWait > myKeepUnreachableResTime) {
                it = reservations.erase(it);
                if (unreachable) {
                    WRITE_WARNINGF("Aborting reservation for customers '%' from '%' after waiting time % because no taxi can reach the pickup location, time=%.",
                                   toString(res->persons), res->from->getID(), time2string(resWait), time2string(SIMSTEP));
                } else {
                    WRITE_WARNINGF("Aborting reservation for customers '%' to '%' after waiting time % because no taxi can reach the dropoff location, time=%.",
                                   toString(res->persons), res->from->getID(), time2string(resWait), time2string(SIMSTEP));
                }
                std::set<const MSTransportable*, ComparatorNumericalIdLess> persons = res->persons;
                for (const MSTransportable* p : persons) {
                    removeReservation(const_cast<MSTransportable*>(p), res->from, res->fromPos, res->to, res->toPos, res->group);
                }
            } else {
                it++;
                numPostponed++;
            }
        } else {
            numDispatched += dispatch(closest, it, router, reservations);
            available.erase(closest);
        }
    }
    // check if any taxis are able to service the remaining requests
    myHasServableReservations = reservations.size() > 0 && (available.size() < fleet.size() || numPostponed > 0 || numDispatched > 0);
#ifdef DEBUG_SERVABLE
    std::cout << SIMTIME << " reservations=" << reservations.size() << " avail=" << available.size()
              << " fleet=" << fleet.size() << " postponed=" << numPostponed << " dispatched=" << numDispatched << "\n";
#endif
}


int
MSDispatch_Greedy::dispatch(MSDevice_Taxi* taxi, std::vector<Reservation*>::iterator& resIt, SUMOAbstractRouter<MSEdge, SUMOVehicle>& /*router*/, std::vector<Reservation*>& reservations) {
#ifdef DEBUG_DISPATCH
    if (DEBUG_COND2(*resIt)) {
        std::cout << SIMTIME << " dispatch taxi=" << taxi->getHolder().getID() << " person=" << toString((*resIt)->persons) << "\n";
    }
#endif
    taxi->dispatch(**resIt);
    servedReservation(*resIt, taxi); // deleting res
    resIt = reservations.erase(resIt);
    return 1;
}


// ===========================================================================
// MSDispatch_GreedyClosest methods
// ===========================================================================

void
MSDispatch_GreedyClosest::computeDispatch(SUMOTime now, const std::vector<MSDevice_Taxi*>& fleet) {
    bool havePostponed = false;
    int numDispatched = 0;
    // find available vehicles
    std::set<MSDevice_Taxi*, MSVehicleDevice::ComparatorNumericalVehicleIdLess> available;
    for (auto* taxi : fleet) {
        if (taxi->isEmpty()) {
            available.insert(taxi);
        }
    }
#ifdef DEBUG_DISPATCH
    std::cout << SIMTIME << " computeDispatch fleet=" << fleet.size() << " available=" << available.size() << "\n";
#endif
    // greedy assign closest vehicle
    SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = myRoutingMode == 1 ? MSRoutingEngine::getRouterTT(0, SVC_TAXI) : MSNet::getInstance()->getRouterTT(0);
    std::vector<Reservation*> activeReservations;
    for (Reservation* res : getReservations()) {
        if (res->recheck <= now) {
            activeReservations.push_back(res);
        }
    }
    if (!available.empty() && !activeReservations.empty()) {
        struct DispatchCandidate {
            SUMOTime travelTime;
            int resIdx;
            int taxiIdx;
            Reservation* res;
            bool operator<(const DispatchCandidate& other) const {
                if (travelTime != other.travelTime) {
                    return travelTime < other.travelTime;
                }
                if (resIdx != other.resIdx) {
                    return resIdx < other.resIdx;
                }
                return taxiIdx < other.taxiIdx;
            }
        };
        const std::vector<MSDevice_Taxi*> availableVec(available.begin(), available.end());
        std::vector<DispatchCandidate> candidates;
        candidates.reserve(activeReservations.size() * availableVec.size());
        router.init(-1, now);
        router.setAutoBulkMode(true);
        for (int tIdx = 0; tIdx < (int)availableVec.size(); ++tIdx) {
            MSDevice_Taxi* const taxi = availableVec[tIdx];
            for (int rIdx = 0; rIdx < (int)activeReservations.size(); ++rIdx) {
                Reservation* const res = activeReservations[rIdx];
                if (remainingCapacity(taxi, res) < 0 || !taxi->compatibleLine(res)) {
                    continue;
                }
                const SUMOTime travelTime = computePickupTime(now, taxi, *res, router);
                const SUMOTime taxiWait = res->pickupTime - (now + travelTime);
#ifdef DEBUG_TRAVELTIME
                if (DEBUG_COND2(res)) std::cout << SIMTIME << " taxi=" << taxi->getHolder().getID() << " person=" << toString(res->persons)
                                                       << " traveltime=" << time2string(travelTime)
                                                       << " pickupTime=" << time2string(res->pickupTime)
                                                       << " taxiWait=" << time2string(taxiWait) << "\n";
#endif
                if (travelTime < SUMOTime_MAX && taxiWait < myMaximumWaitingTime) {
                    candidates.push_back({travelTime, rIdx, tIdx, res});
                }
            }
        }
        router.setAutoBulkMode(false);
        std::sort(candidates.begin(), candidates.end());
        for (const DispatchCandidate& cand : candidates) {
            if (available.empty() || activeReservations.empty()) {
                break;
            }
            MSDevice_Taxi* const closestTaxi = availableVec[cand.taxiIdx];
            if (available.count(closestTaxi) == 0) {
                continue;
            }
            auto closeIt = std::find(activeReservations.begin(), activeReservations.end(), cand.res);
            if (closeIt == activeReservations.end()) {
                continue;
            }
#ifdef DEBUG_DISPATCH
            if (DEBUG_COND2(cand.res)) std::cout << SIMTIME << " bestTaxi=" << closestTaxi->getHolder().getID() << " bestRes=" << toString(cand.res->persons)
                                                        << " taxiPos=" << closestTaxi->getHolder().getPositionOnLane() << " resFromPos=" << cand.res->fromPos
                                                        << " traveltime=" << time2string(cand.travelTime)
                                                        << " taxiWait=" << time2string(cand.res->pickupTime - (now + cand.travelTime)) << "\n";
#endif
            numDispatched += dispatch(closestTaxi, closeIt, router, activeReservations);
            available.erase(closestTaxi);
        }
        if (!available.empty() && !activeReservations.empty()) {
            // all remaining reservations are too early, unreachable, or too big
            havePostponed = true;
        }
    }
    // check if any taxis are able to service the remaining requests
    myHasServableReservations = getReservations().size() > 0 && (available.size() < fleet.size() || havePostponed || numDispatched > 0);
#ifdef DEBUG_SERVABLE
    std::cout << SIMTIME << " reservations=" << getReservations().size() << " avail=" << available.size()
              << " fleet=" << fleet.size() << " postponed=" << havePostponed << " dispatched=" << numDispatched << "\n";
#endif
}


/****************************************************************************/
