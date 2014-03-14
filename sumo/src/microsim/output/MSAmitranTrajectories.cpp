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
// static member definitions
// ===========================================================================
std::map<std::string, int> MSAmitranTrajectories::myWrittenTypes;
std::map<std::string, int> MSAmitranTrajectories::myWrittenVehicles;


// ===========================================================================
// method definitions
// ===========================================================================
void
MSAmitranTrajectories::write(OutputDevice& of, const SUMOTime timestep) {
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    for (MSVehicleControl::constVehIt v = vc.loadedVehBegin(); v != vc.loadedVehEnd(); ++v) {
        writeVehicle(of, *v->second, timestep);
    }
}


void
MSAmitranTrajectories::writeVehicle(OutputDevice& of, const SUMOVehicle& veh, const SUMOTime timestep) {
    if (veh.isOnRoad()) {
        const std::string& type = veh.getVehicleType().getID();
        if (myWrittenTypes.count(type) == 0) {
            const int index = (int)myWrittenTypes.size();
            of.openTag("actorConfig") << " id=\"" << index << "\"";
            of.closeTag();
            myWrittenTypes[type] = index;
        }
        if (myWrittenVehicles.count(veh.getID()) == 0) {
            const int index = (int)myWrittenVehicles.size();
            of.openTag("vehicle") << " id=\"" << index
                                  << "\" actorConfig=\"" << myWrittenTypes[type]
                                  << "\" startTime=\"" << time2string(veh.getDeparture()) << "\"";
            of.closeTag();
            myWrittenVehicles[veh.getID()] = index;
        }
        of.openTag("motionState") << " vehicle=\"" << myWrittenVehicles[veh.getID()]
                                  << "\" speed=\"" << int(100*veh.getSpeed() + 0.5)
                                  << "\" time=\"" << timestep << "\"";
        of.closeTag();
    }
}


/****************************************************************************/
