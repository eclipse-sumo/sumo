/****************************************************************************/
/// @file    MSDevice_Container.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Mon, 16 Jun 2014
/// @version $Id$
///
// A device which is used to keep track of containers riding with a vehicle
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

//#include <utils/iodevices/OutputDevice.h>
#include <microsim/MSNet.h>
//#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSContainer.h>
#include <microsim/trigger/MSContainerStop.h>
#include <microsim/MSContainerControl.h>
#include "MSDevice_Container.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
MSDevice_Container*
MSDevice_Container::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    MSDevice_Container* device = new MSDevice_Container(v, "container_" + v.getID());
    into.push_back(device);
    return device;
}


// ---------------------------------------------------------------------------
// MSDevice_Container-methods
// ---------------------------------------------------------------------------
MSDevice_Container::MSDevice_Container(SUMOVehicle& holder, const std::string& id)
    : MSDevice(holder, id), myContainers(), myStopped(holder.isStopped()) {
}


MSDevice_Container::~MSDevice_Container() {
}


bool
MSDevice_Container::notifyMove(SUMOVehicle& veh, SUMOReal /*oldPos*/, SUMOReal /*newPos*/, SUMOReal /*newSpeed*/) {
    if (myStopped) {
        // if veh is not anymore at the stop
        if (!veh.isStopped()) {
            for (std::vector<MSContainer*>::iterator i = myContainers.begin(); i != myContainers.end(); ++i) {
                (*i)->setDeparted(MSNet::getInstance()->getCurrentTimeStep());
            }
            myStopped = false;
        }
    } else {
        if (veh.isStopped()) {
            for (std::vector<MSContainer*>::iterator i = myContainers.begin(); i != myContainers.end();) {
                MSContainer* container = *i;
                const MSContainerStop* currentDestinationStop = container->getDestinationContainerStop();
                if (&(container->getDestination()) == veh.getEdge()){
                    if (!container->proceed(MSNet::getInstance(), MSNet::getInstance()->getCurrentTimeStep())) {
                        MSNet::getInstance()->getContainerControl().erase(container);
                    }
                    i = myContainers.erase(i);
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
MSDevice_Container::notifyEnter(SUMOVehicle& /*veh*/, MSMoveReminder::Notification reason) {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
        for (std::vector<MSContainer*>::iterator i = myContainers.begin(); i != myContainers.end(); ++i) {
            (*i)->setDeparted(MSNet::getInstance()->getCurrentTimeStep());
        }
    }
    return true;
}


bool
MSDevice_Container::notifyLeave(SUMOVehicle& veh, SUMOReal /*lastPos*/,
                             MSMoveReminder::Notification reason) {
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED) {
        for (std::vector<MSContainer*>::iterator i = myContainers.begin(); i != myContainers.end(); ++i) {
            MSContainer* container = *i;
            if (&(container->getDestination()) != veh.getEdge()) {
                WRITE_WARNING("Teleporting container '" + container->getID() +
                              "' from vehicle destination '" + veh.getEdge()->getID() +
                              "' to intended destination '" + container->getDestination().getID() + "'");
            }
            if (!container->proceed(MSNet::getInstance(), MSNet::getInstance()->getCurrentTimeStep())) {
                MSNet::getInstance()->getContainerControl().erase(container);
            };
        }
    }
    return true;
}


void
MSDevice_Container::addContainer(MSContainer* container) {
    myContainers.push_back(container);
}


/****************************************************************************/

