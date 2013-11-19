/****************************************************************************/
/// @file    MSInsertionControl.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Inserts vehicles into the network when their departure time is reached
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

#include <iostream>
#include <algorithm>
#include <cassert>
#include <iterator>
#include "MSInsertionControl.h"
#include "MSVehicle.h"
#include "MSLane.h"
#include "MSEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MSInsertionControl::MSInsertionControl(MSVehicleControl& vc,
                                       SUMOTime maxDepartDelay,
                                       bool checkEdgesOnce)
    : myVehicleControl(vc), myMaxDepartDelay(maxDepartDelay),
      myCheckEdgesOnce(checkEdgesOnce) {}


MSInsertionControl::~MSInsertionControl() {
    for (std::vector<Flow>::iterator i = myFlows.begin(); i != myFlows.end(); ++i) {
        delete(i->pars);
    }
}


void
MSInsertionControl::add(SUMOVehicle* veh) {
    myAllVeh.add(veh);
}


void
MSInsertionControl::add(SUMOVehicleParameter* pars) {
    Flow flow;
    flow.pars = pars;
    flow.isVolatile = pars->departLaneProcedure == DEPART_LANE_RANDOM ||
                      pars->departPosProcedure == DEPART_POS_RANDOM ||
                      MSNet::getInstance()->getVehicleControl().hasVTypeDistribution(pars->vtypeid);
    if (!flow.isVolatile) {
        const RandomDistributor<const MSRoute*>* dist = MSRoute::distDictionary(pars->routeid);
        if (dist != 0) {
            const std::vector<const MSRoute*>& routes = dist->getVals();
            const MSEdge* e = 0;
            for (std::vector<const MSRoute*>::const_iterator i = routes.begin(); i != routes.end(); ++i) {
                if (e == 0) {
                    e = (*i)->getEdges()[0];
                } else {
                    if (e != (*i)->getEdges()[0]) {
                        flow.isVolatile = true;
                        break;
                    }
                }
            }
        }
    }
    flow.vehicle = 0;
    myFlows.push_back(flow);
}


unsigned int
MSInsertionControl::emitVehicles(SUMOTime time) {
    checkPrevious(time);
    // check whether any vehicles shall be emitted within this time step
    if (!myAllVeh.anyWaitingFor(time) && myRefusedEmits1.empty() && myRefusedEmits2.empty() && myFlows.empty()) {
        return 0;
    }
    unsigned int noEmitted = 0;
    // we use buffering for the refused emits to save time
    //  for this, we have two lists; one contains previously refused emits, the second
    //  will be used to append those vehicles that will not be able to depart in this
    //  time step
    assert(myRefusedEmits1.size() == 0 || myRefusedEmits2.size() == 0);
    MSVehicleContainer::VehicleVector& refusedEmits =
        myRefusedEmits1.size() == 0 ? myRefusedEmits1 : myRefusedEmits2;
    MSVehicleContainer::VehicleVector& previousRefused =
        myRefusedEmits2.size() == 0 ? myRefusedEmits1 : myRefusedEmits2;

    // go through the list of previously refused vehicles, first
    MSVehicleContainer::VehicleVector::const_iterator veh;
    for (veh = previousRefused.begin(); veh != previousRefused.end(); veh++) {
        noEmitted += tryInsert(time, *veh, refusedEmits);
    }
    // clear previously refused vehicle container
    previousRefused.clear();

    // Insert vehicles from myTrips into the net until the next vehicle's
    //  departure time is greater than the current time.
    // Retrieve the list of vehicles to emit within this time step

    noEmitted += checkFlows(time, refusedEmits);
    while (myAllVeh.anyWaitingFor(time)) {
        const MSVehicleContainer::VehicleVector& next = myAllVeh.top();
        // go through the list and try to emit
        for (veh = next.begin(); veh != next.end(); veh++) {
            noEmitted += tryInsert(time, *veh, refusedEmits);
        }
        // let the MSVehicleContainer clear the vehicles
        myAllVeh.pop();
    }
    // Return the number of emitted vehicles
    return noEmitted;
}


