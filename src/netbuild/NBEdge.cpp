/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBEdge.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Methods for the representation of a single edge
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
#include <algorithm>
#include "NBEdgeCont.h"
#include "NBNode.h"
#include "NBNodeCont.h"
#include "NBContHelper.h"
#include "NBHelpers.h"
#include "NBTrafficLightDefinition.h"
#include <cmath>
#include <iomanip>
#include "NBTypeCont.h"
#include <utils/geom/GeomHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>
#include "NBEdge.h"
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>

//#define DEBUG_CONNECTION_GUESSING
//#define DEBUG_ANGLES
//#define DEBUG_NODE_BORDER
#define DEBUGCOND (getID() == "disabled")
#define DEBUGCOND2(obj) ((obj != 0 && (obj)->getID() == "disabled"))

// ===========================================================================
// static members
// ===========================================================================
const double NBEdge::UNSPECIFIED_WIDTH = -1;
const double NBEdge::UNSPECIFIED_OFFSET = 0;
const double NBEdge::UNSPECIFIED_SPEED = -1;
const double NBEdge::UNSPECIFIED_CONTPOS = -1;
const double NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE = -1;

const double NBEdge::UNSPECIFIED_SIGNAL_OFFSET = -1;
const double NBEdge::UNSPECIFIED_LOADED_LENGTH = -1;
const double NBEdge::ANGLE_LOOKAHEAD = 10.0;
const int NBEdge::UNSPECIFIED_INTERNAL_LANE_INDEX = -1;

// ===========================================================================
// method definitions
// ===========================================================================
std::string
NBEdge::Connection::getInternalLaneID() const {
    return id + "_" + toString(internalLaneIndex);
}


std::string
NBEdge::Connection::getDescription(const NBEdge* parent) const {
    return Named::getIDSecure(parent) + "_" + toString(fromLane) + "->" + Named::getIDSecure(toEdge) + "_" + toString(toLane);
}


NBEdge::Connection::Connection(int fromLane_, NBEdge* toEdge_, int toLane_) :
    fromLane(fromLane_),
    toEdge(toEdge_),
    toLane(toLane_),
    tlLinkNo(-1),
    mayDefinitelyPass(false),
    keepClear(true),
    contPos(UNSPECIFIED_CONTPOS),
    visibility(UNSPECIFIED_VISIBILITY_DISTANCE),
    speed(UNSPECIFIED_SPEED),
    id(toEdge_ == 0 ? "" : toEdge->getFromNode()->getID()),
    haveVia(false),
    internalLaneIndex(UNSPECIFIED_INTERNAL_LANE_INDEX),
    uncontrolled(false) {
}


NBEdge::Connection::Connection(int fromLane_, NBEdge* toEdge_, int toLane_, bool mayDefinitelyPass_, bool keepClear_, double contPos_,
                               double visibility_, double speed_, bool haveVia_, bool uncontrolled_, const PositionVector& customShape_) :
    fromLane(fromLane_),
    toEdge(toEdge_),
    toLane(toLane_),
    tlLinkNo(-1),
    mayDefinitelyPass(mayDefinitelyPass_),
    keepClear(keepClear_),
    contPos(contPos_),
    visibility(visibility_),
    speed(speed_),
    customShape(customShape_),
    id(toEdge_ == 0 ? "" : toEdge->getFromNode()->getID()),
    haveVia(haveVia_),
    internalLaneIndex(UNSPECIFIED_INTERNAL_LANE_INDEX),
    uncontrolled(uncontrolled_) {
}


NBEdge::Lane::Lane(NBEdge* e, const std::string& origID_) :
    speed(e->getSpeed()),
    permissions(SVCAll),
    preferred(0),
    endOffset(e->getEndOffset()), width(e->getLaneWidth()),
    accelRamp(false),
    connectionsDone(false) {
    if (origID_ != "") {
        setParameter(SUMO_PARAM_ORIGID, origID_);
    }
}


/* -------------------------------------------------------------------------
 * NBEdge::ToEdgeConnectionsAdder-methods
 * ----------------------------------------------------------------------- */
void
NBEdge::ToEdgeConnectionsAdder::execute(const int lane, const int virtEdge) {
    // check
    assert((int)myTransitions.size() > virtEdge);
    // get the approached edge
    NBEdge* succEdge = myTransitions[virtEdge];
    std::vector<int> lanes;

    // check whether the currently regarded, approached edge has already
    //  a connection starting at the edge which is currently being build
    std::map<NBEdge*, std::vector<int> >::iterator i = myConnections.find(succEdge);
    if (i != myConnections.end()) {
        // if there were already lanes assigned, get them
        lanes = (*i).second;
    }

    // check whether the current lane was already used to connect the currently
    //  regarded approached edge
    std::vector<int>::iterator j = find(lanes.begin(), lanes.end(), lane);
    if (j == lanes.end()) {
        // if not, add it to the list
        lanes.push_back(lane);
    }
    // set information about connecting lanes
    myConnections[succEdge] = lanes;
}



/* -------------------------------------------------------------------------
 * NBEdge::MainDirections-methods
 * ----------------------------------------------------------------------- */
NBEdge::MainDirections::MainDirections(const EdgeVector& outgoing,
                                       NBEdge* parent, NBNode* to, int indexOfStraightest) {
    if (outgoing.size() == 0) {
        return;
    }
    // check whether the right turn has a higher priority
    assert(outgoing.size() > 0);
    const LinkDirection straightestDir = to->getDirection(parent, outgoing[indexOfStraightest]);
#ifdef DEBUG_CONNECTION_GUESSING
    if (DEBUGCOND2(parent)) {
        std::cout << " MainDirections edge=" << parent->getID() << " straightest=" << outgoing[indexOfStraightest]->getID() << " dir=" << toString(straightestDir) << "\n";
    }
#endif
    if (NBNode::isTrafficLight(to->getType()) &&
            (straightestDir == LINKDIR_STRAIGHT || straightestDir == LINKDIR_PARTLEFT || straightestDir == LINKDIR_PARTRIGHT)) {
        myDirs.push_back(MainDirections::DIR_FORWARD);
        return;
    }
    if (outgoing[0]->getJunctionPriority(to) == 1) {
        myDirs.push_back(MainDirections::DIR_RIGHTMOST);
    }
    // check whether the left turn has a higher priority
    if (outgoing.back()->getJunctionPriority(to) == 1) {
        // ok, the left turn belongs to the higher priorised edges on the junction
        //  let's check, whether it has also a higher priority (lane number/speed)
        //  than the current
        EdgeVector tmp(outgoing);
        sort(tmp.begin(), tmp.end(), NBContHelper::edge_similar_direction_sorter(parent));
        if (outgoing.back()->getPriority() > tmp[0]->getPriority()) {
            myDirs.push_back(MainDirections::DIR_LEFTMOST);
        } else {
            if (outgoing.back()->getNumLanes() > tmp[0]->getNumLanes()) {
                myDirs.push_back(MainDirections::DIR_LEFTMOST);
            }
        }
    }
    // check whether the forward direction has a higher priority
    //  try to get the forward direction
    EdgeVector tmp(outgoing);
    sort(tmp.begin(), tmp.end(), NBContHelper::edge_similar_direction_sorter(parent));
    NBEdge* edge = *(tmp.begin());
    // check whether it has a higher priority and is going straight
    if (edge->getJunctionPriority(to) == 1 && to->getDirection(parent, edge) == LINKDIR_STRAIGHT) {
        myDirs.push_back(MainDirections::DIR_FORWARD);
    }
}


NBEdge::MainDirections::~MainDirections() {}


bool
NBEdge::MainDirections::empty() const {
    return myDirs.empty();
}


bool
NBEdge::MainDirections::includes(Direction d) const {
    return find(myDirs.begin(), myDirs.end(), d) != myDirs.end();
}


/* -------------------------------------------------------------------------
 * NBEdge::connections_relative_edgelane_sorter-methods
 * ----------------------------------------------------------------------- */
int
NBEdge::connections_relative_edgelane_sorter::operator()(const Connection& c1, const Connection& c2) const {
    if (c1.toEdge != c2.toEdge) {
        return NBContHelper::relative_outgoing_edge_sorter(myEdge)(c1.toEdge, c2.toEdge);
    }
    return c1.toLane < c2.toLane;
}


/* -------------------------------------------------------------------------
 * NBEdge-methods
 * ----------------------------------------------------------------------- */
NBEdge::NBEdge(const std::string& id, NBNode* from, NBNode* to,
               std::string type, double speed, int nolanes,
               int priority, double laneWidth, double offset,
               const std::string& streetName,
               LaneSpreadFunction spread) :
    Named(StringUtils::convertUmlaute(id)),
    myStep(INIT),
    myType(StringUtils::convertUmlaute(type)),
    myFrom(from), myTo(to),
    myStartAngle(0), myEndAngle(0), myTotalAngle(0),
    myPriority(priority), mySpeed(speed),
    myTurnDestination(0),
    myPossibleTurnDestination(0),
    myFromJunctionPriority(-1), myToJunctionPriority(-1),
    myLaneSpreadFunction(spread), myEndOffset(offset), myLaneWidth(laneWidth),
    myLoadedLength(UNSPECIFIED_LOADED_LENGTH),
    myAmInnerEdge(false), myAmMacroscopicConnector(false),
    myStreetName(streetName),
    mySignalOffset(UNSPECIFIED_SIGNAL_OFFSET) {
    init(nolanes, false, "");
}


NBEdge::NBEdge(const std::string& id, NBNode* from, NBNode* to,
               std::string type, double speed, int nolanes,
               int priority, double laneWidth, double offset,
               PositionVector geom,
               const std::string& streetName,
               const std::string& origID,
               LaneSpreadFunction spread, bool tryIgnoreNodePositions) :
    Named(StringUtils::convertUmlaute(id)),
    myStep(INIT),
    myType(StringUtils::convertUmlaute(type)),
    myFrom(from), myTo(to),
    myStartAngle(0), myEndAngle(0), myTotalAngle(0),
    myPriority(priority), mySpeed(speed),
    myTurnDestination(0),
    myPossibleTurnDestination(0),
    myFromJunctionPriority(-1), myToJunctionPriority(-1),
    myGeom(geom), myLaneSpreadFunction(spread), myEndOffset(offset), myLaneWidth(laneWidth),
    myLoadedLength(UNSPECIFIED_LOADED_LENGTH),
    myAmInnerEdge(false), myAmMacroscopicConnector(false),
    myStreetName(streetName),
    mySignalOffset(UNSPECIFIED_SIGNAL_OFFSET) {
    init(nolanes, tryIgnoreNodePositions, origID);
}


NBEdge::NBEdge(const std::string& id, NBNode* from, NBNode* to, NBEdge* tpl, const PositionVector& geom, int numLanes) :
    Named(StringUtils::convertUmlaute(id)),
    myStep(INIT),
    myType(tpl->getTypeID()),
    myFrom(from), myTo(to),
    myStartAngle(0), myEndAngle(0), myTotalAngle(0),
    myPriority(tpl->getPriority()), mySpeed(tpl->getSpeed()),
    myTurnDestination(0),
    myPossibleTurnDestination(0),
    myFromJunctionPriority(-1), myToJunctionPriority(-1),
    myGeom(geom),
    myLaneSpreadFunction(tpl->getLaneSpreadFunction()),
    myEndOffset(tpl->getEndOffset()),
    myLaneWidth(tpl->getLaneWidth()),
    myLoadedLength(UNSPECIFIED_LOADED_LENGTH),
    myAmInnerEdge(false),
    myAmMacroscopicConnector(false),
    myStreetName(tpl->getStreetName()),
    mySignalOffset(to == tpl->myTo ? tpl->mySignalOffset : UNSPECIFIED_SIGNAL_OFFSET) {
    init(numLanes > 0 ? numLanes : tpl->getNumLanes(), myGeom.size() > 0, "");
    for (int i = 0; i < getNumLanes(); i++) {
        const int tplIndex = MIN2(i, tpl->getNumLanes() - 1);
        setSpeed(i, tpl->getLaneSpeed(tplIndex));
        setPermissions(tpl->getPermissions(tplIndex), i);
        setLaneWidth(i, tpl->myLanes[tplIndex].width);
        myLanes[i].updateParameter(tpl->myLanes[tplIndex].getMap());;
        if (to == tpl->myTo) {
            setEndOffset(i, tpl->myLanes[tplIndex].endOffset);
        }
    }
}


void
NBEdge::reinit(NBNode* from, NBNode* to, const std::string& type,
               double speed, int nolanes, int priority,
               PositionVector geom, double laneWidth, double offset,
               const std::string& streetName,
               LaneSpreadFunction spread,
               bool tryIgnoreNodePositions) {
    if (myFrom != from) {
        myFrom->removeEdge(this, false);
    }
    if (myTo != to) {
        myTo->removeEdge(this, false);
    }
    myType = StringUtils::convertUmlaute(type);
    myFrom = from;
    myTo = to;
    myPriority = priority;
    //?myTurnDestination(0),
    //?myFromJunctionPriority(-1), myToJunctionPriority(-1),
    myGeom = geom;
    myLaneSpreadFunction = spread;
    myLoadedLength = UNSPECIFIED_LOADED_LENGTH;
    myStreetName = streetName;
    //?, myAmTurningWithAngle(0), myAmTurningOf(0),
    //?myAmInnerEdge(false), myAmMacroscopicConnector(false)

    // preserve lane-specific settings (geometry must be recomputed)
    // if new lanes are added they copy the values from the leftmost lane (if specified)
    const std::vector<Lane> oldLanes = myLanes;
    init(nolanes, tryIgnoreNodePositions, oldLanes.empty() ? "" : oldLanes[0].getParameter(SUMO_PARAM_ORIGID));
    for (int i = 0; i < (int)nolanes; ++i) {
        PositionVector newShape = myLanes[i].shape;
        myLanes[i] = oldLanes[MIN2(i, (int)oldLanes.size() - 1)];
        myLanes[i].shape = newShape;
    }
    // however, if the new edge defaults are explicityly given, they override the old settings
    if (offset != UNSPECIFIED_OFFSET) {
        setEndOffset(-1, offset);
    }
    if (laneWidth != UNSPECIFIED_WIDTH) {
        setLaneWidth(-1, laneWidth);
    }
    if (speed != UNSPECIFIED_SPEED) {
        setSpeed(-1, speed);
    }
}


void
NBEdge::reinitNodes(NBNode* from, NBNode* to) {
    // connections may still be valid
    if (from == 0 || to == 0) {
        throw ProcessError("At least one of edge's '" + myID + "' nodes is not known.");
    }
    if (myFrom != from) {
        myFrom->removeEdge(this, false);
        myFrom = from;
        myFrom->addOutgoingEdge(this);
    }
    if (myTo != to) {
        myTo->removeEdge(this, false);
        myTo = to;
        myTo->addIncomingEdge(this);
    }
    computeAngle();
}


