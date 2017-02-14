/****************************************************************************/
/// @file    IntermodalRouter.h
/// @author  Jakob Erdmann
/// @date    Mon, 03 March 2014
/// @version $Id$
///
// The Pedestrian Router build a special network and (delegegates to a SUMOAbstractRouter)
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
#include "CarEdge.h"
#include "PedestrianRouter.h"

//#define IntermodalRouter_DEBUG_ROUTES


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class IntermodalRouter
 * The router for pedestrians (on a bidirectional network of sidewalks and crossings)
 */
template<class E, class L, class N, class V, class INTERNALROUTER = DijkstraRouterTT<IntermodalEdge<E, L, N, V>, IntermodalTrip<E, N, V>, prohibited_withPermissions<IntermodalEdge<E, L, N, V>, IntermodalTrip<E, N, V> > > >
class IntermodalRouter : public SUMOAbstractRouter<E, IntermodalTrip<E, N, V> > {
private:
    typedef void(* CreateNetCallback)(IntermodalRouter<E, L, N, V, INTERNALROUTER>&);
    typedef IntermodalEdge<E, L, N, V> _IntermodalEdge;
    typedef PublicTransportEdge<E, L, N, V> _PTEdge;
    typedef AccessEdge<E, L, N, V> _AccessEdge;
    typedef IntermodalNetwork<E, L, N, V> _IntermodalNetwork;
    typedef IntermodalTrip<E, N, V> _IntermodalTrip;

public:
    struct TripItem {
        TripItem(const std::string& _line = "") : line(_line) {}
        std::string line;
        std::string destStop;
        std::vector<const E*> edges;
    };

