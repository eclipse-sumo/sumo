/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    TraCIServerAPI_GUI.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Mirko Barthauer
/// @date    07.05.2009
///
// APIs for getting/setting GUI values via TraCI
/****************************************************************************/
#include <config.h>

#include <libsumo/GUI.h>
#include <libsumo/StorageHelper.h>
#include <libsumo/TraCIConstants.h>
#include "GUISUMOViewParent.h"
#include "TraCIServerAPI_GUI.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_GUI::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                               tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(libsumo::RESPONSE_GET_GUI_VARIABLE, variable, id);
    try {
        if (!libsumo::GUI::handleVariable(id, variable, &server, &inputStorage)) {
            switch (variable) {
                case libsumo::VAR_SELECT: {
                    std::string objType;
                    if (!server.readTypeCheckingString(inputStorage, objType)) {
                        return server.writeErrorStatusCmd(libsumo::CMD_GET_GUI_VARIABLE, "The type of the object must be given as a string.", outputStorage);
                    }
                    StoHelp::writeTypedInt(server.getWrapperStorage(), libsumo::GUI::isSelected(id, objType) ? 1 : 0);
                    break;
                }
                default:
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_GUI_VARIABLE, "Get GUI Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
            }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_GET_GUI_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_GET_GUI_VARIABLE, libsumo::RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_GUI::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                               tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    const int variable = inputStorage.readUnsignedByte();
    if (variable != libsumo::VAR_VIEW_ZOOM && variable != libsumo::VAR_VIEW_OFFSET
            && variable != libsumo::VAR_VIEW_SCHEMA && variable != libsumo::VAR_VIEW_BOUNDARY
            && variable != libsumo::VAR_SCREENSHOT && variable != libsumo::VAR_TRACK_VEHICLE
            && variable != libsumo::VAR_SELECT && variable != libsumo::VAR_ANGLE
            && variable != libsumo::ADD && variable != libsumo::REMOVE
       ) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "Change GUI State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    const std::string id = inputStorage.readString();
    try {
        switch (variable) {
            case libsumo::VAR_VIEW_ZOOM: {
                double zoom = 0.;
                if (!server.readTypeCheckingDouble(inputStorage, zoom)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The zoom must be given as a double.", outputStorage);
                }
                libsumo::GUI::setZoom(id, zoom);
                break;
            }
            case libsumo::VAR_VIEW_OFFSET: {
                libsumo::TraCIPosition tp;
                if (!server.readTypeCheckingPosition2D(inputStorage, tp)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The view port must be given as a position.", outputStorage);
                }
                libsumo::GUI::setOffset(id, tp.x, tp.y);
                break;
            }
            case libsumo::VAR_SELECT: {
                std::string objType;
                if (!server.readTypeCheckingString(inputStorage, objType)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The type of the object must be given as a string.", outputStorage);
                }
                libsumo::GUI::toggleSelection(id, objType);
                break;
            }
            case libsumo::VAR_VIEW_SCHEMA: {
                std::string schema;
                if (!server.readTypeCheckingString(inputStorage, schema)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The scheme must be specified by a string.", outputStorage);
                }
                libsumo::GUI::setSchema(id, schema);
                break;
            }
            case libsumo::VAR_VIEW_BOUNDARY: {
                PositionVector p;
                if (!server.readTypeCheckingPolygon(inputStorage, p)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The boundary must be specified by a bounding box.", outputStorage);
                }
                libsumo::GUI::setBoundary(id, p[0].x(), p[0].y(), p[1].x(), p[1].y());
                break;
            }
            case libsumo::VAR_ANGLE: {
                double rot;
                if (!server.readTypeCheckingDouble(inputStorage, rot)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The rotation must be given as a double.", outputStorage);
                }
                libsumo::GUI::setAngle(id, rot);
                break;
            }
            case libsumo::VAR_SCREENSHOT: {
                if (inputStorage.readUnsignedByte() != libsumo::TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "Screenshot requires a compound object.", outputStorage);
                }
                int parameterCount = inputStorage.readInt();
                if (parameterCount != 3) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "Screenshot requires three values as parameter.", outputStorage);
                }
                std::string filename;
                if (!server.readTypeCheckingString(inputStorage, filename)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The first variable must be a file name.", outputStorage);
                }
                int width = 0, height = 0;
                if (!server.readTypeCheckingInt(inputStorage, width)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The second variable must be the width given as int.", outputStorage);
                }
                if (!server.readTypeCheckingInt(inputStorage, height)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The third variable must be the height given as int.", outputStorage);
                }
                // take screenshot after the current step is finished (showing the same state as sumo-gui and netstate-output)
                libsumo::GUI::screenshot(id, filename, width, height);
                break;
            }
            case libsumo::VAR_TRACK_VEHICLE: {
                std::string objID;
                if (!server.readTypeCheckingString(inputStorage, objID)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "Tracking requires a string ID.", outputStorage);
                }
                libsumo::GUI::trackVehicle(id, objID);
                break;
            }
            case libsumo::ADD: {
                if (inputStorage.readUnsignedByte() != libsumo::TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "Adding a view requires a compound object.", outputStorage);
                }
                int parameterCount = inputStorage.readInt();
                if (parameterCount != 2) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "Adding a view requires two values as parameter.", outputStorage);
                }
                std::string scheme;
                if (!server.readTypeCheckingString(inputStorage, scheme)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The first variable must be a scheme name.", outputStorage);
                }
                int viewType;
                if (!server.readTypeCheckingInt(inputStorage, viewType)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The second variable must be the view type given as int.", outputStorage);
                }
                libsumo::GUI::addView(id, scheme,
                                      viewType == 1 ?  GUISUMOViewParent::VIEW_3D_OSG : GUISUMOViewParent::VIEW_2D_OPENGL);
                break;
            }
            case libsumo::REMOVE: {
                libsumo::GUI::removeView(id);
                break;
            }
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, libsumo::RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
