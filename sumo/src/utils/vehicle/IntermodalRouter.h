/****************************************************************************/
/// @file    IntermodalRouter.h
/// @author  Jakob Erdmann
/// @date    Mon, 03 March 2014
/// @version $Id$
///
// The Pedestrian Router build a special network and (delegegates to a SUMOAbstractRouter)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef IntermodalRouter_h
#define IntermodalRouter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include "SUMOAbstractRouter.h"
#include "SUMOVehicleParameter.h"
#include "DijkstraRouterTT.h"
#include "IntermodalNetwork.h"

//#define IntermodalRouter_DEBUG_ROUTES


// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the car edge type that is given to the internal router (SUMOAbstractRouter)
template<class E, class L, class N, class V>
class CarEdge : public IntermodalEdge<E, L, N, V> {
public:
    CarEdge(unsigned int numericalID, const E* edge) :
        IntermodalEdge<E, L, N, V>(edge->getID() + "_car", numericalID, edge, "!car") { }

    bool includeInRoute(bool /* allEdges */) const {
        return true;
    }

    /// @name The interface as required by SUMOAbstractRouter routes
    /// @{

    bool prohibits(const IntermodalTrip<E, N, V>* const trip) const {
        return myEdge->prohibits(trip->vehicle);
    }

    /// @}

    SUMOReal getTravelTime(const IntermodalTrip<E, N, V>* const trip, SUMOReal time) const {
        return E::getTravelTimeStatic(myEdge, trip->vehicle, time);
    }

};


/// @brief the car edge type that is given to the internal router (SUMOAbstractRouter)
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
    PublicTransportEdge(const std::string id, unsigned int numericalID, const std::string& line) :
        IntermodalEdge<E, L, N, V>(id, numericalID, 0, line) { }

    bool includeInRoute(bool /* allEdges */) const {
        return true;
    }

    void addSchedule(const SUMOTime begin, const SUMOTime end, const SUMOTime period, const SUMOReal travelTimeSec) {
        mySchedules.insert(std::make_pair(-STEPS2TIME(end), Schedule(begin, end, period, travelTimeSec)));
    }

    SUMOReal getTravelTime(const IntermodalTrip<E, N, V>* const /* trip */, SUMOReal time) const {
        SUMOReal minArrivalSec = std::numeric_limits<SUMOReal>::max();
        for (std::multimap<SUMOReal, Schedule>::const_iterator it = mySchedules.begin(); it != mySchedules.end(); ++it) {
            if (-it->first <= time) {
                break;
            }
            const long long int running = MAX2((SUMOTime)0, TIME2STEPS(time) - it->second.begin) / it->second.period;
            const SUMOTime nextDepart = it->second.begin + running * it->second.period;
            minArrivalSec = MIN2(STEPS2TIME(nextDepart) + it->second.travelTimeSec, minArrivalSec);
        }
        return minArrivalSec - time;
    }

private:
    std::multimap<SUMOReal, Schedule> mySchedules;
};


/**
 * @class IntermodalRouter
 * The router for pedestrians (on a bidirectional network of sidewalks and crossings)
 */
template<class E, class L, class N, class V, class INTERNALROUTER>
class IntermodalRouter : public SUMOAbstractRouter<E, IntermodalTrip<E, N, V> > {
private:

    typedef IntermodalEdge<E, L, N, V> _IntermodalEdge;
    typedef PublicTransportEdge<E, L, N, V> _PTEdge;
    typedef IntermodalNetwork<E, L, N, V> _IntermodalNetwork;
    typedef IntermodalTrip<E, N, V> _IntermodalTrip;

public:
    /// Constructor
    IntermodalRouter():
        SUMOAbstractRouter<E, _IntermodalTrip>(0, "IntermodalRouter"), myAmClone(false) {
        myPedNet = new _IntermodalNetwork(E::getAllEdges());
        myNumericalID = (int)myPedNet->getAllEdges().size();
        addCarEdges(E::getAllEdges());
        myInternalRouter = new INTERNALROUTER(myPedNet->getAllEdges(), true, &_IntermodalEdge::getTravelTimeStatic);
    }

    IntermodalRouter(_IntermodalNetwork* net):
        SUMOAbstractRouter<E, _IntermodalTrip>(0, "PedestrianRouter"), myAmClone(true) {
        myPedNet = net;
        myInternalRouter = new INTERNALROUTER(myPedNet->getAllEdges(), true, &_IntermodalEdge::getTravelTimeStatic);
    }

    /// Destructor
    virtual ~IntermodalRouter() {
        delete myInternalRouter;
        if (!myAmClone) {
            delete myPedNet;
        }
    }

    virtual SUMOAbstractRouter<E, _IntermodalTrip>* clone() const {
        return new IntermodalRouter<E, L, N, V, INTERNALROUTER>(myPedNet);
    }

