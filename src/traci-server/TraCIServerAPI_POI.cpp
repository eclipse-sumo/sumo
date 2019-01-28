/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <microsim/MSNet.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include "TraCIConstants.h"
#include <libsumo/POI.h>
#include "TraCIServerAPI_POI.h"

// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_POI::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                               tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(RESPONSE_GET_POI_VARIABLE, variable, id);
    try {
        if (!libsumo::POI::handleVariable(id, variable, &server)) {
            switch (variable) {
                case VAR_PARAMETER: {
                    std::string paramName = "";
                    if (!server.readTypeCheckingString(inputStorage, paramName)) {
                        return server.writeErrorStatusCmd(CMD_GET_POI_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                    }
                    server.getWrapperStorage().writeUnsignedByte(TYPE_STRING);
                    server.getWrapperStorage().writeString(libsumo::POI::getParameter(id, paramName));
                    break;
                }
                default:
                    return server.writeErrorStatusCmd(CMD_GET_POI_VARIABLE, "Get PoI Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
            }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_POI_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_POI_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
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
			variable != VAR_WIDTH &&
			variable != VAR_HEIGHT &&
			variable != VAR_ANGLE &&
			variable != VAR_IMAGFILE &&
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
                libsumo::POI::setType(id, type);
            }
            break;
            case VAR_COLOR: {
                libsumo::TraCIColor col;
                if (!server.readTypeCheckingColor(inputStorage, col)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The color must be given using an according type.", outputStorage);
                }
                libsumo::POI::setColor(id, col);
            }
            break;
            case VAR_POSITION: {
                libsumo::TraCIPosition pos;
                if (!server.readTypeCheckingPosition2D(inputStorage, pos)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The position must be given using an according type.", outputStorage);
                }
                libsumo::POI::setPosition(id, pos.x, pos.y);
            }
            break;
			case VAR_WIDTH: {
				double width;
				if (!server.readTypeCheckingDouble(inputStorage, width)) {
					return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The width must be given using an according type.", outputStorage);
				}
				libsumo::POI::setWidth(id, width);
			}
			break;
			case VAR_HEIGHT: {
				double height;
				if (!server.readTypeCheckingDouble(inputStorage, height)) {
					return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The height must be given using an according type.", outputStorage);
				}
				libsumo::POI::setHeight(id, height);
			}
			break;
			case VAR_ANGLE: {
				double angle;
				if (!server.readTypeCheckingDouble(inputStorage, angle)) {
					return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The angle must be given using an according type.", outputStorage);
				}
				libsumo::POI::setAngle(id, angle);
			}
			break;
            case VAR_IMAGFILE: {
                std::string imageFile;
                if (!server.readTypeCheckingString(inputStorage, imageFile)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The type must be given as a string.", outputStorage);
                }
                libsumo::POI::setImageFile(id, imageFile);
            }
            break;
            case ADD: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "A compound object is needed for setting a new PoI.", outputStorage);
                }
                //read itemNo
                const int parameterCount = inputStorage.readInt();
                std::string type;
                if (!server.readTypeCheckingString(inputStorage, type)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The first PoI parameter must be the type encoded as a string.", outputStorage);
                }
                libsumo::TraCIColor col;
                if (!server.readTypeCheckingColor(inputStorage, col)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The second PoI parameter must be the color.", outputStorage);
                }
                int layer = 0;
                if (!server.readTypeCheckingInt(inputStorage, layer)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The third PoI parameter must be the layer encoded as int.", outputStorage);
                }
                libsumo::TraCIPosition pos;
                if (!server.readTypeCheckingPosition2D(inputStorage, pos)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The fourth PoI parameter must be the position.", outputStorage);
                }
                if (parameterCount == 4) {
                    if (!libsumo::POI::add(id, pos.x, pos.y, col, type, layer)) {
                        return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "Could not add PoI.", outputStorage);
                    }
                }
                else if (parameterCount == 8) {
                    std::string imgFile;
                    if (!server.readTypeCheckingString(inputStorage, imgFile)) {
                        return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The fifth PoI parameter must be the imgFile encoded as a string.", outputStorage);
                    }
                    double width;
                    if (!server.readTypeCheckingDouble(inputStorage, width)) {
                        return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The sixth PoI parameter must be the width encoded as a double.", outputStorage);
                    }
                    double height;
                    if (!server.readTypeCheckingDouble(inputStorage, height)) {
                        return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The seventh PoI parameter must be the height encoded as a double.", outputStorage);
                    }
                    double angle;
                    if (!server.readTypeCheckingDouble(inputStorage, angle)) {
                        return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The eighth PoI parameter must be the angle encoded as a double.", outputStorage);
                    }
                    //
                    if (!libsumo::POI::add(id, pos.x, pos.y, col, type, layer, imgFile, width, height, angle)) {
                        return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "Could not add PoI.", outputStorage);
                    }
                }
                else {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE,
                        "Adding a PoI requires either only type, color, layer and position parameters or these and imageFile, width, height and angle parameters.",
                        outputStorage);
                }
            }
            break;
            case REMOVE: {
                int layer = 0; // !!! layer not used yet (shouldn't the id be enough?)
                if (!server.readTypeCheckingInt(inputStorage, layer)) {
                    return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, "The layer must be given using an int.", outputStorage);
                }
                if (!libsumo::POI::remove(id, layer)) {
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
                libsumo::POI::setParameter(id, name, value);
            }
            break;
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_SET_POI_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_SET_POI_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
