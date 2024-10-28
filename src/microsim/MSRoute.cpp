/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2002-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSRoute.cpp
/// @author  Daniel Krajzewicz
/// @author  Friedemann Wesner
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
///
// A vehicle route
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <algorithm>
#include <limits>
#include <utils/common/FileHelpers.h>
#include <utils/common/RGBColor.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSEdge.h"
#include "MSLane.h"
#include "MSRoute.h"


// ===========================================================================
// static member variables
// ===========================================================================
MSRoute::RouteDict MSRoute::myDict;
MSRoute::RouteDistDict MSRoute::myDistDict;
#ifdef HAVE_FOX
FXMutex MSRoute::myDictMutex(true);
#endif


// ===========================================================================
// member method definitions
// ===========================================================================
MSRoute::MSRoute(const std::string& id,
                 const ConstMSEdgeVector& edges,
                 const bool isPermanent, const RGBColor* const c,
                 const std::vector<SUMOVehicleParameter::Stop>& stops,
                 SUMOTime replacedTime,
                 int replacedIndex) :
    Named(id), myEdges(edges), myAmPermanent(isPermanent),
    myColor(c),
    myPeriod(0),
    myCosts(-1),
    mySavings(0),
    myReroute(false),
    myStops(stops),
    myReplacedTime(replacedTime),
    myReplacedIndex(replacedIndex)
{}


MSRoute::~MSRoute() {
    delete myColor;
}


MSRouteIterator
MSRoute::begin() const {
    return myEdges.begin();
}


MSRouteIterator
MSRoute::end() const {
    return myEdges.end();
}


int
MSRoute::size() const {
    return (int)myEdges.size();
}


const MSEdge*
MSRoute::getLastEdge() const {
    assert(myEdges.size() > 0);
    return myEdges.back();
}


void
MSRoute::checkRemoval() const {
#ifdef HAVE_FOX
    FXMutexLock f(myDictMutex);
#endif
    if (!myAmPermanent) {
        myDict.erase(getID());
    }
}


bool
MSRoute::dictionary(const std::string& id, ConstMSRoutePtr route) {
#ifdef HAVE_FOX
    FXMutexLock f(myDictMutex);
#endif
    if (myDict.find(id) == myDict.end() && myDistDict.find(id) == myDistDict.end()) {
        myDict[id] = route;
        return true;
    }
    return false;
}


bool
MSRoute::dictionary(const std::string& id, RandomDistributor<ConstMSRoutePtr>* const routeDist, const bool permanent) {
#ifdef HAVE_FOX
    FXMutexLock f(myDictMutex);
#endif
    if (myDict.find(id) == myDict.end() && myDistDict.find(id) == myDistDict.end()) {
        myDistDict[id] = std::make_pair(routeDist, permanent);
        return true;
    }
    return false;
}


ConstMSRoutePtr
MSRoute::dictionary(const std::string& id, SumoRNG* rng) {
#ifdef HAVE_FOX
    FXMutexLock f(myDictMutex);
#endif
    RouteDict::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        RouteDistDict::iterator it2 = myDistDict.find(id);
        if (it2 == myDistDict.end() || it2->second.first->getOverallProb() == 0) {
            return nullptr;
        }
        return it2->second.first->get(rng);
    }
    return it->second;
}


bool
MSRoute::hasRoute(const std::string& id) {
#ifdef HAVE_FOX
    FXMutexLock f(myDictMutex);
#endif
    return myDict.find(id) != myDict.end();
}


RandomDistributor<ConstMSRoutePtr>*
MSRoute::distDictionary(const std::string& id) {
#ifdef HAVE_FOX
    FXMutexLock f(myDictMutex);
#endif
    RouteDistDict::iterator it2 = myDistDict.find(id);
    if (it2 == myDistDict.end()) {
        return nullptr;
    }
    return it2->second.first;
}


void
MSRoute::clear() {
#ifdef HAVE_FOX
    FXMutexLock f(myDictMutex);
#endif
    for (RouteDistDict::iterator i = myDistDict.begin(); i != myDistDict.end(); ++i) {
        delete i->second.first;
    }
    myDistDict.clear();
    myDict.clear();
}


