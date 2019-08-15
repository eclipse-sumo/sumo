/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSTransportableControl.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Stores all persons in the net and handles their waiting for cars.
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <algorithm>
#include "MSNet.h"
#include "MSEdge.h"
#include <microsim/pedestrians/MSPerson.h>
#include "MSContainer.h"
#include "MSVehicle.h"
#include "MSTransportableControl.h"
#include <utils/iodevices/OutputDevice.h>
#include <utils/iodevices/OutputDevice_String.h>
#include <utils/options/OptionsCont.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSTransportableControl::MSTransportableControl():
    myLoadedNumber(0),
    myRunningNumber(0),
    myJammedNumber(0),
    myWaitingForVehicleNumber(0),
    myHaveNewWaiting(false) {
}


MSTransportableControl::~MSTransportableControl() {
    for (std::map<std::string, MSTransportable*>::iterator i = myTransportables.begin(); i != myTransportables.end(); ++i) {
        delete (*i).second;
    }
    myTransportables.clear();
    myWaiting4Vehicle.clear();
}


bool
MSTransportableControl::add(MSTransportable* transportable) {
    const SUMOVehicleParameter& param = transportable->getParameter();
    if (myTransportables.find(param.id) == myTransportables.end()) {
        myTransportables[param.id] = transportable;
        const SUMOTime step = param.depart % DELTA_T == 0 ? param.depart : (param.depart / DELTA_T + 1) * DELTA_T;
        myWaiting4Departure[step].push_back(transportable);
        myLoadedNumber++;
        return true;
    }
    return false;
}


MSTransportable*
MSTransportableControl::get(const std::string& id) const {
    std::map<std::string, MSTransportable*>::const_iterator i = myTransportables.find(id);
    if (i == myTransportables.end()) {
        return nullptr;
    }
    return (*i).second;
}


void
MSTransportableControl::erase(MSTransportable* transportable) {
    if (OptionsCont::getOptions().isSet("tripinfo-output")) {
        transportable->tripInfoOutput(OutputDevice::getDeviceByOption("tripinfo-output"));
    } else if (OptionsCont::getOptions().getBool("duration-log.statistics")) {
        // collecting statistics is a sideffect
        OutputDevice_String dev;
        transportable->tripInfoOutput(dev);
    }
    if (OptionsCont::getOptions().isSet("vehroute-output")) {
        transportable->routeOutput(OutputDevice::getDeviceByOption("vehroute-output"), OptionsCont::getOptions().getBool("vehroute-output.route-length"));
    }
    const std::map<std::string, MSTransportable*>::iterator i = myTransportables.find(transportable->getID());
    if (i != myTransportables.end()) {
        myRunningNumber--;
        delete i->second;
        myTransportables.erase(i);
    }
}


void
MSTransportableControl::setWaitEnd(const SUMOTime time, MSTransportable* transportable) {
    const SUMOTime step = time % DELTA_T == 0 ? time : (time / DELTA_T + 1) * DELTA_T;
    // avoid double registration
    const TransportableVector& transportables = myWaiting4Departure[step];
    if (std::find(transportables.begin(), transportables.end(), transportable) == transportables.end()) {
        myWaitingUntil[step].push_back(transportable);
    }
}


void
MSTransportableControl::checkWaiting(MSNet* net, const SUMOTime time) {
    myHaveNewWaiting = false;
    while (myWaiting4Departure.find(time) != myWaiting4Departure.end()) {
        const TransportableVector& transportables = myWaiting4Departure[time];
        // we cannot use an iterator here because there might be additions to the vector while proceeding
        for (int i = 0; i < (int)transportables.size(); ++i) {
            if (transportables[i]->proceed(net, time)) {
                myRunningNumber++;
            } else {
                erase(transportables[i]);
            }
        }
        myWaiting4Departure.erase(time);
    }
    while (myWaitingUntil.find(time) != myWaitingUntil.end()) {
        const TransportableVector& transportables = myWaitingUntil[time];
        // we cannot use an iterator here because there might be additions to the vector while proceeding
        for (int i = 0; i < (int)transportables.size(); ++i) {
            if (!transportables[i]->proceed(net, time)) {
                erase(transportables[i]);
            }
        }
        myWaitingUntil.erase(time);
    }
}


