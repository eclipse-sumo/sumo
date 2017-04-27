/****************************************************************************/
/// @file    TraCIServerAPI_POI.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Robert Hilbrich
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting POI values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2017-2017 DLR (http://www.dlr.de/) and contributors
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
#include "lib/TraCI_POI.h"
#include "TraCIServerAPI_POI.h"

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
    if (variable != ID_LIST &&
            variable != VAR_TYPE &&
            variable != VAR_COLOR &&
            variable != VAR_POSITION &&
            variable != VAR_POSITION3D &&
            variable != ID_COUNT &&
            variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_GET_POI_VARIABLE, "Get PoI Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_POI_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    try {
        switch (variable) {
            case ID_LIST:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(TraCI_POI::getIDList());
                break;
            case ID_COUNT:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt((int) TraCI_POI::getIDCount());
                break;
            case VAR_TYPE:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(TraCI_POI::getType(id));
                break;
            case VAR_COLOR:
                tempMsg.writeUnsignedByte(TYPE_COLOR);
                tempMsg.writeUnsignedByte(TraCI_POI::getColor(id).r);
                tempMsg.writeUnsignedByte(TraCI_POI::getColor(id).g);
                tempMsg.writeUnsignedByte(TraCI_POI::getColor(id).b);
                tempMsg.writeUnsignedByte(TraCI_POI::getColor(id).a);
                break;
            case VAR_POSITION:
                tempMsg.writeUnsignedByte(POSITION_2D);
                tempMsg.writeDouble(TraCI_POI::getPosition(id).x);
                tempMsg.writeDouble(TraCI_POI::getPosition(id).y);
                break;
            case VAR_POSITION3D:
                tempMsg.writeUnsignedByte(POSITION_3D);
                tempMsg.writeDouble(TraCI_POI::getPosition(id).x);
                tempMsg.writeDouble(TraCI_POI::getPosition(id).y);
                tempMsg.writeDouble(TraCI_POI::getPosition(id).z);
                break;
            case VAR_PARAMETER: {
                std::string paramName = "";
                if (!server.readTypeCheckingString(inputStorage, paramName)) {
                    return server.writeErrorStatusCmd(CMD_GET_POI_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                }
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(TraCI_POI::getParameter(id, paramName));
                break;
            }
            default:
                break;
        }
    } catch (TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_POI_VARIABLE, e.what(), outputStorage);
    }

    server.writeStatusCmd(CMD_GET_POI_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);

    return true;
}


bool
TraCIServerAPI_POI::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                               tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != VAR_TYPE &&
            variable != VAR_COLOR &&
            variable != VAR_POSITION &&
            variable != ADD &&
            variable != REMOVE &&
            variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "Change PoI State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // process
    try {
        switch (variable) {
            case VAR_TYPE: {
                std::string type;
                if (!server.readTypeCheckingString(inputStorage, type)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The type must be given as a string.", outputStorage);
                }
                TraCI_POI::setType(id, type);
            }
            break;
            case VAR_COLOR: {
                TraCIColor col;
                if (!server.readTypeCheckingColor(inputStorage, col)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The color must be given using an according type.", outputStorage);
                }
                TraCI_POI::setColor(id, col);
            }
            break;
            case VAR_POSITION: {
                TraCIPosition pos;
                if (!server.readTypeCheckingPosition2D(inputStorage, pos)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The position must be given using an accoring type.", outputStorage);
                }
                TraCI_POI::setPosition(id, pos);
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
                TraCIColor col;
                if (!server.readTypeCheckingColor(inputStorage, col)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The second PoI parameter must be the color.", outputStorage);
                }
                int layer = 0;
                if (!server.readTypeCheckingInt(inputStorage, layer)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The third PoI parameter must be the layer encoded as int.", outputStorage);
                }
                TraCIPosition pos;
                if (!server.readTypeCheckingPosition2D(inputStorage, pos)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The fourth PoI parameter must be the position.", outputStorage);
                }
                //
                if (!TraCI_POI::add(id, pos, col, type, layer)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "Could not add PoI.", outputStorage);
                }
            }
            break;
            case REMOVE: {
                int layer = 0; // !!! layer not used yet (shouldn't the id be enough?)
                if (!server.readTypeCheckingInt(inputStorage, layer)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The layer must be given using an int.", outputStorage);
                }
                if (!TraCI_POI::remove(id, layer)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "Could not remove PoI '" + id + "'", outputStorage);
                }
            }
            break;
            case VAR_PARAMETER: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "A compound object is needed for setting a parameter.", outputStorage);
                }
                //readt itemNo
                inputStorage.readInt();
                std::string name;
                if (!server.readTypeCheckingString(inputStorage, name)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The name of the parameter must be given as a string.", outputStorage);
                }
                std::string value;
                if (!server.readTypeCheckingString(inputStorage, value)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The value of the parameter must be given as a string.", outputStorage);
                }
                TraCI_POI::addParameter(id, name, value);
            }
            break;
            default:
                break;
        }
    } catch (TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, e.what(), outputStorage);
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

