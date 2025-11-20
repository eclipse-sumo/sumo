/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2002-2025 German Aerospace Center (DLR) and others.
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
/// @file    TraCIServerAPI_Polygon.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Christoph Sommer
/// @author  Gregor Laemmel
/// @date    Sept 2002
///
// APIs for getting/setting polygon values via TraCI
/****************************************************************************/
#include <config.h>

#include <utils/common/StdDefs.h>
#include <microsim/MSNet.h>
#include <utils/shapes/ShapeContainer.h>
#include <libsumo/Polygon.h>
#include <libsumo/Helper.h>
#include <libsumo/StorageHelper.h>
#include <libsumo/TraCIConstants.h>
#include "TraCIServerAPI_Polygon.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Polygon::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                   tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(libsumo::RESPONSE_GET_POLYGON_VARIABLE, variable, id);
    try {
        if (!libsumo::Polygon::handleVariable(id, variable, &server, &inputStorage)) {
            return server.writeErrorStatusCmd(libsumo::CMD_GET_POLYGON_VARIABLE, "Get Polygon Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_GET_POLYGON_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_GET_POLYGON_VARIABLE, libsumo::RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}

bool
TraCIServerAPI_Polygon::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                   tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != libsumo::VAR_TYPE && variable != libsumo::VAR_COLOR && variable != libsumo::VAR_SHAPE && variable != libsumo::VAR_FILL
            && variable != libsumo::VAR_WIDTH && variable != libsumo::VAR_MOVE_TO
            && variable != libsumo::ADD && variable != libsumo::REMOVE && variable != libsumo::VAR_PARAMETER) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_POLYGON_VARIABLE,
                                          "Change Polygon State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    try {
        // process
        switch (variable) {
            case libsumo::VAR_TYPE:
                libsumo::Polygon::setType(id, StoHelp::readTypedString(inputStorage, "The type must be given as a string."));
                break;
            case libsumo::VAR_COLOR:
                libsumo::Polygon::setColor(id, StoHelp::readTypedColor(inputStorage, "The color must be given using an according type."));
                break;
            case libsumo::VAR_SHAPE:
                libsumo::Polygon::setShape(id, StoHelp::readTypedPolygon(inputStorage, "The shape must be given using an according type."));
                break;
            case libsumo::VAR_FILL:
                libsumo::Polygon::setFilled(id, StoHelp::readTypedInt(inputStorage, "'fill' must be defined using an integer.") != 0);
                break;
            case libsumo::VAR_WIDTH:
                libsumo::Polygon::setLineWidth(id, StoHelp::readTypedDouble(inputStorage, "'lineWidth' must be defined using a double."));
                break;
            case libsumo::ADD: {
                const int parameterCount = StoHelp::readCompound(inputStorage, -1, "A compound object is needed for adding a new polygon.");
                if (parameterCount != 5 && parameterCount != 6) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Adding a polygon needs five to six parameters.", outputStorage);
                }
                const std::string type = StoHelp::readTypedString(inputStorage, "The type must be given as a string.");
                const libsumo::TraCIColor col = StoHelp::readTypedColor(inputStorage, "The second polygon parameter must be the color.");
                const bool fill = StoHelp::readBool(inputStorage, "The third polygon parameter must be 'fill' encoded as ubyte.");
                const int layer = StoHelp::readTypedInt(inputStorage, "The fourth polygon parameter must be the layer encoded as int.");
                const libsumo::TraCIPositionVector tp = StoHelp::readTypedPolygon(inputStorage, "The fifth polygon parameter must be the shape.");
                double lineWidth = 1.;
                if (parameterCount == 6) {
                    lineWidth = StoHelp::readTypedDouble(inputStorage, "The sixth polygon parameter must be the lineWidth encoded as double.");
                }
                libsumo::Polygon::add(id, tp, col, fill, type, layer, lineWidth);
            }
            break;
            case libsumo::VAR_ADD_DYNAMICS: {
                if (inputStorage.readUnsignedByte() != libsumo::TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_POLYGON_VARIABLE, "A compound object is needed for adding dynamics to a polygon.", outputStorage);
                }
                int itemNo = inputStorage.readInt();
                if (itemNo != 5) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Adding polygon dynamics needs four parameters.", outputStorage);
                }
                const std::string trackedID = StoHelp::readTypedString(inputStorage, "The first parameter for adding polygon dynamics must be ID of the tracked object as a string ('' to disregard tracking).");
                const std::vector<double> timeSpan = StoHelp::readTypedDoubleList(inputStorage, "The second parameter for adding polygon dynamics must be the timespan of the animation (length=0 to disregard animation).");
                const std::vector<double> alphaSpan = StoHelp::readTypedDoubleList(inputStorage, "The third parameter for adding polygon dynamics must be the alphaSpanStr of the animation (length=0 to disregard alpha animation).");
                const bool looped = StoHelp::readBool(inputStorage, "The fourth parameter for adding polygon dynamics must be boolean indicating whether the animation should be looped.");
                const bool rotate = StoHelp::readBool(inputStorage, "The fifth parameter for adding polygon dynamics must be boolean indicating whether the tracking polygon should be rotated.");
                libsumo::Polygon::addDynamics(id, trackedID, timeSpan, alphaSpan, looped, rotate);
            }
            break;
            case libsumo::REMOVE: {
                libsumo::Polygon::remove(id, StoHelp::readTypedInt(inputStorage, "The layer must be given using an int."));
            }
            break;
            case libsumo::VAR_PARAMETER: {
                StoHelp::readCompound(inputStorage, 2, "A compound object of size 2 is needed for setting a parameter.");
                const std::string name = StoHelp::readTypedString(inputStorage, "The name of the parameter must be given as a string.");
                const std::string value = StoHelp::readTypedString(inputStorage, "The value of the parameter must be given as a string.");
                libsumo::Polygon::setParameter(id, name, value);
            }
            break;
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_POLYGON_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_SET_POLYGON_VARIABLE, libsumo::RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
