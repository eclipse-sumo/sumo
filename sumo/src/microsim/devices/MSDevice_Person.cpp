/****************************************************************************/
/// @file    MSDevice_Person.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which is used to keep track of Persons riding with a vehicle
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include <utils/iodevices/OutputDevice.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSPerson.h>
#include <microsim/MSPersonControl.h>
#include "MSDevice_Person.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
MSDevice_Person*
MSDevice_Person::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    MSDevice_Person* device = new MSDevice_Person(v, "person_" + v.getID());
    into.push_back(device);
    return device;
}


// ---------------------------------------------------------------------------
// MSDevice_Person-methods
// ---------------------------------------------------------------------------
MSDevice_Person::MSDevice_Person(SUMOVehicle& holder, const std::string& id)
    : MSDevice(holder, id), myPersons(), myStopped(holder.isStopped()) {
}


MSDevice_Person::~MSDevice_Person() {
}


bool
MSDevice_Person::notifyMove(SUMOVehicle& veh, SUMOReal /*oldPos*/, SUMOReal /*newPos*/, SUMOReal /*newSpeed*/) {
    if (myStopped) {
        if (!veh.isStopped()) {
            for (std::vector<MSPerson*>::iterator i = myPersons.begin(); i != myPersons.end(); ++i) {
                (*i)->setDeparted(MSNet::getInstance()->getCurrentTimeStep());
            }
            myStopped = false;
        }
    } else {
        if (veh.isStopped()) {
            for (std::vector<MSPerson*>::iterator i = myPersons.begin(); i != myPersons.end();) {
                MSPerson* person = *i;
                if (&(person->getDestination()) == veh.getEdge()) {
                    if (!person->proceed(MSNet::getInstance(), MSNet::getInstance()->getCurrentTimeStep())) {
                        MSNet::getInstance()->getPersonControl().erase(person);
                    }
                    i = myPersons.erase(i);
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
MSDevice_Person::notifyEnter(SUMOVehicle& /*veh*/, MSMoveReminder::Notification reason) {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
        for (std::vector<MSPerson*>::iterator i = myPersons.begin(); i != myPersons.end(); ++i) {
            (*i)->setDeparted(MSNet::getInstance()->getCurrentTimeStep());
        }
    }
    return true;
}


bool
MSDevice_Person::notifyLeave(SUMOVehicle& veh, SUMOReal /*lastPos*/,
                             MSMoveReminder::Notification reason) {
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED) {
        for (std::vector<MSPerson*>::iterator i = myPersons.begin(); i != myPersons.end(); ++i) {
            MSPerson* person = *i;
            if (&(person->getDestination()) != veh.getEdge()) {
                WRITE_WARNING("Teleporting person '" + person->getID() +
                              "' from vehicle destination '" + veh.getEdge()->getID() +
                              "' to intended destination '" + person->getDestination().getID() + "'");
            }
            if (!person->proceed(MSNet::getInstance(), MSNet::getInstance()->getCurrentTimeStep())) {
                MSNet::getInstance()->getPersonControl().erase(person);
            };
        }
    }
    return true;
}


void
MSDevice_Person::addPerson(MSPerson* person) {
    myPersons.push_back(person);
}


/****************************************************************************/

