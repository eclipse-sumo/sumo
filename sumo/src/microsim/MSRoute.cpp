/****************************************************************************/
/// @file    MSRoute.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A vehicle route
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <cassert>
#include <algorithm>
#include <limits>
#include "MSRoute.h"
#include "MSEdge.h"
#include "MSLane.h"
#include <utils/common/FileHelpers.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member variables
// ===========================================================================
MSRoute::RouteDict MSRoute::myDict;
MSRoute::RouteDistDict MSRoute::myDistDict;


// ===========================================================================
// member method definitions
// ===========================================================================
MSRoute::MSRoute(const std::string &id,
                 const MSEdgeVector &edges,
                 bool multipleReferenced) throw()
        : Named(id), myEdges(edges),
        myMultipleReferenced(multipleReferenced) {}


MSRoute::~MSRoute() throw() {}

MSRouteIterator
MSRoute::begin() const {
    return myEdges.begin();
}

MSRouteIterator
MSRoute::end() const {
    return myEdges.end();
}

unsigned
MSRoute::size() const {
    return (unsigned) myEdges.size();
}


const MSEdge *
MSRoute::getLastEdge() const {
    assert(myEdges.size()>0);
    return myEdges[myEdges.size()-1];
}

bool
MSRoute::dictionary(const string &id, const MSRoute* route) {
    if (myDict.find(id) == myDict.end() && myDistDict.find(id) == myDistDict.end()) {
        myDict[id] = route;
        return true;
    }
    return false;
}


bool
MSRoute::dictionary(const string &id, RandomDistributor<const MSRoute*>* routeDist) {
    if (myDict.find(id) == myDict.end() && myDistDict.find(id) == myDistDict.end()) {
        myDistDict[id] = routeDist;
        return true;
    }
    return false;
}


const MSRoute*
MSRoute::dictionary(const string &id) {
    RouteDict::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        RouteDistDict::iterator it2 = myDistDict.find(id);
        if (it2 == myDistDict.end() || it2->second->getOverallProb() == 0) {
            return 0;
        }
        return it2->second->get();
    }
    return it->second;
}


RandomDistributor<const MSRoute*> *
MSRoute::distDictionary(const string &id) {
    RouteDistDict::iterator it2 = myDistDict.find(id);
    if (it2 == myDistDict.end()) {
        return 0;
    }
    return it2->second;
}


void
MSRoute::clear() {
    for (RouteDict::iterator i=myDict.begin(); i!=myDict.end(); ++i) {
        delete(*i).second;
    }
    myDict.clear();
}


void
MSRoute::erase(std::string id) {
    RouteDict::iterator i=myDict.find(id);
    assert(i!=myDict.end());
    delete(*i).second;
    myDict.erase(id);
}


bool
MSRoute::inFurtherUse() const {
    return myMultipleReferenced;
}


void
MSRoute::insertIDs(std::vector<std::string> &into) {
    into.reserve(myDict.size()+myDistDict.size()+into.size());
    for (RouteDict::const_iterator i=myDict.begin(); i!=myDict.end(); ++i) {
        into.push_back((*i).first);
    }
    for (RouteDistDict::const_iterator i=myDistDict.begin(); i!=myDistDict.end(); ++i) {
        into.push_back((*i).first);
    }
}


MSRouteIterator
MSRoute::find(const MSEdge *e) const {
    return std::find(myEdges.begin(), myEdges.end(), e);
}


MSRouteIterator
MSRoute::find(const MSEdge *e, const MSRouteIterator &startingAt) const {
    return std::find(startingAt, myEdges.end(), e);
}


void
MSRoute::writeEdgeIDs(OutputDevice &os, const MSEdge *upTo) const {
    MSEdgeVector::const_iterator i = myEdges.begin();
    for (; i!=myEdges.end(); ++i) {
        if (i!=myEdges.begin()) {
            os << ' ';
        }
        if ((*i) == upTo) {
            return;
        }
        os << (*i)->getID();
    }
}


bool
MSRoute::containsAnyOf(const std::vector<MSEdge*> &edgelist) const {
    std::vector<MSEdge*>::const_iterator i = edgelist.begin();
    for (; i!=edgelist.end(); ++i) {
        if (contains(*i)) {
            return true;
        }
    }
    return false;
}


const MSEdge *
MSRoute::operator[](unsigned index) const {
    return myEdges[index];
}