void
NBEdge::init(int noLanes, bool tryIgnoreNodePositions, const std::string& origID) {
    if (noLanes == 0) {
        throw ProcessError("Edge '" + myID + "' needs at least one lane.");
    }
    if (myFrom == 0 || myTo == 0) {
        throw ProcessError("At least one of edge's '" + myID + "' nodes is not known.");
    }
    // revisit geometry
    //  should have at least two points at the end...
    //  and in dome cases, the node positions must be added
    myGeom.removeDoublePoints();
    if (!tryIgnoreNodePositions || myGeom.size() < 2) {
        if (myGeom.size() == 0) {
            myGeom.push_back(myFrom->getPosition());
            myGeom.push_back(myTo->getPosition());
        } else {
            myGeom.push_back_noDoublePos(myTo->getPosition());
            myGeom.push_front_noDoublePos(myFrom->getPosition());
        }
    }
    if (myGeom.size() < 2) {
        myGeom.clear();
        myGeom.push_back(myFrom->getPosition());
        myGeom.push_back(myTo->getPosition());
    }
    if (myGeom.size() == 2 && myGeom[0] == myGeom[1]) {
        WRITE_WARNING("Edge's '" + myID + "' from- and to-node are at the same position.");
        myGeom[1].add(Position(POSITION_EPS, POSITION_EPS));
    }
    //
    myFrom->addOutgoingEdge(this);
    myTo->addIncomingEdge(this);
    // prepare container
    myLength = myFrom->getPosition().distanceTo(myTo->getPosition());
    assert(myGeom.size() >= 2);
    if ((int)myLanes.size() > noLanes) {
        // remove connections starting at the removed lanes
        for (int lane = noLanes; lane < (int)myLanes.size(); ++lane) {
            removeFromConnections(0, lane, -1);
        }
        // remove connections targeting the removed lanes
        const EdgeVector& incoming = myFrom->getIncomingEdges();
        for (EdgeVector::const_iterator i = incoming.begin(); i != incoming.end(); i++) {
            for (int lane = noLanes; lane < (int)myLanes.size(); ++lane) {
                (*i)->removeFromConnections(this, -1, lane);
            }
        }
    }
    myLanes.clear();
    for (int i = 0; i < noLanes; i++) {
        myLanes.push_back(Lane(this, origID));
    }
    computeLaneShapes();
    computeAngle();

#ifdef DEBUG_CONNECTION_GUESSING
    if (DEBUGCOND) {
        std::cout << "init edge=" << getID() << "\n";
        for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
            std::cout << "  conn " << getID() << "_" << (*i).fromLane << " to " << Named::getIDSecure((*i).toEdge) << "_" << (*i).toLane << "\n";
            (*i).shape.mirrorX();
            (*i).viaShape.mirrorX();
        }
    }
#endif
}


NBEdge::~NBEdge() {}


// -----------  Applying offset
void
NBEdge::reshiftPosition(double xoff, double yoff) {
    myGeom.add(xoff, yoff, 0);
    for (int i = 0; i < (int)myLanes.size(); i++) {
        myLanes[i].shape.add(xoff, yoff, 0);
    }
    for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
        (*i).customShape.add(xoff, yoff, 0);
    }
    computeAngle(); // update angles because they are numerically sensitive (especially where based on centroids)
}


void
NBEdge::mirrorX() {
    myGeom.mirrorX();
    for (int i = 0; i < (int)myLanes.size(); i++) {
        myLanes[i].shape.mirrorX();
    }
    for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
        (*i).shape.mirrorX();
        (*i).viaShape.mirrorX();
    }
    computeAngle(); // update angles because they are numerically sensitive (especially where based on centroids)
}


// ----------- Edge geometry access and computation
const PositionVector
NBEdge::getInnerGeometry() const {
    return myGeom.getSubpartByIndex(1, (int)myGeom.size() - 2);
}


bool
NBEdge::hasDefaultGeometry() const {
    return myGeom.size() == 2 && hasDefaultGeometryEndpoints();
}


bool
NBEdge::hasDefaultGeometryEndpoints() const {
    return myGeom.front() == myFrom->getPosition() &&
           myGeom.back() == myTo->getPosition();
}


bool
NBEdge::hasDefaultGeometryEndpointAtNode(const NBNode* node) const {
    // do not extend past the node position
    if (node == myFrom) {
        return myGeom.front() == node->getPosition();
    } else {
        assert(node == myTo);
        return myGeom.back() == node->getPosition();
    }
}

void
NBEdge::setGeometry(const PositionVector& s, bool inner) {
    Position begin = myGeom.front(); // may differ from node position
    Position end = myGeom.back(); // may differ from node position
    myGeom = s;
    if (inner) {
        myGeom.insert(myGeom.begin(), begin);
        myGeom.push_back(end);
    }
    computeLaneShapes();
    computeAngle();
}


void
NBEdge::extendGeometryAtNode(const NBNode* node, double maxExtent) {
    //std::cout << "extendGeometryAtNode edge=" << getID() << " node=" << node->getID() << " nodePos=" << node->getPosition() << " extent=" << maxExtent << " geom=" << myGeom;
    if (node == myFrom) {
        myGeom.extrapolate(maxExtent, true);
        double offset = myGeom.nearest_offset_to_point2D(node->getPosition());
        //std::cout << " geom2=" << myGeom << " offset=" << offset;
        if (offset != GeomHelper::INVALID_OFFSET) {
            myGeom = myGeom.getSubpart2D(MIN2(offset, myGeom.length2D() - 2 * POSITION_EPS), myGeom.length2D());
        }
    } else {
        assert(node == myTo);
        myGeom.extrapolate(maxExtent, false, true);
        double offset = myGeom.nearest_offset_to_point2D(node->getPosition());
        //std::cout << " geom2=" << myGeom << " offset=" << offset;
        if (offset != GeomHelper::INVALID_OFFSET) {
            myGeom = myGeom.getSubpart2D(0, MAX2(offset, 2 * POSITION_EPS));
        }
    }
    //std::cout << " geom3=" << myGeom << "\n";
}


void
NBEdge::shortenGeometryAtNode(const NBNode* node, double reduction) {
    //std::cout << "shortenGeometryAtNode edge=" << getID() << " node=" << node->getID() << " nodePos=" << node->getPosition() << " reduction=" << reduction << " geom=" << myGeom;
    reduction = MIN2(reduction, myGeom.length2D() - 2 * POSITION_EPS);
    if (node == myFrom) {
        myGeom = myGeom.getSubpart2D(reduction, myGeom.length2D());
    } else {
        myGeom = myGeom.getSubpart2D(0, myGeom.length2D() - reduction);
    }
    computeLaneShapes();
    //std::cout << " geom2=" << myGeom << "\n";
}


void
NBEdge::setNodeBorder(const NBNode* node, const Position& p, const Position& p2, bool rectangularCut) {
    PositionVector border;
    if (rectangularCut) {
        const double extend = 100;
        border = myGeom.getOrthogonal(p, extend, node == myTo);
    } else {
        border.push_back(p);
        border.push_back(p2);
    }
    if (border.size() == 2) {
        double edgeWidth = 0;
        for (int i = 0; i < (int)myLanes.size(); i++) {
            edgeWidth += getLaneWidth(i);
        }
        border.extrapolate2D(getTotalWidth());
        if (node == myFrom) {
            myFromBorder = border;
        } else {
            assert(node == myTo);
            myToBorder = border;
        }
    }
#ifdef DEBUG_NODE_BORDER
    gDebugFlag1 = DEBUGCOND;
    if (DEBUGCOND) std::cout << "setNodeBorder edge=" << getID() << " node=" << node->getID()
                                 << " rect=" << rectangularCut
                                 << " p=" << p << " p2=" << p2
                                 << " border=" << border
                                 << " myGeom=" << myGeom
                                 << "\n";

#endif
}


void
NBEdge::resetNodeBorder(const NBNode* node) {
    if (node == myFrom) {
        myFromBorder.clear();
    } else {
        assert(node == myTo);
        myToBorder.clear();
    }
}


bool 
NBEdge::isBidiRail() {
    return (isRailway(getPermissions()) 
            && myLaneSpreadFunction == LANESPREAD_CENTER 
            && myPossibleTurnDestination != 0 
            && myPossibleTurnDestination->getLaneSpreadFunction() == LANESPREAD_CENTER
            && myPossibleTurnDestination->getGeometry().reverse() == getGeometry());
}


PositionVector
NBEdge::cutAtIntersection(const PositionVector& old) const {
    PositionVector shape = old;
    shape = startShapeAt(shape, myFrom, myFromBorder);
    if (shape.size() < 2) {
        // only keep the last snippet
        const double oldLength = old.length();
        shape = old.getSubpart(oldLength - 2 * POSITION_EPS, oldLength);
    }
    shape = startShapeAt(shape.reverse(), myTo, myToBorder).reverse();
    // sanity checks
    if (shape.length() < POSITION_EPS) {
        if (old.length() < 2 * POSITION_EPS) {
            shape = old;
        } else {
            const double midpoint = old.length() / 2;
            // EPS*2 because otherwhise shape has only a single point
            shape = old.getSubpart(midpoint - POSITION_EPS, midpoint + POSITION_EPS);
            assert(shape.size() >= 2);
            assert(shape.length() > 0);
        }
    } else {
        // @note If the node shapes are overlapping we may get a shape which goes in the wrong direction
        // in this case the result shape should shortened
        if (DEG2RAD(135) < fabs(GeomHelper::angleDiff(shape.beginEndAngle(), old.beginEndAngle()))) {
            // eliminate intermediate points
            PositionVector tmp;
            tmp.push_back(shape[0]);
            tmp.push_back(shape[-1]);
            // 3D geometry may be messed up since positions were extrapolated rather than interpolated due to cutting in the wrong direction
            // make the edge level with one of the intersections (try to pick one that needs to be flat as well)
            if (myTo->geometryLike()) {
                tmp[0].setz(myFrom->getPosition().z());
                tmp[1].setz(myFrom->getPosition().z());
            } else {
                tmp[0].setz(myTo->getPosition().z());
                tmp[1].setz(myTo->getPosition().z());
            }
            shape = tmp;
            if (tmp.length() < POSITION_EPS) {
                // fall back to original shape
                if (old.length() < 2 * POSITION_EPS) {
                    shape = old;
                } else {
                    const double midpoint = old.length() / 2;
                    // EPS*2 because otherwhise shape has only a single point
                    shape = old.getSubpart(midpoint - POSITION_EPS, midpoint + POSITION_EPS);
                    assert(shape.size() >= 2);
                    assert(shape.length() > 0);
                }
            } else {
                const double midpoint = shape.length() / 2;
                // cut to size and reverse
                shape = shape.getSubpart(midpoint - POSITION_EPS, midpoint + POSITION_EPS);
                if (shape.length() < POSITION_EPS) {
                    assert(false);
                    // the shape has a sharp turn near the midpoint
                }
                shape = shape.reverse();
            }
        }
    }
    return shape;
}


void
NBEdge::computeEdgeShape() {
    for (int i = 0; i < (int)myLanes.size(); i++) {
        myLanes[i].shape = cutAtIntersection(myLanes[i].shape);
    }
    // recompute edge's length as the average of lane lenghts
    double avgLength = 0;
    for (int i = 0; i < (int)myLanes.size(); i++) {
        assert(myLanes[i].shape.length() > 0);
        avgLength += myLanes[i].shape.length();
    }
    myLength = avgLength / (double) myLanes.size();
    computeAngle(); // update angles using the finalized node and lane shapes
}


PositionVector
NBEdge::startShapeAt(const PositionVector& laneShape, const NBNode* startNode, PositionVector nodeShape) const {
    if (nodeShape.size() == 0) {
        nodeShape = startNode->getShape();
        nodeShape.closePolygon();
    }
    PositionVector lb(laneShape.begin(), laneShape.begin() + 2);
    // this doesn't look reasonable @todo use lb.extrapolateFirstBy(100.0);
    lb.extrapolate(100.0);
    if (nodeShape.intersects(laneShape)) {
        // shape intersects directly
        std::vector<double> pbv = laneShape.intersectsAtLengths2D(nodeShape);
        assert(pbv.size() > 0);
        // ensure that the subpart has at least two points
        double pb = MIN2(laneShape.length2D() - POSITION_EPS - NUMERICAL_EPS, VectorHelper<double>::maxValue(pbv));
        if (pb < 0) {
            return laneShape;
        }
        PositionVector ns = laneShape.getSubpart2D(pb, laneShape.length2D());
        //PositionVector ns = pb < (laneShape.length() - POSITION_EPS) ? laneShape.getSubpart2D(pb, laneShape.length()) : laneShape;
        if (!startNode->geometryLike() || pb < 1) {
            // make "real" intersections and small intersections flat
            ns[0].setz(startNode->getPosition().z());
            // cutting and patching z-coordinate may cause steep grades which should be smoothed
            const double dZ = ns.size() >= 2 ? fabs(ns[0].z() - ns[1].z()) : 0;
            if (dZ > 0) {
                ns = ns.smoothedZFront(MIN2(ns.length2D(),
                                            dZ * 4 * OptionsCont::getOptions().getFloat("geometry.max-grade")));
            }
        }
        assert(ns.size() >= 2);
        return ns;
    } else if (nodeShape.intersects(lb)) {
        // extension of first segment intersects
        std::vector<double> pbv = lb.intersectsAtLengths2D(nodeShape);
        assert(pbv.size() > 0);
        double pb = VectorHelper<double>::maxValue(pbv);
        assert(pb >= 0);
        PositionVector result = laneShape.getSubpartByIndex(1, (int)laneShape.size() - 1);
        Position np = PositionVector::positionAtOffset2D(lb[0], lb[1], pb);
        if (!startNode->geometryLike()) {
            // make "real" intersections flat
            np.setz(startNode->getPosition().z());
        }
        result.push_front_noDoublePos(np);
        const double dZ = result.size() >= 2 ? fabs(result[0].z() - result[1].z()) : 0;
        if (dZ > 0) {
            result = result.smoothedZFront(MIN2(result.length2D(),
                                                dZ * 4 * OptionsCont::getOptions().getFloat("geometry.max-grade")));
        }
        return result;
        //if (result.size() >= 2) {
        //    return result;
        //} else {
        //    WRITE_WARNING(error + " (resulting shape is too short)");
        //    return laneShape;
        //}
    } else {
        // could not find proper intersection. Probably the edge is very short
        // and lies within nodeShape
        // @todo enable warning WRITE_WARNING(error + " (laneShape lies within nodeShape)");
        return laneShape;
    }
}


const PositionVector&
NBEdge::getLaneShape(int i) const {
    return myLanes[i].shape;
}


void
NBEdge::setLaneSpreadFunction(LaneSpreadFunction spread) {
    myLaneSpreadFunction = spread;
}


void
NBEdge::addGeometryPoint(int index, const Position& p) {
    if (index >= 0) {
        myGeom.insert_noDoublePos(myGeom.begin() + index, p);
    } else {
        myGeom.insert_noDoublePos(myGeom.end() + index, p);
    }
}


