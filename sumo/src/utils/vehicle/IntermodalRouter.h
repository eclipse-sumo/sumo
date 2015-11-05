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
#include "PedestrianRouter.h"

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
        return trip->vehicle == 0 || this->myEdge->prohibits(trip->vehicle);
    }

    /// @}

    SUMOReal getTravelTime(const IntermodalTrip<E, N, V>* const trip, SUMOReal time) const {
        return E::getTravelTimeStatic(this->myEdge, trip->vehicle, time);
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
    PublicTransportEdge(const std::string id, unsigned int numericalID, const E* startEdge, const E* endEdge, const std::string& line) :
        IntermodalEdge<E, L, N, V>(line + ":" + (id != "" ? id : endEdge->getID()), numericalID, endEdge, line), myStartEdge(startEdge) { }

    bool includeInRoute(bool /* allEdges */) const {
        return true;
    }

    const E* getStartEdge() const {
        return myStartEdge;
    }

    void addSchedule(const SUMOTime begin, const SUMOTime end, const SUMOTime period, const SUMOReal travelTimeSec) {
        mySchedules.insert(std::make_pair(STEPS2TIME(begin), Schedule(begin, end, period, travelTimeSec)));
    }

    SUMOReal getTravelTime(const IntermodalTrip<E, N, V>* const /* trip */, SUMOReal time) const {
        SUMOReal minArrivalSec = std::numeric_limits<SUMOReal>::max();
        for (typename std::multimap<SUMOReal, Schedule>::const_iterator it = mySchedules.begin(); it != mySchedules.end(); ++it) {
            if (it->first > minArrivalSec) {
                break;
            }
            if (time < STEPS2TIME(it->second.end)) {
                const long long int running = MAX2((SUMOTime)0, TIME2STEPS(time) - it->second.begin) / it->second.period;
                const SUMOTime nextDepart = it->second.begin + running * it->second.period;
                minArrivalSec = MIN2(STEPS2TIME(nextDepart) + it->second.travelTimeSec, minArrivalSec);
            }
        }
        return minArrivalSec - time;
    }

private:
    std::multimap<SUMOReal, Schedule> mySchedules;
    const E* const myStartEdge;

};


/// @brief the access edge connecting diferent modes that is given to the internal router (SUMOAbstractRouter)
template<class E, class L, class N, class V>
class AccessEdge : public IntermodalEdge<E, L, N, V> {
private:
    typedef IntermodalEdge<E, L, N, V> _IntermodalEdge;

public:
    AccessEdge(unsigned int numericalID, const _IntermodalEdge* inEdge, const _IntermodalEdge* outEdge, const SUMOReal inScale, const SUMOReal outScale) :
        _IntermodalEdge(inEdge->getID() + ":" + outEdge->getID(), numericalID, outEdge->getEdge(), "!access"),
        myInEdge(inEdge), myOutEdge(outEdge), myInScale(inScale), myOutScale(outScale) { }

    const E* getStartEdge() const {
        return myInEdge->getEdge();
    }

    SUMOReal getTravelTime(const IntermodalTrip<E, N, V>* const trip, SUMOReal time) const {
        return - myInEdge->getTravelTime(trip, time) * myInScale - myOutEdge->getTravelTime(trip, time) * myOutScale;
    }

private:
    const _IntermodalEdge* const myInEdge;
    const _IntermodalEdge* const myOutEdge;
    const SUMOReal myInScale, myOutScale;

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
    typedef AccessEdge<E, L, N, V> _AccessEdge;
    typedef IntermodalNetwork<E, L, N, V> _IntermodalNetwork;
    typedef IntermodalTrip<E, N, V> _IntermodalTrip;

public:
    /// Constructor
    IntermodalRouter() :
        SUMOAbstractRouter<E, _IntermodalTrip>(0, "IntermodalRouter"), myAmClone(false), myInternalRouter(0), myIntermodalNet(0), myNumericalID(0) {
    }

    /// Destructor
    virtual ~IntermodalRouter() {
        delete myInternalRouter;
        if (!myAmClone) {
            delete myIntermodalNet;
        }
    }

    SUMOAbstractRouter<E, _IntermodalTrip>* clone() {
        createNet();
        return new IntermodalRouter<E, L, N, V, INTERNALROUTER>(myIntermodalNet);
    }

