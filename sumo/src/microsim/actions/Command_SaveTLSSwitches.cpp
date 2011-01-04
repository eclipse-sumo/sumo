/****************************************************************************/
/// @file    Command_SaveTLSSwitches.cpp
/// @author  Daniel Krajzewicz
/// @date    06 Jul 2006
/// @version $Id$
///
// Writes information about the green durations of a tls
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include "Command_SaveTLSSwitches.h"
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <microsim/MSLink.h>
#include <microsim/MSLane.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
Command_SaveTLSSwitches::Command_SaveTLSSwitches(const MSTLLogicControl::TLSLogicVariants &logics,
        OutputDevice &od) throw()
        : myOutputDevice(od), myLogics(logics) {
    MSNet::getInstance()->getEndOfTimestepEvents().addEvent(this, 0, MSEventControl::ADAPT_AFTER_EXECUTION);
    myOutputDevice.writeXMLHeader("tls-switches");
}


Command_SaveTLSSwitches::~Command_SaveTLSSwitches() throw() {
}


SUMOTime
Command_SaveTLSSwitches::execute(SUMOTime currentTime) throw(ProcessError) {
    MSTrafficLightLogic *light = myLogics.getActive();
    const MSTrafficLightLogic::LinkVectorVector &links = light->getLinks();
    const std::string &state = light->getCurrentPhaseDef().getState();
    for (unsigned int i=0; i<(unsigned int) links.size(); i++) {
        if (state[i]==MSLink::LINKSTATE_TL_GREEN_MAJOR||state[i]==MSLink::LINKSTATE_TL_GREEN_MINOR) {
            if (myPreviousLinkStates.find(i)==myPreviousLinkStates.end()) {
                // was not saved before
                myPreviousLinkStates[i] = currentTime;
                continue;
            }
        } else {
            if (myPreviousLinkStates.find(i)==myPreviousLinkStates.end()) {
                // was not yet green
                continue;
            }
            const MSTrafficLightLogic::LinkVector &currLinks = links[i];
            const MSTrafficLightLogic::LaneVector &currLanes = light->getLanesAt(i);
            SUMOTime lastOn = myPreviousLinkStates[i];
            for (int j=0; j<(int) currLinks.size(); j++) {
                MSLink *link = currLinks[j];
                myOutputDevice << "   <tlsswitch id=\"" << light->getID()
                << "\" programID=\"" << light->getProgramID()
                << "\" fromLane=\"" << currLanes[j]->getID()
                << "\" toLane=\"" << link->getLane()->getID()
                << "\" begin=\"" << time2string(lastOn)
                << "\" end=\"" << time2string(currentTime)
                << "\" duration=\"" << time2string(currentTime-lastOn)
                << "\"/>\n";
            }
            myPreviousLinkStates.erase(myPreviousLinkStates.find(i));
        }
    }
    return DELTA_T;
}



/****************************************************************************/

