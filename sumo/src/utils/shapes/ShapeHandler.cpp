
/****************************************************************************/
/// @file    ShapeHandler.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2015
/// @version $Id$
///
// The XML-Handler for network loading
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ShapeHandler::ShapeHandler(const std::string& file, ShapeContainer& sc) :
    SUMOSAXHandler(file), myShapeContainer(sc),
    myPrefix(""), myDefaultColor(RGBColor::RED), myDefaultLayer(), myDefaultFill(false) {}


ShapeHandler::~ShapeHandler() {}


void
ShapeHandler::myStartElement(int element,
                             const SUMOSAXAttributes& attrs) {
    try {
        switch (element) {
            case SUMO_TAG_POLY:
                myDefaultLayer = Shape::DEFAULT_LAYER;
                addPoly(attrs, false, false);
                break;
            case SUMO_TAG_POI:
                myDefaultLayer = (SUMOReal)GLO_POI;
                addPOI(attrs, false, false);
                break;
            default:
                break;
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
}



void
ShapeHandler::addPOI(const SUMOSAXAttributes& attrs, const bool ignorePruning, const bool useProcessing) {
    bool ok = true;
    const SUMOReal INVALID_POSITION(-1000000);
    const std::string id = myPrefix + attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    SUMOReal x = attrs.getOpt<SUMOReal>(SUMO_ATTR_X, id.c_str(), ok, INVALID_POSITION);
    const SUMOReal y = attrs.getOpt<SUMOReal>(SUMO_ATTR_Y, id.c_str(), ok, INVALID_POSITION);
    SUMOReal lon = attrs.getOpt<SUMOReal>(SUMO_ATTR_LON, id.c_str(), ok, INVALID_POSITION);
    SUMOReal lat = attrs.getOpt<SUMOReal>(SUMO_ATTR_LAT, id.c_str(), ok, INVALID_POSITION);
    const SUMOReal lanePos = attrs.getOpt<SUMOReal>(SUMO_ATTR_POSITION, id.c_str(), ok, INVALID_POSITION);
    const SUMOReal layer = attrs.getOpt<SUMOReal>(SUMO_ATTR_LAYER, id.c_str(), ok, myDefaultLayer);
    const std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    const std::string laneID = attrs.getOpt<std::string>(SUMO_ATTR_LANE, id.c_str(), ok, "");
    const RGBColor color = attrs.hasAttribute(SUMO_ATTR_COLOR) ? attrs.get<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), ok) : myDefaultColor;
    const SUMOReal angle = attrs.getOpt<SUMOReal>(SUMO_ATTR_ANGLE, id.c_str(), ok, Shape::DEFAULT_ANGLE);
    std::string imgFile = attrs.getOpt<std::string>(SUMO_ATTR_IMGFILE, id.c_str(), ok, Shape::DEFAULT_IMG_FILE);
    if (imgFile != "" && !FileHelpers::isAbsolute(imgFile)) {
        imgFile = FileHelpers::getConfigurationRelative(getFileName(), imgFile);
    }
    const SUMOReal width = attrs.getOpt<SUMOReal>(SUMO_ATTR_WIDTH, id.c_str(), ok, Shape::DEFAULT_IMG_WIDTH);
    const SUMOReal height = attrs.getOpt<SUMOReal>(SUMO_ATTR_HEIGHT, id.c_str(), ok, Shape::DEFAULT_IMG_HEIGHT);
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
            pos = getLanePos(id, laneID, lanePos);
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
    if (!myShapeContainer.addPOI(id, type, color, layer, angle, imgFile, pos, width, height, ignorePruning)) {
        WRITE_ERROR("PoI '" + id + "' already exists.");
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
    const SUMOReal layer = attrs.getOpt<SUMOReal>(SUMO_ATTR_LAYER, id.c_str(), ok, myDefaultLayer);
    const bool fill = attrs.getOpt<bool>(SUMO_ATTR_FILL, id.c_str(), ok, myDefaultFill);
    const std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, Shape::DEFAULT_TYPE);
    const RGBColor color = attrs.hasAttribute(SUMO_ATTR_COLOR) ? attrs.get<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), ok) : myDefaultColor;
    PositionVector shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok);
    if (attrs.getOpt<bool>(SUMO_ATTR_GEO, id.c_str(), ok, false)) {
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
    const SUMOReal angle = attrs.getOpt<SUMOReal>(SUMO_ATTR_ANGLE, id.c_str(), ok, Shape::DEFAULT_ANGLE);
    std::string imgFile = attrs.getOpt<std::string>(SUMO_ATTR_IMGFILE, id.c_str(), ok, Shape::DEFAULT_IMG_FILE);
    if (imgFile != "" && !FileHelpers::isAbsolute(imgFile)) {
        imgFile = FileHelpers::getConfigurationRelative(getFileName(), imgFile);
    }
    if (!myShapeContainer.addPolygon(id, type, color, layer, angle, imgFile, shape, fill, ignorePruning)) {
        WRITE_ERROR("Polygon '" + id + "' already exists.");
    }
}



bool
ShapeHandler::loadFiles(const std::vector<std::string>& files, ShapeHandler& sh) {
    for (std::vector<std::string>::const_iterator fileIt = files.begin(); fileIt != files.end(); ++fileIt) {
        if (!XMLSubSys::runParser(sh, *fileIt, false)) {
            WRITE_MESSAGE("Loading of shapes from " + *fileIt + " failed.");
            return false;
        }
    }
    return true;
}


void
ShapeHandler::setDefaults(const std::string& prefix, const RGBColor& color, const SUMOReal layer, const bool fill) {
    myPrefix = prefix;
    myDefaultColor = color;
    myDefaultLayer = layer;
    myDefaultFill = fill;
}



/****************************************************************************/
