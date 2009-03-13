/****************************************************************************/
/// @file    Command_SaveTLCoupledLaneDet.cpp
/// @author  Daniel Krajzewicz
/// @date    15 Feb 2004
/// @version $Id$
///
// Writes e2-state of a link for the time the link has yellow/red
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
Command_SaveTLCoupledLaneDet::Command_SaveTLCoupledLaneDet(MSTLLogicControl::TLSLogicVariants &tlls,
        MSDetectorFileOutput *dtf,
        unsigned int begin,
        OutputDevice& device,
        MSLink *link) throw()
        : Command_SaveTLCoupledDet(tlls, dtf, begin, device),
        myLink(link), myLastState(MSLink::LINKSTATE_TL_RED),
        myHadOne(false) {}


Command_SaveTLCoupledLaneDet::~Command_SaveTLCoupledLaneDet() throw() {
}


void
Command_SaveTLCoupledLaneDet::execute() throw() {
    // !!! we have to do this to have the correct information set
    myLogics.getActive()->maskRedLinks();
    if (myLink->getState()==myLastState&&myHadOne) {
        return;
    }
    myHadOne = true;
    if (myLastState==MSLink::LINKSTATE_TL_RED&&myLink->getState()!=MSLink::LINKSTATE_TL_RED) {
        SUMOTime end = MSNet::getInstance()->getCurrentTimeStep();
        if (myStartTime!=end) {
            myDetector->writeXMLOutput(myDevice, myStartTime, end);
            myStartTime = end;
        }
    }
    if (myLink->getState()!=MSLink::LINKSTATE_TL_RED) {
        myDetector->reset();
        myStartTime = MSNet::getInstance()->getCurrentTimeStep();
    }
    myLastState = myLink->getState();
}



/****************************************************************************/

