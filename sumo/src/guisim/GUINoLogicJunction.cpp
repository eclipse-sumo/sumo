/****************************************************************************/
/// @file    GUINoLogicJunction.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 1 Jul 2003
/// @version $Id$
///
// A MSNoLogicJunction with a graphical representation
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

#include <string>
#include <vector>
#include <bitset>
#include <microsim/MSNoLogicJunction.h>
#include "GUIJunctionWrapper.h"
#include "GUINoLogicJunction.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GUINoLogicJunction::GUINoLogicJunction(const std::string &id,
                                       const Position2D &position,
                                       LaneCont incoming,
#ifdef HAVE_INTERNAL_LANES
                                       LaneCont internal,
#endif
                                       const Position2DVector &shape)
        : MSNoLogicJunction(id, position,
#ifdef HAVE_INTERNAL_LANES
                            incoming, internal),
#else
                            incoming),
#endif
        myShape(shape)
{}


GUINoLogicJunction::~GUINoLogicJunction()
{}


GUIJunctionWrapper *
GUINoLogicJunction::buildJunctionWrapper(GUIGlObjectStorage &idStorage)
{
    return new GUIJunctionWrapper(idStorage, *this, myShape);
}



/****************************************************************************/

