/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2005-2024 German Aerospace Center (DLR) and others.
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
/// @file    PointOfInterest.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Melanie Knocke
/// @date    2005-09-15
///
// A point-of-interest (2D)
/****************************************************************************/
#include <config.h>

#include "PointOfInterest.h"


// ===========================================================================
// member method definitions
// ===========================================================================

PointOfInterest::PointOfInterest(const std::string& id, const std::string& type, const RGBColor& color, const Position& pos,
                                 bool geo, const std::string& lane, double posOverLane, bool friendlyPos, double posLat,
                                 const std::string& icon, double layer, double angle, const std::string& imgFile, bool relativePath,
                                 double width, double height, const std::string& name, const Parameterised::Map& parameters) :
    Shape(id, type, color, layer, angle, imgFile, name, relativePath),
    Position(pos),
    Parameterised(parameters),
    myGeo(geo),
    myLane(lane),
    myPosOverLane(posOverLane),
    myFriendlyPos(friendlyPos),
    myPosLat(posLat),
    myIcon(SUMOXMLDefinitions::POIIcons.get(icon)),
    myHalfImgWidth(width / 2.0),
    myHalfImgHeight(height / 2.0) {
}


PointOfInterest::~PointOfInterest() {}


POIIcon
PointOfInterest::getIcon() const {
    return myIcon;
}


const std::string&
PointOfInterest::getIconStr() const {
    return SUMOXMLDefinitions::POIIcons.getString(myIcon);
}


double
PointOfInterest::getWidth() const {
    return myHalfImgWidth * 2.0;
}


double
PointOfInterest::getHeight() const {
    return myHalfImgHeight * 2.0;
}


Position
PointOfInterest::getCenter() const {
    return { x() + myHalfImgWidth, y() + myHalfImgHeight };
}


bool
PointOfInterest::getFriendlyPos() const {
    return myFriendlyPos;
}


void
PointOfInterest::setIcon(const std::string& icon) {
    myIcon = SUMOXMLDefinitions::POIIcons.get(icon);
}


void
PointOfInterest::setWidth(double width) {
    myHalfImgWidth = width / 2.0;
}


void
PointOfInterest::setHeight(double height) {
    myHalfImgHeight = height / 2.0;
}


void
PointOfInterest::setFriendlyPos(const bool friendlyPos) {
    myFriendlyPos = friendlyPos;
}


void
PointOfInterest::writeXML(OutputDevice& out, const bool geo, const double zOffset, const std::string laneID,
                          const double pos, const bool friendlyPos, const double posLat) const {
    out.openTag(SUMO_TAG_POI);
    out.writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML(getID()));
    if (getShapeType().size() > 0) {
        out.writeAttr(SUMO_ATTR_TYPE, StringUtils::escapeXML(getShapeType()));
    }
    if (myIcon != POIIcon::NONE) {
        out.writeAttr(SUMO_ATTR_ICON, SUMOXMLDefinitions::POIIcons.getString(myIcon));
    }
    out.writeAttr(SUMO_ATTR_COLOR, getShapeColor());
    out.writeAttr(SUMO_ATTR_LAYER, getShapeLayer() + zOffset);
    if (!getShapeName().empty()) {
        out.writeAttr(SUMO_ATTR_NAME, getShapeName());
    }
    if (laneID != "") {
        out.writeAttr(SUMO_ATTR_LANE, laneID);
        out.writeAttr(SUMO_ATTR_POSITION, pos);
        if (posLat != 0) {
            out.writeAttr(SUMO_ATTR_POSITION_LAT, posLat);
        }
        if (friendlyPos) {
            out.writeAttr(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
        }
    } else {
        if (geo) {
            Position POICartesianPos(*this);
            GeoConvHelper::getFinal().cartesian2geo(POICartesianPos);
            out.setPrecision(gPrecisionGeo);
            out.writeAttr(SUMO_ATTR_LON, POICartesianPos.x());
            out.writeAttr(SUMO_ATTR_LAT, POICartesianPos.y());
            out.setPrecision();
        } else {
            out.writeAttr(SUMO_ATTR_X, x());
            out.writeAttr(SUMO_ATTR_Y, y());
        }
        if (z() != 0.) {
            out.writeAttr(SUMO_ATTR_Z, z());
        }
    }
    if (getShapeNaviDegree() != Shape::DEFAULT_ANGLE) {
        out.writeAttr(SUMO_ATTR_ANGLE, getShapeNaviDegree());
    }
    if (getShapeImgFile() != Shape::DEFAULT_IMG_FILE) {
        if (getShapeRelativePath()) {
            // write only the file name, without file path
            std::string file = getShapeImgFile();
            file.erase(0, FileHelpers::getFilePath(getShapeImgFile()).size());
            out.writeAttr(SUMO_ATTR_IMGFILE, file);
        } else {
            out.writeAttr(SUMO_ATTR_IMGFILE, getShapeImgFile());
        }
    }
    if (getWidth() != Shape::DEFAULT_IMG_WIDTH) {
        out.writeAttr(SUMO_ATTR_WIDTH, getWidth());
    }
    if (getHeight() != Shape::DEFAULT_IMG_HEIGHT) {
        out.writeAttr(SUMO_ATTR_HEIGHT, getHeight());
    }
    writeParams(out);
    out.closeTag();
}

/****************************************************************************/
