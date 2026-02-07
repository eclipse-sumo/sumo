/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
                    StoHelp::writeTypedInt(server.getWrapperStorage(), libsumo::GUI::isSelected(id, StoHelp::readTypedString(inputStorage, "The type of the object must be given as a string.")) ? 1 : 0);
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
                libsumo::GUI::setZoom(id, StoHelp::readTypedDouble(inputStorage, "The zoom must be given as a double."));
                break;
            }
            case libsumo::VAR_VIEW_OFFSET: {
                const libsumo::TraCIPosition tp = StoHelp::readTypedPosition2D(inputStorage,  "The view port must be given as a position.");
                libsumo::GUI::setOffset(id, tp.x, tp.y);
                break;
            }
            case libsumo::VAR_SELECT: {
                libsumo::GUI::toggleSelection(id, StoHelp::readTypedString(inputStorage, "The type of the object must be given as a string."));
                break;
            }
            case libsumo::VAR_VIEW_SCHEMA: {
                libsumo::GUI::setSchema(id, StoHelp::readTypedString(inputStorage, "The scheme must be specified by a string."));
                break;
            }
            case libsumo::VAR_VIEW_BOUNDARY: {
                const libsumo::TraCIPositionVector tp = StoHelp::readTypedPolygon(inputStorage, "The boundary must be specified by a bounding box.");
                libsumo::GUI::setBoundary(id, tp.value[0].x, tp.value[0].y, tp.value[1].x, tp.value[1].y);
                break;
            }
            case libsumo::VAR_ANGLE: {
                libsumo::GUI::setAngle(id, StoHelp::readTypedDouble(inputStorage, "The rotation must be given as a double."));
                break;
            }
            case libsumo::VAR_SCREENSHOT: {
                StoHelp::readCompound(inputStorage, 3, "Screenshot requires three values as parameter.");
                const std::string filename = StoHelp::readTypedString(inputStorage, "The first variable must be a file name.");
                const int width = StoHelp::readTypedInt(inputStorage, "The second variable must be the width given as int.");
                const int height = StoHelp::readTypedInt(inputStorage, "The third variable must be the height given as int.");
                // take screenshot after the current step is finished (showing the same state as sumo-gui and netstate-output)
                libsumo::GUI::screenshot(id, filename, width, height);
                break;
            }
            case libsumo::VAR_TRACK_VEHICLE: {
                libsumo::GUI::trackVehicle(id, StoHelp::readTypedString(inputStorage, "Tracking requires a string ID."));
                break;
            }
            case libsumo::ADD: {
                StoHelp::readCompound(inputStorage, 2, "Adding a view requires two values as parameter.");
                const std::string scheme = StoHelp::readTypedString(inputStorage, "The first variable must be a scheme name.");
                const int viewType = StoHelp::readTypedInt(inputStorage, "The second variable must be the view type given as int.");
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
