/****************************************************************************/
/// @file    NBEdgeCont.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Storage for edges, including some functionality operating on multiple edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <cassert>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <utils/geom/GeomHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include "NBNetBuilder.h"
#include "NBEdgeCont.h"
#include "NBNodeCont.h"
#include "NBHelpers.h"
#include "NBCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBDistrictCont.h"
#include <cmath>
#include "NBTypeCont.h"
#include <iostream>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NBEdgeCont::NBEdgeCont(NBTypeCont& tc)
    : myEdgesSplit(0), myTypeCont(tc) {}


NBEdgeCont::~NBEdgeCont() {
    clear();
}


void
NBEdgeCont::applyOptions(OptionsCont& oc) {
    myAmLeftHanded = oc.getBool("lefthand");
    // set edges dismiss/accept options
    myEdgesMinSpeed = oc.isSet("keep-edges.min-speed") ? oc.getFloat("keep-edges.min-speed") : -1;
    myRemoveEdgesAfterJoining = oc.exists("keep-edges.postload") && oc.getBool("keep-edges.postload");
    if (oc.isSet("keep-edges.explicit")) {
        const std::vector<std::string> edges = oc.getStringVector("keep-edges.explicit");
        myEdges2Keep.insert(edges.begin(), edges.end());
    }
    if (oc.isSet("remove-edges.explicit")) {
        const std::vector<std::string> edges = oc.getStringVector("remove-edges.explicit");
        myEdges2Remove.insert(edges.begin(), edges.end());
    }
    if (oc.exists("keep-edges.by-vclass") && oc.isSet("keep-edges.by-vclass")) {
        const std::vector<std::string> classes = oc.getStringVector("keep-edges.by-vclass");
        for (std::vector<std::string>::const_iterator i = classes.begin(); i != classes.end(); ++i) {
            SUMOVehicleClass svc = getVehicleClassID(*i);
            myVehicleClasses2Keep.insert(svc);
        }
    }
    if (oc.exists("remove-edges.by-vclass") && oc.isSet("remove-edges.by-vclass")) {
        const std::vector<std::string> classes = oc.getStringVector("remove-edges.by-vclass");
        for (std::vector<std::string>::const_iterator i = classes.begin(); i != classes.end(); ++i) {
            SUMOVehicleClass svc = getVehicleClassID(*i);
            myVehicleClasses2Remove.insert(svc);
        }
    }
    if (oc.exists("keep-edges.by-type") && oc.isSet("keep-edges.by-type")) {
        const std::vector<std::string> types = oc.getStringVector("keep-edges.by-type");
        myTypes2Keep.insert(types.begin(), types.end());
    }
    if (oc.exists("remove-edges.by-type") && oc.isSet("remove-edges.by-type")) {
        const std::vector<std::string> types = oc.getStringVector("remove-edges.by-type");
        myTypes2Remove.insert(types.begin(), types.end());
    }

    if (oc.isSet("keep-edges.in-boundary")) {
        std::vector<std::string> polyS = oc.getStringVector("keep-edges.in-boundary");
        // !!! throw something if length<4 || length%2!=0?
        std::vector<SUMOReal> poly;
        for (std::vector<std::string>::iterator i = polyS.begin(); i != polyS.end(); ++i) {
            poly.push_back(TplConvert<char>::_2SUMOReal((*i).c_str())); // !!! may throw something anyhow...
        }
        if (poly.size() == 4) {
            // prunning boundary (box)
            myPrunningBoundary.push_back(Position(poly[0], poly[1]));
            myPrunningBoundary.push_back(Position(poly[2], poly[1]));
            myPrunningBoundary.push_back(Position(poly[2], poly[3]));
            myPrunningBoundary.push_back(Position(poly[0], poly[3]));
        } else {
            for (std::vector<SUMOReal>::iterator j = poly.begin(); j != poly.end();) {
                SUMOReal x = *j++;
                SUMOReal y = *j++;
                myPrunningBoundary.push_back(Position(x, y));
            }
        }
    }
}


void
NBEdgeCont::clear() {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        delete((*i).second);
    }
    myEdges.clear();
    for (EdgeCont::iterator i = myExtractedEdges.begin(); i != myExtractedEdges.end(); i++) {
        delete((*i).second);
    }
    myExtractedEdges.clear();
}



