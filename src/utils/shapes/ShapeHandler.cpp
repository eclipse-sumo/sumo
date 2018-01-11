/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ShapeHandler.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2015
/// @version $Id$
///
// The XML-Handler for network loading
/****************************************************************************/
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include "Shape.h"
#include "ShapeContainer.h"
#include "ShapeHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================
ShapeHandler::ShapeHandler(const std::string& file, ShapeContainer& sc) :
    SUMOSAXHandler(file), myShapeContainer(sc),
    myPrefix(""), myDefaultColor(RGBColor::RED), myDefaultLayer(), myDefaultFill(false),
    myLastParameterised(0) {
}


ShapeHandler::~ShapeHandler() {}


void
ShapeHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    try {
        switch (element) {
            case SUMO_TAG_POLY:
                myDefaultLayer = Shape::DEFAULT_LAYER;
                addPoly(attrs, false, false);
                break;
            case SUMO_TAG_POI:
                myDefaultLayer = (double)GLO_POI;
                addPOI(attrs, false, false);
                break;
            case SUMO_TAG_PARAM:
                if (myLastParameterised != 0) {
                    bool ok = true;
                    const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, 0, ok);
                    // circumventing empty string test
                    const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
                    myLastParameterised->setParameter(key, val);
                }
            default:
                break;
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
}


void
ShapeHandler::myEndElement(int element) {
    if (element != SUMO_TAG_PARAM) {
        myLastParameterised = 0;
    }
}

void
ShapeHandler::addPOI(const SUMOSAXAttributes& attrs, const bool ignorePruning, const bool useProcessing) {
    bool ok = true;
    const double INVALID_POSITION(-1000000);
    const std::string id = myPrefix + attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    double x = attrs.getOpt<double>(SUMO_ATTR_X, id.c_str(), ok, INVALID_POSITION);
    const double y = attrs.getOpt<double>(SUMO_ATTR_Y, id.c_str(), ok, INVALID_POSITION);
    double lon = attrs.getOpt<double>(SUMO_ATTR_LON, id.c_str(), ok, INVALID_POSITION);
    double lat = attrs.getOpt<double>(SUMO_ATTR_LAT, id.c_str(), ok, INVALID_POSITION);
    const double lanePos = attrs.getOpt<double>(SUMO_ATTR_POSITION, id.c_str(), ok, 0);
    const double lanePosLat = attrs.getOpt<double>(SUMO_ATTR_POSITION_LAT, id.c_str(), ok, 0);
    const double layer = attrs.getOpt<double>(SUMO_ATTR_LAYER, id.c_str(), ok, myDefaultLayer);
    const std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    const std::string laneID = attrs.getOpt<std::string>(SUMO_ATTR_LANE, id.c_str(), ok, "");
    const RGBColor color = attrs.hasAttribute(SUMO_ATTR_COLOR) ? attrs.get<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), ok) : myDefaultColor;
    const double angle = attrs.getOpt<double>(SUMO_ATTR_ANGLE, id.c_str(), ok, Shape::DEFAULT_ANGLE);
    std::string imgFile = attrs.getOpt<std::string>(SUMO_ATTR_IMGFILE, id.c_str(), ok, Shape::DEFAULT_IMG_FILE);
    if (imgFile != "" && !FileHelpers::isAbsolute(imgFile)) {
        imgFile = FileHelpers::getConfigurationRelative(getFileName(), imgFile);
    }
    const double width = attrs.getOpt<double>(SUMO_ATTR_WIDTH, id.c_str(), ok, Shape::DEFAULT_IMG_WIDTH);
    const double height = attrs.getOpt<double>(SUMO_ATTR_HEIGHT, id.c_str(), ok, Shape::DEFAULT_IMG_HEIGHT);
    if (!ok) {
        return;
    }
    const GeoConvHelper& gch = useProcessing ? GeoConvHelper::getProcessing() : GeoConvHelper::getFinal();
    if (useProcessing && gch.usingGeoProjection()) {
        if (lat == INVALID_POSITION || lon == INVALID_POSITION) {
            lon = x;
            lat = y;
            x = INVALID_POSITION;
        }
    }
    Position pos(x, y);
    if (x == INVALID_POSITION || y == INVALID_POSITION) {
        // try computing x,y from lane,pos
        if (laneID != "") {
            pos = getLanePos(id, laneID, lanePos, lanePosLat);
        } else {
            // try computing x,y from lon,lat
            if (lat == INVALID_POSITION || lon == INVALID_POSITION) {
                WRITE_ERROR("Either (x, y), (lon, lat) or (lane, pos) must be specified for PoI '" + id + "'.");
                return;
            } else if (!gch.usingGeoProjection()) {
                WRITE_ERROR("(lon, lat) is specified for PoI '" + id + "' but no geo-conversion is specified for the network.");
                return;
            }
            pos.set(lon, lat);
            bool success = true;
            if (useProcessing) {
                success = GeoConvHelper::getProcessing().x2cartesian(pos);
            } else {
                success = GeoConvHelper::getFinal().x2cartesian_const(pos);
            }
            if (!success) {
                WRITE_ERROR("Unable to project coordinates for PoI '" + id + "'.");
                return;
            }
        }
    }
    if (!myShapeContainer.addPOI(id, type, color, pos, gch.usingGeoProjection(), laneID, lanePos, lanePosLat, layer, angle, imgFile, width, height, ignorePruning)) {
        WRITE_ERROR("PoI '" + id + "' already exists.");
    }
    myLastParameterised = myShapeContainer.getPOIs().get(id);
    if (laneID != "" && addLanePosParams()) {
        myLastParameterised->setParameter(toString(SUMO_ATTR_LANE), laneID);
        myLastParameterised->setParameter(toString(SUMO_ATTR_POSITION), toString(lanePos));
        myLastParameterised->setParameter(toString(SUMO_ATTR_POSITION_LAT), toString(lanePosLat));
    }
}


