/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    RODUAEdgeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Yun-Pang Floetteroed
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Interface for building instances of duarouter-edges
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "RODUAEdgeBuilder.h"
#include <router/ROEdge.h>


// ===========================================================================
// method definitions
// ===========================================================================
RODUAEdgeBuilder::RODUAEdgeBuilder() {
}


RODUAEdgeBuilder::~RODUAEdgeBuilder() {}


ROEdge*
RODUAEdgeBuilder::buildEdge(const std::string& name, RONode* from, RONode* to, const int priority) {
    return new ROEdge(name, from, to, getNextIndex(), priority);
}


/****************************************************************************/

