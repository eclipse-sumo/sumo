/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SUMOPolygon.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Jun 2004
/// @version $Id$
///
// A 2D-polygon
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
#include "SUMOPolygon.h"


// ===========================================================================
// member definitions
// ===========================================================================
SUMOPolygon::SUMOPolygon(const std::string& id, const std::string& type,
                         const RGBColor& color, const PositionVector& shape, bool geo, bool fill,
                         double layer, double angle, const std::string& imgFile) :
    Shape(id, type, color, layer, angle, imgFile),
    myShape(shape),
    myGEO(geo),
    myFill(fill) {
}


SUMOPolygon::~SUMOPolygon() {}


void
SUMOPolygon::writeXML(OutputDevice& out, bool geo) {
    out.openTag(SUMO_TAG_POLY);
    out.writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML(getID()));
    if (getType().size() > 0) {
        out.writeAttr(SUMO_ATTR_TYPE, StringUtils::escapeXML(getType()));
    }
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
