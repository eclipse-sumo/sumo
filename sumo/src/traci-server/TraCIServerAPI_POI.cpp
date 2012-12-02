/****************************************************************************/
/// @file    TraCIServerAPI_POI.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting POI values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
// used namespaces
// ===========================================================================
using namespace traci;


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
        server.writeStatusCmd(CMD_GET_POI_VARIABLE, RTYPE_ERR, "Get PoI Variable: unsupported variable specified", outputStorage);
        return false;
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
        int layer;
        PointOfInterest* p = getPoI(id, layer);
        if (p == 0) {
            server.writeStatusCmd(CMD_GET_POI_VARIABLE, RTYPE_ERR, "POI '" + id + "' is not known", outputStorage);
            return false;
        }
        switch (variable) {
            case VAR_TYPE:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(p->getType());
                break;
            case VAR_COLOR:
                tempMsg.writeUnsignedByte(TYPE_COLOR);
                tempMsg.writeUnsignedByte(static_cast<int>(p->getColor().red() * 255. + .5));
                tempMsg.writeUnsignedByte(static_cast<int>(p->getColor().green() * 255. + .5));
                tempMsg.writeUnsignedByte(static_cast<int>(p->getColor().blue() * 255. + .5));
                tempMsg.writeUnsignedByte(255);
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
        server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "Change PoI State: unsupported variable specified", outputStorage);
        return false;
    }
    // id
    std::string id = inputStorage.readString();
    PointOfInterest* p = 0;
    int layer = 0;
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    if (variable != ADD && variable != REMOVE) {
        p = getPoI(id, layer);
        if (p == 0) {
            server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "POI '" + id + "' is not known", outputStorage);
            return false;
        }
    }
    // process
    int valueDataType = inputStorage.readUnsignedByte();
    switch (variable) {
        case VAR_TYPE: {
            if (valueDataType != TYPE_STRING) {
                server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "The type must be given as a string.", outputStorage);
                return false;
            }
            std::string type = inputStorage.readString();
            p->setType(type);
        }
        break;
        case VAR_COLOR: {
            if (valueDataType != TYPE_COLOR) {
                server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "The color must be given using an accoring type.", outputStorage);
                return false;
            }
            SUMOReal r = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
            SUMOReal g = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
            SUMOReal b = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
            //read SUMOReal a
            inputStorage.readUnsignedByte();
            p->setColor(RGBColor(r, g, b));
        }
        break;
        case VAR_POSITION: {
            if (valueDataType != POSITION_2D) {
                server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "The position must be given using an accoring type.", outputStorage);
                return false;
            }
            SUMOReal x = inputStorage.readDouble();
            SUMOReal y = inputStorage.readDouble();
            shapeCont.movePOI(id, Position(x, y));
        }
        break;
        case ADD: {
            if (valueDataType != TYPE_COMPOUND) {
                server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "A compound object is needed for setting a new PoI.", outputStorage);
                return false;
            }
            //read itemNo
            inputStorage.readInt();
            // type
            if (inputStorage.readUnsignedByte() != TYPE_STRING) {
                server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "The first PoI parameter must be the type encoded as a string.", outputStorage);
                return false;
            }
            std::string type = inputStorage.readString();
            // color
            if (inputStorage.readUnsignedByte() != TYPE_COLOR) {
                server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "The second PoI parameter must be the color.", outputStorage);
                return false;
            }
            SUMOReal r = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
            SUMOReal g = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
            SUMOReal b = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
            //read SUMOReal a
            inputStorage.readUnsignedByte();
            // layer
            if (inputStorage.readUnsignedByte() != TYPE_INTEGER) {
                server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "The third PoI parameter must be the layer encoded as int.", outputStorage);
                return false;
            }
            layer = inputStorage.readInt();
            // pos
            if (inputStorage.readUnsignedByte() != POSITION_2D) {
                server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "The fourth PoI parameter must be the position.", outputStorage);
                return false;
            }
            SUMOReal x = inputStorage.readDouble();
            SUMOReal y = inputStorage.readDouble();
            //
            if (!shapeCont.addPOI(id, type, RGBColor(r, g, b), (SUMOReal)layer,
                                  Shape::DEFAULT_ANGLE, Shape::DEFAULT_IMG_FILE,
                                  Position(x, y),
                                  Shape::DEFAULT_IMG_WIDTH, Shape::DEFAULT_IMG_HEIGHT)) {
                delete p;
                server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "Could not add PoI.", outputStorage);
                return false;
            }
        }
        break;
        case REMOVE: {
            if (valueDataType != TYPE_INTEGER) {
                server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "The layer must be given using an int.", outputStorage);
                return false;
            }
            layer = inputStorage.readInt();
            if (!shapeCont.removePOI(id)) {
                server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "Could not remove PoI '" + id + "'", outputStorage);
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
    int layer;
    PointOfInterest* poi = getPoI(id, layer);
    if (poi == 0) {
        return false;
    }
    p = *poi;
    return true;
}


PointOfInterest*
TraCIServerAPI_POI::getPoI(const std::string& id, int& layer) {
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    return shapeCont.getPOIs().get(id);
}


TraCIRTree*
TraCIServerAPI_POI::getTree() {
    TraCIRTree* t = new TraCIRTree();
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    const std::map<std::string, PointOfInterest*>& pois = shapeCont.getPOIs().getMyMap();
    for (std::map<std::string, PointOfInterest*>::const_iterator i = pois.begin(); i != pois.end(); ++i) {
        t->addObject((*i).second, *(*i).second);
    }
    return t;
}


#endif


/****************************************************************************/