unsigned int
MSInsertionControl::tryInsert(SUMOTime time, SUMOVehicle* veh,
                              MSVehicleContainer::VehicleVector& refusedEmits) {
    assert(veh->getParameter().depart < time + DELTA_T);
    const MSEdge& edge = *veh->getEdge();
    if ((!myCheckEdgesOnce || edge.getLastFailedInsertionTime() != time) && edge.insertVehicle(*veh, time)) {
        // Successful emission.
        checkFlowWait(veh);
        veh->onDepart();
        return 1;
    }
    if (myMaxDepartDelay >= 0 && time - veh->getParameter().depart > myMaxDepartDelay) {
        // remove vehicles waiting too long for departure
        checkFlowWait(veh);
        myVehicleControl.deleteVehicle(veh, true);
    } else if (edge.isVaporizing()) {
        // remove vehicles if the edge shall be empty
        checkFlowWait(veh);
        myVehicleControl.deleteVehicle(veh, true);
    } else if (myAbortedEmits.count(veh) > 0) {
        // remove vehicles which shall not be inserted for some reason
        myAbortedEmits.erase(veh);
        checkFlowWait(veh);
        myVehicleControl.deleteVehicle(veh, true);
    } else {
        // let the vehicle wait one step, we'll retry then
        refusedEmits.push_back(veh);
    }
    edge.setLastFailedInsertionTime(time);
    return 0;
}


void
MSInsertionControl::checkFlowWait(SUMOVehicle* veh) {
    for (std::vector<Flow>::iterator i = myFlows.begin(); i != myFlows.end(); ++i) {
        if (i->vehicle == veh) {
            i->vehicle = 0;
            break;
        }
    }
}


void
MSInsertionControl::checkPrevious(SUMOTime time) {
    // check to which list append to
    MSVehicleContainer::VehicleVector& previousRefused =
        myRefusedEmits2.size() == 0 ? myRefusedEmits1 : myRefusedEmits2;
    while (!myAllVeh.isEmpty() && myAllVeh.topTime() < time) {
        const MSVehicleContainer::VehicleVector& top = myAllVeh.top();
        copy(top.begin(), top.end(), back_inserter(previousRefused));
        myAllVeh.pop();
    }
}


unsigned int
MSInsertionControl::checkFlows(SUMOTime time,
                               MSVehicleContainer::VehicleVector& refusedEmits) {
    MSVehicleControl& vehControl = MSNet::getInstance()->getVehicleControl();
    unsigned int noEmitted = 0;
    for (std::vector<Flow>::iterator i = myFlows.begin(); i != myFlows.end();) {
        SUMOVehicleParameter* pars = i->pars;
        if (!i->isVolatile && i->vehicle != 0) {
            ++i;
            continue;
        }
        while (pars->repetitionsDone < pars->repetitionNumber &&
                pars->depart + pars->repetitionsDone * pars->repetitionOffset < time + DELTA_T) {
            SUMOVehicleParameter* newPars = new SUMOVehicleParameter(*pars);
            newPars->id = pars->id + "." + toString(pars->repetitionsDone);
            newPars->depart = static_cast<SUMOTime>(pars->depart + pars->repetitionsDone * pars->repetitionOffset);
            pars->repetitionsDone++;
            // try to build the vehicle
            if (vehControl.getVehicle(newPars->id) == 0) {
                const MSRoute* route = MSRoute::dictionary(pars->routeid);
                const MSVehicleType* vtype = vehControl.getVType(pars->vtypeid);
                i->vehicle = vehControl.buildVehicle(newPars, route, vtype);
                if (vehControl.isInQuota()) {
                    vehControl.addVehicle(newPars->id, i->vehicle);
                    noEmitted += tryInsert(time, i->vehicle, refusedEmits);
                    if (!i->isVolatile && i->vehicle != 0) {
                        break;
                    }
                } else {
                    vehControl.deleteVehicle(i->vehicle, true);
                    i->vehicle = 0;
                }
            } else {
                // strange: another vehicle with the same id already exists
                if (MSGlobals::gStateLoaded) {
                    break;
                }
                throw ProcessError("Another vehicle with the id '" + newPars->id + "' exists.");
            }
        }
        if (pars->repetitionsDone == pars->repetitionNumber) {
            i = myFlows.erase(i);
            MSRoute::checkDist(pars->routeid);
            delete pars;
        } else {
            ++i;
        }
    }
    return noEmitted;
}


unsigned int
MSInsertionControl::getWaitingVehicleNo() const {
    return (unsigned int)(myRefusedEmits1.size() + myRefusedEmits2.size());
}


int
MSInsertionControl::getPendingFlowCount() const {
    return (int)myFlows.size();
}


void
MSInsertionControl::descheduleDeparture(SUMOVehicle* veh) {
    myAbortedEmits.insert(veh);
}

/****************************************************************************/

