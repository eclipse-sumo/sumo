/****************************************************************************/
/// @file    CarEdge.h
/// @author  Michael Behrisch
/// @date    Mon, 03 March 2014
/// @version $Id$
///
// The CarEdge is a special intermodal edge representing the SUMO network edge
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef CarEdge_h
#define CarEdge_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_FOX
#include <fx.h>
#endif
#include "IntermodalEdge.h"


// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the car edge type that is given to the internal router (SUMOAbstractRouter)
template<class E, class L, class N, class V>
class CarEdge : public IntermodalEdge<E, L, N, V> {
private:
    typedef IntermodalEdge<E, L, N, V> _IntermodalEdge;

public:
    CarEdge(int numericalID, const E* edge, const SUMOReal pos = -1.) :
        _IntermodalEdge(edge->getID() + "_car" + toString(pos), numericalID, edge, "!car"),
        myStartPos(pos >= 0 ? pos : 0.) { }

    bool includeInRoute(bool /* allEdges */) const {
        return true;
    }

    const std::vector<_IntermodalEdge*>& getSuccessors(SUMOVehicleClass vClass) const {
        if (vClass == SVC_IGNORING /* || !RONet::getInstance()->hasPermissions() */) {
            return this->myFollowingEdges;
        }
#ifdef HAVE_FOX
        FXMutexLock locker(myLock);
#endif
        typename std::map<SUMOVehicleClass, std::vector<_IntermodalEdge*> >::const_iterator i = myClassesSuccessorMap.find(vClass);
        if (i != myClassesSuccessorMap.end()) {
            // can use cached value
            return i->second;
        } else {
            // this vClass is requested for the first time. rebuild all successors
            const std::set<const E*> classedCarFollowers = std::set<const E*>(this->getEdge()->getSuccessors(vClass).begin(), this->getEdge()->getSuccessors(vClass).end());
            for (typename std::vector<_IntermodalEdge*>::const_iterator e = this->myFollowingEdges.begin(); e != this->myFollowingEdges.end(); ++e) {
                if (!(*e)->includeInRoute(true) || (*e)->getEdge() == this->getEdge() || classedCarFollowers.count((*e)->getEdge()) > 0) {
                    myClassesSuccessorMap[vClass].push_back(*e);
                }
            }
            return myClassesSuccessorMap[vClass];
        }

    }


    bool prohibits(const IntermodalTrip<E, N, V>* const trip) const {
        return trip->vehicle == 0 || this->getEdge()->prohibits(trip->vehicle);
    }

    SUMOReal getTravelTime(const IntermodalTrip<E, N, V>* const trip, SUMOReal time) const {
        const SUMOReal travelTime = E::getTravelTimeStatic(this->getEdge(), trip->vehicle, time);
        SUMOReal distTravelled = this->getLength();
        // checking arrivalPos first to have it correct for identical depart and arrival edge
        if (this->getEdge() == trip->to) {
            distTravelled = trip->arrivalPos - myStartPos;
        }
        if (this->getEdge() == trip->from) {
            distTravelled -= trip->departPos - myStartPos;
        }
        return travelTime * distTravelled / this->getEdge()->getLength();
    }

private:
    /// @brief the starting position for split edges
    const SUMOReal myStartPos;

    /// @brief The successors available for a given vClass
    mutable std::map<SUMOVehicleClass, std::vector<_IntermodalEdge*> > myClassesSuccessorMap;

#ifdef HAVE_FOX
    /// The mutex used to avoid concurrent updates of myClassesSuccessorMap
    mutable FXMutex myLock;
#endif
};


/// @brief the stop edge type representing bus and train stops
template<class E, class L, class N, class V>
class StopEdge : public IntermodalEdge<E, L, N, V> {
public:
    StopEdge(const std::string id, int numericalID, const E* edge) :
        IntermodalEdge<E, L, N, V>(id, numericalID, edge, "!stop") { }

    bool includeInRoute(bool /* allEdges */) const {
        return true;
    }

    bool prohibits(const IntermodalTrip<E, N, V>* const trip) const {
        return trip->modeSet && SVC_BUS == 0;
    }
};


