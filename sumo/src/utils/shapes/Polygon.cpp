/****************************************************************************/
/// @file    Polygon.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Jun 2004
/// @version $Id$
///
// A 2D-polygon
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2004-2014 DLR (http://www.dlr.de/) and contributors
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

#include <utils/iodevices/OutputDevice.h>
#include <utils/common/StringUtils.h>
#include "Polygon.h"
using namespace SUMO;

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member definitions
// ===========================================================================
Polygon::Polygon(const std::string& id, const std::string& type,
                 const RGBColor& color, const PositionVector& shape, bool fill,
                 SUMOReal layer, SUMOReal angle, const std::string& imgFile) :
    Shape(id, type, color, layer, angle, imgFile),
    myShape(shape),
    myFill(fill)
{}


Polygon::~Polygon() {}


void 
Polygon::writeXML(OutputDevice& out) {
    out.openTag(SUMO_TAG_POLY);
    out.writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML(getID()));
    out.writeAttr(SUMO_ATTR_TYPE, StringUtils::escapeXML(getType()));
    out.writeAttr(SUMO_ATTR_COLOR, getColor());
    out.writeAttr(SUMO_ATTR_FILL,  getFill());
    out.writeAttr(SUMO_ATTR_LAYER, getLayer());
    out.writeAttr(SUMO_ATTR_SHAPE, getShape());
    if (getAngle() != Shape::DEFAULT_ANGLE) {
        out.writeAttr(SUMO_ATTR_ANGLE, getAngle());
    }
    if (getImgFile() != Shape::DEFAULT_IMG_FILE) {
        out.writeAttr(SUMO_ATTR_IMGFILE, getImgFile());
    }
    const std::map<std::string, std::string>& attrs = getMap();
    if (attrs.size() != 0) {
        for (std::map<std::string, std::string>::const_iterator j = attrs.begin(); j != attrs.end(); ++j) {
            out.openTag(SUMO_TAG_PARAM);
            out.writeAttr(SUMO_ATTR_KEY, (*j).first);
            out.writeAttr(SUMO_ATTR_VALUE, (*j).second);
            out.closeTag();
        }
    }
    out.closeTag();
}

/****************************************************************************/

