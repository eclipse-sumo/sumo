/****************************************************************************/
/// @file    NBAlgorithms.cpp
/// @author  Daniel Krajzewicz
/// @date    02. March 2012
/// @version $Id$
///
// Algorithms for network computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <sstream>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include "NBEdge.h"
#include "NBNodeCont.h"
#include "NBTypeCont.h"
#include "NBNode.h"
#include "NBAlgorithms.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NBTurningDirectionsComputer
// ---------------------------------------------------------------------------
void 
NBTurningDirectionsComputer::computeTurnDirections(NBNodeCont &nc) {
    for(std::map<std::string, NBNode*>::const_iterator i=nc.begin(); i!=nc.end(); ++i) {
        computeTurnDirectionsForNode(i->second);
    }
}

void 
NBTurningDirectionsComputer::computeTurnDirectionsForNode(NBNode *node) {
    const std::vector<NBEdge*> &incoming = node->getIncomingEdges();
    const std::vector<NBEdge*> &outgoing = node->getOutgoingEdges();
    std::vector<Combination> combinations;
    for(std::vector<NBEdge*>::const_iterator j=outgoing.begin(); j!=outgoing.end(); ++j) {
        NBEdge *outedge = *j;
        for(std::vector<NBEdge*>::const_iterator k=incoming.begin(); k!=incoming.end(); ++k) {
            NBEdge* e = *k;
            if (e->getConnections().size()!=0 && !e->isConnectedTo(outedge)) {
                // has connections, but not to outedge; outedge will not be the turn direction
                //
                // @todo: this seems to be needed due to legacy issues; actually, we could regard
                //  such pairs, too, and it probably would increase the accuracy. But there is
                //  no mechanism implemented, yet, which would avoid adding them as turnarounds though
                //  no connection is specified.
                continue;
            }

            // @todo: check whether NBHelpers::relAngle is properly defined and whether it should really be used, here
            SUMOReal angle = fabs(NBHelpers::relAngle(e->getAngleAtNode(node), outedge->getAngleAtNode(node)));
            if(angle<160) {
                continue;
            }
            if(e->getFromNode()==outedge->getToNode()) {
                // they connect the same nodes; should be the turnaround direction
                // we'll assign a maximum number
                //
                // @todo: indeed, we have observed some pathological intersections
                //  see "294831560" in OSM/adlershof. Here, several edges are connecting
                //  same nodes. We have to do the angle check before...
                //
                // @todo: and well, there are some other as well, see plain import
                //  of delphi_muenchen (elmar), intersection "59534191". Not that it would
                //  be realistic in any means; we will warn, here.
                angle += 360;
            }
            Combination c;
            c.from = e;
            c.to = outedge;
            c.angle = angle;
            combinations.push_back(c);
        }
    }
    // sort combinations so that the ones with the highest angle are at the begin
    std::sort(combinations.begin(), combinations.end(), combination_by_angle_sorter());
    std::set<NBEdge*> seen;
    bool haveWarned = false;
    for(std::vector<Combination>::const_iterator j=combinations.begin(); j!=combinations.end(); ++j) {
        if(seen.find((*j).from)!=seen.end() || seen.find((*j).to)!=seen.end() ) {
            // do not regard already set edges
            if((*j).angle>360&&!haveWarned) {
                WRITE_WARNING("Ambiguity in turnarounds computation at node '" + node->getID() +"'.");
                haveWarned = true;
            }
            continue;
        }
        // mark as seen
        seen.insert((*j).from);
        seen.insert((*j).to);
        // set turnaround information
        (*j).from->setTurningDestination((*j).to);
    }
}


// ---------------------------------------------------------------------------
// NBNodesEdgesSorter
// ---------------------------------------------------------------------------
void 
NBNodesEdgesSorter::sortNodesEdges(NBNodeCont &nc, bool leftHand) {
    for(std::map<std::string, NBNode*>::const_iterator i=nc.begin(); i!=nc.end(); ++i) {
        NBNode *n = (*i).second;
        if (n->myAllEdges.size() == 0) {
            continue;
        }
        std::vector<NBEdge*> &allEdges = (*i).second->myAllEdges;
        std::vector<NBEdge*> &incoming = (*i).second->myIncomingEdges;
        std::vector<NBEdge*> &outgoing = (*i).second->myOutgoingEdges;
        // sort the edges
        std::sort(allEdges.begin(), allEdges.end(), edge_by_junction_angle_sorter(n));
        std::sort(incoming.begin(), incoming.end(), edge_by_junction_angle_sorter(n));
        std::sort(outgoing.begin(), outgoing.end(), edge_by_junction_angle_sorter(n));
        std::vector<NBEdge*>::iterator j;
        for (j = allEdges.begin(); j != allEdges.end() - 1 && j != allEdges.end(); ++j) {
            swapWhenReversed(n, leftHand, j, j + 1);
        }
        if (allEdges.size() > 1 && j != allEdges.end()) {
            swapWhenReversed(n, leftHand, allEdges.end() - 1, allEdges.begin());
        }
    }
}


