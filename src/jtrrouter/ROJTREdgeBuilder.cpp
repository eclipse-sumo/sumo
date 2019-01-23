/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ROJTREdgeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// The builder for jtrrouter-edges
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "ROJTREdgeBuilder.h"
#include "ROJTREdge.h"


// ===========================================================================
// method definitions
// ===========================================================================
ROJTREdgeBuilder::ROJTREdgeBuilder() {}


ROJTREdgeBuilder::~ROJTREdgeBuilder() {}


ROEdge*
ROJTREdgeBuilder::buildEdge(const std::string& name, RONode* from, RONode* to, const int priority) {
    return new ROJTREdge(name, from, to, getNextIndex(), priority);
}


/****************************************************************************/

