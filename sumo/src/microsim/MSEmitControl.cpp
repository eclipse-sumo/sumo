/***************************************************************************
                          MSEmitControl.C  -  Controls emission of
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <algorithm>
#include <cassert>
#include "MSEmitControl.h"
#include "MSVehicle.h"
#include "MSLane.h"

using namespace std;


// Init static member.
MSEmitControl::DictType MSEmitControl::myDict;


MSEmitControl::MSEmitControl(string id, VehCont* allVeh) :
    myID(id),
    myAllVeh(allVeh)
{
    sort(myAllVeh->begin(), myAllVeh->end(), departTimeSortCrit );
//      myTrips->sort(departTimeSortCrit); // sort for lists, doesn't
//      // work with Sun SC5.0
}


MSEmitControl::~MSEmitControl()
{
}

void 
MSEmitControl::add(MSEmitControl *cont) {
    myAllVeh->reserve(myAllVeh->size() + cont->myAllVeh->size());
    for(VehCont::iterator i=cont->myAllVeh->begin(); 
        i!=cont->myAllVeh->end(); i++)
        myAllVeh->push_back(*i);
    sort(myAllVeh->begin(), myAllVeh->end(), departTimeSortCrit);
}


void
MSEmitControl::emitVehicles(MSNet::Time time)
{
    // Insert vehicles from myTrips into the net until the vehicles
    // departure time is greater than time.
    VehCont refusedEmits; // Tmp-container for vehicles that were
    // not allowed to enter their lane.
    VehCont::iterator veh = myAllVeh->begin();
    while (veh != myAllVeh->end() && (*veh)->desiredDepart() <= time ) { 
        if ((*veh)->departLane().emit(**veh) == true) { 
            // Successful emission.
        }
        else {
            // Emission not successful. Store for next-timestep
            // retry. 
            refusedEmits.push_back(*veh);
        }
        ++veh;
    }
    // Remove range from container, then add refusedEmits.
    myAllVeh->erase(myAllVeh->begin(), veh);
    myAllVeh->insert(myAllVeh->begin(), refusedEmits.begin(),
                     refusedEmits.end());
}

void 
MSEmitControl::addStarting(MSVehicle *veh) {
    myAllVeh->insert(myAllVeh->begin(), veh);
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
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "MSEmitControl.iC"
//#endif

// Local Variables:
// mode:C++
// End:
