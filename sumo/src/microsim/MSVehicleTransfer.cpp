/***************************************************************************
                          MSVehicleTransfer.cpp  -
    A mover of vehicles that got stucked due to grid locks
                             -------------------
    begin                : Sep 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel Krajzewicz@dlr.de
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
// Revision 1.18  2006/01/26 08:30:29  dkrajzew
// patched MSEdge in order to work with a generic router
//
// Revision 1.17  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.16  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.15  2005/07/12 12:28:53  dkrajzew
// debugging
//
// Revision 1.14  2005/05/04 08:35:40  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.13  2005/02/01 10:10:42  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.12  2004/11/23 10:20:11  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.11  2004/08/02 12:40:19  dkrajzew
// method renamed due to semantic reasons
//
// Revision 1.10  2004/07/02 09:56:40  dkrajzew
// debugging while implementing the vss visualisation
//
// Revision 1.9  2004/04/23 12:39:26  dkrajzew
// debug-variables removed
//
// Revision 1.8  2004/03/19 13:09:40  dkrajzew
// debugging
//
// Revision 1.7  2003/12/12 12:37:42  dkrajzew
// proper usage of lane states applied; scheduling of vehicles into the beamer on push failures added
//
// Revision 1.6  2003/12/11 06:31:45  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.5  2003/11/20 14:58:21  dkrajzew
// comments added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <iostream>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include "MSLane.h"
#include "MSVehicle.h"
#include "MSVehicleControl.h"
#include "MSVehicleTransfer.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
MSVehicleTransfer *MSVehicleTransfer::myInstance = 0;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
void
MSVehicleTransfer::addVeh(MSVehicle *veh)
{
    // get the current edge of the vehicle
    MSEdge *e = MSEdge::dictionary(veh->getEdge()->getID());
    WRITE_WARNING("Vehicle '" + veh->id() + "' will be teleported; edge '" + e->getID() + "'.");
    // let the vehicle be on the one
    const MSLane &lane = veh->getLane();
    veh->leaveLaneAtLaneChange();
    veh->onTripEnd(/*lane*/);
    if(veh->proceedVirtualReturnWhetherEnded(MSEdge::dictionary(veh->succEdge(1)->getID()))) {
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        return;
    }
    // mark the next one
    e = MSEdge::dictionary(veh->getEdge()->getID());
    myNoTransfered++;
    assert(e!=0);
    // save information
    myVehicles.push_back(
        VehicleInformation(veh, MSNet::getInstance()->getCurrentTimeStep(), e));
}


void
MSVehicleTransfer::checkEmissions(SUMOTime time)
{
    // go through vehicles
    for(VehicleInfVector::iterator i=myVehicles.begin(); i!=myVehicles.end(); ) {
        // get the vehicle information
        VehicleInformation &desc = *i;
        MSEdge *e = (MSEdge*) desc.myVeh->getEdge();
        // check whether the vehicle may be emitted onto a following edge
        if(e->emit(*(desc.myVeh), time)) {
            // remove from this if so
            WRITE_WARNING("Vehicle '" + desc.myVeh->id()+ "' ends teleporting on edge '" + e->getID()+ "'.");
            i = myVehicles.erase(i);
        } else {
            // otherwise, check whether a consecutive edge may be used
            if(desc.myProceedTime<time) {
                // get the lanes of the next edge (the one the vehicle wiil be
                //  virtually on after all these computations)
                MSLane *tmp = *(e->getLanes()->begin());
                // get the one beyond the one the vehicle moved to
                MSEdge *nextEdge = MSEdge::dictionary(desc.myVeh->succEdge(1)->getID());
                // let the vehicle move to the next edge
                if(desc.myVeh->proceedVirtualReturnWhetherEnded(nextEdge)) {
                    WRITE_WARNING("Vehicle '" + desc.myVeh->id()+ "' ends teleporting on end edge '" + e->getID()+ "'.");
                    MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(desc.myVeh);
                    i = myVehicles.erase(i);
                    continue;
                }
                // get the time the vehicle needs to pass the current edge
                desc.myProceedTime = time + (SUMOTime) (tmp->length() / tmp->maxSpeed()
                    * 2.0); // !!! maybe, the time should be compued in other ways
            }
            i++;
        }

    }
}


MSVehicleTransfer *
MSVehicleTransfer::getInstance()
{
    return myInstance;
}


void
MSVehicleTransfer::setInstance(MSVehicleTransfer *vt)
{
    assert(myInstance==0);
    myInstance = vt;
}


MSVehicleTransfer::MSVehicleTransfer()
    : myNoTransfered(0)
{
}


MSVehicleTransfer::~MSVehicleTransfer()
{
    myInstance = 0;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
