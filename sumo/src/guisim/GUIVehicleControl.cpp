/***************************************************************************
                          GUIVehicleControl.cpp  -
 The class responsible for building and deletion of vehicles within the gui
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
// Revision 1.2  2004/04/02 11:19:16  dkrajzew
// debugging
//
// Revision 1.1  2003/12/11 06:26:27  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <microsim/MSCORN.h>
#include "GUIVehicleControl.h"
#include "GUIVehicle.h"
#include "GUINet.h"


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIVehicleControl::GUIVehicleControl()
    : MSVehicleControl()
{
}


GUIVehicleControl::~GUIVehicleControl()
{
}


MSVehicle *
GUIVehicleControl::buildVehicle(std::string id, MSRoute* route,
                               MSNet::Time departTime,
                               const MSVehicleType* type,
                               int repNo, int repOffset)
{
    throw 1;
}


MSVehicle *
GUIVehicleControl::buildVehicle(std::string id, MSRoute* route,
                               MSNet::Time departTime,
                               const MSVehicleType* type,
                               int repNo, int repOffset, const RGBColor &col)
{
	myLoadedVehNo++;
    MSNet *net = MSNet::getInstance();
    return new GUIVehicle(
        static_cast<GUINet*>(net)->getIDStorage(),
        id, route, departTime, type,
        net->getNDumpIntervalls(), repNo, repOffset, col);
}


void
GUIVehicleControl::scheduleVehicleRemoval(MSVehicle *veh)
{
    assert(myRunningVehNo>0);
    if(MSCORN::wished(MSCORN::CORN_OUT_TRIPOUTPUT)) {
        MSCORN::compute_TripInfoOutput(veh);
    }
	myRunningVehNo--;
	myEndedVehNo++;
    static_cast<GUIVehicle*>(veh)->setRemoved();
    static_cast<GUINet*>(MSNet::getInstance())->getIDStorage().remove(
        static_cast<GUIVehicle*>(veh)->getGlID());
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
