//---------------------------------------------------------------------------//
//                        GUIRightOfWayJunction.cpp -
//  A MSRightOfWayJunction with a graphical representation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 1 Jul 2003
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.3  2003/12/04 13:38:16  dkrajzew
// usage of internal links added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <microsim/MSRightOfWayJunction.h>
#include <bitset>
#include <vector>
#include <string>
#include "GUIJunctionWrapper.h"
#include "GUIRightOfWayJunction.h"


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIRightOfWayJunction::GUIRightOfWayJunction(const std::string &id,
                                             double x, double y,
                                             LaneCont incoming,
                                             LaneCont internal,
                                             MSJunctionLogic* logic,
                                             const Position2DVector &shape)
    : MSRightOfWayJunction(id, x, y, incoming, internal, logic),
    myShape(shape)
{
}


GUIRightOfWayJunction::~GUIRightOfWayJunction()
{
}


GUIJunctionWrapper *
GUIRightOfWayJunction::buildJunctionWrapper(GUIGlObjectStorage &idStorage)
{
    return new GUIJunctionWrapper(idStorage, *this, myShape);
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