// ----- edge access methods
bool
NBEdgeCont::insert(NBEdge* edge, bool ignorePrunning) {
    if (myAmLeftHanded) {
        edge->setLeftHanded();
    }
    if (myEdges.count(edge->getID())) {
        return false;
    }
    if (!ignorePrunning && ignoreFilterMatch(edge)) {
        edge->getFromNode()->removeEdge(edge);
        edge->getToNode()->removeEdge(edge);
        myIgnoredEdges.insert(edge->getID());
        delete edge;
    } else {
        OptionsCont& oc = OptionsCont::getOptions();
        if (oc.exists("dismiss-vclasses") && oc.getBool("dismiss-vclasses")) {
            edge->dismissVehicleClassInformation();
        }
        myEdges[edge->getID()] = edge;
    }
    return true;
}


bool 
NBEdgeCont::ignoreFilterMatch(NBEdge *edge) {
    // remove edges which allow a speed below a set one (set using "keep-edges.min-speed")
    if (edge->getSpeed() < myEdgesMinSpeed) {
        return true;
    }
    // check whether the edge is a named edge to keep
    if (!myRemoveEdgesAfterJoining && myEdges2Keep.size() != 0) {
        if (find(myEdges2Keep.begin(), myEdges2Keep.end(), edge->getID()) == myEdges2Keep.end()) {
            return true;
        }
    }
    // check whether the edge is a named edge to remove
    if (myEdges2Remove.size() != 0) {
        if (find(myEdges2Remove.begin(), myEdges2Remove.end(), edge->getID()) != myEdges2Remove.end()) {
            return true;
        }
    }
    // check whether the edge shall be removed because it does not allow any of the wished classes
    if (myVehicleClasses2Keep.size() != 0) {
        SUMOVehicleClasses allowed = edge->getAllowedVehicleClasses();
        // @todo also check disallowed
        if (allowed.size() > 0) {
            int matching = 0;
            for (SUMOVehicleClasses::const_iterator i = myVehicleClasses2Keep.begin(); i != myVehicleClasses2Keep.end(); ++i) {
                if (allowed.count(*i)) {
                    allowed.erase(*i);
                    matching++;
                }
            }
            if (matching == 0) {
                return true;
            }
        }
    }
    // check whether the edge shall be removed due to allowing unwished classes only
    if (myVehicleClasses2Remove.size() != 0) {
        int matching = 0;
        SUMOVehicleClasses allowed = edge->getAllowedVehicleClasses();
        for (SUMOVehicleClasses::const_iterator i = myVehicleClasses2Remove.begin(); i != myVehicleClasses2Remove.end(); ++i) {
            if (allowed.count(*i)) {
                allowed.erase(*i);
                matching++;
            }
        }
        // remove the edge if all allowed
        if (allowed.size() == 0 && matching != 0) {
            return true;
        }
    }
    // check whether the edge shall be removed because it does not have one of the requested types
    if (myTypes2Keep.size() != 0) {
        if (myTypes2Keep.count(edge->getTypeID()) == 0) {
            return true;
        }
    }
    // check whether the edge shall be removed because it has one of the forbidden types
    if (myTypes2Remove.size() != 0) {
        if (myTypes2Remove.count(edge->getTypeID()) > 0) {
            return true;
        }
    }
    // check whether the edge is within the prunning boundary
    if (myPrunningBoundary.size() != 0) {
        if (!(edge->getGeometry().getBoxBoundary().grow((SUMOReal) POSITION_EPS).overlapsWith(myPrunningBoundary))) {
            return true;
        }
    }
    if (myTypeCont.knows(edge->getTypeID()) && myTypeCont.getShallBeDiscarded(edge->getTypeID())) {
        return true;
    }
    return false;
}


NBEdge*
NBEdgeCont::retrieve(const std::string& id, bool retrieveExtracted) const {
    EdgeCont::const_iterator i = myEdges.find(id);
    if (i == myEdges.end()) {
        if (retrieveExtracted) {
            i = myExtractedEdges.find(id);
            if (i == myExtractedEdges.end()) {
                return 0;
            }
        } else {
            return 0;
        }
    }
    return (*i).second;
}


