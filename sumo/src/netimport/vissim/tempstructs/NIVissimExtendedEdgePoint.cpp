//---------------------------------------------------------------------------//
//                        NIVissimExtendedEdgePoint.cpp -  ccc
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
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
// Revision 1.3  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <utils/common/IntVector.h>
#include "NIVissimExtendedEdgePoint.h"
#include "NIVissimEdge.h"

NIVissimExtendedEdgePoint::NIVissimExtendedEdgePoint(
        int edgeid, IntVector &lanes, double position,
        IntVector &assignedVehicles)
    : myEdgeID(edgeid), myLanes(lanes), myPosition(position),
    myAssignedVehicles(assignedVehicles)
{
}


NIVissimExtendedEdgePoint::~NIVissimExtendedEdgePoint()
{
}


int
NIVissimExtendedEdgePoint::getEdgeID() const
{
    return myEdgeID;
}


double
NIVissimExtendedEdgePoint::getPosition() const
{
    return myPosition;
}


Position2D
NIVissimExtendedEdgePoint::getGeomPosition() const
{
    return
        NIVissimAbstractEdge::dictionary(myEdgeID)->getGeomPosition(myPosition);
}


const IntVector &
NIVissimExtendedEdgePoint::getLanes() const
{
    return myLanes;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


