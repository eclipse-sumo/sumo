/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2020 German Aerospace Center (DLR) and others.
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
/// @file    MSDispatch.h
/// @author  Jakob Erdmann
/// @date    16.12.2019
///
// An algorithm that performs dispatch for the taxi device
/****************************************************************************/
#pragma once
#include <config.h>

#include <set>
#include <vector>
#include <map>
#include <utils/common/Parameterised.h>
#include <utils/common/SUMOTime.h>
#include "MSDevice_Taxi.h"


// ===========================================================================
// class definitions
// ===========================================================================
struct Reservation {
    Reservation(const std::vector<MSTransportable*>& _persons,
                SUMOTime _reservationTime,
                SUMOTime _pickupTime,
                const MSEdge* _from, double _fromPos,
                const MSEdge* _to, double _toPos,
                const std::string& _group) :
        persons(_persons.begin(), _persons.end()),
        reservationTime(_reservationTime),
        pickupTime(_pickupTime),
        from(_from),
        fromPos(_fromPos),
        to(_to),
        toPos(_toPos),
        group(_group),
        recheck(_reservationTime)
    {}

    std::set<MSTransportable*> persons;
    SUMOTime reservationTime;
    SUMOTime pickupTime;
    const MSEdge* from;
    double fromPos;
    const MSEdge* to;
    double toPos;
    std::string group;
    SUMOTime recheck;

    bool operator==(const Reservation& other) const {
        return persons == other.persons
               && reservationTime == other.reservationTime
               && pickupTime == other.pickupTime
               && from == other.from
               && fromPos == other.fromPos
               && to == other.to
               && toPos == other.toPos
               && group == other.group;
    }

    /// @brief debug identification
    std::string getID() const {
        return toString(persons);
    }
};

/**
 * @class MSDispatch
 * @brief An algorithm that performs distpach for a taxi fleet
 */
class MSDispatch : public Parameterised {
public:

    /// @brief sorts reservations by time
    class time_sorter {
    public:
        /// @brief Constructor
        explicit time_sorter() {}

        /// @brief Comparing operator
        int operator()(const Reservation* r1, const Reservation* r2) const {
            return r1->reservationTime < r2->reservationTime;
        }
    };

    /// @brief Constructor;
    MSDispatch(const std::map<std::string, std::string>& params);

    /// @brief Destructor
    virtual ~MSDispatch() { }

    /// @brief add a new reservation
    void addReservation(MSTransportable* person,
                        SUMOTime reservationTime,
                        SUMOTime pickupTime,
                        const MSEdge* from, double fromPos,
                        const MSEdge* to, double toPos,
                        const std::string& group);

    /// @brief computes dispatch and updates reservations
    virtual void computeDispatch(SUMOTime now, const std::vector<MSDevice_Taxi*>& fleet) = 0;


    /// @brief check whether there are still (servable) reservations in the system
    bool hasServableReservations() {
        return myHasServableReservations;
    }

    ///@brief compute time to pick up the given reservation
    static SUMOTime computePickupTime(SUMOTime t, const MSDevice_Taxi* taxi, const Reservation& res, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router);

    ///@brief compute directTime and detourTime
    static double computeDetourTime(SUMOTime t, SUMOTime viaTime, const MSDevice_Taxi* taxi,
                                    const MSEdge* from, double fromPos,
                                    const MSEdge* via, double viaPos,
                                    const MSEdge* to, double toPos,
                                    SUMOAbstractRouter<MSEdge, SUMOVehicle>& router,
                                    double& timeDirect) ;


    /// @brief whether the last call to computeDispatch has left servable reservations
    bool myHasServableReservations = false;

protected:
    std::vector<Reservation*> getReservations();

    void servedReservation(const Reservation* res);

    /// @brief optional file output for dispatch information
    OutputDevice* myOutput;

private:
    std::map<std::string, std::vector<Reservation*> > myGroupReservations;

};


/**
 * @class MSDispatch_Greedy
 * @brief A dispatch algorithm that services customers in reservation order and always sends the closest available taxi
 */
class MSDispatch_Greedy : public MSDispatch {
public:
    MSDispatch_Greedy(const std::map<std::string, std::string>& params) :
        MSDispatch(params),
        myRoutingMode(StringUtils::toInt(getParameter("routingMode", "1"))),
        myMaximumWaitingTime(TIME2STEPS(StringUtils::toInt(getParameter("maxWaitingTime", "300")))),
        myRecheckTime(TIME2STEPS(StringUtils::toInt(getParameter("recheckTime", "120")))),
        myRecheckSafety(TIME2STEPS(StringUtils::toInt(getParameter("recheckSafety", "3600"))))
    { }

    virtual void computeDispatch(SUMOTime now, const std::vector<MSDevice_Taxi*>& fleet);

protected:
    /// @brief trigger taxi dispatch. @note: method exists so subclasses can inject code at this point (ride sharing)
    virtual int dispatch(MSDevice_Taxi* taxi, Reservation* res, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router, std::vector<Reservation*>& reservations);

    /// @brief which router/edge weights to use
    const int myRoutingMode;

    /// @brief maximum time to arrive earlier at customer
    const SUMOTime myMaximumWaitingTime;

    /// @brief recheck interval for early reservations
    const SUMOTime myRecheckTime;
    const SUMOTime myRecheckSafety;

private:
    /// @brief Invalidated assignment operator.
    MSDispatch_Greedy& operator=(const MSDispatch_Greedy&) = delete;

};


/**
 * @class MSDispatch_GreedyClosest
 * @brief A dispatch algorithm that services the reservations with the shortest traveltime-to-pickup first
 */
class MSDispatch_GreedyClosest : public MSDispatch_Greedy {
public:
    MSDispatch_GreedyClosest(const std::map<std::string, std::string>& params) :
        MSDispatch_Greedy(params)
    {}

    void computeDispatch(SUMOTime now, const std::vector<MSDevice_Taxi*>& fleet);

};