void
MSRoute::checkDist(const std::string& id) {
#ifdef HAVE_FOX
    FXMutexLock f(myDictMutex);
#endif
    RouteDistDict::iterator it = myDistDict.find(id);
    if (it != myDistDict.end() && !it->second.second) {
        for (ConstMSRoutePtr rp : it->second.first->getVals()) {
            const MSRoute& r = *rp;
            r.checkRemoval();
        }
        delete it->second.first;
        myDistDict.erase(it);
    }
}


void
MSRoute::insertIDs(std::vector<std::string>& into) {
#ifdef HAVE_FOX
    FXMutexLock f(myDictMutex);
#endif
    into.reserve(myDict.size() + myDistDict.size() + into.size());
    for (RouteDict::const_iterator i = myDict.begin(); i != myDict.end(); ++i) {
        into.push_back((*i).first);
    }
    for (RouteDistDict::const_iterator i = myDistDict.begin(); i != myDistDict.end(); ++i) {
        into.push_back((*i).first);
    }
}


int
MSRoute::writeEdgeIDs(OutputDevice& os, int firstIndex, int lastIndex, bool withInternal, SUMOVehicleClass svc) const {
    //std::cout << SIMTIME << " writeEdgeIDs " << getID() << " first=" << firstIndex << " lastIndex=" << lastIndex << " edges=" << toString(myEdges) << "\n";
    if (lastIndex < 0) {
        lastIndex = (int)myEdges.size();
    }
    int internal = 0;
    for (int i = firstIndex; i < lastIndex; i++) {
        os << myEdges[i]->getID() << ' ';
        if (withInternal && i + 1 < lastIndex) {
            const MSEdge* next = myEdges[i + 1];
            const MSEdge* edge = myEdges[i]->getInternalFollowingEdge(next, svc);
            // Take into account non-internal lengths until next non-internal edge
            while (edge != nullptr && edge->isInternal()) {
                os << edge->getID() << ' ';
                internal++;
                edge = edge->getInternalFollowingEdge(next, svc);
            }
        }
    }
    return internal + lastIndex - firstIndex;
}


bool
MSRoute::containsAnyOf(const MSEdgeVector& edgelist) const {
    MSEdgeVector::const_iterator i = edgelist.begin();
    for (; i != edgelist.end(); ++i) {
        if (contains(*i)) {
            return true;
        }
    }
    return false;
}


const MSEdge*
MSRoute::operator[](int index) const {
    return myEdges[index];
}


void
MSRoute::dict_saveState(OutputDevice& out) {
#ifdef HAVE_FOX
    FXMutexLock f(myDictMutex);
#endif
    for (RouteDict::iterator it = myDict.begin(); it != myDict.end(); ++it) {
        ConstMSRoutePtr r = (*it).second;
        out.openTag(SUMO_TAG_ROUTE);
        out.writeAttr(SUMO_ATTR_ID, r->getID());
        out.writeAttr(SUMO_ATTR_STATE, r->myAmPermanent);
        out.writeAttr(SUMO_ATTR_EDGES, r->myEdges);
        if (r->myColor != nullptr) {
            out.writeAttr(SUMO_ATTR_COLOR, *r->myColor);
        }
        for (auto stop : r->getStops()) {
            stop.write(out);
        }
        out.closeTag();
    }
    for (const auto& item : myDistDict) {
        if (item.second.first->getVals().size() > 0) {
            out.openTag(SUMO_TAG_ROUTE_DISTRIBUTION).writeAttr(SUMO_ATTR_ID, item.first);
            out.writeAttr(SUMO_ATTR_STATE, item.second.second);
            std::ostringstream oss;
            bool space = false;
            for (const auto& route : item.second.first->getVals()) {
                if (space) {
                    oss << " ";
                }
                oss << route->getID();
                space = true;
            }
            out.writeAttr(SUMO_ATTR_ROUTES, oss.str());
            out.writeAttr(SUMO_ATTR_PROBS, item.second.first->getProbs());
            out.closeTag();
        }
    }
}


void
MSRoute::dict_clearState() {
#ifdef HAVE_FOX
    FXMutexLock f(myDictMutex);
#endif
    myDistDict.clear();
    myDict.clear();
}


