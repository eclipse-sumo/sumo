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
// Copyright (C) 2004-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/geom/GeoConvHelper.h>
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
    myFill(fill) {
}


Polygon::~Polygon() {}


void
Polygon::writeXML(OutputDevice& out, bool geo) {
    out.openTag(SUMO_TAG_POLY);
    out.writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML(getID()));
    out.writeAttr(SUMO_ATTR_TYPE, StringUtils::escapeXML(getType()));
    out.writeAttr(SUMO_ATTR_COLOR, getColor());
    out.writeAttr(SUMO_ATTR_FILL,  getFill());
    out.writeAttr(SUMO_ATTR_LAYER, getLayer());
    PositionVector shape = getShape();
    if (geo) {
        out.writeAttr(SUMO_ATTR_GEO, true);
        for (int i = 0; i < (int) shape.size(); i++) {
            GeoConvHelper::getFinal().cartesian2geo(shape[i]);
        }
    }
    out.writeAttr(SUMO_ATTR_SHAPE, shape);
    if (getNaviDegree() != Shape::DEFAULT_ANGLE) {
        out.writeAttr(SUMO_ATTR_ANGLE, getNaviDegree());
    }
    if (getImgFile() != Shape::DEFAULT_IMG_FILE) {
        out.writeAttr(SUMO_ATTR_IMGFILE, getImgFile());
    }
    writeParams(out);
    out.closeTag();
}

/****************************************************************************/

