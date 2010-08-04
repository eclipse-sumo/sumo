/****************************************************************************/
/// @file    TraCIServerAPI_Polygon.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// APIs for getting/setting polygon values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/StdDefs.h>
#include <microsim/MSNet.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_Polygon.h"

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
TraCIServerAPI_Polygon::processGet(TraCIServer &server, tcpip::Storage &inputStorage,
                                   tcpip::Storage &outputStorage) throw(TraCIException, std::invalid_argument) {
    std::string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=VAR_TYPE&&variable!=VAR_COLOR&&variable!=VAR_SHAPE&&variable!=VAR_FILL) {
        server.writeStatusCmd(CMD_GET_POLYGON_VARIABLE, RTYPE_ERR, "Get Polygon Variable: unsupported variable specified", outputStorage);
        return false;
    }
    // begin response building
    Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_POLYGON_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable==ID_LIST) {
        std::vector<std::string> ids;
        ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
        for (int i = shapeCont.getMinLayer(); i <= shapeCont.getMaxLayer(); ++i) {
            shapeCont.getPolygonCont(i).insertIDs(ids);
        }
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
        Polygon2D *p = 0;
        ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
        for (int i = shapeCont.getMinLayer(); i <= shapeCont.getMaxLayer()&&p==0; ++i) {
            p = shapeCont.getPolygonCont(i).get(id);
        }
        if (p==0) {
            server.writeStatusCmd(CMD_GET_POLYGON_VARIABLE, RTYPE_ERR, "Polygon '" + id + "' is not known", outputStorage);
            return false;
        }
        switch (variable) {
        case VAR_TYPE:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(p->getType());
            break;
        case VAR_COLOR:
            tempMsg.writeUnsignedByte(TYPE_COLOR);
            tempMsg.writeUnsignedByte(static_cast<int>(p->getColor().red()*255.+.5));
            tempMsg.writeUnsignedByte(static_cast<int>(p->getColor().green()*255.+.5));
            tempMsg.writeUnsignedByte(static_cast<int>(p->getColor().blue()*255.+.5));
            tempMsg.writeUnsignedByte(255);
            break;
        case VAR_SHAPE:
            tempMsg.writeUnsignedByte(TYPE_POLYGON);
            tempMsg.writeUnsignedByte(MIN2(static_cast<size_t>(255),p->getShape().size()));
            for (int iPoint=0; iPoint < MIN2(static_cast<size_t>(255),p->getShape().size()); ++iPoint) {
                tempMsg.writeFloat(p->getShape()[iPoint].x());
                tempMsg.writeFloat(p->getShape()[iPoint].y());
            }
            break;
        case VAR_FILL:
            tempMsg.writeUnsignedByte(TYPE_UBYTE);
            tempMsg.writeUnsignedByte(p->fill() ? 1 : 0);
            break;
        default:
            break;
        }
    }
        server.writeStatusCmd(CMD_GET_POLYGON_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


bool
TraCIServerAPI_Polygon::processSet(TraCIServer &server, tcpip::Storage &inputStorage,
                                   tcpip::Storage &outputStorage) throw(TraCIException, std::invalid_argument) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable!=VAR_TYPE&&variable!=VAR_COLOR&&variable!=VAR_SHAPE&&variable!=VAR_FILL
            &&variable!=ADD&&variable!=REMOVE) {
        server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_ERR, "Change Polygon State: unsupported variable specified", outputStorage);
        return false;
    }
    // id
    std::string id = inputStorage.readString();
    Polygon2D *p = 0;
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    if (variable!=ADD&&variable!=REMOVE) {
        for (int i = shapeCont.getMinLayer(); i <= shapeCont.getMaxLayer()&&p==0; ++i) {
            p = shapeCont.getPolygonCont(i).get(id);
        }
        if (p==0) {
            server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_ERR, "Polygon '" + id + "' is not known", outputStorage);
            return false;
        }
    }
    // process
    int valueDataType = inputStorage.readUnsignedByte();
    switch (variable) {
    case VAR_TYPE: {
        if (valueDataType!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_ERR, "The type must be given as a string.", outputStorage);
            return false;
        }
        std::string type = inputStorage.readString();
        p->setType(type);
    }
    break;
    case VAR_COLOR: {
        if (valueDataType!=TYPE_COLOR) {
            server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_ERR, "The color must be given using an accoring type.", outputStorage);
            return false;
        }
        SUMOReal r = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
        SUMOReal g = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
        SUMOReal b = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
        SUMOReal a = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
        p->setColor(RGBColor(r,g,b));
    }
    break;
    case VAR_SHAPE: {
        if (valueDataType!=TYPE_POLYGON) {
            server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_ERR, "The shape must be given using an accoring type.", outputStorage);
            return false;
        }
        unsigned int noEntries = inputStorage.readUnsignedByte();
        Position2DVector shape;
        for (int i=0; i<noEntries; ++i) {
            SUMOReal x = inputStorage.readFloat();
            SUMOReal y = inputStorage.readFloat();
            shape.push_back(Position2D(x, y));
        }
        p->setShape(shape);
    }
    break;
    case VAR_FILL: {
        if (valueDataType!=TYPE_UBYTE) {
            server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_ERR, "'fill' must be defined using an unsigned byte.", outputStorage);
            return false;
        }
        bool fill = inputStorage.readUnsignedByte()!=0;
        p->setFill(fill);
    }
    break;
    case ADD: {
        if (valueDataType!=TYPE_COMPOUND) {
            server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_ERR, "A compound object is needed for setting a new polygon.", outputStorage);
            return false;
        }
        unsigned int itemNo = inputStorage.readInt();
        // type
        if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_ERR, "The first polygon parameter must be the type encoded as a string.", outputStorage);
            return false;
        }
        std::string type = inputStorage.readString();
        // color
        if (inputStorage.readUnsignedByte()!=TYPE_COLOR) {
            server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_ERR, "The second polygon parameter must be the color.", outputStorage);
            return false;
        }
        SUMOReal r = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
        SUMOReal g = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
        SUMOReal b = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
        SUMOReal a = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
        // fill
        if (inputStorage.readUnsignedByte()!=TYPE_UBYTE) {
            server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_ERR, "The third polygon parameter must be 'fill' encoded as ubyte.", outputStorage);
            return false;
        }
        bool fill = inputStorage.readUnsignedByte()!=0;
        // layer
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_ERR, "The fourth polygon parameter must be the layer encoded as int.", outputStorage);
            return false;
        }
        int layer = inputStorage.readInt();
        // shape
        if (inputStorage.readUnsignedByte()!=TYPE_POLYGON) {
            server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_ERR, "The fifth polygon parameter must be the shape.", outputStorage);
            return false;
        }
        unsigned int noEntries = inputStorage.readUnsignedByte();
        Position2DVector shape;
        for (int i=0; i<noEntries; ++i) {
            SUMOReal x = inputStorage.readFloat();
            SUMOReal y = inputStorage.readFloat();
            shape.push_back(Position2D(x, y));
        }
        //
        p = new Polygon2D(id, type, RGBColor(r, g, b), shape, fill);
        if (!shapeCont.add(layer, p)) {
            delete p;
            server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_ERR, "Could not add polygon.", outputStorage);
            return false;
        }
    }
    break;
    case REMOVE: {
        if (valueDataType!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "The layer must be given using an int.", outputStorage);
            return false;
        }
        int layer = inputStorage.readInt();
        if (!shapeCont.removePolygon(layer, id)) {
            bool removed = false;
            for (int i = shapeCont.getMinLayer(); i <= shapeCont.getMaxLayer(); ++i) {
                removed |= shapeCont.removePolygon(i, id);
            }
            if (!removed) {
                server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_ERR, "Could not remove PoI '" + id + "'", outputStorage);
                return false;
            }
        }
    }
    break;
    default:
        break;
    }
    server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}



/****************************************************************************/

