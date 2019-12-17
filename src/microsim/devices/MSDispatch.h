/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDispatch.h
/// @author  Jakob Erdmann
/// @date    16.12.2019
///
// An algorithm that performs dispatch for the taxi device
/****************************************************************************/
#ifndef MSDispatch_h
#define MSDispatch_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <set>
#include <vector>
#include <map>
#include <utils/common/SUMOTime.h>
#include "MSDevice_Taxi.h"

#ifdef HAVE_FOX
#include <utils/foxtools/FXWorkerThread.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
struct Reservation {
    Reservation(MSTransportable* _person,
            SUMOTime _reservationTime,
            SUMOTime _pickupTime,
            const MSEdge* _from, double _fromPos,
            const MSEdge* _to, double _toPos) :
        person(_person),
        reservationTime(_reservationTime),
        pickupTime(_pickupTime),
        from(_from),
        fromPos(_fromPos),
        to(_to),
        toPos(_toPos),
        recheck(_reservationTime)
    {}

    MSTransportable* person;
    SUMOTime reservationTime;
    SUMOTime pickupTime;
    const MSEdge* from;
    double fromPos;
    const MSEdge* to;
    double toPos;
    SUMOTime recheck;
};

/**
 * @class MSDispatch
 * @brief An algorithm that performs distpach for a taxi fleet
 */
class MSDispatch {
public:

    /// @brief sorts reservations by time
    class time_sorter {
    public:
        /// @brief Constructor
        explicit time_sorter() {}

        /// @brief Comparing operator
        int operator()(const Reservation& r1, const Reservation& r2) const {
            return r1.reservationTime < r2.reservationTime;
        }
    };

    /// @brief Destructor
    virtual ~MSDispatch() { }

    /// @brief add a new reservation
    void addReservation(MSTransportable* person,
            SUMOTime reservationTime, 
            SUMOTime pickupTime,
            const MSEdge* from, double fromPos,
            const MSEdge* to, double toPos);

    /// @brief computes dispatch and updates reservations
    virtual void computeDispatch(SUMOTime now, const std::vector<MSDevice_Taxi*>& fleet) = 0;

    /// @brief check whether there are still (servable) reservations in the system
    bool hasServableReservations() {
        return myHasServableReservations;
    }

protected:
    std::vector<Reservation> myReservations;

    /// @brief whether the last call to computeDispatch has left servable reservations
    bool myHasServableReservations = false;

};


class MSDispatch_Greedy : public MSDispatch {
public:
    MSDispatch_Greedy(int routingMode = 1, SUMOTime maximumWaitingTime = STEPS2TIME(300)) : 
        myRoutingMode(routingMode),
        myMaximumWaitingTime(maximumWaitingTime)
    {}

    void computeDispatch(SUMOTime now, const std::vector<MSDevice_Taxi*>& fleet);

private:

    /// @brief which router/edge weights to use
    int myRoutingMode;

    /// @brief maximum time to arrive earlier at customer
    const SUMOTime myMaximumWaitingTime;

    /// @brief recheck interval for early reservations
    const SUMOTime myRecheckTime = TIME2STEPS(120);
    const SUMOTime myRecheckSafety = TIME2STEPS(3600);
};


#endif

/****************************************************************************/

