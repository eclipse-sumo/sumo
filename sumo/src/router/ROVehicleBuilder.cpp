//---------------------------------------------------------------------------//
//                        ROVehicleBuilder.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.6  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 12:39:02  dkrajzew
// edge-based mean data implemented; previous lane-based is now optional
//
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

#include <string>
#include "ROVehicle.h"
#include "RORunningVehicle.h"
#include "ROVehicleBuilder.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * member method definitions
 * ======================================================================= */
ROVehicleBuilder::ROVehicleBuilder()
{
}


ROVehicleBuilder::~ROVehicleBuilder()
{
}


ROVehicle *
ROVehicleBuilder::buildVehicle(const std::string &id, RORouteDef *route,
                               unsigned int depart, ROVehicleType *type,
                               const RGBColor &color, int period, int repNo)
{
    return new ROVehicle(*this, id, route, depart, type, color, period, repNo);
}


RORunningVehicle *
ROVehicleBuilder::buildRunningVehicle(const std::string &id,
                                      RORouteDef *route, SUMOTime time,
                                      ROVehicleType *type,
                                      const std::string &lane,
                                      SUMOReal pos, SUMOReal speed,
                                      const RGBColor &col, int period,
                                      int repNo)
{
    return new RORunningVehicle(*this, id, route, time, type, lane, pos, speed,
        col, period, repNo);
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