    void addSchedule(const SUMOVehicleParameter& pars, const std::vector<const E*>& stopEdges) {
        if (pars.stops.size() < 2) {
            WRITE_WARNING("Ignoring public transport line '" + pars.line + "' with less than two stops.");
            return;
        }
        assert(pars.stops.size() == stopEdges.size());
        typename std::vector<_PTEdge*>& lineEdges = myPTLines[pars.line];
        SUMOTime lastTime = pars.stops.front().until;
        if (lineEdges.empty()) {
            const E* lastStopEdge = stopEdges.front();
            std::string lastStopID = pars.stops.front().busstop;
            typename std::vector<const E*>::const_iterator stopEdge = stopEdges.begin() + 1;
            for (std::vector<SUMOVehicleParameter::Stop>::const_iterator s = pars.stops.begin() + 1; s != pars.stops.end(); ++s, ++stopEdge) {
                _PTEdge* const newEdge = new _PTEdge(s->busstop, myNumericalID++, lastStopEdge, *stopEdge, pars.line);
                newEdge->addSchedule(lastTime, pars.repetitionEnd + lastTime - pars.depart, pars.repetitionOffset, STEPS2TIME(s->until - lastTime));
                if (!lineEdges.empty()) {
                    lineEdges.back()->addSuccessor(newEdge);
                }
                lineEdges.push_back(newEdge);
                lastTime = s->until;
                lastStopEdge = *stopEdge;
            }
            // connecting all "bus"stops with the same name for different lines
            _PTEdge* inEdge = 0;
            typename std::vector<_PTEdge*>::const_iterator outEdge = lineEdges.begin();
            for (std::vector<SUMOVehicleParameter::Stop>::const_iterator s = pars.stops.begin(); s != pars.stops.end(); ++s, ++outEdge) {
                if (s->busstop != "") {
                    typename std::vector<_PTEdge*>& incomingEdges = myPTStopIn[s->busstop];
                    if (outEdge != lineEdges.end()) {
                        for (typename std::vector<_PTEdge*>::iterator edge = incomingEdges.begin(); edge != incomingEdges.end(); ++edge) {
                            (*edge)->addSuccessor(*outEdge);
                        }
                    }
                    typename std::vector<_PTEdge*>& outgoingEdges = myPTStopOut[s->busstop];
                    if (inEdge != 0) {
                        for (typename std::vector<_PTEdge*>::iterator edge = outgoingEdges.begin(); edge != outgoingEdges.end(); ++edge) {
                            inEdge->addSuccessor(*edge);
                        }
                        incomingEdges.push_back(inEdge);
                    }
                    if (outEdge == lineEdges.end()) {
                        break;
                    }
                    outgoingEdges.push_back(*outEdge);
                }
                inEdge = *outEdge;
            }
        } else {
            if (pars.stops.size() != lineEdges.size() + 1) {
                WRITE_WARNING("Number of stops for public transport line '" + pars.line + "' does not match earlier definitions, ignoring schedule.");
                return;
            }
            typename std::vector<_PTEdge*>::const_iterator lineEdge = lineEdges.begin();
            if ((*lineEdge)->getStartEdge() != stopEdges.front()) {
                WRITE_WARNING("Different stop for '" + pars.line + "' compared to earlier definitions, ignoring schedule.");
                return;
            }
            typename std::vector<const E*>::const_iterator stopEdge = stopEdges.begin() + 1;
            for (; stopEdge != stopEdges.end(); ++stopEdge, ++lineEdge) {
                if ((*lineEdge)->getEdge() != *stopEdge) {
                    WRITE_WARNING("Different stop for '" + pars.line + "' compared to earlier definitions, ignoring schedule.");
                    return;
                }
            }
            std::vector<SUMOVehicleParameter::Stop>::const_iterator s = pars.stops.begin() + 1;
            for (lineEdge = lineEdges.begin(); lineEdge != lineEdges.end(); ++lineEdge, ++s) {
                (*lineEdge)->addSchedule(lastTime, pars.repetitionEnd + lastTime - pars.depart, pars.repetitionOffset, STEPS2TIME(s->until - lastTime));
                lastTime = s->until;
            }
        }
    }

    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    bool compute(const E* from, const E* to, SUMOReal departPos, SUMOReal arrivalPos, SUMOReal speed,
                 const V* const vehicle, SUMOTime msTime, std::vector<std::pair<std::string, std::vector<const E*> > >& into) {
        createNet();
        _IntermodalTrip trip(from, to, departPos, arrivalPos, speed, msTime, 0, vehicle);
        std::vector<const _IntermodalEdge*> intoPed;
        const bool success = myInternalRouter->compute(myIntermodalNet->getDepartEdge(from),
                                                       myIntermodalNet->getArrivalEdge(to),
                                                       &trip, msTime, intoPed);
        if (success) {
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
                            if (intoPed[i]->getStartEdge() != 0) {
                                into.back().second.push_back(intoPed[i]->getStartEdge());
                            }
                        }
                    }
                    into.back().second.push_back(intoPed[i]->getEdge());
                }
            }
        }
