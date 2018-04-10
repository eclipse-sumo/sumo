/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDevice_Transportable.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which is used to keep track of persons and containers riding with a vehicle
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/output/MSStopOut.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/pedestrians/MSPerson.h>
#include <microsim/MSTransportableControl.h>
#include <microsim/MSContainer.h>
#include "MSDevice_Transportable.h"


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
MSDevice_Transportable*
MSDevice_Transportable::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into, const bool isContainer) {
    MSDevice_Transportable* device = new MSDevice_Transportable(v, isContainer ? "container_" + v.getID() : "person_" + v.getID(), isContainer);
    into.push_back(device);
    return device;
}


// ---------------------------------------------------------------------------
// MSDevice_Transportable-methods
// ---------------------------------------------------------------------------
MSDevice_Transportable::MSDevice_Transportable(SUMOVehicle& holder, const std::string& id, const bool isContainer)
    : MSDevice(holder, id), myAmContainer(isContainer), myTransportables(), myStopped(holder.isStopped()) {
}


MSDevice_Transportable::~MSDevice_Transportable() {
}


bool
MSDevice_Transportable::notifyMove(SUMOVehicle& veh, double /*oldPos*/, double /*newPos*/, double /*newSpeed*/) {
    if (myStopped) {
        if (!veh.isStopped()) {
            for (std::vector<MSTransportable*>::iterator i = myTransportables.begin(); i != myTransportables.end(); ++i) {
                (*i)->setDeparted(MSNet::getInstance()->getCurrentTimeStep());
            }
            myStopped = false;
        }
    } else {
        if (veh.isStopped()) {
            for (std::vector<MSTransportable*>::iterator i = myTransportables.begin(); i != myTransportables.end();) {
                MSTransportable* transportable = *i;
                if (&(transportable->getDestination()) == veh.getEdge()) {
                    if (!transportable->proceed(MSNet::getInstance(), MSNet::getInstance()->getCurrentTimeStep())) {
                        if (myAmContainer) {
                            MSNet::getInstance()->getContainerControl().erase(transportable);
                        } else {
                            MSNet::getInstance()->getPersonControl().erase(transportable);
                        }
                    }
                    if (MSStopOut::active()) {
                        if (myAmContainer) {
                            MSStopOut::getInstance()->unloadedContainers(&veh, 1);
                        } else {
                            MSStopOut::getInstance()->unloadedPersons(&veh, 1);
                        }
                    }
                    i = myTransportables.erase(i);
                } else {
                    ++i;
                }
            }
            myStopped = true;
        }
    }
    return true;
}


bool
MSDevice_Transportable::notifyEnter(SUMOVehicle& /*veh*/, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
        for (std::vector<MSTransportable*>::iterator i = myTransportables.begin(); i != myTransportables.end(); ++i) {
            (*i)->setDeparted(MSNet::getInstance()->getCurrentTimeStep());
        }
    }
    return true;
}


bool
MSDevice_Transportable::notifyLeave(SUMOVehicle& veh, double /*lastPos*/,
                                    MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED) {
        for (std::vector<MSTransportable*>::iterator i = myTransportables.begin(); i != myTransportables.end(); ++i) {
            MSTransportable* transportable = *i;
            if (&(transportable->getDestination()) != veh.getEdge()) {
                WRITE_WARNING((myAmContainer ? "Teleporting container '" : "Teleporting person '") + transportable->getID() +
                              "' from vehicle destination edge '" + veh.getEdge()->getID() +
                              "' to intended destination edge '" + transportable->getDestination().getID() + "'");
            }
            if (!transportable->proceed(MSNet::getInstance(), MSNet::getInstance()->getCurrentTimeStep())) {
                if (myAmContainer) {
                    MSNet::getInstance()->getContainerControl().erase(transportable);
                } else {
                    MSNet::getInstance()->getPersonControl().erase(transportable);
                }
            }
        }
    }
    return true;
}


void
MSDevice_Transportable::addTransportable(MSTransportable* transportable) {
    myTransportables.push_back(transportable);
    if (MSStopOut::active()) {
        if (myAmContainer) {
            MSStopOut::getInstance()->loadedContainers(&myHolder, 1);
        } else {
            MSStopOut::getInstance()->loadedPersons(&myHolder, 1);
        }
    }
}


void
MSDevice_Transportable::removeTransportable(MSTransportable* transportable) {
    myTransportables.erase(std::find(myTransportables.begin(), myTransportables.end(), transportable));
    if (MSStopOut::active() && myHolder.isStopped()) {
        if (myAmContainer) {
            MSStopOut::getInstance()->loadedContainers(&myHolder, 1);
        } else {
            MSStopOut::getInstance()->loadedPersons(&myHolder, 1);
        }
    }
}


std::string
MSDevice_Transportable::getParameter(const std::string& key) const {
    if (key == "IDList") {
        std::vector<std::string> ids;
        for (std::vector<MSTransportable*>::const_iterator i = myTransportables.begin(); i != myTransportables.end(); ++i) {
            ids.push_back((*i)->getID());
        }
        return toString(ids);
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


/****************************************************************************/

