/****************************************************************************/
/// @file    ROVehicleCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for vehicles
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
#include <utils/helpers/NamedObjectCont.h>
#include <queue>
#include "ROVehicle.h"
#include "ROHelper.h"
#include "ROVehicleCont.h"

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
ROVehicleCont::ROVehicleCont()
{}


ROVehicleCont::~ROVehicleCont()
{}


priority_queue<ROVehicle*,
std::vector<ROVehicle*>, ROHelper::VehicleByDepartureComperator> &
ROVehicleCont::sort()
{
    _sorted =
        priority_queue<ROVehicle*,
        std::vector<ROVehicle*>,
        ROHelper::VehicleByDepartureComperator>();
    std::vector<ROVehicle*> v = getTempVector();
    for (std::vector<ROVehicle*>::const_iterator i=v.begin(); i!=v.end(); i++) {
        _sorted.push(*i);
    }
    return _sorted;
}



/****************************************************************************/