bool
NBEdge::splitGeometry(NBEdgeCont& ec, NBNodeCont& nc) {
    // check whether there any splits to perform
    if (myGeom.size() < 3) {
        return false;
    }
    // ok, split
    NBNode* newFrom = myFrom;
    NBNode* myLastNode = myTo;
    NBNode* newTo = 0;
    NBEdge* currentEdge = this;
    for (int i = 1; i < (int) myGeom.size() - 1; i++) {
        // build the node first
        if (i != (int)myGeom.size() - 2) {
            std::string nodename = myID + "_in_between#" + toString(i);
            if (!nc.insert(nodename, myGeom[i])) {
                throw ProcessError("Error on adding in-between node '" + nodename + "'.");
            }
            newTo = nc.retrieve(nodename);
        } else {
            newTo = myLastNode;
        }
        if (i == 1) {
            currentEdge->myTo->removeEdge(this);
            currentEdge->myTo = newTo;
            newTo->addIncomingEdge(currentEdge);
        } else {
            std::string edgename = myID + "[" + toString(i - 1) + "]";
            // @bug lane-specific width, speed, overall offset and restrictions are ignored
            currentEdge = new NBEdge(edgename, newFrom, newTo, myType, mySpeed, (int) myLanes.size(),
                                     myPriority, myLaneWidth, UNSPECIFIED_OFFSET, myStreetName, myLaneSpreadFunction);
            if (!ec.insert(currentEdge, true)) {
                throw ProcessError("Error on adding splitted edge '" + edgename + "'.");
            }
        }
        newFrom = newTo;
    }
    myGeom.clear();
    myGeom.push_back(myFrom->getPosition());
    myGeom.push_back(myTo->getPosition());
    myStep = INIT;
    return true;
}


void
NBEdge::reduceGeometry(const double minDist) {
    myGeom.removeDoublePoints(minDist, true);
}


void
NBEdge::checkGeometry(const double maxAngle, const double minRadius, bool fix) {
    if (myGeom.size() < 3) {
        return;
    }
    //std::cout << "checking geometry of " << getID() << " geometry = " << toString(myGeom) << "\n";
    std::vector<double> angles; // absolute segment angles
    //std::cout << "  absolute angles:";
    for (int i = 0; i < (int)myGeom.size() - 1; ++i) {
        angles.push_back(myGeom.angleAt2D(i));
        //std::cout << " " << angles.back();
    }
    //std::cout << "\n  relative angles: ";
    for (int i = 0; i < (int)angles.size() - 1; ++i) {
        const double relAngle = fabs(GeomHelper::angleDiff(angles[i], angles[i + 1]));
        //std::cout << relAngle << " ";
        if (maxAngle > 0 && relAngle > maxAngle) {
            WRITE_WARNING("Found angle of " + toString(RAD2DEG(relAngle)) + " degrees at edge '" + getID() + "', segment " + toString(i));
        }
        if (relAngle < DEG2RAD(1)) {
            continue;
        }
        if (i == 0 || i == (int)angles.size() - 2) {
            const bool start = i == 0;
            const double dist = (start ? myGeom[0].distanceTo2D(myGeom[1]) : myGeom[-2].distanceTo2D(myGeom[-1]));
            const double r = tan(0.5 * (M_PI - relAngle)) * dist;
            //std::cout << (start ? "  start" : "  end") << " length=" << dist << " radius=" << r << "  ";
            if (minRadius > 0 && r < minRadius) {
                if (fix) {
                    WRITE_MESSAGE("Removing sharp turn with radius " + toString(r) + " at the " +
                                  (start ? "start" : "end") + " of edge '" + getID() + "'.");
                    myGeom.erase(myGeom.begin() + (start ? 1 : i + 1));
                    checkGeometry(maxAngle, minRadius, fix);
                    return;
                } else {
                    WRITE_WARNING("Found sharp turn with radius " + toString(r) + " at the " +
                                  (start ? "start" : "end") + " of edge '" + getID() + "'.");
                }
            }
        }
    }
    //std::cout << "\n";
}


// ----------- Setting and getting connections
bool
NBEdge::addEdge2EdgeConnection(NBEdge* dest) {
    if (myStep == INIT_REJECT_CONNECTIONS) {
        return true;
    }
    // check whether the node was merged and now a connection between
    //  not matching edges is tried to be added
    //  This happens f.e. within the ptv VISSIM-example "Beijing"
    if (dest != 0 && myTo != dest->myFrom) {
        return false;
    }
    if (dest == 0) {
        invalidateConnections();
        myConnections.push_back(Connection(-1, dest, -1));
    } else if (find_if(myConnections.begin(), myConnections.end(), connections_toedge_finder(dest)) == myConnections.end()) {
        myConnections.push_back(Connection(-1, dest, -1));
    }
    if (myStep < EDGE2EDGES) {
        myStep = EDGE2EDGES;
    }
    return true;
}


bool
NBEdge::addLane2LaneConnection(int from, NBEdge* dest,
                               int toLane, Lane2LaneInfoType type,
                               bool mayUseSameDestination,
                               bool mayDefinitelyPass,
                               bool keepClear,
                               double contPos,
                               double visibility,
                               double speed,
                               const PositionVector& customShape) {
    if (myStep == INIT_REJECT_CONNECTIONS) {
        return true;
    }
    // check whether the node was merged and now a connection between
    //  not matching edges is tried to be added
    //  This happens f.e. within the ptv VISSIM-example "Beijing"
    if (myTo != dest->myFrom) {
        return false;
    }
    if (!addEdge2EdgeConnection(dest)) {
        return false;
    }
    return setConnection(from, dest, toLane, type, mayUseSameDestination, mayDefinitelyPass, keepClear, contPos, visibility, speed, customShape);
}


bool
NBEdge::addLane2LaneConnections(int fromLane,
                                NBEdge* dest, int toLane,
                                int no, Lane2LaneInfoType type,
                                bool invalidatePrevious,
                                bool mayDefinitelyPass) {
    if (invalidatePrevious) {
        invalidateConnections(true);
    }
    bool ok = true;
    for (int i = 0; i < no && ok; i++) {
        ok &= addLane2LaneConnection(fromLane + i, dest, toLane + i, type, false, mayDefinitelyPass);
    }
    return ok;
}


bool
NBEdge::setConnection(int lane, NBEdge* destEdge,
                      int destLane, Lane2LaneInfoType type,
                      bool mayUseSameDestination,
                      bool mayDefinitelyPass,
                      bool keepClear,
                      double contPos,
                      double visibility,
                      double speed,
                      const PositionVector& customShape) {
    if (myStep == INIT_REJECT_CONNECTIONS) {
        return false;
    }
    // some kind of a misbehaviour which may occure when the junction's outgoing
    //  edge priorities were not properly computed, what may happen due to
    //  an incomplete or not proper input
    // what happens is that under some circumstances a single lane may set to
    //  be approached more than once by the one of our lanes.
    //  This must not be!
    // we test whether it is the case and do nothing if so - the connection
    //  will be refused
    //
    if (!mayUseSameDestination && hasConnectionTo(destEdge, destLane)) {
        return false;
    }
    if (find_if(myConnections.begin(), myConnections.end(), connections_finder(lane, destEdge, destLane)) != myConnections.end()) {
        return true;
    }
    if ((int)myLanes.size() <= lane || destEdge->getNumLanes() <= (int)destLane) {
        // problem might be corrigible in post-processing
        WRITE_WARNING("Could not set connection from '" + getLaneIDInsecure(lane) + "' to '" + destEdge->getLaneIDInsecure(destLane) + "'.");
        return false;
    }
    for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end();) {
        if ((*i).toEdge == destEdge && ((*i).fromLane == -1 || (*i).toLane == -1)) {
            i = myConnections.erase(i);
        } else {
            ++i;
        }
    }
    myConnections.push_back(Connection(lane, destEdge, destLane));
    if (mayDefinitelyPass) {
        myConnections.back().mayDefinitelyPass = true;
    }
    myConnections.back().keepClear = keepClear;
    myConnections.back().contPos = contPos;
    myConnections.back().visibility = visibility;
    myConnections.back().speed = speed;
    myConnections.back().customShape = customShape;
    if (type == L2L_USER) {
        myStep = LANES2LANES_USER;
    } else {
        // check whether we have to take another look at it later
        if (type == L2L_COMPUTED) {
            // yes, the connection was set using an algorithm which requires a recheck
            myStep = LANES2LANES_RECHECK;
        } else {
            // ok, let's only not recheck it if we did no add something that has to be rechecked
            if (myStep != LANES2LANES_RECHECK) {
                myStep = LANES2LANES_DONE;
            }
        }
    }
    return true;
}


void
NBEdge::insertConnection(NBEdge::Connection connection) {
    myConnections.push_back(connection);
}


std::vector<NBEdge::Connection>
NBEdge::getConnectionsFromLane(int lane) const {
    std::vector<NBEdge::Connection> ret;
    for (std::vector<Connection>::const_iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
        if ((*i).fromLane == lane) {
            ret.push_back(*i);
        }
    }
    return ret;
}


NBEdge::Connection
NBEdge::getConnection(int fromLane, const NBEdge* to, int toLane) const {
    for (std::vector<Connection>::const_iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
        if (
            (*i).fromLane == fromLane
            && (*i).toEdge == to
            && (*i).toLane == toLane) {
            return *i;
        }
    }
    throw ProcessError("Connection from " + getID() + "_" + toString(fromLane)
                       + " to " + to->getID() + "_" + toString(toLane) + " not found");
}

NBEdge::Connection&
NBEdge::getConnectionRef(int fromLane, const NBEdge* to, int toLane) {
    for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
        if (
            (*i).fromLane == fromLane
            && (*i).toEdge == to
            && (*i).toLane == toLane) {
            return *i;
        }
    }
    throw ProcessError("Connection from " + getID() + "_" + toString(fromLane)
                       + " to " + to->getID() + "_" + toString(toLane) + " not found");
}


bool
NBEdge::hasConnectionTo(NBEdge* destEdge, int destLane, int fromLane) const {
    return destEdge != 0 && find_if(myConnections.begin(), myConnections.end(), connections_toedgelane_finder(destEdge, destLane, fromLane)) != myConnections.end();
}


bool
NBEdge::isConnectedTo(const NBEdge* e) const {
    if (e == myTurnDestination) {
        return true;
    }
    return
        find_if(myConnections.begin(), myConnections.end(), connections_toedge_finder(e))
        !=
        myConnections.end();

}


const EdgeVector*
NBEdge::getConnectedSorted() {
    // check whether connections exist and if not, use edges from the node
    EdgeVector outgoing;
    if (myConnections.size() == 0) {
        outgoing = myTo->getOutgoingEdges();
    } else {
        for (std::vector<Connection>::const_iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
            if (find(outgoing.begin(), outgoing.end(), (*i).toEdge) == outgoing.end()) {
                outgoing.push_back((*i).toEdge);
            }
        }
    }
    for (std::vector<Connection>::iterator it = myConnectionsToDelete.begin(); it != myConnectionsToDelete.end(); ++it) {
        if (it->fromLane < 0 && it->toLane < 0) {
            // found an edge that shall not be connected
            EdgeVector::iterator forbidden = find(outgoing.begin(), outgoing.end(), it->toEdge);
            if (forbidden != outgoing.end()) {
                outgoing.erase(forbidden);
            }
        }
    }
    // allocate the sorted container
    int size = (int) outgoing.size();
    EdgeVector* edges = new EdgeVector();
    edges->reserve(size);
    for (EdgeVector::const_iterator i = outgoing.begin(); i != outgoing.end(); i++) {
        NBEdge* outedge = *i;
        if (outedge != 0 && outedge != myTurnDestination) {
            edges->push_back(outedge);
        }
    }
    sort(edges->begin(), edges->end(), NBContHelper::relative_outgoing_edge_sorter(this));
    return edges;
}


EdgeVector
NBEdge::getConnectedEdges() const {
    EdgeVector ret;
    for (std::vector<Connection>::const_iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
        if (find(ret.begin(), ret.end(), (*i).toEdge) == ret.end()) {
            ret.push_back((*i).toEdge);
        }
    }
    return ret;
}


EdgeVector
NBEdge::getIncomingEdges() const {
    EdgeVector ret;
    const EdgeVector& candidates = myFrom->getIncomingEdges();
    for (EdgeVector::const_iterator i = candidates.begin(); i != candidates.end(); i++) {
        if ((*i)->isConnectedTo(this)) {
            ret.push_back(*i);
        }
    }
    return ret;
}


std::vector<int>
NBEdge::getConnectionLanes(NBEdge* currentOutgoing) const {
    std::vector<int> ret;
    if (currentOutgoing != myTurnDestination) {
        for (std::vector<Connection>::const_iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
            if ((*i).toEdge == currentOutgoing) {
                ret.push_back((*i).fromLane);
            }
        }
    }
    return ret;
}


void
NBEdge::sortOutgoingConnectionsByAngle() {
    sort(myConnections.begin(), myConnections.end(), connections_relative_edgelane_sorter(this));
}


void
NBEdge::sortOutgoingConnectionsByIndex() {
    sort(myConnections.begin(), myConnections.end(), connections_sorter);
}


void
NBEdge::remapConnections(const EdgeVector& incoming) {
    EdgeVector connected = getConnectedEdges();
    for (EdgeVector::const_iterator i = incoming.begin(); i != incoming.end(); i++) {
        NBEdge* inc = *i;
        // We have to do this
        inc->myStep = EDGE2EDGES;
        // add all connections
        for (EdgeVector::iterator j = connected.begin(); j != connected.end(); j++) {
            inc->addEdge2EdgeConnection(*j);
        }
        inc->removeFromConnections(this);
    }
}


void
NBEdge::removeFromConnections(NBEdge* toEdge, int fromLane, int toLane, bool tryLater, const bool adaptToLaneRemoval) {
    // remove from "myConnections"
    const int fromLaneRemoved = adaptToLaneRemoval && fromLane >= 0 ? fromLane : -1;
    const int toLaneRemoved = adaptToLaneRemoval && toLane >= 0 ? toLane : -1;
    for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end();) {
        Connection& c = *i;
        if ((toEdge == 0 || c.toEdge == toEdge)
                && (fromLane < 0 || c.fromLane == fromLane)
                && (toLane < 0 || c.toLane == toLane)) {
            if (myTo->isTLControlled()) {
                std::set<NBTrafficLightDefinition*> tldefs = myTo->getControllingTLS();
                for (std::set<NBTrafficLightDefinition*>::iterator it = tldefs.begin(); it != tldefs.end(); it++) {
                    (*it)->removeConnection(NBConnection(this, c.fromLane, c.toEdge, c.toLane));
                }
            }
            i = myConnections.erase(i);
            tryLater = false;
        } else {
            if (fromLaneRemoved >= 0 && c.fromLane > fromLaneRemoved) {
                if (myTo->isTLControlled()) {
                    std::set<NBTrafficLightDefinition*> tldefs = myTo->getControllingTLS();
                    for (std::set<NBTrafficLightDefinition*>::iterator it = tldefs.begin(); it != tldefs.end(); it++) {
                        for (NBConnectionVector::iterator tlcon = (*it)->getControlledLinks().begin(); tlcon != (*it)->getControlledLinks().end(); ++tlcon) {
                            NBConnection& tc = *tlcon;
                            if (tc.getTo() == c.toEdge && tc.getFromLane() == c.fromLane && tc.getToLane() == c.toLane) {
                                tc.shiftLaneIndex(this, -1);
                            }
                        }
                    }
                }
                c.fromLane--;
            }
            if (toLaneRemoved >= 0 && c.toLane > toLaneRemoved) {
                c.toLane--;
            }
            ++i;
        }
    }
    // check whether it was the turn destination
    if (myTurnDestination == toEdge && fromLane < 0) {
        myTurnDestination = 0;
    }
    if (myPossibleTurnDestination == toEdge && fromLane < 0) {
        myPossibleTurnDestination = 0;
    }
    if (tryLater) {
        myConnectionsToDelete.push_back(Connection(fromLane, toEdge, toLane));
    }
}