NBEdge*
NBEdgeCont::retrievePossiblySplitted(const std::string& id,
                                     const std::string& hint,
                                     bool incoming) const {
    // try to retrieve using the given name (iterative)
    NBEdge* edge = retrieve(id);
    if (edge != 0) {
        return edge;
    }
    // now, we did not find it; we have to look over all possibilities
    EdgeVector hints;
    // check whether at least the hint was not splitted
    NBEdge* hintedge = retrieve(hint);
    if (hintedge == 0) {
        hints = getGeneratedFrom(hint);
    } else {
        hints.push_back(hintedge);
    }
    EdgeVector candidates = getGeneratedFrom(id);
    for (EdgeVector::iterator i = hints.begin(); i != hints.end(); i++) {
        NBEdge* hintedge = (*i);
        for (EdgeVector::iterator j = candidates.begin(); j != candidates.end(); j++) {
            NBEdge* poss_searched = (*j);
            NBNode* node = incoming
                           ? poss_searched->myTo : poss_searched->myFrom;
            const EdgeVector& cont = incoming
                                     ? node->getOutgoingEdges() : node->getIncomingEdges();
            if (find(cont.begin(), cont.end(), hintedge) != cont.end()) {
                return poss_searched;
            }
        }
    }
    return 0;
}


NBEdge*
NBEdgeCont::retrievePossiblySplitted(const std::string& id, SUMOReal pos) const {
    // check whether the edge was not split, yet
    NBEdge* edge = retrieve(id);
    if (edge != 0) {
        return edge;
    }
    size_t maxLength = 0;
    std::string tid = id + "[";
    for (EdgeCont::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        if ((*i).first.find(tid) == 0) {
            maxLength = MAX2(maxLength, (*i).first.length());
        }
    }
    // find the part of the edge which matches the position
    SUMOReal seen = 0;
    std::vector<std::string> names;
    names.push_back(id + "[1]");
    names.push_back(id + "[0]");
    while (names.size() > 0) {
        // retrieve the first subelement (to follow)
        std::string cid = names.back();
        names.pop_back();
        edge = retrieve(cid);
        // The edge was splitted; check its subparts within the
        //  next step
        if (edge == 0) {
            if (cid.length() + 3 < maxLength) {
                names.push_back(cid + "[1]");
                names.push_back(cid + "[0]");
            }
        }
        // an edge with the name was found,
        //  check whether the position lies within it
        else {
            seen += edge->getLength();
            if (seen >= pos) {
                return edge;
            }
        }
    }
    return 0;
}


void
NBEdgeCont::erase(NBDistrictCont& dc, NBEdge* edge) {
    extract(dc, edge);
    delete edge;
}


void
NBEdgeCont::extract(NBDistrictCont& dc, NBEdge* edge, bool remember) {
    if (remember) {
        myExtractedEdges[edge->getID()] = edge;
    }
    myEdges.erase(edge->getID());
    edge->myFrom->removeEdge(edge);
    edge->myTo->removeEdge(edge);
    dc.removeFromSinksAndSources(edge);
}



// ----- explicit edge manipulation methods
bool
NBEdgeCont::splitAt(NBDistrictCont& dc, NBEdge* edge, NBNode* node) {
    return splitAt(dc, edge, node, edge->getID() + "[0]", edge->getID() + "[1]",
                   (unsigned int) edge->myLanes.size(), (unsigned int) edge->myLanes.size());
}


bool
NBEdgeCont::splitAt(NBDistrictCont& dc, NBEdge* edge, NBNode* node,
                    const std::string& firstEdgeName,
                    const std::string& secondEdgeName,
                    unsigned int noLanesFirstEdge, unsigned int noLanesSecondEdge) {
    SUMOReal pos;
    pos = edge->getGeometry().nearest_position_on_line_to_point(node->getPosition());
    if (pos <= 0) {
        pos = GeomHelper::nearest_position_on_line_to_point2D(
                  edge->myFrom->getPosition(), edge->myTo->getPosition(),
                  node->getPosition());
    }
    if (pos <= 0 || pos + POSITION_EPS > edge->getGeometry().length()) {
        return false;
    }
    return splitAt(dc, edge, pos, node, firstEdgeName, secondEdgeName,
                   noLanesFirstEdge, noLanesSecondEdge);
}


