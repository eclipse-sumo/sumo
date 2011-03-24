/****************************************************************************/
/// @file    NLGeomShapeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Sep 2005
/// @version $Id$
///
// Factory for building geometry objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/common/RGBColor.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/shapes/Polygon2D.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include "NLGeomShapeBuilder.h"
#include <utils/common/UtilExceptions.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NLGeomShapeBuilder::NLGeomShapeBuilder(MSNet &net)
        : myShapeContainer(net.getShapeContainer()) {}


NLGeomShapeBuilder::~NLGeomShapeBuilder() {}


void
NLGeomShapeBuilder::addPolygon(const std::string &name,
                                 int layer,
                                 const std::string &type,
                                 const RGBColor &c,
                                 bool fill, const Position2DVector &shape) {
    if (!myShapeContainer.addPolygon(name, layer, type, c, fill, shape)) {
        throw InvalidArgument("A duplicate of the polygon '" + name + "' occured.");
    }
}


void
NLGeomShapeBuilder::addPoint(const std::string &name,
                             int layer,
                             const std::string &type,
                             const RGBColor &c,
                             SUMOReal x, SUMOReal y,
                             const std::string &lane, SUMOReal posOnLane) {
    Position2D pos = getPointPosition(x, y, lane, posOnLane);
    if (!myShapeContainer.addPoI(name, layer, type, c, pos)) {
        throw InvalidArgument("A duplicate of the POI '" + name + "' occured.");
    }
}


Position2D
NLGeomShapeBuilder::getPointPosition(SUMOReal x, SUMOReal y,
                                     const std::string &laneID,
                                     SUMOReal posOnLane) const {
    if (x!=INVALID_POSITION&&y!=INVALID_POSITION) {
        return Position2D(x,y);
    }
    MSLane *lane = MSLane::dictionary(laneID);
    if (lane==0) {
        throw InvalidArgument("Lane '" + laneID + "' to place a poi on is not known.");
    }
    if (posOnLane<0) {
        posOnLane = lane->getLength() + posOnLane;
    }
    return lane->getShape().positionAtLengthPosition(posOnLane);
}



/****************************************************************************/

