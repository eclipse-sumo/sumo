//---------------------------------------------------------------------------//
//                        ROEdgeVector.cpp -
//  A vector of edges (a route)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.13  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.12  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.11  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.10  2005/05/04 08:46:09  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.9  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.8  2004/04/02 11:25:34  dkrajzew
// moving the vehicle forward if it shall start at a too short edge added
//
// Revision 1.7  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.6  2003/09/17 10:14:27  dkrajzew
// handling of unset values patched
//
// Revision 1.5  2003/04/10 15:47:01  dkrajzew
// random routes are now being prunned to avoid some stress with turning vehicles
//
// Revision 1.4  2003/04/04 15:40:17  roessel
// Changed 'os << " ";' to 'os << string(" ");'
//
// Revision 1.3  2003/03/20 16:39:16  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.2  2003/02/07 10:45:04  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <deque>
#include <utils/common/UtilExceptions.h>
#include "ROEdge.h"
#include "ROEdgeVector.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method defintions
 * ======================================================================= */
ROEdgeVector::ROEdgeVector()
{
}


ROEdgeVector::ROEdgeVector(size_t toReserve)
{
    _edges.reserve(toReserve);
}


ROEdgeVector::ROEdgeVector(EdgeVector &edges)
    : _edges(edges)
{
}


ROEdgeVector::~ROEdgeVector()
{
}


void
ROEdgeVector::add(ROEdge *edge)
{
    _edges.push_back(edge);
}


std::ostream &operator<<(std::ostream &os, const ROEdgeVector &ev)
{
    for(ROEdgeVector::EdgeVector::const_iterator j=ev._edges.begin(); j!=ev._edges.end(); j++) {
        if(j!=ev._edges.begin()) {
            os << string(" ");
        }
        os << (*j)->getID();
    }
    return os;
}


ROEdge *
ROEdgeVector::getFirst() const
{
    if(_edges.size()==0) {
        throw OutOfBoundsException();
    }
    return _edges[0];
}


ROEdge *
ROEdgeVector::getLast() const
{
    if(_edges.size()==0) {
        throw OutOfBoundsException();
    }
    return _edges[_edges.size()-1];
}



std::deque<std::string>
ROEdgeVector::getIDs() const
{
    std::deque<std::string> ret;
    for(EdgeVector::const_iterator i=_edges.begin(); i!=_edges.end(); i++) {
        ret.push_back((*i)->getID());
    }
    return ret;
}


SUMOReal
ROEdgeVector::recomputeCosts(SUMOTime time) const
{
    SUMOReal costs = 0;
    for(EdgeVector::const_iterator i=_edges.begin(); i!=_edges.end(); i++) {
        costs += (*i)->getCost(time);
        time += ((SUMOTime) (*i)->getDuration(time));
    }
    return costs;
}


bool
ROEdgeVector::equals(const ROEdgeVector &vc) const
{
    if(size()!=vc.size()) {
        return false;
    }
    for(size_t i=0; i<size(); i++) {
        if(_edges[i]!=vc._edges[i]) {
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
    for(EdgeVector::const_reverse_iterator i=_edges.rbegin(); i!=_edges.rend(); i++) {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


