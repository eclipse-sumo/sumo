/****************************************************************************/
/// @file    TraCIServerAPI_Junction.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting junction values via TraCI
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

#include "TraCIConstants.h"
#include <microsim/MSJunction.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSNet.h>
#include "TraCIServerAPI_Junction.h"

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
bool
TraCIServerAPI_Junction::processGet(TraCIServer &server, tcpip::Storage &inputStorage,
                                    tcpip::Storage &outputStorage) {
    Storage tmpResult;
    std::string warning = "";	// additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=VAR_POSITION) {
        server.writeStatusCmd(CMD_GET_JUNCTION_VARIABLE, RTYPE_ERR, "Get Junction Variable: unsupported variable specified", outputStorage);
        return false;
    }
    // begin response building
    Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_JUNCTION_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    if (variable==ID_LIST) {
        std::vector<std::string> ids;
        MSNet::getInstance()->getJunctionControl().insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
        MSJunction *j = MSNet::getInstance()->getJunctionControl().get(id);
        if (j==0) {
            server.writeStatusCmd(CMD_GET_JUNCTION_VARIABLE, RTYPE_ERR, "Junction '" + id + "' is not known", outputStorage);
            return false;
        }
        switch (variable) {
        case ID_LIST:
            break;
        case VAR_POSITION:
            tempMsg.writeUnsignedByte(POSITION_2D);
            tempMsg.writeFloat((float)(j->getPosition().x()));
            tempMsg.writeFloat((float)(j->getPosition().y()));
            break;
        default:
            break;
        }
    }
    server.writeStatusCmd(CMD_GET_JUNCTION_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + (int)tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


/****************************************************************************/