void
ShapeHandler::addPoly(const SUMOSAXAttributes& attrs, const bool ignorePruning, const bool useProcessing) {
    bool ok = true;
    const std::string id = myPrefix + attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    // get the id, report an error if not given or empty...
    if (!ok) {
        return;
    }
    const double layer = attrs.getOpt<double>(SUMO_ATTR_LAYER, id.c_str(), ok, myDefaultLayer);
    const bool fill = attrs.getOpt<bool>(SUMO_ATTR_FILL, id.c_str(), ok, myDefaultFill);
    const std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, Shape::DEFAULT_TYPE);
    const RGBColor color = attrs.hasAttribute(SUMO_ATTR_COLOR) ? attrs.get<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), ok) : myDefaultColor;
    PositionVector shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok);
    bool geo = false;
    if (attrs.getOpt<bool>(SUMO_ATTR_GEO, id.c_str(), ok, false)) {
        geo = true;
        bool success = true;
        for (int i = 0; i < (int)shape.size(); i++) {
            if (useProcessing) {
                success &= GeoConvHelper::getProcessing().x2cartesian(shape[i]);
            } else {
                success &= GeoConvHelper::getFinal().x2cartesian_const(shape[i]);
            }
        }
        if (!success) {
            WRITE_WARNING("Unable to project coordinates for polygon '" + id + "'.");
            return;
        }
    }
    const double angle = attrs.getOpt<double>(SUMO_ATTR_ANGLE, id.c_str(), ok, Shape::DEFAULT_ANGLE);
    std::string imgFile = attrs.getOpt<std::string>(SUMO_ATTR_IMGFILE, id.c_str(), ok, Shape::DEFAULT_IMG_FILE);
    if (imgFile != "" && !FileHelpers::isAbsolute(imgFile)) {
        imgFile = FileHelpers::getConfigurationRelative(getFileName(), imgFile);
    }
    // check that shape's size is valid
    if (shape.size() == 0) {
        WRITE_ERROR("Polygon's shape cannot be empty.");
        return;
    }
    if (!myShapeContainer.addPolygon(id, type, color, layer, angle, imgFile, shape, geo, fill, ignorePruning)) {
        WRITE_ERROR("Polygon '" + id + "' already exists.");
    }
    myLastParameterised = myShapeContainer.getPolygons().get(id);
}



bool
ShapeHandler::loadFiles(const std::vector<std::string>& files, ShapeHandler& sh) {
    for (auto fileIt : files) {
        if (!XMLSubSys::runParser(sh, fileIt, false)) {
            WRITE_MESSAGE("Loading of shapes from " + fileIt + " failed.");
            return false;
        }
    }
    return true;
}


void
ShapeHandler::setDefaults(const std::string& prefix, const RGBColor& color, const double layer, const bool fill) {
    myPrefix = prefix;
    myDefaultColor = color;
    myDefaultLayer = layer;
    myDefaultFill = fill;
}



/****************************************************************************/
