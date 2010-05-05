/****************************************************************************/
/// @file    MSEmitControl.cpp
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Inserts vehicles into the network when their departure time is reached
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

#include <iostream>
#include <algorithm>
#include <cassert>
#include "MSEmitControl.h"
#include "MSVehicle.h"
#include "MSLane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MSEmitControl::MSEmitControl(MSVehicleControl &vc,
                             SUMOTime maxDepartDelay,
                             bool checkEdgesOnce) throw()
        : myVehicleControl(vc), myMaxDepartDelay(maxDepartDelay),
        myCheckEdgesOnce(checkEdgesOnce) {}


MSEmitControl::~MSEmitControl() throw() {
    for (std::vector<Flow>::iterator i=myFlows.begin(); i!=myFlows.end(); ++i) {
        delete(i->pars);
    }
}


void
MSEmitControl::add(MSVehicle *veh) throw() {
    myAllVeh.add(veh);
}


void
MSEmitControl::add(SUMOVehicleParameter *pars) throw() {
    Flow flow;
    flow.pars = pars;
    flow.isVolatile = pars->departLaneProcedure==DEPART_LANE_RANDOM ||
                      pars->departPosProcedure==DEPART_POS_RANDOM ||
                      MSNet::getInstance()->getVehicleControl().hasVTypeDistribution(pars->vtypeid);
    if (!flow.isVolatile) {
        RandomDistributor<const MSRoute*> *dist = MSRoute::distDictionary(pars->routeid);
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
MSEmitControl::emitVehicles(SUMOTime time) throw(ProcessError) {
    checkPrevious(time);
    // check whether any vehicles shall be emitted within this time step
    if (!myAllVeh.anyWaitingFor(time)&&myRefusedEmits1.empty()&&myRefusedEmits2.empty()&&myFlows.empty()) {
        return 0;
    }
    unsigned int noEmitted = 0;
    // we use buffering for the refused emits to save time
    //  for this, we have two lists; one contains previously refused emits, the second
    //  will be used to append those vehicles that will not be able to depart in this
    //  time step
    assert(myRefusedEmits1.size()==0||myRefusedEmits2.size()==0);
    MSVehicleContainer::VehicleVector &refusedEmits =
        myRefusedEmits1.size()==0 ? myRefusedEmits1 : myRefusedEmits2;
    MSVehicleContainer::VehicleVector &previousRefused =
        myRefusedEmits2.size()==0 ? myRefusedEmits1 : myRefusedEmits2;

    // go through the list of previously refused vehicles, first
    MSVehicleContainer::VehicleVector::const_iterator veh;
    for (veh=previousRefused.begin(); veh!=previousRefused.end(); veh++) {
        noEmitted += tryEmit(time, *veh, refusedEmits);
    }
    // clear previously refused vehicle container
    previousRefused.clear();

    // Insert vehicles from myTrips into the net until the next vehicle's
    //  departure time is greater than the current time.
    // Retrieve the list of vehicles to emit within this time step

    noEmitted += checkFlows(time, refusedEmits);
    while (myAllVeh.anyWaitingFor(time)) {
        const MSVehicleContainer::VehicleVector &next = myAllVeh.top();
        // go through the list and try to emit
        for (veh=next.begin(); veh!=next.end(); veh++) {
            noEmitted += tryEmit(time, *veh, refusedEmits);
        }
        // let the MSVehicleContainer clear the vehicles
        myAllVeh.pop();
    }
    // Return the number of emitted vehicles
    return noEmitted;
}


unsigned int
MSEmitControl::tryEmit(SUMOTime time, MSVehicle *veh,
                       MSVehicleContainer::VehicleVector &refusedEmits) throw(ProcessError) {
    assert(veh->getDesiredDepart() <= time);
    veh->onTryEmit();
    const MSEdge &edge = veh->getDepartEdge();
    if ((!myCheckEdgesOnce || edge.getLastFailedEmissionTime()!=time) && edge.emit(*veh, time)) {
        // Successful emission.
        checkFlowWait(veh);
        veh->onDepart();
        return 1;
    }
    if (myMaxDepartDelay != -1 && time - veh->getDesiredDepart() > myMaxDepartDelay) {
        // remove vehicles waiting too long for departure
        checkFlowWait(veh);
        myVehicleControl.deleteVehicle(veh);
    } else if (edge.isVaporizing()) {
        // remove vehicles if the edge shall be empty
        checkFlowWait(veh);
        veh->setWasVaporized(true);
        myVehicleControl.deleteVehicle(veh);
    } else {
        // let the vehicle wait one step, we'll retry then
        refusedEmits.push_back(veh);
    }
    edge.setLastFailedEmissionTime(time);
    return 0;
}


void
MSEmitControl::checkFlowWait(MSVehicle *veh) throw() {
    for (std::vector<Flow>::iterator i=myFlows.begin(); i!=myFlows.end(); ++i) {
        if (i->vehicle == veh) {
            i->vehicle = 0;
            break;
        }
    }
}


void
MSEmitControl::checkPrevious(SUMOTime time) throw() {
    // check to which list append to
    MSVehicleContainer::VehicleVector &previousRefused =
        myRefusedEmits2.size()==0 ? myRefusedEmits1 : myRefusedEmits2;
    while (!myAllVeh.isEmpty()&&myAllVeh.topTime()<time) {
        const MSVehicleContainer::VehicleVector &top = myAllVeh.top();
        copy(top.begin(), top.end(), back_inserter(previousRefused));
        myAllVeh.pop();
    }
}


unsigned int
MSEmitControl::checkFlows(SUMOTime time,
                          MSVehicleContainer::VehicleVector &refusedEmits) throw(ProcessError) {
    unsigned int noEmitted = 0;
    for (std::vector<Flow>::iterator i=myFlows.begin(); i!=myFlows.end();) {
        SUMOVehicleParameter* pars = i->pars;
        if (!i->isVolatile && i->vehicle!=0) {
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
            if (MSNet::getInstance()->getVehicleControl().getVehicle(newPars->id)==0) {
                const MSRoute *route = MSRoute::dictionary(pars->routeid);
                const MSVehicleType *vtype = MSNet::getInstance()->getVehicleControl().getVType(pars->vtypeid);
                i->vehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(newPars, route, vtype);
                MSNet::getInstance()->getVehicleControl().addVehicle(newPars->id, i->vehicle);
                noEmitted += tryEmit(time, i->vehicle, refusedEmits);
                if (!i->isVolatile && i->vehicle!=0) {
                    break;
                }
            } else {
                // strange: another vehicle with the same id already exists
#ifdef HAVE_MESOSIM
                if (MSGlobals::gStateLoaded) {
                    break;
                }
#endif
                throw ProcessError("Another vehicle with the id '" + newPars->id + "' exists.");
            }
        }
        if (pars->repetitionsDone == pars->repetitionNumber) {
            i = myFlows.erase(i);
            delete(pars);
        } else {
            ++i;
        }
    }
    return noEmitted;
}


unsigned int
MSEmitControl::getWaitingVehicleNo() const throw() {
    return (unsigned int)(myRefusedEmits1.size() + myRefusedEmits2.size());
}


bool
MSEmitControl::hasPendingFlows() const throw() {
    return !myFlows.empty();
}


/****************************************************************************/

