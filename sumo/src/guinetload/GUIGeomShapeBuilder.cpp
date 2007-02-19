/****************************************************************************/
/// @file    GUIGeomShapeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
// missing_desc
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
#include <utils/gui/globjects/GUIPolygon2D.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include "GUIGeomShapeBuilder.h"
#include <utils/common/MsgHandler.h>
#include <microsim/MSNet.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIGeomShapeBuilder::GUIGeomShapeBuilder(MSNet &net,
        GUIGlObjectStorage &idStorage)
        : NLGeomShapeBuilder(net), myIdStorage(idStorage)
{}


GUIGeomShapeBuilder::~GUIGeomShapeBuilder()
{}


void
GUIGeomShapeBuilder::polygonEnd(const Position2DVector &shape)
{
    GUIPolygon2D *p =
        new GUIPolygon2D(myIdStorage, myCurrentLayer, myCurrentName, myCurrentType,
                         myCurrentColor, shape, myFillPoly);
    if (!myShapeContainer.add(myCurrentLayer, p)) {
        MsgHandler::getErrorInstance()->inform("A duplicate of the polygon '" + myCurrentName + "' occured.");
        delete p;
    }
}

void
GUIGeomShapeBuilder::addPoint(const std::string &name,
                              int layer,
                              const std::string &type,
                              const RGBColor &c,
                              SUMOReal x, SUMOReal y,
                              const std::string &lane, SUMOReal posOnLane)
{
    Position2D pos = getPointPosition(x, y, lane, posOnLane);
    GUIPointOfInterest *p = new GUIPointOfInterest(myIdStorage, layer, name, type, pos, c);
    if (!myShapeContainer.add(layer, p)) {
        MsgHandler::getErrorInstance()->inform("A duplicate of the POI '" + name + "' occured.");
        delete p;
    }
}



/****************************************************************************/

