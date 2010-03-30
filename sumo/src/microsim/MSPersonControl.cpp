/****************************************************************************/
/// @file    MSPersonControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Stores all persons in the net and handles their waiting for cars.
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

#include <vector>
#include <algorithm>
#include "MSCORN.h"
#include "MSNet.h"
#include "MSPerson.h"
#include "MSVehicle.h"
#include "MSPersonControl.h"
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSPersonControl::MSPersonControl() {}


MSPersonControl::~MSPersonControl() {
    for (std::map<std::string, MSPerson*>::iterator i=myPersons.begin(); i!=myPersons.end(); ++i) {
        delete(*i).second;
    }
    myPersons.clear();
    myWaiting.clear();
}


bool
MSPersonControl::add(const std::string &id, MSPerson *person) {
    if (myPersons.find(id) == myPersons.end()) {
        myPersons[id] = person;
        return true;
    }
    return false;
}


void
MSPersonControl::erase(MSPerson *person) {
    const std::string &id = person->getID();
    if (MSCORN::wished(MSCORN::CORN_OUT_TRIPDURATIONS)) {
        OutputDevice& od = OutputDevice::getDeviceByOption("tripinfo-output");
        od.openTag("personinfo") << " id=\"" << id << "\" ";
        od << "depart=\"" << person->getDesiredDepart() << "\">\n";
        person->tripInfoOutput(od);
        od.closeTag();
    }
    if (MSCORN::wished(MSCORN::CORN_OUT_VEHROUTES)) {
        OutputDevice& od = OutputDevice::getDeviceByOption("vehroute-output");
        od.openTag("person") << " id=\"" << id
        << "\" depart=\"" << person->getDesiredDepart()
        << "\" arrival=\"" << MSNet::getInstance()->getCurrentTimeStep()
        << "\">\n";
        od.closeTag();
        od << "\n";
    }
    if (myPersons.find(id) != myPersons.end()) {
        delete myPersons[id];
        myPersons.erase(id);
    }
 }

void
MSPersonControl::setArrival(const SUMOTime time, MSPerson *person) {
    const SUMOTime step = ceil(time / DELTA_T) * DELTA_T;
    if (myArrivals.find(step)==myArrivals.end()) {
        myArrivals[step] = PersonVector();
    }
    myArrivals[step].push_back(person);
}


bool
MSPersonControl::hasArrivedPersons(SUMOTime time) const {
    return myArrivals.find(time)!=myArrivals.end();
}


const MSPersonControl::PersonVector
MSPersonControl::popArrivedPersons(SUMOTime time) {
    MSPersonControl::PersonVector arrived = myArrivals[time];
    myArrivals.erase(time);
    return arrived;
}


void
MSPersonControl::addWaiting(const MSEdge* const edge, MSPerson *person) throw() {
    if (myWaiting.find(edge) == myWaiting.end()) {
        myWaiting[edge] = std::vector<MSPerson*>();
    }
    myWaiting[edge].push_back(person);
}


void
MSPersonControl::checkWaiting(const MSEdge* const edge, MSVehicle *vehicle) throw() {
    if (myWaiting.find(edge) != myWaiting.end()) {
        PersonVector &waitPersons = myWaiting[edge];
        for (PersonVector::iterator i=waitPersons.begin(); i!=waitPersons.end(); ) {
            const std::string &line = vehicle->getParameter().line == "" ? vehicle->getParameter().id : vehicle->getParameter().line;
            if ((*i)->isWaitingFor(line)) {
                vehicle->addPerson(*i);
                i = waitPersons.erase(i);
            } else {
                ++i;
            }
        }
    }
}


/****************************************************************************/
