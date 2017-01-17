/****************************************************************************/
/// @file    ROMAEdgeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Interface for building instances of duarouter-edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include "ROMAEdgeBuilder.h"
#include "ROMAEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROMAEdgeBuilder::ROMAEdgeBuilder() {
}


ROMAEdgeBuilder::~ROMAEdgeBuilder() {}


ROEdge*
ROMAEdgeBuilder::buildEdge(const std::string& name, RONode* from, RONode* to, const int priority) {
    return new ROMAEdge(name, from, to, getNextIndex(), priority);
}


/****************************************************************************/

