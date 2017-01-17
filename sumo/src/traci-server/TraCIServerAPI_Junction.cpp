/****************************************************************************/
/// @file    TraCIServerAPI_Junction.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting junction values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2009-2017 DLR (http://www.dlr.de/) and contributors
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

#include "TraCIConstants.h"
#include <microsim/MSNet.h>
#include <microsim/MSJunction.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSNet.h>
#include "TraCIServerAPI_Junction.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Junction::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                    tcpip::Storage& outputStorage) {
    // variable
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != VAR_POSITION && variable != ID_COUNT && variable != VAR_SHAPE) {
        return server.writeErrorStatusCmd(CMD_GET_JUNCTION_VARIABLE, "Get Junction Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_JUNCTION_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    if (variable == ID_LIST) {
        std::vector<std::string> ids;
        MSNet::getInstance()->getJunctionControl().insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else if (variable == ID_COUNT) {
        std::vector<std::string> ids;
        MSNet::getInstance()->getJunctionControl().insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt((int) ids.size());
    } else {
        MSJunction* j = MSNet::getInstance()->getJunctionControl().get(id);
        if (j == 0) {
            return server.writeErrorStatusCmd(CMD_GET_JUNCTION_VARIABLE, "Junction '" + id + "' is not known", outputStorage);
        }
        switch (variable) {
            case ID_LIST:
                break;
            case VAR_POSITION:
                tempMsg.writeUnsignedByte(POSITION_2D);
                tempMsg.writeDouble(j->getPosition().x());
                tempMsg.writeDouble(j->getPosition().y());
                break;
            case VAR_SHAPE:
                tempMsg.writeUnsignedByte(TYPE_POLYGON);
                tempMsg.writeUnsignedByte(MIN2(255, (int)j->getShape().size()));
                for (int iPoint = 0; iPoint < MIN2(255, (int)j->getShape().size()); ++iPoint) {
                    tempMsg.writeDouble(j->getShape()[iPoint].x());
                    tempMsg.writeDouble(j->getShape()[iPoint].y());
                }
                break;

            default:
                break;
        }
    }
    server.writeStatusCmd(CMD_GET_JUNCTION_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}

bool
TraCIServerAPI_Junction::getPosition(const std::string& id, Position& p) {
    MSJunction* j = MSNet::getInstance()->getJunctionControl().get(id);
    if (j == 0) {
        return false;
    }
    p = j->getPosition();
    return true;
}


NamedRTree*
TraCIServerAPI_Junction::getTree() {
    NamedRTree* t = new NamedRTree();
    const std::map<std::string, MSJunction*>& junctions = MSNet::getInstance()->getJunctionControl().getMyMap();
    for (std::map<std::string, MSJunction*>::const_iterator i = junctions.begin(); i != junctions.end(); ++i) {
        Boundary b = (*i).second->getShape().getBoxBoundary();
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        t->Insert(cmin, cmax, (*i).second);
    }
    return t;
}

#endif


/****************************************************************************/

