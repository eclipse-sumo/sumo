/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2004-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    ROJTREdgeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Tue, 20 Jan 2004
///
// The builder for jtrrouter-edges
/****************************************************************************/
#include <config.h>

#include "ROJTREdgeBuilder.h"
#include "ROJTREdge.h"


// ===========================================================================
// method definitions
// ===========================================================================
ROJTREdgeBuilder::ROJTREdgeBuilder() {}


ROJTREdgeBuilder::~ROJTREdgeBuilder() {}


ROEdge*
ROJTREdgeBuilder::buildEdge(const std::string& name, RONode* from, RONode* to, const int priority, const std::string& type) {
    return new ROJTREdge(name, from, to, getNextIndex(), priority, type);
}


/****************************************************************************/
