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
    myEdges.reserve(toReserve);
}


ROEdgeVector::ROEdgeVector(const EdgeVector &edges)
        : myEdges(edges)
{}


ROEdgeVector::~ROEdgeVector()
{}


void
ROEdgeVector::add(const ROEdge *edge)
{
    myEdges.push_back(edge);
}


std::ostream &operator<<(std::ostream &os, const ROEdgeVector &ev)
{
    for (ROEdgeVector::EdgeVector::const_iterator j=ev.myEdges.begin(); j!=ev.myEdges.end(); j++) {
        if (j!=ev.myEdges.begin()) {
            os << ' ';
        }
        os << (*j)->getID();
    }
    return os;
}


const ROEdge *
ROEdgeVector::getFirst() const
{
    if (myEdges.size()==0) {
        throw OutOfBoundsException();
    }
    return myEdges[0];
}


const ROEdge *
ROEdgeVector::getLast() const
{
    if (myEdges.size()==0) {
        throw OutOfBoundsException();
    }
    return myEdges[myEdges.size()-1];
}



std::deque<std::string>
ROEdgeVector::getIDs() const
{
    std::deque<std::string> ret;
    for (EdgeVector::const_iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        ret.push_back((*i)->getID());
    }
    return ret;
}


SUMOReal
ROEdgeVector::recomputeCosts(const ROVehicle *const v, SUMOTime time) const
{
    SUMOReal costs = 0;
    for (EdgeVector::const_iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        costs += (*i)->getCost(v, time);
        time += ((SUMOTime)(*i)->getDuration(v, time));
        if ((*i)->prohibits(v)) {
            return -1;
        }
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
        if (myEdges[i]!=vc.myEdges[i]) {
            return false;
        }
    }
    return true;
}


size_t
ROEdgeVector::size() const
{
    return myEdges.size();
}


void
ROEdgeVector::clear()
{
    myEdges.clear();
}


ROEdgeVector
ROEdgeVector::getReverse() const
{
    ROEdgeVector ret(myEdges.size());
    for (EdgeVector::const_reverse_iterator i=myEdges.rbegin(); i!=myEdges.rend(); i++) {
        ret.add(*i);
    }
    return ret;
}


void
ROEdgeVector::removeEnds()
{
    myEdges.erase(myEdges.begin());
    myEdges.erase(myEdges.end()-1);
}


void
ROEdgeVector::removeFirst()
{
    myEdges.erase(myEdges.begin());
}


const ROEdgeVector::EdgeVector &
ROEdgeVector::getEdges() const
{
    return myEdges;
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
        for (int i=0; i<(int) myEdges.size()/2+1; i++) {
            for (int j=i+1; j<(int) myEdges.size()/2+1; j++) {
                if (isTurnaround(myEdges[i], myEdges[j])&&lastReversed<j) {
                    lastReversed = j;
                    found = true;
                }
            }
        }
        if (found) {
//            cout << "Erasing from begin to " << lastReversed << endl;
            myEdges.erase(myEdges.begin(), myEdges.begin()+lastReversed-1);
        }
    }
    //
    if (myEdges.size()<2) {
        return;
    }
    // backward
    {
        int lastReversed = myEdges.size()-1;
        bool found = false;
        for (int i=myEdges.size()-1; i>=0; i--) {
            for (int j=i-1; j>=0; j--) {
                if (isTurnaround(myEdges[i], myEdges[j])&&lastReversed>j) {
//                    cout << endl << myEdges[i]->getID() << " " << myEdges[j]->getID() << endl;
                    lastReversed = j;
                    found = true;
                }
            }
        }
        if (found) {
//            cout << endl;
//            cout << (*this) << endl;
//            cout << "Erasing from "<< lastReversed << " to end " << endl;
            myEdges.erase(myEdges.begin()+lastReversed, myEdges.end());
//            cout << (*this) << endl;
//            cout << "-----------" << endl;
        }
    }
}



/****************************************************************************/

