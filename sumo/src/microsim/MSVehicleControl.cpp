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
// Revision 1.1  2003/12/11 06:31:45  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "MSVehicleControl.h"
#include "MSVehicle.h"


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSVehicleControl::MSVehicleControl(MSNet &net)
    : myLoadedVehNo(0), myEmittedVehNo(0), myRunningVehNo(0), myEndedVehNo(0),
    myNet(net)
{
}


MSVehicleControl::~MSVehicleControl()
{
}


MSVehicle *
MSVehicleControl::buildVehicle(std::string id, MSRoute* route,
                               MSNet::Time departTime,
                               const MSVehicleType* type,
                               int repNo, int repOffset)
{
	myLoadedVehNo++;
    return new MSVehicle(id, route, departTime, type,
        myNet.getNDumpIntervalls(), repNo, repOffset);
}


MSVehicle *
MSVehicleControl::buildVehicle(std::string id, MSRoute* route,
                               MSNet::Time departTime,
                               const MSVehicleType* type,
                               int repNo, int repOffset, const RGBColor &col)
{
	myLoadedVehNo++;
    return new MSVehicle(id, route, departTime, type,
        myNet.getNDumpIntervalls(), repNo, repOffset);
}


void
MSVehicleControl::scheduleVehicleRemoval(MSVehicle *v)
{
    assert(myRunningVehNo>0);
	myRunningVehNo--;
	myEndedVehNo++;
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

