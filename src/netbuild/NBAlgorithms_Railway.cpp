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
#include <utils/iodevices/OutputDevice_String.h>
#include "NBNetBuilder.h"
#include "NBAlgorithms.h"
#include "NBNodeCont.h"
#include "NBEdgeCont.h"
#include "NBNode.h"
#include "NBEdge.h"
#include "NBAlgorithms_Railway.h"

//#define DEBUG_SEQSTOREVERSE
#define DEBUGNODEID  "1085513902"
#define DEBUGNODEID2  "28842974"
#define DEBUGEDGEID  "22820560#0"
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
NBRailwayTopologyAnalyzer::analyzeTopology(NBNetBuilder& nb) {
    getBrokenRailNodes(nb, true);
}


void 
NBRailwayTopologyAnalyzer::repairTopology(NBNetBuilder& nb) {
    reverseEdges(nb);
    NBTurningDirectionsComputer::computeTurnDirections(nb.getNodeCont(), false);
    //std::cout << " numBrokenNodes2=" << brokenNodes.size() << " set2=" << toString(brokenNodes) << "\n";
    addBidiEdgesForBufferStops(nb);
    //NBTurningDirectionsComputer::computeTurnDirections(nb.getNodeCont(), false);
    //std::cout << " numBrokenNodes3=" << brokenNodes.size() << " set3=" << toString(brokenNodes) << "\n";
}


void 
NBRailwayTopologyAnalyzer::getRailEdges(NBNode* node, 
        EdgeVector& inEdges, EdgeVector& outEdges) {
    for (NBEdge* e : node->getIncomingEdges()) {
        if ((e->getPermissions() & SVC_RAIL_CLASSES) != 0) {
            inEdges.push_back(e);
        }
    }
    for (NBEdge* e : node->getOutgoingEdges()) {
        if ((e->getPermissions() & SVC_RAIL_CLASSES) != 0) {
            outEdges.push_back(e);
        }
    }
}



std::set<NBNode*>
NBRailwayTopologyAnalyzer::getBrokenRailNodes(NBNetBuilder& nb, bool verbose) {
    std::set<NBNode*> brokenNodes;; 
    OutputDevice& device = OutputDevice::getDevice(verbose 
            ? OptionsCont::getOptions().getString("railway.topology.output")
            : "/dev/null");
        
    device.writeXMLHeader("railwayTopology", "");
    std::set<NBNode*> railNodes = getRailNodes(nb, verbose);
    std::map<std::pair<int, int>, std::set<NBNode*> > types;
    std::set<NBEdge*, ComparatorIdLess> bidiEdges;
    for (NBNode* node : railNodes) {
        EdgeVector inEdges, outEdges;
        getRailEdges(node, inEdges, outEdges);
        types[std::make_pair((int)inEdges.size(), (int)outEdges.size())].insert(node);
        for (NBEdge* e : outEdges) {
            if (e->isBidiRail() && bidiEdges.count(e->getTurnDestination(true)) == 0) {
                bidiEdges.insert(e);
            }
        }
    }

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
            EdgeVector inRail, outRail;
            getRailEdges(n, inRail, outRail);
            // check if there is a mismatch between angle and edge direction
            // a) edge pair angle supports driving but both are outgoing 
            // b) edge pair angle supports driving but both are incoming 
            // c) an incoming edge has a sharp angle to all outgoing edges
            // d) an outgoing edge has a sharp angle from all incoming edges

            std::string broken = "";
            if (in < 2 && hasStraightPair(n, outRail, outRail)) {
                broken += "a";
                numBrokenA++;
            }
            if (out < 2 && hasStraightPair(n, inRail, inRail)) {
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
            // do not bidi nodes as broken
            if (((in == 1 && out == 1) || (in == 2) && (out == 2))
                    && allBidi(inRail) && allBidi(outRail)) {
                broken = "";
            }

            if (broken.size() > 0) {
                device.writeAttr("broken", broken);
                brokenNodes.insert(n);
                numBrokenType++;
            }
            device.closeTag();
        }
        device.closeTag();
        if (verbose) {
            std::cout << "   " << it.first.first << "," << it.first.second 
                << " count:" << it.second.size() 
                << " broken:" << numBrokenType
                << "\n";
        }

    }
    if (verbose) {
        std::cout << "Found " << brokenNodes.size() << " broken railway nodes (A=" 
            << numBrokenA
            << " B=" << numBrokenB
            << " C=" << numBrokenC
            << " D=" << numBrokenD
            << ")\n";
    }

    for (NBEdge* e : bidiEdges) {
        device.openTag("bidiEdge");
        NBEdge* primary = e;
        NBEdge* secondary = e->getTurnDestination(true);
        if (e->getID()[0] == '-') {
            std::swap(primary, secondary);
        }
        device.writeAttr(SUMO_ATTR_ID, primary->getID());
        device.writeAttr("bidi", secondary->getID());
        device.closeTag();
    }
    if (verbose) {
        std::cout << "Found " << bidiEdges.size() << " bidirectional rail edges\n";
    }

    device.close();
    return brokenNodes;
}


