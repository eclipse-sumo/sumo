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
// Revision 1.8  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/05/04 08:05:25  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.5  2005/02/01 10:10:40  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.4  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.3  2004/07/02 08:58:30  dkrajzew
// using global object selection
//
// Revision 1.2  2004/04/02 11:19:16  dkrajzew
// debugging
//
// Revision 1.1  2003/12/11 06:26:27  dkrajzew
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <microsim/MSCORN.h>
#include "GUIVehicleControl.h"
#include "GUIVehicle.h"
#include "GUINet.h"
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
                               SUMOTime departTime,
                               const MSVehicleType* type,
                               int repNo, int repOffset)
{
    throw 1;
}


MSVehicle *
GUIVehicleControl::buildVehicle(std::string id, MSRoute* route,
                               SUMOTime departTime,
                               const MSVehicleType* type,
                               int repNo, int repOffset, const RGBColor &col)
{
    myLoadedVehNo++;
    MSNet *net = MSNet::getInstance();
    return new GUIVehicle(
        gIDStorage, id, route, departTime, type,
        net->getNDumpIntervalls(), repNo, repOffset, col);
}


void
GUIVehicleControl::removeVehicle(MSVehicle *veh)
{
    static_cast<GUIVehicle*>(veh)->setRemoved();
	if(gIDStorage.remove(static_cast<GUIVehicle*>(veh)->getGlID())) {
		MSVehicle::remove(veh->id());
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
