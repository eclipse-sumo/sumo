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
// Revision 1.7  2003/12/12 12:37:42  dkrajzew
// proper usage of lane states applied; scheduling of vehicles into the beamer on push failures added
//
// Revision 1.6  2003/12/11 06:31:45  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.5  2003/11/20 14:58:21  dkrajzew
// comments added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include "MSLane.h"
#include "MSVehicle.h"
#include "MSVehicleControl.h"
#include "MSVehicleTransfer.h"


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
    MSEdge *e = MSEdge::dictionary(veh->getEdge()->id());
    MsgHandler::getWarningInstance()->inform(
        string("Vehicle '") + veh->id() + string("' will be teleported; edge '")
        + e->id() + string("'."));
    // let the vehicle be on the one
    if(veh->proceedVirtualReturnIfEnded(*this, MSEdge::dictionary(veh->succEdge(1)->id()))) {
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        return;
    }
    // mark the next one
    e = MSEdge::dictionary(veh->getEdge()->id());
    myNoTransfered++;
    assert(e!=0);
    // save information
    myVehicles.push_back(
        VehicleInformation(veh, MSNet::getInstance()->getCurrentTimeStep(), e));
}


void
MSVehicleTransfer::checkEmissions(MSNet::Time time)
{
    // go through vehicles
    for(VehicleInfVector::iterator i=myVehicles.begin(); i!=myVehicles.end(); ) {
        // get the vehicle information
        VehicleInformation &desc = *i;
        MSEdge *e = desc.myNextPossibleEdge;
        // check whether the vehicle may be emitted onto a following edge
        if(e->emit(*(desc.myVeh))) {
            // remove from this if so
            MsgHandler::getWarningInstance()->inform(
                string("Vehicle '") + desc.myVeh->id()
                + string("' ends teleporting on edge '") + e->id()
                + string("'."));
            i = myVehicles.erase(i);
        } else {
            // otherwise, check whether a consecutive edge may be used
            if(desc.myProceedTime<time) {
                // get the lanes of the next edge (the one the vehicle wiil be
                //  virtually on after all these computations)
                MSLane *tmp = *(desc.myNextPossibleEdge->getLanes()->begin());
                // get the one beyond the one the vehicle moved to
                MSEdge *nextEdge = MSEdge::dictionary(desc.myVeh->succEdge(1)->id());
                // let the vehicle move to the next edge
                if(desc.myVeh->proceedVirtualReturnIfEnded(*this, nextEdge)) {
                    MsgHandler::getWarningInstance()->inform(
                        string("Vehicle '") + desc.myVeh->id()
                        + string("' ends teleporting on end edge '") + e->id()
                        + string("'."));
                    MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(desc.myVeh);
                    i = myVehicles.erase(i);
                    continue;
                }
                desc.myNextPossibleEdge = nextEdge;
                // get the time the vehicle needs to pass the current edge
                desc.myProceedTime = time + tmp->length() / tmp->maxSpeed()
                    * 2.0; // !!! maybe, the time should be compued in other ways
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
