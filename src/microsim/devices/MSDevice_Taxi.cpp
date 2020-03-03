/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2020 German Aerospace Center (DLR) and others.
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
#include "MSDispatch.h"
#include "MSDispatch_GreedyShared.h"
#include "MSRoutingEngine.h"
#include "MSDevice_Taxi.h"

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

#define TAXI_SERVICE "taxi"

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
    oc.addDescription("device.taxi.dispatch-algorithm", "Taxi Device", "The dispatch algorithm [greedy|greedyClosest|greedyShared]");

    oc.doRegister("device.taxi.dispatch-algorithm.output", new Option_String("dispatch.xml"));
    oc.addDescription("device.taxi.dispatch-algorithm.output", "Taxi Device", "Write information from the dispatch algorithm to FILE");

    oc.doRegister("device.taxi.dispatch-algorithm.params", new Option_String(""));
    oc.addDescription("device.taxi.dispatch-algorithm.params", "Taxi Device", "Load dispatch algorithm parameters in format KEY1:VALUE1[,KEY2:VALUE]");

    oc.doRegister("device.taxi.dispatch-period", new Option_String("60", "TIME"));
    oc.addDescription("device.taxi.dispatch-period", "Taxi Device", "The period between successive calls to the dispatcher");
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

void
MSDevice_Taxi::addReservation(MSTransportable* person,
                              const std::set<std::string>& lines,
                              SUMOTime reservationTime,
                              SUMOTime pickupTime,
                              const MSEdge* from, double fromPos,
                              const MSEdge* to, double toPos,
                              const std::string& group) {
    if (lines.size() == 1 && *lines.begin() == TAXI_SERVICE) {
        if (myDispatchCommand == nullptr) {
            initDispatch();
        }
        myDispatcher->addReservation(person, reservationTime, pickupTime, from, fromPos, to, toPos, group);
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
    MSVehicleDevice(holder, id),
    myServiceEnd(string2time(getStringParam(holder, OptionsCont::getOptions(), "taxi.end", toString(1e15), false))) {
}


MSDevice_Taxi::~MSDevice_Taxi() {
    myFleet.erase(std::find(myFleet.begin(), myFleet.end(), this));
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
MSDevice_Taxi::dispatchShared(const std::vector<const Reservation*> reservations) {
    if (isEmpty()) {
        if (MSGlobals::gUseMesoSim) {
            throw ProcessError("Dispatch for meso not yet implemented");
        }
        MSVehicle* veh = static_cast<MSVehicle*>(&myHolder);
        veh->abortNextStop();
        ConstMSEdgeVector tmpEdges;
        std::vector<SUMOVehicleParameter::Stop> stops;
        tmpEdges.push_back(myHolder.getEdge());
        double lastPos = myHolder.getPositionOnLane();
        for (const Reservation* res : reservations) {
            bool isPickup = false;
            for (MSTransportable* person : res->persons) {
                if (myCustomers.count(person) == 0) {
                    myCustomers.insert(person);
                    isPickup = true;
                }
            }
            if (isPickup) {
                prepareStop(tmpEdges, stops, lastPos, res->from, res->fromPos, "pickup " + toString(res->persons));
                stops.back().triggered = true;
                //stops.back().awaitedPersons.insert(res.person->getID());
            } else {
                prepareStop(tmpEdges, stops, lastPos, res->to, res->toPos, "dropOff " + toString(res->persons));
                stops.back().duration = TIME2STEPS(60); // pay and collect bags
            }
        }
        stops.back().triggered = true; // stay in the simulaton
        veh->replaceRouteEdges(tmpEdges, -1, 0, "taxi:prepare_dispatch", false, false, false);
        for (auto stop : stops) {
            std::string error;
            myHolder.addStop(stop, error);
            if (error != "") {
                WRITE_WARNINGF("Could not add taxi stop for vehicle '%' to %. time=% error=%", myHolder.getID(), stop.actType, SIMTIME, error)
            }
        }
        SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = MSRoutingEngine::getRouterTT(myHolder.getRNGIndex(), myHolder.getVClass());
        // SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = myHolder.getInfluencer().getRouterTT(veh->getRNGIndex())
        myHolder.reroute(MSNet::getInstance()->getCurrentTimeStep(), "taxi:dispatch", router, false);
    } else {
        throw ProcessError("Dispatch for busy taxis not yet implemented");
    }
    myState = PICKUP;
}


void
MSDevice_Taxi::prepareStop(ConstMSEdgeVector& edges,
                           std::vector<SUMOVehicleParameter::Stop>& stops,
                           double& lastPos, const MSEdge* stopEdge, double stopPos,
                           const std::string& action) {
    assert(!edges.empty());
    if (stopEdge == edges.back() && stopPos == lastPos && !stops.empty()) {
        // no new stop needed
        return;
    }
    if (stopEdge != edges.back() || stopPos < lastPos) {
        edges.push_back(stopEdge);
    }
    lastPos = stopPos;
    SUMOVehicleParameter::Stop stop;
    stop.lane = getStopLane(stopEdge)->getID();
    stop.startPos = stopPos;
    stop.endPos = MAX2(stopPos, MIN2(myHolder.getVehicleType().getLength(), stopEdge->getLength()));
    stop.parking = true;
    stop.actType = action;
    stop.index = STOP_INDEX_END;
    stops.push_back(stop);
}


MSLane*
MSDevice_Taxi::getStopLane(const MSEdge* edge) {
    const std::vector<MSLane*>* allowedLanes = edge->allowedLanes(myHolder.getVClass());
    if (allowedLanes == nullptr) {
        throw ProcessError("Taxi '" + myHolder.getID() + "' cannot pick up person on edge '" + edge->getID() + "'");
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
    if (myHolder.getPersonNumber() > 0) {
        myOccupiedDistance += (newPos - oldPos);
        myOccupiedTime += DELTA_T;
    }
    if (myHolder.isStopped()) {
        if (!myIsStopped) {
            // limit duration of stop
            // @note: stops are not yet added to the vehicle so we can change the loaded parameters. Stops added from a route are not affected
            if (!MSGlobals::gUseMesoSim) {
                MSVehicle& veh = static_cast<MSVehicle&>(myHolder);
                veh.getNextStop().endBoarding = myServiceEnd;
            }
        }
    }
    myIsStopped = myHolder.isStopped();
    return true; // keep the device
}


bool
MSDevice_Taxi::notifyEnter(SUMOTrafficObject& /*veh*/, MSMoveReminder::Notification /*reason*/, const MSLane* /* enteredLane */) {
    return true; // keep the device
}


bool
MSDevice_Taxi::notifyLeave(SUMOTrafficObject& /*veh*/, double /*lastPos*/, MSMoveReminder::Notification /*reason*/, const MSLane* /* enteredLane */) {
    return true; // keep the device
}

void
MSDevice_Taxi::customerEntered() {
    myState = OCCUPIED;
}


void
MSDevice_Taxi::customerArrived(const MSTransportable* person) {
    myCustomersServed++;
    myCustomers.erase(person);
    if (myHolder.getPersonNumber() == 0) {
        myState = EMPTY;
    }
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
    } else if (key == "occupiedtime") {
        return toString(STEPS2TIME(myOccupiedTime));
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
    UNUSED_PARAMETER(doubleValue);
    throw InvalidArgument("Setting parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


/****************************************************************************/
