/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    IntermodalRouter.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 03 March 2014
/// @version $Id$
///
// The IntermodalRouter builds a special network and (delegates to a SUMOAbstractRouter)
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
#include <utils/iodevices/OutputDevice.h>
#include "SUMOAbstractRouter.h"
#include "SUMOVehicleParameter.h"
#include "DijkstraRouter.h"
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
template<class E, class L, class N, class V, class INTERNALROUTER = DijkstraRouter<IntermodalEdge<E, L, N, V>, IntermodalTrip<E, N, V>, prohibited_withPermissions<IntermodalEdge<E, L, N, V>, IntermodalTrip<E, N, V> > > >
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
        TripItem(const std::string& _line = "") : line(_line), cost(0.) {}
        std::string line;
        std::string destStop;
        std::vector<const E*> edges;
        double cost;
    };

    /** @brief where mode changes are possible
    */
    enum ModeChangeOptions {
        /// @brief parking areas
        PARKING_AREAS = 1,
        /// @brief public transport stops and access
        PT_STOPS = 2,
        /// @brief junctions with edges allowing the additional mode
        ALL_JUNCTIONS = 4
    };

    /// Constructor
    IntermodalRouter(CreateNetCallback callback, int carWalkTransfer) :
        SUMOAbstractRouter<E, _IntermodalTrip>(0, "IntermodalRouter"),
        myAmClone(false), myInternalRouter(0), myIntermodalNet(0), myNumericalID(0),
        myCallback(callback), myCarWalkTransfer(carWalkTransfer) {
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

    bool hasNet() const {
        return myIntermodalNet != 0;
    }

    /** @brief Adds access edges for stopping places to the intermodal network
     *
     * This method creates an intermodal stop edge to represent the stoppping place
     *  (if not present yet) and determines the edges which need to be splitted (usually the forward
     *  and the backward pedestrian edges and the car edge) and calls splitEdge for the
     *  actual split and the connection of the stop edge with access edges. After that it adds and adapts
     *  the depart and arrival connectors to the new edge(s).
     *
     * @param[in] stopId The id of the stop to add
     * @param[in] stopEdge The edge on which the stop is located
     * @param[in] pos The relative position on the edge where the stop is located
     * @param[in] category The type of stop
     */
    void addAccess(const std::string& stopId, const E* stopEdge, const double pos, const SumoXMLTag category) {
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
            splitEdge(pair.second, backSplit, stopEdge->getLength() - pos, stopConn, false);
            _IntermodalEdge* carSplit = nullptr;
            if (myCarLookup.count(stopEdge) > 0) {
                carSplit = new CarEdge<E, L, N, V>(myNumericalID++, stopEdge, pos);
                splitEdge(myCarLookup[stopEdge], carSplit, pos, stopConn, true, false);
            }
            if (splitIndex >= 0) {
                if (carSplit != nullptr && ((category == SUMO_TAG_PARKING_AREA && (myCarWalkTransfer & PARKING_AREAS) != 0) || (category == SUMO_TAG_BUS_STOP && (myCarWalkTransfer & PT_STOPS) != 0))) {
                    _IntermodalEdge* const beforeSplit = myAccessSplits[myCarLookup[stopEdge]][splitIndex - 1];
                    for (_IntermodalEdge* conn : {
                                fwdSplit, backSplit
                            }) {
                        _AccessEdge* access = new _AccessEdge(myNumericalID++, beforeSplit, conn);
                        myIntermodalNet->addEdge(access);
                        beforeSplit->addSuccessor(access);
                        access->addSuccessor(conn);
                    }
                }

                _IntermodalEdge* const prevDep = myIntermodalNet->getDepartConnector(stopEdge, splitIndex - 1);
                const std::vector<_IntermodalEdge*>& backSplitList = myAccessSplits[pair.second];
                _IntermodalEdge* const backBeforeSplit = backSplitList[backSplitList.size() - 1 - splitIndex];
                // depart and arrival edges (the router can decide the initial direction to take and the direction to arrive from)
                _IntermodalEdge* const depConn = new _IntermodalEdge(stopEdge->getID() + "_depart_connector" + toString(pos), myNumericalID++, stopEdge, "!connector");
                depConn->addSuccessor(fwdSplit);
                depConn->addSuccessor(backBeforeSplit);
                depConn->setLength(fwdSplit->getLength());
                prevDep->removeSuccessor(backBeforeSplit);
                prevDep->addSuccessor(backSplit);
                prevDep->setLength(backSplit->getLength());
                if (carSplit != nullptr) {
                    depConn->addSuccessor(carSplit);
                }

                _IntermodalEdge* const prevArr = myIntermodalNet->getArrivalConnector(stopEdge, splitIndex - 1);
                _IntermodalEdge* const fwdBeforeSplit = myAccessSplits[pair.first][splitIndex - 1];
                _IntermodalEdge* const arrConn = new _IntermodalEdge(stopEdge->getID() + "_arrival_connector" + toString(pos), myNumericalID++, stopEdge, "!connector");
                fwdSplit->addSuccessor(arrConn);
                backBeforeSplit->addSuccessor(arrConn);
                arrConn->setLength(fwdSplit->getLength());
                fwdSplit->removeSuccessor(prevArr);
                fwdBeforeSplit->addSuccessor(prevArr);
                prevArr->setLength(backSplit->getLength());
                if (carSplit != nullptr) {
                    carSplit->addSuccessor(arrConn);
                    carSplit->removeSuccessor(prevArr);
                    myAccessSplits[myCarLookup[stopEdge]][splitIndex - 1]->addSuccessor(prevArr);
                }
                myIntermodalNet->addConnectors(depConn, arrConn, splitIndex);
            } else {
                delete fwdSplit;
                delete backSplit;
                delete carSplit;
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
    bool compute(const E* from, const E* to, double departPos, double arrivalPos, double speed,
                 const V* const vehicle, const SVCPermissions modeSet, SUMOTime msTime,
                 std::vector<TripItem>& into) {
        createNet();
        _IntermodalTrip trip(from, to, departPos, arrivalPos, speed, msTime, 0, vehicle, modeSet);
        std::vector<const _IntermodalEdge*> intoEdges;
        const bool success = myInternalRouter->compute(myIntermodalNet->getDepartEdge(from, trip.departPos),
                             myIntermodalNet->getArrivalEdge(to, trip.arrivalPos),
                             &trip, msTime, intoEdges);
        if (success) {
            std::string lastLine = "";
            double time = STEPS2TIME(msTime);
            for (const _IntermodalEdge* iEdge : intoEdges) {
                if (iEdge->includeInRoute(false)) {
                    if (iEdge->getLine() == "!stop") {
                        into.back().destStop = iEdge->getID();
                        if (lastLine == "!ped") {
                            lastLine = ""; // a stop always starts a new trip item
                        }
                    } else {
                        if (iEdge->getLine() != lastLine) {
                            lastLine = iEdge->getLine();
                            if (lastLine == "!car") {
                                into.push_back(TripItem(vehicle->getID()));
                            } else if (lastLine == "!ped") {
                                into.push_back(TripItem());
                            } else {
                                into.push_back(TripItem(lastLine));
                            }
                        }
                        if (into.back().edges.empty() || into.back().edges.back() != iEdge->getEdge()) {
                            into.back().edges.push_back(iEdge->getEdge());
                        }
                    }
                }
                const double edgeEffort = myInternalRouter->getEffort(iEdge, &trip, time);
                time += edgeEffort;
                if (!into.empty()) {
                    into.back().cost += edgeEffort;
                }
            }
        }
#ifdef IntermodalRouter_DEBUG_ROUTES
        double time = STEPS2TIME(msTime);
        for (const _IntermodalEdge* iEdge : intoEdges) {
            const double edgeEffort = myInternalRouter->getEffort(iEdge, &trip, time);
            time += edgeEffort;
            std::cout << iEdge->getID() << "(" << iEdge->getLine() << "): " << edgeEffort << std::endl;
        }
        std::cout << TIME2STEPS(msTime) << " trip from " << from->getID() << " to " << to->getID()
                  << " departPos=" << trip.departPos
                  << " arrivalPos=" << trip.arrivalPos
                  << " edges=" << toString(intoEdges)
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

    double recomputeCosts(const std::vector<const E*>&, const _IntermodalTrip* const, SUMOTime) const {
        throw ProcessError("Do not use this method");
    }

    void prohibit(const std::vector<E*>& toProhibit) {
        createNet();
        std::vector<_IntermodalEdge*> toProhibitPE;
        for (typename std::vector<E*>::const_iterator it = toProhibit.begin(); it != toProhibit.end(); ++it) {
            toProhibitPE.push_back(myIntermodalNet->getBothDirections(*it).first);
            toProhibitPE.push_back(myIntermodalNet->getBothDirections(*it).second);
            toProhibitPE.push_back(getCarEdge(*it));
        }
        myInternalRouter->prohibit(toProhibitPE);
    }

    void writeNetwork(OutputDevice& dev) {
        createNet();
        for (_IntermodalEdge* e : myIntermodalNet->getAllEdges()) {
            dev.openTag(SUMO_TAG_EDGE);
            dev.writeAttr(SUMO_ATTR_ID, e->getID());
            dev.writeAttr(SUMO_ATTR_LENGTH, e->getLength());
            std::string succStr;
            for (_IntermodalEdge* suc : e->getSuccessors(SVC_IGNORING)) {
                succStr += suc->getID() + " ";
            }
            dev.writeAttr("successors", succStr);
            dev.closeTag();
        }
    }

    void writeWeights(OutputDevice& dev) {
        createNet();
        for (_IntermodalEdge* e : myIntermodalNet->getAllEdges()) {
            dev.openTag(SUMO_TAG_EDGE);
            dev.writeAttr(SUMO_ATTR_ID, e->getID());
            dev.writeAttr("traveltime", 0. /* e->getTravelTime() */);
            dev.writeAttr("effort", 0. /* e->getEffort() */);
            dev.closeTag();
        }
    }

private:
    IntermodalRouter(_IntermodalNetwork* net):
        SUMOAbstractRouter<E, _IntermodalTrip>(0, "PedestrianRouter"), myAmClone(true),
        myInternalRouter(new INTERNALROUTER(net->getAllEdges(), true, &_IntermodalEdge::getTravelTimeStatic)),
        myIntermodalNet(net), myNumericalID((int)net->getAllEdges().size()), myCarWalkTransfer(0) {}

    /** @brief Splits an edge (if necessary) and connects it to a stopping edge
     *
     * This method determines whether an edge needs to be split at the given position
     *  (if there is not already a split nearby) and connects the stop edge via new access edges.
     *
     * @param[in] toSplit The first edge in the split list
     * @param[in] afterSplit The edge to add if a split is performed
     * @param[in] pos The relative position on the edge where the stop is located
     * @param[in] stopConn The stop edge to connect to
     * @param[in] forward whether we are aplitting a forward edge (backward edges get different names)
     * @param[in] addExit whether we can just enter the stop or exit as well (cars should not exit yet)
     * @return the index in the split list where the new edge has been added (if there was already a split near the position -1)
     */
    int splitEdge(_IntermodalEdge* const toSplit, _IntermodalEdge* afterSplit, const double pos,
                  _IntermodalEdge* const stopConn, const bool forward = true, const bool addExit = true) {
        std::vector<_IntermodalEdge*>& splitList = myAccessSplits[toSplit];
        if (splitList.empty()) {
            splitList.push_back(toSplit);
        }
        double relPos = pos;
        int splitIndex = 0;
        for (const _IntermodalEdge* const split : splitList) {
            if (relPos < split->getLength() + POSITION_EPS) {
                break;
            }
            relPos -= split->getLength();
            splitIndex++;
        }
        assert(splitIndex < (int)splitList.size());
        _IntermodalEdge* beforeSplit = splitList[splitIndex];
        if (splitIndex + 1 < (int)splitList.size() && fabs(relPos - beforeSplit->getLength()) < POSITION_EPS) {
            // don't split, use the present split edges
            afterSplit = splitList[splitIndex + 1];
            splitIndex = -1;
        } else {
            myIntermodalNet->addEdge(afterSplit);
            afterSplit->setSuccessors(beforeSplit->getSuccessors(SVC_IGNORING));
            beforeSplit->clearSuccessors();
            beforeSplit->addSuccessor(afterSplit);
            afterSplit->setLength(beforeSplit->getLength() - relPos);
            beforeSplit->setLength(relPos);
            if (!forward) {
                // rename backward edges for easier referencing
                const std::string newID = beforeSplit->getID();
                beforeSplit->setID(afterSplit->getID());
                afterSplit->setID(newID);
            }
            splitIndex++;
            splitList.insert(splitList.begin() + splitIndex, afterSplit);
        }
        // add access to / from edge
        _AccessEdge* access = new _AccessEdge(myNumericalID++, beforeSplit, stopConn);
        myIntermodalNet->addEdge(access);
        beforeSplit->addSuccessor(access);
        access->addSuccessor(stopConn);
        if (addExit) {
            // pedestrian case only, exit from public to pedestrian
            _AccessEdge* exit = new _AccessEdge(myNumericalID++, stopConn, afterSplit);
            myIntermodalNet->addEdge(exit);
            stopConn->addSuccessor(exit);
            exit->addSuccessor(afterSplit);
        }
        return splitIndex;
    }

    void addCarEdges(const std::vector<E*>& edges) {
        for (const E* const edge : edges) {
            if (edge->getFunction() == EDGEFUNC_NORMAL) {
                myCarLookup[edge] = new CarEdge<E, L, N, V>(myNumericalID++, edge);
                myIntermodalNet->addEdge(myCarLookup[edge]);
            }
        }
        for (const auto& edgePair : myCarLookup) {
            _IntermodalEdge* const carEdge = edgePair.second;
            for (const E* const suc : edgePair.first->getSuccessors()) {
                _IntermodalEdge* const sucCarEdge = getCarEdge(suc);
                if (sucCarEdge != nullptr) {
                    carEdge->addSuccessor(sucCarEdge);
                }
            }
            if ((myCarWalkTransfer & ALL_JUNCTIONS) != 0) {
                _IntermodalEdge* const walkCon = myIntermodalNet->getWalkingConnector(edgePair.first);
                if (walkCon != 0) {
                    carEdge->addSuccessor(walkCon);
                } else {
                    // we are on an edge where pedestrians are forbidden and want to continue on an arbitrary pedestrian edge
                    for (const E* const out : edgePair.first->getToJunction()->getOutgoing()) {
                        if (!out->isInternal() && !out->isTazConnector() && getSidewalk<E, L>(out) != 0) {
                            carEdge->addSuccessor(myIntermodalNet->getBothDirections(out).first);
                        }
                    }
                    for (const E* const in : edgePair.first->getToJunction()->getIncoming()) {
                        if (!in->isInternal() && !in->isTazConnector() && getSidewalk<E, L>(in) != 0) {
                            carEdge->addSuccessor(myIntermodalNet->getBothDirections(in).second);
                        }
                    }
                }
            }
            myIntermodalNet->getDepartConnector(edgePair.first)->addSuccessor(carEdge);
            carEdge->addSuccessor(myIntermodalNet->getArrivalConnector(edgePair.first));
        }
    }

    inline void createNet() {
        if (myIntermodalNet == nullptr) {
            myIntermodalNet = new _IntermodalNetwork(E::getAllEdges(), myNumericalID);
            myNumericalID = (int)myIntermodalNet->getAllEdges().size();
            addCarEdges(E::getAllEdges());
            myCallback(*this);
            myInternalRouter = new INTERNALROUTER(myIntermodalNet->getAllEdges(), true, &_IntermodalEdge::getTravelTimeStatic);
        }
    }

    /// @brief Returns the associated car edge
    _IntermodalEdge* getCarEdge(const E* e) const {
        typename std::map<const E*, _IntermodalEdge*>::const_iterator it = myCarLookup.find(e);
        if (it == myCarLookup.end()) {
            return 0;
        }
        return it->second;
    }

private:
    const bool myAmClone;
    INTERNALROUTER* myInternalRouter;
    _IntermodalNetwork* myIntermodalNet;
    int myNumericalID;
    CreateNetCallback myCallback;
    const int myCarWalkTransfer;

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
