/***************************************************************************
                          NLGeomShapeBuilder.h
              Builder for geometrical objects
                             -------------------
    project              : SUMO
    begin                : Sep 2005
    copyright            : (C) 2005 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
     const char rcsid[] =
         "$Id$";
}
// $Log$
// Revision 1.1  2005/09/15 12:04:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/09/15 08:59:32  dksumo
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/shapes/Polygon2D.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include "NLGeomShapeBuilder.h"
#include <utils/common/MsgHandler.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NLGeomShapeBuilder::NLGeomShapeBuilder()
    : myShapeContainer(new ShapeContainer)
{
}


NLGeomShapeBuilder::~NLGeomShapeBuilder()
{
}


void
NLGeomShapeBuilder::polygonBegin(const std::string &name,
                                 const std::string &type,
                                 const RGBColor &c)
{
    myCurrentName = name;
    myCurrentType = type;
    myCurrentColor = c;
}


void
NLGeomShapeBuilder::polygonEnd(const Position2DVector &shape)
{
    Polygon2D *p =
        new Polygon2D(myCurrentName, myCurrentType, myCurrentColor, shape);
    if(!myShapeContainer->add(p)) {

        MsgHandler::getErrorInstance()->inform("A duplicate of the polygon '" + myCurrentName + "' occured.");
        delete p;
    }
}

void
NLGeomShapeBuilder::addPoint(const std::string &name,
                             const std::string &type,
                             const RGBColor &c,
                             float x, float y)
{
    PointOfInterest *p = new PointOfInterest(name, type, Position2D(x, y), c);
    if(!myShapeContainer->add(p)) {

        MsgHandler::getErrorInstance()->inform("A duplicate of the POI '" + name + "' occured.");
        delete p;
    }
}


ShapeContainer *
NLGeomShapeBuilder::buildShapeContainer() const
{
    return myShapeContainer;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

