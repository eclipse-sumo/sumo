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
// Revision 1.6  2006/01/31 10:53:44  dkrajzew
// pois may be now placed on lane positions
//
// Revision 1.5  2005/11/09 06:32:46  dkrajzew
// problems on loading geometry items patched
//
// Revision 1.4  2005/10/10 12:11:33  dkrajzew
// debugging
//
// Revision 1.3  2005/10/07 11:41:49  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.2  2005/09/23 06:04:11  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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
NLGeomShapeBuilder::NLGeomShapeBuilder(MSNet &net)
    : myShapeContainer(net.getShapeContainer())
{
}


NLGeomShapeBuilder::~NLGeomShapeBuilder()
{
//    delete myShapeContainer;
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
    if(!myShapeContainer.add(p)) {

        MsgHandler::getErrorInstance()->inform("A duplicate of the polygon '" + myCurrentName + "' occured.");
        delete p;
    }
}

void
NLGeomShapeBuilder::addPoint(const std::string &name,
                             const std::string &type,
                             const RGBColor &c,
                             SUMOReal x, SUMOReal y,
							 const std::string &lane, SUMOReal posOnLane)
{
    PointOfInterest *p =
		new PointOfInterest(name, type, getPointPosition(x, y, lane, posOnLane), c);
    if(!myShapeContainer.add(p)) {

        MsgHandler::getErrorInstance()->inform("A duplicate of the POI '" + name + "' occured.");
        delete p;
    }
}


Position2D
NLGeomShapeBuilder::getPointPosition(SUMOReal x, SUMOReal y,
									 const std::string &laneID,
									 SUMOReal posOnLane) const
{
	if(x!=INVALID_POSITION&&y!=INVALID_POSITION) {
		return Position2D(x,y);
	}
	MSLane *lane = MSLane::dictionary(laneID);
	if(lane==0) {
		throw EmptyData();
	}
	if(posOnLane<0) {
		posOnLane = lane->length() + posOnLane;
	}
	return lane->getShape().positionAtLengthPosition(posOnLane);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

