/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2021 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_Taxi.cpp
/// @author  Jakob Erdmann
/// @date    16.12.2019
///
// A device which controls a taxi
/****************************************************************************/
#include <config.h>

#include <utils/common/StringUtils.h>
#include <utils/common/StaticCommand.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSStop.h>
#include <microsim/MSStoppingPlace.h>

#include "MSDispatch.h"
#include "MSDispatch_Greedy.h"
#include "MSDispatch_GreedyShared.h"
#include "MSDispatch_RouteExtension.h"
#include "MSDispatch_TraCI.h"

#include "MSIdling.h"

#include "MSRoutingEngine.h"
#include "MSDevice_Taxi.h"

//#define DEBUG_DISPATCH
#define DEBUG_COND (true)

// ===========================================================================
// static member variables
// ===========================================================================
SUMOTime MSDevice_Taxi::myDispatchPeriod(0);
/// @brief the dispatch algorithm
MSDispatch* MSDevice_Taxi::myDispatcher(nullptr);
/// @brief The repeated call to the dispatcher
Command* MSDevice_Taxi::myDispatchCommand(nullptr);
// @brief the list of available taxis
std::vector<MSDevice_Taxi*> MSDevice_Taxi::myFleet;
int MSDevice_Taxi::myMaxCapacity(0);
int MSDevice_Taxi::myMaxContainerCapacity(0);

#define TAXI_SERVICE "taxi"
#define TAXI_SERVICE_PREFIX "taxi:"

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Taxi::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("Taxi Device");
    insertDefaultAssignmentOptions("taxi", "Taxi Device", oc);

    oc.doRegister("device.taxi.dispatch-algorithm", new Option_String("greedy"));
    oc.addDescription("device.taxi.dispatch-algorithm", "Taxi Device", "The dispatch algorithm [greedy|greedyClosest|greedyShared|routeExtension|traci]");

    oc.doRegister("device.taxi.dispatch-algorithm.output", new Option_FileName());
    oc.addDescription("device.taxi.dispatch-algorithm.output", "Taxi Device", "Write information from the dispatch algorithm to FILE");

    oc.doRegister("device.taxi.dispatch-algorithm.params", new Option_String(""));
    oc.addDescription("device.taxi.dispatch-algorithm.params", "Taxi Device", "Load dispatch algorithm parameters in format KEY1:VALUE1[,KEY2:VALUE]");

    oc.doRegister("device.taxi.dispatch-period", new Option_String("60", "TIME"));
    oc.addDescription("device.taxi.dispatch-period", "Taxi Device", "The period between successive calls to the dispatcher");

    oc.doRegister("device.taxi.idle-algorithm", new Option_String("stop"));
    oc.addDescription("device.taxi.idle-algorithm", "Taxi Device", "The behavior of idle taxis [stop|randomCircling]");

    oc.doRegister("device.taxi.idle-algorithm.output", new Option_FileName());
    oc.addDescription("device.taxi.idle-algorithm.output", "Taxi Device", "Write information from the idling algorithm to FILE");
}


void
MSDevice_Taxi::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "taxi", v, false)) {
        // build the device
        MSDevice_Taxi* device = new MSDevice_Taxi(v, "taxi_" + v.getID());
        into.push_back(device);
        myFleet.push_back(device);
        if (v.getParameter().line == "") {
            // automatically set the line so that persons are willing to enter
            // (see MSStageDriving::isWaitingFor)
            const_cast<SUMOVehicleParameter&>(v.getParameter()).line = TAXI_SERVICE;
        }
        if (v.getVClass() != SVC_TAXI) {
            WRITE_WARNING("Vehicle '" + v.getID() + "' with device.taxi should have vClass taxi instead of '" + toString(v.getVClass()) + "'.");
        }
        const int personCapacity = v.getVehicleType().getPersonCapacity();
        const int containerCapacity = v.getVehicleType().getContainerCapacity();
        myMaxCapacity = MAX2(myMaxCapacity, personCapacity);
        myMaxContainerCapacity = MAX2(myMaxContainerCapacity, containerCapacity);
        if (personCapacity < 1 && containerCapacity < 1) {
            WRITE_WARNINGF("Vehicle '%' with personCapacity % and containerCapacity % is not usable as taxi.", v.getID(), toString(personCapacity), toString(containerCapacity));
        }
    }
}


