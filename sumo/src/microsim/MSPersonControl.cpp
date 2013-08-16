/****************************************************************************/
/// @file    MSPersonControl.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Stores all persons in the net and handles their waiting for cars.
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

#include <vector>
#include <algorithm>
#include "MSNet.h"
#include "MSEdge.h"
#include "MSPerson.h"
#include "MSVehicle.h"
#include "MSPersonControl.h"
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSPersonControl::MSPersonControl() {}


MSPersonControl::~MSPersonControl() {
    for (std::map<std::string, MSPerson*>::iterator i = myPersons.begin(); i != myPersons.end(); ++i) {
        delete(*i).second;
    }
    myPersons.clear();
    myWaiting4Vehicle.clear();
}


bool
MSPersonControl::add(const std::string& id, MSPerson* person) {
    if (myPersons.find(id) == myPersons.end()) {
        myPersons[id] = person;
        return true;
    }
    return false;
}


void
MSPersonControl::erase(MSPerson* person) {
    const std::string& id = person->getID();
    if (OptionsCont::getOptions().isSet("tripinfo-output")) {
        OutputDevice& od = OutputDevice::getDeviceByOption("tripinfo-output");
        od.openTag("personinfo") << " id=\"" << id << "\" ";
        od << "depart=\"" << time2string(person->getDesiredDepart()) << "\"";
        person->tripInfoOutput(od);
        od.closeTag();
    }
    if (OptionsCont::getOptions().isSet("vehroute-output")) {
        OutputDevice& od = OutputDevice::getDeviceByOption("vehroute-output");
        od.openTag("person") << " id=\"" << id
                             << "\" depart=\"" << time2string(person->getDesiredDepart())
                             << "\" arrival=\"" << time2string(MSNet::getInstance()->getCurrentTimeStep())
                             << "\"";
        person->routeOutput(od);
        od.closeTag();
        od << "\n";
    }
    const std::map<std::string, MSPerson*>::iterator i = myPersons.find(id);
    if (i != myPersons.end()) {
        delete i->second;
        myPersons.erase(i);
    }
}


void
MSPersonControl::setDeparture(const SUMOTime time, MSPerson* person) {
    const SUMOTime step = time % DELTA_T == 0 ? time : (time / DELTA_T + 1) * DELTA_T;
    if (myWaiting4Departure.find(step) == myWaiting4Departure.end()) {
        myWaiting4Departure[step] = PersonVector();
    }
    myWaiting4Departure[step].push_back(person);
}


void
MSPersonControl::setWaitEnd(const SUMOTime time, MSPerson* person) {
    const SUMOTime step = time % DELTA_T == 0 ? time : (time / DELTA_T + 1) * DELTA_T;
    if (myWaitingUntil.find(step) == myWaitingUntil.end()) {
        myWaitingUntil[step] = PersonVector();
    }
    myWaitingUntil[step].push_back(person);
}


void
MSPersonControl::checkWaitingPersons(MSNet* net, const SUMOTime time) {
    while (myWaiting4Departure.find(time) != myWaiting4Departure.end()) {
        const PersonVector& persons = myWaiting4Departure[time];
        // we cannot use an iterator here because there might be additions to the vector while proceeding
        for (size_t i = 0; i < persons.size(); ++i) {
            if (!persons[i]->proceed(net, time)) {
                erase(persons[i]);
            }
        }
        myWaiting4Departure.erase(time);
    }
    while (myWaitingUntil.find(time) != myWaitingUntil.end()) {
        const PersonVector& persons = myWaitingUntil[time];
        // we cannot use an iterator here because there might be additions to the vector while proceeding
        for (size_t i = 0; i < persons.size(); ++i) {
            if (!persons[i]->proceed(net, time)) {
                erase(persons[i]);
            }
        }
        myWaitingUntil.erase(time);
    }
}


void
MSPersonControl::addWaiting(const MSEdge* const edge, MSPerson* person) {
    if (myWaiting4Vehicle.find(edge) == myWaiting4Vehicle.end()) {
        myWaiting4Vehicle[edge] = std::vector<MSPerson*>();
    }
    myWaiting4Vehicle[edge].push_back(person);
}


bool
MSPersonControl::isWaiting4Vehicle(const MSEdge* const edge, MSPerson* /* p */) const {
    return myWaiting4Vehicle.find(edge) != myWaiting4Vehicle.end();
}


bool
MSPersonControl::boardAnyWaiting(MSEdge* edge, MSVehicle* vehicle) {
    bool ret = false;
    if (myWaiting4Vehicle.find(edge) != myWaiting4Vehicle.end()) {
        PersonVector& waitPersons = myWaiting4Vehicle[edge];
        for (PersonVector::iterator i = waitPersons.begin(); i != waitPersons.end();) {
            const std::string& line = vehicle->getParameter().line == "" ? vehicle->getParameter().id : vehicle->getParameter().line;
            if ((*i)->isWaitingFor(line)) {
                edge->removePerson(*i);
                vehicle->addPerson(*i);
                static_cast<MSPerson::MSPersonStage_Driving*>((*i)->getCurrentStage())->setVehicle(vehicle);
                i = waitPersons.erase(i);
                ret = true;
            } else {
                ++i;
            }
        }
        if (waitPersons.size() == 0) {
            myWaiting4Vehicle.erase(myWaiting4Vehicle.find(edge));
        }
    }
    return ret;
}


bool
MSPersonControl::hasPersons() const {
    return !myPersons.empty();
}


bool
MSPersonControl::hasNonWaiting() const {
    return !myWaiting4Departure.empty() || !myWaitingUntil.empty() || !myWalking.empty();
}


void
MSPersonControl::setWalking(MSPerson* p) {
    myWalking[p->getID()] = p;
}


void
MSPersonControl::unsetWalking(MSPerson* p) {
    std::map<std::string, MSPerson*>::iterator i = myWalking.find(p->getID());
    if (i != myWalking.end()) {
        myWalking.erase(i);
    }
}


void
MSPersonControl::abortWaiting() {
    for (std::map<const MSEdge*, PersonVector>::const_iterator i = myWaiting4Vehicle.begin(); i != myWaiting4Vehicle.end(); ++i) {
        const MSEdge* edge = (*i).first;
        const PersonVector& pv = (*i).second;
        for (PersonVector::const_iterator j = pv.begin(); j != pv.end(); ++j) {
            MSPerson* p = (*j);
            edge->removePerson(p);
            WRITE_WARNING("Person " + p->getID() + " aborted waiting for a ride that will never come.");
            erase(p);
        }
    }
}


MSPerson*
MSPersonControl::buildPerson(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSPerson::MSPersonPlan* plan) const {
    return new MSPerson(pars, vtype, plan);
}

/****************************************************************************/
