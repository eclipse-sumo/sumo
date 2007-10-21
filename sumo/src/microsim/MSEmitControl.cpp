/****************************************************************************/
/// @file    MSEmitControl.cpp
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// vehicles into the net.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
MSEmitControl::MSEmitControl(MSVehicleControl &vc, SUMOTime maxDepartDelay)
        : myVehicleControl(vc), myMaxDepartDelay(maxDepartDelay)
{}


MSEmitControl::~MSEmitControl()
{}


void
MSEmitControl::add(MSVehicle *veh)
{
    myAllVeh.add(veh);
}

void
MSEmitControl::moveFrom(MSVehicleContainer &cont)
{
    myAllVeh.moveFrom(cont);
}


size_t
MSEmitControl::emitVehicles(SUMOTime time)
{
    checkPrevious(time);
    // check whether any vehicles shall be emitted within this time step
    if (!myAllVeh.anyWaitingFor(time)&&myRefusedEmits1.size()==0&&myRefusedEmits2.size()==0) {
        return 0;
    }
    size_t noEmitted = 0;
    // we use SUMOReal-buffering for the refused emits to save time
    assert(myRefusedEmits1.size()==0||myRefusedEmits2.size()==0);
    MSVehicleContainer::VehicleVector &refusedEmits =
        myRefusedEmits1.size()==0 ? myRefusedEmits1 : myRefusedEmits2;
    MSVehicleContainer::VehicleVector &previousRefused =
        myRefusedEmits2.size()==0 ? myRefusedEmits1 : myRefusedEmits2;
    //
    // go through the list of previously refused first
    MSVehicleContainer::VehicleVector::const_iterator veh;
    for (veh=previousRefused.begin(); veh!=previousRefused.end(); veh++) {
        noEmitted += tryEmit(time, *veh, refusedEmits);
    }
    // clear previously refused vehicle container
    previousRefused.clear();

    // Insert vehicles from myTrips into the net until the vehicles
    // departure time is greater than time.
    // retrieve the list of vehicles to emit within this time step
    if (myAllVeh.anyWaitingFor(time)) {
        const MSVehicleContainer::VehicleVector &next = myAllVeh.top();
        // go through the list and try to emit
        for (veh=next.begin(); veh!=next.end(); veh++) {
            noEmitted += tryEmit(time, *veh, refusedEmits);
        }
        // let the MSVehicleContainer clear the vehicles
        myAllVeh.pop();
    }
    for (MSVehicleContainer::VehicleVector::iterator i=myNewPeriodicalAdds.begin(); i!=myNewPeriodicalAdds.end(); ++i) {
        add(*i);
    }
    myNewPeriodicalAdds.clear();
    return noEmitted;
}


size_t
MSEmitControl::tryEmit(SUMOTime time, MSVehicle *veh,
                       MSVehicleContainer::VehicleVector &refusedEmits)
{
    assert(veh->desiredDepart() <= time);
    const MSEdge &edge = veh->departEdge();
    if (edge.getLastFailedEmissionTime()!=time && edge.emit(*veh, time)) {
        // Successful emission.
        veh->onDepart();
        // Check whether another vehicle shall be
        //  emitted with the same parameter
        if (veh->periodical()) {
            MSVehicle *nextPeriodical = veh->getNextPeriodical();
            if (nextPeriodical!=0) {
                myNewPeriodicalAdds.push_back(nextPeriodical);
                myVehicleControl.addVehicle(nextPeriodical->getID(), nextPeriodical);
            }
        }
        return 1;
    }
    if (myMaxDepartDelay == -1 || time - veh->desiredDepart() <= myMaxDepartDelay) {
        refusedEmits.push_back(veh);
    } else {
        myVehicleControl.deleteVehicle(veh);
    }
    edge.setLastFailedEmissionTime(time);
    return 0;
}


void
MSEmitControl::checkPrevious(SUMOTime time)
{
    // check to which list append to
    MSVehicleContainer::VehicleVector &previousRefused =
        myRefusedEmits2.size()==0 ? myRefusedEmits1 : myRefusedEmits2;
    while (!myAllVeh.isEmpty()&&myAllVeh.topTime()<time) {
        const MSVehicleContainer::VehicleVector &top = myAllVeh.top();
        copy(top.begin(), top.end(), back_inserter(previousRefused));
        myAllVeh.pop();
    }
}


size_t
MSEmitControl::getWaitingVehicleNo() const
{
    return myRefusedEmits1.size() + myRefusedEmits2.size();
}



/****************************************************************************/