void
MSDevice_Taxi::initDispatch() {
    OptionsCont& oc = OptionsCont::getOptions();
    myDispatchPeriod = string2time(oc.getString("device.taxi.dispatch-period"));
    // init dispatch algorithm
    std::string algo = oc.getString("device.taxi.dispatch-algorithm");
    Parameterised params;
    params.setParametersStr(OptionsCont::getOptions().getString("device.taxi.dispatch-algorithm.params"), ":", ",");
    if (algo == "greedy") {
        myDispatcher = new MSDispatch_Greedy(params.getParametersMap());
    } else if (algo == "greedyClosest") {
        myDispatcher = new MSDispatch_GreedyClosest(params.getParametersMap());
    } else if (algo == "greedyShared") {
        myDispatcher = new MSDispatch_GreedyShared(params.getParametersMap());
    } else if (algo == "routeExtension") {
        myDispatcher = new MSDispatch_RouteExtension(params.getParametersMap());
    } else if (algo == "traci") {
        myDispatcher = new MSDispatch_TraCI(params.getParametersMap());
    } else {
        throw ProcessError("Dispatch algorithm '" + algo + "' is not known");
    }
    myDispatchCommand = new StaticCommand<MSDevice_Taxi>(&MSDevice_Taxi::triggerDispatch);
    // round to next multiple of myDispatchPeriod
    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    const SUMOTime begin = string2time(oc.getString("begin"));
    const SUMOTime delay = (myDispatchPeriod - ((now - begin) % myDispatchPeriod)) % myDispatchPeriod;
    MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(myDispatchCommand, now + delay);
}

bool
MSDevice_Taxi::isReservation(const std::set<std::string>& lines) {
    return lines.size() == 1 && (
               *lines.begin() == TAXI_SERVICE
               || StringUtils::startsWith(*lines.begin(), TAXI_SERVICE_PREFIX));
}

void
MSDevice_Taxi::addReservation(MSTransportable* person,
                              const std::set<std::string>& lines,
                              SUMOTime reservationTime,
                              SUMOTime pickupTime,
                              const MSEdge* from, double fromPos,
                              const MSEdge* to, double toPos,
                              const std::string& group) {
    if (!isReservation(lines)) {
        return;
    }
    if ((to->getPermissions() & SVC_TAXI) == 0) {
        throw ProcessError("Cannot add taxi reservation for " + std::string(person->isPerson() ? "person" : "container")
                           + " '" + person->getID() + "' because destination edge '" + to->getID() + "'"
                           + " does not permit taxi access");
    }
    if ((from->getPermissions() & SVC_TAXI) == 0) {
        throw ProcessError("Cannot add taxi reservation for " + std::string(person->isPerson() ? "person" : "container")
                           + " '" + person->getID() + "' because origin edge '" + from->getID() + "'"
                           + " does not permit taxi access");
    }
    if (myDispatchCommand == nullptr) {
        initDispatch();
    }
    myDispatcher->addReservation(person, reservationTime, pickupTime, from, fromPos, to, toPos, group, *lines.begin(), myMaxCapacity, myMaxContainerCapacity);
}

void
MSDevice_Taxi::removeReservation(MSTransportable* person,
                                 const std::set<std::string>& lines,
                                 const MSEdge* from, double fromPos,
                                 const MSEdge* to, double toPos,
                                 const std::string& group) {
    if (myDispatcher != nullptr && lines.size() == 1 && *lines.begin() == TAXI_SERVICE) {
        myDispatcher->removeReservation(person, from, fromPos, to, toPos, group);
    }
}


SUMOTime
MSDevice_Taxi::triggerDispatch(SUMOTime currentTime) {
    std::vector<MSDevice_Taxi*> active;
    for (MSDevice_Taxi* taxi : myFleet) {
        if (taxi->getHolder().hasDeparted()) {
            active.push_back(taxi);
        }
    }
    myDispatcher->computeDispatch(currentTime, active);
    return myDispatchPeriod;
}

bool
MSDevice_Taxi::hasServableReservations() {
    return myDispatcher != nullptr && myDispatcher->hasServableReservations();
}

void
MSDevice_Taxi::cleanup() {
    if (myDispatcher != nullptr) {
        delete myDispatcher;
        myDispatcher = nullptr;
    }
    myDispatchCommand = nullptr;
}

