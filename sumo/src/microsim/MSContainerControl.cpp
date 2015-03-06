/****************************************************************************/
/// @file    MSContainerControl.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Mon, 16 Jun 2014
/// @version $Id$
///
// Stores all containers in the net and handles their waiting for cars.
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
//#include <algorithm>
#include "MSNet.h"
#include "MSEdge.h"
#include "MSContainer.h"
//#include "MSVehicle.h"
#include "MSContainerControl.h"
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSContainerControl::MSContainerControl() {}


MSContainerControl::~MSContainerControl() {
    for (std::map<std::string, MSContainer*>::iterator i = myContainers.begin(); i != myContainers.end(); ++i) {
        delete(*i).second;
    }
    myContainers.clear();
    myWaiting4Vehicle.clear();
}


bool
MSContainerControl::add(const std::string& id, MSContainer* container) {
    if (myContainers.find(id) == myContainers.end()) {
        myContainers[id] = container;
        return true;
    }
    return false;
}


void
MSContainerControl::erase(MSContainer* container) {
    const std::string& id = container->getID();
    if (OptionsCont::getOptions().isSet("tripinfo-output")) {
        OutputDevice& od = OutputDevice::getDeviceByOption("tripinfo-output");
        od.openTag("containerinfo").writeAttr("id", id).writeAttr("depart", time2string(container->getDesiredDepart()));
        container->tripInfoOutput(od);
        od.closeTag();
    }
    if (OptionsCont::getOptions().isSet("vehroute-output")) {
        OutputDevice& od = OutputDevice::getDeviceByOption("vehroute-output");
        od.openTag("container").writeAttr("id", id).writeAttr("depart", time2string(container->getDesiredDepart())).writeAttr("arrival", time2string(MSNet::getInstance()->getCurrentTimeStep()));
        container->routeOutput(od);
        od.closeTag();
        od << "\n";
    }
    const std::map<std::string, MSContainer*>::iterator i = myContainers.find(id);
    if (i != myContainers.end()) {
        delete i->second;
        myContainers.erase(i);
    }
}


void
MSContainerControl::setDeparture(const SUMOTime time, MSContainer* container) {
    const SUMOTime step = time % DELTA_T == 0 ? time : (time / DELTA_T + 1) * DELTA_T;
    if (myWaiting4Departure.find(step) == myWaiting4Departure.end()) {
        myWaiting4Departure[step] = ContainerVector();
    }
    myWaiting4Departure[step].push_back(container);
}


void
MSContainerControl::setWaitEnd(const SUMOTime time, MSContainer* container) {
    const SUMOTime step = time % DELTA_T == 0 ? time : (time / DELTA_T + 1) * DELTA_T;
    if (myWaitingUntil.find(step) == myWaitingUntil.end()) {
        myWaitingUntil[step] = ContainerVector();
    }
    myWaitingUntil[step].push_back(container);
}


void
MSContainerControl::checkWaitingContainers(MSNet* net, const SUMOTime time) {
    while (myWaiting4Departure.find(time) != myWaiting4Departure.end()) {
        const ContainerVector& containers = myWaiting4Departure[time];
        // we cannot use an iterator here because there might be additions to the vector while proceeding
        for (size_t i = 0; i < containers.size(); ++i) {
            if (!containers[i]->proceed(net, time)) {
                erase(containers[i]);
            }
        }
        myWaiting4Departure.erase(time);
    }
    while (myWaitingUntil.find(time) != myWaitingUntil.end()) {
        const ContainerVector& containers = myWaitingUntil[time];
        // we cannot use an iterator here because there might be additions to the vector while proceeding
        for (size_t i = 0; i < containers.size(); ++i) {
            if (!containers[i]->proceed(net, time)) {
                erase(containers[i]);
            }
        }
        myWaitingUntil.erase(time);
    }
}


void
MSContainerControl::addWaiting(const MSEdge* const edge, MSContainer* container) {
    if (myWaiting4Vehicle.find(edge) == myWaiting4Vehicle.end()) {
        myWaiting4Vehicle[edge] = std::vector<MSContainer*>();
    }
    myWaiting4Vehicle[edge].push_back(container);
}


//bool
//MSContainerControl::isWaiting4Vehicle(const MSEdge* const edge, MSContainer* /* p */) const {
//    return myWaiting4Vehicle.find(edge) != myWaiting4Vehicle.end();
//}


bool
MSContainerControl::loadAnyWaiting(MSEdge* edge, MSVehicle* vehicle, MSVehicle::Stop* stop) {
    bool ret = false;
    if (myWaiting4Vehicle.find(edge) != myWaiting4Vehicle.end()) {
        ContainerVector& waitContainers = myWaiting4Vehicle[edge];
        for (ContainerVector::iterator i = waitContainers.begin(); i != waitContainers.end();) {
            const std::string& line = vehicle->getParameter().line == "" ? vehicle->getParameter().id : vehicle->getParameter().line;
			SUMOTime currentTime =  MSNet::getInstance()->getCurrentTimeStep();
			if ((*i)->isWaitingFor(line) && vehicle->getVehicleType().getContainerCapacity() > vehicle->getContainerNumber() 
                && stop->timeToLoadNextContainer <= currentTime 
                && stop->startPos <= (*i)->getEdgePos() && (*i)->getEdgePos() <= stop->endPos) {
                edge->removeContainer(*i);
                vehicle->addContainer(*i);
				//if the time a container needs to get loaded on the vehicle extends the duration of the stop of the vehicle extend
				//the duration by setting it to the loading duration of the container
				const SUMOTime loadingDuration = vehicle->getVehicleType().getLoadingDuration();
				if (loadingDuration >= stop->duration) {					
					stop->duration = loadingDuration;
				}
				//update the time point at which the next container can be loaded on the vehicle
				stop->timeToLoadNextContainer = currentTime + loadingDuration;

                static_cast<MSContainer::MSContainerStage_Driving*>((*i)->getCurrentStage())->setVehicle(vehicle);
                i = waitContainers.erase(i);
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
MSContainerControl::hasContainers() const {
    return !myContainers.empty();
}


bool
MSContainerControl::hasNonWaiting() const {
    return !myWaiting4Departure.empty() || !myWaitingUntil.empty() || !myTranship.empty();
}


void
MSContainerControl::setTranship(MSContainer* c) {
    myTranship[c->getID()] = c;
}


void
MSContainerControl::unsetTranship(MSContainer* c) {
    std::map<std::string, MSContainer*>::iterator i = myTranship.find(c->getID());
    if (i != myTranship.end()) {
        myTranship.erase(i);
    }
}


void
MSContainerControl::abortWaiting() {
    for (std::map<const MSEdge*, ContainerVector>::const_iterator i = myWaiting4Vehicle.begin(); i != myWaiting4Vehicle.end(); ++i) {
        const MSEdge* edge = (*i).first;
        const ContainerVector& pv = (*i).second;
        for (ContainerVector::const_iterator j = pv.begin(); j != pv.end(); ++j) {
            MSContainer* p = (*j);
            edge->removeContainer(p);
            WRITE_WARNING("Container " + p->getID() + " aborted waiting for a transport that will never come.");
            erase(p);
        }
    }
}


MSContainer*
MSContainerControl::buildContainer(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSContainer::MSContainerPlan* plan) const {
    return new MSContainer(pars, vtype, plan);
}

/****************************************************************************/
