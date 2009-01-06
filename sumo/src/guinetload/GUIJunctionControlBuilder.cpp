/****************************************************************************/
/// @file    GUIJunctionControlBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 1 Jul 2003
/// @version $Id$
///
// Builder of guisim-junctions and tls
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
        OptionsCont &oc) throw()
        : NLJunctionControlBuilder(net, oc)
{}


GUIJunctionControlBuilder::~GUIJunctionControlBuilder() throw()
{}


void
GUIJunctionControlBuilder::addJunctionShape(const Position2DVector &shape) throw()
{
    myShape = shape;
}


MSJunction *
GUIJunctionControlBuilder::buildNoLogicJunction() throw()
{
    return new GUINoLogicJunction(myActiveID, myPosition, myActiveIncomingLanes,
#ifdef HAVE_INTERNAL_LANES
                                  myActiveInternalLanes,
#endif
                                  myShape);
    myShape.clear();
}


MSJunction *
GUIJunctionControlBuilder::buildLogicJunction() throw(InvalidArgument)
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

