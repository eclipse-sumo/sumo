/***************************************************************************
                          MSEmitControl.cpp  -  Controls emission of
                          vehicles into the net.
                             -------------------
    begin                : Mon, 12 Mar 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

namespace
{
    const char rcsid[] =
    "$Id$";
}

// $Log$
// Revision 1.11  2005/05/04 08:24:42  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; speed-ups by checked emission and avoiding looping over all edges
//
// Revision 1.10  2005/02/01 10:10:40  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.9  2004/04/02 11:36:27  dkrajzew
// "compute or not"-structure added; added two further simulation-wide output (emission-stats and single vehicle trip-infos)
//
// Revision 1.8  2004/01/26 15:55:04  dkrajzew
// the vehicle is now informed about being emitted (as we want to display the information about the real departure time witin the gui - within microsim, this information may be used for some other stuff
//
// Revision 1.7  2003/09/17 10:11:37  dkrajzew
// error on broke vehicle departure map reference patched
//
// Revision 1.6  2003/07/30 09:01:02  dkrajzew
// false return value for number of emitted vehicles patched
//
// Revision 1.5  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.4  2003/03/20 16:21:11  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.3  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:39:01  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.2  2002/03/20 15:54:00  croessel
// Return to older version.
//
// Revision 2.0  2002/02/14 14:43:14  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.8  2002/02/05 13:51:51  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.7  2002/01/31 13:50:39  croessel
// assert-header included.
//
// Revision 1.6  2001/12/19 17:00:29  croessel
// Default-ctor, copy-ctor and assignment-operator removed.
//
// Revision 1.5  2001/12/06 13:10:40  traffic
// minor bug removed (was: possibly invalid vehicle references were accessed)
//
// Revision 1.4  2001/11/15 17:12:12  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.3  2001/11/14 15:47:33  croessel
// Merged the diffs between the .C and .cpp versions. Numerous changes
// in MSLane, MSVehicle and MSJunction.
//
// Revision 1.2  2001/11/14 10:49:06  croessel
// CR-line-end removed.
//
// Revision 1.1  2001/10/24 07:08:19  traffic
// new extension
//
// Revision 1.5  2001/10/23 09:30:08  traffic
// parser bugs removed
//
// Revision 1.3  2001/07/25 12:16:46  traffic
// CC problems with make_pair repaired
//
// Revision 1.2  2001/07/16 12:55:46  croessel
// Changed id type from unsigned int to string. Added string-pointer
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <algorithm>
#include <cassert>
#include "MSEmitControl.h"
#include "MSVehicle.h"
#include "MSLane.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
MSEmitControl::DictType MSEmitControl::myDict;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSEmitControl::MSEmitControl(string id)
    : myID(id)
{
}


MSEmitControl::~MSEmitControl()
{
}


void
MSEmitControl::add( MSVehicle *veh )
{
    myAllVeh.add(veh);
}

void
MSEmitControl::moveFrom( MSVehicleContainer &cont )
{
    myAllVeh.moveFrom(cont);
}


size_t
MSEmitControl::emitVehicles(SUMOTime time)
{
    checkPrevious(time);
    // check whether any vehicles shall be emitted within this time step
    if(!myAllVeh.anyWaitingFor(time)&&myRefusedEmits1.size()==0&&myRefusedEmits2.size()==0) {
        return 0;
    }
	size_t noEmitted = 0;
    // we use double-buffering for the refused emits to save time
    assert(myRefusedEmits1.size()==0||myRefusedEmits2.size()==0);
    MSVehicleContainer::VehicleVector &refusedEmits =
        myRefusedEmits1.size()==0 ? myRefusedEmits1 : myRefusedEmits2;
    MSVehicleContainer::VehicleVector &previousRefused =
        myRefusedEmits2.size()==0 ? myRefusedEmits1 : myRefusedEmits2;
    //
    // go through the list of previously refused first
    MSVehicleContainer::VehicleVector::const_iterator veh;
    for( veh=previousRefused.begin(); veh!=previousRefused.end(); veh++) {
        noEmitted += tryEmit(time, *veh, refusedEmits);
    }
    // clear previously refused vehicle container
    previousRefused.clear();

    // Insert vehicles from myTrips into the net until the vehicles
    // departure time is greater than time.
    // retrieve the list of vehicles to emit within this time step
    if(myAllVeh.anyWaitingFor(time)) {
        const MSVehicleContainer::VehicleVector &next = myAllVeh.top();
        // go through the list and try to emit
        for( veh=next.begin(); veh!=next.end(); veh++) {
            noEmitted += tryEmit(time, *veh, refusedEmits);
        }
        // let the MSVehicleContainer clear the vehicles
        myAllVeh.pop();
    }
    for(MSVehicleContainer::VehicleVector::iterator i=myNewPeriodicalAdds.begin(); i!=myNewPeriodicalAdds.end(); i++) {
        add(*i);
    }
    myNewPeriodicalAdds.clear();
	return noEmitted;
}


size_t
MSEmitControl::tryEmit(SUMOTime time, MSVehicle *veh,
                       MSVehicleContainer::VehicleVector &refusedEmits)
{
	size_t noEmitted = 0;
    // check whether the edge was used already and failed
    //  (speedup function only)
    MSEdge &edge = veh->departEdge();
    if(edge.getLastFailedEmissionTime()==time) {
        refusedEmits.push_back(veh);
        return noEmitted;
    }
    // try to emit the vehicle
    if (veh->departEdge().emit(*veh)) {
        // Successful emission.
		noEmitted++;
        veh->onDepart();
            // Check whether another vehicle shall be
            //  emitted with the same parameter
        if(veh->periodical()) {
            MSVehicle *nextPeriodical = veh->getNextPeriodical();
            if(nextPeriodical!=0) {
                myNewPeriodicalAdds.push_back(nextPeriodical);
                // !!! add some kind of a check and an error
                //  handler, here
                MSVehicle::dictionary(nextPeriodical->id(), nextPeriodical);
            }
        }
    } else {
        // Emission not successful. Store for next-timestep
        // retry in the next step
        refusedEmits.push_back(veh);
        edge.setLastFailedEmissionTime(time);
    }
	return noEmitted;
}


bool
MSEmitControl::dictionary(string id, MSEmitControl* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}


MSEmitControl*
MSEmitControl::dictionary(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


void
MSEmitControl::clear()
{
    for(DictType::iterator it = myDict.begin(); it!=myDict.end(); it++) {
        delete (*it).second;
    }
}


void
MSEmitControl::checkPrevious(SUMOTime time)
{
    // check to which list append to
    MSVehicleContainer::VehicleVector &previousRefused =
        myRefusedEmits2.size()==0 ? myRefusedEmits1 : myRefusedEmits2;
    while(!myAllVeh.isEmpty()&&myAllVeh.topTime()<time) {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:






