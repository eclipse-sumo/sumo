/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Command_TriggerToCState.h
/// @author  Leonhard Luecken
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    06 Apr 2018
/// @version $Id$
///
// Trigger an MRM on the associated MSDevice_ToC
/****************************************************************************/
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Command_TriggerToCState.h"
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <microsim/devices/MSDevice_ToC.h>

// ===========================================================================
// method definitions
// ===========================================================================
Command_TriggerToCState::Command_TriggerToCState(MSDevice_ToC* tocDevice, MSDevice_ToC::ToCState targetState, SUMOTime executionTime)
    : myDevice(tocDevice), myTargetState(targetState) {
    assert(targetState == MSDevice_ToC::ToCState::MRM || targetState == MSDevice_ToC::ToCState::MANUAL || targetState == MSDevice_ToC::ToCState::AUTOMATED);
    MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(this, executionTime);
}


Command_TriggerToCState::~Command_TriggerToCState() {
}


SUMOTime
Command_TriggerToCState::execute(SUMOTime currentTime) {
    // @todo Ensure myDevice is still valid (use shared pointer??)
    if (myTargetState == MSDevice_ToC::ToCState::MRM) {
        myDevice->triggerMRM();
    } else {
        myDevice->triggerToC(myTargetState);
    }
    // Request de-scheduling the command
    return 0;
}



/****************************************************************************/
