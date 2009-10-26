/****************************************************************************/
/// @file    TraCIServerAPI_Junction.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id: TraCIServerAPI_Junction.cpp 6907 2009-03-13 12:13:38Z dkrajzew $
///
// APIs for getting/setting junction values via TraCI
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

#include "TraCIConstants.h"
#include <microsim/MSJunction.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSNet.h>
#include "TraCIServerAPIHelper.h"
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
TraCIServerAPI_Junction::processGet(tcpip::Storage &inputStorage,
                                    tcpip::Storage &outputStorage) throw(TraCIException) {
    Storage tmpResult;
    string warning = "";	// additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=VAR_POSITION) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_GET_JUNCTION_VARIABLE, RTYPE_ERR, "Unsupported variable specified", outputStorage);
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
            TraCIServerAPIHelper::writeStatusCmd(CMD_GET_JUNCTION_VARIABLE, RTYPE_ERR, "Junction '" + id + "' is not known", outputStorage);
            return false;
        }
        switch (variable) {
        case ID_LIST:
            break;
        case VAR_POSITION:
            tempMsg.writeUnsignedByte(TYPE_POSITION2D);
            tempMsg.writeFloat(j->getPosition().x());
            tempMsg.writeFloat(j->getPosition().y());
            break;
        default:
            break;
        }
    }
    TraCIServerAPIHelper::writeStatusCmd(CMD_GET_JUNCTION_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


/****************************************************************************/

