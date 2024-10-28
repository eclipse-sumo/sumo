/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    NBEdgeCont.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
///
// Storage for edges, including some functionality operating on multiple edges
/****************************************************************************/
#include <config.h>

#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <utils/geom/Boundary.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/IDSupplier.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include "NBNetBuilder.h"
#include "NBEdgeCont.h"
#include "NBNodeCont.h"
#include "NBPTLineCont.h"
#include "NBPTStop.h"
#include "NBHelpers.h"
#include "NBCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBDistrictCont.h"
#include "NBTypeCont.h"

#define JOIN_TRAM_MAX_ANGLE 10
#define JOIN_TRAM_MIN_LENGTH 3

//#define DEBUG_GUESS_ROUNDABOUT
//#define DEBUG_JOIN_TRAM
#define DEBUG_EDGE_ID ""

// ===========================================================================
// method definitions
// ===========================================================================
NBEdgeCont::NBEdgeCont(NBTypeCont& tc) :
    myTypeCont(tc),
    myVehicleClasses2Keep(0),
    myVehicleClasses2Remove(0),
    myNeedGeoTransformedPruningBoundary(false) {
}


NBEdgeCont::~NBEdgeCont() {
    clear();
}


void
NBEdgeCont::applyOptions(OptionsCont& oc) {
    // set edges dismiss/accept options
    myEdgesMinSpeed = oc.getFloat("keep-edges.min-speed");
    myRemoveEdgesAfterLoading = oc.exists("keep-edges.postload") && oc.getBool("keep-edges.postload");
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

        std::string polyPlainString = oc.getValueString(oc.isSet("keep-edges.in-boundary") ?
                                      "keep-edges.in-boundary" : "keep-edges.in-geo-boundary");
        // try interpreting the boundary like shape attribute with spaces
        bool ok = true;
        PositionVector boundaryShape = GeomConvHelper::parseShapeReporting(polyPlainString, "pruning-boundary", 0, ok, false, false);
        if (ok) {
            if (boundaryShape.size() < 2) {
                throw ProcessError(TL("Invalid boundary: need at least 2 coordinates"));
            } else if (boundaryShape.size() == 2) {
                // prunning boundary (box)
                myPruningBoundary.push_back(boundaryShape[0]);
                myPruningBoundary.push_back(Position(boundaryShape[1].x(), boundaryShape[0].y()));
                myPruningBoundary.push_back(boundaryShape[1]);
                myPruningBoundary.push_back(Position(boundaryShape[0].x(), boundaryShape[1].y()));
            } else {
                myPruningBoundary = boundaryShape;
            }
        } else {
            // maybe positions are separated by ',' instead of ' '
            std::vector<std::string> polyS = oc.getStringVector(oc.isSet("keep-edges.in-boundary") ?
                                             "keep-edges.in-boundary" : "keep-edges.in-geo-boundary");
            std::vector<double> poly;
            for (std::vector<std::string>::iterator i = polyS.begin(); i != polyS.end(); ++i) {
                poly.push_back(StringUtils::toDouble((*i))); // !!! may throw something anyhow...
            }
            if (poly.size() < 4) {
                throw ProcessError(TL("Invalid boundary: need at least 2 coordinates"));
            } else if (poly.size() % 2 != 0) {
                throw ProcessError(TL("Invalid boundary: malformed coordinate"));
            } else if (poly.size() == 4) {
                // prunning boundary (box)
                myPruningBoundary.push_back(Position(poly[0], poly[1]));
                myPruningBoundary.push_back(Position(poly[2], poly[1]));
                myPruningBoundary.push_back(Position(poly[2], poly[3]));
                myPruningBoundary.push_back(Position(poly[0], poly[3]));
            } else {
                for (std::vector<double>::iterator j = poly.begin(); j != poly.end();) {
                    double x = *j++;
                    double y = *j++;
                    myPruningBoundary.push_back(Position(x, y));
                }
            }
        }
        myNeedGeoTransformedPruningBoundary = oc.isSet("keep-edges.in-geo-boundary");
    }
}


void
NBEdgeCont::clear() {
    for (const auto& i : myEdges) {
        delete i.second;
    }
    myEdges.clear();
    for (const auto& i : myExtractedEdges) {
        delete i.second;
    }
    myExtractedEdges.clear();
    for (NBEdge* const e : myEdgeCemetery) {
        delete e;
    }
    myEdgeCemetery.clear();
}



