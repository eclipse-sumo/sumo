//---------------------------------------------------------------------------//
//                        MSTriggeredReader.cpp -
//  The basic class for classes that read triggers
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2005/11/09 06:37:52  dkrajzew
// trigger reworked
//
// Revision 1.3  2005/10/17 08:58:24  dkrajzew
// trigger rework#1
//
// Revision 1.2  2005/10/07 11:37:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.1  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/02/01 09:49:25  dksumo
// got rid of MSNet::Time
//
// Revision 1.2  2005/01/06 10:48:07  dksumo
// 0.8.2.1 patches
//
// Revision 1.1  2004/10/22 12:49:26  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.4  2004/07/02 09:56:40  dkrajzew
// debugging while implementing the vss visualisation
//
// Revision 1.3  2003/09/22 14:56:06  dkrajzew
// base debugging
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <microsim/MSNet.h>
#include "MSTriggeredReader.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * MSTriggeredReader::MSTriggerCommand-methods
 * ----------------------------------------------------------------------- */
MSTriggeredReader::MSTriggerCommand::MSTriggerCommand(MSTriggeredReader &parent)
    : _parent(parent)
{
}


MSTriggeredReader::MSTriggerCommand::~MSTriggerCommand( void )
{
}


SUMOTime
MSTriggeredReader::MSTriggerCommand::execute()
{
    if(!_parent.isInitialised()) {
        _parent.init();
    }
    SUMOTime current = _parent._offset;
    SUMOTime next = current;
    // loop until the next action lies in the future
    while(current==next) {
        // run the next action
        //  if it could be accomplished...
        if(_parent.processNextEntryReaderTriggered()) {
            // read the next one
            if(_parent.readNextTriggered()) {
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
    if(_parent._offset - MSNet::getInstance()->getCurrentTimeStep()<=0) {
        // current is delayed;
        return 1;
    }
    return _parent._offset - MSNet::getInstance()->getCurrentTimeStep();
}


/* -------------------------------------------------------------------------
 * MSTriggeredReader-methods
 * ----------------------------------------------------------------------- */
MSTriggeredReader::MSTriggeredReader(MSNet &net)
    : _offset(0), myWasInitialised(false)
{
}


MSTriggeredReader::~MSTriggeredReader()
{
}


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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


