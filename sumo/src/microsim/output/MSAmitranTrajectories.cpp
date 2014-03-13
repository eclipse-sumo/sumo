/****************************************************************************/
/// @file    MSAmitranTrajectories.cpp
/// @author  Michael Behrisch
/// @date    13.03.2014
/// @version $Id$
///
// Realises dumping the complete network state
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSVehicleControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSGlobals.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSAmitranTrajectories.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
MSAmitranTrajectories::write(OutputDevice& of, const MSVehicleControl& vc,
                             const SUMOTime timestep) {
    for (MSVehicleControl::constVehIt v = vc.loadedVehBegin(); v != vc.loadedVehEnd(); ++v) {
        writeVehicle(of, *v->second);
    }
}


void
MSAmitranTrajectories::writeVehicle(OutputDevice& of, const SUMOVehicle& veh) {
    if (veh.isOnRoad()) {
        of.openTag("vehicle") << " id=\"" << veh.getID() << "\" pos=\""
                              << veh.getPositionOnLane() << "\" speed=\"" << veh.getSpeed() << "\"";
        of.closeTag();
    }
}


/****************************************************************************/
