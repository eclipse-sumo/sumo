/****************************************************************************/
/// @file    ROHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Some helping methods for router
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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


namespace ROHelper
{

SUMOReal
recomputeCosts(const std::vector<const ROEdge*> &edges,
               const ROVehicle * const v, SUMOTime time) throw()
{
    SUMOReal costs = 0;
    for (std::vector<const ROEdge*>::const_iterator i=edges.begin(); i!=edges.end(); i++) {
        costs += (*i)->getCost(v, time);
        time += ((SUMOTime)(*i)->getDuration(v, time));
        if ((*i)->prohibits(v)) {
            return -1;
        }
    }
    return costs;
}


void
recheckForLoops(std::vector<const ROEdge*> &edges) throw()
{
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
}


}

std::ostream &operator<<(std::ostream &os, const std::vector<const ROEdge*> &ev)
{
    for (std::vector<const ROEdge*>::const_iterator j=ev.begin(); j!=ev.end(); j++) {
        if (j!=ev.begin()) {
            os << ' ';
        }
        os << (*j)->getID();
    }
    return os;
}


/****************************************************************************/