/// @brief the public transport edge type connecting the stop edges
template<class E, class L, class N, class V>
class PublicTransportEdge : public IntermodalEdge<E, L, N, V> {
private:
    struct Schedule {
        Schedule(const SUMOTime _begin, const SUMOTime _end, const SUMOTime _period, const SUMOReal _travelTimeSec)
            : begin(_begin), end(_end), period(_period), travelTimeSec(_travelTimeSec) {}
        const SUMOTime begin;
        const SUMOTime end;
        const SUMOTime period;
        const SUMOReal travelTimeSec;
    private:
        /// @brief Invalidated assignment operator
        Schedule& operator=(const Schedule& src);
    };

public:
    PublicTransportEdge(const std::string id, int numericalID, const IntermodalEdge<E, L, N, V>* entryStop, const E* endEdge, const std::string& line) :
        IntermodalEdge<E, L, N, V>(line + ":" + (id != "" ? id : endEdge->getID()), numericalID, endEdge, line), myEntryStop(entryStop) { }

    bool includeInRoute(bool /* allEdges */) const {
        return true;
    }

    const IntermodalEdge<E, L, N, V>* getEntryStop() const {
        return myEntryStop;
    }

    void addSchedule(const SUMOTime begin, const SUMOTime end, const SUMOTime period, const SUMOReal travelTimeSec) {
        //std::cout << " edge=" << myEntryStop->getID() << "->" << this->getID() << " beg=" << STEPS2TIME(begin) << " end=" << STEPS2TIME(end) 
        //    << " period=" << STEPS2TIME(period) 
        //    << " travelTime=" << travelTimeSec << "\n";
        mySchedules.insert(std::make_pair(STEPS2TIME(begin), Schedule(begin, end, period, travelTimeSec)));
    }

    SUMOReal getTravelTime(const IntermodalTrip<E, N, V>* const /* trip */, SUMOReal time) const {
        SUMOReal minArrivalSec = std::numeric_limits<SUMOReal>::max();
        for (typename std::multimap<SUMOReal, Schedule>::const_iterator it = mySchedules.begin(); it != mySchedules.end(); ++it) {
            if (it->first > minArrivalSec) {
                break;
            }
            if (time >= STEPS2TIME(it->second.begin) && time < STEPS2TIME(it->second.end)) {
                const int running = MAX2(0, (int)ceil((time - STEPS2TIME(it->second.begin)) / STEPS2TIME(it->second.period)));
                const SUMOTime nextDepart = it->second.begin + running * it->second.period;
                minArrivalSec = MIN2(STEPS2TIME(nextDepart) + it->second.travelTimeSec, minArrivalSec);
                //std::cout << " edge=" << myEntryStop->getID() << "->" << this->getID() << " beg=" << STEPS2TIME(it->second.begin) << " end=" << STEPS2TIME(it->second.end) 
                //    << " atTime=" << time 
                //    << " running=" << running << " nextDepart=" << nextDepart
                //    << " minASec=" << minArrivalSec << " travelTime=" << minArrivalSec - time << "\n";
            }
        }
        return minArrivalSec - time;
    }

private:
    std::multimap<SUMOReal, Schedule> mySchedules;
    const IntermodalEdge<E, L, N, V>* const myEntryStop;

};


/// @brief the access edge connecting different modes that is given to the internal router (SUMOAbstractRouter)
template<class E, class L, class N, class V>
class AccessEdge : public IntermodalEdge<E, L, N, V> {
private:
    typedef IntermodalEdge<E, L, N, V> _IntermodalEdge;

public:
    AccessEdge(int numericalID, const _IntermodalEdge* inEdge, const _IntermodalEdge* outEdge,
               const SUMOReal transferTime = NUMERICAL_EPS) :
        _IntermodalEdge(inEdge->getID() + ":" + outEdge->getID(), numericalID, outEdge->getEdge(), "!access"),
        myTransferTime(transferTime) { }

    SUMOReal getTravelTime(const IntermodalTrip<E, N, V>* const /* trip */, SUMOReal /* time */) const {
        return myTransferTime;
    }

private:
    const SUMOReal myTransferTime;

};


#endif

/****************************************************************************/