bool
NBEdge::removeFromConnections(NBEdge::Connection connectionToRemove) {
    // iterate over connections
    for (std::vector<Connection>::iterator i = myConnections.begin(); i !=  myConnections.end(); i++) {
        if (((*i).toEdge == connectionToRemove.toEdge) && ((*i).fromLane == connectionToRemove.fromLane) && ((*i).toLane == connectionToRemove.toLane)) {
            // remove connection
            myConnections.erase(i);
            return true;
        }
    }
    assert(false);
    return false;
}


void
NBEdge::invalidateConnections(bool reallowSetting) {
    myTurnDestination = 0;
    myConnections.clear();
    if (reallowSetting) {
        myStep = INIT;
    } else {
        myStep = INIT_REJECT_CONNECTIONS;
    }
}


void
NBEdge::replaceInConnections(NBEdge* which, NBEdge* by, int laneOff) {
    // replace in "_connectedEdges"
    for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
        if ((*i).toEdge == which) {
            (*i).toEdge = by;
            (*i).toLane += laneOff;
        }
    }
    // check whether it was the turn destination
    if (myTurnDestination == which) {
        myTurnDestination = by;
    }
}

void
NBEdge::replaceInConnections(NBEdge* which, const std::vector<NBEdge::Connection>& origConns) {
    std::map<int, int> laneMap;
    int minLane = -1;
    int maxLane = -1;
    // get lanes used to approach the edge to remap
    bool wasConnected = false;
    for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
        if ((*i).toEdge != which) {
            continue;
        }
        wasConnected = true;
        if ((*i).fromLane != -1) {
            int fromLane = (*i).fromLane;
            laneMap[(*i).toLane] = fromLane;
            if (minLane == -1 || minLane > fromLane) {
                minLane = fromLane;
            }
            if (maxLane == -1 || maxLane < fromLane) {
                maxLane = fromLane;
            }
        }
    }
    if (!wasConnected) {
        return;
    }
    // remove the remapped edge from connections
    removeFromConnections(which);
    // add new connections
    std::vector<NBEdge::Connection> conns = origConns;
    for (std::vector<NBEdge::Connection>::iterator i = conns.begin(); i != conns.end(); ++i) {
        if ((*i).toEdge == which) {
            continue;
        }
        int fromLane = (*i).fromLane;
        int toUse = -1;
        if (laneMap.find(fromLane) == laneMap.end()) {
            if (fromLane >= 0 && fromLane <= minLane) {
                toUse = minLane;
            }
            if (fromLane >= 0 && fromLane >= maxLane) {
                toUse = maxLane;
            }
        } else {
            toUse = laneMap[fromLane];
        }
        if (toUse == -1) {
            toUse = 0;
        }
        setConnection(toUse, (*i).toEdge, (*i).toLane, L2L_COMPUTED, false, (*i).mayDefinitelyPass, (*i).keepClear,
                      (*i).contPos, (*i).visibility, (*i).speed, (*i).customShape);
    }
}


void
NBEdge::copyConnectionsFrom(NBEdge* src) {
    myStep = src->myStep;
    myConnections = src->myConnections;
}


bool
NBEdge::canMoveConnection(const Connection& con, int newFromLane) const {
    // only allow using newFromLane if at least 1 vClass is permitted to use
    // this connection. If the connection shall be moved to a sidewalk, only create the connection if there is no walking area
    const SVCPermissions common = (getPermissions(newFromLane) & con.toEdge->getPermissions(con.toLane));
    return (common > 0 && common != SVC_PEDESTRIAN);
}


void
NBEdge::moveConnectionToLeft(int lane) {
    int index = 0;
    for (int i = 0; i < (int)myConnections.size(); ++i) {
        if (myConnections[i].fromLane == (int)(lane) && canMoveConnection(myConnections[i], lane + 1)) {
            index = i;
        }
    }
    std::vector<Connection>::iterator i = myConnections.begin() + index;
    Connection c = *i;
    myConnections.erase(i);
    setConnection(lane + 1, c.toEdge, c.toLane, L2L_VALIDATED, false);
}


void
NBEdge::moveConnectionToRight(int lane) {
    for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
        if ((*i).fromLane == (int)lane && canMoveConnection(*i, lane - 1)) {
            Connection c = *i;
            i = myConnections.erase(i);
            setConnection(lane - 1, c.toEdge, c.toLane, L2L_VALIDATED, false);
            return;
        }
    }
}


void
NBEdge::buildInnerEdges(const NBNode& n, int noInternalNoSplits, int& linkIndex, int& splitIndex) {
    const int numPoints = OptionsCont::getOptions().getInt("junctions.internal-link-detail");
    const bool joinTurns = OptionsCont::getOptions().getBool("junctions.join-turns");
    std::string innerID = ":" + n.getID();
    NBEdge* toEdge = 0;
    int edgeIndex = linkIndex;
    int internalLaneIndex = 0;
    for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
        Connection& con = *i;
        con.haveVia = false; // reset first since this may be called multiple times
        if (con.toEdge == 0) {
            continue;
        }
        LinkDirection dir = n.getDirection(this, con.toEdge);
        const bool isRightTurn = (dir == LINKDIR_RIGHT || dir == LINKDIR_PARTRIGHT);
        const bool isTurn = (isRightTurn || dir == LINKDIR_LEFT || dir == LINKDIR_PARTLEFT);

        // put turning internal lanes on separate edges
        if (con.toEdge != toEdge || (isTurn && !joinTurns)) {
            // skip indices to keep some correspondence between edge ids and link indices:
            // internalEdgeIndex + internalLaneIndex = linkIndex
            edgeIndex = linkIndex;
            toEdge = (*i).toEdge;
            internalLaneIndex = 0;
        }
        PositionVector shape = n.computeInternalLaneShape(this, con, numPoints, myTo);
        std::vector<int> foeInternalLinks;

        if (dir != LINKDIR_STRAIGHT && shape.length() < POSITION_EPS) {
            WRITE_WARNING("Connection '" + getID() + "_" + toString(con.fromLane) + "->" + con.toEdge->getID() + "_" + toString(con.toLane) + "' is only " + toString(shape.length()) + " short.");
        }

        // crossingPosition, list of foe link indices
        std::pair<double, std::vector<int> > crossingPositions(-1, std::vector<int>());
        std::set<std::string> tmpFoeIncomingLanes;
        switch (dir) {
            case LINKDIR_RIGHT:
            case LINKDIR_PARTRIGHT:
            case LINKDIR_LEFT:
            case LINKDIR_PARTLEFT:
            case LINKDIR_TURN: {
                int index = 0;
                const std::vector<NBEdge*>& incoming = n.getIncomingEdges();
                for (EdgeVector::const_iterator i2 = incoming.begin(); i2 != incoming.end(); ++i2) {
                    const std::vector<Connection>& elv = (*i2)->getConnections();
                    for (std::vector<NBEdge::Connection>::const_iterator k2 = elv.begin(); k2 != elv.end(); k2++) {
                        if ((*k2).toEdge == 0) {
                            continue;
                        }
                        bool needsCont = n.needsCont(this, *i2, con, *k2);
                        // compute the crossing point
                        if (needsCont) {
                            crossingPositions.second.push_back(index);
                            const PositionVector otherShape = n.computeInternalLaneShape(*i2, *k2, numPoints);
                            // vehicles are typically less wide than the lane
                            // they drive on but but bicycle lanes should be kept clear for their whole width
                            double width2 = (*k2).toEdge->getLaneWidth((*k2).toLane);
                            if ((*k2).toEdge->getPermissions((*k2).toLane) != SVC_BICYCLE) {
                                width2 *= 0.5;
                            }
                            const double minDV = firstIntersection(shape, otherShape, width2);
                            if (minDV < shape.length() - POSITION_EPS && minDV > POSITION_EPS) { // !!!?
                                assert(minDV >= 0);
                                if (crossingPositions.first < 0 || crossingPositions.first > minDV) {
                                    crossingPositions.first = minDV;
                                }
                            }
                        }
                        const bool rightTurnConflict = NBNode::rightTurnConflict(
                                                           this, con.toEdge, con.fromLane, (*i2), (*k2).toEdge, (*k2).fromLane);
                        // compute foe internal lanes
                        if (n.foes(this, con.toEdge, *i2, (*k2).toEdge) || rightTurnConflict) {
                            foeInternalLinks.push_back(index);
                        }
                        // compute foe incoming lanes
                        const bool signalised = hasSignalisedConnectionTo(con.toEdge);
                        if ((n.forbids(*i2, (*k2).toEdge, this, con.toEdge, signalised) || rightTurnConflict) && (needsCont || dir == LINKDIR_TURN)) {
                            tmpFoeIncomingLanes.insert((*i2)->getID() + "_" + toString((*k2).fromLane));
                        }
                        index++;
                    }
                }
                // foe pedestrian crossings
                std::vector<NBNode::Crossing*> crossings = n.getCrossings();
                for (auto c : crossings) {
                    const NBNode::Crossing& crossing = *c;
                    for (EdgeVector::const_iterator it_e = crossing.edges.begin(); it_e != crossing.edges.end(); ++it_e) {
                        const NBEdge* edge = *it_e;
                        // compute foe internal lanes
                        if (this == edge || con.toEdge == edge) {
                            foeInternalLinks.push_back(index);
                            if (con.toEdge == edge &&
                                    ((isRightTurn && getJunctionPriority(&n) > 0) || (isTurn && n.isTLControlled()))) {
                                // build internal junctions (not for left turns at uncontrolled intersections)
                                PositionVector crossingShape = crossing.shape;
                                crossingShape.extrapolate(1.0); // sometimes shapes miss each other by a small margin
                                const double minDV = firstIntersection(shape, crossingShape, crossing.width / 2);
                                if (minDV < shape.length() - POSITION_EPS && minDV > POSITION_EPS) {
                                    assert(minDV >= 0);
                                    if (crossingPositions.first < 0 || crossingPositions.first > minDV) {
                                        crossingPositions.first = minDV;
                                    }
                                }
                            }
                        }
                    }
                    index++;
                }

                if (dir == LINKDIR_TURN && crossingPositions.first < 0 && crossingPositions.second.size() != 0 && shape.length() > 2. * POSITION_EPS) {
                    // let turnarounds wait in the middle if no other crossing point was found and it has a sensible length
                    // (if endOffset is used, the crossing point is in the middle of the part within the junction shape)
                    crossingPositions.first = (double)(shape.length() + getEndOffset(con.fromLane)) / 2.;
                }
            }
            break;
            default:
                break;
        }
        if (con.contPos != UNSPECIFIED_CONTPOS) {
            // apply custom internal junction position
            if (con.contPos <= 0 || con.contPos >= shape.length()) {
                // disable internal junction
                crossingPositions.first = -1;
            } else {
                // set custom position
                crossingPositions.first = con.contPos;
            }
        }

        // @todo compute the maximum speed allowed based on angular velocity
        //  see !!! for an explanation (with a_lat_mean ~0.3)
        /*
        double vmax = (double) 0.3 * (double) 9.80778 *
                        getLaneShape(con.fromLane).back().distanceTo(
                            con.toEdge->getLaneShape(con.toLane).front())
                        / (double) 2.0 / (double) M_PI;
        vmax = MIN2(vmax, ((getSpeed() + con.toEdge->getSpeed()) / (double) 2.0));
        */
        if (con.speed == UNSPECIFIED_SPEED) {
            con.vmax = (myLanes[con.fromLane].speed + con.toEdge->getLanes()[con.toLane].speed) / (double) 2.0;
        } else {
            con.vmax = con.speed;
        }
        //
        Position end = con.toEdge->getLaneShape(con.toLane).front();
        Position beg = getLaneShape(con.fromLane).back();

        assert(shape.size() >= 2);
        // get internal splits if any
        con.id = innerID + "_" + toString(edgeIndex);
        if (crossingPositions.first >= 0) {
            std::pair<PositionVector, PositionVector> split = shape.splitAt(crossingPositions.first);
            con.shape = split.first;
            con.foeIncomingLanes = joinToString(tmpFoeIncomingLanes, " ");
            con.foeInternalLinks = foeInternalLinks; // resolve link indices to lane ids later
            con.viaID = innerID + "_" + toString(splitIndex + noInternalNoSplits);
            ++splitIndex;
            con.viaShape = split.second;
            con.haveVia = true;
        } else {
            con.shape = shape;
        }
        con.internalLaneIndex = internalLaneIndex;
        ++internalLaneIndex;
        ++linkIndex;
    }
}


double
NBEdge::firstIntersection(const PositionVector& v1, const PositionVector& v2, double width2) {
    double intersect = std::numeric_limits<double>::max();
    if (v2.length() < POSITION_EPS) {
        return intersect;
    }
    PositionVector v2Right = v2;
    v2Right.move2side(width2);

    PositionVector v2Left = v2;
    v2Left.move2side(-width2);

    // intersect center line of v1 with left and right border of v2
    std::vector<double> tmp = v1.intersectsAtLengths2D(v2Right);
    if (tmp.size() > 0) {
        intersect = MIN2(intersect, tmp[0]);
    }
    tmp = v1.intersectsAtLengths2D(v2Left);
    if (tmp.size() > 0) {
        intersect = MIN2(intersect, tmp[0]);
    }
    return intersect;
}


// -----------
int
NBEdge::getJunctionPriority(const NBNode* const node) const {
    if (node == myFrom) {
        return myFromJunctionPriority;
    } else {
        return myToJunctionPriority;
    }
}


void
NBEdge::setJunctionPriority(const NBNode* const node, int prio) {
    if (node == myFrom) {
        myFromJunctionPriority = prio;
    } else {
        myToJunctionPriority = prio;
    }
}


double
NBEdge::getAngleAtNode(const NBNode* const atNode) const {
    // myStartAngle, myEndAngle are in [0,360] and this returns results in [-180,180]
    if (atNode == myFrom) {
        return GeomHelper::legacyDegree(myGeom.angleAt2D(0));
    } else {
        assert(atNode == myTo);
        return GeomHelper::legacyDegree(myGeom.angleAt2D(-2));
    }
}


double
NBEdge::getAngleAtNodeToCenter(const NBNode* const atNode) const {
    if (atNode == myFrom) {
        double res = myStartAngle - 180;
        if (res < 0) {
            res += 360;
        }
        return res;
    } else {
        assert(atNode == myTo);
        return myEndAngle;
    }
}


void
NBEdge::setTurningDestination(NBEdge* e, bool onlyPossible) {
    if (!onlyPossible) {
        myTurnDestination = e;
    }
    myPossibleTurnDestination = e;
}


double
NBEdge::getLaneSpeed(int lane) const {
    return myLanes[lane].speed;
}


