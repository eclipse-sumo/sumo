/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ROMAEdge.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christian Roessel
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A basic edge for routing applications
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "ROMAEdge.h"


// ===========================================================================
// method definitions
// ===========================================================================
ROMAEdge::ROMAEdge(const std::string& id, RONode* from, RONode* to, int index, const int priority)
    : ROEdge(id, from, to, index, priority) {
}


ROMAEdge::~ROMAEdge() {
}


void
ROMAEdge::addSuccessor(ROEdge* s, ROEdge* via, std::string dir) {
    ROEdge::addSuccessor(s, via, dir);
    if (dir == "l" || dir == "L") {
        myLeftTurns.insert(static_cast<ROMAEdge*>(s));
    }
}


/****************************************************************************/

