/****************************************************************************/
/// @file    RODUAEdgeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// The builder for dua-edges
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <router/ROEdge.h>
#include "RODUAEdgeBuilder.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RODUAEdgeBuilder::RODUAEdgeBuilder(bool useBoundariesOnOverride)
        : myUseBoundariesOnOverride(useBoundariesOnOverride)
{}


RODUAEdgeBuilder::~RODUAEdgeBuilder()
{}


ROEdge *
RODUAEdgeBuilder::buildEdge(const std::string &name)
{
    return new ROEdge(name, getCurrentIndex(), myUseBoundariesOnOverride);
}



/****************************************************************************/

