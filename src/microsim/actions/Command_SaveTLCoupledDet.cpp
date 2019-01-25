/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Command_SaveTLCoupledDet.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    15 Feb 2004
/// @version $Id$
///
// Writes e2 state on each tls switch
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "Command_SaveTLCoupledDet.h"
#include <microsim/MSNet.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// method definitions
// ===========================================================================
Command_SaveTLCoupledDet::Command_SaveTLCoupledDet(MSTLLogicControl::TLSLogicVariants& tlls,
        MSDetectorFileOutput* dtf, SUMOTime begin, OutputDevice& device)
    : myDevice(device), myLogics(tlls), myDetector(dtf),
      myStartTime(begin) {
    tlls.addSwitchCommand(this);
    dtf->writeXMLDetectorProlog(device);
}


Command_SaveTLCoupledDet::~Command_SaveTLCoupledDet() {
}


void
Command_SaveTLCoupledDet::execute() {
    SUMOTime end = MSNet::getInstance()->getCurrentTimeStep();
    if (myStartTime != end) {
        myDetector->writeXMLOutput(myDevice, myStartTime, end);
        myStartTime = end;
    }
}



/****************************************************************************/