double
MSRoute::getDistanceBetween(double fromPos, double toPos,
                            const MSLane* fromLane, const MSLane* toLane, int routePosition) const {
    // std::cout << SIMTIME << " getDistanceBetween from=" << fromEdge->getID() << " to=" << toEdge->getID() << " fromPos=" << fromPos << " toPos=" << toPos << "\n";
    assert(fromPos >= 0. && fromPos <= fromLane->getLength());
    assert(toPos >= 0. && toPos <= toLane->getLength());
    assert(routePosition >= 0 && routePosition < (int)myEdges.size());
    assert(routePosition == 0 || !myEdges.front()->isInternal());
    const MSEdge* fromEdge = &fromLane->getEdge();
    const MSEdge* toEdge = &toLane->getEdge();
    if (fromEdge == toEdge && fromPos <= toPos) {
        return toPos - fromPos;
    }
    // TODO If fromEdge and toEdge are identical or both are internal and directly connected,
    // the code does not check whether they are in any relation to the route.
    if (fromEdge->isInternal()) {
        double minDist = std::numeric_limits<double>::max();
        for (const auto& via : fromEdge->getViaSuccessors()) {
            const MSEdge* const succ = via.second == nullptr ? via.first : via.second;
            assert(succ != nullptr);
            // std::cout << "  recurse fromSucc=" << succ->getID() << "\n";
            const double d = getDistanceBetween(0., toPos, succ->getLanes()[0], toLane, routePosition);
            if (d != std::numeric_limits<double>::max() && fromLane->getLength() - fromPos + d  < minDist) {
                minDist = fromLane->getLength() - fromPos + d;
            }
        }
        return minDist;
    }
    if (toEdge->isInternal()) {
        const MSEdge* const pred = toEdge->getPredecessors().front();
        assert(pred != nullptr);
        // std::cout << "  recurse toPred=" << pred->getID() << "\n";
        const double d = getDistanceBetween(fromPos, pred->getLength(), fromLane, pred->getLanes()[0], routePosition);
        return d == std::numeric_limits<double>::max() ? d : toPos + d;
    }
    ConstMSEdgeVector::const_iterator fromIt = std::find(myEdges.begin() + routePosition, myEdges.end(), fromEdge);
    if (fromIt == myEdges.end()) {
        // start not contained in route
        return std::numeric_limits<double>::max();
    }
    ConstMSEdgeVector::const_iterator toIt = std::find(fromIt + 1, myEdges.end(), toEdge);
    if (toIt == myEdges.end()) {
        // destination not contained in route
        return std::numeric_limits<double>::max();
    }
    return getDistanceBetween(fromPos, toPos, fromIt, toIt, true);
}


double
MSRoute::getDistanceBetween(double fromPos, double toPos,
                            const MSRouteIterator& fromEdge, const MSRouteIterator& toEdge, bool includeInternal) const {
    bool isFirstIteration = true;
    double distance = -fromPos;
    MSRouteIterator it = fromEdge;
    if (fromEdge == toEdge) {
        // destination position is on start edge
        if (fromPos <= toPos) {
            return toPos - fromPos;
        } else {
            // we cannot go backwards. Something is wrong here
            return std::numeric_limits<double>::max();
        }
    } else if (fromEdge > toEdge) {
        // we don't visit the edge again
        return std::numeric_limits<double>::max();
    }
    for (; it != end(); ++it) {
        if (it == toEdge && !isFirstIteration) {
            distance += toPos;
            break;
        } else {
            distance += (*it)->getLength();
            if (includeInternal && (it + 1) != end()) {
                // XXX the length may be wrong if there are parallel internal edges for different vClasses
                distance += (*it)->getInternalFollowingLengthTo(*(it + 1), SVC_IGNORING);
            }
        }
        isFirstIteration = false;
    }
    return distance;
}


const RGBColor&
MSRoute::getColor() const {
    if (myColor == nullptr) {
        return RGBColor::DEFAULT_COLOR;
    }
    return *myColor;
}


const std::vector<SUMOVehicleParameter::Stop>&
MSRoute::getStops() const {
    return myStops;
}


/****************************************************************************/
