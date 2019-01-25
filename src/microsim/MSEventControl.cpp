/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSEventControl.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Matthias Heppner
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Stores time-dependant events and executes them at the proper time
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <cassert>
#include "MSEventControl.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/Command.h>
#include "MSNet.h"


// ===========================================================================
// member definitions
// ===========================================================================
MSEventControl::MSEventControl()
    : currentTimeStep(-1), myEvents() {}


MSEventControl::~MSEventControl() {
    // delete the events
    while (!myEvents.empty()) {
        Event e = myEvents.top();
        delete e.first;
        myEvents.pop();
    }
}


void
MSEventControl::addEvent(Command* operation, SUMOTime execTimeStep) {
    myEvents.push(Event(operation, execTimeStep));
}


void
MSEventControl::execute(SUMOTime execTime) {
    // Execute all events that are scheduled for execTime.
    while (!myEvents.empty()) {
        Event currEvent = myEvents.top();
        if (currEvent.second < 0) {
            currEvent.second = execTime;
        }
        if (currEvent.second < execTime + DELTA_T) {
            Command* command = currEvent.first;
            myEvents.pop();
            SUMOTime time = 0;
            try {
                time = command->execute(execTime);
            } catch (...) {
                delete command;
                throw;
            }

            // Delete nonrecurring events, reinsert recurring ones
            // with new execution time = execTime + returned offset.
            if (time <= 0) {
                if (time < 0) {
                    WRITE_WARNING("Command returned negative repeat number; will be deleted.");
                }
                delete currEvent.first;
            } else {
                currEvent.second += time;
                myEvents.push(currEvent);
            }
        } else {
            break;
        }
    }
}


bool
MSEventControl::isEmpty() {
    return myEvents.empty();
}

void
MSEventControl::setCurrentTimeStep(SUMOTime time) {
    currentTimeStep = time;
}

SUMOTime
MSEventControl::getCurrentTimeStep() {
    if (currentTimeStep < 0) {
        return MSNet::getInstance()->getCurrentTimeStep();
    }
    return currentTimeStep;
}



/****************************************************************************/