#ifdef IntermodalRouter_DEBUG_ROUTES
        SUMOReal time = STEPS2TIME(msTime);
        for (size_t i = 0; i < intoPed.size(); ++i) {
            time += myInternalRouter->getEffort(intoPed[i], &trip, time);
        }
        std::cout << TIME2STEPS(msTime) << " trip from " << from->getID() << " to " << to->getID()
                  << " departPos=" << departPos
                  << " arrivalPos=" << arrivalPos
                  << " edges=" << toString(intoPed)
//                  << " resultEdges=" << toString(into)
                  << " time=" << time
                  << "\n";
#endif
        return success;
    }

    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    bool compute(const E*, const E*, const _IntermodalTrip* const,
                 SUMOTime, std::vector<const E*>&) {
        throw ProcessError("Do not use this method");
    }

    SUMOReal recomputeCosts(const std::vector<const E*>&, const _IntermodalTrip* const, SUMOTime) const {
        throw ProcessError("Do not use this method");
    }

    void prohibit(const std::vector<E*>& toProhibit) {
        std::vector<_IntermodalEdge*> toProhibitPE;
        for (typename std::vector<E*>::const_iterator it = toProhibit.begin(); it != toProhibit.end(); ++it) {
            toProhibitPE.push_back(myIntermodalNet->getBothDirections(*it).first);
            toProhibitPE.push_back(myIntermodalNet->getBothDirections(*it).second);
            toProhibitPE.push_back(getCarEdge(*it));
        }
        myInternalRouter->prohibit(toProhibitPE);
    }

