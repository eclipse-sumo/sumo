/****************************************************************************/
/// @file    GUIVehicleControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include "GUIVehicleControl.h"
#include "GUIVehicle.h"
#include "GUINet.h"
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GUIVehicleControl::GUIVehicleControl() throw()
        : MSVehicleControl() {}


GUIVehicleControl::~GUIVehicleControl() throw() {}


SUMOVehicle *
GUIVehicleControl::buildVehicle(SUMOVehicleParameter* defs,
                                const MSRoute* route, const MSVehicleType* type) throw(ProcessError) {
    myLoadedVehNo++;
    MSVehicle *built = new GUIVehicle(GUIGlObjectStorage::gIDStorage, defs, route, type, myLoadedVehNo-1);
    MSNet::getInstance()->informVehicleStateListener(built, MSNet::VEHICLE_STATE_BUILT);
    return built;
}


void
GUIVehicleControl::insertVehicleIDs(std::vector<GLuint> &into) throw() {
    into.reserve(myVehicleDict.size());
    for (VehicleDictType::iterator i=myVehicleDict.begin(); i!=myVehicleDict.end(); ++i) {
        SUMOVehicle *veh = (*i).second;
        if (veh->isOnRoad()) {
            into.push_back(static_cast<GUIVehicle*>((*i).second)->getGlID());
        }
    }
}



/****************************************************************************/