// ---------------------------------------------------------------------------
// MSDevice_Taxi-methods
// ---------------------------------------------------------------------------
MSDevice_Taxi::MSDevice_Taxi(SUMOVehicle& holder, const std::string& id) :
    MSVehicleDevice(holder, id) {
    std::string defaultServiceEnd = toString(1e15);
    const std::string algo = getStringParam(holder, OptionsCont::getOptions(), "taxi.idle-algorithm", "", false);
    if (algo == "stop") {
        myIdleAlgorithm = new MSIdling_Stop();
    } else if (algo == "randomCircling") {
        myIdleAlgorithm = new MSIdling_RandomCircling();
        // make sure simulation terminates
        defaultServiceEnd = toString(STEPS2TIME(
                                         myHolder.getParameter().departProcedure == DEPART_GIVEN
                                         ? myHolder.getParameter().depart
                                         : MSNet::getInstance()->getCurrentTimeStep()) + (3600 * 8));
    } else {
        throw ProcessError("Idle algorithm '" + algo + "' is not known for vehicle '" + myHolder.getID() + "'");
    }
    myServiceEnd = string2time(getStringParam(holder, OptionsCont::getOptions(), "taxi.end", defaultServiceEnd, false));
}


MSDevice_Taxi::~MSDevice_Taxi() {
    myFleet.erase(std::find(myFleet.begin(), myFleet.end(), this));
    // recompute myMaxCapacity
    myMaxCapacity = 0;
    myMaxContainerCapacity = 0;
    for (MSDevice_Taxi* taxi : myFleet) {
        myMaxCapacity = MAX2(myMaxCapacity, taxi->getHolder().getVehicleType().getPersonCapacity());
        myMaxContainerCapacity = MAX2(myMaxContainerCapacity, taxi->getHolder().getVehicleType().getContainerCapacity());
    }
}


SUMOVehicle*
MSDevice_Taxi::getTaxi() {
    if (myFleet.size() > 0) {
        return &myFleet[0]->getHolder();
    } else {
        return nullptr;
    }
}


void
MSDevice_Taxi::dispatch(const Reservation& res) {
    dispatchShared({&res, &res});
}


