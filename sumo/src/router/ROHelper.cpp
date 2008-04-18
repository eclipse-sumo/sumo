/****************************************************************************/
/// @file    ROHelper.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: ROHelper.h 4669 2007-11-08 10:18:35Z behrisch $
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


namespace ROHelper {

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


bool 
equal(const std::vector<const ROEdge*> &edges1,
      const std::vector<const ROEdge*> &edges2) throw()
{
    if (edges1.size()!=edges2.size()) {
        return false;
    }
    for (size_t i=0; i<edges1.size(); i++) {
        if (edges1[i]!=edges2[i]) {
            return false;
        }
    }
    return true;
}


bool 
isTurnaround(const ROEdge *e1, const ROEdge *e2) throw()
{
    return e1->getFromNode()==e2->getToNode() && e1->getToNode()==e2->getFromNode();
}


void 
recheckForLoops(std::vector<const ROEdge*> &edges) throw()
{
    // forward
    {
        int lastReversed = 0;
        bool found = false;
        for (int i=0; i<(int) edges.size()/2+1; i++) {
            for (int j=i+1; j<(int) edges.size()/2+1; j++) {
                if (isTurnaround(edges[i], edges[j])&&lastReversed<j) {
                    lastReversed = j;
                    found = true;
                }
            }
        }
        if (found) {
            edges.erase(edges.begin(), edges.begin()+lastReversed-1);
        }
    }
    //
    if (edges.size()<2) {
        return;
    }
    // backward
    {
        int lastReversed = (int) edges.size()-1;
        bool found = false;
        for (int i=(int) edges.size()-1; i>=0; i--) {
            for (int j=i-1; j>=0; j--) {
                if (isTurnaround(edges[i], edges[j])&&lastReversed>j) {
                    lastReversed = j;
                    found = true;
                }
            }
        }
        if (found) {
            edges.erase(edges.begin()+lastReversed, edges.end());
        }
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

