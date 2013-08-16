/****************************************************************************/
/// @file    TraCIServerAPI_Route.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting route values via TraCI
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

#ifndef NO_TRACI

#include <microsim/MSNet.h>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_Route.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace traci;


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Route::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                 tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != VAR_EDGES && variable != ID_COUNT) {
        return server.writeErrorStatusCmd(CMD_GET_ROUTE_VARIABLE, "Get Route Variable: unsupported variable specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_ROUTE_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable == ID_LIST) {
        std::vector<std::string> ids;
        MSRoute::insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else if (variable == ID_COUNT) {
        std::vector<std::string> ids;
        MSRoute::insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt((int) ids.size());
    } else {
        const MSRoute* r = MSRoute::dictionary(id);
        if (r == 0) {
            return server.writeErrorStatusCmd(CMD_GET_ROUTE_VARIABLE, "Route '" + id + "' is not known", outputStorage);
        }
        switch (variable) {
            case VAR_EDGES:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeInt(r->size());
                for (MSRouteIterator i = r->begin(); i != r->end(); ++i) {
                    tempMsg.writeString((*i)->getID());
                }
                break;
            default:
                break;
        }
    }
    server.writeStatusCmd(CMD_GET_ROUTE_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


bool
TraCIServerAPI_Route::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                 tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != ADD) {
        return server.writeErrorStatusCmd(CMD_SET_ROUTE_VARIABLE, "Change Route State: unsupported variable specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    // process
    switch (variable) {
        case ADD: {
            std::vector<std::string> edgeIDs;
            if (!server.readTypeCheckingStringList(inputStorage, edgeIDs)) {
                return server.writeErrorStatusCmd(CMD_SET_ROUTE_VARIABLE, "A string list is needed for adding a new route.", outputStorage);
            }
            //read itemNo
            MSEdgeVector edges;
            for (std::vector<std::string>::const_iterator i = edgeIDs.begin(); i != edgeIDs.end(); ++i) {
                MSEdge* edge = MSEdge::dictionary(*i);
                if (edge == 0) {
                    return server.writeErrorStatusCmd(CMD_SET_ROUTE_VARIABLE, "Unknown edge '" + *i + "' in route.", outputStorage);
                }
                edges.push_back(edge);
            }
            const std::vector<SUMOVehicleParameter::Stop> stops;
            if (!MSRoute::dictionary(id, new MSRoute(id, edges, 1, 0, stops))) {
                return server.writeErrorStatusCmd(CMD_SET_ROUTE_VARIABLE, "Could not add route.", outputStorage);
            }
        }
        break;
        default:
            break;
    }
    server.writeStatusCmd(CMD_SET_ROUTE_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}

#endif


/****************************************************************************/