void
NBEdge::computeLaneShapes() {
    // vissim needs this
    if (myFrom == myTo) {
        return;
    }
    // compute lane offset, first
    std::vector<double> offsets(myLanes.size(), 0.);
    double offset = 0;
    for (int i = (int)myLanes.size() - 2; i >= 0; --i) {
        offset += (getLaneWidth(i) + getLaneWidth(i + 1)) / 2. + SUMO_const_laneOffset;
        offsets[i] = offset;
    }
    if (myLaneSpreadFunction == LANESPREAD_RIGHT) {
        double laneWidth = myLanes.back().width != UNSPECIFIED_WIDTH ? myLanes.back().width : SUMO_const_laneWidth;
        offset = (laneWidth + SUMO_const_laneOffset) / 2.; // @todo: why is the lane offset counted in here?
    } else {
        double width = 0;
        for (int i = 0; i < (int)myLanes.size(); ++i) {
            width += getLaneWidth(i);
        }
        width += SUMO_const_laneOffset * double(myLanes.size() - 1);
        offset = -width / 2. + getLaneWidth((int)myLanes.size() - 1) / 2.;
    }
    for (int i = 0; i < (int)myLanes.size(); ++i) {
        offsets[i] += offset;
    }

    // build the shape of each lane
    for (int i = 0; i < (int)myLanes.size(); ++i) {
        if (myLanes[i].customShape.size() != 0) {
            myLanes[i].shape = myLanes[i].customShape;
            continue;
        }
        try {
            myLanes[i].shape = computeLaneShape(i, offsets[i]);
        } catch (InvalidArgument& e) {
            WRITE_WARNING("In edge '" + getID() + "': lane shape could not be determined (" + e.what() + ").");
            myLanes[i].shape = myGeom;
        }
    }
}


PositionVector
NBEdge::computeLaneShape(int lane, double offset) const {
    PositionVector shape = myGeom;
    try {
        shape.move2side(offset);
    } catch (InvalidArgument& e) {
        WRITE_WARNING("In lane '" + getLaneID(lane) + "': Could not build shape (" + e.what() + ").");
    }
    return shape;
}


void
NBEdge::computeAngle() {
    // taking the angle at the first might be unstable, thus we take the angle
    // at a certain distance. (To compare two edges, additional geometry
    // segments are considered to resolve ambiguities)
    const bool hasFromShape = myFrom->getShape().size() > 0;
    const bool hasToShape = myTo->getShape().size() > 0;
    Position fromCenter = (hasFromShape ? myFrom->getShape().getCentroid() : myFrom->getPosition());
    Position toCenter = (hasToShape ? myTo->getShape().getCentroid() : myTo->getPosition());
    PositionVector shape = ((hasFromShape || hasToShape) && getNumLanes() > 0 ?
                            (myLaneSpreadFunction == LANESPREAD_RIGHT ?
                             myLanes[getNumLanes() - 1].shape
                             : myLanes[getNumLanes() / 2].shape)
                            : myGeom);

    // if the junction shape is suspicious we cannot trust the angle to the centroid
    if (hasFromShape && (myFrom->getShape().distance2D(shape[0]) > 2 * POSITION_EPS
                         || myFrom->getShape().around(shape[-1])
                         || !(myFrom->getShape().around(fromCenter)))) {
        fromCenter = myFrom->getPosition();
    }
    if (hasToShape && (myTo->getShape().distance2D(shape[-1]) > 2 * POSITION_EPS
                       || myTo->getShape().around(shape[0])
                       || !(myTo->getShape().around(toCenter)))) {
        toCenter = myTo->getPosition();
    }

    const double angleLookahead = MIN2(shape.length2D() / 2, ANGLE_LOOKAHEAD);
    const Position referencePosStart = shape.positionAtOffset2D(angleLookahead);
    myStartAngle = GeomHelper::legacyDegree(fromCenter.angleTo2D(referencePosStart), true);
    const Position referencePosEnd = shape.positionAtOffset2D(shape.length() - angleLookahead);
    myEndAngle = GeomHelper::legacyDegree(referencePosEnd.angleTo2D(toCenter), true);
    myTotalAngle = GeomHelper::legacyDegree(myFrom->getPosition().angleTo2D(myTo->getPosition()), true);
#ifdef DEBUG_ANGLES
    if (DEBUGCOND) std::cout << "computeAngle edge=" << getID() << " fromCenter=" << fromCenter << " toCenter=" << toCenter
                                 << " refStart=" << referencePosStart << " refEnd=" << referencePosEnd << " shape=" << shape
                                 << " hasFromShape=" << hasFromShape
                                 << " hasToShape=" << hasToShape
                                 << " numLanes=" << getNumLanes()
                                 << " shapeLane=" << getNumLanes() / 2
                                 << " startA=" << myStartAngle << " endA=" << myEndAngle << " totA=" << myTotalAngle << "\n";
#endif
}


double
NBEdge::getShapeStartAngle() const {
    const double angleLookahead = MIN2(myGeom.length2D() / 2, ANGLE_LOOKAHEAD);
    const Position referencePosStart = myGeom.positionAtOffset2D(angleLookahead);
    return GeomHelper::legacyDegree(myGeom.front().angleTo2D(referencePosStart), true);
}


double
NBEdge::getShapeEndAngle() const {
    const double angleLookahead = MIN2(myGeom.length2D() / 2, ANGLE_LOOKAHEAD);
    const Position referencePosEnd = myGeom.positionAtOffset2D(myGeom.length() - angleLookahead);
    return GeomHelper::legacyDegree(referencePosEnd.angleTo2D(myGeom.back()), true);
}


bool
NBEdge::hasPermissions() const {
    for (std::vector<Lane>::const_iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        if ((*i).permissions != SVCAll) {
            return true;
        }
    }
    return false;
}


bool
NBEdge::hasLaneSpecificPermissions() const {
    std::vector<Lane>::const_iterator i = myLanes.begin();
    SVCPermissions firstLanePermissions = i->permissions;
    i++;
    for (; i != myLanes.end(); ++i) {
        if (i->permissions != firstLanePermissions) {
            return true;
        }
    }
    return false;
}


bool
NBEdge::hasLaneSpecificSpeed() const {
    for (std::vector<Lane>::const_iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        if (i->speed != getSpeed()) {
            return true;
        }
    }
    return false;
}


bool
NBEdge::hasLaneSpecificWidth() const {
    for (std::vector<Lane>::const_iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        if (i->width != myLanes.begin()->width) {
            return true;
        }
    }
    return false;
}


bool
NBEdge::hasLaneSpecificEndOffset() const {
    for (std::vector<Lane>::const_iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        if (i->endOffset != myLanes.begin()->endOffset) {
            return true;
        }
    }
    return false;
}


bool
NBEdge::hasAccelLane() const {
    for (std::vector<Lane>::const_iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        if (i->accelRamp) {
            return true;
        }
    }
    return false;
}


bool
NBEdge::hasCustomLaneShape() const {
    for (std::vector<Lane>::const_iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        if (i->customShape.size() > 0) {
            return true;
        }
    }
    return false;
}


bool
NBEdge::hasLaneParams() const {
    for (std::vector<Lane>::const_iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        if (i->getMap().size() > 0) {
            return true;
        }
    }
    return false;
}

bool
NBEdge::needsLaneSpecificOutput() const {
    return (hasLaneSpecificPermissions()
            || hasLaneSpecificSpeed()
            || hasLaneSpecificWidth()
            || hasLaneSpecificEndOffset()
            || hasAccelLane()
            || hasCustomLaneShape()
            || hasLaneParams()
            || (!myLanes.empty() && myLanes.back().oppositeID != ""));
}



bool
NBEdge::computeEdge2Edges(bool noLeftMovers) {
    // return if this relationship has been build in previous steps or
    //  during the import
    if (myStep >= EDGE2EDGES) {
        return true;
    }
    const EdgeVector& o = myTo->getOutgoingEdges();
    for (EdgeVector::const_iterator i = o.begin(); i != o.end(); ++i) {
        if (noLeftMovers && myTo->isLeftMover(this, *i)) {
            continue;
        }
        myConnections.push_back(Connection(-1, *i, -1));
    }
    myStep = EDGE2EDGES;
    return true;
}


bool
NBEdge::computeLanes2Edges() {
    // return if this relationship has been build in previous steps or
    //  during the import
    if (myStep >= LANES2EDGES) {
        return true;
    }
    assert(myStep == EDGE2EDGES);
    // get list of possible outgoing edges sorted by direction clockwise
    //  the edge in the backward direction (turnaround) is not in the list
    const EdgeVector* edges = getConnectedSorted();
    if (myConnections.size() != 0 && edges->size() == 0) {
        // dead end per definition!?
        myConnections.clear();
    } else {
        // divide the lanes on reachable edges
        divideOnEdges(edges);
    }
    delete edges;
    myStep = LANES2EDGES;
    return true;
}


bool
NBEdge::recheckLanes() {
#ifdef DEBUG_CONNECTION_GUESSING
    if (DEBUGCOND) {
        std::cout << "recheckLanes (initial) edge=" << getID() << "\n";
        for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
            std::cout << "  conn " << getID() << "_" << (*i).fromLane << " to " << Named::getIDSecure((*i).toEdge) << "_" << (*i).toLane << "\n";
            (*i).shape.mirrorX();
            (*i).viaShape.mirrorX();
        }
    }
#endif
    std::vector<int> connNumbersPerLane(myLanes.size(), 0);
    for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end();) {
        if ((*i).toEdge == 0 || (*i).fromLane < 0 || (*i).toLane < 0) {
            i = myConnections.erase(i);
        } else {
            if ((*i).fromLane >= 0) {
                ++connNumbersPerLane[(*i).fromLane];
            }
            ++i;
        }
    }
    if (myStep != LANES2LANES_DONE && myStep != LANES2LANES_USER) {
        // check #1:
        // If there is a lane with no connections and any neighbour lane has
        //  more than one connections, try to move one of them.
        // This check is only done for edges which connections were assigned
        //  using the standard algorithm.
        for (int i = 0; i < (int)myLanes.size(); i++) {
            if (connNumbersPerLane[i] == 0 && !isForbidden(getPermissions((int)i))) {
                if (i > 0 && connNumbersPerLane[i - 1] > 1 && getPermissions(i) == getPermissions(i - 1)) {
                    moveConnectionToLeft(i - 1);
                } else if (i < (int)myLanes.size() - 1 && connNumbersPerLane[i + 1] > 1 && getPermissions(i) == getPermissions(i + 1)) {
                    moveConnectionToRight(i + 1);
                }
            }
        }
        // check restrictions
        for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end();) {
            Connection& c = *i;
            const SVCPermissions common = getPermissions(c.fromLane) & c.toEdge->getPermissions(c.toLane);
            if (common == SVC_PEDESTRIAN || getPermissions(c.fromLane) == SVC_PEDESTRIAN) {
                // these are computed in NBNode::buildWalkingAreas
                i = myConnections.erase(i);
            } else if (common == 0) {
                // no common permissions.
                // try to find a suitable target lane to the right
                const int origToLane = c.toLane;
                c.toLane = -1; // ignore this connection when calling hasConnectionTo
                int toLane = origToLane;
                while (toLane > 0
                        && (getPermissions(c.fromLane) & c.toEdge->getPermissions(toLane)) == 0
                        && !hasConnectionTo(c.toEdge, toLane)
                      ) {
                    toLane--;
                }
                if ((getPermissions(c.fromLane) & c.toEdge->getPermissions(toLane)) != 0
                        && !hasConnectionTo(c.toEdge, toLane)) {
                    c.toLane = toLane;
                    ++i;
                } else {
                    // try to find a suitable target lane to the left
                    int toLane = origToLane;
                    while (toLane < (int)c.toEdge->getNumLanes() - 1
                            && (getPermissions(c.fromLane) & c.toEdge->getPermissions(toLane)) == 0
                            && !hasConnectionTo(c.toEdge, toLane)
                          ) {
                        toLane++;
                    }
                    if ((getPermissions(c.fromLane) & c.toEdge->getPermissions(toLane)) != 0
                            && !hasConnectionTo(c.toEdge, toLane)) {
                        c.toLane = toLane;
                        ++i;
                    } else {
                        // no alternative target found
                        i = myConnections.erase(i);
                    }
                }
            } else if (isRailway(getPermissions(c.fromLane)) && isRailway(c.toEdge->getPermissions(c.toLane))
                       && isTurningDirectionAt(c.toEdge))  {
                // do not allow sharp rail turns
                i = myConnections.erase(i);
            } else {
                ++i;
            }
        }
    }
    // check delayed removals
    for (std::vector<Connection>::iterator it = myConnectionsToDelete.begin(); it != myConnectionsToDelete.end(); ++it) {
        removeFromConnections(it->toEdge, it->fromLane, it->toLane);
    }
    // check involuntary dead end at "real" junctions
    if (getPermissions() != SVC_PEDESTRIAN) {
        if (myConnections.empty() && myTo->getOutgoingEdges().size() > 1) {
            WRITE_WARNING("Edge '" + getID() + "' is not connected to outgoing edges at junction '" + myTo->getID() + "'.");
        }
        const EdgeVector& incoming = myFrom->getIncomingEdges();
        if (incoming.size() > 1) {
            for (int i = 0; i < (int)myLanes.size(); i++) {
                if (getPermissions(i) != 0 && getPermissions(i) != SVC_PEDESTRIAN) {
                    bool connected = false;
                    for (std::vector<NBEdge*>::const_iterator in = incoming.begin(); in != incoming.end(); ++in) {
                        if ((*in)->hasConnectionTo(this, i)) {
                            connected = true;
                            break;
                        }
                    }
                    if (!connected) {
                        WRITE_WARNING("Lane '" + getID() + "_" + toString(i) + "' is not connected from any incoming edge at junction '" + myFrom->getID() + "'.");
                    }
                }
            }
        }
    }
#ifdef DEBUG_CONNECTION_GUESSING
    if (DEBUGCOND) {
        std::cout << "recheckLanes (final) edge=" << getID() << "\n";
        for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
            std::cout << "  conn " << getID() << "_" << (*i).fromLane << " to " << Named::getIDSecure((*i).toEdge) << "_" << (*i).toLane << "\n";
            (*i).shape.mirrorX();
            (*i).viaShape.mirrorX();
        }
    }
#endif
    return true;
}