void
MSTransportableControl::addWaiting(const MSEdge* const edge, MSTransportable* transportable) {
    myWaiting4Vehicle[edge].push_back(transportable);
    myWaitingForVehicleNumber++;
    myHaveNewWaiting = true;
}


bool
MSTransportableControl::boardAnyWaiting(MSEdge* edge, SUMOVehicle* vehicle, const SUMOVehicleParameter::Stop& stop, SUMOTime& timeToBoardNextPerson, SUMOTime& stopDuration) {
    bool ret = false;
    if (myWaiting4Vehicle.find(edge) != myWaiting4Vehicle.end()) {
        TransportableVector& wait = myWaiting4Vehicle[edge];
        SUMOTime currentTime =  MSNet::getInstance()->getCurrentTimeStep();
        for (TransportableVector::iterator i = wait.begin(); i != wait.end();) {
            if ((*i)->isWaitingFor(vehicle)
                    && vehicle->getVehicleType().getPersonCapacity() > vehicle->getPersonNumber()
                    && timeToBoardNextPerson <= currentTime
                    && stop.startPos <= (*i)->getEdgePos()
                    && (*i)->getEdgePos() <= stop.endPos) {
                edge->removePerson(*i);
                vehicle->addPerson(*i);
                if (timeToBoardNextPerson >= 0) { // meso does not have boarding times
                    //if the time a person needs to enter the vehicle extends the duration of the stop of the vehicle extend
                    //the duration by setting it to the boarding duration of the person
                    const SUMOTime boardingDuration = vehicle->getVehicleType().getBoardingDuration();
                    if (boardingDuration >= stopDuration) {
                        stopDuration = boardingDuration;
                    }
                    //update the time point at which the next person can board the vehicle
                    if (timeToBoardNextPerson > currentTime - DELTA_T) {
                        timeToBoardNextPerson += boardingDuration;
                    } else {
                        timeToBoardNextPerson = currentTime + boardingDuration;
                    }
                }

                static_cast<MSTransportable::Stage_Driving*>((*i)->getCurrentStage())->setVehicle(vehicle);
                i = wait.erase(i);
                myWaitingForVehicleNumber--;
                ret = true;
            } else {
                ++i;
            }
        }
        if (wait.size() == 0) {
            myWaiting4Vehicle.erase(myWaiting4Vehicle.find(edge));
        }
    }
    return ret;
}


bool
MSTransportableControl::loadAnyWaiting(MSEdge* edge, SUMOVehicle* vehicle, const SUMOVehicleParameter::Stop& stop, SUMOTime& timeToLoadNextContainer, SUMOTime& stopDuration) {
    bool ret = false;
    if (myWaiting4Vehicle.find(edge) != myWaiting4Vehicle.end()) {
        TransportableVector& waitContainers = myWaiting4Vehicle[edge];
        for (TransportableVector::iterator i = waitContainers.begin(); i != waitContainers.end();) {
            SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
            if ((*i)->isWaitingFor(vehicle)
                    && vehicle->getVehicleType().getContainerCapacity() > vehicle->getContainerNumber()
                    && timeToLoadNextContainer <= currentTime
                    && stop.startPos <= (*i)->getEdgePos()
                    && (*i)->getEdgePos() <= stop.endPos) {
                edge->removeContainer(*i);
                vehicle->addContainer(*i);
                //if the time a container needs to get loaded on the vehicle extends the duration of the stop of the vehicle extend
                //the duration by setting it to the loading duration of the container
                const SUMOTime loadingDuration = vehicle->getVehicleType().getLoadingDuration();
                if (loadingDuration >= stopDuration) {
                    stopDuration = loadingDuration;
                }
                //update the time point at which the next container can be loaded on the vehicle
                timeToLoadNextContainer = currentTime + loadingDuration;

                static_cast<MSContainer::MSContainerStage_Driving*>((*i)->getCurrentStage())->setVehicle(vehicle);
                i = waitContainers.erase(i);
                myWaitingForVehicleNumber--;
                ret = true;
            } else {
                ++i;
            }
        }
        if (waitContainers.size() == 0) {
            myWaiting4Vehicle.erase(myWaiting4Vehicle.find(edge));
        }
    }
    return ret;
}