// ----- edge access methods
bool
NBEdgeCont::insert(NBEdge* edge, bool ignorePrunning) {
    if (myEdges.count(edge->getID()) != 0) {
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
    if (!myRemoveEdgesAfterLoading) {
        // check whether the edge is a named edge to keep
        if (myEdges2Keep.size() != 0) {
            if (myEdges2Keep.count(edge->getID()) == 0) {
                // explicit whitelisting may be combined additively with other filters
                if (myVehicleClasses2Keep == 0 && myVehicleClasses2Remove == 0
                        && myTypes2Keep.size() == 0 && myTypes2Remove.size() == 0
                        && myPruningBoundary.size() == 0) {
                    return true;
                }
            } else {
                // explicit whitelisting overrides other filters
                return false;
            }
        }
        // remove edges which allow a speed below a set one (set using "keep-edges.min-speed")
        if (edge->getSpeed() < myEdgesMinSpeed) {
            return true;
        }
        // check whether the edge shall be removed because it does not allow any of the wished classes
        if (myVehicleClasses2Keep != 0 && (myVehicleClasses2Keep & edge->getPermissions()) == 0) {
            return true;
        }
        // check whether the edge shall be removed due to allowing unwished classes only
        if (myVehicleClasses2Remove != 0 && (myVehicleClasses2Remove | edge->getPermissions()) == myVehicleClasses2Remove) {
            return true;
        }
    }
    // check whether the edge is a named edge to remove
    if (myEdges2Remove.size() != 0) {
        if (myEdges2Remove.count(edge->getID()) != 0) {
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
    // check whether the edge is within the pruning boundary
    if (myPruningBoundary.size() != 0) {
        if (myNeedGeoTransformedPruningBoundary) {
            if (GeoConvHelper::getProcessing().usingGeoProjection()) {
                NBNetBuilder::transformCoordinates(myPruningBoundary, false);
            } else if (GeoConvHelper::getLoaded().usingGeoProjection()) {
                // XXX what if input file with different projections are loaded?
                for (int i = 0; i < (int) myPruningBoundary.size(); i++) {
                    GeoConvHelper::getLoaded().x2cartesian_const(myPruningBoundary[i]);
                }
            } else {
                WRITE_ERROR(TL("Cannot prune edges using a geo-boundary because no projection has been loaded"));
            }
            myNeedGeoTransformedPruningBoundary = false;
        }
        if (!(edge->getGeometry().getBoxBoundary().grow(POSITION_EPS).overlapsWith(myPruningBoundary))) {
            return true;
        } else if (!(edge->getGeometry().partialWithin(myPruningBoundary, 2 * POSITION_EPS) || edge->getGeometry().intersects(myPruningBoundary))) {
            // a more detailed check is necessary because the bounding box may be much bigger than the edge
            // @note: overlapsWith implicitly closes the edge shape but this is not wanted here
            return true;
        }
    }
    if (myTypeCont.knows(edge->getTypeID()) && myTypeCont.getEdgeTypeShallBeDiscarded(edge->getTypeID())) {
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
                return nullptr;
            }
        } else {
            return nullptr;
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
    if (edge != nullptr) {
        return edge;
    }
    // NOTE: (TODO) for multiply split edges (e.g. 15[0][0]) one could try recursion
    if ((retrieve(id + "[0]") != nullptr) && (retrieve(id + "[1]") != nullptr)) {
        // Edge was split during the netbuilding process
        if (downstream) {
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
    if (edge != nullptr) {
        return edge;
    }
    // now, we did not find it; we have to look over all possibilities
    EdgeVector hints;
    // check whether at least the hint was not splitted
    NBEdge* hintedge = retrieve(hint);
    if (hintedge == nullptr) {
        hints = getGeneratedFrom(hint);
    } else {
        hints.push_back(hintedge);
    }
    EdgeVector candidates = getGeneratedFrom(id);
    for (const NBEdge* const currHint : hints) {
        for (NBEdge* const poss_searched : candidates) {
            const NBNode* const node = incoming ? poss_searched->myTo : poss_searched->myFrom;
            const EdgeVector& cont = incoming ? node->getOutgoingEdges() : node->getIncomingEdges();
            if (find(cont.begin(), cont.end(), currHint) != cont.end()) {
                return poss_searched;
            }
        }
    }
    return nullptr;
}


NBEdge*
NBEdgeCont::retrievePossiblySplit(const std::string& id, double pos) const {
    // check whether the edge was not split, yet
    NBEdge* edge = retrieve(id);
    if (edge != nullptr) {
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
    double seen = 0;
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
        if (edge == nullptr) {
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
    return nullptr;
}


void
NBEdgeCont::erase(NBDistrictCont& dc, NBEdge* edge) {
    extract(dc, edge);
    delete edge;
}


void
NBEdgeCont::extract(NBDistrictCont& dc, NBEdge* edge, bool remember) {
    if (remember) {
        const auto& prevExtracted = myExtractedEdges.find(edge->getID());
        if (prevExtracted != myExtractedEdges.end()) {
            if (edge != prevExtracted->second) {
                myEdgeCemetery.insert(prevExtracted->second);
                prevExtracted->second = edge;
            }
        } else {
            myExtractedEdges[edge->getID()] = edge;
        }
    }
    myEdges.erase(edge->getID());
    edge->myFrom->removeEdge(edge);
    edge->myTo->removeEdge(edge);
    dc.removeFromSinksAndSources(edge);
}


void
NBEdgeCont::rename(NBEdge* edge, const std::string& newID) {
    if (myEdges.count(newID) != 0) {
        throw ProcessError(TLF("Attempt to rename edge using existing id '%'", newID));
    }
    myEdges.erase(edge->getID());
    edge->setID(newID);
    myEdges[newID] = edge;
    // update oppositeID
    if (edge->getLanes().back().oppositeID != "") {
        NBEdge* oppo = retrieve(SUMOXMLDefinitions::getEdgeIDFromLane(edge->getLanes().back().oppositeID));
        if (oppo != nullptr) {
            oppo->getLaneStruct(oppo->getNumLanes() - 1).oppositeID = edge->getLaneID(edge->getNumLanes() - 1);
        }
    }
}


// ----- explicit edge manipulation methods

void
NBEdgeCont::processSplits(NBEdge* e, std::vector<Split> splits,
                          NBNodeCont& nc, NBDistrictCont& dc, NBTrafficLightLogicCont& tlc) {
    if (splits.empty()) {
        return;
    }
    const std::string origID = e->getID();
    sort(splits.begin(), splits.end(), split_sorter());
    int maxNumLanes = e->getNumLanes();
    // compute the node positions and sort the lanes
    for (Split& split : splits) {
        sort(split.lanes.begin(), split.lanes.end());
        maxNumLanes = MAX2(maxNumLanes, (int)split.lanes.size());
    }
    // split the edge
    std::vector<int> currLanes;
    for (int l = 0; l < e->getNumLanes(); ++l) {
        currLanes.push_back(l);
    }
    if (e->getNumLanes() != (int)splits.back().lanes.size()) {
        // invalidate traffic light definitions loaded from a SUMO network
        e->getToNode()->invalidateTLS(tlc, true, true);
        // if the number of lanes changes the connections should be
        // recomputed
        e->invalidateConnections(true);
    }

    std::string firstID = "";
    double seen = 0;
    for (const Split& exp : splits) {
        assert(exp.lanes.size() != 0);
        if (exp.pos > 0 && e->getLoadedLength() + seen > exp.pos && exp.pos > seen) {
            nc.insert(exp.node);
            nc.markAsSplit(exp.node);
            //  split the edge
            const std::string idBefore = exp.idBefore == "" ? e->getID() : exp.idBefore;
            const std::string idAfter = exp.idAfter == "" ? exp.nameID : exp.idAfter;
            if (firstID == "") {
                firstID = idBefore;
            }
            const bool ok = splitAt(dc, e, exp.pos - seen, exp.node,
                                    idBefore, idAfter, e->getNumLanes(), (int) exp.lanes.size(), exp.speed);
            if (!ok) {
                WRITE_WARNINGF(TL("Error on parsing a split (edge '%')."), origID);
                return;
            }
            seen = exp.pos;
            std::vector<int> newLanes = exp.lanes;
            NBEdge* pe = retrieve(idBefore);
            NBEdge* ne = retrieve(idAfter);
            // reconnect lanes
            pe->invalidateConnections(true);
            //  new on right
            int rightMostP = currLanes[0];
            int rightMostN = newLanes[0];
            for (int l = 0; l < (int) rightMostP - (int) rightMostN; ++l) {
                pe->addLane2LaneConnection(0, ne, l, NBEdge::Lane2LaneInfoType::VALIDATED, true);
            }
            //  new on left
            int leftMostP = currLanes.back();
            int leftMostN = newLanes.back();
            for (int l = 0; l < (int) leftMostN - (int) leftMostP; ++l) {
                pe->addLane2LaneConnection(pe->getNumLanes() - 1, ne, leftMostN - l - rightMostN, NBEdge::Lane2LaneInfoType::VALIDATED, true);
            }
            //  all other connected
            for (int l = 0; l < maxNumLanes; ++l) {
                if (find(currLanes.begin(), currLanes.end(), l) == currLanes.end()) {
                    continue;
                }
                if (find(newLanes.begin(), newLanes.end(), l) == newLanes.end()) {
                    continue;
                }
                pe->addLane2LaneConnection(l - rightMostP, ne, l - rightMostN, NBEdge::Lane2LaneInfoType::VALIDATED, true);
            }
            //  if there are edges at this node which are not connected
            //  we can assume that this split was attached to an
            //  existing node. Reset all connections to let the default
            //  algorithm recompute them
            if (exp.node->getIncomingEdges().size() > 1 || exp.node->getOutgoingEdges().size() > 1) {
                for (NBEdge* in : exp.node->getIncomingEdges()) {
                    in->invalidateConnections(true);
                }
            }
            // move to next
            e = ne;
            currLanes = newLanes;
        }  else if (exp.pos == 0) {
            const int laneCountDiff = e->getNumLanes() - (int)exp.lanes.size();
            if (laneCountDiff < 0) {
                e->incLaneNo(-laneCountDiff);
            } else {
                e->decLaneNo(laneCountDiff);
            }
            currLanes = exp.lanes;
            // invalidate traffic light definition loaded from a SUMO network
            // XXX it would be preferable to reconstruct the phase definitions heuristically
            e->getFromNode()->invalidateTLS(tlc, true, true);
            if (exp.speed != -1.) {
                e->setSpeed(-1, exp.speed);
            }
        } else {
            WRITE_WARNINGF(TL("Split at '%' lies beyond the edge's length (edge '%')."), toString(exp.pos), origID);
        }
    }
    // patch lane offsets
    e = retrieve(firstID);
    if (e != nullptr) {
        if (splits.front().pos != 0) {
            // add a dummy split at the beginning to ensure correct offset
            Split start;
            start.pos = 0;
            for (int lane = 0; lane < (int)e->getNumLanes(); ++lane) {
                start.lanes.push_back(lane);
            }
            start.offset = splits.front().offset;
            start.offsetFactor = splits.front().offsetFactor;
            splits.insert(splits.begin(), start);
        }
        for (const Split& split : splits) {
            int maxLeft = split.lanes.back();
            double offset = split.offset;
            if (maxLeft < maxNumLanes) {
                if (e->getLaneSpreadFunction() == LaneSpreadFunction::RIGHT) {
                    offset += split.offsetFactor * SUMO_const_laneWidth * (maxNumLanes - 1 - maxLeft);
                } else {
                    offset += split.offsetFactor * SUMO_const_halfLaneWidth * (maxNumLanes - 1 - maxLeft);
                }
            }
            int maxRight = split.lanes.front();
            if (maxRight > 0 && e->getLaneSpreadFunction() == LaneSpreadFunction::CENTER) {
                offset -= split.offsetFactor * SUMO_const_halfLaneWidth * maxRight;
            }
            //std::cout << " processSplits " << origID << " splitOffset=" << (*i).offset << " offset=" << offset << "\n";
            if (offset != 0) {
                PositionVector g = e->getGeometry();
                g.move2side(offset);
                e->setGeometry(g);
            }
            if (e->getToNode()->getOutgoingEdges().size() != 0) {
                e = e->getToNode()->getOutgoingEdges()[0];
            }
        }
    }
}


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
                    const double speed, const double friction,
                    const int changedLeft) {
    double pos;
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
                   noLanesFirstEdge, noLanesSecondEdge, speed, friction, changedLeft);
}


bool
NBEdgeCont::splitAt(NBDistrictCont& dc,
                    NBEdge* edge, double pos, NBNode* node,
                    const std::string& firstEdgeName,
                    const std::string& secondEdgeName,
                    int noLanesFirstEdge, int noLanesSecondEdge,
                    const double speed, const double friction,
                    const int changedLeft) {
    if (firstEdgeName != edge->getID() && myEdges.count(firstEdgeName) != 0) {
        WRITE_ERRORF(TL("Could not insert edge '%' before split of edge '%'."), firstEdgeName, edge->getID());
        return false;
    }
    if (secondEdgeName == firstEdgeName || (secondEdgeName != edge->getID() && myEdges.count(secondEdgeName) != 0)) {
        WRITE_ERRORF(TL("Could not insert edge '%' after split of edge '%'."), secondEdgeName, edge->getID());
        return false;
    }
    // there must be at least some overlap between first and second edge
    assert(changedLeft > -((int)noLanesFirstEdge));
    assert(changedLeft < (int)noLanesSecondEdge);

    // build the new edges' geometries
    double geomPos = pos;
    if (edge->hasLoadedLength()) {
        geomPos *= edge->getGeometry().length() / edge->getLoadedLength();
    }
    std::pair<PositionVector, PositionVector> geoms = edge->getGeometry().splitAt(geomPos);
    // reduce inaccuracies and preserve bidi
    if (geoms.first[-1].almostSame(node->getPosition()) || edge->isBidi()) {
        geoms.first[-1] = node->getPosition();
        geoms.second[0] = node->getPosition();
    }
    // build and insert the edges
    NBEdge* one = new NBEdge(firstEdgeName, edge->myFrom, node, edge, geoms.first, noLanesFirstEdge);
    NBEdge* two = new NBEdge(secondEdgeName, node, edge->myTo, edge, geoms.second, noLanesSecondEdge);
    if (OptionsCont::getOptions().getBool("output.original-names")) {
        const std::string origID = edge->getLaneStruct(0).getParameter(SUMO_PARAM_ORIGID, edge->getID());
        if (firstEdgeName != origID) {
            one->setOrigID(origID, false);
        }
        if (secondEdgeName != origID) {
            two->setOrigID(origID, false);
        }
    }
    two->copyConnectionsFrom(edge);
    if (speed != -1.) {
        two->setSpeed(-1, speed);
    }
    if (friction != -1.) {
        two->setFriction(-1, friction);
    }
    if (edge->getDistance() != 0) {
        one->setDistance(edge->getDistance());
        two->setDistance(one->getDistance() + pos);
    }
    if (edge->hasLoadedLength()) {
        one->setLoadedLength(pos);
        two->setLoadedLength(edge->getLoadedLength() - pos);
    }
    // replace information about this edge within the nodes
    edge->myFrom->replaceOutgoing(edge, one, 0);
    edge->myTo->replaceIncoming(edge, two, 0);
    // patch tls
    for (NBTrafficLightDefinition* const tld : edge->myFrom->getControllingTLS()) {
        tld->replaceRemoved(edge, -1, one, -1, false);
    }
    for (NBTrafficLightDefinition* const tld : edge->myTo->getControllingTLS()) {
        tld->replaceRemoved(edge, -1, two, -1, true);
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
        if (!one->addLane2LaneConnection(i1, two, i2, NBEdge::Lane2LaneInfoType::COMPUTED)) {
            throw ProcessError(TL("Could not set connection!"));
        }
    }
    if (myRemoveEdgesAfterLoading) {
        if (myEdges2Keep.count(edge->getID()) != 0) {
            myEdges2Keep.insert(one->getID());
            myEdges2Keep.insert(two->getID());
        }
        if (myEdges2Remove.count(edge->getID()) != 0) {
            myEdges2Remove.insert(one->getID());
            myEdges2Remove.insert(two->getID());
        }
    }
    // erase the splitted edge
    patchRoundabouts(edge, one, two, myRoundabouts);
    patchRoundabouts(edge, one, two, myGuessedRoundabouts);
    const std::string oldID = edge->getID();
    extract(dc, edge, true);
    insert(one, true);  // duplicate id check happened earlier
    insert(two, true);  // duplicate id check happened earlier
    myEdgesSplit[edge] = {one, two};
    myWasSplit.insert(one);
    myWasSplit.insert(two);
    return true;
}


void
NBEdgeCont::patchRoundabouts(NBEdge* orig, NBEdge* part1, NBEdge* part2, std::set<EdgeSet>& roundabouts) {
    std::set<EdgeSet> addLater;
    for (std::set<EdgeSet>::iterator it = roundabouts.begin(); it != roundabouts.end(); ++it) {
        EdgeSet roundaboutSet = *it;
        if (roundaboutSet.count(orig) > 0) {
            roundaboutSet.erase(orig);
            roundaboutSet.insert(part1);
            roundaboutSet.insert(part2);
        }
        addLater.insert(roundaboutSet);
    }
    roundabouts.clear();
    roundabouts.insert(addLater.begin(), addLater.end());
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
int
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
    return (int)toRemove.size();
}


void
NBEdgeCont::splitGeometry(NBDistrictCont& dc, NBNodeCont& nc) {
    // make a copy of myEdges because splitting will modify it
    EdgeCont edges = myEdges;
    for (auto& item : edges) {
        NBEdge* edge = item.second;
        if (edge->getGeometry().size() < 3) {
            continue;
        }
        PositionVector geom = edge->getGeometry();
        const std::string id = edge->getID();
        double offset = 0;
        for (int i = 1; i < (int)geom.size() - 1; i++) {
            offset += geom[i - 1].distanceTo(geom[i]);
            std::string nodeID = id + "." + toString((int)offset);
            if (!nc.insert(nodeID, geom[i])) {
                WRITE_WARNING("Could not split geometry of edge '" + id + "' at index " + toString(i));
                continue;
            }
            NBNode* node = nc.retrieve(nodeID);
            splitAt(dc, edge, node, edge->getID(), nodeID, edge->getNumLanes(), edge->getNumLanes());
            edge = retrieve(nodeID);
        }
    }
}


void
NBEdgeCont::reduceGeometries(const double minDist) {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        (*i).second->reduceGeometry(minDist);
    }
}


void
NBEdgeCont::checkGeometries(const double maxAngle, bool fixAngle, const double minRadius, bool fix, bool fixRailways, bool silent) {
    if (maxAngle > 0 || minRadius > 0) {
        for (auto& item : myEdges) {
            if (isSidewalk(item.second->getPermissions()) || isForbidden(item.second->getPermissions())) {
                continue;
            }
            item.second->checkGeometry(maxAngle, fixAngle, minRadius, fix || (fixRailways && isRailway(item.second->getPermissions())), silent);
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
    const bool fixOppositeLengths = OptionsCont::getOptions().getBool("opposites.guess.fix-lengths");
    for (const auto& edgeIt : myEdges) {
        NBEdge* const edge = edgeIt.second;
        edge->recheckLanes();
        // check opposites
        if (edge->getNumLanes() > 0) {
            const int leftmostLane = edge->getNumLanes() - 1;
            // check oppositeID stored in other lanes
            for (int i = 0; i < leftmostLane; i++) {
                const std::string& oppositeID = edge->getLanes()[i].oppositeID;
                NBEdge* oppEdge = retrieve(oppositeID.substr(0, oppositeID.rfind("_")));
                if (oppositeID != "" && oppositeID != "-") {
                    if (edge->getLanes().back().oppositeID == "" && oppEdge != nullptr) {
                        edge->getLaneStruct(leftmostLane).oppositeID = oppositeID;
                        WRITE_WARNINGF(TL("Moving opposite lane '%' from invalid lane '%' to lane index %."), oppositeID, edge->getLaneID(i), leftmostLane);
                    } else {
                        WRITE_WARNINGF(TL("Removing opposite lane '%' for invalid lane '%'."), oppositeID, edge->getLaneID(i));
                    }
                    edge->getLaneStruct(i).oppositeID = "";
                }
            }
            const std::string& oppositeID = edge->getLanes().back().oppositeID;
            if (oppositeID != "" && oppositeID != "-") {
                NBEdge* oppEdge = retrieve(oppositeID.substr(0, oppositeID.rfind("_")));
                if (oppEdge == nullptr) {
                    WRITE_WARNINGF(TL("Removing unknown opposite lane '%' for edge '%'."), oppositeID, edge->getID());
                    edge->getLaneStruct(leftmostLane).oppositeID = "";
                    continue;
                } else if (oppEdge->getLaneID(oppEdge->getNumLanes() - 1) != oppositeID) {
                    const std::string oppEdgeLeftmost = oppEdge->getLaneID(oppEdge->getNumLanes() - 1);
                    WRITE_WARNINGF(TL("Adapting invalid opposite lane '%' for edge '%' to '%'."), oppositeID, edge->getID(), oppEdgeLeftmost);
                    edge->getLaneStruct(leftmostLane).oppositeID = oppEdgeLeftmost;
                }
                NBEdge::Lane& oppLane = oppEdge->getLaneStruct(oppEdge->getNumLanes() - 1);
                if (oppLane.oppositeID == "") {
                    const std::string leftmostID = edge->getLaneID(leftmostLane);
                    WRITE_WARNINGF(TL("Adapting missing opposite lane '%' for edge '%'."), leftmostID, oppEdge->getID());
                    oppLane.oppositeID = leftmostID;
                }
                if (fabs(oppEdge->getLoadedLength() - edge->getLoadedLength()) > NUMERICAL_EPS) {
                    if (fixOppositeLengths) {
                        const double avgLength = 0.5 * (edge->getFinalLength() + oppEdge->getFinalLength());
                        WRITE_WARNINGF(TL("Averaging edge lengths for lane '%' (length %) and edge '%' (length %)."),
                                       oppositeID, oppEdge->getLoadedLength(), edge->getID(), edge->getLoadedLength());
                        edge->setLoadedLength(avgLength);
                        oppEdge->setLoadedLength(avgLength);
                    } else {
                        WRITE_ERROR("Opposite lane '" + oppositeID + "' (length " + toString(oppEdge->getLoadedLength()) +
                                    ") differs in length from edge '" + edge->getID() + "' (length " +
                                    toString(edge->getLoadedLength()) + "). Set --opposites.guess.fix-lengths to fix this.");
                        edge->getLaneStruct(edge->getNumLanes() - 1).oppositeID = "";
                        continue;
                    }
                }
                if (oppEdge->getFromNode() != edge->getToNode() || oppEdge->getToNode() != edge->getFromNode()) {
                    WRITE_ERRORF(TL("Opposite lane '%' does not connect the same nodes as edge '%'!"), oppositeID, edge->getID());
                    edge->getLaneStruct(edge->getNumLanes() - 1).oppositeID = "";
                }
            }
        }
        // check for matching bidi lane shapes (at least for the simple case of 1-lane edges)
        const NBEdge* bidi = edge->getBidiEdge();
        if (bidi != nullptr && edge->getNumLanes() == 1 && bidi->getNumLanes() == 1 && edge->getID() < bidi->getID()) {
            edge->getLaneStruct(0).shape = bidi->getLaneStruct(0).shape.reverse();
        }

        // check for valid offset and speed
        const double startOffset = edge->isBidiRail() ? edge->getTurnDestination(true)->getEndOffset() : 0;
        int i = 0;
        for (const NBEdge::Lane& l : edge->getLanes()) {
            if (startOffset + l.endOffset > edge->getLength()) {
                WRITE_WARNINGF(TL("Invalid endOffset % at lane '%' with length % (startOffset %)."),
                               toString(l.endOffset), edge->getLaneID(i), toString(l.shape.length()), toString(startOffset));
            } else if (l.speed < 0.) {
                WRITE_WARNINGF(TL("Negative allowed speed (%) on lane '%', use --speed.minimum to prevent this."), toString(l.speed), edge->getLaneID(i));
            } else if (l.speed == 0.) {
                WRITE_WARNINGF(TL("Lane '%' has a maximum allowed speed of 0."), edge->getLaneID(i));
            }
            i++;
        }
    }
}


void
NBEdgeCont::appendTurnarounds(bool noTLSControlled, bool noFringe, bool onlyDeadends, bool onlyTurnlane, bool noGeometryLike) {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        (*i).second->appendTurnaround(noTLSControlled, noFringe, onlyDeadends, onlyTurnlane, noGeometryLike, true);
    }
}


void
NBEdgeCont::appendTurnarounds(const std::set<std::string>& ids, bool noTLSControlled) {
    for (std::set<std::string>::const_iterator it = ids.begin(); it != ids.end(); it++) {
        myEdges[*it]->appendTurnaround(noTLSControlled, false, false, false, false, false);
    }
}


void
NBEdgeCont::appendRailwayTurnarounds(const NBPTStopCont& sc) {
    std::set<std::string> stopEdgeIDs;
    for (auto& stopItem : sc.getStops()) {
        stopEdgeIDs.insert(stopItem.second->getEdgeId());
    }
    for (auto& item : myEdges) {
        NBEdge* edge = item.second;
        if (edge->isBidiRail()
                && (stopEdgeIDs.count(item.first) > 0 ||
                    stopEdgeIDs.count(edge->getTurnDestination(true)->getID()) > 0)) {
            NBEdge* to = edge->getTurnDestination(true);
            assert(to != 0);
            edge->setConnection(edge->getNumLanes() - 1,
                                to, to->getNumLanes() - 1, NBEdge::Lane2LaneInfoType::VALIDATED, false, false,
                                KEEPCLEAR_UNSPECIFIED,
                                NBEdge::UNSPECIFIED_CONTPOS, NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE,
                                SUMO_const_haltingSpeed);
        }
    }
}

void
NBEdgeCont::computeEdgeShapes(double smoothElevationThreshold) {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        (*i).second->computeEdgeShape(smoothElevationThreshold);
    }
    // equalize length of opposite edges
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        NBEdge* edge = i->second;
        const std::string& oppositeID = edge->getLanes().back().oppositeID;
        if (oppositeID != "" && oppositeID != "-") {
            NBEdge* oppEdge = retrieve(oppositeID.substr(0, oppositeID.rfind("_")));
            if (oppEdge == nullptr || oppEdge->getLaneID(oppEdge->getNumLanes() - 1) != oppositeID) {
                continue;
            }
            if (fabs(oppEdge->getLength() - edge->getLength()) > NUMERICAL_EPS) {
                double avgLength = (oppEdge->getLength() + edge->getLength()) / 2;
                edge->setAverageLengthWithOpposite(avgLength);
                oppEdge->setAverageLengthWithOpposite(avgLength);
            }
        }
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
    double speed = 0;
    int priority = -1;
    bool joinEdges = true;
    std::string id;
    sort(edges.begin(), edges.end(), NBContHelper::same_connection_edge_sorter());
    // retrieve the connected nodes
    NBEdge* tpledge = *(edges.begin());
    NBNode* from = tpledge->getFromNode();
    NBNode* to = tpledge->getToNode();
    EdgeVector::const_iterator i;
    int myPriority = (*edges.begin())->getPriority();
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
        // merged edges should have the same inherited priority
        if (myPriority == (*i)->getPriority()) {
            priority = myPriority;
        } else {
            priority = -1;
            joinEdges = false;
        }
    }
    if (joinEdges) {
        speed /= (double)edges.size();
        // build the new edge
        NBEdge* newEdge = new NBEdge(id, from, to, "", speed, NBEdge::UNSPECIFIED_FRICTION, nolanes, priority,
                                     NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET,
                                     tpledge->myLaneSpreadFunction, tpledge->getStreetName());
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
                tlc.replaceRemoved(*i, j, newEdge, currLane, true);
                tlc.replaceRemoved(*i, j, newEdge, currLane, false);
            }
        }
        // delete joined edges
        for (i = edges.begin(); i != edges.end(); i++) {
            extract(dc, *i, true);
        }
    }
}


void
NBEdgeCont::guessOpposites() {
    //@todo magic values
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        NBEdge* edge = i->second;
        edge->guessOpposite();
    }
}


