/****************************************************************************/
/// @file    Command_SaveTLCoupledLaneDet.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    15 Feb 2004
/// @version $Id$
///
// Writes e2 state of a link for the time the link has yellow/red
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

#include "Command_SaveTLCoupledLaneDet.h"
#include <microsim/MSNet.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <microsim/MSLinkCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
Command_SaveTLCoupledLaneDet::Command_SaveTLCoupledLaneDet(MSTLLogicControl::TLSLogicVariants& tlls,
        MSDetectorFileOutput* dtf, unsigned int begin, OutputDevice& device, MSLink* link)
    : Command_SaveTLCoupledDet(tlls, dtf, begin, device),
      myLink(link), myLastState(LINKSTATE_TL_RED),
      myHadOne(false) {
    execute();
}


Command_SaveTLCoupledLaneDet::~Command_SaveTLCoupledLaneDet() {
}


void
Command_SaveTLCoupledLaneDet::execute() {
    if (myLink->getState() == myLastState && myHadOne) {
        return;
    }
    myHadOne = true;
    if (myLastState == LINKSTATE_TL_RED && myLink->getState() != LINKSTATE_TL_RED) {
        SUMOTime end = MSNet::getInstance()->getCurrentTimeStep();
        if (myStartTime != end) {
            myDetector->writeXMLOutput(myDevice, myStartTime, end);
            myStartTime = end;
        }
    } else if (myLink->getState() == LINKSTATE_TL_RED) {
        myDetector->reset();
        myStartTime = MSNet::getInstance()->getCurrentTimeStep();
    }
    myLastState = myLink->getState();
}



/****************************************************************************/

