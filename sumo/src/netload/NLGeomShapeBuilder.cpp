/****************************************************************************/
/// @file    NLGeomShapeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Sep 2005
/// @version $Id$
///
// Factory for building geometry objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NLGeomShapeBuilder::NLGeomShapeBuilder(MSNet &net) throw()
        : myShapeContainer(net.getShapeContainer()) {}


NLGeomShapeBuilder::~NLGeomShapeBuilder() throw() {}


void
NLGeomShapeBuilder::polygonBegin(const std::string &name,
                                 int layer,
                                 const std::string &type,
                                 const RGBColor &c,
                                 bool fill) throw() {
    myCurrentName = name;
    myCurrentType = type;
    myCurrentColor = c;
    myCurrentLayer = layer;
    myFillPoly = fill;
}


void
NLGeomShapeBuilder::polygonEnd(const Position2DVector &shape) throw(InvalidArgument) {
    Polygon2D *p =
        new Polygon2D(myCurrentName, myCurrentType, myCurrentColor, shape, myFillPoly);
    if (!myShapeContainer.add(myCurrentLayer, p)) {
        delete p;
        throw InvalidArgument("A duplicate of the polygon '" + myCurrentName + "' occured.");
    }
}


void
NLGeomShapeBuilder::addPoint(const std::string &name,
                             int layer,
                             const std::string &type,
                             const RGBColor &c,
                             SUMOReal x, SUMOReal y,
                             const std::string &lane, SUMOReal posOnLane) throw(InvalidArgument) {
    Position2D pos = getPointPosition(x, y, lane, posOnLane);
    PointOfInterest *p = new PointOfInterest(name, type, pos, c);
    if (!myShapeContainer.add(layer, p)) {
        delete p;
        throw InvalidArgument("A duplicate of the POI '" + name + "' occured.");
    }
}


Position2D
NLGeomShapeBuilder::getPointPosition(SUMOReal x, SUMOReal y,
                                     const std::string &laneID,
                                     SUMOReal posOnLane) const throw(InvalidArgument) {
    if (x!=INVALID_POSITION&&y!=INVALID_POSITION) {
        return Position2D(x,y);
    }
    MSLane *lane = MSLane::dictionary(laneID);
    if (lane==0) {
        throw InvalidArgument("Lane '" + laneID + "' to place a poi on is not known.");
    }
    if (posOnLane<0) {
        posOnLane = lane->length() + posOnLane;
    }
    return lane->getShape().positionAtLengthPosition(posOnLane);
}



/****************************************************************************/