void
NBEdgeCont::recheckLaneSpread() {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        NBEdge* opposite = getOppositeByID(i->first);
        if (opposite != nullptr) {
            i->second->setLaneSpreadFunction(LaneSpreadFunction::RIGHT);
            opposite->setLaneSpreadFunction(LaneSpreadFunction::RIGHT);
        } else {
            i->second->setLaneSpreadFunction(LaneSpreadFunction::CENTER);
        }
    }
}


NBEdge*
NBEdgeCont::getOppositeByID(const std::string& edgeID) const {
    const std::string oppositeID = edgeID[0] == '-' ? edgeID.substr(1) :  "-" + edgeID;
    EdgeCont::const_iterator it = myEdges.find(oppositeID);
    return it != myEdges.end() ? it->second : (NBEdge*)nullptr;
}

NBEdge*
NBEdgeCont::getByID(const std::string& edgeID) const {
    EdgeCont::const_iterator it = myEdges.find(edgeID);
    return it != myEdges.end() ? it->second : (NBEdge*)nullptr;
}

// ----- other
void
NBEdgeCont::addPostProcessConnection(const std::string& from, int fromLane, const std::string& to, int toLane, bool mayDefinitelyPass,
                                     KeepClear keepClear, double contPos, double visibility, double speed, double friction, double length,
                                     const PositionVector& customShape, bool uncontrolled, bool warnOnly,
                                     SVCPermissions permissions, bool indirectLeft, const std::string& edgeType, SVCPermissions changeLeft, SVCPermissions changeRight) {
    myConnections[from].push_back(PostProcessConnection(from, fromLane, to, toLane, mayDefinitelyPass, keepClear, contPos, visibility,
                                  speed, friction, length, customShape, uncontrolled, warnOnly, permissions, indirectLeft, edgeType, changeLeft, changeRight));
}

