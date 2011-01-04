/****************************************************************************/
/// @file    ROHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Some helping methods for router
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
/*
SUMOReal
recomputeCosts(SUMOAbstractRouter<ROEdge,ROVehicle> &router,
               const std::vector<const ROEdge*> &edges,
               const ROVehicle * const v, SUMOTime time) throw() {
    SUMOReal costs = 0;
    for (std::vector<const ROEdge*>::const_iterator i=edges.begin(); i!=edges.end(); i++) {
        costs += router.getEffort(v, time + costs, *i);
        if ((*i)->prohibits(v)) {
            return -1;
        }
    }
    return costs;
}
*/

void
recheckForLoops(std::vector<const ROEdge*> &edges) throw() {
    // remove loops at the route's begin
    //  (vehicle makes a turnaround to get into the right direction at an already passed node)
    RONode* start = edges[0]->getFromNode();
    unsigned lastStart = 0;
    for (unsigned i=1; i<edges.size(); i++) {
        if (edges[i]->getFromNode() == start) {
            lastStart = i;
        }
    }
    if (lastStart > 0) {
        edges.erase(edges.begin(), edges.begin() + lastStart - 1);
    }
    // remove loops at the route's end
    //  (vehicle makes a turnaround to get into the right direction at an already passed node)
    RONode* end = edges.back()->getToNode();
    size_t firstEnd = edges.size()-1;
    for (unsigned i=0; i<firstEnd; i++) {
        if (edges[i]->getToNode() == end) {
            firstEnd = i;
        }
    }
    if (firstEnd < edges.size()-1) {
        edges.erase(edges.begin() + firstEnd + 2, edges.end());
    }
    // remove loops within the route
    std::vector<RONode*> nodes;
    for (std::vector<const ROEdge*>::iterator i=edges.begin(); i!=edges.end(); ++i) {
        nodes.push_back((*i)->getFromNode());
    }
    nodes.push_back(edges.back()->getToNode());
    bool changed = false;
    do {
        changed = false;
        for (unsigned int b=0; b<nodes.size()&&!changed; ++b) {
            RONode *bn = nodes[b];
            for (unsigned int e=b+1; e<nodes.size()&&!changed; ++e) {
                if (bn==nodes[e]) {
                    changed = true;
                    nodes.erase(nodes.begin()+b, nodes.begin()+e);
                    edges.erase(edges.begin()+b, edges.begin()+e);
                }
            }
        }
    } while (changed);
    /*
    */
}


}

std::ostream &operator<<(std::ostream &os, const std::vector<const ROEdge*> &ev) {
    bool hadFirst = false;
    for (std::vector<const ROEdge*>::const_iterator j=ev.begin(); j!=ev.end(); j++) {
        if ((*j)->getType() != ROEdge::ET_DISTRICT) {
            if (hadFirst) {
                os << ' ';
            }
            os << (*j)->getID();
            hadFirst = true;
        }
    }
    return os;
}


/****************************************************************************/

