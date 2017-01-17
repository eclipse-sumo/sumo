/****************************************************************************/
/// @file    MEVehicleControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles (meso-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/iodevices/OutputDevice.h>
#include <microsim/MSNet.h>
#include <microsim/MSRouteHandler.h>
#include <microsim/MSVehicleType.h>
#include "MESegment.h"
#include "MEVehicle.h"
#include "MEVehicleControl.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MEVehicleControl::MEVehicleControl()
    : MSVehicleControl() {}


MEVehicleControl::~MEVehicleControl() {}


SUMOVehicle*
MEVehicleControl::buildVehicle(SUMOVehicleParameter* defs,
                               const MSRoute* route, const MSVehicleType* type,
                               const bool ignoreStopErrors, const bool fromRouteFile) {
    myLoadedVehNo++;
    if (fromRouteFile) {
        defs->depart += computeRandomDepartOffset();
    }
    MEVehicle* built = new MEVehicle(defs, route, type, type->computeChosenSpeedDeviation(fromRouteFile ? MSRouteHandler::getParsingRNG() : 0));
    built->addStops(ignoreStopErrors);
    MSNet::getInstance()->informVehicleStateListener(built, MSNet::VEHICLE_STATE_BUILT);
    return built;
}


/****************************************************************************/

