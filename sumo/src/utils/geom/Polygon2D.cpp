//---------------------------------------------------------------------------//
//                        Polygon2D.h -
//  A 2d-polygon
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jun 2004
//  copyright            : (C) 2004 by Danilo Boyom
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : ---
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
// Revision 1.1  2004/06/17 13:08:36  dkrajzew
// Polygon visualisation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "Polygon2D.h"
#include <utils/gfx/RGBColor.h>


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
                     const RGBColor color, const Position2DVector &Pos)
        : myName(name),  myType(type), myColor(color), myPos(Pos)
{
}

Polygon2D::Polygon2D(const std::string name, const std::string type, 
                     const RGBColor color)
        : myName(name), myType(type), myColor(color)
{
}


Polygon2D::~Polygon2D()
{
    myPos.clear();
     
}


//////////////////////////////////////////////////////////////////////
// Definitions of the Methods
//////////////////////////////////////////////////////////////////////

/// return the name of the Polygon
std::string
Polygon2D::getName(void)
{
    return myName;
}

/// return the typ of the Polygon
std::string 
Polygon2D::getType(void)
{
    return myType;
}

/// return the Color of the polygon
RGBColor 
Polygon2D::getColor(void)
{
    return myColor;
}

/// return the Positions Vector of the Polygon
Position2DVector 
Polygon2D::getPosition2DVector(void)
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


