/****************************************************************************/
/// @file    GUIJunctionControlBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 1 Jul 2003
/// @version $Id$
///
// A MSJunctionControlBuilder that builds GUIJunctions instead of MSJunctions
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

#include <algorithm>
#include <netload/NLJunctionControlBuilder.h>
#include <utils/geom/Position2DVector.h>
#include "GUIJunctionControlBuilder.h"
#include <guisim/GUINoLogicJunction.h>
#include <guisim/GUIRightOfWayJunction.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GUIJunctionControlBuilder::GUIJunctionControlBuilder(MSNet &net,
        OptionsCont &oc)
        : NLJunctionControlBuilder(net, oc)
{}


GUIJunctionControlBuilder::~GUIJunctionControlBuilder()
{}


void
GUIJunctionControlBuilder::addJunctionShape(const Position2DVector &shape)
{
    myShape = shape;
}


MSJunction *
GUIJunctionControlBuilder::buildNoLogicJunction()
{
    return new GUINoLogicJunction(myActiveID, myPosition, myActiveIncomingLanes,
#ifdef HAVE_INTERNAL_LANES
                                  myActiveInternalLanes,
#endif
                                  myShape);
    myShape.clear();
}


MSJunction *
GUIJunctionControlBuilder::buildLogicJunction()
{
    MSJunctionLogic *jtype = getJunctionLogicSecure();
    // build the junction
    return new GUIRightOfWayJunction(myActiveID, myPosition, myActiveIncomingLanes,
#ifdef HAVE_INTERNAL_LANES
                                     myActiveInternalLanes,
#endif
                                     jtype, myShape);
    myShape.clear();
}



/****************************************************************************/