#ifdef HAVE_MESOSIM
void
MSRoute::dict_saveState(std::ostream &os) throw() {
    FileHelpers::writeUInt(os, (unsigned int) myDict.size());
    for (RouteDict::iterator it = myDict.begin(); it!=myDict.end(); ++it) {
        FileHelpers::writeString(os, (*it).second->getID());
        FileHelpers::writeUInt(os, (unsigned int)(*it).second->myEdges.size());
        FileHelpers::writeByte(os, (*it).second->myMultipleReferenced);
        for (MSEdgeVector::const_iterator i = (*it).second->myEdges.begin(); i!=(*it).second->myEdges.end(); ++i) {
            FileHelpers::writeUInt(os, (*i)->getNumericalID());
        }
    }
    FileHelpers::writeUInt(os, (unsigned int) myDistDict.size());
    for (RouteDistDict::iterator it = myDistDict.begin(); it!=myDistDict.end(); ++it) {
        FileHelpers::writeString(os, (*it).first);
        const unsigned int size = (unsigned int)(*it).second->getVals().size();
        FileHelpers::writeUInt(os, size);
        for (unsigned int i = 0; i < size; ++i) {
            FileHelpers::writeString(os, (*it).second->getVals()[i]->getID());
            FileHelpers::writeFloat(os, (*it).second->getProbs()[i]);
        }
    }
}


void
MSRoute::dict_loadState(BinaryInputDevice &bis) throw() {
    unsigned int numRoutes;
    bis >> numRoutes;
    for (; numRoutes>0; numRoutes--) {
        string id;
        bis >> id;
        unsigned int no;
        bis >> no;
        bool multipleReferenced;
        bis >> multipleReferenced;
        if (dictionary(id)==0) {
            MSEdgeVector edges;
            edges.reserve(no);
            for (; no>0; no--) {
                unsigned int edgeID;
                bis >> edgeID;
                MSEdge *e = MSEdge::dictionary(edgeID);
                assert(e!=0);
                edges.push_back(e);
            }
            MSRoute *r = new MSRoute(id, edges, multipleReferenced);
            dictionary(id, r);
        } else {
            for (; no>0; no--) {
                unsigned int edgeID;
                bis >> edgeID;
            }
        }
    }
    unsigned int numRouteDists;
    bis >> numRouteDists;
    for (; numRouteDists>0; numRouteDists--) {
        string id;
        bis >> id;
        unsigned int no;
        bis >> no;
        if (dictionary(id)==0) {
            RandomDistributor<const MSRoute*> *dist = new RandomDistributor<const MSRoute*>();
            for (; no>0; no--) {
                string routeID;
                bis >> routeID;
                const MSRoute *r = dictionary(routeID);
                assert(r!=0);
                SUMOReal prob;
                bis >> prob;
                dist->add(prob, r, false);
            }
            dictionary(id, dist);
        } else {
            for (; no>0; no--) {
                string routeID;
                bis >> routeID;
                SUMOReal prob;
                bis >> prob;
            }
        }
    }
}
#endif


unsigned
MSRoute::posInRoute(const MSRouteIterator &currentEdge) const {
    return (unsigned int) distance(myEdges.begin(), currentEdge);
}


SUMOReal
MSRoute::getLength() const {
    SUMOReal ret = 0;
    for (MSEdgeVector::const_iterator i=myEdges.begin(); i!=myEdges.end(); ++i) {
        ret += (*(*i)->getLanes())[0]->length();
    }
    return ret;
}


SUMOReal
MSRoute::getDistanceBetween(SUMOReal fromPos, SUMOReal toPos, const MSEdge* fromEdge, const MSEdge* toEdge) const {
    bool isFirstIteration = true;
    SUMOReal distance = -fromPos;

    if ((find(fromEdge) == end()) || (find(toEdge) == end())) {
        // start or destination not contained in route
        return std::numeric_limits<SUMOReal>::max();
    }
    if (fromEdge == toEdge) {
        if (fromPos <= toPos) {
            // destination position is on start edge
            return (toPos - fromPos);
        } else {
            // start and destination edge are equal: ensure that this edge is contained at least twice in the route
            if (std::find(find(fromEdge)+1, end(), fromEdge) == end()) {
                return std::numeric_limits<SUMOReal>::max();
            }
        }
    }
    for (MSRouteIterator it = find(fromEdge); it!=end(); ++it) {
        if ((*it) == toEdge && !isFirstIteration) {
            distance += toPos;
            break;
        } else {
            const MSEdge::LaneCont& lanes = *((*it)->getLanes());
            distance += lanes[0]->length();
#ifdef HAVE_INTERNAL_LANES
            // add length of internal lanes to the result
            for (MSEdge::LaneCont::const_iterator laneIt = lanes.begin(); laneIt != lanes.end(); laneIt++) {
                const MSLinkCont& links = (*laneIt)->getLinkCont();
                for (MSLinkCont::const_iterator linkIt = links.begin(); linkIt != links.end(); linkIt++) {
                    if ((*linkIt)==0||(*linkIt)->getLane()==0) {
                        continue;
                    }
                    std::string succLaneId = (*(*(it+1))->getLanes()->begin())->getID();
                    if ((*linkIt)->getLane()->getID().compare(succLaneId) == 0) {
                        distance += (*linkIt)->getLength();
                    }
                }
            }
#endif
        }
        isFirstIteration = false;
    }
    return distance;
}


/****************************************************************************/