    void addSchedule(const SUMOVehicleParameter& pars) {
        if (myPTLines.count(pars.line) == 0) {
            std::vector<_PTEdge*>& lineEdges = myPTLines[pars.line];
            std::string lastStopID = pars.stops.front().busstop;
            SUMOTime lastTime = pars.stops.front().until;
            for (std::vector<SUMOVehicleParameter::Stop>::const_iterator s = pars.stops.begin() + 1; s != pars.stops.end(); ++s) {
                lineEdges.push_back(new _PTEdge(pars.line + ":" + lastStopID + "_to_" + s->busstop, myNumericalID++, pars.line));
                lineEdges.back()->addSchedule(pars.depart, pars.repetitionEnd, pars.repetitionOffset, STEPS2TIME(s->until - lastTime));
                lastStopID = s->busstop;
                lastTime = s->until;
            }
        }
    }

    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    void compute(const E* from, const E* to, SUMOReal departPos, SUMOReal arrivalPos, SUMOReal speed,
                 const V* const vehicle, SUMOTime msTime, std::vector<std::pair<std::string, std::vector<const E*> > >& into) {
        //startQuery();
        _IntermodalTrip trip(from, to, departPos, arrivalPos, speed, msTime, 0, vehicle);
        std::vector<const _IntermodalEdge*> intoPed;
        myInternalRouter->compute(myPedNet->getDepartEdge(from),
                                  myPedNet->getArrivalEdge(to), &trip, msTime, intoPed);
        if (!intoPed.empty()) {
            std::string lastLine = "";
            for (size_t i = 0; i < intoPed.size(); ++i) {
                if (intoPed[i]->includeInRoute(false)) {
                    if (intoPed[i]->getLine() != lastLine) {
                        lastLine = intoPed[i]->getLine();
                        if (lastLine == "!car") {
                            into.push_back(std::make_pair(vehicle->getID(), std::vector<const E*>()));
                        } else if (lastLine == "!ped") {
                            into.push_back(std::make_pair("", std::vector<const E*>()));
                        } else {
                            into.push_back(std::make_pair(lastLine, std::vector<const E*>()));
                        }
                    }
                    into.back().second.push_back(intoPed[i]->getEdge());
                }
            }
        }
#ifdef IntermodalRouter_DEBUG_ROUTES
        SUMOReal time = msTime;
        for (size_t i = 0; i < intoPed.size(); ++i) {
            time += myInternalRouter->getEffort(intoPed[i], &trip, time);
        }
        std::cout << TIME2STEPS(msTime) << " trip from " << from->getID() << " to " << to->getID()
                  << " departPos=" << departPos
                  << " arrivalPos=" << arrivalPos
                  << " onlyNode=" << (onlyNode == 0 ? "NULL" : onlyNode->getID())
                  << " edges=" << toString(intoPed)
                  << " resultEdges=" << toString(into)
                  << " time=" << time
                  << "\n";
#endif
        //endQuery();
    }

    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    void compute(const E*, const E*, const _IntermodalTrip* const,
                 SUMOTime, std::vector<const E*>&) {
        throw ProcessError("Do not use this method");
    }

    SUMOReal recomputeCosts(const std::vector<const E*>&, const _IntermodalTrip* const, SUMOTime) const {
        throw ProcessError("Do not use this method");
    }

    void prohibit(const std::vector<E*>& toProhibit) {
        std::vector<_IntermodalEdge*> toProhibitPE;
        for (typename std::vector<E*>::const_iterator it = toProhibit.begin(); it != toProhibit.end(); ++it) {
            toProhibitPE.push_back(myPedNet->getBothDirections(*it).first);
            toProhibitPE.push_back(myPedNet->getBothDirections(*it).second);
            toProhibitPE.push_back(getCarEdge(*it));
        }
        myInternalRouter->prohibit(toProhibitPE);
    }

private:
    void addCarEdges(const std::vector<E*>& edges) {
        for (typename std::vector<E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            const E* const edge = *i;
            if (!edge->isInternal()) {
                myCarLookup[edge] = new CarEdge<E, L, N, V>(myNumericalID++, edge);
                myPedNet->addEdge(myCarLookup[edge]);
            }
        }
        for (typename std::vector<E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            const E* const edge = *i;
            if (!edge->isInternal()) {
                _IntermodalEdge* startConnector = myPedNet->getDepartEdge(edge);
                _IntermodalEdge* endConnector = myPedNet->getArrivalEdge(edge);
                _IntermodalEdge* carEdge = getCarEdge(edge);
                if (getSidewalk<E, L>(edge) != 0) {
                    const std::pair<_IntermodalEdge*, _IntermodalEdge*>& pair = myPedNet->getBothDirections(edge);
                    carEdge->addSuccessor(pair.first);
                    carEdge->addSuccessor(pair.second);
                }
                const std::vector<E*>& successors = edge->getSuccessors();
                for (std::vector<E*>::const_iterator it = successors.begin(); it != successors.end(); ++it) {
                    carEdge->addSuccessor(getCarEdge(*it));
                }
                startConnector->addSuccessor(carEdge);
                carEdge->addSuccessor(endConnector);
            }
        }
    }

    /// @brief Returns the associated car edge
    _IntermodalEdge* getCarEdge(const E* e) {
        typename std::map<const E*, _IntermodalEdge*>::const_iterator it = myCarLookup.find(e);
        if (it == myCarLookup.end()) {
            throw ProcessError("Car edge '" + e->getID() + "' not found in pedestrian network.");
        }
        return it->second;
    }

private:
    const bool myAmClone;
    INTERNALROUTER* myInternalRouter;
    _IntermodalNetwork* myPedNet;
    int myNumericalID;

    /// @brief retrieve the car edge for the given input edge E
    std::map<const E*, _IntermodalEdge*> myCarLookup;

    /// @brief retrieve the public transport edge for the given input edge E
    std::map<const E*, _PTEdge*> myPTLookup;

    /// @brief retrieve the public transport edges for the given line
    std::map<std::string, std::vector<_PTEdge*> > myPTLines;


private:
    /// @brief Invalidated assignment operator
    IntermodalRouter& operator=(const IntermodalRouter& s);

};

// common specializations
template<class E, class L, class N, class V>
class IntermodalRouterDijkstra : public IntermodalRouter < E, L, N, V,
        DijkstraRouterTT<IntermodalEdge<E, L, N, V>, IntermodalTrip<E, N, V>, prohibited_withPermissions<IntermodalEdge<E, L, N, V>, IntermodalTrip<E, N, V> > > > { };


#endif

/****************************************************************************/
