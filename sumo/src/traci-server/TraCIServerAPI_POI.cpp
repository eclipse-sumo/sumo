/****************************************************************************/
/// @file    TraCIServerAPI_POI.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting POI values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 20019-2014 DLR (http://www.dlr.de/) and contributors
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
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_POI.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_POI::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                               tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != VAR_TYPE && variable != VAR_COLOR && variable != VAR_POSITION && variable != ID_COUNT) {
        return server.writeErrorStatusCmd(CMD_GET_POI_VARIABLE, "Get PoI Variable: unsupported variable specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_POI_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable == ID_LIST || variable == ID_COUNT) {
        std::vector<std::string> ids;
        ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
        shapeCont.getPOIs().insertIDs(ids);
        if (variable == ID_LIST) {
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(ids);
        } else {
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt((int) ids.size());
        }
    } else {
        PointOfInterest* p = getPoI(id);
        if (p == 0) {
            return server.writeErrorStatusCmd(CMD_GET_POI_VARIABLE, "POI '" + id + "' is not known", outputStorage);
        }
        switch (variable) {
            case VAR_TYPE:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(p->getType());
                break;
            case VAR_COLOR:
                tempMsg.writeUnsignedByte(TYPE_COLOR);
                tempMsg.writeUnsignedByte(p->getColor().red());
                tempMsg.writeUnsignedByte(p->getColor().green());
                tempMsg.writeUnsignedByte(p->getColor().blue());
                tempMsg.writeUnsignedByte(p->getColor().alpha());
                break;
            case VAR_POSITION:
                tempMsg.writeUnsignedByte(POSITION_2D);
                tempMsg.writeDouble(p->x());
                tempMsg.writeDouble(p->y());
                break;
            default:
                break;
        }
    }
    server.writeStatusCmd(CMD_GET_POI_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


bool
TraCIServerAPI_POI::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                               tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != VAR_TYPE && variable != VAR_COLOR && variable != VAR_POSITION
            && variable != ADD && variable != REMOVE) {
        return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "Change PoI State: unsupported variable specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    PointOfInterest* p = 0;
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    if (variable != ADD && variable != REMOVE) {
        p = getPoI(id);
        if (p == 0) {
            return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "POI '" + id + "' is not known", outputStorage);
        }
    }
    // process
    switch (variable) {
        case VAR_TYPE: {
            std::string type;
            if (!server.readTypeCheckingString(inputStorage, type)) {
                return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The type must be given as a string.", outputStorage);
            }
            p->setType(type);
        }
        break;
        case VAR_COLOR: {
            RGBColor col;
            if (!server.readTypeCheckingColor(inputStorage, col)) {
                return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The color must be given using an according type.", outputStorage);
            }
            p->setColor(col);
        }
        break;
        case VAR_POSITION: {
            Position pos;
            if (!server.readTypeCheckingPosition2D(inputStorage, pos)) {
                return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The position must be given using an accoring type.", outputStorage);
            }
            shapeCont.movePOI(id, pos);
        }
        break;
        case ADD: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "A compound object is needed for setting a new PoI.", outputStorage);
            }
            //read itemNo
            inputStorage.readInt();
            std::string type;
            if (!server.readTypeCheckingString(inputStorage, type)) {
                return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The first PoI parameter must be the type encoded as a string.", outputStorage);
            }
            RGBColor col;
            if (!server.readTypeCheckingColor(inputStorage, col)) {
                return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The second PoI parameter must be the color.", outputStorage);
            }
            int layer = 0;
            if (!server.readTypeCheckingInt(inputStorage, layer)) {
                return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The third PoI parameter must be the layer encoded as int.", outputStorage);
            }
            Position pos;
            if (!server.readTypeCheckingPosition2D(inputStorage, pos)) {
                return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The fourth PoI parameter must be the position.", outputStorage);
            }
            //
            if (!shapeCont.addPOI(id, type, col, (SUMOReal)layer,
                                  Shape::DEFAULT_ANGLE, Shape::DEFAULT_IMG_FILE, pos,
                                  Shape::DEFAULT_IMG_WIDTH, Shape::DEFAULT_IMG_HEIGHT)) {
                delete p;
                return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "Could not add PoI.", outputStorage);
            }
        }
        break;
        case REMOVE: {
            int layer = 0; // !!! layer not used yet (shouldn't the id be enough?)
            if (!server.readTypeCheckingInt(inputStorage, layer)) {
                return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The layer must be given using an int.", outputStorage);
            }
            if (!shapeCont.removePOI(id)) {
                return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "Could not remove PoI '" + id + "'", outputStorage);
            }
        }
        break;
        default:
            break;
    }
    server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


bool
TraCIServerAPI_POI::getPosition(const std::string& id, Position& p) {
    PointOfInterest* poi = getPoI(id);
    if (poi == 0) {
        return false;
    }
    p = *poi;
    return true;
}


PointOfInterest*
TraCIServerAPI_POI::getPoI(const std::string& id) {
    return MSNet::getInstance()->getShapeContainer().getPOIs().get(id);
}


NamedRTree*
TraCIServerAPI_POI::getTree() {
    NamedRTree* t = new NamedRTree();
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    const std::map<std::string, PointOfInterest*>& pois = shapeCont.getPOIs().getMyMap();
    for (std::map<std::string, PointOfInterest*>::const_iterator i = pois.begin(); i != pois.end(); ++i) {
        const float cmin[2] = {(float)(*i).second->x(), (float)(*i).second->y()};
        const float cmax[2] = {(float)(*i).second->x(), (float)(*i).second->y()};
        t->Insert(cmin, cmax, (*i).second);
    }
    return t;
}


#endif


/****************************************************************************/

