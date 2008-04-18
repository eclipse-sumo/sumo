/****************************************************************************/
/// @file    ROVehicleBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
// Router vehicles building factory
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "ROVehicle.h"
#include "ROVehicleBuilder.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
ROVehicleBuilder::ROVehicleBuilder() throw()
{}


ROVehicleBuilder::~ROVehicleBuilder() throw()
{}


ROVehicle *
ROVehicleBuilder::buildVehicle(const SUMOVehicleParameter &pars, 
              RORouteDef *route, ROVehicleType *type) throw()
{
    return new ROVehicle(*this, pars, route, type);
}


/****************************************************************************/

