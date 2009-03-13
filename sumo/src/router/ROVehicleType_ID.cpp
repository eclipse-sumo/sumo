/****************************************************************************/
/// @file    ROVehicleType_ID.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 07.12.2005
/// @version $Id$
///
// A type which is only described by its id (other values are invalid)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <iostream>
#include "ROVehicleType.h"
#include "ROVehicleType_ID.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;



// ===========================================================================
// method definitions
// ===========================================================================
ROVehicleType_ID::ROVehicleType_ID(const std::string &id) throw()
        : ROVehicleType(id, "", 1, SVC_UNKNOWN) {}


ROVehicleType_ID::~ROVehicleType_ID() throw() {}


OutputDevice &
ROVehicleType_ID::writeXMLDefinition(OutputDevice &dev) const {
    return dev;
}



/****************************************************************************/

