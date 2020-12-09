/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2020 German Aerospace Center (DLR) and others.
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
}


void
POI::setPosition(const std::string& poiID, double x, double y) {
}


void
POI::setColor(const std::string& poiID, const libsumo::TraCIColor& c) {
}


void
POI::setWidth(const std::string& poiID, double width) {
}


void
POI::setHeight(const std::string& poiID, double height) {
}


void
POI::setAngle(const std::string& poiID, double angle) {
}


void
POI::setImageFile(const std::string& poiID, const std::string& imageFile) {
}


bool
POI::add(const std::string& poiID, double x, double y, const libsumo::TraCIColor& color, const std::string& poiType, int layer, const std::string& imgFile, double width, double height, double angle) {
    return true;
}


bool
POI::remove(const std::string& poiID, int /* layer */) {
    return true;
}


void
POI::highlight(const std::string& poiID, const libsumo::TraCIColor& col, double size, const int alphaMax, const double duration, const int type) {
}


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(POI, POI)


}


/****************************************************************************/
