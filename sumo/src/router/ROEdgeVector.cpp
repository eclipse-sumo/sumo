/****************************************************************************/
/// @file    ROEdgeVector.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A vector of edges (a route)
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

#include <vector>
#include <string>
#include <deque>
#include <iostream>
#include <utils/common/UtilExceptions.h>
#include "ROEdge.h"
#include "ROEdgeVector.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method defintions
// ===========================================================================
ROEdgeVector::ROEdgeVector()
{}


ROEdgeVector::ROEdgeVector(size_t toReserve)
{
    _edges.reserve(toReserve);
}


ROEdgeVector::ROEdgeVector(const EdgeVector &edges)
        : _edges(edges)
{}


ROEdgeVector::~ROEdgeVector()
{}


void
ROEdgeVector::add(const ROEdge *edge)
{
    _edges.push_back(edge);
}


std::ostream &operator<<(std::ostream &os, const ROEdgeVector &ev)
{
    for (ROEdgeVector::EdgeVector::const_iterator j=ev._edges.begin(); j!=ev._edges.end(); j++) {
        if (j!=ev._edges.begin()) {
            os << ' ';
        }
        os << (*j)->getID();
    }
    return os;
}


const ROEdge *
ROEdgeVector::getFirst() const
{
    if (_edges.size()==0) {
        throw OutOfBoundsException();
    }
    return _edges[0];
}


const ROEdge *
ROEdgeVector::getLast() const
{
    if (_edges.size()==0) {
        throw OutOfBoundsException();
    }
    return _edges[_edges.size()-1];
}



std::deque<std::string>
ROEdgeVector::getIDs() const
{
    std::deque<std::string> ret;
    for (EdgeVector::const_iterator i=_edges.begin(); i!=_edges.end(); i++) {
        ret.push_back((*i)->getID());
    }
    return ret;
}


SUMOReal
ROEdgeVector::recomputeCosts(const ROVehicle *const v, SUMOTime time) const
{
    SUMOReal costs = 0;
    for (EdgeVector::const_iterator i=_edges.begin(); i!=_edges.end(); i++) {
        costs += (*i)->getCost(v, time);
        time += ((SUMOTime)(*i)->getDuration(v, time));
    }
    return costs;
}


bool
ROEdgeVector::equals(const ROEdgeVector &vc) const
{
    if (size()!=vc.size()) {
        return false;
    }
    for (size_t i=0; i<size(); i++) {
        if (_edges[i]!=vc._edges[i]) {
            return false;
        }
    }
    return true;
}


size_t
ROEdgeVector::size() const
{
    return _edges.size();
}


void
ROEdgeVector::clear()
{
    _edges.clear();
}


ROEdgeVector
ROEdgeVector::getReverse() const
{
    ROEdgeVector ret(_edges.size());
    for (EdgeVector::const_reverse_iterator i=_edges.rbegin(); i!=_edges.rend(); i++) {
        ret.add(*i);
    }
    return ret;
}


void
ROEdgeVector::removeEnds()
{
    _edges.erase(_edges.begin());
    _edges.erase(_edges.end()-1);
}


void
ROEdgeVector::removeFirst()
{
    _edges.erase(_edges.begin());
}


const ROEdgeVector::EdgeVector &
ROEdgeVector::getEdges() const
{
    return _edges;
}


bool isTurnaround(const ROEdge *e1, const ROEdge *e2)
{
    return e1->getFromNode()==e2->getToNode() && e1->getToNode()==e2->getFromNode();
}

void
ROEdgeVector::recheckForLoops()
{
    // forward
    {
        int lastReversed = 0;
        bool found = false;
        for (int i=0; i<(int) _edges.size()/2+1; i++) {
            for (int j=i+1; j<(int) _edges.size()/2+1; j++) {
                if (isTurnaround(_edges[i], _edges[j])&&lastReversed<j) {
                    lastReversed = j;
                    found = true;
                }
            }
        }
        if (found) {
//            cout << "Erasing from begin to " << lastReversed << endl;
            _edges.erase(_edges.begin(), _edges.begin()+lastReversed-1);
        }
    }
    //
    if (_edges.size()<2) {
        return;
    }
    // backward
    {
        int lastReversed = _edges.size()-1;
        bool found = false;
        for (int i=_edges.size()-1; i>=0; i--) {
            for (int j=i-1; j>=0; j--) {
                if (isTurnaround(_edges[i], _edges[j])&&lastReversed>j) {
//                    cout << endl << _edges[i]->getID() << " " << _edges[j]->getID() << endl;
                    lastReversed = j;
                    found = true;
                }
            }
        }
        if (found) {
//            cout << endl;
//            cout << (*this) << endl;
//            cout << "Erasing from "<< lastReversed << " to end " << endl;
            _edges.erase(_edges.begin()+lastReversed, _edges.end());
//            cout << (*this) << endl;
//            cout << "-----------" << endl;
        }
    }
}



/****************************************************************************/

