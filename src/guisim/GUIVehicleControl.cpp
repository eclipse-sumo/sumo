/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <microsim/MSRouteHandler.h>
#include "GUIVehicleControl.h"
#include "GUIVehicle.h"
#include "GUINet.h"
#include <gui/GUIGlobals.h>


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
                                const MSRoute* route, MSVehicleType* type,
                                const bool ignoreStopErrors, const bool fromRouteFile) {
    myLoadedVehNo++;
    MSVehicle* built = new GUIVehicle(defs, route, type, type->computeChosenSpeedDeviation(fromRouteFile ? MSRouteHandler::getParsingRNG() : nullptr));
    built->addStops(ignoreStopErrors);
    MSNet::getInstance()->informVehicleStateListener(built, MSNet::VEHICLE_STATE_BUILT);
    return built;
}


bool
GUIVehicleControl::addVehicle(const std::string& id, SUMOVehicle* v) {
    FXMutexLock locker(myLock);
    return MSVehicleControl::addVehicle(id, v);
}


void
GUIVehicleControl::deleteVehicle(SUMOVehicle* veh, bool discard) {
    FXMutexLock locker(myLock);
    MSVehicleControl::deleteVehicle(veh, discard);
}


int
GUIVehicleControl::getHaltingVehicleNo() const {
    FXMutexLock locker(myLock);
    return MSVehicleControl::getHaltingVehicleNo();
}


std::pair<double, double>
GUIVehicleControl::getVehicleMeanSpeeds() const {
    FXMutexLock locker(myLock);
    return MSVehicleControl::getVehicleMeanSpeeds();
}


void
GUIVehicleControl::insertVehicleIDs(std::vector<GUIGlID>& into, bool listParking, bool listTeleporting) {
    FXMutexLock locker(myLock);
    into.reserve(myVehicleDict.size());
    for (VehicleDictType::iterator i = myVehicleDict.begin(); i != myVehicleDict.end(); ++i) {
        SUMOVehicle* veh = (*i).second;
        if (veh->isOnRoad() || (listParking && veh->isParking()) || listTeleporting) {
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

