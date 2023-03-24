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
/// @file    POI.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include "Domain.h"
#include <libsumo/POI.h>
#include <libsumo/StorageHelper.h>


namespace libtraci {

typedef Domain<libsumo::CMD_GET_POI_VARIABLE, libsumo::CMD_SET_POI_VARIABLE> Dom;

// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
POI::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


int
POI::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


std::string
POI::getType(const std::string& poiID) {
    return Dom::getString(libsumo::VAR_TYPE, poiID);
}


libsumo::TraCIColor
POI::getColor(const std::string& poiID) {
    return Dom::getCol(libsumo::VAR_COLOR, poiID);
}


libsumo::TraCIPosition
POI::getPosition(const std::string& poiID, const bool includeZ) {
    return includeZ ? Dom::getPos3D(libsumo::VAR_POSITION3D, poiID) : Dom::getPos(libsumo::VAR_POSITION, poiID);
}


double
POI::getWidth(const std::string& poiID) {
    return Dom::getDouble(libsumo::VAR_WIDTH, poiID);
}


double
POI::getHeight(const std::string& poiID) {
    return Dom::getDouble(libsumo::VAR_HEIGHT, poiID);
}


double
POI::getAngle(const std::string& poiID) {
    return Dom::getDouble(libsumo::VAR_ANGLE, poiID);
}


std::string
POI::getImageFile(const std::string& poiID) {
    return Dom::getString(libsumo::VAR_IMAGEFILE, poiID);
}


LIBTRACI_PARAMETER_IMPLEMENTATION(POI, POI)


void
POI::setType(const std::string& poiID, const std::string& type) {
    Dom::setString(libsumo::VAR_TYPE, poiID, type);
}


void
POI::setPosition(const std::string& poiID, double x, double y) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::POSITION_2D);
    content.writeDouble(x);
    content.writeDouble(y);
    Dom::set(libsumo::VAR_POSITION, poiID, &content);
}


void
POI::setColor(const std::string& poiID, const libsumo::TraCIColor& c) {
    Dom::setCol(libsumo::VAR_COLOR, poiID, c);
}


void
POI::setWidth(const std::string& poiID, double width) {
    Dom::setDouble(libsumo::VAR_WIDTH, poiID, width);
}


void
POI::setHeight(const std::string& poiID, double height) {
    Dom::setDouble(libsumo::VAR_HEIGHT, poiID, height);
}


void
POI::setAngle(const std::string& poiID, double angle) {
    Dom::setDouble(libsumo::VAR_ANGLE, poiID, angle);
}


void
POI::setImageFile(const std::string& poiID, const std::string& imageFile) {
    Dom::setString(libsumo::VAR_IMAGEFILE, poiID, imageFile);
}


bool
POI::add(const std::string& poiID, double x, double y, const libsumo::TraCIColor& color, const std::string& poiType, int layer, const std::string& imgFile, double width, double height, double angle) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 8);
    StoHelp::writeTypedString(content, poiType);
    content.writeUnsignedByte(libsumo::TYPE_COLOR);
    content.writeUnsignedByte(color.r);
    content.writeUnsignedByte(color.g);
    content.writeUnsignedByte(color.b);
    content.writeUnsignedByte(color.a);
    StoHelp::writeTypedInt(content, layer);
    content.writeUnsignedByte(libsumo::POSITION_2D);
    content.writeDouble(x);
    content.writeDouble(y);
    StoHelp::writeTypedString(content, imgFile);
    StoHelp::writeTypedDouble(content, width);
    StoHelp::writeTypedDouble(content, height);
    StoHelp::writeTypedDouble(content, angle);
    Dom::set(libsumo::ADD, poiID, &content);
    return true;
}


bool
POI::remove(const std::string& poiID, int layer) {
    Dom::setInt(libsumo::REMOVE, poiID, layer);
    return true;
}


void
POI::highlight(const std::string& poiID, const libsumo::TraCIColor& col, double size, const int alphaMax, const double duration, const int type) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, alphaMax > 0 ? 5 : 2);
    content.writeUnsignedByte(libsumo::TYPE_COLOR);
    content.writeUnsignedByte(col.r);
    content.writeUnsignedByte(col.g);
    content.writeUnsignedByte(col.b);
    content.writeUnsignedByte(col.a);
    StoHelp::writeTypedDouble(content, size);
    if (alphaMax > 0) {
        content.writeUnsignedByte(libsumo::TYPE_UBYTE);
        content.writeUnsignedByte(alphaMax);
        StoHelp::writeTypedDouble(content, duration);
        content.writeUnsignedByte(libsumo::TYPE_UBYTE);
        content.writeUnsignedByte(type);
    }
    Dom::set(libsumo::VAR_HIGHLIGHT, poiID, &content);
}


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(POI, POI)


}


/****************************************************************************/
