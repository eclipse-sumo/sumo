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
/// @file    Polygon.cpp
/// @author  Gregor L\"ammel
/// @date    15.03.2017
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include "Domain.h"
#include <libsumo/Polygon.h>
#include <libsumo/StorageHelper.h>


namespace libtraci {

typedef Domain<libsumo::CMD_GET_POLYGON_VARIABLE, libsumo::CMD_SET_POLYGON_VARIABLE> Dom;

// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
Polygon::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


int
Polygon::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


std::string
Polygon::getType(const std::string& polygonID) {
    return Dom::getString(libsumo::VAR_TYPE, polygonID);
}


libsumo::TraCIPositionVector
Polygon::getShape(const std::string& polygonID) {
    return Dom::getPolygon(libsumo::VAR_SHAPE, polygonID);
}


bool
Polygon::getFilled(const std::string& polygonID) {
    return Dom::getInt(libsumo::VAR_FILL, polygonID) != 0;
}


double
Polygon::getLineWidth(const std::string& polygonID) {
    return Dom::getDouble(libsumo::VAR_WIDTH, polygonID);
}


libsumo::TraCIColor
Polygon::getColor(const std::string& polygonID) {
    return Dom::getCol(libsumo::VAR_COLOR, polygonID);
}


LIBTRACI_PARAMETER_IMPLEMENTATION(Polygon, POLYGON)


void
Polygon::setType(const std::string& polygonID, const std::string& setType) {
    Dom::setString(libsumo::VAR_TYPE, polygonID, setType);
}


void
Polygon::setShape(const std::string& polygonID, const libsumo::TraCIPositionVector& shape) {
    tcpip::Storage content;
    StoHelp::writePolygon(content, shape);
    return Dom::set(libsumo::VAR_SHAPE, polygonID, &content);
}


void
Polygon::setColor(const std::string& polygonID, const libsumo::TraCIColor& c) {
    Dom::setCol(libsumo::VAR_COLOR, polygonID, c);
}


void
Polygon::add(const std::string& polygonID, const libsumo::TraCIPositionVector& shape, const libsumo::TraCIColor& color, bool fill, const std::string& polygonType, int layer, double lineWidth) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 6);
    StoHelp::writeTypedString(content, polygonType);
    content.writeUnsignedByte(libsumo::TYPE_COLOR);
    content.writeUnsignedByte(color.r);
    content.writeUnsignedByte(color.g);
    content.writeUnsignedByte(color.b);
    content.writeUnsignedByte(color.a);
    content.writeUnsignedByte(libsumo::TYPE_UBYTE);
    content.writeUnsignedByte(fill);
    StoHelp::writeTypedInt(content, layer);
    StoHelp::writePolygon(content, shape);
    StoHelp::writeTypedDouble(content, lineWidth);
    Dom::set(libsumo::ADD, polygonID, &content);
}


void
Polygon::addDynamics(const std::string& polygonID, const std::string& trackedObjectID, const std::vector<double>& timeSpan, const std::vector<double>& alphaSpan, bool looped, bool rotate) {
    tcpip::Storage content;
    StoHelp::writeCompound(content, 5);
    StoHelp::writeTypedString(content, trackedObjectID);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLELIST);
    content.writeInt((int)timeSpan.size());
    for (const double d : timeSpan) {
        content.writeDouble(d);
    }
    content.writeUnsignedByte(libsumo::TYPE_DOUBLELIST);
    content.writeInt((int)alphaSpan.size());
    for (const double d : alphaSpan) {
        content.writeDouble(d);
    }
    content.writeUnsignedByte(libsumo::TYPE_UBYTE);
    content.writeUnsignedByte(looped);
    content.writeUnsignedByte(libsumo::TYPE_UBYTE);
    content.writeUnsignedByte(rotate);
    Dom::set(libsumo::VAR_ADD_DYNAMICS, polygonID, &content);
}


void
Polygon::remove(const std::string& polygonID, int layer) {
    Dom::setInt(libsumo::REMOVE, polygonID, layer);
}


void
Polygon::setFilled(std::string polygonID, bool filled) {
    Dom::setInt(libsumo::VAR_FILL, polygonID, filled);
}


void
Polygon::setLineWidth(std::string polygonID, double lineWidth) {
    Dom::setDouble(libsumo::VAR_WIDTH, polygonID, lineWidth);
}


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(Polygon, POLYGON)


}


/****************************************************************************/
