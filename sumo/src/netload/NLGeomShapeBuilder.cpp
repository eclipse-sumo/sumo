/****************************************************************************/
/// @file    NLGeomShapeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Sep 2005
/// @version $Id$
///
// }
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/shapes/Polygon2D.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include "NLGeomShapeBuilder.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
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
NLGeomShapeBuilder::NLGeomShapeBuilder(MSNet &net)
        : myShapeContainer(net.getShapeContainer())
{}


NLGeomShapeBuilder::~NLGeomShapeBuilder()
{}


void
NLGeomShapeBuilder::polygonBegin(const std::string &name,
                                 int layer,
                                 const std::string &type,
                                 const RGBColor &c,
                                 bool fill)
{
    myCurrentName = name;
    myCurrentType = type;
    myCurrentColor = c;
    myCurrentLayer = layer;
    myFillPoly = fill;
}


void
NLGeomShapeBuilder::polygonEnd(const Position2DVector &shape)
{
    Polygon2D *p =
        new Polygon2D(myCurrentName, myCurrentType, myCurrentColor, shape, myFillPoly);
    if (!myShapeContainer.add(myCurrentLayer, p)) {
        MsgHandler::getErrorInstance()->inform("A duplicate of the polygon '" + myCurrentName + "' occured.");
        delete p;
    }
}


void
NLGeomShapeBuilder::addPoint(const std::string &name,
                             int layer,
                             const std::string &type,
                             const RGBColor &c,
                             SUMOReal x, SUMOReal y,
                             const std::string &lane, SUMOReal posOnLane)
{
    Position2D pos = getPointPosition(x, y, lane, posOnLane);
    PointOfInterest *p = new PointOfInterest(name, type, pos, c);
    if (!myShapeContainer.add(layer, p)) {
        MsgHandler::getErrorInstance()->inform("A duplicate of the POI '" + name + "' occured.");
        delete p;
    }
}


Position2D
NLGeomShapeBuilder::getPointPosition(SUMOReal x, SUMOReal y,
                                     const std::string &laneID,
                                     SUMOReal posOnLane) const
{
    if (x!=INVALID_POSITION&&y!=INVALID_POSITION) {
        return Position2D(x,y);
    }
    MSLane *lane = MSLane::dictionary(laneID);
    if (lane==0) {
        throw EmptyData();
    }
    if (posOnLane<0) {
        posOnLane = lane->length() + posOnLane;
    }
    return lane->getShape().positionAtLengthPosition(posOnLane);
}



/****************************************************************************/