bool
NBEdgeCont::hasPostProcessConnection(const std::string& from, const std::string& to) {
    if (myConnections.count(from) == 0) {
        return false;
    } else {
        if (to == "") {
            // wildcard
            return true;
        }
        for (const auto& ppc : myConnections[from]) {
            if (ppc.to == to) {
                return true;
            }
        }
        return false;
    }
}

void
NBEdgeCont::recheckPostProcessConnections() {
    const bool warnOnly = OptionsCont::getOptions().exists("ignore-errors.connections") && OptionsCont::getOptions().getBool("ignore-errors.connections");
    for (const auto& item : myConnections) {
        for (std::vector<PostProcessConnection>::const_iterator i = item.second.begin(); i != item.second.end(); ++i) {
            NBEdge* from = retrievePossiblySplit((*i).from, true);
            NBEdge* to = retrievePossiblySplit((*i).to, false);
            if (from == nullptr || to == nullptr ||
                    !from->addLane2LaneConnection((*i).fromLane, to, (*i).toLane, NBEdge::Lane2LaneInfoType::USER, true, (*i).mayDefinitelyPass,
                                                  (*i).keepClear, (*i).contPos, (*i).visibility, (*i).speed, (*i).friction, (*i).customLength, (*i).customShape,
                                                  (*i).uncontrolled, (*i).permissions, (*i).indirectLeft, (*i).edgeType, (*i).changeLeft, (*i).changeRight,
                                                  true)) {
                const std::string msg = "Could not insert connection between '" + (*i).from + "' and '" + (*i).to + "' after build.";
                if (warnOnly || (*i).warnOnly) {
                    WRITE_WARNING(msg);
                } else {
                    WRITE_ERROR(msg);
                }
            }
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
            if (c.toEdge != nullptr && c.toEdge->getFromNode() != to) {
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
    SVCPermissions valid = SVCAll & ~SVC_PEDESTRIAN;
    for (EdgeCont::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        NBEdge* e = (*i).second;
        NBNode* const to = e->getToNode();
        if (e->getTurnDestination() == nullptr
                && to->getConnectionTo(e->getFromNode()) == nullptr
                && (e->getPermissions() & valid) != 0) {
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
#ifdef DEBUG_GUESS_ROUNDABOUT
        gDebugFlag1 = e->getID() == DEBUG_EDGE_ID;
#endif
        do {
#ifdef DEBUG_GUESS_ROUNDABOUT
            if (gDebugFlag1) {
                std::cout << " e=" << e->getID() << " loopEdges=" << toString(loopEdges) << "\n";
                gDebugFlag1 = true;
            }
#endif
            visited.insert(e);
            const EdgeVector& edges = e->getToNode()->getEdges();
            if ((e->getToNode()->getType() == SumoXMLNodeType::RIGHT_BEFORE_LEFT || e->getToNode()->getType() == SumoXMLNodeType::LEFT_BEFORE_RIGHT)
                    && !e->getToNode()->typeWasGuessed()) {
                doLoop = false;
#ifdef DEBUG_GUESS_ROUNDABOUT
                if (gDebugFlag1) {
                    std::cout << " rbl\n";
                }
                gDebugFlag1 = false;
#endif
                break;
            }
            if (edges.size() < 2) {
                doLoop = false;
#ifdef DEBUG_GUESS_ROUNDABOUT
                if (gDebugFlag1) {
                    std::cout << " deadend\n";
                }
                gDebugFlag1 = false;
#endif
                break;
            }
            if (e->getTurnDestination() != nullptr || e->getToNode()->getConnectionTo(e->getFromNode()) != nullptr) {
                // do not follow turn-arounds while in a (tentative) loop
                doLoop = false;
#ifdef DEBUG_GUESS_ROUNDABOUT
                if (gDebugFlag1) {
                    std::cout << " invalid turnAround e=" << e->getID() << " dest=" << Named::getIDSecure(e->getTurnDestination()) << "\n";
                }
                gDebugFlag1 = false;
#endif
                break;
            }
            EdgeVector::const_iterator me = std::find(edges.begin(), edges.end(), e);
            NBContHelper::nextCW(edges, me);
            NBEdge* left = *me;
            while ((left->getPermissions() & valid) == 0 && left != e) {
                NBContHelper::nextCW(edges, me);
                left = *me;
            }
            if (left == e) {
                // no usable continuation edge found
                doLoop = false;
#ifdef DEBUG_GUESS_ROUNDABOUT
                if (gDebugFlag1) {
                    std::cout << " noContinuation\n";
                }
                gDebugFlag1 = false;
#endif
                break;
            }
            NBContHelper::nextCW(edges, me);
            NBEdge* nextLeft = *me;
            double angle = fabs(NBHelpers::relAngle(e->getAngleAtNode(e->getToNode()), left->getAngleAtNode(e->getToNode())));
            double nextAngle = nextLeft == e ? 180 : fabs(NBHelpers::relAngle(e->getAngleAtNode(e->getToNode()), nextLeft->getAngleAtNode(e->getToNode())));
#ifdef DEBUG_GUESS_ROUNDABOUT
            if (gDebugFlag1) {
                std::cout << "   e=" << e->getID() << " left=" << left->getID() << " nextLeft=" << nextLeft->getID() << " angle=" << angle << " nextAngle=" << nextAngle << " eLength=" << e->getLength() << " lLength=" << left->getLength() << " dist=" << e->getLaneShape(0).back().distanceTo2D(left->getLaneShape(0).front()) << "\n";
            }
#endif
            if (angle >= 120
                    || (angle >= 90 &&
                        // if the edges are long or the junction shape is small we should expect roundness (low angles)
                        (MAX2(e->getLength(), left->getLength()) > 5
                         || e->getLaneShape(0).back().distanceTo2D(left->getLaneShape(0).front()) < 10
                         // there should be no straigher edge further left
                         || (nextAngle < 45)
                        ))) {
                // roundabouts do not have sharp turns (or they wouldn't be called 'round')
                // however, if the roundabout is very small then most of the roundness may be in the junction so the angle may be as high as 120
                doLoop = false;
#ifdef DEBUG_GUESS_ROUNDABOUT
                if (gDebugFlag1) {
                    std::cout << "     failed angle=" << angle << "\n";
                }
                gDebugFlag1 = false;
#endif
                break;
            }
            EdgeVector::const_iterator loopClosed = std::find(loopEdges.begin(), loopEdges.end(), left);
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
#ifdef DEBUG_GUESS_ROUNDABOUT
                    if (gDebugFlag1) {
                        std::cout << " attachments=" << attachments << "\n";
                    }
                    gDebugFlag1 = false;
#endif
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
#ifdef DEBUG_GUESS_ROUNDABOUT
            if (gDebugFlag1) {
                std::cout << " formFactor=" << formFactor(loopEdges) << "\n";
            }
#endif
            double loopLength = 0;
            for (const NBEdge* const le : loopEdges) {
                loopLength += le->getLoadedLength();
            }
            if (formFactor(loopEdges) > 0.6
                    && loopLength < OptionsCont::getOptions().getFloat("roundabouts.guess.max-length")) {
                // collected edges are marked in markRoundabouts
                EdgeSet guessed(loopEdges.begin(), loopEdges.end());
                if (loadedRoundaboutEdges.count(loopEdges.front()) != 0) {
                    if (find(myRoundabouts.begin(), myRoundabouts.end(), guessed) == myRoundabouts.end()) {
                        for (auto it = myRoundabouts.begin(); it != myRoundabouts.end(); it++) {
                            if ((*it).count(loopEdges.front()) != 0) {
                                WRITE_WARNINGF(TL("Replacing loaded roundabout '%' with '%'."), toString(*it), toString(guessed));
                                myRoundabouts.erase(it);
                                break;
                            }
                        }
                        myGuessedRoundabouts.insert(guessed);
                    }
                } else {
                    myGuessedRoundabouts.insert(guessed);
#ifdef DEBUG_GUESS_ROUNDABOUT
                    if (gDebugFlag1) {
                        std::cout << " foundRoundabout=" << toString(loopEdges) << "\n";
                    }
#endif
                }
            }
        }
#ifdef DEBUG_GUESS_ROUNDABOUT
        gDebugFlag1 = false;
#endif
    }
    return (int)myGuessedRoundabouts.size();
}


int
NBEdgeCont::extractRoundabouts() {
    std::set<NBEdge*> candidateEdges;
    for (const auto& edge : myEdges) {
        NBEdge* const e = edge.second;
        if (e->getJunctionPriority(e->getToNode()) == NBEdge::JunctionPriority::ROUNDABOUT || e->getJunctionPriority(e->getFromNode()) == NBEdge::JunctionPriority::ROUNDABOUT) {
            candidateEdges.insert(e);
        }
    }
    std::set<NBEdge*> visited;
    int extracted = 0;
    for (const auto& edgeIt : candidateEdges) {
        EdgeVector loopEdges;
        NBEdge* e = edgeIt;
        if (visited.count(e) > 0) {
            // already seen
            continue;
        }
        loopEdges.push_back(e);
        bool doLoop = true;
        //
        do {
            if (std::find(visited.begin(), visited.end(), e) != visited.end()) {
                if (loopEdges.size() > 1) {
                    addRoundabout(EdgeSet(loopEdges.begin(), loopEdges.end()));
                    ++extracted;
                }
                doLoop = false;
                break;
            }
            visited.insert(e);
            loopEdges.push_back(e);
            const EdgeVector& outgoingEdges = e->getToNode()->getOutgoingEdges();
            EdgeVector::const_iterator me = std::find_if(outgoingEdges.begin(), outgoingEdges.end(), [](const NBEdge * outgoingEdge) {
                return outgoingEdge->getJunctionPriority(outgoingEdge->getToNode()) == NBEdge::JunctionPriority::ROUNDABOUT;
            });
            if (me == outgoingEdges.end()) { // no closed loop
                doLoop = false;
            } else {
                e = *me;
            }
        } while (doLoop);
    }
    return extracted;
}


void
NBEdgeCont::cleanupRoundabouts() {
    // only loaded roundabouts are of concern here since guessing comes later
    std::set<EdgeSet> validRoundabouts;
    std::set<NBEdge*> validEdges;
    for (auto item : myEdges) {
        validEdges.insert(item.second);
    }
    for (EdgeSet roundabout : myRoundabouts) {
        EdgeSet validRoundabout;
        for (NBEdge* cand : roundabout) {
            if (validEdges.count(cand) != 0) {
                validRoundabout.insert(cand);
            }
        }
        if (validRoundabout.size() > 0) {
            validRoundabouts.insert(validRoundabout);
        }
    }
    myRoundabouts = validRoundabouts;
}


double
NBEdgeCont::formFactor(const EdgeVector& loopEdges) {
    // A circle (which maximizes area per circumference) has a formfactor of 1, non-circular shapes have a smaller value
    PositionVector points;
    for (EdgeVector::const_iterator it = loopEdges.begin(); it != loopEdges.end(); ++it) {
        points.append((*it)->getGeometry());
    }
    double circumference = points.length2D();
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
NBEdgeCont::removeRoundabout(const NBNode* node) {
    for (auto it = myRoundabouts.begin(); it != myRoundabouts.end(); ++it) {
        for (NBEdge* e : *it) {
            if (e->getToNode() == node) {
                myRoundabouts.erase(it);
                return;
            }
        }
    }
}

void
NBEdgeCont::removeRoundaboutEdges(const EdgeSet& toRemove) {
    removeRoundaboutEdges(toRemove, myRoundabouts);
    removeRoundaboutEdges(toRemove, myGuessedRoundabouts);
}

void
NBEdgeCont::removeRoundaboutEdges(const EdgeSet& toRemove, std::set<EdgeSet>& roundabouts) {
    // members of a set are constant so we have to do some tricks
    std::vector<EdgeSet> rList;
    for (const EdgeSet& r : roundabouts) {
        EdgeSet r2;
        std::set_difference(r.begin(), r.end(), toRemove.begin(), toRemove.end(), std::inserter(r2, r2.end()));
        rList.push_back(r2);
    }
    roundabouts.clear();
    roundabouts.insert(rList.begin(), rList.end());
}


void
NBEdgeCont::markRoundabouts() {
    for (const EdgeSet& roundaboutSet : getRoundabouts()) {
        for (NBEdge* const edge : roundaboutSet) {
            // disable turnarounds on incoming edges
            NBNode* const node = edge->getToNode();
            for (NBEdge* const inEdge : node->getIncomingEdges()) {
                if (roundaboutSet.count(inEdge) > 0) {
                    continue;
                }
                if (inEdge->getStep() >= NBEdge::EdgeBuildingStep::LANES2LANES_USER) {
                    continue;
                }
                if (inEdge->getTurnDestination() != nullptr) {
                    inEdge->removeFromConnections(inEdge->getTurnDestination(), -1);
                } else {
                    // also remove connections that are effecively a turnaround but
                    // where not correctly detector due to geometrical quirks
                    const std::vector<NBEdge::Connection> cons = inEdge->getConnections();
                    for (const NBEdge::Connection& con : cons) {
                        if (con.toEdge && roundaboutSet.count(con.toEdge) == 0) {
                            const double angle = fabs(NBHelpers::normRelAngle(inEdge->getAngleAtNode(node), con.toEdge->getAngleAtNode(node)));
                            if (angle > 160) {
                                inEdge->removeFromConnections(con.toEdge, -1);
                            }
                        }
                    }
                }

            }
            // let the connections to succeeding roundabout edge have a higher priority
            edge->setJunctionPriority(node, NBEdge::JunctionPriority::ROUNDABOUT);
            edge->setJunctionPriority(edge->getFromNode(), NBEdge::JunctionPriority::ROUNDABOUT);
            node->setRoundabout();
        }
    }
}


void
NBEdgeCont::generateStreetSigns() {
    for (EdgeCont::iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        NBEdge* e = i->second;
        const double offset = MAX2(0., e->getLength() - 3);
        if (e->getToNode()->isSimpleContinuation(false)) {
            // not a "real" junction?
            continue;
        }
        const SumoXMLNodeType nodeType = e->getToNode()->getType();
        switch (nodeType) {
            case SumoXMLNodeType::PRIORITY:
                // yield or major?
                if (e->getJunctionPriority(e->getToNode()) > 0) {
                    e->addSign(NBSign(NBSign::SIGN_TYPE_PRIORITY, offset));
                } else {
                    e->addSign(NBSign(NBSign::SIGN_TYPE_YIELD, offset));
                }
                break;
            case SumoXMLNodeType::PRIORITY_STOP:
                // yield or major?
                if (e->getJunctionPriority(e->getToNode()) > 0) {
                    e->addSign(NBSign(NBSign::SIGN_TYPE_PRIORITY, offset));
                } else {
                    e->addSign(NBSign(NBSign::SIGN_TYPE_STOP, offset));
                }
                break;
            case SumoXMLNodeType::ALLWAY_STOP:
                e->addSign(NBSign(NBSign::SIGN_TYPE_ALLWAY_STOP, offset));
                break;
            case SumoXMLNodeType::RIGHT_BEFORE_LEFT:
                e->addSign(NBSign(NBSign::SIGN_TYPE_RIGHT_BEFORE_LEFT, offset));
                break;
            case SumoXMLNodeType::LEFT_BEFORE_RIGHT:
                e->addSign(NBSign(NBSign::SIGN_TYPE_LEFT_BEFORE_RIGHT, offset));
                break;
            default:
                break;
        }
    }
}


int
NBEdgeCont::guessSpecialLanes(SUMOVehicleClass svc, double width, double minSpeed, double maxSpeed, bool fromPermissions, const std::string& excludeOpt,
                              NBTrafficLightLogicCont& tlc) {
    int lanesCreated = 0;
    std::vector<std::string> edges;
    if (excludeOpt != "") {
        edges = OptionsCont::getOptions().getStringVector(excludeOpt);
    }
    std::set<std::string> exclude(edges.begin(), edges.end());
    for (EdgeCont::iterator it = myEdges.begin(); it != myEdges.end(); it++) {
        NBEdge* edge = it->second;
        if (// not excluded
            exclude.count(edge->getID()) == 0
            // does not yet have a sidewalk
            && !edge->hasRestrictedLane(svc)
            && (
                // guess.from-permissions
                (fromPermissions && (edge->getPermissions() & svc) != 0)
                // guess from speed
                || (!fromPermissions && edge->getSpeed() > minSpeed && edge->getSpeed() <= maxSpeed)
            )) {
            edge->addRestrictedLane(width, svc);
            lanesCreated += 1;
            if (svc != SVC_PEDESTRIAN) {
                edge->invalidateConnections(true);
                edge->getFromNode()->invalidateOutgoingConnections(true);
                edge->getFromNode()->invalidateTLS(tlc, true, true);
                edge->getToNode()->invalidateTLS(tlc, true, true);
            }
        }
    }
    return lanesCreated;
}


void
NBEdgeCont::updateAllChangeRestrictions(SVCPermissions ignoring) {
    for (auto item : myEdges) {
        item.second->updateChangeRestrictions(ignoring);
    }
}


void
NBEdgeCont::addPrefix(const std::string& prefix) {
    // make a copy of node containers
    const auto nodeContainerCopy = myEdges;
    myEdges.clear();
    for (const auto& node : nodeContainerCopy) {
        node.second->setID(prefix + node.second->getID());
        myEdges[node.second->getID()] = node.second;
    }
}


int
NBEdgeCont::remapIDs(bool numericaIDs, bool reservedIDs, const std::string& prefix, NBPTStopCont& sc) {
    bool startGiven = !OptionsCont::getOptions().isDefault("numerical-ids.edge-start");
    if (!numericaIDs && !reservedIDs && prefix == "" && !startGiven) {
        return 0;
    }
    std::vector<std::string> avoid;
    if (startGiven) {
        avoid.push_back(toString(OptionsCont::getOptions().getInt("numerical-ids.edge-start") - 1));
    } else {
        avoid = getAllNames();
    }
    std::set<std::string> reserve;
    if (reservedIDs) {
        NBHelpers::loadPrefixedIDsFomFile(OptionsCont::getOptions().getString("reserved-ids"), "edge:", reserve);
        avoid.insert(avoid.end(), reserve.begin(), reserve.end());
    }
    IDSupplier idSupplier("", avoid);
    std::set<NBEdge*, ComparatorIdLess> toChange;
    for (EdgeCont::iterator it = myEdges.begin(); it != myEdges.end(); it++) {
        if (startGiven) {
            toChange.insert(it->second);
            continue;
        }
        if (numericaIDs) {
            try {
                StringUtils::toLong(it->first);
            } catch (NumberFormatException&) {
                toChange.insert(it->second);
            }
        }
        if (reservedIDs && reserve.count(it->first) > 0) {
            toChange.insert(it->second);
        }
    }

    std::map<std::string, std::vector<std::shared_ptr<NBPTStop> > > stopsOnEdge;
    for (const auto& item : sc.getStops()) {
        stopsOnEdge[item.second->getEdgeId()].push_back(item.second);
    }

    const bool origNames = OptionsCont::getOptions().getBool("output.original-names");
    for (NBEdge* edge : toChange) {
        myEdges.erase(edge->getID());
    }
    for (NBEdge* edge : toChange) {
        const std::string origID = edge->getID();
        if (origNames) {
            edge->setOrigID(origID, false);
        }
        edge->setID(idSupplier.getNext());
        myEdges[edge->getID()] = edge;
        for (std::shared_ptr<NBPTStop> stop : stopsOnEdge[origID]) {
            stop->setEdgeId(prefix + edge->getID(), *this);
        }
    }
    if (prefix.empty()) {
        return (int)toChange.size();
    } else {
        int renamed = 0;
        // make a copy because we will modify the map
        auto oldEdges = myEdges;
        for (auto item : oldEdges) {
            if (!StringUtils::startsWith(item.first, prefix)) {
                rename(item.second, prefix + item.first);
                renamed++;
            }
        }
        return renamed;
    }
}


void
NBEdgeCont::checkOverlap(double threshold, double zThreshold) const {
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
                const double overlap = outline1.getOverlapWith(outline2, zThreshold);
                if (overlap > threshold) {
                    WRITE_WARNINGF(TL("Edge '%' overlaps with edge '%' by %."), e1->getID(), e2->getID(), overlap);
                }
            }
        }
    }
}


void
NBEdgeCont::checkGrade(double threshold) const {
    for (EdgeCont::const_iterator it = myEdges.begin(); it != myEdges.end(); it++) {
        const NBEdge* edge = it->second;
        for (int i = 0; i < (int)edge->getNumLanes(); i++) {
            double maxJump = 0;
            const double grade = edge->getLaneShape(i).getMaxGrade(maxJump);
            if (maxJump > 0.01) {
                WRITE_WARNINGF(TL("Edge '%' has a vertical jump of %m."), edge->getID(), maxJump);
            } else if (grade > threshold) {
                WRITE_WARNINGF(TL("Edge '%' has a grade of %%."), edge->getID(), grade * 100, "%");
                break;
            }
        }
        const std::vector<NBEdge::Connection>& connections = edge->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator it_con = connections.begin(); it_con != connections.end(); ++it_con) {
            const NBEdge::Connection& c = *it_con;
            double maxJump = 0;
            const double grade = MAX2(c.shape.getMaxGrade(maxJump), c.viaShape.getMaxGrade(maxJump));
            if (maxJump > 0.01) {
                WRITE_WARNINGF(TL("Connection '%' has a vertical jump of %m."), c.getDescription(edge), maxJump);
            } else if (grade > threshold) {
                WRITE_WARNINGF(TL("Connection '%' has a grade of %%."), c.getDescription(edge), grade * 100, "%");
                break;
            }
        }
    }
}


int
NBEdgeCont::joinLanes(SVCPermissions perms) {
    int affectedEdges = 0;
    for (auto item : myEdges) {
        if (item.second->joinLanes(perms)) {
            affectedEdges++;
        }
    }
    return affectedEdges;
}


bool
NBEdgeCont::MinLaneComparatorIdLess::operator()(const std::pair<NBEdge*, int>& a, const std::pair<NBEdge*, int>& b) const {
    if (a.first->getID() == b.first->getID()) {
        return a.second < b.second;
    }
    return a.first->getID() < b.first->getID();
}

int
NBEdgeCont::joinTramEdges(NBDistrictCont& dc, NBPTStopCont& sc, NBPTLineCont& lc, double maxDist) {
    // this is different from joinSimilarEdges because there don't need to be
    // shared nodes and tram edges may be split
    std::vector<NBEdge*> tramEdges;
    std::vector<NBEdge*> targetEdges;
    for (auto item : myEdges) {
        SVCPermissions permissions = item.second->getPermissions();
        if (isTram(permissions)) {
            if (item.second->getNumLanes() == 1) {
                tramEdges.push_back(item.second);
            } else {
                WRITE_WARNINGF(TL("Not joining tram edge '%' with % lanes."), item.second->getID(), item.second->getNumLanes());
            }
        } else if ((permissions & (SVC_PASSENGER | SVC_BUS)) != 0) {
            targetEdges.push_back(item.second);
        }
    }
    if (tramEdges.empty() || targetEdges.empty()) {
        return 0;
    }
    int numJoined = 0;
    NamedRTree tramTree;
    for (NBEdge* const edge : tramEdges) {
        const Boundary& bound = edge->getGeometry().getBoxBoundary();
        float min[2] = { static_cast<float>(bound.xmin()), static_cast<float>(bound.ymin()) };
        float max[2] = { static_cast<float>(bound.xmax()), static_cast<float>(bound.ymax()) };
        tramTree.Insert(min, max, edge);
    }
    // {targetEdge, laneIndex : tramEdge}
    std::map<std::pair<NBEdge*, int>, NBEdge*, MinLaneComparatorIdLess> matches;

    for (NBEdge* const edge : targetEdges) {
        Boundary bound = edge->getGeometry().getBoxBoundary();
        bound.grow(maxDist + edge->getTotalWidth());
        float min[2] = { static_cast<float>(bound.xmin()), static_cast<float>(bound.ymin()) };
        float max[2] = { static_cast<float>(bound.xmax()), static_cast<float>(bound.ymax()) };
        std::set<const Named*> near;
        Named::StoringVisitor visitor(near);
        tramTree.Search(min, max, visitor);
        // the nearby set is actually just re-sorting according to the id to make the tests comparable
        std::set<NBEdge*, ComparatorIdLess> nearby;
        for (const Named* namedEdge : near) {
            nearby.insert(const_cast<NBEdge*>(static_cast<const NBEdge*>(namedEdge)));
        }
        for (NBEdge* const tramEdge : nearby) {
            // find a continous stretch of tramEdge that runs along one of the
            // lanes of the road edge
            const PositionVector& tramShape = tramEdge->getGeometry();
            double minEdgeDist = maxDist + 1;
            int minLane = -1;
            // find the lane where the maximum distance from the tram geometry
            // is minimal and within maxDist
            for (int i = 0; i < edge->getNumLanes(); i++) {
                double maxLaneDist = -1;
                if ((edge->getPermissions(i) & (SVC_PASSENGER | SVC_BUS)) != 0) {
                    const PositionVector& laneShape = edge->getLaneShape(i);
                    for (Position pos : laneShape) {
                        const double dist = tramShape.distance2D(pos, false);
#ifdef DEBUG_JOIN_TRAM
                        //if (edge->getID() == "106838214#1") {
                        //    std::cout << " edge=" << edge->getID() << " tramEdge=" << tramEdge->getID() << " lane=" << i << " pos=" << pos << " dist=" << dist << "\n";
                        //}
#endif
                        if (dist == GeomHelper::INVALID_OFFSET || dist > maxDist) {
                            maxLaneDist = -1;
                            break;
                        }
                        maxLaneDist = MAX2(maxLaneDist, dist);
                    }
                    if (maxLaneDist >= 0 && maxLaneDist < minEdgeDist) {
                        minEdgeDist = maxLaneDist;
                        minLane = i;
                    }
                }
            }
            if (minLane >= 0) {
                // edge could run in the wrong direction and still fit the threshold we check the angle as well
                const PositionVector& laneShape = edge->getLaneShape(minLane);
                const double offset1 = tramShape.nearest_offset_to_point2D(laneShape.front(), false);
                const double offset2 = tramShape.nearest_offset_to_point2D(laneShape.back(), false);
                Position p1 = tramShape.positionAtOffset2D(offset1);
                Position p2 = tramShape.positionAtOffset2D(offset2);
                double tramAngle = GeomHelper::legacyDegree(p1.angleTo2D(p2), true);
                bool angleOK = GeomHelper::getMinAngleDiff(tramAngle, edge->getTotalAngle()) < JOIN_TRAM_MAX_ANGLE;
                if (angleOK && offset2 > offset1) {
                    std::pair<NBEdge*, int> key = std::make_pair(edge, minLane);
                    if (matches.count(key) == 0) {
                        matches[key] = tramEdge;
                    } else {
                        WRITE_WARNINGF(TL("Ambiguous tram edges '%' and '%' for lane '%'."), matches[key]->getID(), tramEdge->getID(), edge->getLaneID(minLane));
                    }
#ifdef DEBUG_JOIN_TRAM
                    std::cout << edge->getLaneID(minLane) << " is close to tramEdge " << tramEdge->getID() << " maxLaneDist=" << minEdgeDist << " tramLength=" << tramEdge->getLength() << " edgeLength=" << edge->getLength() << " tramAngle=" << tramAngle << " edgeAngle=" << edge->getTotalAngle() << "\n";
#endif
                }
            }
        }
    }
    if (matches.size() == 0) {
        return 0;
    }
    const bool origNames = OptionsCont::getOptions().getBool("output.original-names");
    // find continous runs of matched edges for each tramEdge
    for (NBEdge* tramEdge : tramEdges) {
        std::vector<std::pair<double, std::pair<NBEdge*, int> > > roads;
        for (auto item : matches) {
            if (item.second == tramEdge) {
                NBEdge* road = item.first.first;
                int laneIndex = item.first.second;
                const PositionVector& laneShape = road->getLaneShape(laneIndex);
                double tramPos = tramEdge->getGeometry().nearest_offset_to_point2D(laneShape.front(), false);
                roads.push_back(std::make_pair(tramPos, item.first));
            }
        }
        if (roads.size() != 0) {

            sort(roads.begin(), roads.end());
#ifdef DEBUG_JOIN_TRAM
            std::cout << " tramEdge=" << tramEdge->getID() << " roads=";
            for (auto item : roads) {
                std::cout << item.second.first->getLaneID(item.second.second) << ",";
            }
            std::cout << " offsets=";
            for (auto item : roads) {
                std::cout << item.first << ",";
            }
            std::cout << "\n";
#endif
            // merge tramEdge into road lanes
            EdgeVector replacement;
            double pos = 0;
            int tramPart = 0;
            std::string tramEdgeID = tramEdge->getID();
            NBNode* tramFrom = tramEdge->getFromNode();
            PositionVector tramShape = tramEdge->getGeometry();
            const double tramLength = tramShape.length();
            EdgeVector incoming = tramFrom->getIncomingEdges();
            bool erasedLast = false;
            for (const auto& item : roads) {
                const double gap = item.first - pos;
                NBEdge* road = item.second.first;
                int laneIndex = item.second.second;
                if (gap >= JOIN_TRAM_MIN_LENGTH) {
#ifdef DEBUG_JOIN_TRAM
                    std::cout << "    splitting tramEdge=" << tramEdge->getID() << " at " << item.first << " (gap=" << gap << ")\n";
#endif
                    const std::string firstPartID = tramEdgeID + "#" + toString(tramPart++);
                    splitAt(dc, tramEdge, gap, road->getFromNode(), firstPartID, tramEdgeID, 1, 1);
                    tramEdge = retrieve(tramEdgeID); // second part;
                    NBEdge* firstPart = retrieve(firstPartID);
                    firstPart->invalidateConnections(true);
                    incoming.clear();
                    incoming.push_back(firstPart);
                    replacement.push_back(firstPart);
                }
                pos = item.first + road->getGeometry().length();
                numJoined++;
                replacement.push_back(road);
                // merge section of tramEdge into road lane
                if (road->getToNode() != tramEdge->getToNode() && (tramLength - pos) >= JOIN_TRAM_MIN_LENGTH) {
                    tramEdge->reinitNodes(road->getToNode(), tramEdge->getToNode());
                    tramEdge->setGeometry(tramShape.getSubpart(pos, tramShape.length()));
                    erasedLast = false;
#ifdef DEBUG_JOIN_TRAM
                    std::cout << "    shorted tramEdge=" << tramEdge->getID() << " (joined with roadEdge=" << road->getID() << "\n";
#endif
                } else {
#ifdef DEBUG_JOIN_TRAM
                    std::cout << "    erased tramEdge=" << tramEdge->getID() << "\n";
#endif
                    extract(dc, tramEdge, true);
                    erasedLast = true;
                }
                road->setPermissions(road->getPermissions(laneIndex) | SVC_TRAM, laneIndex);
                if (origNames) {
                    road->setOrigID(tramEdgeID, true, laneIndex);
                }
                for (NBEdge* in : incoming) {
                    if (isTram(in->getPermissions()) && !in->isConnectedTo(road)) {
                        if (in->getFromNode() != road->getFromNode()) {
                            in->reinitNodes(in->getFromNode(), road->getFromNode());
                        } else {
                            extract(dc, in, true);
#ifdef DEBUG_JOIN_TRAM
                            std::cout << "    erased incoming tramEdge=" << in->getID() << "\n";
#endif
                        }
                    }
                }
                incoming.clear();
            }
            NBEdge* lastRoad = roads.back().second.first;
            if (erasedLast) {
                // copy to avoid concurrent modification
                auto outEdges = tramEdge->getToNode()->getOutgoingEdges();
                for (NBEdge* out : outEdges) {
                    if (isTram(out->getPermissions()) && !lastRoad->isConnectedTo(out)) {
                        if (lastRoad->getToNode() != out->getToNode()) {
                            out->reinitNodes(lastRoad->getToNode(), out->getToNode());
                        } else {
                            extract(dc, out, true);
#ifdef DEBUG_JOIN_TRAM
                            std::cout << "    erased outgoing tramEdge=" << out->getID() << "\n";
#endif

                        }
                    }
                }
            } else {
                replacement.push_back(tramEdge);
            }
            // update ptstops and ptlines
            sc.replaceEdge(tramEdgeID, replacement);
            lc.replaceEdge(tramEdgeID, replacement);
        }
    }

    return numJoined;
}


EdgeVector
NBEdgeCont::getAllEdges() const {
    EdgeVector result;
    for (auto item : myEdges) {
        item.second->setNumericalID((int)result.size());
        result.push_back(item.second);
    }
    return result;
}

RouterEdgeVector
NBEdgeCont::getAllRouterEdges() const {
    EdgeVector all = getAllEdges();
    return RouterEdgeVector(all.begin(), all.end());
}

bool
NBEdgeCont::checkConsistency(const NBNodeCont& nc) {
    bool ok = true;
    for (const auto& item : myEdges) {
        NBEdge* e = item.second;
        if (nc.retrieve(e->getFromNode()->getID()) == nullptr) {
            WRITE_ERRORF(TL("Edge's '%' from-node '%' is not known."), e->getID(), e->getFromNode()->getID());
            ok = false;
        }
        if (nc.retrieve(e->getToNode()->getID()) == nullptr) {
            WRITE_ERRORF(TL("Edge's '%' to-node '%' is not known."), e->getID(), e->getToNode()->getID());
            ok = false;
        }

    }
    return ok;
}


void
NBEdgeCont::fixSplitCustomLength() {
    for (auto item : myEdges) {
        NBEdge* e = item.second;
        if (e->hasLoadedLength() && myWasSplit.count(e) != 0) {
            // subtract half the length of the longest incoming / outgoing connection
            double maxLengthOut = 0;
            for (const NBEdge::Connection& c : e->getConnections()) {
                maxLengthOut = MAX2(maxLengthOut, c.length + c.viaLength);
            }
            double maxLengthIn = 0;
            for (const NBEdge* in : e->getIncomingEdges()) {
                for (const NBEdge::Connection& c : in->getConnectionsFromLane(-1, e, -1)) {
                    maxLengthIn = MAX2(maxLengthIn, c.length + c.viaLength);
                }
            }
            e->setLoadedLength(MAX2(POSITION_EPS, e->getLoadedLength() - (maxLengthIn + maxLengthOut) / 2));
        }
    }
}

void
NBEdgeCont::computeAngles() {
    for (auto item : myEdges) {
        item.second->computeAngle();
    }
}


std::set<std::string>
NBEdgeCont::getUsedTypes() const {
    std::set<std::string> result;
    for (auto item : myEdges) {
        if (item.second->getTypeID() != "") {
            result.insert(item.second->getTypeID());
        }
    }
    return result;
}


int
NBEdgeCont::removeEdgesBySpeed(NBDistrictCont& dc) {
    EdgeSet toRemove;
    for (auto item : myEdges) {
        NBEdge* edge = item.second;
        // remove edges which allow a speed below a set one (set using "keep-edges.min-speed")
        if (edge->getSpeed() < myEdgesMinSpeed) {
            toRemove.insert(edge);
        }
    }
    int numRemoved = 0;
    for (NBEdge* edge : toRemove) {
        // explicit whitelist overrides removal
        if (myEdges2Keep.size() == 0 || myEdges2Keep.count(edge->getID()) == 0) {
            extract(dc, edge);
            numRemoved++;
        }
    }
    return numRemoved;
}


int
NBEdgeCont::removeEdgesByPermissions(NBDistrictCont& dc) {
    EdgeSet toRemove;
    for (auto item : myEdges) {
        NBEdge* edge = item.second;
        // check whether the edge shall be removed because it does not allow any of the wished classes
        if (myVehicleClasses2Keep != 0 && (myVehicleClasses2Keep & edge->getPermissions()) == 0) {
            toRemove.insert(edge);
        }
        // check whether the edge shall be removed due to allowing unwished classes only
        if (myVehicleClasses2Remove != 0 && (myVehicleClasses2Remove | edge->getPermissions()) == myVehicleClasses2Remove) {
            toRemove.insert(edge);
        }
    }
    int numRemoved = 0;
    for (NBEdge* edge : toRemove) {
        // explicit whitelist overrides removal
        if (myEdges2Keep.size() == 0 || myEdges2Keep.count(edge->getID()) == 0) {
            extract(dc, edge);
            numRemoved++;
        }
    }
    return numRemoved;
}


int
NBEdgeCont::removeLanesByWidth(NBDistrictCont& dc, const double minWidth) {
    EdgeSet toRemove;
    for (auto item : myEdges) {
        NBEdge* const edge = item.second;
        std::vector<int> indices;
        int idx = 0;
        for (const auto& lane : edge->getLanes()) {
            if (lane.width != NBEdge::UNSPECIFIED_WIDTH && lane.width < minWidth) {
                indices.push_back(idx);
            }
            idx++;
        }
        if ((int)indices.size() == edge->getNumLanes()) {
            toRemove.insert(edge);
        } else {
            for (const int i : indices) {
                edge->deleteLane(i, false, true);
            }
        }
    }
    int numRemoved = 0;
    for (NBEdge* edge : toRemove) {
        // explicit whitelist overrides removal
        if (myEdges2Keep.size() == 0 || myEdges2Keep.count(edge->getID()) == 0) {
            extract(dc, edge);
            numRemoved++;
        }
    }
    return numRemoved;
}


/****************************************************************************/
