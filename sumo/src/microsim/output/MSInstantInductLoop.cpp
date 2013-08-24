/****************************************************************************/
/// @file    MSInstantInductLoop.cpp
/// @author  Daniel Krajzewicz
/// @date    2011-09.08
/// @version $Id$
///
// An instantaneous induction loop
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

#include "MSInstantInductLoop.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <utils/common/WrappingCommand.h>
#include <utils/common/ToString.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSNet.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringUtils.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSInstantInductLoop::MSInstantInductLoop(const std::string& id,
        OutputDevice& od, MSLane* const lane, SUMOReal positionInMeters) :
    MSMoveReminder(id, lane),
    MSDetectorFileOutput(id),
    myOutputDevice(od),
    myPosition(positionInMeters), myLastExitTime(-1) {
    assert(myPosition >= 0 && myPosition <= myLane->getLength());
    writeXMLDetectorProlog(od);
}


MSInstantInductLoop::~MSInstantInductLoop() {
}


bool
MSInstantInductLoop::notifyMove(SUMOVehicle& veh, SUMOReal oldPos,
                                SUMOReal newPos, SUMOReal newSpeed) {
    if (newPos < myPosition) {
        // detector not reached yet
        return true;
    }
    if (newPos >= myPosition && oldPos < myPosition/* && static_cast<MSVehicle&>(veh).getLane() == myLane*/) {
        SUMOReal entryTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
        if (newSpeed != 0) {
            if (myPosition > oldPos) {
                entryTime += (myPosition - oldPos) / newSpeed;
            }
        }
        if (myLastExitTime >= 0) {
            write("enter", entryTime, veh, newSpeed, "gap", entryTime - myLastExitTime);
        } else {
            write("enter", entryTime, veh, newSpeed);
        }
        myEntryTimes[&veh] = entryTime;
        return true;
    }
    if (newPos - veh.getVehicleType().getLength() > myPosition) {
        // vehicle passed the detector
        SUMOReal leaveTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
        leaveTime += (myPosition - oldPos + veh.getVehicleType().getLength()) / newSpeed;
        std::map<SUMOVehicle*, SUMOReal>::iterator i = myEntryTimes.find(&veh);
        if (i != myEntryTimes.end()) {
            write("leave", leaveTime, veh, newSpeed, "occupancy", leaveTime - (*i).second);
            myEntryTimes.erase(i);
        } else {
            write("leave", leaveTime, veh, newSpeed);
        }
        myLastExitTime = leaveTime;
        return false;
    }
    // vehicle stays on the detector
    write("stay", STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()), veh, newSpeed);
    return true;
}


void
MSInstantInductLoop::write(const char* state, SUMOReal t, SUMOVehicle& veh, SUMOReal speed, const char* add, SUMOReal addValue) {
    myOutputDevice.openTag("instantOut").writeAttr(
        "id", getID()).writeAttr("time", toString(t)).writeAttr("state", state).writeAttr(
            "vehID", veh.getID()).writeAttr("speed", toString(speed)).writeAttr(
                "length", toString(veh.getVehicleType().getLength())).writeAttr(
                    "type", veh.getVehicleType().getID());
    if (add != 0) {
        myOutputDevice.writeAttr(add, toString(addValue));
    }
    myOutputDevice.closeTag();
}

bool
MSInstantInductLoop::notifyLeave(SUMOVehicle& veh, SUMOReal /*lastPos*/, MSMoveReminder::Notification reason) {
    std::map<SUMOVehicle*, SUMOReal>::iterator i = myEntryTimes.find(&veh);
    if (i != myEntryTimes.end()) {
        myEntryTimes.erase(i);
    }
    if (reason != MSMoveReminder::NOTIFICATION_JUNCTION) {
        write("leave", STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()), veh, veh.getSpeed());
        return false;
    }
    return true;
}


void
MSInstantInductLoop::writeXMLDetectorProlog(OutputDevice& dev) const {
    dev.writeXMLHeader("instantE1");
}



/****************************************************************************/

