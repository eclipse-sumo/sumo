/****************************************************************************/
/// @file    TraCIServerAPI_Polygon.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Christoph Sommer
/// @date    Sept 2002
/// @version $Id$
///
// APIs for getting/setting polygon values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/StdDefs.h>
#include <microsim/MSNet.h>
#include <utils/shapes/Polygon.h>
#include <utils/shapes/ShapeContainer.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_Polygon.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Polygon::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                   tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != VAR_TYPE && variable != VAR_COLOR && variable != VAR_SHAPE && variable != VAR_FILL
            && variable != ID_COUNT && variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_GET_POLYGON_VARIABLE, "Get Polygon Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_POLYGON_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable == ID_LIST || variable == ID_COUNT) {
        std::vector<std::string> ids;
        ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
        shapeCont.getPolygons().insertIDs(ids);
        if (variable == ID_LIST) {
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(ids);
        } else {
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt((int) ids.size());
        }
    } else {
        SUMO::Polygon* p = getPolygon(id);
        if (p == 0) {
            return server.writeErrorStatusCmd(CMD_GET_POLYGON_VARIABLE, "Polygon '" + id + "' is not known", outputStorage);
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
            case VAR_SHAPE:
                tempMsg.writeUnsignedByte(TYPE_POLYGON);
                tempMsg.writeUnsignedByte(MIN2(255, (int)p->getShape().size()));
                for (int iPoint = 0; iPoint < MIN2(255, (int)p->getShape().size()); ++iPoint) {
                    tempMsg.writeDouble(p->getShape()[iPoint].x());
                    tempMsg.writeDouble(p->getShape()[iPoint].y());
                }
                break;
            case VAR_FILL:
                tempMsg.writeUnsignedByte(TYPE_UBYTE);
                tempMsg.writeUnsignedByte(p->getFill() ? 1 : 0);
                break;
            case VAR_PARAMETER: {
                std::string paramName = "";
                if (!server.readTypeCheckingString(inputStorage, paramName)) {
                    return server.writeErrorStatusCmd(CMD_GET_POLYGON_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                }
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(p->getParameter(paramName, ""));
            }
            break;
            default:
                break;
        }
    }
    server.writeStatusCmd(CMD_GET_POLYGON_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


bool
TraCIServerAPI_Polygon::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                   tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != VAR_TYPE && variable != VAR_COLOR && variable != VAR_SHAPE && variable != VAR_FILL
            && variable != ADD && variable != REMOVE && variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "Change Polygon State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    SUMO::Polygon* p = 0;
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    if (variable != ADD && variable != REMOVE) {
        p = getPolygon(id);
        if (p == 0) {
            return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "Polygon '" + id + "' is not known", outputStorage);
        }
    }
    // process
    switch (variable) {
        case VAR_TYPE: {
            std::string type;
            if (!server.readTypeCheckingString(inputStorage, type)) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The type must be given as a string.", outputStorage);
            }
            p->setType(type);
        }
        break;
        case VAR_COLOR: {
            RGBColor col;
            if (!server.readTypeCheckingColor(inputStorage, col)) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The color must be given using an according type.", outputStorage);
            }
            p->setColor(col);
        }
        break;
        case VAR_SHAPE: {
            PositionVector shape;
            if (!server.readTypeCheckingPolygon(inputStorage, shape)) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The shape must be given using an accoring type.", outputStorage);
            }
            shapeCont.reshapePolygon(id, shape);
        }
        break;
        case VAR_FILL: {
            int value = 0;
            if (!server.readTypeCheckingUnsignedByte(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "'fill' must be defined using an unsigned byte.", outputStorage);
            }
            p->setFill(value != 0);
        }
        break;
        case ADD: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "A compound object is needed for setting a new polygon.", outputStorage);
            }
            //readt itemNo
            inputStorage.readInt();
            std::string type;
            if (!server.readTypeCheckingString(inputStorage, type)) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The type must be given as a string.", outputStorage);
            }
            RGBColor col;
            if (!server.readTypeCheckingColor(inputStorage, col)) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The second polygon parameter must be the color.", outputStorage);
            }
            int value = 0;
            if (!server.readTypeCheckingUnsignedByte(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The third polygon parameter must be 'fill' encoded as ubyte.", outputStorage);
            }
            bool fill = value != 0;
            int layer = 0;
            if (!server.readTypeCheckingInt(inputStorage, layer)) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The fourth polygon parameter must be the layer encoded as int.", outputStorage);
            }
            PositionVector shape;
            if (!server.readTypeCheckingPolygon(inputStorage, shape)) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The fifth polygon parameter must be the shape.", outputStorage);
            }
            //
            if (!shapeCont.addPolygon(id, type, col, (SUMOReal)layer,
                                      Shape::DEFAULT_ANGLE, Shape::DEFAULT_IMG_FILE, shape, fill)) {
                delete p;
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "Could not add polygon.", outputStorage);
            }
        }
        break;
        case REMOVE: {
            int layer = 0; // !!! layer not used yet (shouldn't the id be enough?)
            if (!server.readTypeCheckingInt(inputStorage, layer)) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The layer must be given using an int.", outputStorage);
            }
            if (!shapeCont.removePolygon(id)) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "Could not remove polygon '" + id + "'", outputStorage);
            }
        }
        break;
        case VAR_PARAMETER: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "A compound object is needed for setting a parameter.", outputStorage);
            }
            //readt itemNo
            inputStorage.readInt();
            std::string name;
            if (!server.readTypeCheckingString(inputStorage, name)) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The name of the parameter must be given as a string.", outputStorage);
            }
            std::string value;
            if (!server.readTypeCheckingString(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The value of the parameter must be given as a string.", outputStorage);
            }
            p->addParameter(name, value);
        }
        break;
        default:
            break;
    }
    server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


bool
TraCIServerAPI_Polygon::getShape(const std::string& id, PositionVector& shape) {
    SUMO::Polygon* poly = getPolygon(id);
    if (poly == 0) {
        return false;
    }
    shape = poly->getShape();
    return true;
}


SUMO::Polygon*
TraCIServerAPI_Polygon::getPolygon(const std::string& id) {
    return MSNet::getInstance()->getShapeContainer().getPolygons().get(id);
}


NamedRTree*
TraCIServerAPI_Polygon::getTree() {
    NamedRTree* t = new NamedRTree();
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    const std::map<std::string, SUMO::Polygon*>& polygons = shapeCont.getPolygons().getMyMap();
    for (std::map<std::string, SUMO::Polygon*>::const_iterator i = polygons.begin(); i != polygons.end(); ++i) {
        Boundary b = (*i).second->getShape().getBoxBoundary();
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        t->Insert(cmin, cmax, (*i).second);
    }
    return t;
}


#endif


/****************************************************************************/

