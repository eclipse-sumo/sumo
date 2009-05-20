/****************************************************************************/
/// @file    TraCIServerAPIHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id: TraCIServerAPIHelper.cpp 6907 2009-03-13 12:13:38Z dkrajzew $
///
// Some helper functions for communication
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "TraCIConstants.h"
#include "TraCIServerAPIHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace traci;
using namespace tcpip;


// ===========================================================================
// method definitions
// ===========================================================================
void
TraCIServerAPIHelper::writeStatusCmd(int commandId, int status, 
                                     const std::string &description, tcpip::Storage &outputStorage) {
    if (status == RTYPE_ERR) {
        MsgHandler::getErrorInstance()->inform("Answered with error to command " + toString(commandId) +
                                               ": " + description);
    } else if (status == RTYPE_NOTIMPLEMENTED) {
        MsgHandler::getErrorInstance()->inform("Requested command not implemented (" + toString(commandId) +
                                               "): " + description);
    }
    // command length
    outputStorage.writeUnsignedByte(1 + 1 + 1 + 4 + static_cast<int>(description.length()));
    // command type
    outputStorage.writeUnsignedByte(commandId);
    // status
    outputStorage.writeUnsignedByte(status);
    // description
    outputStorage.writeString(description);
    return;
}



/****************************************************************************/