std::set<NBNode*> 
NBRailwayTopologyAnalyzer::getRailNodes(NBNetBuilder& nb, bool verbose) {
    std::set<NBNode*> railNodes;

    NBEdgeCont& ec = nb.getEdgeCont();
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
    if (verbose) {
        std::cout << "Found " << numRailEdges << " railway edges and " << railNodes.size() << " railway nodes (" << railSignals.size() << " signals).\n";
    }
    return railNodes;
}


bool 
NBRailwayTopologyAnalyzer::hasStraightPair(const NBNode* node, const EdgeVector& edges,
        const EdgeVector& edges2) {
#ifdef DEBUG_SEQSTOREVERSE
    //if (node->getID() == DEBUGNODEID2) {
    //    std::cout << " edges=" << toString(edges) << " edges2=" << toString(edges2) << "\n";
    //}
#endif
    for (NBEdge* e1 : edges) {
        for (NBEdge* e2 : edges2) {
            //if (e1->getID() == "195411601#2" && e2->getID() == "93584120#3") {
            //    std::cout 
            //        << " DEBUG normRelA=" << NBHelpers::normRelAngle(
            //                    e1->getAngleAtNode(node), 
            //                    e2->getAngleAtNode(node))
            //        << "\n";
            //}
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


bool 
NBRailwayTopologyAnalyzer::allBidi(const EdgeVector& edges) {
    for (NBEdge* e : edges) {
        if (!e->isBidiRail()) {
            return false;
        }
    }
    return true;
}


void 
NBRailwayTopologyAnalyzer::reverseEdges(NBNetBuilder& nb) {
    std::set<NBNode*> brokenNodes = getBrokenRailNodes(nb);
    // find reversible edge sequences between broken nodes
    // XXX also search backwards to get sequences that start at the network boundary
    std::vector<EdgeVector> seqsToReverse;
    for (NBNode* n : brokenNodes) {
        EdgeVector inRail, outRail;
        getRailEdges(n, inRail, outRail);
        for (NBEdge* start : outRail) {
            EdgeVector tmp;
            tmp.push_back(start);
            if (!allSharp(n, inRail, tmp) 
                    || (inRail.size() == 1 && outRail.size() == 1)) {
                continue;
            }

            //std::cout << " get sequences from " << start->getID() << "\n";
            bool forward = true;
            EdgeVector seq;
            while(forward) {
                seq.push_back(start);
                //std::cout << " seq=" << toString(seq) << "\n";
                NBNode* n2 = start->getToNode();
                EdgeVector inRail2, outRail2;
                getRailEdges(n2, inRail2, outRail2);
                if (brokenNodes.count(n2) != 0) {
                    EdgeVector tmp2;
                    tmp2.push_back(start);
                    if ((outRail2.size() > 0 && allSharp(n2, tmp, outRail2))
                            || hasStraightPair(n2, tmp2, inRail2)) {
                        seqsToReverse.push_back(seq);
                    } else {
#ifdef DEBUG_SEQSTOREVERSE
                        if (n->getID() == DEBUGNODEID) std::cout << " abort at n2=" << n2->getID() << " (not all sharp)\n";
#endif
                    }
                    forward = false;
                } else {
                    if (outRail2.size() == 0) {
                        // stop at network border
                        seqsToReverse.push_back(seq);
                        forward = false;
#ifdef DEBUG_SEQSTOREVERSE
                    if (n->getID() == DEBUGNODEID) std::cout << " abort at n2=" << n2->getID() << " (border)\n";
#endif
                    } else if (outRail2.size() > 1 || inRail2.size() > 1) {
                        // stop at switch
                        forward = false;
#ifdef DEBUG_SEQSTOREVERSE
                    if (n->getID() == DEBUGNODEID) std::cout << " abort at n2=" << n2->getID() << " (switch)\n";
#endif
                    } else {
                        start = outRail2.front();
                    }
                }
            }
        }
    }
    // sort by sequence length
    std::sort(seqsToReverse.begin(), seqsToReverse.end(), 
            [](const EdgeVector& a, const EdgeVector& b){ return a.size() < b.size(); });
    if (seqsToReverse.size() > 0) {
        WRITE_MESSAGE("Found " + toString(seqsToReverse.size()) + " reversible edge sequences between broken rail nodes");
    }
    for (EdgeVector& seq : seqsToReverse) {
        WRITE_MESSAGE("  seq=" + toString(seq));
        for (NBEdge* e : seq) {
            e->reinitNodes(e->getToNode(), e->getFromNode());
            e->setGeometry(e->getGeometry().reverse());
        }
    }
}


void
NBRailwayTopologyAnalyzer::addBidiEdgesForBufferStops(NBNetBuilder& nb) {
    std::set<NBNode*> brokenNodes = getBrokenRailNodes(nb);
    std::set<NBNode*> railNodes = getRailNodes(nb);
    NBEdgeCont& ec = nb.getEdgeCont();
    // find buffer stops and ensure that thay are connect to the network in both directions
    int numBufferStops = 0;
    int numAddedBidiTotal = 0;
    for (NBNode* node : railNodes) {
        if (node->getParameter("buffer_stop", "false") == "true") {
            if (node->getEdges().size() != 1) {
                WRITE_WARNING("Ignoring buffer stop junction '" + node->getID() + "' with " + toString(node->getEdges().size()) + " edges\n");
                continue;
            }
            NBNode* bufferStop = node;
            int numAddedBidi = 0;
            numBufferStops++;
            NBEdge* prev = 0;
            NBEdge* prev2 = 0;
            EdgeVector inRail, outRail;
            getRailEdges(node, inRail, outRail);
            bool addAway = true; // add new edges away from buffer stop
            while (prev == 0 || (inRail.size() + outRail.size()) == 3) {
                NBEdge* e = 0;
                if (prev == 0) {
                    assert(node->getEdges().size() == 1);
                    e = node->getEdges().front();
                    addAway = node == e->getToNode();
                } else {
                    if (addAway) {
                        assert(inRail.size() == 2);
                        e = inRail.front() == prev2 ? inRail.back() : inRail.front();
                    } else {
                        assert(outRail.size() == 2);
                        e = outRail.front() == prev2 ? outRail.back() : outRail.front();
                    }
                }
                e->setLaneSpreadFunction(LANESPREAD_CENTER);
                NBNode* e2From = 0;
                NBNode* e2To = 0;
                if (addAway) {
                    e2From = node;
                    e2To = e->getFromNode();
                    node = e2To;
                } else {
                    e2From = e->getToNode();
                    e2To = node;
                    node = e2From;
                }
                NBEdge* e2 = new NBEdge("-" + e->getID(), e2From, e2To, 
                        e, e->getGeometry().reverse());
                if (!ec.insert(e2)) {
                    delete e2;
                    WRITE_ERROR("Could not add edge '" + e2->getID() + "'.");
                    break;
                }
                prev = e;
                prev2 = e2;
                numAddedBidi++;
                numAddedBidiTotal++;
                inRail.clear();
                outRail.clear();
                getRailEdges(node, inRail, outRail);
            }
            if (numAddedBidi > 0) {
                WRITE_MESSAGE(" added " + toString(numAddedBidi) + " edges between buffer stop junction '" + bufferStop->getID() + "' and junction '" + node->getID() + "'");
            }
        }
    }
    if (numAddedBidiTotal > 0) {
        WRITE_MESSAGE(" added " + toString(numAddedBidiTotal) + " edges to connect " + toString(numBufferStops) + " buffer stops in both directions.");
    }
}

/****************************************************************************/

