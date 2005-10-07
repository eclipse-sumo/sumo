//---------------------------------------------------------------------------//
//                        Polygon2D.h -
//  A 2d-polygon
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jun 2004
//  copyright            : (C) 2004 by DLR/IVF http://ivf.dlr.de/
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.2  2005/10/07 11:47:18  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.1  2005/09/15 12:22:04  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2005/09/09 12:56:51  dksumo
// shape handling added
//
// Revision 1.2  2005/06/14 11:29:50  dksumo
// documentation added
//
// Revision 1.1  2004/10/22 12:50:44  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.1  2004/06/17 13:08:36  dkrajzew
// Polygon visualisation added
//
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

#include "Polygon2D.h"
#include <utils/gfx/RGBColor.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member definitions
 * ======================================================================= */
//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////


Polygon2D::Polygon2D(const std::string name, const std::string type,
                     const RGBColor &color, const Position2DVector &Pos)
        : myName(name),  myType(type), myColor(color), myPos(Pos)
{
}


Polygon2D::~Polygon2D()
{
}


//////////////////////////////////////////////////////////////////////
// Definitions of the Methods
//////////////////////////////////////////////////////////////////////

/// return the name of the Polygon
const std::string &
Polygon2D::getName(void) const
{
    return myName;
}

/// return the typ of the Polygon
const std::string &
Polygon2D::getType(void) const
{
    return myType;
}

/// return the Color of the polygon
const RGBColor &
Polygon2D::getColor(void) const
{
    return myColor;
}

/// return the Positions Vector of the Polygon
const Position2DVector &
Polygon2D::getPosition2DVector(void) const
{
    return myPos;
}

/// add the Position2DVector to the polygon
void
Polygon2D::addPolyPosition(Position2DVector &myNewPos)
{
     myPos = myNewPos;

}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