void
NBEdge::divideOnEdges(const EdgeVector* outgoing) {
    if (outgoing->size() == 0) {
        // we have to do this, because the turnaround may have been added before
        myConnections.clear();
        return;
    }
    // precompute edge priorities; needed as some kind of assumptions for
    //  priorities of directions (see preparePriorities)
    std::vector<int>* priorities = prepareEdgePriorities(outgoing);
    // compute the indices of lanes that should have connections (excluding
    // forbidden lanes and pedestrian lanes that will be connected via walkingAreas)


#ifdef DEBUG_CONNECTION_GUESSING
    if (DEBUGCOND) {
        std::cout << " divideOnEdges " << getID() << " outgoing=" << toString(*outgoing) << " prios=" << toString(*priorities) << "\n";
    }
#endif

    // build connections for miv lanes
    std::vector<int> availableLanes;
    for (int i = 0; i < (int)myLanes.size(); ++i) {
        if ((getPermissions(i) & SVC_PASSENGER) != 0) {
            availableLanes.push_back(i);
        }
    }
    if (availableLanes.size() > 0) {
        divideSelectedLanesOnEdges(outgoing, availableLanes, priorities);
    }
    // build connections for miscellaneous further modes (more than bike,peds,bus and without passenger)
    availableLanes.clear();
    for (int i = 0; i < (int)myLanes.size(); ++i) {
        const SVCPermissions perms = getPermissions(i);
        if ((perms & ~(SVC_PEDESTRIAN | SVC_BICYCLE | SVC_BUS)) == 0 || (perms & SVC_PASSENGER) != 0 || isForbidden(perms)) {
            continue;
        }
        availableLanes.push_back(i);
    }
    if (availableLanes.size() > 0) {
        divideSelectedLanesOnEdges(outgoing, availableLanes, priorities);
    }
    // build connections for busses (possibly combined with bicycles)
    availableLanes.clear();
    for (int i = 0; i < (int)myLanes.size(); ++i) {
        const SVCPermissions perms = getPermissions(i);
        if (perms != SVC_BUS && perms != (SVC_BUS | SVC_BICYCLE)) {
            continue;
        }
        availableLanes.push_back(i);
    }
    if (availableLanes.size() > 0) {
        divideSelectedLanesOnEdges(outgoing, availableLanes, priorities);
    }
    // build connections for bicycles (possibly combined with pedestrians)
    availableLanes.clear();
    for (int i = 0; i < (int)myLanes.size(); ++i) {
        const SVCPermissions perms = getPermissions(i);
        if (perms != SVC_BICYCLE && perms != (SVC_BICYCLE | SVC_PEDESTRIAN)) {
            continue;
        }
        availableLanes.push_back(i);
    }
    if (availableLanes.size() > 0) {
        divideSelectedLanesOnEdges(outgoing, availableLanes, priorities);
    }
    // clean up unassigned fromLanes
    for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end();) {
        if ((*i).fromLane == -1) {
            i = myConnections.erase(i);
        } else {
            ++i;
        }
    }
    sortOutgoingConnectionsByIndex();

    delete priorities;
}


void
NBEdge::divideSelectedLanesOnEdges(const EdgeVector* outgoing, const std::vector<int>& availableLanes, const std::vector<int>* priorities) {
    //std::cout << "divideSelectedLanesOnEdges " << getID() << " out=" << toString(*outgoing) << " prios=" << toString(*priorities) << " avail=" << toString(availableLanes) << "\n";
    // compute the sum of priorities (needed for normalisation)
    int prioSum = computePrioritySum(*priorities);
    // compute the resulting number of lanes that should be used to
    //  reach the following edge
    const int numOutgoing = (int) outgoing->size();
    std::vector<double> resultingLanes;
    resultingLanes.reserve(numOutgoing);
    double sumResulting = 0.; // the sum of resulting lanes
    double minResulting = 10000.; // the least number of lanes to reach an edge
    for (int i = 0; i < numOutgoing; i++) {
        // res will be the number of lanes which are meant to reach the
        //  current outgoing edge
        double res =
            (double)(*priorities)[i] *
            (double) availableLanes.size() / (double) prioSum;
        // do not let this number be greater than the number of available lanes
        if (res > availableLanes.size()) {
            res = (double) availableLanes.size();
        }
        // add it to the list
        resultingLanes.push_back(res);
        sumResulting += res;
        if (minResulting > res && res > 0) {
            // prevent minResulting from becoming 0
            minResulting = res;
        }
    }
    // compute the number of virtual edges
    //  a virtual edge is used as a replacement for a real edge from now on
    //  it shall allow to divide the existing lanes on this structure without
    //  regarding the structure of outgoing edges
    int numVirtual = 0;
    // compute the transition from virtual to real edges
    EdgeVector transition;
    transition.reserve(numOutgoing);
    for (int i = 0; i < numOutgoing; i++) {
        // tmpNo will be the number of connections from this edge
        //  to the next edge
        assert(i < (int)resultingLanes.size());
        const int tmpNum = (int)std::ceil(resultingLanes[i] / minResulting);
        numVirtual += tmpNum;
        for (double j = 0; j < tmpNum; j++) {
            transition.push_back((*outgoing)[i]);
        }
    }
#ifdef DEBUG_CONNECTION_GUESSING
    if (DEBUGCOND) {
        std::cout << "   prioSum=" << prioSum << " sumResulting=" << sumResulting << " minResulting=" << minResulting << " numVirtual=" << numVirtual << " availLanes=" << toString(availableLanes) << " resLanes=" << toString(resultingLanes) << " transition=" << toString(transition) << "\n";
    }
#endif

    // assign lanes to edges
    //  (conversion from virtual to real edges is done)
    ToEdgeConnectionsAdder adder(transition);
    Bresenham::compute(&adder, static_cast<int>(availableLanes.size()), numVirtual);
    const std::map<NBEdge*, std::vector<int> >& l2eConns = adder.getBuiltConnections();
    for (EdgeVector::const_iterator i = outgoing->begin(); i != outgoing->end(); ++i) {
        NBEdge* target = (*i);
        assert(l2eConns.find(target) != l2eConns.end());
        const std::vector<int> lanes = (l2eConns.find(target))->second;
        for (std::vector<int>::const_iterator j = lanes.begin(); j != lanes.end(); ++j) {
            const int fromIndex = availableLanes[*j];
            if ((getPermissions(fromIndex) & target->getPermissions()) == 0) {
                // exclude connection if fromLane and toEdge have no common permissions
                continue;
            }
            if ((getPermissions(fromIndex) & target->getPermissions()) == SVC_PEDESTRIAN) {
                // exclude connection if the only commonly permitted class are pedestrians
                // these connections are later built in NBNode::buildWalkingAreas
                continue;
            }
            // avoid building more connections than the edge has viable lanes (earlier
            // ones have precedence). This is necessary when running divideSelectedLanesOnEdges more than once.
            //    @todo To decide which target lanes are still available we need to do a
            // preliminary lane-to-lane assignment in regard to permissions (rather than to ordering)
            const int numConsToTarget = (int)count_if(myConnections.begin(), myConnections.end(), connections_toedge_finder(target, true));
            int targetLanes = (int)target->getNumLanes();
            if (target->getPermissions(0) == SVC_PEDESTRIAN) {
                --targetLanes;
            }
            if (numConsToTarget >= targetLanes) {
                // let bicycles move onto the road to allow continuation
                // the speed limit is taken from rural roads (which allow cycles)
                // (pending implementation of #1859)
                if (getPermissions(fromIndex) == SVC_BICYCLE && getSpeed() <= (101 / 3.6)) {
                    for (int ii = 0; ii < (int)myLanes.size(); ++ii) {
                        if (myLanes[ii].permissions != SVC_PEDESTRIAN) {
                            myLanes[ii].permissions |= SVC_BICYCLE;
                        }
                    }
                }
                continue;
            }
            if (myLanes[fromIndex].connectionsDone) {
                // we already have complete information about connections from
                // this lane. do not add anything else
                continue;
            }
            myConnections.push_back(Connection(fromIndex, target, -1));
#ifdef DEBUG_CONNECTION_GUESSING
            if (DEBUGCOND) {
                std::cout << "     request connection from " << getID() << "_" << fromIndex << " to " << target->getID() << "\n";
            }
#endif
        }
    }

    addStraightConnections(outgoing, availableLanes, priorities);
}


void
NBEdge::addStraightConnections(const EdgeVector* outgoing, const std::vector<int>& availableLanes, const std::vector<int>* priorities) {
    // ensure sufficient straight connections for the (hightest-priority straight target)
    const int numOutgoing = (int) outgoing->size();
    NBEdge* target = 0;
    NBEdge* rightOfTarget = 0;
    NBEdge* leftOfTarget = 0;
    int maxPrio = 0;
    for (int i = 0; i < numOutgoing; i++) {
        if (maxPrio < (*priorities)[i]) {
            const LinkDirection dir = myTo->getDirection(this, (*outgoing)[i]);
            if (dir == LINKDIR_STRAIGHT) {
                maxPrio = (*priorities)[i];
                target = (*outgoing)[i];
                rightOfTarget = i == 0 ? outgoing->back() : (*outgoing)[i - 1];
                leftOfTarget = i + 1 == numOutgoing ? outgoing->front() : (*outgoing)[i + 1];
            }
        }
    }
    if (target == 0) {
        return;
    }
    int numConsToTarget = (int)count_if(myConnections.begin(), myConnections.end(), connections_toedge_finder(target, true));
    int targetLanes = (int)target->getNumLanes();
    if (target->getPermissions(0) == SVC_PEDESTRIAN) {
        --targetLanes;
    }
    const int numDesiredConsToTarget = MIN2(targetLanes, (int)availableLanes.size());
#ifdef DEBUG_CONNECTION_GUESSING
    if (DEBUGCOND) {
        std::cout << "  checking extra lanes for target=" << target->getID() << " cons=" << numConsToTarget << " desired=" << numDesiredConsToTarget << "\n";
    }
#endif
    std::vector<int>::const_iterator it_avail = availableLanes.begin();
    while (numConsToTarget < numDesiredConsToTarget && it_avail != availableLanes.end()) {
        const int fromIndex = *it_avail;
        if (
            // not yet connected
            (count_if(myConnections.begin(), myConnections.end(), connections_finder(fromIndex, target, -1)) == 0)
            // matching permissions
            && ((getPermissions(fromIndex) & target->getPermissions()) != 0)
            // more than pedestrians
            && ((getPermissions(fromIndex) & target->getPermissions()) != SVC_PEDESTRIAN)
            // lane not yet fully defined
            && !myLanes[fromIndex].connectionsDone
        ) {
#ifdef DEBUG_CONNECTION_GUESSING
            if (DEBUGCOND) {
                std::cout << "    candidate from " << getID() << "_" << fromIndex << " to " << target->getID() << "\n";
            }
#endif
            // prevent same-edge conflicts
            if (
                // no outgoing connections to the right from further left
                ((it_avail + 1) == availableLanes.end() || count_if(myConnections.begin(), myConnections.end(), connections_conflict_finder(fromIndex, rightOfTarget, false)) == 0)
                // no outgoing connections to the left from further right
                && (it_avail == availableLanes.begin() || count_if(myConnections.begin(), myConnections.end(), connections_conflict_finder(fromIndex, leftOfTarget, true)) == 0)) {
#ifdef DEBUG_CONNECTION_GUESSING
                if (DEBUGCOND) {
                    std::cout << "     request additional connection from " << getID() << "_" << fromIndex << " to " << target->getID() << "\n";
                }
#endif
                myConnections.push_back(Connection(fromIndex, target, -1));
                numConsToTarget++;
            } else {
#ifdef DEBUG_CONNECTION_GUESSING
                if (DEBUGCOND) std::cout
                            << "     fail check1="
                            << ((it_avail + 1) == availableLanes.end() || count_if(myConnections.begin(), myConnections.end(), connections_conflict_finder(fromIndex, rightOfTarget, false)) == 0)
                            << " check2=" << (it_avail == availableLanes.begin() || count_if(myConnections.begin(), myConnections.end(), connections_conflict_finder(fromIndex, leftOfTarget, true)) == 0)
                            << " rightOfTarget=" << rightOfTarget->getID()
                            << " leftOfTarget=" << leftOfTarget->getID()
                            << "\n";
#endif

            }
        }
        ++it_avail;
    }
}


std::vector<int>*
NBEdge::prepareEdgePriorities(const EdgeVector* outgoing) {
    // copy the priorities first
    std::vector<int>* priorities = new std::vector<int>();
    if (outgoing->size() == 0) {
        return priorities;
    }
    priorities->reserve(outgoing->size());
    EdgeVector::const_iterator i;
    for (i = outgoing->begin(); i != outgoing->end(); i++) {
        //int prio = (*i)->getJunctionPriority(myTo);
        int prio = NBNode::isTrafficLight(myTo->getType()) ? 0 : (*i)->getJunctionPriority(myTo);
        assert((prio + 1) * 2 > 0);
        prio = (prio + 1) * 2;
        priorities->push_back(prio);
    }
    // when the right turning direction has not a higher priority, divide
    //  the importance by 2 due to the possibility to leave the junction
    //  faster from this lane
    EdgeVector tmp(*outgoing);
    sort(tmp.begin(), tmp.end(), NBContHelper::straightness_sorter(myTo, this));
    i = find(outgoing->begin(), outgoing->end(), *(tmp.begin()));
    int dist = (int) distance(outgoing->begin(), i);
    MainDirections mainDirections(*outgoing, this, myTo, dist);
#ifdef DEBUG_CONNECTION_GUESSING
    if (DEBUGCOND) std::cout << "  prepareEdgePriorities " << getID()
                                 << " outgoing=" << toString(*outgoing)
                                 << " priorities1=" << toString(*priorities)
                                 << " tmp=" << toString(tmp)
                                 << " mainDirs=" << toString(mainDirections.myDirs)
                                 << " dist=" << dist
                                 << "\n";
#endif
    if (dist != 0 && !mainDirections.includes(MainDirections::DIR_RIGHTMOST)) {
        assert(priorities->size() > 0);
        (*priorities)[0] /= 2;
#ifdef DEBUG_CONNECTION_GUESSING
        if (DEBUGCOND) {
            std::cout << "   priorities2=" << toString(*priorities) << "\n";
        }
#endif
    }
    // HEURISTIC:
    // when no higher priority exists, let the forward direction be
    //  the main direction
    if (mainDirections.empty()) {
        assert(dist < (int)priorities->size());
        (*priorities)[dist] *= 2;
#ifdef DEBUG_CONNECTION_GUESSING
        if (DEBUGCOND) {
            std::cout << "   priorities3=" << toString(*priorities) << "\n";
        }
#endif
    }
    if (NBNode::isTrafficLight(myTo->getType())) {
        (*priorities)[dist] += 1;
    } else {
        // try to ensure separation of left turns
        if (mainDirections.includes(MainDirections::DIR_RIGHTMOST) && mainDirections.includes(MainDirections::DIR_LEFTMOST)) {
            (*priorities)[0] /= 4;
            (*priorities)[(int)priorities->size() - 1] /= 2;
#ifdef DEBUG_CONNECTION_GUESSING
            if (DEBUGCOND) {
                std::cout << "   priorities6=" << toString(*priorities) << "\n";
            }
#endif
        }
    }
    if (mainDirections.includes(MainDirections::DIR_FORWARD)) {
        if (myLanes.size() > 2) {
            (*priorities)[dist] *= 2;
#ifdef DEBUG_CONNECTION_GUESSING
            if (DEBUGCOND) {
                std::cout << "   priorities4=" << toString(*priorities) << "\n";
            }
#endif
        } else {
            (*priorities)[dist] *= 3;
#ifdef DEBUG_CONNECTION_GUESSING
            if (DEBUGCOND) {
                std::cout << "   priorities5=" << toString(*priorities) << "\n";
            }
#endif
        }
    }
    // return
    return priorities;
}


int
NBEdge::computePrioritySum(const std::vector<int>& priorities) {
    int sum = 0;
    for (std::vector<int>::const_iterator i = priorities.begin(); i != priorities.end(); i++) {
        sum += *i;
    }
    return sum;
}


