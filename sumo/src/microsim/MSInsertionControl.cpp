/****************************************************************************/
/// @file    MSInsertionControl.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Inserts vehicles into the network when their departure time is reached
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

#include <iostream>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <microsim/devices/MSDevice_Routing.h>
#include "MSGlobals.h"
#include "MSInsertionControl.h"
#include "MSVehicle.h"
#include "MSLane.h"
#include "MSEdge.h"
#include "MSRouteHandler.h"

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
    flow.index = 0;
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
    // check whether any vehicles shall be emitted within this time step
    const bool havePreChecked = MSDevice_Routing::isEnabled();
    if (myPendingEmits.empty() || (havePreChecked && myEmitCandidates.empty())) {
        return 0;
    }
    unsigned int numEmitted = 0;
    // we use buffering for the refused emits to save time
    //  for this, we have two lists; one contains previously refused emits, the second
    //  will be used to append those vehicles that will not be able to depart in this
    //  time step
    MSVehicleContainer::VehicleVector refusedEmits;

    // go through the list of previously refused vehicles, first
    MSVehicleContainer::VehicleVector::const_iterator veh;
    for (veh = myPendingEmits.begin(); veh != myPendingEmits.end(); veh++) {
        if (havePreChecked && (myEmitCandidates.count(*veh) == 0)) {
            refusedEmits.push_back(*veh);
        } else {
            numEmitted += tryInsert(time, *veh, refusedEmits);
        }
    }
    myEmitCandidates.clear();
    myPendingEmits = refusedEmits;
    return numEmitted;
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
MSInsertionControl::checkCandidates(SUMOTime time, const bool preCheck) {
    while (myAllVeh.anyWaitingBefore(time + DELTA_T)) {
        const MSVehicleContainer::VehicleVector& top = myAllVeh.top();
        copy(top.begin(), top.end(), back_inserter(myPendingEmits));
        myAllVeh.pop();
    }
    if (preCheck) {
        MSVehicleContainer::VehicleVector::const_iterator veh;
        for (veh = myPendingEmits.begin(); veh != myPendingEmits.end(); veh++) {
            SUMOVehicle* const v = *veh;
            const MSEdge* const edge = v->getEdge();
            if ((!myCheckEdgesOnce || edge->getLastFailedInsertionTime() != time) && edge->insertVehicle(*v, time, true)) {
                myEmitCandidates.insert(v);
            } else {
                MSDevice_Routing* dev = static_cast<MSDevice_Routing*>(v->getDevice(typeid(MSDevice_Routing)));
                if (dev != 0) {
                    dev->skipRouting(time);
                }
            }
        }
    }
}


void
MSInsertionControl::determineCandidates(SUMOTime time) {
    MSVehicleControl& vehControl = MSNet::getInstance()->getVehicleControl();
    for (std::vector<Flow>::iterator i = myFlows.begin(); i != myFlows.end();) {
        SUMOVehicleParameter* pars = i->pars;
        if (!i->isVolatile && i->vehicle != 0 && pars->repetitionProbability < 0) {
            ++i;
            //std::cout << SIMTIME << " volatile=" << i->isVolatile << " veh=" << i->vehicle << "\n";
            continue;
        }
        bool tryEmitByProb = pars->repetitionProbability > 0;
        while ((pars->repetitionProbability < 0
                && pars->repetitionsDone < pars->repetitionNumber
                && pars->depart + pars->repetitionsDone * pars->repetitionOffset < time + DELTA_T)
                || (tryEmitByProb
                    && pars->depart < time + DELTA_T
                    && pars->repetitionEnd > time
                    // only call rand if all other conditions are met
                    && RandHelper::rand() < (pars->repetitionProbability * TS))
              ) {
            tryEmitByProb = false; // only emit one per step
            SUMOVehicleParameter* newPars = new SUMOVehicleParameter(*pars);
            newPars->id = pars->id + "." + toString(i->index);
            newPars->depart = static_cast<SUMOTime>(pars->depart + pars->repetitionsDone * pars->repetitionOffset);
            pars->repetitionsDone++;
            // try to build the vehicle
            if (vehControl.getVehicle(newPars->id) == 0) {
                const MSRoute* route = MSRoute::dictionary(pars->routeid);
                const MSVehicleType* vtype = vehControl.getVType(pars->vtypeid, MSRouteHandler::getParsingRNG());
                i->vehicle = vehControl.buildVehicle(newPars, route, vtype, false);
                unsigned int quota = vehControl.getQuota();
                if (quota > 0) {
                    vehControl.addVehicle(newPars->id, i->vehicle);
                    add(i->vehicle);
                    i->index++;
                    while (--quota > 0) {
                        SUMOVehicleParameter* quotaPars = new SUMOVehicleParameter(*pars);
                        quotaPars->id = pars->id + "." + toString(i->index);
                        quotaPars->depart = static_cast<SUMOTime>(pars->depart + pars->repetitionsDone * pars->repetitionOffset);
                        i->vehicle = vehControl.buildVehicle(quotaPars, route, vtype, false);
                        vehControl.addVehicle(quotaPars->id, i->vehicle);
                        add(i->vehicle);
                        i->index++;
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
        if (pars->repetitionsDone == pars->repetitionNumber || (pars->repetitionProbability > 0 && pars->repetitionEnd <= time)) {
            i = myFlows.erase(i);
            MSRoute::checkDist(pars->routeid);
            delete pars;
        } else {
            ++i;
        }
    }
    checkCandidates(time, MSDevice_Routing::isEnabled());
}


unsigned int
MSInsertionControl::getWaitingVehicleNo() const {
    return (unsigned int)myPendingEmits.size();
}


int
MSInsertionControl::getPendingFlowCount() const {
    return (int)myFlows.size();
}


void
MSInsertionControl::descheduleDeparture(SUMOVehicle* veh) {
    myAbortedEmits.insert(veh);
}

void
MSInsertionControl::clearPendingVehicles(std::string& route) {
    //clear out the refused vehicle list, deleting the vehicles entirely
    MSVehicleContainer::VehicleVector::iterator veh;
    for (veh = myPendingEmits.begin(); veh != myPendingEmits.end();) {
        if ((*veh)->getRoute().getID() == route || route == "") {
            myVehicleControl.deleteVehicle(*veh, true);
            veh = myPendingEmits.erase(veh);
        } else {
            ++veh;
        }
    }
}


/****************************************************************************/

