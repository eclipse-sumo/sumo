/***************************************************************************
                          MSVehicleControl.cpp  -
    The class responsible for building and deletion of vehicles
                             -------------------
    begin                : Wed, 10. Dec 2003
    copyright            : (C) 2002 by DLR http://ivf.dlr.de
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
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
// Revision 1.7  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/05/04 08:35:40  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.5  2005/02/01 10:10:42  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.4  2004/11/23 10:20:11  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.3  2004/07/02 09:56:40  dkrajzew
// debugging while implementing the vss visualisation
//
// Revision 1.2  2004/04/02 11:36:28  dkrajzew
// "compute or not"-structure added; added two further simulation-wide output
//  (emission-stats and single vehicle trip-infos)
//
// Revision 1.1  2003/12/11 06:31:45  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSCORN.h"
#include "MSVehicleControl.h"
#include "MSVehicle.h"
#include "MSSaveState.h"
#include <utils/common/FileHelpers.h>
#include <utils/bindevice/BinaryInputDevice.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSVehicleControl::MSVehicleControl()
    : myLoadedVehNo(0), myEmittedVehNo(0), myRunningVehNo(0), myEndedVehNo(0),
    myAbsVehWaitingTime(0), myAbsVehTravelTime(0)
{
}


MSVehicleControl::~MSVehicleControl()
{
}


MSVehicle *
MSVehicleControl::buildVehicle(std::string id, MSRoute* route,
                               SUMOTime departTime,
                               const MSVehicleType* type,
                               int repNo, int repOffset)
{
    myLoadedVehNo++;
    route->incReferenceCnt();
    return new MSVehicle(id, route, departTime, type,
        MSNet::getInstance()->getNDumpIntervalls(), repNo, repOffset);
}


MSVehicle *
MSVehicleControl::buildVehicle(std::string id, MSRoute* route,
                               SUMOTime departTime,
                               const MSVehicleType* type,
                               int repNo, int repOffset, const RGBColor &col)
{
    myLoadedVehNo++;
    route->incReferenceCnt();
    return new MSVehicle(id, route, departTime, type,
        MSNet::getInstance()->getNDumpIntervalls(), repNo, repOffset);
}


void
MSVehicleControl::scheduleVehicleRemoval(MSVehicle *v)
{
    assert(myRunningVehNo>0);
    // check whether to generate the information about the vehicle's trip
    if(MSCORN::wished(MSCORN::CORN_OUT_TRIPDURATIONS)) {
        MSCORN::compute_TripDurationsOutput(v);
    }
    if(MSCORN::wished(MSCORN::CORN_OUT_VEHROUTES)) {
        MSCORN::compute_VehicleRouteOutput(v);
    }
    // check whether to save information about the vehicle's trip
    if(MSCORN::wished(MSCORN::CORN_MEAN_VEH_TRAVELTIME)) {
        myAbsVehTravelTime +=
            (MSNet::getInstance()->getCurrentTimeStep()
            -
            (long) v->getCORNDoubleValue(MSCORN::CORN_VEH_REALDEPART));
    }
    myRunningVehNo--;
    myEndedVehNo++;
    removeVehicle(v);
}


void
MSVehicleControl::removeVehicle(MSVehicle *v)
{
    MSVehicle::remove(v->id());
}




void
MSVehicleControl::newUnbuildVehicleLoaded()
{
    myLoadedVehNo++;
}


void
MSVehicleControl::newUnbuildVehicleBuild()
{
    myLoadedVehNo--;
}



size_t
MSVehicleControl::getLoadedVehicleNo() const
{
    return myLoadedVehNo;
}


size_t
MSVehicleControl::getEndedVehicleNo() const
{
    return myEndedVehNo;
}


size_t
MSVehicleControl::getRunningVehicleNo() const
{
    return myRunningVehNo;
}


size_t
MSVehicleControl::getEmittedVehicleNo() const
{
    return myEmittedVehNo;
}


size_t
MSVehicleControl::getWaitingVehicleNo() const
{
    return myLoadedVehNo - myEmittedVehNo;
}


double
MSVehicleControl::getMeanWaitingTime() const
{
    if(myEmittedVehNo==0) {
        return -1;
    }
    return (double) myAbsVehWaitingTime / (double) myEmittedVehNo;
}


double
MSVehicleControl::getMeanTravelTime() const
{
    if(myEndedVehNo==0) {
        return -1;
    }
    return (double) myAbsVehTravelTime / (double) myEndedVehNo;
}


void
MSVehicleControl::vehiclesEmitted(size_t no)
{
    myEmittedVehNo += no;
    myRunningVehNo += no;
}


bool
MSVehicleControl::haveAllVehiclesQuit() const
{
    return myLoadedVehNo==myEndedVehNo;
}


void
MSVehicleControl::vehicleEmitted(MSVehicle *v)
{
    if(MSCORN::wished(MSCORN::CORN_MEAN_VEH_WAITINGTIME)) {
        myAbsVehWaitingTime +=
            ((long) v->getCORNDoubleValue(MSCORN::CORN_VEH_REALDEPART)
            -
            v->desiredDepart());
    }
}


void
MSVehicleControl::vehicleMoves(MSVehicle *v)
{
}


void
MSVehicleControl::saveState(std::ostream &os, long what)
{
    FileHelpers::writeUInt(os, myLoadedVehNo);
    FileHelpers::writeUInt(os, myEmittedVehNo);
    FileHelpers::writeUInt(os, myRunningVehNo);
    FileHelpers::writeUInt(os, myEndedVehNo);

    FileHelpers::writeInt(os, myAbsVehWaitingTime);
    FileHelpers::writeInt(os, myAbsVehTravelTime);
//    MSVehicleType::saveState(os, what);
    MSRoute::dict_saveState(os, what);
    MSVehicle::dict_saveState(os, what);
}

void
MSVehicleControl::loadState(BinaryInputDevice &bis, long what)
{
    bis >> myLoadedVehNo;
    bis >> myEmittedVehNo;
    bis >> myRunningVehNo;
    bis >> myEndedVehNo;

    bis >> myAbsVehWaitingTime;
    bis >> myAbsVehTravelTime;

//    long t;
    //os >> t;
    MSRoute::dict_loadState(bis, what);
    MSVehicle::dict_loadState(bis, what);
    MSRoute::clearLoadedState();
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