void
NBEdge::appendTurnaround(bool noTLSControlled, bool checkPermissions) {
    // do nothing if no turnaround is known
    if (myTurnDestination == 0 || myTo->getType() == NODETYPE_RAIL_CROSSING) {
        return;
    }
    // do nothing if the destination node is controlled by a tls and no turnarounds
    //  shall be appended for such junctions
    if (noTLSControlled && myTo->isTLControlled()) {
        return;
    }
    const int fromLane = (int)myLanes.size() - 1;
    const int toLane = (int)myTurnDestination->getNumLanes() - 1;
    if (checkPermissions) {
        if ((getPermissions(fromLane) & myTurnDestination->getPermissions(toLane)) == 0) {
            // exclude connection if fromLane and toEdge have no common permissions
            return;
        }
        if ((getPermissions(fromLane) & myTurnDestination->getPermissions(toLane)) == SVC_PEDESTRIAN) {
            // exclude connection if the only commonly permitted class are pedestrians
            // these connections are later built in NBNode::buildWalkingAreas
            return;
        }
    }
    setConnection(fromLane, myTurnDestination, toLane, L2L_VALIDATED);
}


bool
NBEdge::isTurningDirectionAt(const NBEdge* const edge) const {
    // maybe it was already set as the turning direction
    if (edge == myTurnDestination) {
        return true;
    } else if (myTurnDestination != 0) {
        // otherwise - it's not if a turning direction exists
        return false;
    }
    return edge == myPossibleTurnDestination;
}


NBNode*
NBEdge::tryGetNodeAtPosition(double pos, double tolerance) const {
    // return the from-node when the position is at the begin of the edge
    if (pos < tolerance) {
        return myFrom;
    }
    // return the to-node when the position is at the end of the edge
    if (pos > myLength - tolerance) {
        return myTo;
    }
    return 0;
}


void
NBEdge::moveOutgoingConnectionsFrom(NBEdge* e, int laneOff) {
    int lanes = e->getNumLanes();
    for (int i = 0; i < lanes; i++) {
        std::vector<NBEdge::Connection> elv = e->getConnectionsFromLane(i);
        for (std::vector<NBEdge::Connection>::iterator j = elv.begin(); j != elv.end(); j++) {
            NBEdge::Connection el = *j;
            assert(el.tlID == "");
            addLane2LaneConnection(i + laneOff, el.toEdge, el.toLane, L2L_COMPUTED);
        }
    }
}


bool
NBEdge::lanesWereAssigned() const {
    return myStep == LANES2LANES_DONE || myStep == LANES2LANES_USER;
}


double
NBEdge::getMaxLaneOffset() {
    return (double) SUMO_const_laneWidthAndOffset * myLanes.size();
}


bool
NBEdge::mayBeTLSControlled(int fromLane, NBEdge* toEdge, int toLane) const {
    TLSDisabledConnection tpl;
    tpl.fromLane = fromLane;
    tpl.to = toEdge;
    tpl.toLane = toLane;
    std::vector<TLSDisabledConnection>::const_iterator i = find_if(myTLSDisabledConnections.begin(), myTLSDisabledConnections.end(), tls_disable_finder(tpl));
    return i == myTLSDisabledConnections.end();
}


bool
NBEdge::setControllingTLInformation(const NBConnection& c, const std::string& tlID) {
    const int fromLane = c.getFromLane();
    NBEdge* toEdge = c.getTo();
    const int toLane = c.getToLane();
    const int tlIndex = c.getTLIndex();
    // check whether the connection was not set as not to be controled previously
    TLSDisabledConnection tpl;
    tpl.fromLane = fromLane;
    tpl.to = toEdge;
    tpl.toLane = toLane;
    std::vector<TLSDisabledConnection>::iterator i = find_if(myTLSDisabledConnections.begin(), myTLSDisabledConnections.end(), tls_disable_finder(tpl));
    if (i != myTLSDisabledConnections.end()) {
        return false;
    }

    assert(fromLane < 0 || fromLane < (int) myLanes.size());
    // try to use information about the connections if given
    if (fromLane >= 0 && toLane >= 0) {
        // find the specified connection
        std::vector<Connection>::iterator i =
            find_if(myConnections.begin(), myConnections.end(), connections_finder(fromLane, toEdge, toLane));
        // ok, we have to test this as on the removal of self-loop edges some connections
        //  will be reassigned
        if (i != myConnections.end()) {
            // get the connection
            Connection& connection = *i;
            // set the information about the tl
            connection.tlID = tlID;
            connection.tlLinkNo = tlIndex;
            return true;
        }
    }
    // if the original connection was not found, set the information for all
    //  connections
    int no = 0;
    bool hadError = false;
    for (std::vector<Connection>::iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
        if ((*i).toEdge != toEdge) {
            continue;
        }
        if (fromLane >= 0 && fromLane != (*i).fromLane) {
            continue;
        }
        if (toLane >= 0 && toLane != (*i).toLane) {
            continue;
        }
        if ((*i).tlID == "") {
            (*i).tlID = tlID;
            (*i).tlLinkNo = tlIndex;
            no++;
        } else {
            if ((*i).tlID != tlID && (*i).tlLinkNo == tlIndex) {
                WRITE_WARNING("The lane '" + toString<int>((*i).fromLane) + "' on edge '" + getID() + "' already had a traffic light signal.");
                hadError = true;
            }
        }
    }
    if (hadError && no == 0) {
        WRITE_WARNING("Could not set any signal of the tlLogic '" + tlID + "' (unknown group)");
    }
    return true;
}


void
NBEdge::clearControllingTLInformation() {
    for (std::vector<Connection>::iterator it = myConnections.begin(); it != myConnections.end(); it++) {
        it->tlID = "";
    }
}


void
NBEdge::disableConnection4TLS(int fromLane, NBEdge* toEdge, int toLane) {
    TLSDisabledConnection c;
    c.fromLane = fromLane;
    c.to = toEdge;
    c.toLane = toLane;
    myTLSDisabledConnections.push_back(c);
}


PositionVector
NBEdge::getCWBoundaryLine(const NBNode& n) const {
    PositionVector ret;
    double width;
    int lane;
    if (myFrom == (&n)) {
        // outgoing
        lane = getFirstAllowedLaneIndex(NBNode::FORWARD);
        ret = myLanes[lane].shape;
    } else {
        // incoming
        lane = getFirstAllowedLaneIndex(NBNode::BACKWARD);
        ret = myLanes[lane].shape.reverse();
    }
    width = getLaneWidth(lane);
    ret.move2side(width * 0.5);
    return ret;
}


PositionVector
NBEdge::getCCWBoundaryLine(const NBNode& n) const {
    PositionVector ret;
    double width;
    int lane;
    if (myFrom == (&n)) {
        // outgoing
        lane = getFirstAllowedLaneIndex(NBNode::BACKWARD);
        ret = myLanes[lane].shape;
    } else {
        // incoming
        lane = getFirstAllowedLaneIndex(NBNode::FORWARD);
        ret = myLanes[lane].shape.reverse();
    }
    width = getLaneWidth(lane);
    ret.move2side(-width * 0.5);
    return ret;
}


bool
NBEdge::expandableBy(NBEdge* possContinuation, std::string& reason) const {
    // ok, the number of lanes must match
    if (myLanes.size() != possContinuation->myLanes.size()) {
        reason = "laneNumber";
        return false;
    }
    // the priority, too (?)
    if (getPriority() != possContinuation->getPriority()) {
        reason = "priority";
        return false;
    }
    // the speed allowed
    if (mySpeed != possContinuation->mySpeed) {
        reason = "speed";
        return false;
    }
    // spreadtype should match or it will look ugly
    if (myLaneSpreadFunction != possContinuation->myLaneSpreadFunction) {
        reason = "spreadType";
        return false;
    }
    // do not create self loops
    if (myFrom == possContinuation->myTo) {
        reason = "loop";
        return false;
    }
    // matching lanes must have identical properties
    for (int i = 0; i < (int)myLanes.size(); i++) {
        if (myLanes[i].speed != possContinuation->myLanes[i].speed) {
            reason = "lane " + toString(i) + " speed";
            return false;
        } else if (myLanes[i].permissions != possContinuation->myLanes[i].permissions) {
            reason = "lane " + toString(i) + " permissions";
            return false;
        } else if (myLanes[i].width != possContinuation->myLanes[i].width) {
            reason = "lane " + toString(i) + " width";
            return false;
        }
    }

    // the vehicle class constraints, too
    /*!!!
    if (myAllowedOnLanes!=possContinuation->myAllowedOnLanes
            ||
            myNotAllowedOnLanes!=possContinuation->myNotAllowedOnLanes) {
        return false;
    }
    */
    // also, check whether the connections - if any exit do allow to join
    //  both edges
    // This edge must have a one-to-one connection to the following lanes
    switch (myStep) {
        case INIT_REJECT_CONNECTIONS:
            break;
        case INIT:
            break;
        case EDGE2EDGES: {
            // the following edge must be connected
            const EdgeVector& conn = getConnectedEdges();
            if (find(conn.begin(), conn.end(), possContinuation) == conn.end()) {
                reason = "disconnected";
                return false;
            }
        }
        break;
        case LANES2EDGES:
        case LANES2LANES_RECHECK:
        case LANES2LANES_DONE:
        case LANES2LANES_USER: {
            // the possible continuation must be connected
            if (find_if(myConnections.begin(), myConnections.end(), connections_toedge_finder(possContinuation)) == myConnections.end()) {
                reason = "disconnected";
                return false;
            }
            // all lanes must go to the possible continuation
            std::vector<int> conns = getConnectionLanes(possContinuation);
            const int offset = MAX2(0, getFirstNonPedestrianLaneIndex(NBNode::FORWARD, true));
            if (conns.size() != myLanes.size() - offset) {
                reason = "some lanes disconnected";
                return false;
            }
        }
        break;
        default:
            break;
    }
    return true;
}


void
NBEdge::append(NBEdge* e) {
    // append geometry
    myGeom.append(e->myGeom);
    for (int i = 0; i < (int)myLanes.size(); i++) {
        myLanes[i].shape.append(e->myLanes[i].shape);
        if (myLanes[i].knowsParameter(SUMO_PARAM_ORIGID) || e->myLanes[i].knowsParameter(SUMO_PARAM_ORIGID)
                || OptionsCont::getOptions().getBool("output.original-names")) {
            const std::string origID = myLanes[i].getParameter(SUMO_PARAM_ORIGID, getID());
            const std::string origID2 = e->myLanes[i].getParameter(SUMO_PARAM_ORIGID, e->getID());
            if (origID != origID2) {
                myLanes[i].setParameter(SUMO_PARAM_ORIGID, origID + " " + origID2);
            }
        }
        myLanes[i].connectionsDone = e->myLanes[i].connectionsDone;
    }
    // recompute length
    myLength += e->myLength;
    // copy the connections and the building step if given
    myStep = e->myStep;
    myConnections = e->myConnections;
    myTurnDestination = e->myTurnDestination;
    myPossibleTurnDestination = e->myPossibleTurnDestination;
    // set the node
    myTo = e->myTo;
    myToBorder = e->myToBorder;
    if (e->getSignalOffset() != UNSPECIFIED_SIGNAL_OFFSET) {
        mySignalOffset = e->getSignalOffset();
    } else {
        mySignalOffset += e->getLength();
    }
    computeAngle(); // myEndAngle may be different now
}


bool
NBEdge::hasSignalisedConnectionTo(const NBEdge* const e) const {
    for (std::vector<Connection>::const_iterator i = myConnections.begin(); i != myConnections.end(); ++i) {
        if ((*i).toEdge == e && (*i).tlID != "") {
            return true;
        }
    }
    return false;
}


NBEdge*
NBEdge::getTurnDestination(bool possibleDestination) const {
    if (myTurnDestination == 0 && possibleDestination) {
        return myPossibleTurnDestination;
    }
    return myTurnDestination;
}


std::string
NBEdge::getLaneID(int lane) const {
    return myID + "_" + toString(lane);
}


std::string
NBEdge::getLaneIDInsecure(int lane) const {
    return myID + "_" + toString(lane);
}


bool
NBEdge::isNearEnough2BeJoined2(NBEdge* e, double threshold) const {
    std::vector<double> distances = myGeom.distances(e->getGeometry());
    assert(distances.size() > 0);
    return VectorHelper<double>::maxValue(distances) < threshold;
}


void
NBEdge::addLane(int index, bool recomputeShape, bool recomputeConnections, bool shiftIndices) {
    assert(index <= (int)myLanes.size());
    myLanes.insert(myLanes.begin() + index, Lane(this, ""));
    // copy attributes
    if (myLanes.size() > 1) {
        int templateIndex = index > 0 ? index - 1 : index + 1;
        myLanes[index].speed = myLanes[templateIndex].speed;
        myLanes[index].permissions = myLanes[templateIndex].permissions;
        myLanes[index].preferred = myLanes[templateIndex].preferred;
        myLanes[index].endOffset = myLanes[templateIndex].endOffset;
        myLanes[index].width = myLanes[templateIndex].width;
        myLanes[index].updateParameter(myLanes[templateIndex].getMap());
    }
    const EdgeVector& incs = myFrom->getIncomingEdges();
    if (recomputeShape) {
        computeLaneShapes();
    }
    if (recomputeConnections) {
        for (EdgeVector::const_iterator i = incs.begin(); i != incs.end(); ++i) {
            (*i)->invalidateConnections(true);
        }
        invalidateConnections(true);
    } else if (shiftIndices) {
        // shift outgoing connections above the added lane to the left
        for (Connection& c : myConnections) {
            if (c.fromLane >= index) {
                c.fromLane += 1;
            }
        }
        // shift incoming connections above the added lane to the left
        for (NBEdge* inc : myFrom->getIncomingEdges()) {
            for (Connection& c : inc->myConnections) {
                if (c.toLane >= index) {
                    c.toLane += 1;
                }
            }
        }
        myFrom->shiftTLConnectionLaneIndex(this, +1, index-1);
        myTo->shiftTLConnectionLaneIndex(this, +1, index-1);
    }
}

void
NBEdge::incLaneNo(int by) {
    int newLaneNo = (int)myLanes.size() + by;
    while ((int)myLanes.size() < newLaneNo) {
        // recompute shapes on last addition
        const bool recompute = ((int)myLanes.size() == newLaneNo - 1) && myStep < LANES2LANES_USER;
        addLane((int)myLanes.size(), recompute, recompute, false);
    }
}


void
NBEdge::deleteLane(int index, bool recompute, bool shiftIndices) {
    assert(index < (int)myLanes.size());
    myLanes.erase(myLanes.begin() + index);
    if (recompute) {
        computeLaneShapes();
        const EdgeVector& incs = myFrom->getIncomingEdges();
        for (EdgeVector::const_iterator i = incs.begin(); i != incs.end(); ++i) {
            (*i)->invalidateConnections(true);
        }
        invalidateConnections(true);
    } else if (shiftIndices) {
        // shift outgoing connections above the delete lane to the right
        for (Connection& c : myConnections) {
            if (c.fromLane > index) {
                c.fromLane -= 1;
            }
        }
        // shift incoming connections above the delete lane to the right
        for (NBEdge* inc : myFrom->getIncomingEdges()) {
            for (Connection& c : inc->myConnections) {
                if (c.toLane > index) {
                    c.toLane -= 1;
                }
            }
        }
        myFrom->shiftTLConnectionLaneIndex(this, -1, index);
        myTo->shiftTLConnectionLaneIndex(this, -1, index);
    }
}


