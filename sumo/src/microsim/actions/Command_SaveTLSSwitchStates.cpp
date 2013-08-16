/****************************************************************************/
/// @file    Command_SaveTLSSwitchStates.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    08.05.2007
/// @version $Id$
///
// Writes the switch times of a tls into a file when the tls switches
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

#include "Command_SaveTLSSwitchStates.h"
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
Command_SaveTLSSwitchStates::Command_SaveTLSSwitchStates(const MSTLLogicControl::TLSLogicVariants& logics,
        OutputDevice& od)
    : myOutputDevice(od), myLogics(logics) {
    MSNet::getInstance()->getEndOfTimestepEvents().addEvent(this, 0, MSEventControl::ADAPT_AFTER_EXECUTION);
    myOutputDevice.writeXMLHeader("tls-switch-states");
}


Command_SaveTLSSwitchStates::~Command_SaveTLSSwitchStates() {
}


SUMOTime
Command_SaveTLSSwitchStates::execute(SUMOTime currentTime) {
    const std::string& state = myLogics.getActive()->getCurrentPhaseDef().getState();
    if (state != myPreviousState || myLogics.getActive()->getProgramID() != myPreviousProgramID) {
        myOutputDevice << "    <tlsstate time=\"" << time2string(currentTime)
                       << "\" id=\"" << myLogics.getActive()->getID()
                       << "\" programID=\"" << myLogics.getActive()->getProgramID()
                       << "\" phase=\"" << myLogics.getActive()->getCurrentPhaseIndex()
                       << "\" state=\"" << state << "\"/>\n";
        myPreviousState = state;
        myPreviousProgramID = myLogics.getActive()->getProgramID();
    }
    return DELTA_T;
}



/****************************************************************************/