bool
NBEdgeCont::splitAt(NBDistrictCont& dc,
                    NBEdge* edge, SUMOReal pos, NBNode* node,
                    const std::string& firstEdgeName,
                    const std::string& secondEdgeName,
                    unsigned int noLanesFirstEdge, unsigned int noLanesSecondEdge) {
    // build the new edges' geometries
    std::pair<PositionVector, PositionVector> geoms =
        edge->getGeometry().splitAt(pos);
    if (geoms.first[-1] != node->getPosition()) {
        geoms.first.pop_back();
        geoms.first.push_back(node->getPosition());
    }

    if (geoms.second[0] != node->getPosition()) {
        geoms.second.pop_front();
        geoms.second.push_front(node->getPosition());
    }
    // build and insert the edges
    NBEdge* one = new NBEdge(firstEdgeName,
                             edge->myFrom, node, edge->myType, edge->mySpeed, noLanesFirstEdge,
                             edge->getPriority(), edge->myWidth, 0, geoms.first,
                             edge->getStreetName(), edge->myLaneSpreadFunction, true);
    for (unsigned int i = 0; i < noLanesFirstEdge && i < edge->getNumLanes(); i++) {
        one->setSpeed(i, edge->getLaneSpeed(i));
    }
    NBEdge* two = new NBEdge(secondEdgeName,
                             node, edge->myTo, edge->myType, edge->mySpeed, noLanesSecondEdge,
                             edge->getPriority(), edge->myWidth, edge->myOffset, geoms.second,
                             edge->getStreetName(), edge->myLaneSpreadFunction, true);
    for (unsigned int i = 0; i < noLanesSecondEdge && i < edge->getNumLanes(); i++) {
        two->setSpeed(i, edge->getLaneSpeed(i));
    }
    two->copyConnectionsFrom(edge);
    // replace information about this edge within the nodes
    edge->myFrom->replaceOutgoing(edge, one, 0);
    edge->myTo->replaceIncoming(edge, two, 0);
    // the edge is now occuring twice in both nodes...
    //  clean up
    edge->myFrom->removeDoubleEdges();
    edge->myTo->removeDoubleEdges();
    // add connections from the first to the second edge
    // check special case:
    //  one in, one out, the outgoing has one lane more
    if (noLanesFirstEdge == noLanesSecondEdge - 1) {
        for (unsigned int i = 0; i < one->getNumLanes(); i++) {
            if (!one->addLane2LaneConnection(i, two, i + 1, NBEdge::L2L_COMPUTED)) { // !!! Bresenham, here!!!
                throw ProcessError("Could not set connection!");
            }
        }
        one->addLane2LaneConnection(0, two, 0, NBEdge::L2L_COMPUTED);
    } else {
        for (unsigned int i = 0; i < one->getNumLanes() && i < two->getNumLanes(); i++) {
            if (!one->addLane2LaneConnection(i, two, i, NBEdge::L2L_COMPUTED)) {// !!! Bresenham, here!!!
                throw ProcessError("Could not set connection!");
            }
        }
    }
    if (myRemoveEdgesAfterJoining) {
        if (find(myEdges2Keep.begin(), myEdges2Keep.end(), edge->getID()) != myEdges2Keep.end()) {
            myEdges2Keep.insert(one->getID());
            myEdges2Keep.insert(two->getID());
        }
        if (find(myEdges2Remove.begin(), myEdges2Remove.end(), edge->getID()) != myEdges2Remove.end()) {
            myEdges2Remove.insert(one->getID());
            myEdges2Remove.insert(two->getID());
        }
    }
    // erase the splitted edge
    erase(dc, edge);
    insert(one, true);
    insert(two, true);
    myEdgesSplit++;
    return true;
}



// ----- container access methods
std::vector<std::string>
NBEdgeCont::getAllNames() const {
    std::vector<std::string> ret;
    for (EdgeCont::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        ret.push_back((*i).first);
    }
    return ret;
}


// ----- Adapting the input
void
NBEdgeCont::removeUnwishedEdges(NBDistrictCont& dc) {
    EdgeVector toRemove;
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        NBEdge* edge = (*i).second;
        if (!myEdges2Keep.count(edge->getID())) {
            edge->getFromNode()->removeEdge(edge);
            edge->getToNode()->removeEdge(edge);
            toRemove.push_back(edge);
        }
    }
    for (EdgeVector::iterator j = toRemove.begin(); j != toRemove.end(); ++j) {
        erase(dc, *j);
    }
}


void
NBEdgeCont::splitGeometry(NBNodeCont& nc) {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        if ((*i).second->getGeometry().size() < 3) {
            continue;
        }
        (*i).second->splitGeometry(*this, nc);
    }
}


// ----- processing methods
void
NBEdgeCont::computeTurningDirections() {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        (*i).second->computeTurningDirections();
    }
}


void
NBEdgeCont::clearControllingTLInformation() const {
    for (EdgeCont::const_iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        (*i).second->clearControllingTLInformation();
    }
}


void
NBEdgeCont::sortOutgoingLanesConnections() {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        (*i).second->sortOutgoingConnectionsByAngle();
    }
}


