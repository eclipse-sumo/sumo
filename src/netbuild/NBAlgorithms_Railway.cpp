/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBAlgorithms_Railway.cpp
/// @author  Jakob Erdmann
/// @author  Melanie Weber
/// @date    29. March 2018
/// @version $Id$
///
// Algorithms for highway on-/off-ramps computation
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <cassert>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NBNetBuilder.h"
#include "NBNodeCont.h"
#include "NBEdgeCont.h"
#include "NBNode.h"
#include "NBEdge.h"
#include "NBAlgorithms_Railway.h"

//#define DEBUG_RAMPS
#define DEBUGNODEID  "260479469"
#define DEBUGCOND(obj) ((obj != 0 && (obj)->getID() == DEBUGNODEID))

// ===========================================================================
// static members
// ===========================================================================

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NBRampsComputer
// ---------------------------------------------------------------------------
void
NBRailwayTopologyAnalyzer::analyzeTopology(NBNetBuilder& nb, OptionsCont& oc) {
    NBEdgeCont& ec = nb.getEdgeCont();
    NBNodeCont& nc = nb.getNodeCont();
    std::set<NBNode*> railNodes;
    int numRailEdges = 0;
    for (auto it = ec.begin(); it != ec.end(); it++) {
        if (isRailway(it->second->getPermissions())) {
            numRailEdges++;
            railNodes.insert(it->second->getFromNode());
            railNodes.insert(it->second->getToNode());

        }
    }
    std::cout << "Found " << numRailEdges << " railway edges and " << railNodes.size() << " railway nodes.\n";
}


/****************************************************************************/

