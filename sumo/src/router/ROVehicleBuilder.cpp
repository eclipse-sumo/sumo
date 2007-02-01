/****************************************************************************/
/// @file    ROVehicleBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id: $
///
//
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "ROVehicle.h"
#include "RORunningVehicle.h"
#include "ROVehicleBuilder.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// member method definitions
// ===========================================================================
ROVehicleBuilder::ROVehicleBuilder()
{}


ROVehicleBuilder::~ROVehicleBuilder()
{}


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



/****************************************************************************/

