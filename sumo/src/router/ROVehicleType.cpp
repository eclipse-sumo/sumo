/****************************************************************************/
/// @file    ROVehicleType.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Base class for routers' representation of vehicle types
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

#include <iostream>
#include <string>
#include "ReferencedItem.h"
#include "ROVehicleType.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROVehicleType::ROVehicleType(const std::string &id, const std::string &col,
                             SUMOReal length, SUMOVehicleClass vclass)
        : ReferencedItem(), myID(id), myColor(col), myLength(length),
        myClass(vclass)
{}


ROVehicleType::~ROVehicleType()
{}


std::string
ROVehicleType::getID() const
{
    return myID;
}


SUMOReal
ROVehicleType::getLength() const
{
    return myLength;
}


SUMOVehicleClass
ROVehicleType::getClass() const
{
    return myClass;
}

/****************************************************************************/

