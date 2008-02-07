/****************************************************************************/
/// @file    MSTriggeredReader.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The basic class for classes that read triggers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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

#include <string>
#include <microsim/MSNet.h>
#include "MSTriggeredReader.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSTriggeredReader::MSTriggerCommand-methods
 * ----------------------------------------------------------------------- */
MSTriggeredReader::MSTriggerCommand::MSTriggerCommand(MSTriggeredReader &parent) throw()
        : myParent(parent)
{}


MSTriggeredReader::MSTriggerCommand::~MSTriggerCommand() throw()
{}


SUMOTime
MSTriggeredReader::MSTriggerCommand::execute(SUMOTime current) throw(ProcessError)
{
    if (!myParent.isInitialised()) {
        myParent.init();
    }
    SUMOTime next = current;
    // loop until the next action lies in the future
    while (current==next) {
        // run the next action
        //  if it could be accomplished...
        if (myParent.processNextEntryReaderTriggered()) {
            // read the next one
            if (myParent.readNextTriggered()) {
                // set the time for comparison if a next one exists
                next = myParent.myOffset;
            } else {
                // leave if no further exists
                return 0;
            }
        } else {
            // action could not been accomplished; try next time step
            return 1;
        }
    }
    // come back if the next action shall be executed
    if (myParent.myOffset - current<=0) {
        // current is delayed;
        return 1;
    }
    return myParent.myOffset - current;
}


/* -------------------------------------------------------------------------
 * MSTriggeredReader-methods
 * ----------------------------------------------------------------------- */
MSTriggeredReader::MSTriggeredReader(MSNet &)
        : myOffset(0), myWasInitialised(false)
{}


MSTriggeredReader::~MSTriggeredReader()
{}


void
MSTriggeredReader::init()
{
    myInit();
    myWasInitialised = true;
}


bool
MSTriggeredReader::isInitialised() const
{
    return myWasInitialised;
}



/****************************************************************************/