void
NBEdgeCont::computeEdge2Edges(bool noLeftMovers) {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        (*i).second->computeEdge2Edges(noLeftMovers);
    }
}


void
NBEdgeCont::computeLanes2Edges() {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        (*i).second->computeLanes2Edges();
    }
}


void
NBEdgeCont::recheckLanes() {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        (*i).second->recheckLanes();
    }
}


void
NBEdgeCont::appendTurnarounds(bool noTLSControlled) {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        (*i).second->appendTurnaround(noTLSControlled);
    }
}


void
NBEdgeCont::appendTurnarounds(const std::set<std::string> &ids, bool noTLSControlled) {
    for (std::set<std::string>::const_iterator it = ids.begin(); it != ids.end(); it++) {
        myEdges[*it]->appendTurnaround(noTLSControlled);
    }
}


void
NBEdgeCont::computeEdgeShapes() {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        (*i).second->computeEdgeShape();
    }
}


void
NBEdgeCont::recomputeLaneShapes() {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        (*i).second->computeLaneShapes();
    }
}


void
NBEdgeCont::joinSameNodeConnectingEdges(NBDistrictCont& dc,
                                        NBTrafficLightLogicCont& tlc,
                                        EdgeVector edges) {
    // !!! Attention!
    //  No merging of the geometry to come is being done
    //  The connections are moved from one edge to another within
    //   the replacement where the edge is a node's incoming edge.

    // count the number of lanes, the speed and the id
    unsigned int nolanes = 0;
    SUMOReal speed = 0;
    int priority = 0;
    std::string id;
    sort(edges.begin(), edges.end(), NBContHelper::same_connection_edge_sorter());
    // retrieve the connected nodes
    NBEdge* tpledge = *(edges.begin());
    NBNode* from = tpledge->getFromNode();
    NBNode* to = tpledge->getToNode();
    EdgeVector::const_iterator i;
    for (i = edges.begin(); i != edges.end(); i++) {
        // some assertions
        assert((*i)->getFromNode() == from);
        assert((*i)->getToNode() == to);
        // ad the number of lanes the current edge has
        nolanes += (*i)->getNumLanes();
        // build the id
        if (i != edges.begin()) {
            id += "+";
        }
        id += (*i)->getID();
        // compute the speed
        speed += (*i)->getSpeed();
        // build the priority
        priority = MAX2(priority, (*i)->getPriority());
    }
    speed /= edges.size();
    // build the new edge
    // @bug new edge does not know about allowed vclass of old edges
    // @bug both the width and the offset are not regarded
    NBEdge* newEdge = new NBEdge(id, from, to, "", speed, nolanes, priority,
                                 NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET,
                                 tpledge->getStreetName(), tpledge->myLaneSpreadFunction);
    insert(newEdge, true);
    // replace old edge by current within the nodes
    //  and delete the old
    from->replaceOutgoing(edges, newEdge);
    to->replaceIncoming(edges, newEdge);
    // patch connections
    //  add edge2edge-information
    for (i = edges.begin(); i != edges.end(); i++) {
        EdgeVector ev = (*i)->getConnectedEdges();
        for (EdgeVector::iterator j = ev.begin(); j != ev.end(); j++) {
            newEdge->addEdge2EdgeConnection(*j);
        }
    }
    //  move lane2lane-connections
    unsigned int currLane = 0;
    for (i = edges.begin(); i != edges.end(); i++) {
        newEdge->moveOutgoingConnectionsFrom(*i, currLane);
        currLane += (*i)->getNumLanes();
    }
    // patch tl-information
    currLane = 0;
    for (i = edges.begin(); i != edges.end(); i++) {
        unsigned int noLanes = (*i)->getNumLanes();
        for (unsigned int j = 0; j < noLanes; j++, currLane++) {
            // replace in traffic lights
            tlc.replaceRemoved(*i, j, newEdge, currLane);
        }
    }
    // delete joined edges
    for (i = edges.begin(); i != edges.end(); i++) {
        erase(dc, *i);
    }
}


void
NBEdgeCont::recheckLaneSpread() {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        std::string oppositeID;
        if ((*i).first[0] == '-') {
            oppositeID = (*i).first.substr(1);
        } else {
            oppositeID = "-" + (*i).first;
        }
        if (myEdges.find(oppositeID) != myEdges.end()) {
            (*i).second->setLaneSpreadFunction(LANESPREAD_RIGHT);
            myEdges.find(oppositeID)->second->setLaneSpreadFunction(LANESPREAD_RIGHT);
        } else {
            (*i).second->setLaneSpreadFunction(LANESPREAD_CENTER);
        }
    }
}



