/****************************************************************************/
/// @file    Command_SaveTLSSwitchStates.cpp
/// @author  Daniel Krajzewicz
/// @date    08.05.2007
/// @version $Id$
///
// Writes the switch times of a tls into a file when the tls switches
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
Command_SaveTLSSwitchStates::Command_SaveTLSSwitchStates(const MSTLLogicControl::TLSLogicVariants &logics,
        OutputDevice &od) throw()
        : myOutputDevice(od), myLogics(logics)
{
    MSNet::getInstance()->getEndOfTimestepEvents().addEvent(this,
            0, MSEventControl::ADAPT_AFTER_EXECUTION);
    myOutputDevice.writeXMLHeader("tls-switch-states");
}


Command_SaveTLSSwitchStates::~Command_SaveTLSSwitchStates() throw()
{
}


SUMOTime
Command_SaveTLSSwitchStates::execute(SUMOTime currentTime) throw(ProcessError)
{
    string state = myLogics.getActive()->buildStateList();
    if (state!=myPreviousState||myLogics.getActive()->getSubID()!=myPreviousSubID) {
        myOutputDevice << "    <tlsstate time=\"" << currentTime
        << "\" id=\"" << myLogics.getActive()->getID()
        << "\" subid=\"" << myLogics.getActive()->getSubID()
        << "\" phase=\"" << myLogics.getActive()->getCurrentPhaseIndex()
        << "\">"
        << myLogics.getActive()->buildStateList() << "</tlsstate>" << "\n";
        myPreviousState = state;
        myPreviousSubID = myLogics.getActive()->getSubID();
    }
    return 1;
}



/****************************************************************************/

