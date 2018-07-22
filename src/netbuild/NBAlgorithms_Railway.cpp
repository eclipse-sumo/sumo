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

#define SHARP_THRESHOLD_SAMEDIR 100
#define SHARP_THRESHOLD 80

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
        int in = 0;
        int out = 0;
        for (NBEdge* e : node->getIncomingEdges()) {
            if ((e->getPermissions() & SVC_RAIL_CLASSES) != 0) {
                in++;
            }
        }
        for (NBEdge* e : node->getOutgoingEdges()) {
            if ((e->getPermissions() & SVC_RAIL_CLASSES) != 0) {
                out++;
            }
        }
        types[std::make_pair(in, out)].insert(node);
    }

    int numBroken = 0;
    int numBrokenA = 0;
    int numBrokenB = 0;
    int numBrokenC = 0;
    int numBrokenD = 0;
    for (auto it : types) {
        int numBrokenType = 0;
        device.openTag("railNodeType");
        int in = it.first.first;
        int out = it.first.second;
        device.writeAttr("in", in);
        device.writeAttr("out", out);
        for (NBNode* n : it.second) {
            device.openTag(SUMO_TAG_NODE);
            device.writeAttr(SUMO_ATTR_ID, n->getID());
            EdgeVector inRail;
            for (NBEdge* e : n->getIncomingEdges()) {
                if ((e->getPermissions() & SVC_RAIL_CLASSES) != 0) {
                    inRail.push_back(e);
                }
            }
            EdgeVector outRail;
            for (NBEdge* e : n->getOutgoingEdges()) {
                if ((e->getPermissions() & SVC_RAIL_CLASSES) != 0) {
                    outRail.push_back(e);
                }
            }
            // check if there is a mismatch between angle and edge direction
            // a) edge pair angle supports driving but both are outgoing 
            // b) edge pair angle supports driving but both are incoming 
            // c) an incoming edge has a sharp angle to all outgoing edges
            // d) an outgoing edge has a sharp angle from all incoming edges

            std::string broken = "";
            if (in < 2 && hasStraightPair(n, outRail)) {
                broken += "a";
                numBrokenA++;
            }
            if (out < 2 && hasStraightPair(n, inRail)) {
                broken += "b";
                numBrokenB++;
            }
            if (out > 0) {
                for (NBEdge* e : inRail) {
                    EdgeVector tmp;
                    tmp.push_back(e);
                    if (allSharp(n, tmp, outRail)) {
                        broken += "c";
                        numBrokenC++;
                        break;
                    }
                }
            }
            if (in > 0) {
                for (NBEdge* e : outRail) {
                    EdgeVector tmp;
                    tmp.push_back(e);
                    if (allSharp(n, inRail, tmp)) {
                        broken += "d";
                        numBrokenD++;
                        break;
                    }
                }
            }

            if (broken.size() > 0) {
                device.writeAttr("broken", broken);
                numBroken++;
                numBrokenType++;
            }
            device.closeTag();
        }
        device.closeTag();
        std::cout << "   " << it.first.first << "," << it.first.second 
            << " count:" << it.second.size() 
            << " broken:" << numBrokenType
            << "\n";

    }
    std::cout << "Found " << numBroken << " broken railway nodes (A=" 
        << numBrokenA
        << " B=" << numBrokenB
        << " C=" << numBrokenC
        << " D=" << numBrokenD
        << ")\n";

    device.close();
}


bool 
NBRailwayTopologyAnalyzer::hasStraightPair(const NBNode* node, const EdgeVector& edges) {
    for (NBEdge* e1 : edges) {
        for (NBEdge* e2 : edges) {
            if (e1 != e2) {
                if (fabs(NBHelpers::normRelAngle(
                                e1->getAngleAtNode(node), 
                                e2->getAngleAtNode(node))) > SHARP_THRESHOLD_SAMEDIR) {
                    /*
                    std::cout << "hasStraightPair n=" << node->getID() 
                        << " e1=" << e1->getID()
                        << " e2=" << e2->getID()
                        << " a1=" << e1->getAngleAtNode(node)
                        << " a2=" << e2->getAngleAtNode(node)
                        << " rel=" << NBHelpers::normRelAngle(e1->getAngleAtNode(node), e2->getAngleAtNode(node))
                        << "\n";
                        */
                    return true;
                }
            }
        }
    }
    return false;
}


bool 
NBRailwayTopologyAnalyzer::allSharp(const NBNode* node, const EdgeVector& in, const EdgeVector& out) {
    for (NBEdge* e1 : in) {
        for (NBEdge* e2 : out) {
            if (e1 != e2) {
                if (fabs(NBHelpers::normRelAngle(
                                e1->getAngleAtNode(node), 
                                e2->getAngleAtNode(node))) < SHARP_THRESHOLD) {
                    return false;
                }
            }
        }
    }
    return true;
}



/****************************************************************************/

