/****************************************************************************/
/// @file    MSEventControl.cpp
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Stores time-dependant events and executes them at the proper time
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
MSEventControl::MSEventControl() throw()
    : currentTimeStep(-1), myEvents() {}


MSEventControl::~MSEventControl() throw() {
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
                         AdaptType type) throw() {
    SUMOTime currTimeStep = getCurrentTimeStep();
    if (type == ADAPT_AFTER_EXECUTION && execTimeStep <= currTimeStep) {
        execTimeStep = currTimeStep;
    }
    Event newEvent = Event(operation, execTimeStep);
    myEvents.push(newEvent);
    return execTimeStep;
}


void
MSEventControl::execute(SUMOTime execTime) throw(ProcessError) {
    // Execute all events that are scheduled for execTime.
    for (; !myEvents.empty();) {
        Event currEvent = myEvents.top();
        if (currEvent.second == execTime || currEvent.second<execTime+DELTA_T) {
            Command *command = currEvent.first;
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
                if (time<0) {
                    WRITE_WARNING("Command returned negative repeat number; will be deleted.");
                }
                delete currEvent.first;
            } else {
                currEvent.second = execTime + time;
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
MSEventControl::isEmpty() throw() {
    return myEvents.empty();
}

void
MSEventControl::setCurrentTimeStep(SUMOTime time) {
    currentTimeStep = time;
}

SUMOTime
MSEventControl::getCurrentTimeStep() throw() {
    if (currentTimeStep < 0) {
        return MSNet::getInstance()->getCurrentTimeStep();
    }
    return currentTimeStep;
}



/****************************************************************************/

