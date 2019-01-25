/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "ROMAEdgeBuilder.h"
#include "ROMAEdge.h"


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

