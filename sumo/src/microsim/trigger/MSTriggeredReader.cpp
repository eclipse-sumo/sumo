/****************************************************************************/
/// @file    MSTriggeredReader.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The basic class for classes that read triggers
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

#include <string>
#include <microsim/MSNet.h>
#include "MSTriggeredReader.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSTriggeredReader::MSTriggeredReader(MSNet&)
    : myOffset(0), myWasInitialised(false) {}


MSTriggeredReader::~MSTriggeredReader() {}


void
MSTriggeredReader::init() {
    myInit();
    myWasInitialised = true;
}


bool
MSTriggeredReader::isInitialised() const {
    return myWasInitialised;
}


SUMOTime
MSTriggeredReader::wrappedExecute(SUMOTime current) {
    if (!isInitialised()) {
        init();
    }
    SUMOTime next = current;
    // loop until the next action lies in the future
    while (current == next) {
        // run the next action
        //  if it could be accomplished...
        if (processNextEntryReaderTriggered()) {
            // read the next one
            if (readNextTriggered()) {
                // set the time for comparison if a next one exists
                next = myOffset;
            } else {
                // leave if no further exists
                return 0;
            }
        } else {
            // action could not be accomplished; try next time step
            return DELTA_T;
        }
    }
    // come back if the next action shall be executed
    if (myOffset - current <= 0) {
        // current is delayed;
        return DELTA_T;
    }
    return myOffset - current;
}



/****************************************************************************/

