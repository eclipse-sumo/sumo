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
/// @file    MSDispatch_TraCI.cpp
/// @author  Jakob Erdmann
/// @date    16.12.2019
///
// An algorithm that performs dispatch for the taxi device
/****************************************************************************/
#include <config.h>

#include <limits>
#include <microsim/transportables/MSTransportable.h>
#include "MSDispatch_TraCI.h"

//#define DEBUG_RESERVATION
//#define DEBUG_DISPATCH
//#define DEBUG_SERVABLE
//#define DEBUG_TRAVELTIME
//#define DEBUG_DETOUR
//#define DEBUG_COND2(obj) (obj->getID() == "p0")
#define DEBUG_COND2(obj) (true)

// ===========================================================================
// MSDispatch_TraCI methods
// ===========================================================================

Reservation*
MSDispatch_TraCI::addReservation(MSTransportable* person,
                                 SUMOTime reservationTime,
                                 SUMOTime pickupTime,
                                 const MSEdge* from, double fromPos,
                                 const MSEdge* to, double toPos,
                                 std::string group,
                                 const std::string& line,
                                 int maxCapacity,
                                 int maxContainerCapacity) {
    Reservation* res = MSDispatch::addReservation(person, reservationTime, pickupTime, from, fromPos, to, toPos, group, line, maxCapacity, maxContainerCapacity);
    if (!myReservationLookup.has(res)) {
        myReservationLookup.insert(res->id, res);
    }
    return res;
}

std::string
MSDispatch_TraCI::removeReservation(MSTransportable* person,
                                    const MSEdge* from, double fromPos,
                                    const MSEdge* to, double toPos,
                                    std::string group) {
    const std::string removedID = MSDispatch::removeReservation(person, from, fromPos, to, toPos, group);
    if (myReservationLookup.hasString(removedID)) {
        // warning! res is already deleted
        const Reservation* res = myReservationLookup.get(removedID);
        myReservationLookup.remove(removedID, res);
    }
    return removedID;
}


void
MSDispatch_TraCI::fulfilledReservation(const Reservation* res) {
    myReservationLookup.remove(res->id, res);
    MSDispatch::fulfilledReservation(res);
}

void
MSDispatch_TraCI::interpretDispatch(MSDevice_Taxi* taxi, const std::vector<std::string>& reservationsIDs) {
    std::vector<const Reservation*> reservations;
    for (std::string resID : reservationsIDs) {
        if (myReservationLookup.hasString(resID)) {
            reservations.push_back(myReservationLookup.get(resID));
        } else {
            throw InvalidArgument("Reservation id '" + resID + "' is not known");
        }
    }
    try {
        if (reservations.size() == 1) {
            taxi->dispatch(*reservations.front());
        } else {
            taxi->dispatchShared(reservations);
        }
    } catch (ProcessError& e) {
        throw InvalidArgument(e.what());
    }
    // in case of ride sharing the same reservation may occur multiple times
    std::set<const Reservation*> unique(reservations.begin(), reservations.end());
    for (const Reservation* res : unique) {
        servedReservation(res);
    }
}


std::string
MSDispatch_TraCI::splitReservation(std::string resID, std::vector<std::string> personIDs) {
    if (myReservationLookup.hasString(resID)) {
        Reservation* res = const_cast<Reservation*>(myReservationLookup.get(resID));
        if (myRunningReservations.count(res) != 0) {
            throw InvalidArgument("Cannot split reservation '" + resID + "' after dispatch");
        }
        std::set<std::string> allPersons;
        for (MSTransportable* t : res->persons) {
            allPersons.insert(t->getID());
        }
        for (std::string p : personIDs) {
            if (allPersons.count(p) == 0) {
                throw InvalidArgument("Person '" + p + "' is not part of reservation '" + resID + "'");
            }
        }
        if (personIDs.size() == allPersons.size()) {
            throw InvalidArgument("Cannot remove all person from reservation '" + resID + "'");
        }
        std::vector<MSTransportable*> split;
        for (const std::string& p : personIDs) {
            for (MSTransportable* const t : res->persons) {
                if (t->getID() == p) {
                    res->persons.erase(t);
                    split.push_back(t);
                    break;
                }
            }
        }
        Reservation* newRes = new Reservation(toString(myReservationCount++), split,
                                              res->reservationTime, res->pickupTime,
                                              res->from, res->fromPos,
                                              res->to, res->toPos, res->group, res->line);
        myGroupReservations[res->group].push_back(newRes);
        myReservationLookup.insert(newRes->id, newRes);
        return newRes->id;
    } else {
        throw InvalidArgument("Reservation id '" + resID + "' is not known");
    }
}



//
/****************************************************************************/
