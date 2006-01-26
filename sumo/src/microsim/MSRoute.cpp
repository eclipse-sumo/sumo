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
// Revision 1.10  2006/01/26 08:30:29  dkrajzew
// patched MSEdge in order to work with a generic router
//
// Revision 1.9  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/05/04 08:32:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.6  2004/12/16 12:24:45  dkrajzew
// debugging
//
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

#include <cassert>
#include <algorithm>
#include "MSRoute.h"
#include "MSEdge.h"
#include <utils/common/FileHelpers.h>
#include <utils/bindevice/BinaryInputDevice.h>

#ifdef ABS_DEBUG
#include "MSNet.h"
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
    _multipleReferenced(multipleReferenced),
    myReferenceNo(0)
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


const MSEdge *
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
MSRoute::erase(std::string id)
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
MSRoute::find(const MSEdge *e) const
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
		os << (*i)->getID();
	}
}


bool
MSRoute::contains(MSEdge *edge) const
{
    return find(edge)!=_edges.end();
}


bool
MSRoute::containsAnyOf(const std::vector<MSEdge*> &edgelist) const
{
    std::vector<MSEdge*>::const_iterator i = edgelist.begin();
    for(; i!=edgelist.end(); ++i) {
        if(contains(*i)) {
            return true;
        }
    }
    return false;
}


const MSEdge *
MSRoute::operator[](size_t index)
{
    return _edges[index];
}


void
MSRoute::dict_saveState(std::ostream &os, long what)
{
    FileHelpers::writeUInt(os, myDict.size());
    for(RouteDict::iterator it = myDict.begin(); it!=myDict.end(); ++it) {
        (*it).second->saveState(os, what);
    }
}


void
MSRoute::saveState(std::ostream &os, long what)
{
    FileHelpers::writeString(os, getID());
    FileHelpers::writeUInt(os, _edges.size());
    FileHelpers::writeByte(os, _multipleReferenced);
    for(MSEdgeVector::const_iterator i = _edges.begin(); i!=_edges.end(); ++i) {
        FileHelpers::writeUInt(os, (*i)->getNumericalID());
    }
}


void
MSRoute::dict_loadState(BinaryInputDevice &bis, long what)
{
    unsigned int noRoutes;
    bis >> noRoutes;
    while(noRoutes>0) {
        string id;
        bis >> id;
        unsigned int no;
        bis >> no;
        bool multipleReferenced;
        bis >> multipleReferenced;
        bool had = dictionary(id)!=0;
        MSEdgeVector edges;
        if(!had) {
            edges.reserve(no);
        }
        while(no>0) {
            unsigned int edgeID;
            bis >> edgeID;
            if(!had) {
                MSEdge *e = MSEdge::dictionary(edgeID);
                assert(e!=0);
                edges.push_back(e);
            }
            no--;
        }
        if(!had) {
            MSRoute *r = new MSRoute(id, edges, multipleReferenced);
            dictionary(id, r);
        }
        noRoutes--;
    }
}


size_t
MSRoute::posInRoute(const MSRouteIterator &currentEdge) const
{
    return distance(_edges.begin(), currentEdge);
}


void
MSRoute::clearLoadedState()
{
    std::vector<MSRoute*> toDel;
    for(RouteDict::iterator it = myDict.begin(); it!=myDict.end(); ++it) {
        if((*it).second->noReferences()==0&&!(*it).second->inFurtherUse()) {
            toDel.push_back((*it).second);
        }
    }
    for(std::vector<MSRoute*>::iterator i=toDel.begin(); i!=toDel.end(); ++i) {
        erase((*i)->getID());
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


