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
// Revision 1.4  2004/12/16 12:23:37  dkrajzew
// first steps towards a better parametrisation of traffic lights
//
// Revision 1.3  2004/08/02 11:56:31  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.2  2003/12/04 13:25:52  dkrajzew
// handling of internal links added; documentation added; some dead code removed
//
//
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
/*    MSNoLogicJunction::InLaneCont *cont =
        new MSNoLogicJunction::InLaneCont();
    cont->reserve(m_pActiveInLanes.size());
    for(LaneCont::iterator i=m_pActiveInLanes.begin();
            i!=m_pActiveInLanes.end(); i++) {
        cont->push_back(*i);
    }*/
/*    MSNoLogicJunction::LaneCont *cont =
        new MSNoLogicJunction::LaneCont();
    std::copy(m_pActiveIncomingLanes.begin(), m_pActiveIncomingLanes.end(),
        std::back_inserter(*cont));*/
/*    cont->reserve(m_pActiveInLanes.size());
    for(LaneCont::iterator i=m_pActiveInLanes.begin();
            i!=m_pActiveInLanes.end(); i++) {
        cont->push_back(*i);
    }*/
    return new GUINoLogicJunction(m_CurrentId, myPosition,
        m_pActiveIncomingLanes, m_pActiveInternalLanes, myShape);
}


MSJunction *
GUIJunctionControlBuilder::buildLogicJunction()
{
    MSJunctionLogic *jtype = getJunctionLogicSecure();
/*    MSRightOfWayJunction::LaneCont internal = getInternalLaneContSecure();
    MSRightOfWayJunction::LaneCont incoming = getIncomingLaneContSecure();*/
    // build the junction
    return new GUIRightOfWayJunction(m_CurrentId, myPosition,
        m_pActiveIncomingLanes, m_pActiveInternalLanes, jtype, myShape);
    myShape.clear();
}

/*
MSJunction *
GUIJunctionControlBuilder::buildTrafficLightJunction()
{
    throw 1;
}
*/


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
