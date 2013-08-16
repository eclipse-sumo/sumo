/****************************************************************************/
/// @file    GUIVehicleControl.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <utils/foxtools/MFXMutex.h>
#include "GUIVehicleControl.h"
#include "GUIVehicle.h"
#include "GUINet.h"
#include <gui/GUIGlobals.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GUIVehicleControl::GUIVehicleControl()
    : MSVehicleControl() {}


GUIVehicleControl::~GUIVehicleControl() {
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


SUMOVehicle*
GUIVehicleControl::buildVehicle(SUMOVehicleParameter* defs,
                                const MSRoute* route, const MSVehicleType* type) {
    myLoadedVehNo++;
    MSVehicle* built = new GUIVehicle(defs, route, type, type->computeChosenSpeedDeviation(myVehicleParamsRNG), myLoadedVehNo - 1);
    MSNet::getInstance()->informVehicleStateListener(built, MSNet::VEHICLE_STATE_BUILT);
    return built;
}


bool
GUIVehicleControl::addVehicle(const std::string& id, SUMOVehicle* v) {
    AbstractMutex::ScopedLocker locker(myLock);
    return MSVehicleControl::addVehicle(id, v);
}


void
GUIVehicleControl::deleteVehicle(SUMOVehicle* veh, bool discard) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSVehicleControl::deleteVehicle(veh, discard);
}


void
GUIVehicleControl::insertVehicleIDs(std::vector<GUIGlID>& into) {
    AbstractMutex::ScopedLocker locker(myLock);
    into.reserve(myVehicleDict.size());
    for (VehicleDictType::iterator i = myVehicleDict.begin(); i != myVehicleDict.end(); ++i) {
        SUMOVehicle* veh = (*i).second;
        if (veh->isOnRoad()) {
            into.push_back(static_cast<GUIVehicle*>((*i).second)->getGlID());
        }
    }
}


void
GUIVehicleControl::secureVehicles() {
    myLock.lock();
}


void
GUIVehicleControl::releaseVehicles() {
    myLock.unlock();
}



/****************************************************************************/

