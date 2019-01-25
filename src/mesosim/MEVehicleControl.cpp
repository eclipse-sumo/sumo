/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MEVehicleControl.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles (meso-version)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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


// ===========================================================================
// member method definitions
// ===========================================================================
MEVehicleControl::MEVehicleControl()
    : MSVehicleControl() {}


MEVehicleControl::~MEVehicleControl() {}


SUMOVehicle*
MEVehicleControl::buildVehicle(SUMOVehicleParameter* defs,
                               const MSRoute* route, MSVehicleType* type,
                               const bool ignoreStopErrors, const bool fromRouteFile) {
    myLoadedVehNo++;
    MEVehicle* built = new MEVehicle(defs, route, type, type->computeChosenSpeedDeviation(fromRouteFile ? MSRouteHandler::getParsingRNG() : nullptr));
    built->addStops(ignoreStopErrors);
    MSNet::getInstance()->informVehicleStateListener(built, MSNet::VEHICLE_STATE_BUILT);
    return built;
}


/****************************************************************************/

