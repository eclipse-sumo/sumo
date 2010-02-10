/****************************************************************************/
/// @file    Command_SaveTLCoupledDet.cpp
/// @author  Daniel Krajzewicz
/// @date    15 Feb 2004
/// @version $Id$
///
// Writes e2-state on each tls switch
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

#include "Command_SaveTLCoupledDet.h"
#include <microsim/MSNet.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/output/MSDetectorFileOutput.h>
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
Command_SaveTLCoupledDet::Command_SaveTLCoupledDet(MSTLLogicControl::TLSLogicVariants &tlls,
        MSDetectorFileOutput *dtf,
        unsigned int begin,
        OutputDevice& device) throw()
        : myDevice(device), myLogics(tlls), myDetector(dtf),
        myStartTime(begin) {
    tlls.addSwitchCommand(this);
    dtf->writeXMLDetectorProlog(device);
}


Command_SaveTLCoupledDet::~Command_SaveTLCoupledDet() throw() {
}


void
Command_SaveTLCoupledDet::execute() throw() {
    SUMOTime end = MSNet::getInstance()->getCurrentTimeStep();
    if (myStartTime!=end) {
        myDetector->writeXMLOutput(myDevice, myStartTime, end);
        myStartTime = end;
    }
}



/****************************************************************************/

