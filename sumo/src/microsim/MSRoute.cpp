//---------------------------------------------------------------------------//
//                        MSRoute.cpp -
//  A vehicle route
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
// Revision 1.5  2004/11/23 10:20:10  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.4  2004/07/02 09:26:23  dkrajzew
// classes prepared to be derived
//
// Revision 1.3  2003/03/03 14:56:23  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <cassert>
#include <algorithm>
#include "MSRoute.h"
#include "MSEdge.h"

#ifdef ABS_DEBUG
#include "MSNet.h"
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member variables
 * ======================================================================= */
MSRoute::RouteDict MSRoute::myDict;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSRoute::MSRoute(const std::string &id,
                 const MSEdgeVector &edges,
                 bool multipleReferenced)
    : Named(id), _edges(edges),
    _multipleReferenced(multipleReferenced)
{
}


MSRoute::~MSRoute()
{
}

MSRouteIterator
MSRoute::begin() const
{
    return _edges.begin();
}

MSRouteIterator
MSRoute::end() const
{
    return _edges.end();
}

size_t
MSRoute::size() const
{
    return _edges.size();
}


MSEdge *
MSRoute::getLastEdge() const
{
    assert(_edges.size()>0);
    return _edges[_edges.size()-1];
}

bool
MSRoute::dictionary(const string &id, MSRoute* route)
{
    RouteDict::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(RouteDict::value_type(id, route));
        return true;
    }
    return false;
}


MSRoute*
MSRoute::dictionary(const string &id)
{
    RouteDict::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


void
MSRoute::clear()
{
    for(RouteDict::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}


void
MSRoute::erase(const std::string &id)
{
    RouteDict::iterator i=myDict.find(id);
    assert(i!=myDict.end());
    delete (*i).second;
    myDict.erase(id);
}


bool
MSRoute::inFurtherUse() const
{
    return _multipleReferenced;
}


bool
MSRoute::replaceBy(const MSEdgeVector &edges, MSRouteIterator &currentEdge)
{
    // do not replace if the vehicle is already out of the route
    MSEdgeVector::const_iterator i =
        std::find(edges.begin(), edges.end(), *currentEdge);
    if(i==edges.end()) {
        return false;
    }
    MSEdgeVector n;
    copy(_edges.begin(), std::find(_edges.begin(), _edges.end(), *currentEdge),
        back_inserter(n));
    copy(i, edges.end(), back_inserter(n));
    _edges = n;
    return true;
}


MSRouteIterator
MSRoute::find(MSEdge *e) const
{
    return std::find(_edges.begin(), _edges.end(), e);
}


void
MSRoute::writeEdgeIDs(std::ostream &os) const
{
    MSEdgeVector::const_iterator i = _edges.begin();
    for(;i!=_edges.end(); ++i) {
        if(i!=_edges.begin()) {
            os << ' ';
        }
        os << (*i)->id();
    }
}


MSEdge *
MSRoute::operator[](size_t index)
{
    return _edges[index];
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