void
MSDevice_Taxi::dispatchShared(std::vector<const Reservation*> reservations) {
#ifdef DEBUG_DISPATCH
    if (DEBUG_COND) {
        std::cout << SIMTIME << " taxi=" << myHolder.getID() << " dispatch:\n";
        for (const Reservation* res : reservations) {
            std::cout << "   persons=" << toString(res->persons) << "\n";
        }
    }
#endif
    ConstMSEdgeVector tmpEdges;
    std::vector<SUMOVehicleParameter::Stop> stops;
    double lastPos = myHolder.getPositionOnLane();
    MSBaseVehicle* veh = dynamic_cast<MSBaseVehicle*>(&myHolder);
    assert(veh != nullptr);
    const MSEdge* rerouteOrigin = veh->getRerouteOrigin();
    if (isEmpty()) {
        // start fresh from the current edge
        myHolder.abortNextStop();
        assert(!veh->hasStops());
        tmpEdges.push_back(myHolder.getEdge());
        if (myHolder.getEdge() != rerouteOrigin) {
            tmpEdges.push_back(rerouteOrigin);
        }
    } else {
        assert(veh->hasStops());
        // check how often existing customers appear in the new reservations
        std::map<const MSTransportable*, int> nOccur;
        for (const Reservation* res : reservations) {
            for (MSTransportable* person : res->persons) {
                if (myCustomers.count(person) != 0) {
                    nOccur[person] += 1;
                    if (myCurrentReservations.count(res) == 0) {
                        throw ProcessError("Invalid Re-dispatch for existing customer '" + person->getID() + "' with a new reservation");
                    }
                }
            }
        }
        if (nOccur.size() == 0) {
            // no overlap with existing customers - extend route
            tmpEdges = myHolder.getRoute().getEdges();
            lastPos = veh->getStops().back().pars.endPos;
#ifdef DEBUG_DISPATCH
            if (DEBUG_COND) {
                std::cout << " re-dispatch with route-extension\n";
            }
#endif
        } else if (nOccur.size() == myCustomers.size()) {
            // redefine route (verify correct number of mentions)
            std::set<const MSTransportable*> onBoard;
            const std::vector<MSTransportable*>& onBoardP = myHolder.getPersons();
            const std::vector<MSTransportable*>& onBoardC = myHolder.getContainers();
            onBoard.insert(onBoardP.begin(), onBoardP.end());
            onBoard.insert(onBoardC.begin(), onBoardC.end());
            std::set<const MSTransportable*> redundantPickup;
            for (auto item : nOccur) {
                if (item.second == 1) {
                    // customers must already be on board
                    if (onBoard.count(item.first) == 0) {
                        throw ProcessError("Re-dispatch did not mention pickup for existing customer '" + item.first->getID() + "'");
                    }
                } else if (item.second == 2) {
                    if (onBoard.count(item.first) == 0) {
                        // treat like a new customer
                        myCustomers.erase(item.first);
                    } else {
                        redundantPickup.insert(item.first);
                    }
                } else {
                    throw ProcessError("Re-dispatch mentions existing customer '" + item.first->getID() + "' " + toString(item.second) + " times");
                }
            }
            // remove redundancy
            if (!redundantPickup.empty()) {
                for (auto it = reservations.begin(); it != reservations.end();) {
                    bool isRedundant = false;
                    for (const MSTransportable* person : (*it)->persons) {
                        if (redundantPickup.count(person) != 0) {
                            isRedundant = true;
                            break;
                        }
                    }
                    if (isRedundant) {
                        for (const MSTransportable* person : (*it)->persons) {
                            redundantPickup.erase(person);
                        }
                        it = reservations.erase(it);
                    } else {
                        it++;
                    }
                }
            }
            while (veh->hasStops()) {
                myHolder.abortNextStop();
            }
            tmpEdges.push_back(myHolder.getEdge());
            if (myHolder.getEdge() != rerouteOrigin) {
                tmpEdges.push_back(rerouteOrigin);
            }
#ifdef DEBUG_DISPATCH
            if (DEBUG_COND) {
                std::cout << " re-dispatch from scratch\n";
            }
#endif
        } else {
            // inconsistent re-dispatch
            std::vector<std::string> missing;
            for (const MSTransportable* c : myCustomers) {
                if (nOccur.count(c) == 0) {
                    missing.push_back(c->getID());
                }
            }
            throw ProcessError("Re-dispatch did mention some customers but failed to mention " + joinToStringSorting(missing, " "));
        }
    }

    const SUMOTime t = MSNet::getInstance()->getCurrentTimeStep();
    for (const Reservation* res : reservations) {
        myCurrentReservations.insert(res);
        bool isPickup = false;
        for (MSTransportable* person : res->persons) {
            if (myCustomers.count(person) == 0) {
                myCustomers.insert(person);
                isPickup = true;
            }
        }
        if (isPickup) {
            prepareStop(tmpEdges, stops, lastPos, res->from, res->fromPos, "pickup " + toString(res->persons) + " (" + res->id + ")");
            for (const MSTransportable* const transportable : res->persons) {
                if (transportable->isPerson()) {
                    stops.back().triggered = true;
                } else {
                    stops.back().containerTriggered = true;
                }
                stops.back().permitted.insert(transportable->getID());
            }
            //stops.back().awaitedPersons.insert(res.person->getID());
            stops.back().parametersSet |= STOP_PERMITTED_SET;
            if (stops.back().duration == -1) {
                // keep dropOffDuration if the stop is dropOff and pickUp
                stops.back().duration = TIME2STEPS(getFloatParam(myHolder, OptionsCont::getOptions(), "taxi.pickUpDuration", 0, false));
            }
        } else {
            prepareStop(tmpEdges, stops, lastPos, res->to, res->toPos, "dropOff " + toString(res->persons) + " (" + res->id + ")");
            stops.back().duration = TIME2STEPS(getFloatParam(myHolder, OptionsCont::getOptions(), "taxi.dropOffDuration", 60, false)); // pay and collect bags
        }
    }
#ifdef DEBUG_DISPATCH
    if (DEBUG_COND) {
        std::cout << "   tmpEdges=" << toString(tmpEdges) << "\n";
    }
#endif
    if (!myHolder.replaceRouteEdges(tmpEdges, -1, 0, "taxi:prepare_dispatch", false, false, false)) {
        throw ProcessError("Route replacement for taxi dispatch failed for vehicle '" + myHolder.getID()
                           + "' at time " + time2string(MSNet::getInstance()->getCurrentTimeStep()));
    }
#ifdef DEBUG_DISPATCH
    if (DEBUG_COND) std::cout << "   replacedRoute=" << toString(tmpEdges)
                                  << "\n     actualRoute=" << toString(myHolder.getRoute().getEdges()) << "\n";
#endif
    for (SUMOVehicleParameter::Stop& stop : stops) {
        std::string error;
        myHolder.addStop(stop, error);
        if (error != "") {
            WRITE_WARNINGF("Could not add taxi stop for vehicle '%' to %. time=% error=%", myHolder.getID(), stop.actType, time2string(t), error)
        }
    }
    SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = MSRoutingEngine::getRouterTT(myHolder.getRNGIndex(), myHolder.getVClass());
    // SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = myHolder.getInfluencer().getRouterTT(veh->getRNGIndex())
    myHolder.reroute(t, "taxi:dispatch", router, false);
#ifdef DEBUG_DISPATCH
    if (DEBUG_COND) {
        std::cout << "\n      finalRoute=" << toString(myHolder.getRoute().getEdges()) << " routeIndex=" << myHolder.getRoutePosition() << "\n";
    }
#endif
    myState |= PICKUP;
}


void
MSDevice_Taxi::prepareStop(ConstMSEdgeVector& edges,
                           std::vector<SUMOVehicleParameter::Stop>& stops,
                           double& lastPos, const MSEdge* stopEdge, double stopPos,
                           const std::string& action) {
    assert(!edges.empty());
    if (stopPos < lastPos && stopPos + NUMERICAL_EPS >= lastPos) {
        stopPos = lastPos;
    }
    if (stopEdge == edges.back() && !stops.empty()) {
        if (stopPos >= lastPos && stopPos <= stops.back().endPos) {
            // no new stop and no adaption needed
            stops.back().actType += "," + action;
            return;
        }
        if (stopPos >= lastPos && stopPos <= lastPos + myHolder.getVehicleType().getLength()) {
            // stop length adaption needed
            stops.back().endPos = MIN2(lastPos + myHolder.getVehicleType().getLength(), stopEdge->getLength());
            stops.back().actType += "," + action;
            return;
        }
    }
    if (stopEdge != edges.back() || stopPos < lastPos) {
        edges.push_back(stopEdge);
    }
    lastPos = stopPos;
    SUMOVehicleParameter::Stop stop;
    stop.lane = getStopLane(stopEdge, action)->getID();
    stop.startPos = stopPos;
    stop.endPos = MAX2(stopPos, MIN2(myHolder.getVehicleType().getLength(), stopEdge->getLength()));
    stop.parking = getBoolParam(myHolder, OptionsCont::getOptions(), "taxi.parking", true, false);
    stop.actType = action;
    stop.index = STOP_INDEX_END;
    stops.push_back(stop);
}


MSLane*
MSDevice_Taxi::getStopLane(const MSEdge* edge, const std::string& action) {
    const std::vector<MSLane*>* allowedLanes = edge->allowedLanes(myHolder.getVClass());
    if (allowedLanes == nullptr) {
        throw ProcessError("Taxi vehicle '" + myHolder.getID() + "' cannot stop on edge '" + edge->getID() + "' (" + action + ")");
    }
    return allowedLanes->front();
}

bool
MSDevice_Taxi::isEmpty() {
    return myState == EMPTY;
}


bool
MSDevice_Taxi::allowsBoarding(MSTransportable* t) const {
    return myCustomers.count(t) != 0;
}


bool
MSDevice_Taxi::notifyMove(SUMOTrafficObject& /*tObject*/, double oldPos,
                          double newPos, double /*newSpeed*/) {
    if (myHolder.getPersonNumber() > 0 || myHolder.getContainerNumber() > 0) {
        myOccupiedDistance += (newPos - oldPos);
        myOccupiedTime += DELTA_T;
    }
    if (isEmpty() && MSNet::getInstance()->getCurrentTimeStep() < myServiceEnd) {
        myIdleAlgorithm->idle(this);
    }
    if (myHolder.isStopped()) {
        if (!myIsStopped) {
            // limit duration of stop
            // @note: stops are not yet added to the vehicle so we can change the loaded parameters. Stops added from a route are not affected
            myHolder.getNextStop().endBoarding = myServiceEnd;
        }
    }
    myIsStopped = myHolder.isStopped();
    return true; // keep the device
}


bool
MSDevice_Taxi::notifyEnter(SUMOTrafficObject& /*veh*/, MSMoveReminder::Notification /*reason*/, const MSLane* /* enteredLane */) {
    if (isEmpty() && MSNet::getInstance()->getCurrentTimeStep() < myServiceEnd) {
        myIdleAlgorithm->idle(this);
    }
    return true; // keep the device
}


bool
MSDevice_Taxi::notifyLeave(SUMOTrafficObject& /*veh*/, double /*lastPos*/, MSMoveReminder::Notification /*reason*/, const MSLane* /* enteredLane */) {
    return true; // keep the device
}

