//---------------------------------------------------------------------------//
//                        GUIJunctionControlBuilder.cpp -
//  A MSJunctionControlBuilder that builds GUIJunctions instead of MSJunctions
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
// Revision 1.5  2005/05/04 07:56:43  dkrajzew
// level 3 warnings removed
//
// Revision 1.4  2004/12/16 12:23:37  dkrajzew
// first steps towards a better parametrisation of traffic lights
//
// Revision 1.3  2004/08/02 11:56:31  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.2  2003/12/04 13:25:52  dkrajzew
// handling of internal links added; documentation added; some dead code removed
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <algorithm>
#include <netload/NLJunctionControlBuilder.h>
#include <utils/geom/Position2DVector.h>
#include "GUIJunctionControlBuilder.h"
#include <guisim/GUINoLogicJunction.h>
#include <guisim/GUIRightOfWayJunction.h>


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIJunctionControlBuilder::GUIJunctionControlBuilder()
    : NLJunctionControlBuilder()
{
}


GUIJunctionControlBuilder::~GUIJunctionControlBuilder()
{
}


void
GUIJunctionControlBuilder::addJunctionShape(const Position2DVector &shape)
{
    myShape = shape;
}


MSJunction *
GUIJunctionControlBuilder::buildNoLogicJunction()
{
    return new GUINoLogicJunction(m_CurrentId, myPosition,
        m_pActiveIncomingLanes, m_pActiveInternalLanes, myShape);
}


MSJunction *
GUIJunctionControlBuilder::buildLogicJunction()
{
    MSJunctionLogic *jtype = getJunctionLogicSecure();
    // build the junction
    return new GUIRightOfWayJunction(m_CurrentId, myPosition,
        m_pActiveIncomingLanes, m_pActiveInternalLanes, jtype, myShape);
    myShape.clear();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
