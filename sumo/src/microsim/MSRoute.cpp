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
// Revision 1.3  2003/03/03 14:56:23  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <cassert>
#include "MSRoute.h"

using namespace std;

MSRoute::RouteDict MSRoute::myDict;

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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSRoute.icc"
//#endif

// Local Variables:
// mode:C++
// End:


