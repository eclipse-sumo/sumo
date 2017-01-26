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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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
MSDevice_Transportable::notifyMove(SUMOVehicle& veh, SUMOReal /*oldPos*/, SUMOReal /*newPos*/, SUMOReal /*newSpeed*/) {
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
MSDevice_Transportable::notifyEnter(SUMOVehicle& /*veh*/, MSMoveReminder::Notification reason) {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
        for (std::vector<MSTransportable*>::iterator i = myTransportables.begin(); i != myTransportables.end(); ++i) {
            (*i)->setDeparted(MSNet::getInstance()->getCurrentTimeStep());
        }
    }
    return true;
}


bool
MSDevice_Transportable::notifyLeave(SUMOVehicle& veh, SUMOReal /*lastPos*/,
                                    MSMoveReminder::Notification reason) {
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


/****************************************************************************/

