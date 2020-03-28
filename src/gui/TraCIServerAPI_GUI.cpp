/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @date    07.05.2009
///
// APIs for getting/setting GUI values via TraCI
/****************************************************************************/
#include <config.h>

#include <fx.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUIGlChildWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <microsim/MSVehicleControl.h>
#include <libsumo/TraCIConstants.h>
#include <guisim/GUINet.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIBaseVehicle.h>
#include "TraCIServerAPI_GUI.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_GUI::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                               tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != libsumo::TRACI_ID_LIST && variable != libsumo::VAR_VIEW_ZOOM && variable != libsumo::VAR_VIEW_OFFSET
            && variable != libsumo::VAR_VIEW_SCHEMA && variable != libsumo::VAR_VIEW_BOUNDARY && variable != libsumo::VAR_HAS_VIEW
            && variable != libsumo::VAR_TRACK_VEHICLE) {
        return server.writeErrorStatusCmd(libsumo::CMD_GET_GUI_VARIABLE, "Get GUI Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(libsumo::RESPONSE_GET_GUI_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable == libsumo::TRACI_ID_LIST) {
        std::vector<std::string> ids = GUIMainWindow::getInstance()->getViewIDs();
        tempMsg.writeUnsignedByte(libsumo::TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
        GUISUMOAbstractView* v = getNamedView(id);
        if (v == nullptr && variable != libsumo::VAR_HAS_VIEW) {
            return server.writeErrorStatusCmd(libsumo::CMD_GET_GUI_VARIABLE, "View '" + id + "' is not known", outputStorage);
        }
        switch (variable) {
            case libsumo::VAR_VIEW_ZOOM:
                tempMsg.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                tempMsg.writeDouble(v->getChanger().getZoom());
                break;
            case libsumo::VAR_VIEW_OFFSET:
                tempMsg.writeUnsignedByte(libsumo::POSITION_2D);
                tempMsg.writeDouble(v->getChanger().getXPos());
                tempMsg.writeDouble(v->getChanger().getYPos());
                break;
            case libsumo::VAR_VIEW_SCHEMA:
                tempMsg.writeUnsignedByte(libsumo::TYPE_STRING);
                tempMsg.writeString(v->getVisualisationSettings().name);
                break;
            case libsumo::VAR_VIEW_BOUNDARY: {
                tempMsg.writeUnsignedByte(libsumo::TYPE_POLYGON);
                Boundary b = v->getVisibleBoundary();
                tempMsg.writeByte(2);
                tempMsg.writeDouble(b.xmin());
                tempMsg.writeDouble(b.ymin());
                tempMsg.writeDouble(b.xmax());
                tempMsg.writeDouble(b.ymax());
            }
            break;
            case libsumo::VAR_HAS_VIEW: {
                tempMsg.writeUnsignedByte(libsumo::TYPE_INTEGER);
                tempMsg.writeInt(v != nullptr ? 1 : 0);
            }
            break;
            case libsumo::VAR_TRACK_VEHICLE: {
                GUIVehicle* gv = 0;
                std::string id;
                GUIGlID gid = v->getTrackedID();
                if (gid != GUIGlObject::INVALID_ID) {
                    gv = static_cast<GUIVehicle*>(GUIGlObjectStorage::gIDStorage.getObjectBlocking(gid));
                }
                if (gv == 0) {
                    id = "";
                } else {
                    id = gv->getID();
                }
                tempMsg.writeUnsignedByte(libsumo::TYPE_STRING);
                tempMsg.writeString(id);
                if (gid != GUIGlObject::INVALID_ID) {
                    GUIGlObjectStorage::gIDStorage.unblockObject(gid);
                }
            }
            break;
            default:
                break;
        }
    }
    server.writeStatusCmd(libsumo::CMD_GET_GUI_VARIABLE, libsumo::RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


bool
TraCIServerAPI_GUI::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                               tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != libsumo::VAR_VIEW_ZOOM && variable != libsumo::VAR_VIEW_OFFSET
            && variable != libsumo::VAR_VIEW_SCHEMA && variable != libsumo::VAR_VIEW_BOUNDARY
            && variable != libsumo::VAR_SCREENSHOT && variable != libsumo::VAR_TRACK_VEHICLE
       ) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "Change GUI State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    const std::string id = inputStorage.readString();
    GUISUMOAbstractView* v = getNamedView(id);
    if (v == nullptr) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "View '" + id + "' is not known", outputStorage);
    }
    // process
    switch (variable) {
        case libsumo::VAR_VIEW_ZOOM: {
            Position off, p;
            double zoom = 1;
            if (!server.readTypeCheckingDouble(inputStorage, zoom)) {
                return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The zoom must be given as a double.", outputStorage);
            }
            off.set(v->getChanger().getXPos(), v->getChanger().getYPos(), v->getChanger().zoom2ZPos(zoom));
            p.set(off.x(), off.y(), 0);
            v->setViewportFromToRot(off, p, v->getChanger().getRotation());
        }
        break;
        case libsumo::VAR_VIEW_OFFSET: {
            libsumo::TraCIPosition tp;
            if (!server.readTypeCheckingPosition2D(inputStorage, tp)) {
                return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The view port must be given as a position.", outputStorage);
            }

            Position off, p;
            off.set(tp.x, tp.y, v->getChanger().getZPos());
            p.set(tp.x, tp.y, 0);
            v->setViewportFromToRot(off, p, v->getChanger().getRotation());
        }
        break;
        case libsumo::VAR_VIEW_SCHEMA: {
            std::string schema;
            if (!server.readTypeCheckingString(inputStorage, schema)) {
                return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The scheme must be specified by a string.", outputStorage);
            }
            if (!v->setColorScheme(schema)) {
                return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The scheme is not known.", outputStorage);
            }
        }
        break;
        case libsumo::VAR_VIEW_BOUNDARY: {
            PositionVector p;
            if (!server.readTypeCheckingPolygon(inputStorage, p)) {
                return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "The boundary must be specified by a bounding box.", outputStorage);
            }
            v->centerTo(Boundary(p[0].x(), p[0].y(), p[1].x(), p[1].y()));
        }
        break;
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
            v->addSnapshot(MSNet::getInstance()->getCurrentTimeStep(), filename, width, height);
        }
        break;
        case libsumo::VAR_TRACK_VEHICLE: {
            std::string vehID;
            if (!server.readTypeCheckingString(inputStorage, vehID)) {
                return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "Tracking requires a string vehicle ID.", outputStorage);
            }
            if (vehID == "") {
                v->stopTrack();
            } else {
                SUMOVehicle* veh = MSNet::getInstance()->getVehicleControl().getVehicle(vehID);
                if (veh == nullptr) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, "Could not find vehicle '" + vehID + "'.", outputStorage);
                }
                if (v->getTrackedID() != static_cast<GUIVehicle*>(veh)->getGlID()) {
                    v->startTrack(static_cast<GUIVehicle*>(veh)->getGlID());
                }
            }
        }
        break;
        default:
            break;
    }
    server.writeStatusCmd(libsumo::CMD_SET_GUI_VARIABLE, libsumo::RTYPE_OK, warning, outputStorage);
    return true;
}


GUISUMOAbstractView*
TraCIServerAPI_GUI::getNamedView(const std::string& id) {
    GUIMainWindow* const mw = GUIMainWindow::getInstance();
    if (mw == nullptr) {
        return nullptr;
    }
    GUIGlChildWindow* const c = mw->getViewByID(id);
    if (c == nullptr) {
        return nullptr;
    }
    return c->getView();
}


/****************************************************************************/
