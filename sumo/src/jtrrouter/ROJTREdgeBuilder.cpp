/****************************************************************************/
/// @file    ROJTREdgeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// The builder for jp-edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include "ROJTREdgeBuilder.h"
#include "ROJTREdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROJTREdgeBuilder::ROJTREdgeBuilder() throw() {}


ROJTREdgeBuilder::~ROJTREdgeBuilder() throw() {}


ROEdge *
ROJTREdgeBuilder::buildEdge(const std::string &name, RONode *from, RONode *to) throw() {
    return new ROJTREdge(name, from, to, getNextIndex());
}


/****************************************************************************/