    /// Constructor
    IntermodalRouter(CreateNetCallback callback) :
        SUMOAbstractRouter<E, _IntermodalTrip>(0, "IntermodalRouter"), myAmClone(false), myInternalRouter(0), myIntermodalNet(0), myNumericalID(0), myCallback(callback) {
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

    void addAccess(const std::string& stopId, const E* stopEdge, const SUMOReal pos) {
        assert(stopEdge != 0);
        if (myStopConnections.count(stopId) == 0) {
            myStopConnections[stopId] = new StopEdge<E, L, N, V>(stopId, myNumericalID++, stopEdge);
            myIntermodalNet->addEdge(myStopConnections[stopId]);
        }
        _IntermodalEdge* const stopConn = myStopConnections[stopId];
        const L* lane = getSidewalk<E, L>(stopEdge);
        if (lane != 0) {
            const std::pair<_IntermodalEdge*, _IntermodalEdge*>& pair = myIntermodalNet->getBothDirections(stopEdge);
            _IntermodalEdge* const fwdSplit = new PedestrianEdge<E, L, N, V>(myNumericalID++, stopEdge, lane, true, pos);
            const int splitIndex = splitEdge(pair.first, fwdSplit, pos, stopConn);
            _IntermodalEdge* const backSplit = new PedestrianEdge<E, L, N, V>(myNumericalID++, stopEdge, lane, false, pos);
            splitEdge(pair.second, backSplit, stopEdge->getLength() - pos, stopConn);
            if (splitIndex >= 0) {
                _IntermodalEdge* carSplit = 0;
                if (myCarLookup.count(stopEdge) > 0) {
                    carSplit = new CarEdge<E, L, N, V>(myNumericalID++, stopEdge, pos);
                    splitEdge(myCarLookup[stopEdge], carSplit, pos, fwdSplit, backSplit);
                }

                _IntermodalEdge* const prevDep = myIntermodalNet->getDepartEdge(stopEdge, pos);
                _IntermodalEdge* const backBeforeSplit = myAccessSplits[pair.second][myAccessSplits[pair.second].size() - 1 - splitIndex];
                // depart and arrival edges (the router can decide the initial direction to take and the direction to arrive from)
                _IntermodalEdge* const depConn = new _IntermodalEdge(stopEdge->getID() + "_depart_connector" + toString(pos), myNumericalID++, stopEdge, "!connector");
                depConn->addSuccessor(fwdSplit);
                depConn->addSuccessor(backBeforeSplit);
                depConn->setLength(fwdSplit->getLength());
                prevDep->removeSuccessor(backBeforeSplit);
                prevDep->addSuccessor(backSplit);
                prevDep->setLength(backSplit->getLength());
                if (carSplit != 0) {
                    depConn->addSuccessor(carSplit);
                }

                _IntermodalEdge* const arrConn = new _IntermodalEdge(stopEdge->getID() + "_arrival_connector" + toString(pos), myNumericalID++, stopEdge, "!connector");
                fwdSplit->addSuccessor(arrConn);
                backBeforeSplit->addSuccessor(arrConn);
                if (carSplit != 0) {
                    carSplit->addSuccessor(arrConn);
                }
                myIntermodalNet->addConnectors(depConn, arrConn, splitIndex);
            }
        }
    }

    void addSchedule(const SUMOVehicleParameter& pars, const std::vector<SUMOVehicleParameter::Stop>* addStops = 0) {
        SUMOTime lastUntil = 0;
        std::vector<SUMOVehicleParameter::Stop> validStops;
        if (addStops != 0) {
            // stops are part of a stand-alone route. until times are offsets from vehicle departure
            for (std::vector<SUMOVehicleParameter::Stop>::const_iterator s = addStops->begin(); s != addStops->end(); ++s) {
                if (myStopConnections.count(s->busstop) > 0) {
                    // compute stop times for the first vehicle
                    SUMOVehicleParameter::Stop stop = *s;
                    stop.until += pars.depart;
                    if (stop.until >= lastUntil) {
                        validStops.push_back(stop);
                        lastUntil = stop.until;
                    } else {
                        WRITE_WARNING("Ignoring unordered stop at '" + stop.busstop + "' at " + time2string(stop.until) + "  for vehicle '" + pars.id + "'.");
                    }
                }
            }
        }
        for (std::vector<SUMOVehicleParameter::Stop>::const_iterator s = pars.stops.begin(); s != pars.stops.end(); ++s) {
            // stops are part of the vehicle until times are absolute times for the first vehicle
            if (myStopConnections.count(s->busstop) > 0 && s->until >= lastUntil) {
                validStops.push_back(*s);
                lastUntil = s->until;
            }
        }
        if (validStops.size() < 2) {
            WRITE_WARNING("Ignoring public transport line '" + pars.line + "' with less than two usable stops.");
            return;
        }

        typename std::vector<_PTEdge*>& lineEdges = myPTLines[pars.line];
        if (lineEdges.empty()) {
            _IntermodalEdge* lastStop = 0;
            SUMOTime lastTime = 0;
            for (std::vector<SUMOVehicleParameter::Stop>::const_iterator s = validStops.begin(); s != validStops.end(); ++s) {
                _IntermodalEdge* currStop = myStopConnections[s->busstop];
                if (lastStop != 0) {
                    _PTEdge* const newEdge = new _PTEdge(s->busstop, myNumericalID++, lastStop, currStop->getEdge(), pars.line);
                    myIntermodalNet->addEdge(newEdge);
                    newEdge->addSchedule(lastTime, lastTime + pars.repetitionOffset * (pars.repetitionNumber - 1), pars.repetitionOffset, STEPS2TIME(s->until - lastTime));
                    lastStop->addSuccessor(newEdge);
                    newEdge->addSuccessor(currStop);
                    lineEdges.push_back(newEdge);
                }
                lastTime = s->until;
                lastStop = currStop;
            }
        } else {
            if (validStops.size() != lineEdges.size() + 1) {
                WRITE_WARNING("Number of stops for public transport line '" + pars.line + "' does not match earlier definitions, ignoring schedule.");
                return;
            }
            if (lineEdges.front()->getEntryStop() != myStopConnections[validStops.front().busstop]) {
                WRITE_WARNING("Different stop for '" + pars.line + "' compared to earlier definitions, ignoring schedule.");
                return;
            }
            typename std::vector<_PTEdge*>::const_iterator lineEdge = lineEdges.begin();
            typename std::vector<SUMOVehicleParameter::Stop>::const_iterator s = validStops.begin() + 1;
            for (; s != validStops.end(); ++s, ++lineEdge) {
                if ((*lineEdge)->getSuccessors(SVC_IGNORING)[0] != myStopConnections[s->busstop]) {
                    WRITE_WARNING("Different stop for '" + pars.line + "' compared to earlier definitions, ignoring schedule.");
                    return;
                }
            }
            SUMOTime lastTime = validStops.front().until;
            for (lineEdge = lineEdges.begin(), s = validStops.begin() + 1; lineEdge != lineEdges.end(); ++lineEdge, ++s) {
                (*lineEdge)->addSchedule(lastTime, lastTime + pars.repetitionOffset * (pars.repetitionNumber - 1), pars.repetitionOffset, STEPS2TIME(s->until - lastTime));
                lastTime = s->until;
            }
        }
    }

    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    bool compute(const E* from, const E* to, SUMOReal departPos, SUMOReal arrivalPos, SUMOReal speed,
                 const V* const vehicle, const SVCPermissions modeSet, SUMOTime msTime,
                 std::vector<TripItem>& into) {
        createNet();
        _IntermodalTrip trip(from, to, departPos, arrivalPos, speed, msTime, 0, vehicle, modeSet);
        std::vector<const _IntermodalEdge*> intoPed;
        const bool success = myInternalRouter->compute(myIntermodalNet->getDepartEdge(from, departPos),
                             myIntermodalNet->getArrivalEdge(to, arrivalPos),
                             &trip, msTime, intoPed);
        if (success) {
            std::string lastLine = "";
            for (int i = 0; i < (int)intoPed.size(); ++i) {
                if (intoPed[i]->includeInRoute(false)) {
                    if (intoPed[i]->getLine() == "!stop") {
                        into.back().destStop = intoPed[i]->getID();
                    } else {
                        if (intoPed[i]->getLine() != lastLine) {
                            lastLine = intoPed[i]->getLine();
                            if (lastLine == "!car") {
                                into.push_back(TripItem(vehicle->getID()));
                            } else if (lastLine == "!ped") {
                                into.push_back(TripItem());
                            } else {
                                into.push_back(TripItem(lastLine));
                            }
                        }
                        if (into.back().edges.empty() || into.back().edges.back() != intoPed[i]->getEdge()) {
                            into.back().edges.push_back(intoPed[i]->getEdge());
                        }
                    }
                }
            }
        }
#ifdef IntermodalRouter_DEBUG_ROUTES
        SUMOReal time = STEPS2TIME(msTime);
        for (int i = 0; i < intoPed.size(); ++i) {
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

    int splitEdge(_IntermodalEdge* const toSplit, _IntermodalEdge* afterSplit, const SUMOReal pos,
                  _IntermodalEdge* const fwdConn, _IntermodalEdge* const backConn = 0) {
        int splitIndex = 1;
        std::vector<_IntermodalEdge*>& splitList = myAccessSplits[toSplit];
        if (splitList.empty()) {
            splitList.push_back(toSplit);
        }
        typename std::vector<_IntermodalEdge*>::iterator splitIt = splitList.begin();
        SUMOReal relPos = pos;
        while (splitIt != splitList.end() && relPos > (*splitIt)->getLength() + POSITION_EPS) {
            relPos -= (*splitIt)->getLength();
            ++splitIt;
            splitIndex++;
        }
        assert(splitIt != splitList.end());
        _IntermodalEdge* const beforeSplit = *splitIt;
        if (fabs(relPos - beforeSplit->getLength()) < POSITION_EPS && splitIt + 1 != splitList.end()) {
            // don't split, use the present split edges
            splitIndex = -1;
            afterSplit = *(splitIt + 1);
        } else {
            myIntermodalNet->addEdge(afterSplit);
            afterSplit->setSuccessors(beforeSplit->getSuccessors(SVC_IGNORING));
            beforeSplit->clearSuccessors();
            beforeSplit->addSuccessor(afterSplit);
            afterSplit->setLength(beforeSplit->getLength() - relPos);
            beforeSplit->setLength(relPos);
            splitList.insert(splitIt + 1, afterSplit);
        }
        // add access to / from edge
        _AccessEdge* access = new _AccessEdge(myNumericalID++, beforeSplit, fwdConn);
        myIntermodalNet->addEdge(access);
        beforeSplit->addSuccessor(access);
        access->addSuccessor(fwdConn);
        if (backConn == 0) {
            _AccessEdge* exit = new _AccessEdge(myNumericalID++, fwdConn, afterSplit);
            myIntermodalNet->addEdge(exit);
            fwdConn->addSuccessor(exit);
            exit->addSuccessor(afterSplit);
        } else {
            _AccessEdge* backward = new _AccessEdge(myNumericalID++, beforeSplit, backConn);
            myIntermodalNet->addEdge(backward);
            beforeSplit->addSuccessor(backward);
            backward->addSuccessor(backConn);
        }
        return splitIndex;
    }

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
                const std::vector<E*>& successors = edge->getSuccessors();
                for (typename std::vector<E*>::const_iterator it = successors.begin(); it != successors.end(); ++it) {
                    carEdge->addSuccessor(getCarEdge(*it));
                }
                startConnector->addSuccessor(carEdge);
                carEdge->addSuccessor(endConnector);
            }
        }
    }

