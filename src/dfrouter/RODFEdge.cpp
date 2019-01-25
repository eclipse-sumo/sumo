/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    RODFEdge.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Yun-Pang Floetteroed
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// An edge within the DFROUTER
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <algorithm>
#include <utils/common/MsgHandler.h>
#include "RODFEdge.h"


// ===========================================================================
// method definitions
// ===========================================================================
RODFEdge::RODFEdge(const std::string& id, RONode* from, RONode* to, int index, const int priority)
    : ROEdge(id, from, to, index, priority) {}


RODFEdge::~RODFEdge() {}


void
RODFEdge::setFlows(const std::vector<FlowDef>& flows) {
    myFlows = flows;
}


const std::vector<FlowDef>&
RODFEdge::getFlows() const {
    return myFlows;
}


/****************************************************************************/

