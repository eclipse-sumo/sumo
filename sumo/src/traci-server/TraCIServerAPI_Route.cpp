/****************************************************************************/
/// @file    TraCIServerAPI_Route.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id: TraCIServerAPI_Route.cpp 6907 2009-03-13 12:13:38Z dkrajzew $
///
// APIs for getting/setting route values via TraCI
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

#include <microsim/MSNet.h>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include "TraCIConstants.h"
#include "TraCIServerAPIHelper.h"
#include "TraCIServerAPI_Route.h"

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
TraCIServerAPI_Route::processGet(tcpip::Storage &inputStorage,
                                 tcpip::Storage &outputStorage,
                                 bool withStatus) throw(TraCIException) {
    string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=VAR_EDGES) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_GET_ROUTE_VARIABLE, RTYPE_ERR, "Unsupported variable specified", outputStorage);
        return false;
    }
    // begin response building
    Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_ROUTE_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable==ID_LIST) {
        std::vector<std::string> ids;
        MSRoute::insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
        const MSRoute *r = MSRoute::dictionary(id);
        if (r==0) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_GET_ROUTE_VARIABLE, RTYPE_ERR, "Route '" + id + "' is not known", outputStorage);
            return false;
        }
        switch (variable) {
        case VAR_EDGES:
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeInt(r->size());
            for (MSRouteIterator i=r->begin(); i!=r->end(); ++i) {
                tempMsg.writeString((*i)->getID());
            }
            break;
        default:
            break;
        }
    }
    if (withStatus) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_GET_ROUTE_VARIABLE, RTYPE_OK, warning, outputStorage);
    }
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


bool
TraCIServerAPI_Route::processSet(tcpip::Storage &inputStorage,
                                 tcpip::Storage &outputStorage) throw(TraCIException) {
    return true;
}



/****************************************************************************/