    inline void createNet() {
        if (myIntermodalNet == 0) {
            myIntermodalNet = new _IntermodalNetwork(E::getAllEdges(), myNumericalID);
            myNumericalID = (int)myIntermodalNet->getAllEdges().size();
            addCarEdges(E::getAllEdges());
            myCallback(*this);
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
    CreateNetCallback myCallback;

    /// @brief retrieve the car edge for the given input edge E
    std::map<const E*, _IntermodalEdge*> myCarLookup;

    /// @brief retrieve the public transport edges for the given line
    std::map<std::string, std::vector<_PTEdge*> > myPTLines;

    /// @brief retrieve the connecting edges for the given "bus" stop
    std::map<std::string, _IntermodalEdge*> myStopConnections;

    /// @brief retrieve the splitted edges for the given "original"
    std::map<_IntermodalEdge*, std::vector<_IntermodalEdge*> > myAccessSplits;


private:
    /// @brief Invalidated assignment operator
    IntermodalRouter& operator=(const IntermodalRouter& s);

};



/**
 * @class RouterProvider
 * The encapsulation of the routers for vehicles and pedestrians
 */
template<class E, class L, class N, class V>
class RouterProvider {
public:
    RouterProvider(SUMOAbstractRouter<E, V>* vehRouter,
                   PedestrianRouterDijkstra<E, L, N, V>* pedRouter,
                   IntermodalRouter<E, L, N, V>* interRouter)
        : myVehRouter(vehRouter), myPedRouter(pedRouter), myInterRouter(interRouter) {}

    RouterProvider(const RouterProvider& original)
        : myVehRouter(original.getVehicleRouter().clone()),
          myPedRouter(static_cast<PedestrianRouterDijkstra<E, L, N, V>*>(original.myPedRouter == 0 ? 0 : original.getPedestrianRouter().clone())),
          myInterRouter(static_cast<IntermodalRouter<E, L, N, V>*>(original.myInterRouter == 0 ? 0 : original.getIntermodalRouter().clone())) {}

    SUMOAbstractRouter<E, V>& getVehicleRouter() const {
        return *myVehRouter;
    }

    PedestrianRouterDijkstra<E, L, N, V>& getPedestrianRouter() const {
        return *myPedRouter;
    }

    IntermodalRouter<E, L, N, V>& getIntermodalRouter() const {
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
    IntermodalRouter<E, L, N, V>* const myInterRouter;


private:
    /// @brief Invalidated assignment operator
    RouterProvider& operator=(const RouterProvider& src);

};


#endif

/****************************************************************************/