void
NBEdge::decLaneNo(int by) {
    int newLaneNo = (int) myLanes.size() - by;
    assert(newLaneNo > 0);
    while ((int)myLanes.size() > newLaneNo) {
        // recompute shapes on last removal
        const bool recompute = (int)myLanes.size() == newLaneNo + 1 && myStep < LANES2LANES_USER;
        deleteLane((int)myLanes.size() - 1, recompute, false);
    }
}


void
NBEdge::markAsInLane2LaneState() {
    assert(myTo->getOutgoingEdges().size() == 0);
    myStep = LANES2LANES_DONE;
}


void
NBEdge::allowVehicleClass(int lane, SUMOVehicleClass vclass) {
    if (lane < 0) { // all lanes are meant...
        for (int i = 0; i < (int)myLanes.size(); i++) {
            allowVehicleClass(i, vclass);
        }
    } else {
        assert(lane < (int)myLanes.size());
        myLanes[lane].permissions |= vclass;
    }
}


void
NBEdge::disallowVehicleClass(int lane, SUMOVehicleClass vclass) {
    if (lane < 0) { // all lanes are meant...
        for (int i = 0; i < (int)myLanes.size(); i++) {
            disallowVehicleClass((int) i, vclass);
        }
    } else {
        assert(lane < (int)myLanes.size());
        myLanes[lane].permissions &= ~vclass;
    }
}


void
NBEdge::preferVehicleClass(int lane, SUMOVehicleClass vclass) {
    if (lane < 0) { // all lanes are meant...
        for (int i = 0; i < (int)myLanes.size(); i++) {
            allowVehicleClass(i, vclass);
        }
    } else {
        assert(lane < (int)myLanes.size());
        myLanes[lane].preferred |= vclass;
    }
}


void
NBEdge::setLaneWidth(int lane, double width) {
    if (lane < 0) {
        // all lanes are meant...
        myLaneWidth = width;
        for (int i = 0; i < (int)myLanes.size(); i++) {
            // ... do it for each lane
            setLaneWidth(i, width);
        }
        return;
    }
    assert(lane < (int)myLanes.size());
    myLanes[lane].width = width;
}


double
NBEdge::getLaneWidth(int lane) const {
    return myLanes[lane].width != UNSPECIFIED_WIDTH
           ? myLanes[lane].width
           : getLaneWidth() != UNSPECIFIED_WIDTH ? getLaneWidth() : SUMO_const_laneWidth;
}


double
NBEdge::getTotalWidth() const {
    double result = 0;
    for (int i = 0; i < (int)myLanes.size(); i++) {
        result += getLaneWidth(i);
    }
    return result;
}

double
NBEdge::getEndOffset(int lane) const {
    return myLanes[lane].endOffset != UNSPECIFIED_OFFSET ? myLanes[lane].endOffset : getEndOffset();
}


void
NBEdge::setEndOffset(int lane, double offset) {
    if (lane < 0) {
        // all lanes are meant...
        myEndOffset = offset;
        for (int i = 0; i < (int)myLanes.size(); i++) {
            // ... do it for each lane
            setEndOffset(i, offset);
        }
        return;
    }
    assert(lane < (int)myLanes.size());
    myLanes[lane].endOffset = offset;
}


void
NBEdge::setSpeed(int lane, double speed) {
    if (lane < 0) {
        // all lanes are meant...
        mySpeed = speed;
        for (int i = 0; i < (int)myLanes.size(); i++) {
            // ... do it for each lane
            setSpeed(i, speed);
        }
        return;
    }
    assert(lane < (int)myLanes.size());
    myLanes[lane].speed = speed;
}

void
NBEdge::setAcceleration(int lane, bool accelRamp) {
    assert(lane >= 0);
    assert(lane < (int)myLanes.size());
    myLanes[lane].accelRamp = accelRamp;
}


void
NBEdge::setLaneShape(int lane, const PositionVector& shape) {
    assert(lane >= 0);
    assert(lane < (int)myLanes.size());
    myLanes[lane].customShape = shape;
}


void
NBEdge::setPermissions(SVCPermissions permissions, int lane) {
    if (lane < 0) {
        for (int i = 0; i < (int)myLanes.size(); i++) {
            // ... do it for each lane
            setPermissions(permissions, i);
        }
    } else {
        assert(lane < (int)myLanes.size());
        myLanes[lane].permissions = permissions;
    }
}


void
NBEdge::setPreferredVehicleClass(SVCPermissions permissions, int lane) {
    if (lane < 0) {
        for (int i = 0; i < (int)myLanes.size(); i++) {
            // ... do it for each lane
            setPreferredVehicleClass(permissions, i);
        }
    } else {
        assert(lane < (int)myLanes.size());
        myLanes[lane].preferred = permissions;
    }
}


SVCPermissions
NBEdge::getPermissions(int lane) const {
    if (lane < 0) {
        SVCPermissions result = 0;
        for (int i = 0; i < (int)myLanes.size(); i++) {
            result |= getPermissions(i);
        }
        return result;
    } else {
        assert(lane < (int)myLanes.size());
        return myLanes[lane].permissions;
    }
}


void
NBEdge::setLoadedLength(double val) {
    myLoadedLength = val;
}


void
NBEdge::dismissVehicleClassInformation() {
    for (std::vector<Lane>::iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        (*i).permissions = SVCAll;
        (*i).preferred = 0;
    }
}


bool
NBEdge::connections_sorter(const Connection& c1, const Connection& c2) {
    if (c1.fromLane != c2.fromLane) {
        return c1.fromLane < c2.fromLane;
    }
    if (c1.toEdge != c2.toEdge) {
        return false; // do not change ordering among toEdges as this is determined by angle in an earlier step
    }
    return c1.toLane < c2.toLane;
}


int
NBEdge::getFirstNonPedestrianLaneIndex(int direction, bool exclusive) const {
    assert(direction == NBNode::FORWARD || direction == NBNode::BACKWARD);
    const int start = (direction == NBNode::FORWARD ? 0 : (int)myLanes.size() - 1);
    const int end = (direction == NBNode::FORWARD ? (int)myLanes.size() : - 1);
    for (int i = start; i != end; i += direction) {
        // SVCAll, does not count as a sidewalk, green verges (permissions = 0) do not count as road
        // in the exclusive case, lanes that allow pedestrians along with any other class also count as road
        if ((exclusive && myLanes[i].permissions != SVC_PEDESTRIAN && myLanes[i].permissions != 0)
                || (myLanes[i].permissions == SVCAll || ((myLanes[i].permissions & SVC_PEDESTRIAN) == 0 && myLanes[i].permissions != 0))) {
            return i;
        }
    }
    return -1;
}


int
NBEdge::getFirstAllowedLaneIndex(int direction) const {
    assert(direction == NBNode::FORWARD || direction == NBNode::BACKWARD);
    const int start = (direction == NBNode::FORWARD ? 0 : (int)myLanes.size() - 1);
    const int end = (direction == NBNode::FORWARD ? (int)myLanes.size() : - 1);
    for (int i = start; i != end; i += direction) {
        if (myLanes[i].permissions != 0) {
            return i;
        }
    }
    return end - direction;
}


std::set<SVCPermissions>
NBEdge::getPermissionVariants(int iStart, int iEnd) const {
    std::set<SVCPermissions> result;
    if (iStart < 0 || iStart >= getNumLanes() || iEnd > getNumLanes()) {
        throw ProcessError("invalid indices iStart " + toString(iStart) + " iEnd " + toString(iEnd) + " for edge with " + toString(getNumLanes()) + " lanes.");
    }
    for (int i = iStart; i < iEnd; ++i) {
        result.insert(getPermissions(i));
    }
    return result;
}


double
NBEdge::getCrossingAngle(NBNode* node) {
    double angle = getAngleAtNode(node) + (getFromNode() == node ? 180.0 : 0.0);
    if (angle < 0) {
        angle += 360.0;
    }
    if (angle >= 360) {
        angle -= 360.0;
    }
    if (gDebugFlag1) {
        std::cout << getID() << " angle=" << getAngleAtNode(node) << " convAngle=" << angle << "\n";
    }
    return angle;
}


NBEdge::Lane
NBEdge::getFirstNonPedestrianLane(int direction) const {
    int index = getFirstNonPedestrianLaneIndex(direction);
    if (index < 0) {
        throw ProcessError("Edge " + getID() + " allows pedestrians on all lanes");
    }
    return myLanes[index];
}

std::string
NBEdge::getSidewalkID() {
    // see IntermodalEdge::getSidewalk()
    for (int i = 0; i < (int)myLanes.size(); i++) {
        if (myLanes[i].permissions == SVC_PEDESTRIAN) {
            return getLaneID(i);
        }
    }
    for (int i = 0; i < (int)myLanes.size(); i++) {
        if ((myLanes[i].permissions & SVC_PEDESTRIAN) != 0) {
            return getLaneID(i);
        }
    }
    return getLaneID(0);
}

void
NBEdge::addSidewalk(double width) {
    addRestrictedLane(width, SVC_PEDESTRIAN);
}


void
NBEdge::restoreSidewalk(std::vector<NBEdge::Lane> oldLanes, PositionVector oldGeometry, std::vector<NBEdge::Connection> oldConnections) {
    restoreRestrictedLane(SVC_PEDESTRIAN, oldLanes, oldGeometry, oldConnections);
}


void
NBEdge::addBikeLane(double width) {
    addRestrictedLane(width, SVC_BICYCLE);
}


void
NBEdge::restoreBikelane(std::vector<NBEdge::Lane> oldLanes, PositionVector oldGeometry, std::vector<NBEdge::Connection> oldConnections) {
    restoreRestrictedLane(SVC_BICYCLE, oldLanes, oldGeometry, oldConnections);
}


void
NBEdge::addRestrictedLane(double width, SUMOVehicleClass vclass) {
    if (myLanes[0].permissions == vclass) {
        WRITE_WARNING("Edge '" + getID() + "' already has a dedicated lane for " + toString(vclass) + "s. Not adding another one.");
        return;
    }
    if (myLaneSpreadFunction == LANESPREAD_CENTER) {
        myGeom.move2side(width / 2);
    }
    // disallow pedestrians on all lanes to ensure that sidewalks are used and
    // crossings can be guessed
    disallowVehicleClass(-1, vclass);
    // add new lane
    myLanes.insert(myLanes.begin(), Lane(this, myLanes[0].getParameter(SUMO_PARAM_ORIGID)));
    myLanes[0].permissions = vclass;
    myLanes[0].width = width;
    // shift outgoing connections to the left
    for (std::vector<Connection>::iterator it = myConnections.begin(); it != myConnections.end(); ++it) {
        Connection& c = *it;
        if (c.fromLane >= 0) {
            c.fromLane += 1;
        }
    }
    // shift incoming connections to the left
    const EdgeVector& incoming = myFrom->getIncomingEdges();
    for (EdgeVector::const_iterator it = incoming.begin(); it != incoming.end(); ++it) {
        (*it)->shiftToLanesToEdge(this, 1);
    }
    myFrom->shiftTLConnectionLaneIndex(this, 1);
    myTo->shiftTLConnectionLaneIndex(this, 1);
    computeLaneShapes();
}


void
NBEdge::restoreRestrictedLane(SUMOVehicleClass vclass, std::vector<NBEdge::Lane> oldLanes, PositionVector oldGeometry, std::vector<NBEdge::Connection> oldConnections) {
    // check that previously lane was transformed
    if (myLanes[0].permissions != vclass) {
        WRITE_WARNING("Edge '" + getID() + "' don't have a dedicated lane for " + toString(vclass) + "s. Cannot be restored");
        return;
    }
    // restore old values
    myGeom = oldGeometry;
    myLanes = oldLanes;
    myConnections = oldConnections;
    // shift incoming connections to the right
    const EdgeVector& incoming = myFrom->getIncomingEdges();
    for (EdgeVector::const_iterator it = incoming.begin(); it != incoming.end(); ++it) {
        (*it)->shiftToLanesToEdge(this, 0);
    }
    // Shift TL conections
    myFrom->shiftTLConnectionLaneIndex(this, 0);
    myTo->shiftTLConnectionLaneIndex(this, 0);
    computeLaneShapes();
}


void
NBEdge::shiftToLanesToEdge(NBEdge* to, int laneOff) {
    /// XXX could we repurpose the function replaceInConnections ?
    for (std::vector<Connection>::iterator it = myConnections.begin(); it != myConnections.end(); ++it) {
        if ((*it).toEdge == to && (*it).toLane >= 0) {
            (*it).toLane += laneOff;
        }
    }
}


void
NBEdge::shiftPositionAtNode(NBNode* node, NBEdge* other) {
    if (myLaneSpreadFunction == LANESPREAD_CENTER && !isRailway(getPermissions())) {
        const int i = (node == myTo ? -1 : 0);
        const int i2 = (node == myTo ? 0 : -1);
        const double dist = myGeom[i].distanceTo2D(node->getPosition());
        const double neededOffset = (getTotalWidth() + getNumLanes() * SUMO_const_laneOffset) / 2;
        const double dist2 = MIN2(myGeom.distance2D(other->getGeometry()[i2]),
                                  other->getGeometry().distance2D(myGeom[i]));
        const double neededOffset2 = neededOffset + (other->getTotalWidth() + other->getNumLanes() * SUMO_const_laneOffset) / 2;
        if (dist < neededOffset && dist2 < neededOffset2) {
            PositionVector tmp = myGeom;
            // @note this doesn't work well for vissim networks
            //tmp.move2side(MIN2(neededOffset - dist, neededOffset2 - dist2));
            try {
                tmp.move2side(neededOffset - dist);
                myGeom[i] = tmp[i];
            } catch (InvalidArgument&) {
                WRITE_WARNING("Could not avoid overlapping shape at node '" + node->getID() + "' for edge '" + getID() + "'");
            }
        }
    }
}


double
NBEdge::getFinalLength() const {
    double result = getLoadedLength();
    if (OptionsCont::getOptions().getBool("no-internal-links") && !hasLoadedLength()) {
        // use length to junction center even if a modified geometry was given
        PositionVector geom = cutAtIntersection(myGeom);
        geom.push_back_noDoublePos(getToNode()->getCenter());
        geom.push_front_noDoublePos(getFromNode()->getCenter());
        result = geom.length();
    }
    return MAX2(result, POSITION_EPS);
}

void
NBEdge::setOrigID(const std::string origID) {
    if (origID != "") {
        for (int i = 0; i < (int)myLanes.size(); i++) {
            myLanes[i].setParameter(SUMO_PARAM_ORIGID, origID);
        }
    } else {
        // do not record empty origID parameter
        for (int i = 0; i < (int)myLanes.size(); i++) {
            myLanes[i].unsetParameter(SUMO_PARAM_ORIGID);
        }
    }
}

/****************************************************************************/
