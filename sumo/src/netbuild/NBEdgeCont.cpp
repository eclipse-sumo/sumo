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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/geom/Boundary.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/common/IDSupplier.h>
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
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NBEdgeCont::NBEdgeCont(NBTypeCont& tc) :
    myTypeCont(tc),
    myEdgesSplit(0),
    myVehicleClasses2Keep(0),
    myVehicleClasses2Remove(0),
    myNeedGeoTransformedPrunningBoundary(false) {
}


NBEdgeCont::~NBEdgeCont() {
    clear();
}


void
NBEdgeCont::applyOptions(OptionsCont& oc) {
    // set edges dismiss/accept options
    myEdgesMinSpeed = oc.getFloat("keep-edges.min-speed");
    myRemoveEdgesAfterJoining = oc.exists("keep-edges.postload") && oc.getBool("keep-edges.postload");
    // we possibly have to load the edges to keep/remove
    if (oc.isSet("keep-edges.input-file")) {
        NBHelpers::loadEdgesFromFile(oc.getString("keep-edges.input-file"), myEdges2Keep);
    }
    if (oc.isSet("remove-edges.input-file")) {
        NBHelpers::loadEdgesFromFile(oc.getString("remove-edges.input-file"), myEdges2Remove);
    }
    if (oc.isSet("keep-edges.explicit")) {
        const std::vector<std::string> edges = oc.getStringVector("keep-edges.explicit");
        myEdges2Keep.insert(edges.begin(), edges.end());
    }
    if (oc.isSet("remove-edges.explicit")) {
        const std::vector<std::string> edges = oc.getStringVector("remove-edges.explicit");
        myEdges2Remove.insert(edges.begin(), edges.end());
    }
    if (oc.exists("keep-edges.by-vclass") && oc.isSet("keep-edges.by-vclass")) {
        myVehicleClasses2Keep = parseVehicleClasses(oc.getStringVector("keep-edges.by-vclass"));
    }
    if (oc.exists("remove-edges.by-vclass") && oc.isSet("remove-edges.by-vclass")) {
        myVehicleClasses2Remove = parseVehicleClasses(oc.getStringVector("remove-edges.by-vclass"));
    }
    if (oc.exists("keep-edges.by-type") && oc.isSet("keep-edges.by-type")) {
        const std::vector<std::string> types = oc.getStringVector("keep-edges.by-type");
        myTypes2Keep.insert(types.begin(), types.end());
    }
    if (oc.exists("remove-edges.by-type") && oc.isSet("remove-edges.by-type")) {
        const std::vector<std::string> types = oc.getStringVector("remove-edges.by-type");
        myTypes2Remove.insert(types.begin(), types.end());
    }

    if (oc.isSet("keep-edges.in-boundary") || oc.isSet("keep-edges.in-geo-boundary")) {
        std::vector<std::string> polyS = oc.getStringVector(oc.isSet("keep-edges.in-boundary") ?
                                         "keep-edges.in-boundary" : "keep-edges.in-geo-boundary");
        // !!! throw something if length<4 || length%2!=0?
        std::vector<SUMOReal> poly;
        for (std::vector<std::string>::iterator i = polyS.begin(); i != polyS.end(); ++i) {
            poly.push_back(TplConvert::_2SUMOReal((*i).c_str())); // !!! may throw something anyhow...
        }
        if (poly.size() < 4) {
            throw ProcessError("Invalid boundary: need at least 2 coordinates");
        } else if (poly.size() % 2 != 0) {
            throw ProcessError("Invalid boundary: malformed coordinate");
        } else if (poly.size() == 4) {
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
        myNeedGeoTransformedPrunningBoundary = oc.isSet("keep-edges.in-geo-boundary");
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
NBEdgeCont::ignoreFilterMatch(NBEdge* edge) {
    // remove edges which allow a speed below a set one (set using "keep-edges.min-speed")
    if (edge->getSpeed() < myEdgesMinSpeed) {
        return true;
    }
    // check whether the edge is a named edge to keep
    if (!myRemoveEdgesAfterJoining && myEdges2Keep.size() != 0) {
        if (find(myEdges2Keep.begin(), myEdges2Keep.end(), edge->getID()) == myEdges2Keep.end()) {
            // explicit whitelisting may be combined additively with other filters
            if (myVehicleClasses2Keep == 0 && myVehicleClasses2Remove == 0
                    && myTypes2Keep.size() == 0 && myTypes2Remove.size() == 0
                    && myPrunningBoundary.size() == 0) {
                return true;
            }
        } else {
            // explicit whitelisting overrides other filters
            return false;
        }
    }
    // check whether the edge is a named edge to remove
    if (myEdges2Remove.size() != 0) {
        if (find(myEdges2Remove.begin(), myEdges2Remove.end(), edge->getID()) != myEdges2Remove.end()) {
            return true;
        }
    }
    // check whether the edge shall be removed because it does not allow any of the wished classes
    if (myVehicleClasses2Keep != 0 && (myVehicleClasses2Keep & edge->getPermissions()) == 0) {
        return true;
    }
    // check whether the edge shall be removed due to allowing unwished classes only
    if (myVehicleClasses2Remove != 0 && (myVehicleClasses2Remove | edge->getPermissions()) == myVehicleClasses2Remove) {
        return true;
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
        if (myNeedGeoTransformedPrunningBoundary) {
            if (GeoConvHelper::getProcessing().usingGeoProjection()) {
                NBNetBuilder::transformCoordinates(myPrunningBoundary, false);
            } else if (GeoConvHelper::getLoaded().usingGeoProjection()) {
                // XXX what if input file with different projections are loaded?
                for (int i = 0; i < (int) myPrunningBoundary.size(); i++) {
                    GeoConvHelper::getLoaded().x2cartesian_const(myPrunningBoundary[i]);
                }
            } else {
                WRITE_ERROR("Cannot prune edges using a geo-boundary because no projection has been loaded");
            }
            myNeedGeoTransformedPrunningBoundary = false;
        }
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

// FIXME: This can't work
/*
NBEdge*
NBEdgeCont::retrievePossiblySplit(const std::string& id, bool downstream) const {
    NBEdge* edge = retrieve(id);
    if (edge == 0) {
        return 0;
    }
    const EdgeVector* candidates = downstream ? &edge->getToNode()->getOutgoingEdges() : &edge->getFromNode()->getIncomingEdges();
    while (candidates->size() == 1) {
        const std::string& nextID = candidates->front()->getID();
        if (nextID.find(id) != 0 || nextID.size() <= id.size() + 1 || (nextID[id.size()] != '.' && nextID[id.size()] != '-')) {
            break;
        }
        edge = candidates->front();
        candidates = downstream ? &edge->getToNode()->getOutgoingEdges() : &edge->getFromNode()->getIncomingEdges();
    }
    return edge;
}*/

NBEdge*
NBEdgeCont::retrievePossiblySplit(const std::string& id, bool downstream) const {
    NBEdge* edge = retrieve(id);
    if (edge != 0) {
        return edge;
    }
    // NOTE: (TODO) for multiply split edges (e.g. 15[0][0]) one could try recursion
    if ((retrieve(id + "[0]") != 0) && (retrieve(id + "[1]") != 0)) {
        // Edge was split during the netbuilding process
        if (downstream == true) {
            return retrieve(id + "[1]");
        } else {
            return retrieve(id + "[0]");
        }
    }
    return edge;
}


NBEdge*
NBEdgeCont::retrievePossiblySplit(const std::string& id, const std::string& hint, bool incoming) const {
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
NBEdgeCont::retrievePossiblySplit(const std::string& id, SUMOReal pos) const {
    // check whether the edge was not split, yet
    NBEdge* edge = retrieve(id);
    if (edge != 0) {
        return edge;
    }
    int maxLength = 0;
    std::string tid = id + "[";
    for (EdgeCont::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        if ((*i).first.find(tid) == 0) {
            maxLength = MAX2(maxLength, (int)(*i).first.length());
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
            if ((int)cid.length() + 3 < maxLength) {
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


void
NBEdgeCont::rename(NBEdge* edge, const std::string& newID) {
    if (myEdges.count(newID) != 0) {
        throw ProcessError("Attempt to rename edge using existing id '" + newID + "'");
    }
    myEdges.erase(edge->getID());
    edge->setID(newID);
    myEdges[newID] = edge;
}


// ----- explicit edge manipulation methods
bool
NBEdgeCont::splitAt(NBDistrictCont& dc, NBEdge* edge, NBNode* node) {
    return splitAt(dc, edge, node, edge->getID() + "[0]", edge->getID() + "[1]",
                   (int) edge->myLanes.size(), (int) edge->myLanes.size());
}


bool
NBEdgeCont::splitAt(NBDistrictCont& dc, NBEdge* edge, NBNode* node,
                    const std::string& firstEdgeName,
                    const std::string& secondEdgeName,
                    int noLanesFirstEdge, int noLanesSecondEdge,
                    const SUMOReal speed,
                    const int changedLeft) {
    SUMOReal pos;
    pos = edge->getGeometry().nearest_offset_to_point2D(node->getPosition());
    if (pos <= 0) {
        pos = GeomHelper::nearest_offset_on_line_to_point2D(
                  edge->myFrom->getPosition(), edge->myTo->getPosition(),
                  node->getPosition());
    }
    if (pos <= 0 || pos + POSITION_EPS > edge->getGeometry().length()) {
        return false;
    }
    return splitAt(dc, edge, pos, node, firstEdgeName, secondEdgeName,
                   noLanesFirstEdge, noLanesSecondEdge, speed, changedLeft);
}


bool
NBEdgeCont::splitAt(NBDistrictCont& dc,
                    NBEdge* edge, SUMOReal pos, NBNode* node,
                    const std::string& firstEdgeName,
                    const std::string& secondEdgeName,
                    int noLanesFirstEdge, int noLanesSecondEdge,
                    const SUMOReal speed,
                    const int changedLeft
                   ) {
    // there must be at least some overlap between first and second edge
    assert(changedLeft > -((int)noLanesFirstEdge));
    assert(changedLeft < (int)noLanesSecondEdge);

    // build the new edges' geometries
    std::pair<PositionVector, PositionVector> geoms =
        edge->getGeometry().splitAt(pos);
    if (geoms.first[-1] != node->getPosition()) {
        geoms.first.pop_back();
        geoms.first.push_back(node->getPosition());
    }

    if (geoms.second[0] != node->getPosition()) {
        geoms.second[0] = node->getPosition();
    }
    // build and insert the edges
    NBEdge* one = new NBEdge(firstEdgeName, edge->myFrom, node, edge, geoms.first, noLanesFirstEdge);
    NBEdge* two = new NBEdge(secondEdgeName, node, edge->myTo, edge, geoms.second, noLanesSecondEdge);
    two->copyConnectionsFrom(edge);
    if (speed != -1.) {
        two->setSpeed(-1, speed);
    }
    // replace information about this edge within the nodes
    edge->myFrom->replaceOutgoing(edge, one, 0);
    edge->myTo->replaceIncoming(edge, two, 0);
    // patch tls
    std::set<NBTrafficLightDefinition*> fromTLS = edge->myFrom->getControllingTLS();
    for (std::set<NBTrafficLightDefinition*>::iterator i = fromTLS.begin(); i != fromTLS.end(); ++i) {
        (*i)->replaceRemoved(edge, -1, one, -1);
    }
    std::set<NBTrafficLightDefinition*> toTLS = edge->myTo->getControllingTLS();
    for (std::set<NBTrafficLightDefinition*>::iterator i = toTLS.begin(); i != toTLS.end(); ++i) {
        (*i)->replaceRemoved(edge, -1, two, -1);
    }
    // the edge is now occuring twice in both nodes...
    //  clean up
    edge->myFrom->removeDoubleEdges();
    edge->myTo->removeDoubleEdges();
    // add connections from the first to the second edge
    // there will be as many connections as there are lanes on the second edge
    // by default lanes will be added / discontinued on the right side
    // (appropriate for highway on-/off-ramps)
    const int offset = (int)one->getNumLanes() - (int)two->getNumLanes() + changedLeft;
    for (int i2 = 0; i2 < (int)two->getNumLanes(); i2++) {
        const int i1 = MIN2(MAX2((int)0, i2 + offset), (int)one->getNumLanes());
        if (!one->addLane2LaneConnection(i1, two, i2, NBEdge::L2L_COMPUTED)) {
            throw ProcessError("Could not set connection!");
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
    patchRoundabouts(edge, one, two, myRoundabouts);
    patchRoundabouts(edge, one, two, myGuessedRoundabouts);
    erase(dc, edge);
    insert(one, true);
    insert(two, true);
    myEdgesSplit++;
    return true;
}


void
NBEdgeCont::patchRoundabouts(NBEdge* orig, NBEdge* part1, NBEdge* part2, std::set<EdgeSet>& roundabouts) {
    for (std::set<EdgeSet>::iterator it = roundabouts.begin(); it != roundabouts.end(); ++it) {
        EdgeSet roundaboutSet = *it;
        if (roundaboutSet.count(orig) > 0) {
            roundabouts.erase(roundaboutSet);
            roundaboutSet.erase(orig);
            roundaboutSet.insert(part1);
            roundaboutSet.insert(part2);
            roundabouts.insert(roundaboutSet);
        }
    }
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


void
NBEdgeCont::reduceGeometries(const SUMOReal minDist) {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        (*i).second->reduceGeometry(minDist);
    }
}


void
NBEdgeCont::checkGeometries(const SUMOReal maxAngle, const SUMOReal minRadius, bool fix) {
    if (maxAngle > 0 || minRadius > 0) {
        for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
            (*i).second->checkGeometry(maxAngle, minRadius, fix);
        }
    }
}


// ----- processing methods
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
        NBEdge* edge = i->second;
        edge->recheckLanes();
        // check opposites
        if (edge->getNumLanes() > 0) {
            const std::string& oppositeID = edge->getLanes().back().oppositeID;
            if (oppositeID != "" && oppositeID != "-") {
                NBEdge* oppEdge = retrieve(oppositeID.substr(0, oppositeID.rfind("_")));
                if (oppEdge == 0 || oppEdge->getLaneID(oppEdge->getNumLanes() - 1) != oppositeID) {
                    WRITE_WARNING("Removing unknown opposite lane '" + oppositeID + "' for edge '" + edge->getID() + "'.");
                    edge->getLaneStruct(edge->getNumLanes() - 1).oppositeID = "";
                    continue;
                }
                if (fabs(oppEdge->getLoadedLength() - edge->getLoadedLength()) > POSITION_EPS) {
                    throw ProcessError("Opposite lane '" + oppositeID + "' (length " + toString(oppEdge->getLoadedLength()) + ") differs in length from edge '" + edge->getID() + "' (length "
                                       + toString(edge->getLoadedLength()) + ")!");
                }
                if (oppEdge->getFromNode() != edge->getToNode() || oppEdge->getToNode() != edge->getFromNode()) {
                    throw ProcessError("Opposite lane '" + oppositeID + "' does not connect the same nodes as edge '" + edge->getID() + "'!");
                }
            }
        }
    }
}


void
NBEdgeCont::appendTurnarounds(bool noTLSControlled) {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        (*i).second->appendTurnaround(noTLSControlled, true);
    }
}


void
NBEdgeCont::appendTurnarounds(const std::set<std::string>& ids, bool noTLSControlled) {
    for (std::set<std::string>::const_iterator it = ids.begin(); it != ids.end(); it++) {
        myEdges[*it]->appendTurnaround(noTLSControlled, false);
    }
}


void
NBEdgeCont::computeEdgeShapes() {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        (*i).second->computeEdgeShape();
    }
}


void
NBEdgeCont::computeLaneShapes() {
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
    int nolanes = 0;
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
    NBEdge* newEdge = new NBEdge(id, from, to, "", speed, nolanes, priority,
                                 NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET,
                                 tpledge->getStreetName(), tpledge->myLaneSpreadFunction);
    // copy lane attributes
    int laneIndex = 0;
    for (i = edges.begin(); i != edges.end(); ++i) {
        const std::vector<NBEdge::Lane>& lanes = (*i)->getLanes();
        for (int j = 0; j < (int)lanes.size(); ++j) {
            newEdge->setPermissions(lanes[j].permissions, laneIndex);
            newEdge->setLaneWidth(laneIndex, lanes[j].width);
            newEdge->setEndOffset(laneIndex, lanes[j].endOffset);
            laneIndex++;
        }
    }
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
    //  copy outgoing connections to the new edge
    int currLane = 0;
    for (i = edges.begin(); i != edges.end(); i++) {
        newEdge->moveOutgoingConnectionsFrom(*i, currLane);
        currLane += (*i)->getNumLanes();
    }
    // patch tl-information
    currLane = 0;
    for (i = edges.begin(); i != edges.end(); i++) {
        int noLanes = (*i)->getNumLanes();
        for (int j = 0; j < noLanes; j++, currLane++) {
            // replace in traffic lights
            tlc.replaceRemoved(*i, j, newEdge, currLane);
        }
    }
    // delete joined edges
    for (i = edges.begin(); i != edges.end(); i++) {
        extract(dc, *i, true);
    }
}


void
NBEdgeCont::guessOpposites() {
    //@todo magic values
    const SUMOReal distanceThreshold = 7;
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        NBEdge* edge = i->second;
        const int numLanes = edge->getNumLanes();
        if (numLanes > 0) {
            NBEdge::Lane& lastLane = edge->getLaneStruct(numLanes - 1);
            if (lastLane.oppositeID == "") {
                NBEdge* opposite = 0;
                //SUMOReal minOppositeDist = std::numeric_limits<SUMOReal>::max();
                for (EdgeVector::const_iterator j = edge->getToNode()->getOutgoingEdges().begin(); j != edge->getToNode()->getOutgoingEdges().end(); ++j) {
                    if ((*j)->getToNode() == edge->getFromNode() && !(*j)->getLanes().empty()) {
                        const SUMOReal distance = VectorHelper<SUMOReal>::maxValue(lastLane.shape.distances((*j)->getLanes().back().shape));
                        if (distance < distanceThreshold) {
                            //minOppositeDist = distance;
                            opposite = *j;
                        }
                    }
                }
                if (opposite != 0) {
                    lastLane.oppositeID = opposite->getLaneID(opposite->getNumLanes() - 1);
                }
            }
        }
    }
}


void
NBEdgeCont::recheckLaneSpread() {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        NBEdge* opposite = getOppositeByID(i->first);
        if (opposite != 0) {
            i->second->setLaneSpreadFunction(LANESPREAD_RIGHT);
            opposite->setLaneSpreadFunction(LANESPREAD_RIGHT);
        } else {
            i->second->setLaneSpreadFunction(LANESPREAD_CENTER);
        }
    }
}


NBEdge*
NBEdgeCont::getOppositeByID(const std::string& edgeID) const {
    const std::string oppositeID = edgeID[0] == '-' ? edgeID.substr(1) :  "-" + edgeID;
    EdgeCont::const_iterator it = myEdges.find(oppositeID);
    return it != myEdges.end() ? it->second : (NBEdge*)0;
}


// ----- other
void
NBEdgeCont::addPostProcessConnection(const std::string& from, int fromLane, const std::string& to, int toLane, bool mayDefinitelyPass, bool keepClear, SUMOReal contPos, SUMOReal visibility) {
    myConnections.push_back(PostProcessConnection(from, fromLane, to, toLane, mayDefinitelyPass, keepClear, contPos, visibility));
}


void
NBEdgeCont::recheckPostProcessConnections() {
    for (std::vector<PostProcessConnection>::const_iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
        NBEdge* from = retrievePossiblySplit((*i).from, true);
        NBEdge* to = retrievePossiblySplit((*i).to, false);
        if (from == 0 || to == 0 ||
                !from->addLane2LaneConnection((*i).fromLane, to, (*i).toLane, NBEdge::L2L_USER, true, (*i).mayDefinitelyPass, (*i).keepClear, (*i).contPos)) {
            WRITE_ERROR("Could not insert connection between '" + (*i).from + "' and '" + (*i).to + "' after build.");
        }
    }
    // during loading we also kept some ambiguous connections in hope they might be valid after processing
    // we need to make sure that all invalid connections are removed now
    for (EdgeCont::iterator it = myEdges.begin(); it != myEdges.end(); ++it) {
        NBEdge* edge = it->second;
        NBNode* to = edge->getToNode();
        // make a copy because we may delete connections
        std::vector<NBEdge::Connection> connections = edge->getConnections();
        for (std::vector<NBEdge::Connection>::iterator it_con = connections.begin(); it_con != connections.end(); ++it_con) {
            NBEdge::Connection& c = *it_con;
            if (c.toEdge != 0 && c.toEdge->getFromNode() != to) {
                WRITE_WARNING("Found and removed invalid connection from edge '" + edge->getID() +
                              "' to edge '" + c.toEdge->getID() + "' via junction '" + to->getID() + "'.");
                edge->removeFromConnections(c.toEdge);
            }
        }
    }
}


EdgeVector
NBEdgeCont::getGeneratedFrom(const std::string& id) const {
    int len = (int)id.length();
    EdgeVector ret;
    for (EdgeCont::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        std::string curr = (*i).first;
        // the next check makes it possibly faster - we don not have
        //  to compare the names
        if ((int)curr.length() <= len) {
            continue;
        }
        // the name must be the same as the given id but something
        //  beginning with a '[' must be appended to it
        if (curr.substr(0, len) == id && curr[len] == '[') {
            ret.push_back((*i).second);
            continue;
        }
        // ok, maybe the edge is a compound made during joining of edges
        std::string::size_type pos = curr.find(id);
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


int
NBEdgeCont::guessRoundabouts() {
    myGuessedRoundabouts.clear();
    std::set<NBEdge*> loadedRoundaboutEdges;
    for (std::set<EdgeSet>::const_iterator it = myRoundabouts.begin(); it != myRoundabouts.end(); ++it) {
        loadedRoundaboutEdges.insert(it->begin(), it->end());
    }
    // step 1: keep only those edges which have no turnarounds and which are not
    // part of a loaded roundabout
    std::set<NBEdge*> candidates;
    for (EdgeCont::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        NBEdge* e = (*i).second;
        NBNode* const to = e->getToNode();
        if (e->getTurnDestination() == 0 && to->getConnectionTo(e->getFromNode()) == 0 && loadedRoundaboutEdges.count(e) == 0) {
            candidates.insert(e);
        }
    }

    // step 2:
    std::set<NBEdge*> visited;
    for (std::set<NBEdge*>::const_iterator i = candidates.begin(); i != candidates.end(); ++i) {
        EdgeVector loopEdges;
        // start with a random edge (this doesn't have to be a roundabout edge)
        // loop over connected edges (using always the leftmost one)
        // and keep the list in loopEdges
        // continue until we loop back onto a loopEdges and extract the loop
        NBEdge* e = (*i);
        if (visited.count(e) > 0) {
            // already seen
            continue;
        }
        loopEdges.push_back(e);
        bool doLoop = true;
        do {
            visited.insert(e);
            const EdgeVector& edges = e->getToNode()->getEdges();
            if (edges.size() < 2) {
                doLoop = false;
                break;
            }
            if (e->getTurnDestination() != 0 || e->getToNode()->getConnectionTo(e->getFromNode()) != 0) {
                // do not follow turn-arounds while in a (tentative) loop
                doLoop = false;
                break;
            }
            EdgeVector::const_iterator me = find(edges.begin(), edges.end(), e);
            NBContHelper::nextCW(edges, me);
            NBEdge* left = *me;
            SUMOReal angle = fabs(NBHelpers::relAngle(e->getAngleAtNode(e->getToNode()), left->getAngleAtNode(e->getToNode())));
            if (angle >= 90) {
                // roundabouts do not have sharp turns (or they wouldn't be called 'round')
                doLoop = false;
                break;
            }
            EdgeVector::const_iterator loopClosed = find(loopEdges.begin(), loopEdges.end(), left);
            const int loopSize = (int)(loopEdges.end() - loopClosed);
            if (loopSize > 0) {
                // loop found
                if (loopSize < 3) {
                    doLoop = false; // need at least 3 edges for a roundabout
                } else if (loopSize < (int)loopEdges.size()) {
                    // remove initial edges not belonging to the loop
                    EdgeVector(loopEdges.begin() + (loopEdges.size() - loopSize), loopEdges.end()).swap(loopEdges);
                }
                // count attachments to the outside. need at least 3 or a roundabout doesn't make much sense
                int attachments = 0;
                for (EdgeVector::const_iterator j = loopEdges.begin(); j != loopEdges.end(); ++j) {
                    if ((*j)->getToNode()->getEdges().size() > 2) {
                        attachments++;
                    }
                }
                if (attachments < 3) {
                    doLoop = false;
                }
                break;
            }
            if (visited.count(left) > 0) {
                doLoop = false;
            } else {
                // keep going
                loopEdges.push_back(left);
                e = left;
            }
        } while (doLoop);
        if (doLoop) {
            // check form factor to avoid elongated shapes (circle: 1, square: ~0.79)
            if (formFactor(loopEdges) > 0.6) {
                // collected edges are marked in markRoundabouts
                myGuessedRoundabouts.insert(EdgeSet(loopEdges.begin(), loopEdges.end()));
            }
        }
    }
    return (int)myGuessedRoundabouts.size();
}


SUMOReal
NBEdgeCont::formFactor(const EdgeVector& loopEdges) {
    PositionVector points;
    for (EdgeVector::const_iterator it = loopEdges.begin(); it != loopEdges.end(); ++it) {
        points.append((*it)->getGeometry());
    }
    SUMOReal circumference = points.length2D();
    return 4 * M_PI * points.area() / (circumference * circumference);
}


const std::set<EdgeSet>
NBEdgeCont::getRoundabouts() const {
    std::set<EdgeSet> result = myRoundabouts;
    result.insert(myGuessedRoundabouts.begin(), myGuessedRoundabouts.end());
    return result;
}


void
NBEdgeCont::addRoundabout(const EdgeSet& roundabout) {
    if (roundabout.size() > 0) {
        if (find(myRoundabouts.begin(), myRoundabouts.end(), roundabout) != myRoundabouts.end()) {
            WRITE_WARNING("Ignoring duplicate roundabout: " + toString(roundabout));
        } else {
            myRoundabouts.insert(roundabout);
        }
    }
}


void
NBEdgeCont::markRoundabouts() {
    const std::set<EdgeSet> roundabouts = getRoundabouts();
    for (std::set<EdgeSet>::const_iterator it = roundabouts.begin(); it != roundabouts.end(); ++it) {
        const EdgeSet roundaboutSet = *it;
        for (std::set<NBEdge*>::const_iterator j = roundaboutSet.begin(); j != roundaboutSet.end(); ++j) {
            // disable turnarounds on incoming edges
            NBNode* node = (*j)->getToNode();
            const EdgeVector& incoming = node->getIncomingEdges();
            for (EdgeVector::const_iterator k = incoming.begin(); k != incoming.end(); ++k) {
                NBEdge* inEdge = *k;
                if (roundaboutSet.count(inEdge) > 0) {
                    continue;
                }
                if ((inEdge)->getStep() >= NBEdge::LANES2LANES_USER) {
                    continue;
                }
                inEdge->removeFromConnections(inEdge->getTurnDestination(), -1);
            }
            // let the connections to succeeding roundabout edge have a higher priority
            (*j)->setJunctionPriority(node, NBEdge::ROUNDABOUT);
            (*j)->setJunctionPriority((*j)->getFromNode(), NBEdge::ROUNDABOUT);
            node->setRoundabout();
        }
    }
}

void
NBEdgeCont::generateStreetSigns() {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        NBEdge* e = i->second;
        const SUMOReal offset = MAX2((SUMOReal)0, e->getLength() - 3);
        if (e->getToNode()->isSimpleContinuation(false)) {
            // not a "real" junction?
            continue;
        }
        const SumoXMLNodeType nodeType = e->getToNode()->getType();
        switch (nodeType) {
            case NODETYPE_PRIORITY:
                // yield or major?
                if (e->getJunctionPriority(e->getToNode()) > 0) {
                    e->addSign(NBSign(NBSign::SIGN_TYPE_PRIORITY, offset));
                } else {
                    e->addSign(NBSign(NBSign::SIGN_TYPE_YIELD, offset));
                }
                break;
            case NODETYPE_PRIORITY_STOP:
                // yield or major?
                if (e->getJunctionPriority(e->getToNode()) > 0) {
                    e->addSign(NBSign(NBSign::SIGN_TYPE_PRIORITY, offset));
                } else {
                    e->addSign(NBSign(NBSign::SIGN_TYPE_STOP, offset));
                }
                break;
            case NODETYPE_ALLWAY_STOP:
                e->addSign(NBSign(NBSign::SIGN_TYPE_ALLWAY_STOP, offset));
                break;
            case NODETYPE_RIGHT_BEFORE_LEFT:
                e->addSign(NBSign(NBSign::SIGN_TYPE_RIGHT_BEFORE_LEFT, offset));
                break;
            default:
                break;
        }
    }
}


int
NBEdgeCont::guessSidewalks(SUMOReal width, SUMOReal minSpeed, SUMOReal maxSpeed, bool fromPermissions) {
    int sidewalksCreated = 0;
    const std::vector<std::string> edges = OptionsCont::getOptions().getStringVector("sidewalks.guess.exclude");
    std::set<std::string> exclude(edges.begin(), edges.end());
    for (EdgeCont::iterator it = myEdges.begin(); it != myEdges.end(); it++) {
        NBEdge* edge = it->second;
        if (// not excluded
            exclude.count(edge->getID()) == 0
            // does not yet have a sidewalk
            && edge->getPermissions(0) != SVC_PEDESTRIAN
            && (
                // guess.from-permissions
                (fromPermissions && (edge->getPermissions() & SVC_PEDESTRIAN) != 0)
                // guess from speed
                || (!fromPermissions && edge->getSpeed() > minSpeed && edge->getSpeed() <= maxSpeed)
            )) {
            edge->addSidewalk(width);
            sidewalksCreated += 1;
        }
    }
    return sidewalksCreated;
}


int
NBEdgeCont::remapIDs(bool numericaIDs, bool reservedIDs) {
    std::vector<std::string> avoid = getAllNames();
    std::set<std::string> reserve;
    if (reservedIDs) {
        NBHelpers::loadPrefixedIDsFomFile(OptionsCont::getOptions().getString("reserved-ids"), "edge:", reserve);
        avoid.insert(avoid.end(), reserve.begin(), reserve.end());
    }
    IDSupplier idSupplier("", avoid);
    std::set<NBEdge*, Named::ComparatorIdLess> toChange;
    for (EdgeCont::iterator it = myEdges.begin(); it != myEdges.end(); it++) {
        if (numericaIDs) {
            try {
                TplConvert::_str2long(it->first);
            } catch (NumberFormatException&) {
                toChange.insert(it->second);
            }
        }
        if (reservedIDs && reserve.count(it->first) > 0) {
            toChange.insert(it->second);
        }
    }
    for (std::set<NBEdge*, Named::ComparatorIdLess>::iterator it = toChange.begin(); it != toChange.end(); ++it) {
        NBEdge* edge = *it;
        myEdges.erase(edge->getID());
        edge->setID(idSupplier.getNext());
        myEdges[edge->getID()] = edge;
    }
    return (int)toChange.size();
}


void
NBEdgeCont::checkOverlap(SUMOReal threshold, SUMOReal zThreshold) const {
    for (EdgeCont::const_iterator it = myEdges.begin(); it != myEdges.end(); it++) {
        const NBEdge* e1 = it->second;
        Boundary b1 = e1->getGeometry().getBoxBoundary();
        b1.grow(e1->getTotalWidth());
        PositionVector outline1 = e1->getCCWBoundaryLine(*e1->getFromNode());
        outline1.append(e1->getCCWBoundaryLine(*e1->getToNode()));
        // check is symmetric. only check once per pair
        for (EdgeCont::const_iterator it2 = it; it2 != myEdges.end(); it2++) {
            const NBEdge* e2 = it2->second;
            if (e1 == e2) {
                continue;
            }
            Boundary b2 = e2->getGeometry().getBoxBoundary();
            b2.grow(e2->getTotalWidth());
            if (b1.overlapsWith(b2)) {
                PositionVector outline2 = e2->getCCWBoundaryLine(*e2->getFromNode());
                outline2.append(e2->getCCWBoundaryLine(*e2->getToNode()));
                const SUMOReal overlap = outline1.getOverlapWith(outline2, zThreshold);
                if (overlap > threshold) {
                    WRITE_WARNING("Edge '" + e1->getID() + "' overlaps with edge '" + e2->getID() + "' by " + toString(overlap) + ".");
                }
            }
        }
    }
}


void
NBEdgeCont::checkGrade(SUMOReal threshold) const {
    for (EdgeCont::const_iterator it = myEdges.begin(); it != myEdges.end(); it++) {
        const NBEdge* edge = it->second;
        for (int i = 0; i < (int)edge->getNumLanes(); i++) {
            const SUMOReal grade = edge->getLaneShape(i).getMaxGrade();
            if (grade > threshold) {
                WRITE_WARNING("Edge '" + edge->getID() + "' has a grade of " + toString(grade * 100) + "%.");
                break;
            }
        }
        const std::vector<NBEdge::Connection>& connections = edge->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator it_con = connections.begin(); it_con != connections.end(); ++it_con) {
            const NBEdge::Connection& c = *it_con;
            const SUMOReal grade = MAX2(c.shape.getMaxGrade(), c.viaShape.getMaxGrade());
            if (grade > threshold) {
                WRITE_WARNING("Connection '" + c.getDescription(edge) + "' has a grade of " + toString(grade * 100) + "%.");
                break;
            }
        }
    }
}

/****************************************************************************/
