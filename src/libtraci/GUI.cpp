/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2023 German Aerospace Center (DLR) and others.
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
/// @file    GUI.cpp
/// @author  Michael Behrisch
/// @date    07.04.2021
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include "Domain.h"
#include <libsumo/GUI.h>


namespace libtraci {

typedef Domain<libsumo::CMD_GET_GUI_VARIABLE, libsumo::CMD_SET_GUI_VARIABLE> Dom;

// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
GUI::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


int
GUI::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


double
GUI::getZoom(const std::string& viewID) {
    return Dom::getDouble(libsumo::VAR_VIEW_ZOOM, viewID);
}


double
GUI::getAngle(const std::string& viewID) {
    return Dom::getDouble(libsumo::VAR_ANGLE, viewID);
}


libsumo::TraCIPosition
GUI::getOffset(const std::string& viewID) {
    return Dom::getPos(libsumo::VAR_VIEW_OFFSET, viewID);
}


std::string
GUI::getSchema(const std::string& viewID) {
    return Dom::getString(libsumo::VAR_VIEW_SCHEMA, viewID);
}


libsumo::TraCIPositionVector
GUI::getBoundary(const std::string& viewID) {
    return Dom::getPolygon(libsumo::VAR_VIEW_BOUNDARY, viewID);
}


LIBTRACI_PARAMETER_IMPLEMENTATION(GUI, GUI)


void
GUI::setZoom(const std::string& viewID, double zoom) {
    Dom::setDouble(libsumo::VAR_VIEW_ZOOM, viewID, zoom);
}


void
GUI::setAngle(const std::string& viewID, double angle) {
    Dom::setDouble(libsumo::VAR_ANGLE, viewID, angle);
}


void
GUI::setOffset(const std::string& viewID, double x, double y) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::POSITION_2D);
    content.writeDouble(x);
    content.writeDouble(y);
    Dom::set(libsumo::VAR_VIEW_OFFSET, viewID, &content);
}


void
GUI::setSchema(const std::string& viewID, const std::string& schemeName) {
    Dom::setString(libsumo::VAR_VIEW_SCHEMA, viewID, schemeName);
}

void
GUI::addView(const std::string& viewID, const std::string& schemeName, bool in3D) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 2);
    StoHelp::writeTypedString(content, schemeName);
    StoHelp::writeTypedInt(content, in3D ? 1 : 0);
    Dom::set(libsumo::ADD, viewID, &content);
}

void
GUI::removeView(const std::string& viewID) {
    Dom::set(libsumo::REMOVE, viewID, nullptr);
}


void
GUI::setBoundary(const std::string& viewID, double xmin, double ymin, double xmax, double ymax) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_POLYGON);
    content.writeUnsignedByte(2);
    content.writeDouble(xmin);
    content.writeDouble(ymin);
    content.writeDouble(xmax);
    content.writeDouble(ymax);
    Dom::set(libsumo::VAR_VIEW_BOUNDARY, viewID, &content);
}


void
GUI::screenshot(const std::string& viewID, const std::string& filename, const int width, const int height) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 3);
    StoHelp::writeTypedString(content, filename);
    StoHelp::writeTypedInt(content, width);
    StoHelp::writeTypedInt(content, height);
    Dom::set(libsumo::VAR_SCREENSHOT, viewID, &content);
}


void
GUI::trackVehicle(const std::string& viewID, const std::string& vehID) {
    Dom::setString(libsumo::VAR_TRACK_VEHICLE, viewID, vehID);
}


bool
GUI::hasView(const std::string& viewID) {
    return Dom::getInt(libsumo::VAR_HAS_VIEW, viewID) != 0;
}


std::string
GUI::getTrackedVehicle(const std::string& viewID) {
    return Dom::getString(libsumo::VAR_TRACK_VEHICLE, viewID);
}


void
GUI::track(const std::string& objID, const std::string& viewID) {
    Dom::setString(libsumo::VAR_TRACK_VEHICLE, viewID, objID);
}


bool
GUI::isSelected(const std::string& objID, const std::string& objType) {
    tcpip::Storage content;
    StoHelp::writeTypedString(content, objType);
    return Dom::getInt(libsumo::VAR_SELECT, objID, &content) != 0;
}


void
GUI::toggleSelection(const std::string& objID, const std::string& objType) {
    Dom::setString(libsumo::VAR_SELECT, objID, objType);
}


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(GUI, GUI)


}


/****************************************************************************/