bool
MSTransportableControl::hasTransportables() const {
    return !myTransportables.empty();
}


bool
MSTransportableControl::hasNonWaiting() const {
    return !myWaiting4Departure.empty() || myWaitingForVehicleNumber < myRunningNumber || myHaveNewWaiting;
}


int
MSTransportableControl::getActiveCount() {
    return (int)myWaiting4Departure.size() + myRunningNumber - myWaitingForVehicleNumber;
}


void
MSTransportableControl::abortAnyWaitingForVehicle() {
    for (std::map<const MSEdge*, TransportableVector>::const_iterator i = myWaiting4Vehicle.begin(); i != myWaiting4Vehicle.end(); ++i) {
        const MSEdge* edge = (*i).first;
        const TransportableVector& pv = (*i).second;
        for (TransportableVector::const_iterator j = pv.begin(); j != pv.end(); ++j) {
            MSTransportable* p = (*j);
            std::string transportableType;
            if (dynamic_cast<MSPerson*>(p) != nullptr) {
                edge->removePerson(p);
                transportableType = "Person";
            } else {
                transportableType = "Container";
                edge->removeContainer(p);
            }
            MSTransportable::Stage_Driving* stage = dynamic_cast<MSTransportable::Stage_Driving*>(p->getCurrentStage());
            const std::string waitDescription = stage == nullptr ? "waiting" : stage->getWaitingDescription();
            WRITE_WARNING(transportableType + " '" + p->getID() + "' aborted " + waitDescription + ".");
            erase(p);
        }
    }
}

void
MSTransportableControl::abortWaitingForVehicle(MSTransportable* t) {
    const MSEdge* edge = t->getEdge();
    auto it = myWaiting4Vehicle.find(edge);
    if (it != myWaiting4Vehicle.end()) {
        TransportableVector& waiting = it->second;
        auto it2 = std::find(waiting.begin(), waiting.end(), t);
        if (it2 != waiting.end()) {
            waiting.erase(it2);
        }
    }
}

void
MSTransportableControl::abortWaiting(MSTransportable* t) {
    for (std::map<SUMOTime, TransportableVector>::iterator it = myWaiting4Departure.begin(); it != myWaiting4Departure.end(); ++it) {
        TransportableVector& ts = it->second;
        TransportableVector::iterator it2 = std::find(ts.begin(), ts.end(), t);
        if (it2 != ts.end()) {
            ts.erase(it2);
        }
    }
    for (std::map<SUMOTime, TransportableVector>::iterator it = myWaitingUntil.begin(); it != myWaitingUntil.end(); ++it) {
        TransportableVector& ts = it->second;
        TransportableVector::iterator it2 = std::find(ts.begin(), ts.end(), t);
        if (it2 != ts.end()) {
            ts.erase(it2);
        }
    }
}


MSTransportable*
MSTransportableControl::buildPerson(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan,
                                    std::mt19937* rng) const {
    const double speedFactor = vtype->computeChosenSpeedDeviation(rng);
    return new MSPerson(pars, vtype, plan, speedFactor);
}


MSTransportable*
MSTransportableControl::buildContainer(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan) const {
    return new MSContainer(pars, vtype, plan);
}


/****************************************************************************/