// ----- other
EdgeVector
NBEdgeCont::getGeneratedFrom(const std::string& id) const {
    size_t len = id.length();
    EdgeVector ret;
    for (EdgeCont::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        std::string curr = (*i).first;
        // the next check makes it possibly faster - we don not have
        //  to compare the names
        if (curr.length() <= len) {
            continue;
        }
        // the name must be the same as the given id but something
        //  beginning with a '[' must be appended to it
        if (curr.substr(0, len) == id && curr[len] == '[') {
            ret.push_back((*i).second);
            continue;
        }
        // ok, maybe the edge is a compound made during joining of edges
        size_t pos = curr.find(id);
        // surely not
        if (pos == std::string::npos) {
            continue;
        }
        // check leading char
        if (pos > 0) {
            if (curr[pos - 1] != ']' && curr[pos - 1] != '+') {
                // actually, this is another id
                continue;
            }
        }
        if (pos + id.length() < curr.length()) {
            if (curr[pos + id.length()] != '[' && curr[pos + id.length()] != '+') {
                // actually, this is another id
                continue;
            }
        }
        ret.push_back((*i).second);
    }
    return ret;
}


void
NBEdgeCont::guessRoundabouts(std::vector<std::set<NBEdge*> > &marked) {
    // step 1: keep only those edges which have no turnarounds
    std::set<NBEdge*> candidates;
    for (EdgeCont::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        NBEdge* e = (*i).second;
        NBNode* const to = e->getToNode();
        if (e->getTurnDestination() == 0 && to->getConnectionTo(e->getFromNode()) == 0) {
            candidates.insert(e);
        }
    }
    // step 2:
    std::set<NBEdge*> visited;
    for (std::set<NBEdge*>::const_iterator i = candidates.begin(); i != candidates.end(); ++i) {
        std::set<NBEdge*> loopEdges;
        // start with a random edge, keep it as "begin"
        NBEdge* begin = (*i);
        if (find(visited.begin(), visited.end(), begin) != visited.end()) {
            // already seen
            continue;
        }
        NBEdge* e = (*i);
        // loop over connected edges (using always the leftmost one)
        bool noLoop = false;
        do {
            visited.insert(e);
            EdgeVector edges = e->getToNode()->getEdges();
            if (edges.size() < 2) {
                noLoop = true;
                break;
            }
            sort(edges.begin(), edges.end(), NBContHelper::edge_by_junction_angle_sorter(e->getToNode()));
            EdgeVector::const_iterator me = find(edges.begin(), edges.end(), e);
            NBContHelper::nextCW(edges, me);
            NBEdge* left = *me;
            loopEdges.insert(left);
            if (left == begin) {
                break;
            }
            if (find(candidates.begin(), candidates.end(), left) == candidates.end()) {
                noLoop = true;
                break;
            }
            if (find(visited.begin(), visited.end(), left) != visited.end()) {
                noLoop = true;
                break;
            }
            e = left;
        } while (true);
        // mark collected edges in the case a loop (roundabout) was found
        if (!noLoop) {
            for (std::set<NBEdge*>::const_iterator j = loopEdges.begin(); j != loopEdges.end(); ++j) {

                // disable turnarounds on incoming edges
                const EdgeVector& incoming = (*j)->getToNode()->getIncomingEdges();
                const EdgeVector& outgoing = (*j)->getToNode()->getOutgoingEdges();
                for (EdgeVector::const_iterator k = incoming.begin(); k != incoming.end(); ++k) {
                    if (loopEdges.find(*k) != loopEdges.end()) {
                        continue;
                    }
                    if ((*k)->getStep() >= NBEdge::LANES2LANES_USER) {
                        continue;
                    }
                    for (EdgeVector::const_iterator l = outgoing.begin(); l != outgoing.end(); ++l) {
                        if (loopEdges.find(*l) != loopEdges.end()) {
                            (*k)->addEdge2EdgeConnection(*l);
                        } else {
                            (*k)->removeFromConnections(*l, -1);
                        }
                    }
                }


                // let the connections to succeeding roundabout edge have a higher priority
                (*j)->setJunctionPriority((*j)->getToNode(), 1000);
            }
            marked.push_back(loopEdges);
        }
    }
}


/****************************************************************************/
