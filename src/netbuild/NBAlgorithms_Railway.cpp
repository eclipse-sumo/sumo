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
#include <utils/iodevices/OutputDevice.h>
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
    OutputDevice& device = OutputDevice::getDevice(oc.getString("railway.topology.output"));
    device.writeXMLHeader("railwayTopology", "");
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
    std::set<NBNode*> railSignals;
    for (NBNode* node : railNodes) {
        if (node->getType() == NODETYPE_RAIL_SIGNAL) {
            railSignals.insert(node);
        }
    }
    std::cout << "Found " << numRailEdges << " railway edges and " << railNodes.size() << " railway nodes (" << railSignals.size() << " signals).\n";


    std::map<std::pair<int, int>, std::set<NBNode*> > types;
    for (NBNode* node : railNodes) {
        const int in = (int)node->getIncomingEdges().size();
        const int out = (int)node->getOutgoingEdges().size();
        types[std::make_pair(in, out)].insert(node);
    }
    for (auto it : types) {
        std::cout << "   " << it.first.first << "," << it.first.second << " count:" << it.second.size() << "\n";
        device.openTag("railNodeType");
        device.writeAttr("in", it.first.first);
        device.writeAttr("out", it.first.second);
        for (NBNode* n : it.second) {
            device.openTag(SUMO_TAG_NODE);
            device.writeAttr(SUMO_ATTR_ID, n->getID());
            device.closeTag();
        }
        device.closeTag();

    }

    device.close();
}


/****************************************************************************/

