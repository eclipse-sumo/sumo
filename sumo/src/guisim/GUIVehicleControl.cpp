/****************************************************************************/
/// @file    GUIVehicleControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles (gui-version)
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

#include <microsim/MSCORN.h>
#include "GUIVehicleControl.h"
#include "GUIVehicle.h"
#include "GUINet.h"
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GUIVehicleControl::GUIVehicleControl() throw()
        : MSVehicleControl() {}


GUIVehicleControl::~GUIVehicleControl() throw() {}


MSVehicle *
GUIVehicleControl::buildVehicle(SUMOVehicleParameter* defs,
                                const MSRoute* route, const MSVehicleType* type) throw() {
    myLoadedVehNo++;
    return new GUIVehicle(gIDStorage, defs, route, type, myLoadedVehNo-1);
}


void
GUIVehicleControl::deleteVehicle(MSVehicle *veh) throw() {
    static_cast<GUIVehicle*>(veh)->setRemoved();
    if (gIDStorage.remove(static_cast<GUIVehicle*>(veh)->getGlID())) {
        MSVehicleControl::deleteVehicle(veh);
    }
}


void
GUIVehicleControl::insertVehicleIDs(std::vector<GLuint> &into) throw() {
    into.reserve(myVehicleDict.size());
    for (VehicleDictType::iterator i=myVehicleDict.begin(); i!=myVehicleDict.end(); ++i) {
        MSVehicle *veh = (*i).second;
        if (veh->isOnRoad()) {
            into.push_back(static_cast<GUIVehicle*>((*i).second)->getGlID());
        }
    }
}



/****************************************************************************/

