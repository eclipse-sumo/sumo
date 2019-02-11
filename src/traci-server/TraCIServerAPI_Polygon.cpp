/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_Polygon.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Christoph Sommer
/// @author  Gregor L\"ammel
/// @date    Sept 2002
/// @version $Id$
///
// APIs for getting/setting polygon values via TraCI
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/StdDefs.h>
#include <microsim/MSNet.h>
#include <utils/shapes/ShapeContainer.h>
#include <libsumo/Polygon.h>
#include <libsumo/Helper.h>
#include <libsumo/TraCIConstants.h>
#include "TraCIServerAPI_Polygon.h"

using namespace libsumo;

// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Polygon::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                   tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(RESPONSE_GET_POLYGON_VARIABLE, variable, id);
    try {
        if (!libsumo::Polygon::handleVariable(id, variable, &server)) {
            switch (variable) {
                case VAR_SHAPE:
                    server.writePositionVector(server.getWrapperStorage(), libsumo::Polygon::getShape(id));
                    break;
                case VAR_PARAMETER: {
                    std::string paramName = "";
                    if (!server.readTypeCheckingString(inputStorage, paramName)) {
                        return server.writeErrorStatusCmd(CMD_GET_POLYGON_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                    }
                    server.getWrapperStorage().writeUnsignedByte(TYPE_STRING);
                    server.getWrapperStorage().writeString(libsumo::Polygon::getParameter(id, paramName));
                    break;
                }
                default:
                    return server.writeErrorStatusCmd(CMD_GET_POLYGON_VARIABLE, "Get Polygon Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
            }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_POLYGON_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_POLYGON_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}

bool
TraCIServerAPI_Polygon::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                   tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != VAR_TYPE && variable != VAR_COLOR && variable != VAR_SHAPE && variable != VAR_FILL
            && variable != VAR_WIDTH
            && variable != ADD && variable != REMOVE && variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE,
                                          "Change Polygon State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    try {
        // process
        switch (variable) {
            case VAR_TYPE: {
                std::string type;
                if (!server.readTypeCheckingString(inputStorage, type)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The type must be given as a string.", outputStorage);
                }
                libsumo::Polygon::setType(id, type);
            }
            break;
            case VAR_COLOR: {
                libsumo::TraCIColor col;
                if (!server.readTypeCheckingColor(inputStorage, col)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The color must be given using an according type.", outputStorage);
                }
                libsumo::Polygon::setColor(id, col);
            }
            break;
            case VAR_SHAPE: {
                PositionVector shape;
                if (!server.readTypeCheckingPolygon(inputStorage, shape)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The shape must be given using an according type.", outputStorage);
                }
                libsumo::Polygon::setShape(id, libsumo::Helper::makeTraCIPositionVector(shape));
            }
            break;
            case VAR_FILL: {
                int value = 0;
                if (!server.readTypeCheckingInt(inputStorage, value)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "'fill' must be defined using an integer.", outputStorage);
                }
                libsumo::Polygon::setFilled(id, value != 0);
            }
            break;
            case VAR_WIDTH: {
                double value = 0;
                if (!server.readTypeCheckingDouble(inputStorage, value)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "'lineWidth' must be defined using an double.", outputStorage);
                }
                libsumo::Polygon::setLineWidth(id, value);
            }
            break;
            case ADD: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "A compound object is needed for setting a new polygon.", outputStorage);
                }
                int itemNo = inputStorage.readInt();
                if (itemNo != 5 && itemNo != 6) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Adding a polygon needs five to six parameters.", outputStorage);
                }
                std::string type;
                if (!server.readTypeCheckingString(inputStorage, type)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The type must be given as a string.", outputStorage);
                }
                libsumo::TraCIColor col;
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
                double lineWidth = 1;
                if (itemNo == 6) {
                    if (!server.readTypeCheckingDouble(inputStorage, lineWidth)) {
                        return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The sixth polygon parameter must be the lineWidth encoded as double.", outputStorage);
                    }
                }
                libsumo::TraCIPositionVector tp = libsumo::Helper::makeTraCIPositionVector(shape);

                libsumo::Polygon::add(id, tp, col, fill, lineWidth, type, layer);

            }
            break;
            case REMOVE: {
                int layer = 0; // !!! layer not used yet (shouldn't the id be enough?)
                if (!server.readTypeCheckingInt(inputStorage, layer)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The layer must be given using an int.", outputStorage);
                }

                libsumo::Polygon::remove(id, layer);

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
                libsumo::Polygon::setParameter(id, name, value);

            }
            break;
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
