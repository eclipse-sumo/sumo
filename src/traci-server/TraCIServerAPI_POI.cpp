/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2017-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    TraCIServerAPI_POI.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Robert Hilbrich
/// @date    07.05.2009
///
// APIs for getting/setting POI values via TraCI
/****************************************************************************/
#include <config.h>

#include <microsim/MSNet.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include <libsumo/POI.h>
#include <libsumo/StorageHelper.h>
#include <libsumo/TraCIConstants.h>
#include "TraCIServerAPI_POI.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_POI::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                               tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != libsumo::VAR_TYPE &&
            variable != libsumo::VAR_COLOR &&
            variable != libsumo::VAR_POSITION &&
            variable != libsumo::VAR_WIDTH &&
            variable != libsumo::VAR_HEIGHT &&
            variable != libsumo::VAR_ANGLE &&
            variable != libsumo::VAR_IMAGEFILE &&
            variable != libsumo::VAR_HIGHLIGHT &&
            variable != libsumo::ADD &&
            variable != libsumo::REMOVE &&
            variable != libsumo::VAR_PARAMETER) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_POI_VARIABLE, "Change PoI State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // process
    try {
        switch (variable) {
            case libsumo::VAR_TYPE: {
                libsumo::POI::setType(id, StoHelp::readTypedString(inputStorage, "The type must be given as a string."));
            }
            break;
            case libsumo::VAR_COLOR: {
                libsumo::POI::setColor(id, StoHelp::readTypedColor(inputStorage, "The color must be given using the according type."));
            }
            break;
            case libsumo::VAR_POSITION: {
                const libsumo::TraCIPosition pos = StoHelp::readTypedPosition2D(inputStorage, "The position must be given using an according type.");
                libsumo::POI::setPosition(id, pos.x, pos.y);
            }
            break;
            case libsumo::VAR_WIDTH: {
                libsumo::POI::setWidth(id, StoHelp::readTypedDouble(inputStorage, "The width must be given as a double."));
            }
            break;
            case libsumo::VAR_HEIGHT: {
                libsumo::POI::setHeight(id, StoHelp::readTypedDouble(inputStorage, "The height must be given as a double."));
            }
            break;
            case libsumo::VAR_ANGLE: {
                libsumo::POI::setAngle(id, StoHelp::readTypedDouble(inputStorage, "The angle must be given as a double."));
            }
            break;
            case libsumo::VAR_IMAGEFILE: {
                libsumo::POI::setImageFile(id, StoHelp::readTypedString(inputStorage, "The image file must be given as a string."));
            }
            break;
            case libsumo::VAR_HIGHLIGHT: {
                const int parameterCount = StoHelp::readCompound(inputStorage, -1, "A compound object is needed for highlighting an object.");
                if (parameterCount > 5) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_POI_VARIABLE, "Highlighting an object needs zero to five parameters.", outputStorage);
                }
                libsumo::TraCIColor col = libsumo::TraCIColor(255, 0, 0);
                if (parameterCount > 0) {
                    col = StoHelp::readTypedColor(inputStorage, "The first parameter for highlighting must be the highlight color.");
                }
                double size = -1;
                if (parameterCount > 1) {
                    size = StoHelp::readTypedDouble(inputStorage, "The second parameter for highlighting must be the highlight size.");
                }
                int alphaMax = -1;
                if (parameterCount > 2) {
                    alphaMax = StoHelp::readTypedUnsignedByte(inputStorage, "The third parameter for highlighting must be maximal alpha.");
                }
                double duration = -1;
                if (parameterCount > 3) {
                    duration = StoHelp::readTypedDouble(inputStorage, "The fourth parameter for highlighting must be the highlight duration.");
                }
                int type = 0;
                if (parameterCount > 4) {
                    type = StoHelp::readTypedUnsignedByte(inputStorage, "The fifth parameter for highlighting must be the highlight type id as ubyte.");
                }
                libsumo::POI::highlight(id, col, size, alphaMax, duration, type);
            }
            break;
            case libsumo::ADD: {
                const int parameterCount = StoHelp::readCompound(inputStorage, -1, "A compound object is needed for adding a new PoI.");
                const std::string type = StoHelp::readTypedString(inputStorage, "The first PoI parameter must be the type encoded as a string.");
                libsumo::TraCIColor col = StoHelp::readTypedColor(inputStorage, "The second PoI parameter must be the color.");
                const int layer = StoHelp::readTypedInt(inputStorage, "The third PoI parameter must be the layer encoded as int.");
                const libsumo::TraCIPosition pos = StoHelp::readTypedPosition2D(inputStorage, "The fourth PoI parameter must be the position.");
                if (parameterCount == 4) {
                    if (!libsumo::POI::add(id, pos.x, pos.y, col, type, layer)) {
                        return server.writeErrorStatusCmd(libsumo::CMD_SET_POI_VARIABLE, "Could not add PoI.", outputStorage);
                    }
                } else if (parameterCount >= 8) {
                    const std::string imgFile = StoHelp::readTypedString(inputStorage, "The fifth PoI parameter must be the imgFile encoded as a string.");
                    const double width = StoHelp::readTypedDouble(inputStorage, "The sixth PoI parameter must be the width encoded as a double.");
                    const double height = StoHelp::readTypedDouble(inputStorage, "The seventh PoI parameter must be the height encoded as a double.");
                    const double angle = StoHelp::readTypedDouble(inputStorage, "The eighth PoI parameter must be the angle encoded as a double.");
                    std::string icon;
                    if (parameterCount == 9) {
                        icon = StoHelp::readTypedString(inputStorage, "The ninth PoI parameter must be the icon encoded as a string.");
                    }
                    if (!libsumo::POI::add(id, pos.x, pos.y, col, type, layer, imgFile, width, height, angle, icon)) {
                        return server.writeErrorStatusCmd(libsumo::CMD_SET_POI_VARIABLE, "Could not add PoI.", outputStorage);
                    }
                } else {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_POI_VARIABLE,
                                                      "Adding a PoI requires either only type, color, layer and position parameters or these and icon, imageFile, width, height and angle parameters.",
                                                      outputStorage);
                }
            }
            break;
            case libsumo::REMOVE: {
                const int layer = StoHelp::readTypedInt(inputStorage, "The layer must be given using an int.");
                if (!libsumo::POI::remove(id, layer)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_POI_VARIABLE, "Could not remove PoI '" + id + "'", outputStorage);
                }
            }
            break;
            case libsumo::VAR_PARAMETER: {
                StoHelp::readCompound(inputStorage, 2, "A compound object of size 2 is needed for setting a parameter.");
                const std::string name = StoHelp::readTypedString(inputStorage, "The name of the parameter must be given as a string.");
                const std::string value = StoHelp::readTypedString(inputStorage, "The value of the parameter must be given as a string.");
                libsumo::POI::setParameter(id, name, value);
            }
            break;
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_POI_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_SET_POI_VARIABLE, libsumo::RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
