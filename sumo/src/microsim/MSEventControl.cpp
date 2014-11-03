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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include <cassert>
#include "MSEventControl.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/Command.h>
#include "MSNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member definitions
// ===========================================================================
MSEventControl::MSEventControl()
    : currentTimeStep(-1), myEvents() {}


MSEventControl::~MSEventControl() {
    // delete the events
    while (! myEvents.empty()) {
        Event e = myEvents.top();
        delete e.first;
        myEvents.pop();
    }
}


SUMOTime
MSEventControl::addEvent(Command* operation,
                         SUMOTime execTimeStep,
                         AdaptType type) {
    SUMOTime currTimeStep = getCurrentTimeStep();
    if (type == ADAPT_AFTER_EXECUTION && execTimeStep <= currTimeStep) {
        execTimeStep = currTimeStep;
    }
    Event newEvent = Event(operation, execTimeStep);
    myEvents.push(newEvent);
    return execTimeStep;
}


void
MSEventControl::execute(SUMOTime execTime) {
    // Execute all events that are scheduled for execTime.
    for (; !myEvents.empty();) {
        Event currEvent = myEvents.top();
        if (currEvent.second == execTime || currEvent.second < execTime + DELTA_T) {
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
            if (currEvent.second < execTime) {
                // !!! more verbose information
                WRITE_WARNING("Could not execute scheduled event.");
                delete currEvent.first;
                myEvents.pop();
            } else {
                break;
            }
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

