/****************************************************************************/
/// @file    ROHelper.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Some helping methods for router
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <functional>
#include <vector>
#include "ROEdge.h"
#include "ROVehicle.h"


// ===========================================================================
// class definitions
// ===========================================================================


namespace ROHelper {
void
recheckForLoops(ConstROEdgeVector& edges) {
    // XXX check for stops, departLane, departPos, departSpeed, ....

    // removal of edge loops within the route (edge occurs twice)
    std::map<const ROEdge*, int> lastOccurence; // index of the last occurence of this edge
    for (int ii = 0; ii < (int)edges.size(); ++ii) {
        std::map<const ROEdge*, int>::iterator it_pre = lastOccurence.find(edges[ii]);
        if (it_pre != lastOccurence.end()) {
            edges.erase(edges.begin() + it_pre->second, edges.begin() + ii);
            ii = it_pre->second;
        } else {
            lastOccurence[edges[ii]] = ii;
        }
    }

    // remove loops at the route's begin
    //  (vehicle makes a turnaround to get into the right direction at an already passed node)
    const RONode* start = edges[0]->getFromJunction();
    int lastStart = 0;
    for (int i = 1; i < (int)edges.size(); i++) {
        if (edges[i]->getFromJunction() == start) {
            lastStart = i;
        }
    }
    if (lastStart > 0) {
        edges.erase(edges.begin(), edges.begin() + lastStart - 1);
    }
    // remove loops at the route's end
    //  (vehicle makes a turnaround to get into the right direction at an already passed node)
    const RONode* end = edges.back()->getToJunction();
    int firstEnd = (int)edges.size() - 1;
    for (int i = 0; i < firstEnd; i++) {
        if (edges[i]->getToJunction() == end) {
            firstEnd = i;
        }
    }
    if (firstEnd < (int)edges.size() - 1) {
        edges.erase(edges.begin() + firstEnd + 2, edges.end());
    }

    // removal of node loops (node occurs twice) is not done because these may occur legitimately
    /*
    std::vector<RONode*> nodes;
    for (ConstROEdgeVector::iterator i = edges.begin(); i != edges.end(); ++i) {
        nodes.push_back((*i)->getFromJunction());
    }
    nodes.push_back(edges.back()->getToJunction());
    bool changed = false;
    do {
        changed = false;
        for (int b = 0; b < nodes.size() && !changed; ++b) {
            RONode* bn = nodes[b];
            for (int e = b + 1; e < nodes.size() && !changed; ++e) {
                if (bn == nodes[e]) {
                    changed = true;
                    nodes.erase(nodes.begin() + b, nodes.begin() + e);
                    edges.erase(edges.begin() + b, edges.begin() + e);
                }
            }
        }
    } while (changed);
    */
}


}


/****************************************************************************/