void
NBNodesEdgesSorter::swapWhenReversed(const NBNode * const n, bool leftHand,
                                     const std::vector<NBEdge*>::iterator& i1,
                                     const std::vector<NBEdge*>::iterator& i2) {
    NBEdge* e1 = *i1;
    NBEdge* e2 = *i2;
    if (leftHand) {
        // @todo: check this; shouldn't it be "swap(*e1, *e2)"?
        std::swap(e1, e2);
    }
    // @todo: The difference between "isTurningDirectionAt" and "isTurnaround"
    //  is not nice. Maybe we could get rid of it if we would always mark edges
    //  as turnarounds, even if they do not have to be added, as mentioned in
    //  notes on NBTurningDirectionsComputer::computeTurnDirectionsForNode
    if (e2->getToNode() == n && e2->isTurningDirectionAt(n, e1)) {
        std::swap(*i1, *i2);
    }
}


// ---------------------------------------------------------------------------
// NBNodeTypeComputer
// ---------------------------------------------------------------------------
NBNodeTypeComputer::JunctionTypesMatrix::JunctionTypesMatrix() {
    myMap['t'] = NODETYPE_TRAFFIC_LIGHT;
    myMap['x'] = NODETYPE_NOJUNCTION;
    myMap['p'] = NODETYPE_PRIORITY_JUNCTION;
    myMap['r'] = NODETYPE_RIGHT_BEFORE_LEFT;
    myRanges.push_back(std::pair<SUMOReal, SUMOReal>((SUMOReal)(0. / 3.6), (SUMOReal)(10. / 3.6)));
    myRanges.push_back(std::pair<SUMOReal, SUMOReal>((SUMOReal)(10. / 3.6), (SUMOReal)(30. / 3.6)));
    myRanges.push_back(std::pair<SUMOReal, SUMOReal>((SUMOReal)(30. / 3.6), (SUMOReal)(50. / 3.6)));
    myRanges.push_back(std::pair<SUMOReal, SUMOReal>((SUMOReal)(50. / 3.6), (SUMOReal)(70. / 3.6)));
    myRanges.push_back(std::pair<SUMOReal, SUMOReal>((SUMOReal)(70. / 3.6), (SUMOReal)(100. / 3.6)));
    myRanges.push_back(std::pair<SUMOReal, SUMOReal>((SUMOReal)(100. / 3.6), (SUMOReal)(999999. / 3.6)));
    myValues.push_back("rppppp"); // 000 - 010
    myValues.push_back(" rpppp"); // 010 - 030
    myValues.push_back("  rppp"); // 030 - 050
    myValues.push_back("   ppp"); // 050 - 070
    myValues.push_back("    pp"); // 070 - 100
    myValues.push_back("     p"); // 100 -
}


NBNodeTypeComputer::JunctionTypesMatrix::~JunctionTypesMatrix() {}


SumoXMLNodeType
NBNodeTypeComputer::JunctionTypesMatrix::getType(SUMOReal speed1, SUMOReal speed2) const {
    std::vector<std::pair<SUMOReal, SUMOReal> >::const_iterator p1 = find_if(myRanges.begin(), myRanges.end(), range_finder(speed1));
    std::vector<std::pair<SUMOReal, SUMOReal> >::const_iterator p2 = find_if(myRanges.begin(), myRanges.end(), range_finder(speed2));
    char name = getNameAt(distance(myRanges.begin(), p1), distance(myRanges.begin(), p2));
    return myMap.find(name)->second;
}


char
NBNodeTypeComputer::JunctionTypesMatrix::getNameAt(size_t pos1, size_t pos2) const {
    std::string str = myValues[pos1];
    if (str[pos2] == ' ') {
        return getNameAt(pos2, pos1);
    }
    return str[pos2];
}


void 
NBNodeTypeComputer::computeNodeTypes(NBNodeCont &nc) {
    JunctionTypesMatrix m;
    for(std::map<std::string, NBNode*>::const_iterator i=nc.begin(); i!=nc.end(); ++i) {
        NBNode *n = (*i).second;
        // the type may already be set from the data
        if (n->myType != NODETYPE_UNKNOWN) {
            continue;
        }
        // check whether the junction is not a real junction
        if (n->myIncomingEdges.size() == 1) {
            n->myType = NODETYPE_PRIORITY_JUNCTION;
            continue;
        }
        // !!!
        if (n->isSimpleContinuation()) {
            n->myType = NODETYPE_PRIORITY_JUNCTION;
            continue;
        }
        // choose the uppermost type as default
        SumoXMLNodeType type = NODETYPE_RIGHT_BEFORE_LEFT;
        // determine the type
        for (EdgeVector::const_iterator i = n->myIncomingEdges.begin(); i != n->myIncomingEdges.end(); i++) {
            for (EdgeVector::const_iterator j = i + 1; j != n->myIncomingEdges.end(); j++) {
                bool isOpposite = false;
                // @todo "getOppositeIncoming" should probably be refactored into something the edge
                //  knows
                if (n->getOppositeIncoming(*j) == *i && n->myIncomingEdges.size() > 2) {
                    isOpposite = true;
                }

                // This usage of defaults is not very well, still we do not have any
                //  methods for the conversion of foreign, sometimes not supplied
                //  road types into an own format
                SumoXMLNodeType tmptype = type;
                if (!isOpposite) {
                    tmptype = m.getType((*i)->getSpeed(), (*j)->getSpeed());
                    if (tmptype < type && tmptype != NODETYPE_UNKNOWN && tmptype != NODETYPE_NOJUNCTION) {
                        type = tmptype;
                    }
                }
            }
        }
        n->myType = type;
    }
}


/****************************************************************************/

