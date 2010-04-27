/****************************************************************************/
/// @file    Command_SaveTLSState.cpp
/// @author  Daniel Krajzewicz
/// @date    15 Feb 2004
/// @version $Id$
///
// Writes the state of the tls to a file (in each second)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include "Command_SaveTLSState.h"
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
Command_SaveTLSState::Command_SaveTLSState(const MSTLLogicControl::TLSLogicVariants &logics,
        OutputDevice &od) throw()
        : myOutputDevice(od), myLogics(logics) {
    MSNet::getInstance()->getEndOfTimestepEvents().addEvent(this,
            0, MSEventControl::ADAPT_AFTER_EXECUTION);
    myOutputDevice.writeXMLHeader("tls-states");
}


Command_SaveTLSState::~Command_SaveTLSState() throw() {
}


SUMOTime
Command_SaveTLSState::execute(SUMOTime currentTime) throw(ProcessError) {
    myOutputDevice << "    <tlsstate time=\"" << time2string(currentTime)
    << "\" id=\"" << myLogics.getActive()->getID()
    << "\" subid=\"" << myLogics.getActive()->getSubID()
    << "\" phase=\"" << myLogics.getActive()->getCurrentPhaseIndex()
    << "\" state=\"" << myLogics.getActive()->getCurrentPhaseDef().getState() << "\"/>\n";
    return DELTA_T;
}



/****************************************************************************/