void
MSDevice_Taxi::customerEntered(const MSTransportable* t) {
    myState |= OCCUPIED;
    if (!hasFuturePickup()) {
        myState &= ~PICKUP;
    }
    for (const Reservation* res : myCurrentReservations) {
        for (const MSTransportable* cand : res->persons) {
            if (cand == t) {
                const_cast<Reservation*>(res)->state = Reservation::ONBOARD;
                break;
            }
        }
    }
}


void
MSDevice_Taxi::customerArrived(const MSTransportable* person) {
    myCustomersServed++;
    myCustomers.erase(person);
    if (myHolder.getPersonNumber() == 0 && myHolder.getContainerNumber() == 0) {
        myState &= ~OCCUPIED;
        if (myHolder.getStops().size() > 1 && (myState & PICKUP) == 0) {
            WRITE_WARNINGF("All customers left vehicle '%' at time % but there are % remaining stops",
                           myHolder.getID(), time2string(MSNet::getInstance()->getCurrentTimeStep()), myHolder.getStops().size() - 1);
            while (myHolder.getStops().size() > 1) {
                myHolder.abortNextStop(1);
            }
        }
    }
    if (isEmpty()) {
        // cleanup
        for (const Reservation* res : myCurrentReservations) {
            myDispatcher->fulfilledReservation(res);
        }
        myCurrentReservations.clear();
    } else {
        // check whether a single reservation has been fulfilled
        for (auto resIt = myCurrentReservations.begin(); resIt != myCurrentReservations.end();) {
            bool fulfilled = true;
            for (MSTransportable* t : (*resIt)->persons) {
                if (myCustomers.count(t) != 0) {
                    fulfilled = false;
                    break;
                }
            }
            if (fulfilled) {
                myDispatcher->fulfilledReservation(*resIt);
                resIt = myCurrentReservations.erase(resIt);
            } else {
                ++resIt;
            }
        }
    }
}

bool
MSDevice_Taxi::hasFuturePickup() {
    for (const auto& stop : myHolder.getStops()) {
        if (stop.reached) {
            continue;
        }
        if (stop.pars.permitted.size() > 0) {
            return true;
        }
    }
    return false;
}

void
MSDevice_Taxi::generateOutput(OutputDevice* tripinfoOut) const {
    if (tripinfoOut != nullptr) {
        tripinfoOut->openTag("taxi");
        tripinfoOut->writeAttr("customers", toString(myCustomersServed));
        tripinfoOut->writeAttr("occupiedDistance", toString(myOccupiedDistance));
        tripinfoOut->writeAttr("occupiedTime", time2string(myOccupiedTime));
        tripinfoOut->closeTag();
    }
}

std::string
MSDevice_Taxi::getParameter(const std::string& key) const {
    if (key == "customers") {
        return toString(myCustomersServed);
    } else if (key == "occupiedDistance") {
        return toString(myOccupiedDistance);
    } else if (key == "occupiedTime") {
        return toString(STEPS2TIME(myOccupiedTime));
    } else if (key == "state") {
        return toString(myState);
    } else if (key == "currentCustomers") {
        return joinNamedToStringSorting(myCustomers, " ");
    } else if (key == "pickUpDuration") {
        return getStringParam(myHolder, OptionsCont::getOptions(), "taxi.pickUpDuration", "0", false);
    } else if (key == "dropOffDuration") {
        return getStringParam(myHolder, OptionsCont::getOptions(), "taxi.dropOffDuration", "60", false);
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


void
MSDevice_Taxi::setParameter(const std::string& key, const std::string& value) {
    double doubleValue;
    try {
        doubleValue = StringUtils::toDouble(value);
    } catch (NumberFormatException&) {
        throw InvalidArgument("Setting parameter '" + key + "' requires a number for device of type '" + deviceName() + "'");
    }
    if (key == "pickUpDuration" || key == "dropOffDuration") {
        // store as generic vehicle parameters
        ((SUMOVehicleParameter&)myHolder.getParameter()).setParameter("device.taxi." + key, value);
    } else {
        UNUSED_PARAMETER(doubleValue);
        throw InvalidArgument("Setting parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
    }
}

bool
MSDevice_Taxi::compatibleLine(const std::string& taxiLine, const std::string& rideLine) {
    return (taxiLine == rideLine
            || (taxiLine == TAXI_SERVICE && StringUtils::startsWith(rideLine, "taxi:"))
            || (rideLine == TAXI_SERVICE && StringUtils::startsWith(taxiLine, "taxi:")));
}

bool
MSDevice_Taxi::compatibleLine(const Reservation* res) {
    return compatibleLine(myHolder.getParameter().line, res->line);
}


/****************************************************************************/
