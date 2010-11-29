/****************************************************************************/
/// @file    MSDevice_Person.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which collects vehicular emissions (using HBEFA-reformulation)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include "MSDevice_Person.h"
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSPerson.h>
#include <utils/iodevices/OutputDevice.h>

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
MSDevice_Person::buildVehicleDevices(SUMOVehicle &v, std::vector<MSDevice*> &into) throw() {
    MSDevice_Person* device = new MSDevice_Person(v, "person_" + v.getID());
    into.push_back(device);
    return device;
}


// ---------------------------------------------------------------------------
// MSDevice_Person-methods
// ---------------------------------------------------------------------------
MSDevice_Person::MSDevice_Person(SUMOVehicle &holder, const std::string &id) throw()
      : MSDevice(holder, id), myPersons(), myStopped(holder.isStopped()) {
}


MSDevice_Person::~MSDevice_Person() throw() {
}


bool
MSDevice_Person::notifyMove(SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    if (myStopped) {
        if (!veh.isStopped()) {
            for (std::vector<MSPerson*>::iterator i=myPersons.begin(); i!=myPersons.end(); ++i) {
                (*i)->setDeparted(MSNet::getInstance()->getCurrentTimeStep());
            }
            myStopped = false;
        }
    } else {
        if (veh.isStopped()) {
            for (std::vector<MSPerson*>::iterator i=myPersons.begin(); i!=myPersons.end();) {
                if (&(*i)->getDestination() == veh.getEdge()) {
                    (*i)->proceed(MSNet::getInstance(), MSNet::getInstance()->getCurrentTimeStep());
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
MSDevice_Person::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason) throw() {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
        for (std::vector<MSPerson*>::iterator i=myPersons.begin(); i!=myPersons.end(); ++i) {
            (*i)->setDeparted(MSNet::getInstance()->getCurrentTimeStep());
        }
    }
    return true;
}


bool
MSDevice_Person::notifyLeave(SUMOVehicle& veh, SUMOReal lastPos,
                             MSMoveReminder::Notification reason) throw() {
    if (reason == MSMoveReminder::NOTIFICATION_ARRIVED) {
        for (std::vector<MSPerson*>::iterator i=myPersons.begin(); i!=myPersons.end(); ++i) {
            (*i)->proceed(MSNet::getInstance(), MSNet::getInstance()->getCurrentTimeStep());
        }
    }
    return true;
}


void
MSDevice_Person::addPerson(MSPerson* person) throw() {
    myPersons.push_back(person);
}


/****************************************************************************/