private:
    IntermodalRouter(_IntermodalNetwork* net):
        SUMOAbstractRouter<E, _IntermodalTrip>(0, "PedestrianRouter"), myAmClone(true),
        myInternalRouter(new INTERNALROUTER(net->getAllEdges(), true, &_IntermodalEdge::getTravelTimeStatic)),
        myIntermodalNet(net), myNumericalID((int)net->getAllEdges().size()) {}

    void addCarEdges(const std::vector<E*>& edges) {
        for (typename std::vector<E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            const E* const edge = *i;
            if (!edge->isInternal()) {
                myCarLookup[edge] = new CarEdge<E, L, N, V>(myNumericalID++, edge);
                myIntermodalNet->addEdge(myCarLookup[edge]);
            }
        }
        for (typename std::vector<E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            const E* const edge = *i;
            if (!edge->isInternal()) {
                _IntermodalEdge* startConnector = myIntermodalNet->getDepartEdge(edge);
                _IntermodalEdge* endConnector = myIntermodalNet->getArrivalEdge(edge);
                _IntermodalEdge* carEdge = getCarEdge(edge);
                if (getSidewalk<E, L>(edge) != 0) {
                    const std::pair<_IntermodalEdge*, _IntermodalEdge*>& pair = myIntermodalNet->getBothDirections(edge);
                    _IntermodalEdge* forwardAccess = new _AccessEdge(myNumericalID++, carEdge, pair.first, 0, 1);
                    myIntermodalNet->addEdge(forwardAccess);
                    carEdge->addSuccessor(forwardAccess);
                    forwardAccess->addSuccessor(pair.first);

                    _IntermodalEdge* backwardAccess = new _AccessEdge(myNumericalID++, carEdge, pair.second, 1, 1);
                    myIntermodalNet->addEdge(backwardAccess);
                    carEdge->addSuccessor(backwardAccess);
                    backwardAccess->addSuccessor(pair.second);
                }
                const std::vector<E*>& successors = edge->getSuccessors();
                for (typename std::vector<E*>::const_iterator it = successors.begin(); it != successors.end(); ++it) {
                    carEdge->addSuccessor(getCarEdge(*it));
                }
                startConnector->addSuccessor(carEdge);
                carEdge->addSuccessor(endConnector);
            }
        }
    }

    void addPTEdges() {
        for (typename std::map<std::string, std::vector<_PTEdge*> >::const_iterator it = myPTLines.begin(); it != myPTLines.end(); ++it) {
            for (typename std::vector<_PTEdge*>::const_iterator e = it->second.begin(); e != it->second.end(); ++e) {
                _PTEdge* const ptEdge = *e;
                myIntermodalNet->addEdge(ptEdge);
                const E* const startEdge = ptEdge->getStartEdge();
                const E* const endEdge = ptEdge->getEdge();
                if (getSidewalk<E, L>(startEdge) != 0) {
                    const std::pair<_IntermodalEdge*, _IntermodalEdge*>& pair = myIntermodalNet->getBothDirections(startEdge);
                    pair.first->addSuccessor(ptEdge);
                    pair.second->addSuccessor(ptEdge);
                }
                if (getSidewalk<E, L>(endEdge) != 0) {
                    const std::pair<_IntermodalEdge*, _IntermodalEdge*>& pair = myIntermodalNet->getBothDirections(endEdge);
                    ptEdge->addSuccessor(pair.first);
                    ptEdge->addSuccessor(pair.second);
                }
                if (myCarLookup.count(startEdge) > 0) {
                    getCarEdge(startEdge)->addSuccessor(ptEdge);
                }
                _IntermodalEdge* startConnector = myIntermodalNet->getDepartEdge(startEdge);
                _IntermodalEdge* endConnector = myIntermodalNet->getArrivalEdge(endEdge);
                startConnector->addSuccessor(ptEdge);
                ptEdge->addSuccessor(endConnector);
            }
        }
    }

    inline void createNet() {
        if (myIntermodalNet == 0) {
            myIntermodalNet = new _IntermodalNetwork(E::getAllEdges(), myNumericalID);
            myNumericalID = (int)myIntermodalNet->getAllEdges().size();
            addCarEdges(E::getAllEdges());
            addPTEdges();
            myInternalRouter = new INTERNALROUTER(myIntermodalNet->getAllEdges(), true, &_IntermodalEdge::getTravelTimeStatic);
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
    _IntermodalNetwork* myIntermodalNet;
    int myNumericalID;

    /// @brief retrieve the car edge for the given input edge E
    std::map<const E*, _IntermodalEdge*> myCarLookup;

    /// @brief retrieve the public transport edges for the given line
    std::map<std::string, std::vector<_PTEdge*> > myPTLines;

    /// @brief retrieve the public transport edges arriving at the given "bus" stop
    std::map<std::string, std::vector<_PTEdge*> > myPTStopIn;

    /// @brief retrieve the public transport edges leaving the given "bus" stop
    std::map<std::string, std::vector<_PTEdge*> > myPTStopOut;


private:
    /// @brief Invalidated assignment operator
    IntermodalRouter& operator=(const IntermodalRouter& s);

};

// common specializations
template<class E, class L, class N, class V>
class IntermodalRouterDijkstra : public IntermodalRouter < E, L, N, V,
        DijkstraRouterTT<IntermodalEdge<E, L, N, V>, IntermodalTrip<E, N, V>, prohibited_withPermissions<IntermodalEdge<E, L, N, V>, IntermodalTrip<E, N, V> > > > { };


/**
 * @class RouterProvider
 * The encapsulation of the routers for vehicles and pedestrians
 */
template<class E, class L, class N, class V>
class RouterProvider {
public:
    RouterProvider(SUMOAbstractRouter<E, V>* vehRouter,
                   PedestrianRouterDijkstra<E, L, N, V>* pedRouter,
                   IntermodalRouterDijkstra<E, L, N, V>* interRouter)
        : myVehRouter(vehRouter), myPedRouter(pedRouter), myInterRouter(interRouter) {}

    RouterProvider(const RouterProvider& original)
        : myVehRouter(original.getVehicleRouter().clone()),
        myPedRouter(static_cast<PedestrianRouterDijkstra<E, L, N, V>*>(original.getPedestrianRouter().clone())),
        myInterRouter(static_cast<IntermodalRouterDijkstra<E, L, N, V>*>(original.getIntermodalRouter().clone())) {}

    SUMOAbstractRouter<E, V>& getVehicleRouter() const {
        return *myVehRouter;
    }

    PedestrianRouterDijkstra<E, L, N, V>& getPedestrianRouter() const {
        return *myPedRouter;
    }

    IntermodalRouterDijkstra<E, L, N, V>& getIntermodalRouter() const {
        return *myInterRouter;
    }

    virtual ~RouterProvider() {
        delete myVehRouter;
        delete myPedRouter;
        delete myInterRouter;
    }


private:
    SUMOAbstractRouter<E, V>* const myVehRouter;
    PedestrianRouterDijkstra<E, L, N, V>* const myPedRouter;
    IntermodalRouterDijkstra<E, L, N, V>* const myInterRouter;


private:
    /// @brief Invalidated assignment operator
    RouterProvider& operator=(const RouterProvider& src);

};


#endif

/****************************************************************************/
