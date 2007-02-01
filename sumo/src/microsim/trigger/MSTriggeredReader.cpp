/****************************************************************************/
/// @file    MSTriggeredReader.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <microsim/MSNet.h>
#include "MSTriggeredReader.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSTriggeredReader::MSTriggerCommand-methods
 * ----------------------------------------------------------------------- */
MSTriggeredReader::MSTriggerCommand::MSTriggerCommand(MSTriggeredReader &parent)
        : _parent(parent)
{}


MSTriggeredReader::MSTriggerCommand::~MSTriggerCommand(void)
{}


SUMOTime
MSTriggeredReader::MSTriggerCommand::execute(SUMOTime current)
{
    if (!_parent.isInitialised()) {
        _parent.init();
    }
    SUMOTime next = current;
    // loop until the next action lies in the future
    while (current==next) {
        // run the next action
        //  if it could be accomplished...
        if (_parent.processNextEntryReaderTriggered()) {
            // read the next one
            if (_parent.readNextTriggered()) {
                // set the time for comparison if a next one exists
                next = _parent._offset;
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
    if (_parent._offset - current<=0) {
        // current is delayed;
        return 1;
    }
    return _parent._offset - current;
}


/* -------------------------------------------------------------------------
 * MSTriggeredReader-methods
 * ----------------------------------------------------------------------- */
MSTriggeredReader::MSTriggeredReader(MSNet &)
        : _offset(0), myWasInitialised(false)
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

