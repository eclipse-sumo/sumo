/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    RODFEdgeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// Interface for building instances of dfrouter-edges
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "RODFEdgeBuilder.h"
#include "RODFEdge.h"


// ===========================================================================
// method definitions
// ===========================================================================
RODFEdgeBuilder::RODFEdgeBuilder() {}


RODFEdgeBuilder::~RODFEdgeBuilder() {}


ROEdge*
RODFEdgeBuilder::buildEdge(const std::string& name, RONode* from, RONode* to, const int priority) {
    return new RODFEdge(name, from, to, getNextIndex(), priority);
}


/****************************************************************************/

