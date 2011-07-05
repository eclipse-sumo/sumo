/****************************************************************************/
/// @file    NBEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Methods for the representation of a single edge
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include "NBEdgeCont.h"
#include "NBNode.h"
#include "NBNodeCont.h"
#include "NBContHelper.h"
#include "NBHelpers.h"
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static members
// ===========================================================================
const SUMOReal NBEdge::UNSPECIFIED_WIDTH = -1;
const SUMOReal NBEdge::UNSPECIFIED_LOADED_LENGTH = -1;
const SUMOReal NBEdge::UNSPECIFIED_OFFSET = 0;

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * NBEdge::ToEdgeConnectionsAdder-methods
 * ----------------------------------------------------------------------- */
void
NBEdge::ToEdgeConnectionsAdder::execute(const unsigned int lane, const unsigned int virtEdge) throw() {
    // check
    assert(myTransitions.size() > virtEdge);
    // get the approached edge
    NBEdge *succEdge = myTransitions[virtEdge];
    std::vector<unsigned int> lanes;

    // check whether the currently regarded, approached edge has already
    //  a connection starting at the edge which is currently being build
    std::map<NBEdge*, std::vector<unsigned int> >::iterator i=myConnections.find(succEdge);
    if (i!=myConnections.end()) {
        // if there were already lanes assigned, get them
        lanes = (*i).second;
    }

    // check whether the current lane was already used to connect the currently
    //  regarded approached edge
    std::vector<unsigned int>::iterator j=find(lanes.begin(), lanes.end(), lane);
    if (j==lanes.end()) {
        // if not, add it to the list
        lanes.push_back(lane);
    }
    // set information about connecting lanes
    myConnections[succEdge] = lanes;
}



/* -------------------------------------------------------------------------
 * NBEdge::MainDirections-methods
 * ----------------------------------------------------------------------- */
NBEdge::MainDirections::MainDirections(const EdgeVector &outgoing,
                                       NBEdge *parent, NBNode *to) {
    if (outgoing.size()==0) {
        return;
    }
    // check whether the right turn has a higher priority
    assert(outgoing.size()>0);
    if (outgoing[0]->getJunctionPriority(to)==1) {
        myDirs.push_back(MainDirections::DIR_RIGHTMOST);
    }
    // check whether the left turn has a higher priority
    if (outgoing.back()->getJunctionPriority(to)==1) {
        // ok, the left turn belongs to the higher priorised edges on the junction
        //  let's check, whether it has also a higher priority (lane number/speed)
        //  than the current
        EdgeVector tmp(outgoing);
        sort(tmp.begin(), tmp.end(), NBContHelper::edge_similar_direction_sorter(parent));
        if (outgoing.back()->getPriority()>tmp[0]->getPriority()) {
            myDirs.push_back(MainDirections::DIR_LEFTMOST);
        } else {
            if (outgoing.back()->getNumLanes()>tmp[0]->getNumLanes()) {
                myDirs.push_back(MainDirections::DIR_LEFTMOST);
            }
        }
    }
    // check whether the forward direction has a higher priority
    //  try to get the forward direction
    EdgeVector tmp(outgoing);
    sort(tmp.begin(), tmp.end(), NBContHelper::edge_similar_direction_sorter(parent));
    NBEdge *edge = *(tmp.begin());
    // check whether it has a higher priority and is going straight
    if (edge->getJunctionPriority(to)==1 && to->getDirection(parent, edge)==LINKDIR_STRAIGHT) {
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
    return find(myDirs.begin(), myDirs.end(), d)!=myDirs.end();
}



/* -------------------------------------------------------------------------
 * NBEdge-methods
 * ----------------------------------------------------------------------- */
NBEdge::NBEdge(const std::string &id, NBNode *from, NBNode *to,
               std::string type, SUMOReal speed, unsigned int nolanes,
               int priority, SUMOReal width, SUMOReal offset, 
               const std::string &streetName,
               LaneSpreadFunction spread) throw(ProcessError) :
    Named(StringUtils::convertUmlaute(id)),
    myStep(INIT),
    myType(StringUtils::convertUmlaute(type)),
    myFrom(from), myTo(to), myAngle(0),
    myPriority(priority), mySpeed(speed),
    myTurnDestination(0),
    myFromJunctionPriority(-1), myToJunctionPriority(-1),
    myLaneSpreadFunction(spread), myOffset(offset), myWidth(width),
    myLoadedLength(UNSPECIFIED_LOADED_LENGTH), myAmLeftHand(false), myAmTurningWithAngle(0), myAmTurningOf(0),
    myAmInnerEdge(false), myAmMacroscopicConnector(false),
    myStreetName(streetName)
{
    init(nolanes, false);
}


NBEdge::NBEdge(const std::string &id, NBNode *from, NBNode *to,
               std::string type, SUMOReal speed, unsigned int nolanes,
               int priority, SUMOReal width, SUMOReal offset, 
               PositionVector geom,
               const std::string &streetName,
               LaneSpreadFunction spread, bool tryIgnoreNodePositions) throw(ProcessError) :
    Named(StringUtils::convertUmlaute(id)),
    myStep(INIT),
    myType(StringUtils::convertUmlaute(type)),
    myFrom(from), myTo(to), myAngle(0),
    myPriority(priority), mySpeed(speed),
    myTurnDestination(0),
    myFromJunctionPriority(-1), myToJunctionPriority(-1),
    myGeom(geom), myLaneSpreadFunction(spread), myOffset(offset), myWidth(width),
    myLoadedLength(UNSPECIFIED_LOADED_LENGTH), myAmLeftHand(false), myAmTurningWithAngle(0), myAmTurningOf(0),
    myAmInnerEdge(false), myAmMacroscopicConnector(false),
    myStreetName(streetName)
{
    init(nolanes, tryIgnoreNodePositions);
}


NBEdge::NBEdge(const std::string &id, NBNode *from, NBNode *to, NBEdge *tpl) :
    Named(StringUtils::convertUmlaute(id)),
    myStep(INIT),
    myType(tpl->getTypeID()),
    myFrom(from), myTo(to), myAngle(0),
    myPriority(tpl->getPriority()), mySpeed(tpl->getSpeed()),
    myTurnDestination(0),
    myFromJunctionPriority(-1), myToJunctionPriority(-1),
    myLaneSpreadFunction(tpl->getLaneSpreadFunction()), 
    myOffset(tpl->getOffset()), 
    myWidth(tpl->getWidth()),
    myLoadedLength(UNSPECIFIED_LOADED_LENGTH), myAmLeftHand(false), myAmTurningWithAngle(0), myAmTurningOf(0),
    myAmInnerEdge(false), myAmMacroscopicConnector(false),
    myStreetName(tpl->getStreetName())
{
    init(tpl->getNumLanes(), false);
    for (unsigned int i = 0; i < getNumLanes(); i++) {
        setLaneSpeed(i, tpl->getLaneSpeed(i));
        setVehicleClasses( tpl->getAllowedVehicleClasses(i), tpl->getDisallowedVehicleClasses(i), i);
    }
}

void
NBEdge::reinit(NBNode *from, NBNode *to, const std::string &type,
               SUMOReal speed, unsigned int nolanes, int priority,
               PositionVector geom, SUMOReal width, SUMOReal offset, LaneSpreadFunction spread) throw(ProcessError) {
    if (myFrom!=from) {
        myFrom->removeOutgoing(this);
    }
    if (myTo!=to) {
        myTo->removeIncoming(this);
    }
    myType = StringUtils::convertUmlaute(type);
    myFrom = from;
    myTo = to;
    myPriority = priority;
    mySpeed = speed;
    //?myTurnDestination(0),
    //?myFromJunctionPriority(-1), myToJunctionPriority(-1),
    myGeom = geom;
    myLaneSpreadFunction = spread;
    myOffset = offset;
    myWidth = width;
    myLoadedLength = UNSPECIFIED_LOADED_LENGTH;
    //?, myAmTurningWithAngle(0), myAmTurningOf(0),
    //?myAmInnerEdge(false), myAmMacroscopicConnector(false)
    init(nolanes, false);
}


void
NBEdge::init(unsigned int noLanes, bool tryIgnoreNodePositions) throw(ProcessError) {
    if (noLanes==0) {
        throw ProcessError("Edge '" + myID + "' needs at least one lane.");
    }
    if (myFrom==0||myTo==0) {
        throw ProcessError("At least one of edge's '" + myID + "' nodes is not known.");
    }
    // revisit geometry
    //  should have at least two points at the end...
    //  and in dome cases, the node positions must be added
    myGeom.removeDoublePoints();
    if (!tryIgnoreNodePositions||myGeom.size()<2) {
        if (myGeom.size()==0) {
            myGeom.push_back(myTo->getPosition());
            myGeom.push_front(myFrom->getPosition());
        } else {
            myGeom.push_back_noDoublePos(myTo->getPosition());
            myGeom.push_front_noDoublePos(myFrom->getPosition());
        }
    }
    if(myGeom.size()<2) {
        myGeom.clear();
        myGeom.push_back(myTo->getPosition());
        myGeom.push_front(myFrom->getPosition());
    }
    if(myGeom.size()==2&&myGeom[0]==myGeom[1]) {
        WRITE_ERROR("Edge's '" + myID + "' from- and to-node are at the same position.");
        myGeom[1].add(Position(POSITION_EPS, POSITION_EPS));
    }
    //
    myAngle = NBHelpers::angle(
                  myFrom->getPosition().x(), myFrom->getPosition().y(),
                  myTo->getPosition().x(), myTo->getPosition().y()
              );
    myFrom->addOutgoingEdge(this);
    myTo->addIncomingEdge(this);
    // prepare container
    myLength = myFrom->getPosition().distanceTo(myTo->getPosition());
    assert(myGeom.size()>=2);
    myLanes.clear();
    for (unsigned int i=0; i<noLanes; i++) {
        Lane l;
        l.speed = mySpeed;
        l.offset = myOffset;
        l.width = myWidth;
        myLanes.push_back(l);
    }
    computeLaneShapes();
}


NBEdge::~NBEdge() throw() {}


// ----------- Edge geometry access and computation
const PositionVector 
NBEdge::getInnerGeometry() const {
    PositionVector result = getGeometry();
    result.pop_front();
    result.pop_back();
    return result;
}


bool 
NBEdge::hasDefaultGeometry() const {
    return myGeom.size() == 2 && hasDefaultGeometryEndpoints();
}


bool 
NBEdge::hasDefaultGeometryEndpoints() const {
    return myGeom.getBegin() == myFrom->getPosition() &&
        myGeom.getEnd() == myTo->getPosition();
}


void
NBEdge::setGeometry(const PositionVector &s, bool inner) throw() {
    Position begin = myGeom.getBegin(); // may differ from node position
    Position end = myGeom.getEnd(); // may differ from node position
    myGeom = s;
    if (inner) {
        myGeom.push_front(begin);
        myGeom.push_back(end);
    }
    computeLaneShapes();
}


void
NBEdge::computeEdgeShape() throw() {
    unsigned int i;
    for (i=0; i<myLanes.size(); i++) {
        PositionVector &shape = myLanes[i].shape;
        // get lane begin and end
        Line lb = Line(shape[0], shape[1]);
        Line le = Line(shape[-1], shape[-2]);
        lb.extrapolateBy(100.0);
        le.extrapolateBy(100.0);
        //
        PositionVector old = shape;
        Position nb, ne;
        // lane begin
        if (myFrom->getShape().intersects(shape)) {
            // get the intersection position with the junction
            DoubleVector pbv = shape.intersectsAtLengths(myFrom->getShape());
            if (pbv.size()>0) {
                SUMOReal pb = VectorHelper<SUMOReal>::maxValue(pbv);
                if (pb>=0&&pb<=shape.length()) {
                    shape = shape.getSubpart(pb, shape.length());
                }
            }
        } else if (myFrom->getShape().intersects(lb.p1(), lb.p2())) {
            DoubleVector pbv = lb.intersectsAtLengths(myFrom->getShape());
            if (pbv.size()>0) {
                SUMOReal pb = VectorHelper<SUMOReal>::maxValue(pbv);
                if (pb>=0) {
                    shape.eraseAt(0);
                    shape.push_front_noDoublePos(lb.getPositionAtDistance(pb));
                }
            }
        }
        // lane end
        if (myTo->getShape().intersects(shape)) {
            // get the intersection position with the junction
            DoubleVector pev = shape.intersectsAtLengths(myTo->getShape());
            if (pev.size()>0) {
                SUMOReal pe = VectorHelper<SUMOReal>::minValue(pev);
                if (pe>=0&&pe<=shape.length()) {
                    shape = shape.getSubpart(0, pe);
                }
            }
        } else if (myTo->getShape().intersects(le.p1(), le.p2())) {
            DoubleVector pev = le.intersectsAtLengths(myTo->getShape());
            if (pev.size()>0) {
                SUMOReal pe = VectorHelper<SUMOReal>::maxValue(pev);
                if (pe>=0) {
                    shape.eraseAt((int) shape.size()-1);
                    shape.push_back(le.getPositionAtDistance(pe));
                }
            }
        }
        if (shape.length() < POSITION_EPS) {
            WRITE_MESSAGE("Lane '" + myID + "' has calculated shape length near zero. Revert it back to old shape.");
            shape = old;
        } else {
            Line lc(shape[0], shape[-1]);
            Line lo(old[0], old[-1]);
            if (135<GeomHelper::getMinAngleDiff(lc.atan2DegreeAngle(), lo.atan2DegreeAngle())) {
                shape = shape.reverse();
            }
        }
    }
    // recompute edge's length
    SUMOReal length = 0;
    for (i=0; i<myLanes.size(); i++) {
        assert(myLanes[i].shape.length()>0);
        length += myLanes[i].shape.length();
    }
    myLength = length / (SUMOReal) myLanes.size();
}


const PositionVector &
NBEdge::getLaneShape(unsigned int i) const throw() {
    return myLanes[i].shape;
}


void
NBEdge::setLaneSpreadFunction(LaneSpreadFunction spread) throw() {
    myLaneSpreadFunction = spread;
}


void
NBEdge::addGeometryPoint(int index, const Position &p) throw() {
    myGeom.insertAt(index, p);
}


void
NBEdge::reshiftPosition(SUMOReal xoff, SUMOReal yoff) throw() {
    myGeom.reshiftRotate(xoff, yoff, 0);
    for (unsigned int i=0; i<myLanes.size(); i++) {
        myLanes[i].shape.reshiftRotate(xoff, yoff, 0);
    }
}


// ----------- Setting and getting connections
bool
NBEdge::addEdge2EdgeConnection(NBEdge *dest) throw() {
    if (myStep==INIT_REJECT_CONNECTIONS) {
        return true;
    }
    // check whether the node was merged and now a connection between
    //  not matching edges is tried to be added
    //  This happens f.e. within the ptv VISSIM-example "Beijing"
    if (dest!=0 && myTo!=dest->myFrom) {
        return false;
    }
    if (find_if(myConnections.begin(), myConnections.end(), connections_toedge_finder(dest))==myConnections.end()) {
        myConnections.push_back(Connection(-1, dest, -1));
    }
    if (myStep<EDGE2EDGES) {
        myStep = EDGE2EDGES;
    }
    return true;
}


bool
NBEdge::addLane2LaneConnection(unsigned int from, NBEdge *dest,
                               unsigned int toLane, Lane2LaneInfoType type,
                               bool mayUseSameDestination,
                               bool mayDefinitelyPass) throw() {
    if (myStep==INIT_REJECT_CONNECTIONS) {
        return true;
    }
    // check whether the node was merged and now a connection between
    //  not matching edges is tried to be added
    //  This happens f.e. within the ptv VISSIM-example "Beijing"
    if (myTo!=dest->myFrom) {
        return false;
    }
    if (!addEdge2EdgeConnection(dest)) {
        return false;
    }
    setConnection(from, dest, toLane, type, mayUseSameDestination, mayDefinitelyPass);
    return true;
}


bool
NBEdge::addLane2LaneConnections(unsigned int fromLane,
                                NBEdge *dest, unsigned int toLane,
                                unsigned int no, Lane2LaneInfoType type,
                                bool invalidatePrevious,
                                bool mayDefinitelyPass) throw() {
    if (invalidatePrevious) {
        invalidateConnections(true);
    }
    bool ok = true;
    for (unsigned int i=0; i<no&&ok; i++) {
        ok &= addLane2LaneConnection(fromLane+i, dest, toLane+i, type, false, mayDefinitelyPass);
    }
    return ok;
}


void
NBEdge::setConnection(unsigned int lane, NBEdge *destEdge,
                      unsigned int destLane, Lane2LaneInfoType type,
                      bool mayUseSameDestination,
                      bool mayDefinitelyPass) throw() {
    if (myStep==INIT_REJECT_CONNECTIONS) {
        return;
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
        return;
    }
    if (find_if(myConnections.begin(), myConnections.end(), connections_finder(lane, destEdge, destLane))!=myConnections.end()) {
        return;
    }
    if (myLanes.size()<=lane) {
        WRITE_ERROR("Could not set connection from '" + getLaneID(lane) + "' to '" + destEdge->getLaneID(destLane) + "'.");
        return;
    }
    for (std::vector<Connection>::iterator i=myConnections.begin(); i!=myConnections.end();) {
        if ((*i).toEdge==destEdge && ((*i).fromLane==-1 || (*i).toLane==-1)) {
            i = myConnections.erase(i);
        } else {
            ++i;
        }
    }
    myConnections.push_back(Connection(lane, destEdge, destLane));
    if (mayDefinitelyPass) {
        myConnections.back().mayDefinitelyPass = true;
    }
    if (type==L2L_USER) {
        myStep = LANES2LANES_USER;
    } else {
        // check whether we have to take another look at it later
        if (type==L2L_COMPUTED) {
            // yes, the connection was set using an algorithm which requires a recheck
            myStep = LANES2LANES_RECHECK;
        } else {
            // ok, let's only not recheck it if we did no add something that has to be recheked
            if (myStep!=LANES2LANES_RECHECK) {
                myStep = LANES2LANES_DONE;
            }
        }
    }
}


std::vector<NBEdge::Connection>
NBEdge::getConnectionsFromLane(unsigned int lane) const throw() {
    std::vector<NBEdge::Connection> ret;
    for (std::vector<Connection>::const_iterator i=myConnections.begin(); i!=myConnections.end(); ++i) {
        if ((*i).fromLane==static_cast<int>(lane)) {
            ret.push_back(*i);
        }
    }
    return ret;
}


bool
NBEdge::hasConnectionTo(NBEdge *destEdge, unsigned int destLane) const throw() {
    return destEdge!=0&&find_if(myConnections.begin(), myConnections.end(), connections_toedgelane_finder(destEdge, destLane))!=myConnections.end();
}
// -----------


int
NBEdge::getJunctionPriority(const NBNode * const node) const {
    if (node==myFrom) {
        return myFromJunctionPriority;
    } else {
        return myToJunctionPriority;
    }
}


void
NBEdge::setJunctionPriority(const NBNode * const node, int prio) {
    if (node==myFrom) {
        myFromJunctionPriority = prio;
    } else {
        myToJunctionPriority = prio;
    }
}


void
NBEdge::computeTurningDirections() {
    myTurnDestination = 0;
    EdgeVector outgoing = myTo->getOutgoingEdges();
    for (EdgeVector::iterator i=outgoing.begin(); i!=outgoing.end(); i++) {
        NBEdge *outedge = *i;
        if (myConnections.size()!=0&&!isConnectedTo(outedge)) {
            continue;
        }
        SUMOReal relAngle =
            NBHelpers::relAngle(getAngle(*myTo), outedge->getAngle(*myTo));
        // do not append the turnaround
        if (fabs(relAngle)>160) {
            setTurningDestination(outedge);
        }
    }
}


SUMOReal
NBEdge::getAngle(const NBNode &atNode) const {
    if (&atNode==myFrom) {
        return myGeom.getBegLine().atan2DegreeAngle();
    } else {
        assert(&atNode==myTo);
        return myGeom.getEndLine().atan2DegreeAngle();
    }
}


void
NBEdge::setTurningDestination(NBEdge *e) {
    SUMOReal cur = fabs(NBHelpers::relAngle(getAngle(), e->getAngle()));
    SUMOReal old =
        myTurnDestination==0
        ? 0
        : fabs(NBHelpers::relAngle(getAngle(), myTurnDestination->getAngle()));
    if (cur>old
            &&
            e->acceptBeingTurning(this)) {

        myTurnDestination = e;
    }
}


bool
NBEdge::acceptBeingTurning(NBEdge *e) {
    if (e==myAmTurningOf) {
        return true;
    }
    SUMOReal angle = fabs(NBHelpers::relAngle(getAngle(), e->getAngle()));
    if (myAmTurningWithAngle>angle) {
        return false;
    }
    if (myAmTurningWithAngle==angle) {
        return false; // !!! ok, this happens only within a cell-network (backgrnd), we have to take a further look sometime
    }
    NBEdge *previous = myAmTurningOf;
    myAmTurningWithAngle = angle;
    myAmTurningOf = e;
    if (previous!=0) {
        previous->computeTurningDirections();
    }
    return true;
}


void
NBEdge::setLaneSpeed(unsigned int lane, SUMOReal speed) {
    myLanes[lane].speed = speed;
}


SUMOReal
NBEdge::getLaneSpeed(unsigned int lane) const {
    return myLanes[lane].speed;
}


void
NBEdge::computeLaneShapes() throw() {
    // vissim needs this
    if (myFrom==myTo) {
        return;
    }
    // build the shape of each lane
    for (unsigned int i=0; i<myLanes.size(); i++) {
        try {
            myLanes[i].shape = computeLaneShape(i);
        } catch (InvalidArgument &e) {
            WRITE_WARNING("In edge '" + getID() + "': lane shape could not been determined (" + e.what() + ")");
            myLanes[i].shape = myGeom;
        }
    }
}


PositionVector
NBEdge::computeLaneShape(unsigned int lane) throw(InvalidArgument) {
    PositionVector shape;
    bool haveWarned = false;
    for (int i=0; i<(int) myGeom.size(); i++) {
        if (i==0) {
            Position from = myGeom[i];
            Position to = myGeom[i+1];
            std::pair<SUMOReal, SUMOReal> offsets = laneOffset(from, to, SUMO_const_laneWidthAndOffset, (unsigned int)(myLanes.size()-1-lane));
            shape.push_back(
                // (methode umbenennen; was heisst hier "-")
                Position(from.x()-offsets.first, from.y()-offsets.second));
        } else if (i==static_cast<int>(myGeom.size()-1)) {
            Position from = myGeom[i-1];
            Position to = myGeom[i];
            std::pair<SUMOReal, SUMOReal> offsets = laneOffset(from, to, SUMO_const_laneWidthAndOffset, (unsigned int)(myLanes.size()-1-lane));
            shape.push_back(
                // (methode umbenennen; was heisst hier "-")
                Position(to.x()-offsets.first, to.y()-offsets.second));
        } else {
            Position from = myGeom[i-1];
            Position me = myGeom[i];
            Position to = myGeom[i+1];
            std::pair<SUMOReal, SUMOReal> offsets = laneOffset(from, me, SUMO_const_laneWidthAndOffset, (unsigned int)(myLanes.size()-1-lane));
            std::pair<SUMOReal, SUMOReal> offsets2 = laneOffset(me, to, SUMO_const_laneWidthAndOffset, (unsigned int)(myLanes.size()-1-lane));
            Line l1(
                Position(from.x()-offsets.first, from.y()-offsets.second),
                Position(me.x()-offsets.first, me.y()-offsets.second));
            l1.extrapolateBy(100);
            Line l2(
                Position(me.x()-offsets2.first, me.y()-offsets2.second),
                Position(to.x()-offsets2.first, to.y()-offsets2.second));
            SUMOReal angle = GeomHelper::getCWAngleDiff(l1.atan2DegreeAngle(), l2.atan2DegreeAngle());
            if (angle<10.||angle>350.) {
                shape.push_back(
                    // (methode umbenennen; was heisst hier "-")
                    Position(me.x()-offsets.first, me.y()-offsets.second));
                continue;
            }
            l2.extrapolateBy(100);
            if (l1.intersects(l2)) {
                shape.push_back(l1.intersectsAt(l2));
            } else {
                if (!haveWarned) {
                    WRITE_WARNING("In lane '" + getLaneID(lane) + "': Could not build shape.");
                    haveWarned = true;
                }
            }
        }
    }
    return shape;
}


std::pair<SUMOReal, SUMOReal>
NBEdge::laneOffset(const Position &from, const Position &to,
                   SUMOReal lanewidth, unsigned int lane) throw(InvalidArgument) {
    return laneOffset(from, to, lanewidth, lane, 
            myLanes.size(), myLaneSpreadFunction, myAmLeftHand);
}


std::pair<SUMOReal, SUMOReal>
NBEdge::laneOffset(const Position &from, const Position &to,
                   SUMOReal lanewidth, unsigned int lane, 
                   size_t noLanes, LaneSpreadFunction lsf, bool leftHand) {
    std::pair<SUMOReal, SUMOReal> offsets =
        GeomHelper::getNormal90D_CW(from, to, lanewidth);
    SUMOReal xoff = offsets.first / (SUMOReal) 2.0;
    SUMOReal yoff = offsets.second / (SUMOReal) 2.0;
    if (lsf==LANESPREAD_RIGHT) {
        xoff += (offsets.first * (SUMOReal) lane);
        yoff += (offsets.second * (SUMOReal) lane);
    } else {
        xoff += (offsets.first * (SUMOReal) lane) - (offsets.first * (SUMOReal) noLanes / (SUMOReal) 2.0);
        yoff += (offsets.second * (SUMOReal) lane) - (offsets.second * (SUMOReal) noLanes / (SUMOReal) 2.0);
    }
    if (leftHand) {
        return std::pair<SUMOReal, SUMOReal>(-xoff, -yoff);
    } else {
        return std::pair<SUMOReal, SUMOReal>(xoff, yoff);
    }
}


bool
NBEdge::hasRestrictions() const {
    for (std::vector<Lane>::const_iterator i=myLanes.begin(); i!=myLanes.end(); ++i) {
        if ((*i).allowed.size()!=0 || (*i).notAllowed.size()!=0) {
            return true;
        }
    }
    return false;
}


bool
NBEdge::hasLaneSpecificAllow() const {
    std::vector<Lane>::const_iterator i=myLanes.begin();
    const SUMOVehicleClasses &allowed = i->allowed;
    i++;
    for (; i!=myLanes.end(); ++i) {
        if (i->allowed != allowed) {
            return true;
        }
    }
    return false;
}


bool
NBEdge::hasLaneSpecificDisallow() const {
    std::vector<Lane>::const_iterator i=myLanes.begin();
    const SUMOVehicleClasses &notAllowed = i->notAllowed;
    i++;
    for (; i!=myLanes.end(); ++i) {
        if (i->notAllowed != notAllowed) {
            return true;
        }
    }
    return false;
}


bool
NBEdge::hasLaneSpecificWidth() const {
    for (std::vector<Lane>::const_iterator i=myLanes.begin(); i!=myLanes.end(); ++i) {
        if (i->width != getWidth()) {
            return true;
        }
    }
    return false;
}


bool
NBEdge::hasLaneSpecificSpeed() const {
    for (std::vector<Lane>::const_iterator i=myLanes.begin(); i!=myLanes.end(); ++i) {
        if (i->speed != getSpeed()) {
            return true;
        }
    }
    return false;
}


bool
NBEdge::hasLaneSpecificOffset() const {
    for (std::vector<Lane>::const_iterator i=myLanes.begin(); i!=myLanes.end(); ++i) {
        if (i->offset != getOffset()) {
            return true;
        }
    }
    return false;
}


bool 
NBEdge::needsLaneSpecificOutput() const {
    return hasRestrictions() || hasLaneSpecificSpeed() || hasLaneSpecificWidth() || hasLaneSpecificOffset();
}



bool
NBEdge::computeEdge2Edges() {
    // return if this relationship has been build in previous steps or
    //  during the import
    if (myStep>=EDGE2EDGES) {
        return true;
    }
    if (myConnections.size()==0) {
        const EdgeVector &o = myTo->getOutgoingEdges();
        for (EdgeVector::const_iterator i=o.begin(); i!=o.end(); ++i) {
            myConnections.push_back(Connection(-1, *i, -1));
        }
    }
    myStep = EDGE2EDGES;
    return true;
}


bool
NBEdge::computeLanes2Edges() {
    // return if this relationship has been build in previous steps or
    //  during the import
    if (myStep>=LANES2EDGES) {
        return true;
    }
    assert(myStep==EDGE2EDGES);
    // get list of possible outgoing edges sorted by direction clockwise
    //  the edge in the backward direction (turnaround) is not in the list
    const EdgeVector *edges = getConnectedSorted();
    if (myConnections.size()!=0&&edges->size()==0) {
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
    std::vector<unsigned int> connNumbersPerLane(myLanes.size(), 0);
    for (std::vector<Connection>::iterator i=myConnections.begin(); i!=myConnections.end();) {
        if ((*i).toEdge==0||(*i).fromLane<0||(*i).toLane<0) {
            i = myConnections.erase(i);
        } else {
            if ((*i).fromLane>=0) {
                ++connNumbersPerLane[(*i).fromLane];
            }
            ++i;
        }
    }
    if (myStep!=LANES2LANES_DONE&&myStep!=LANES2LANES_USER) {
        // check #1:
        // If there is a lane with no connections and any neighbour lane has
        //  more than one connections, try to move one of them.
        // This check is only done for edges which connections were assigned
        //  using the standard algorithm.
        for (unsigned int i=0; i<myLanes.size(); i++) {
            if (connNumbersPerLane[i]==0) {
                if (i>0&&connNumbersPerLane[i-1]>1) {
                    moveConnectionToLeft(i-1);
                } else if (i<myLanes.size()-1&&connNumbersPerLane[i+1]>1) {
                    moveConnectionToRight(i+1);
                }
            }
        }
    }
    return true;
}


void
NBEdge::moveConnectionToLeft(unsigned int lane) {
    unsigned int index = 0;
    if (myAmLeftHand) {
        for (int i=(int) myConnections.size()-1; i>=0; --i) {
            if (myConnections[i].fromLane==static_cast<int>(lane)&&getTurnDestination()!=myConnections[i].toEdge) {
                index = i;
            }
        }
    } else {
        for (unsigned int i=0; i<myConnections.size(); ++i) {
            if (myConnections[i].fromLane==static_cast<int>(lane)) {
                index = i;
            }
        }
    }
    std::vector<Connection>::iterator i = myConnections.begin() + index;
    Connection c = *i;
    myConnections.erase(i);
    setConnection(lane+1, c.toEdge, c.toLane, L2L_VALIDATED, false);
}


void
NBEdge::moveConnectionToRight(unsigned int lane) {
    if (myAmLeftHand) {
        for (int i=(int) myConnections.size()-1; i>=0; --i) {
            if (myConnections[i].fromLane==static_cast<int>(lane)&&getTurnDestination()!=myConnections[i].toEdge) {
                Connection c = myConnections[i];
                myConnections.erase(myConnections.begin() + i);
                setConnection(lane-1, c.toEdge, c.toLane, L2L_VALIDATED, false);
                return;
            }
        }
    } else {
        for (std::vector<Connection>::iterator i=myConnections.begin(); i!=myConnections.end(); ++i) {
            if ((*i).fromLane==static_cast<int>(lane)) {
                Connection c = *i;
                i = myConnections.erase(i);
                setConnection(lane-1, c.toEdge, c.toLane, L2L_VALIDATED, false);
                return;
            }
        }
    }
}


std::vector<int>
NBEdge::getConnectionLanes(NBEdge *currentOutgoing) const {
    std::vector<int> ret;
    if (currentOutgoing!=myTurnDestination) {
        for (std::vector<Connection>::const_iterator i=myConnections.begin(); i!=myConnections.end(); ++i) {
            if ((*i).toEdge==currentOutgoing) {
                ret.push_back((*i).fromLane);
            }
        }
    }
    return ret;
}


const EdgeVector *
NBEdge::getConnectedSorted() {
    // check whether connections exist and if not, use edges from the node
    EdgeVector outgoing;
    if (myConnections.size()==0) {
        outgoing = myTo->getOutgoingEdges();
    } else {
        for (std::vector<Connection>::const_iterator i=myConnections.begin(); i!=myConnections.end(); ++i) {
            if (find(outgoing.begin(), outgoing.end(), (*i).toEdge)==outgoing.end()) {
                outgoing.push_back((*i).toEdge);
            }
        }
    }
    // allocate the sorted container
    unsigned int size = (unsigned int) outgoing.size();
    EdgeVector *edges = new EdgeVector();
    edges->reserve(size);
    for (EdgeVector::const_iterator i=outgoing.begin(); i!=outgoing.end(); i++) {
        NBEdge *outedge = *i;
        if (outedge!=0&&outedge!=myTurnDestination) {
            edges->push_back(outedge);
        }
    }
    sort(edges->begin(), edges->end(), NBContHelper::relative_edge_sorter(this, myTo));
    return edges;
}


void
NBEdge::divideOnEdges(const EdgeVector *outgoing) {
    if (outgoing->size()==0) {
        // we have to do this, because the turnaround may have been added before
        myConnections.clear();
        return;
    }
    // precompute priorities; needed as some kind of assumptions for
    //  priorities of directions (see preparePriorities)
    std::vector<unsigned int> *priorities = preparePriorities(outgoing);

    // compute the sum of priorities (needed for normalisation)
    unsigned int prioSum = computePrioritySum(priorities);
    // compute the resulting number of lanes that should be used to
    //  reach the following edge
    unsigned int size = (unsigned int) outgoing->size();
    std::vector<SUMOReal> resultingLanes;
    resultingLanes.reserve(size);
    SUMOReal sumResulting = 0; // the sum of resulting lanes
    SUMOReal minResulting = 10000; // the least number of lanes to reach an edge
    unsigned int i;
    for (i=0; i<size; i++) {
        // res will be the number of lanes which are meant to reach the
        //  current outgoing edge
        SUMOReal res =
            (SUMOReal)(*priorities)[i] *
            (SUMOReal) myLanes.size() / (SUMOReal) prioSum;
        // do not let this number be greater than the number of available lanes
        if (res>myLanes.size()) {
            res = (SUMOReal) myLanes.size();
        }
        // add it to the list
        resultingLanes.push_back(res);
        sumResulting += res;
        if (minResulting>res) {
            minResulting = res;
        }
    }
    // compute the number of virtual edges
    //  a virtual edge is used as a replacement for a real edge from now on
    //  it shall ollow to divide the existing lanes on this structure without
    //  regarding the structure of outgoing edges
    sumResulting += minResulting / (SUMOReal) 2.;
    unsigned int noVirtual = (unsigned int)(sumResulting / minResulting);
    // compute the transition from virtual to real edges
    EdgeVector transition;
    transition.reserve(size);
    for (i=0; i<size; i++) {
        // tmpNo will be the number of connections from this edge
        //  to the next edge
        assert(i<resultingLanes.size());
        SUMOReal tmpNo = (SUMOReal) resultingLanes[i] / (SUMOReal) minResulting;
        for (SUMOReal j=0; j<tmpNo; j++) {
            assert(outgoing->size()>i);
            transition.push_back((*outgoing)[i]);
        }
    }

    // assign lanes to edges
    //  (conversion from virtual to real edges is done)
    ToEdgeConnectionsAdder adder(transition);
    Bresenham::compute(&adder, static_cast<unsigned int>(myLanes.size()), noVirtual);
    const std::map<NBEdge*, std::vector<unsigned int> > &l2eConns = adder.getBuiltConnections();
    myConnections.clear();
    for (std::map<NBEdge*, std::vector<unsigned int> >::const_iterator i=l2eConns.begin(); i!=l2eConns.end(); ++i) {
        const std::vector<unsigned int> lanes = (*i).second;
        for (std::vector<unsigned int>::const_iterator j=lanes.begin(); j!=lanes.end(); ++j) {
            if (myAmLeftHand) {
                myConnections.push_back(Connection(int(myLanes.size() - 1 - *j), (*i).first, -1));
            } else {
                myConnections.push_back(Connection(int(*j), (*i).first, -1));
            }
        }
    }
    delete priorities;
}


std::vector<unsigned int> *
NBEdge::preparePriorities(const EdgeVector *outgoing) {
    // copy the priorities first
    std::vector<unsigned int> *priorities = new std::vector<unsigned int>();
    if (outgoing->size()==0) {
        return priorities;
    }
    priorities->reserve(outgoing->size());
    EdgeVector::const_iterator i;
    for (i=outgoing->begin(); i!=outgoing->end(); i++) {
        int prio = (*i)->getJunctionPriority(myTo);
        assert((prio+1)*2>0);
        prio = (prio+1) * 2;
        priorities->push_back(prio);
    }
    // when the right turning direction has not a higher priority, divide
    //  the importance by 2 due to the possibility to leave the junction
    //  faster from this lane
    MainDirections mainDirections(*outgoing, this, myTo);
    EdgeVector tmp(*outgoing);
    sort(tmp.begin(), tmp.end(), NBContHelper::edge_similar_direction_sorter(this));
    i=find(outgoing->begin(), outgoing->end(), *(tmp.begin()));
    unsigned int dist = (unsigned int) distance(outgoing->begin(), i);
    if (dist!=0&&!mainDirections.includes(MainDirections::DIR_RIGHTMOST)) {
        assert(priorities->size()>0);
        (*priorities)[0] = (*priorities)[0] / 2;
    }
    // HEURISTIC:
    // when no higher priority exists, let the forward direction be
    //  the main direction
    if (mainDirections.empty()) {
        assert(dist<priorities->size());
        (*priorities)[dist] = (*priorities)[dist] * 2;
    }
    if (mainDirections.includes(MainDirections::DIR_FORWARD)&&myLanes.size()>2) {
        (*priorities)[dist] = (*priorities)[dist] * 2;
    }
    // return
    return priorities;
}


unsigned int
NBEdge::computePrioritySum(std::vector<unsigned int> *priorities) {
    unsigned int sum = 0;
    for (std::vector<unsigned int>::iterator i=priorities->begin(); i!=priorities->end(); i++) {
        sum += int(*i);
    }
    return sum;
}


void
NBEdge::appendTurnaround(bool noTLSControlled) throw() {
    // do nothing if no turnaround is known
    if (myTurnDestination==0) {
        return;
    }
    // do nothing if the destination node is controlled by a tls and no turnarounds
    //  shall be appended for such junctions
    if (noTLSControlled&&myTo->isTLControlled()) {
        return;
    }
    setConnection((unsigned int)(myLanes.size()-1), myTurnDestination, myTurnDestination->getNumLanes()-1, L2L_VALIDATED);
}


void
NBEdge::sortOutgoingConnectionsByAngle() {
    sort(myConnections.begin(), myConnections.end(), connections_relative_edgelane_sorter(this, myTo));
}


void
NBEdge::sortOutgoingConnectionsByIndex() {
    sort(myConnections.begin(), myConnections.end(), connections_sorter());
}


bool
NBEdge::isTurningDirectionAt(const NBNode *n, const NBEdge * const edge) const throw() {
    // maybe it was already set as the turning direction
    if (edge == myTurnDestination) {
        return true;
    } else if (myTurnDestination!=0) {
        // otherwise - it's not if a turning direction exists
        return false;
    }
    // if the same nodes are connected
    if (myFrom==edge->myTo &&myTo==edge->myFrom) {
        return true;
    }
    // we have to checke whether the connection between the nodes is
    //  geometrically similar
    SUMOReal thisFromAngle2 = getAngle(*n);
    SUMOReal otherToAngle2 = edge->getAngle(*n);
    if (thisFromAngle2<otherToAngle2) {
        std::swap(thisFromAngle2, otherToAngle2);
    }
    if (thisFromAngle2-otherToAngle2>170&&thisFromAngle2-otherToAngle2<190) {
        return true;
    }
    return false;
}



NBNode *
NBEdge::tryGetNodeAtPosition(SUMOReal pos, SUMOReal tolerance) const {
    // return the from-node when the position is at the begin of the edge
    if (pos<tolerance) {
        return myFrom;
    }
    // return the to-node when the position is at the end of the edge
    if (pos>myLength-tolerance) {
        return myTo;
    }
    return 0;
}


void
NBEdge::replaceInConnections(NBEdge *which, NBEdge *by, unsigned int laneOff) {
    UNUSED_PARAMETER(laneOff);
    // replace in "_connectedEdges"
    for (std::vector<Connection>::iterator i=myConnections.begin(); i!=myConnections.end(); ++i) {
        if ((*i).toEdge==which) {
            (*i).toEdge = by;
        }
    }
    // check whether it was the turn destination
    if (myTurnDestination==which) {
        myTurnDestination = by;
    }
}


void
NBEdge::moveOutgoingConnectionsFrom(NBEdge *e, unsigned int laneOff) {
    unsigned int lanes = e->getNumLanes();
    for (unsigned int i=0; i<lanes; i++) {
        std::vector<NBEdge::Connection> elv = e->getConnectionsFromLane(i);
        for (std::vector<NBEdge::Connection>::iterator j=elv.begin(); j!=elv.end(); j++) {
            NBEdge::Connection el = *j;
            assert(el.tlID=="");
            bool ok = addLane2LaneConnection(i+laneOff, el.toEdge, el.toLane, L2L_COMPUTED);
            assert(ok);
        }
    }
}


bool
NBEdge::isConnectedTo(NBEdge *e) {
    if (e==myTurnDestination) {
        return true;
    }
    return
        find_if(myConnections.begin(), myConnections.end(), connections_toedge_finder(e))
        !=
        myConnections.end();

}


EdgeVector
NBEdge::getConnectedEdges() const throw() {
    EdgeVector ret;
    for (std::vector<Connection>::const_iterator i=myConnections.begin(); i!=myConnections.end(); ++i) {
        if (find(ret.begin(), ret.end(), (*i).toEdge)==ret.end()) {
            ret.push_back((*i).toEdge);
        }
    }
    return ret;
}


void
NBEdge::remapConnections(const EdgeVector &incoming) {
    EdgeVector connected = getConnectedEdges();
    for (EdgeVector::const_iterator i=incoming.begin(); i!=incoming.end(); i++) {
        NBEdge *inc = *i;
        // We have to do this
        inc->myStep = EDGE2EDGES;
        // add all connections
        for (EdgeVector::iterator j=connected.begin(); j!=connected.end(); j++) {
            inc->addEdge2EdgeConnection(*j);
        }
        inc->removeFromConnections(this);
    }
}


void
NBEdge::removeFromConnections(NBEdge *toEdge, int fromLane, int toLane) {
    // remove from "myConnections"
    for (std::vector<Connection>::iterator i=myConnections.begin(); i!=myConnections.end();) {
        Connection &c = *i;
        if (c.toEdge==toEdge 
                && (fromLane<0 || c.fromLane==fromLane) 
                && (toLane<0 || c.toLane==toLane)) {
            i = myConnections.erase(i);
        } else {
            ++i;
        }
    }
    // check whether it was the turn destination
    if (myTurnDestination==toEdge && fromLane<0) {
        myTurnDestination = 0;
    }
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


bool
NBEdge::lanesWereAssigned() const {
    return myStep==LANES2LANES_DONE||myStep==LANES2LANES_USER;
}


SUMOReal
NBEdge::getMaxLaneOffset() {
    return (SUMOReal) SUMO_const_laneWidthAndOffset * myLanes.size();
}


Position
NBEdge::getMinLaneOffsetPositionAt(NBNode *node, SUMOReal width) const {
    const PositionVector &shape0 = myLanes[0].shape;
    const PositionVector &shapel = myLanes.back().shape;
    width = width < shape0.length()/(SUMOReal) 2.0
            ? width
            : shape0.length()/(SUMOReal) 2.0;
    if (node==myFrom) {
        Position pos =  shapel.positionAtLengthPosition(width);
        GeomHelper::transfer_to_side(pos, shapel[0], shapel[-1], SUMO_const_halfLaneAndOffset);
        return pos;
    } else {
        Position pos = shape0.positionAtLengthPosition(shape0.length() - width);
        GeomHelper::transfer_to_side(pos, shape0[-1], shape0[0], SUMO_const_halfLaneAndOffset);
        return pos;
    }
}


Position
NBEdge::getMaxLaneOffsetPositionAt(NBNode *node, SUMOReal width) const {
    const PositionVector &shape0 = myLanes[0].shape;
    const PositionVector &shapel = myLanes.back().shape;
    width = width < shape0.length()/(SUMOReal) 2.0
            ? width
            : shape0.length()/(SUMOReal) 2.0;
    if (node==myFrom) {
        Position pos = shape0.positionAtLengthPosition(width);
        GeomHelper::transfer_to_side(pos, shape0[0], shape0[-1], -SUMO_const_halfLaneAndOffset);
        return pos;
    } else {
        Position pos = shapel.positionAtLengthPosition(shapel.length() - width);
        GeomHelper::transfer_to_side(pos, shapel[-1], shapel[0], -SUMO_const_halfLaneAndOffset);
        return pos;
    }
}


bool
NBEdge::mayBeTLSControlled(int fromLane, NBEdge *toEdge, int toLane) const throw() {
    TLSDisabledConnection tpl;
    tpl.fromLane = fromLane;
    tpl.to = toEdge;
    tpl.toLane = toLane;
    std::vector<TLSDisabledConnection>::const_iterator i = find_if(myTLSDisabledConnections.begin(), myTLSDisabledConnections.end(), tls_disable_finder(tpl));
    return i==myTLSDisabledConnections.end();
}


bool
NBEdge::setControllingTLInformation(int fromLane, NBEdge *toEdge, int toLane,
                                    const std::string &tlID, unsigned int tlPos) {
    // check whether the connection was not set as not to be controled previously
    TLSDisabledConnection tpl;
    tpl.fromLane = fromLane;
    tpl.to = toEdge;
    tpl.toLane = toLane;
    std::vector<TLSDisabledConnection>::iterator i = find_if(myTLSDisabledConnections.begin(), myTLSDisabledConnections.end(), tls_disable_finder(tpl));
    if (i!=myTLSDisabledConnections.end()) {
        return false;
    }

    assert(fromLane<0||fromLane<(int) myLanes.size());
    // try to use information about the connections if given
    if (fromLane>=0&&toLane>=0) {
        // find the specified connection
        std::vector<Connection>::iterator i =
            find_if(myConnections.begin(), myConnections.end(), connections_finder(fromLane, toEdge, toLane));
        // ok, we have to test this as on the removal of dummy edges some connections
        //  will be reassigned
        if (i!=myConnections.end()) {
            // get the connection
            Connection &connection = *i;
            // set the information about the tl
            connection.tlID = tlID;
            connection.tlLinkNo = tlPos;
            return true;
        }
    }
    // if the original connection was not found, set the information for all
    //  connections
    unsigned int no = 0;
    bool hadError = false;
    for (std::vector<Connection>::iterator i=myConnections.begin(); i!=myConnections.end(); ++i) {
        if ((*i).toEdge!=toEdge) {
            continue;
        }
        if (fromLane>=0&&fromLane!=(*i).fromLane) {
            continue;
        }
        if (toLane>=0&&toLane!=(*i).toLane) {
            continue;
        }
        if ((*i).tlID=="") {
            (*i).tlID = tlID;
            (*i).tlLinkNo = tlPos;
            no++;
        } else {
            if ((*i).tlID!=tlID&&(*i).tlLinkNo==tlPos) {
                WRITE_WARNING("The lane " + toString<int>((*i).fromLane)+ " on edge " + getID()+ " already had a traffic light signal.");
                hadError = true;
            }
        }
    }
    if (hadError&&no==0) {
        WRITE_WARNING("Could not set any signal of the traffic light '" + tlID + "' (unknown group)");
    }
    return true;
}


void
NBEdge::disableConnection4TLS(int fromLane, NBEdge *toEdge, int toLane) {
    TLSDisabledConnection c;
    c.fromLane = fromLane;
    c.to = toEdge;
    c.toLane = toLane;
    myTLSDisabledConnections.push_back(c);
}


PositionVector
NBEdge::getCWBoundaryLine(const NBNode &n, SUMOReal offset) const {
    PositionVector ret;
    if (myFrom==(&n)) {
        // outgoing
        ret = !myAmLeftHand ? myLanes[0].shape : myLanes.back().shape;
    } else {
        // incoming
        ret = !myAmLeftHand ? myLanes.back().shape.reverse() : myLanes[0].shape.reverse();
    }
    ret.move2side(offset);
    return ret;
}


PositionVector
NBEdge::getCCWBoundaryLine(const NBNode &n, SUMOReal offset) const {
    PositionVector ret;
    if (myFrom==(&n)) {
        // outgoing
        ret = !myAmLeftHand ? myLanes.back().shape : myLanes[0].shape;
    } else {
        // incoming
        ret = !myAmLeftHand ? myLanes[0].shape.reverse() : myLanes.back().shape.reverse();
    }
    ret.move2side(-offset);
    return ret;
}


SUMOReal
NBEdge::width() const {
    return (SUMOReal) myLanes.size() * SUMO_const_laneWidth + (SUMOReal)(myLanes.size()-1) * SUMO_const_laneOffset;
}


bool
NBEdge::expandableBy(NBEdge *possContinuation) const {
    // ok, the number of lanes must match
    if (myLanes.size()!=possContinuation->myLanes.size()) {
        return false;
    }
    // the priority, too (?)
    if (getPriority()!=possContinuation->getPriority()) {
        return false;
    }
    // the speed allowed
    if (mySpeed!=possContinuation->mySpeed) {
        return false;
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
        const EdgeVector &conn = getConnectedEdges();
        if (find(conn.begin(), conn.end(), possContinuation)
                ==conn.end()) {

            return false;
        }
    }
    break;
    case LANES2EDGES:
    case LANES2LANES_RECHECK:
    case LANES2LANES_DONE:
    case LANES2LANES_USER: {
        // the possible continuation must be connected
        if (find_if(myConnections.begin(), myConnections.end(), connections_toedge_finder(possContinuation))==myConnections.end()) {
            return false;
        }
        // all lanes must go to the possible continuation
        std::vector<int> conns = getConnectionLanes(possContinuation);
        if (conns.size()!=myLanes.size()) {
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
NBEdge::append(NBEdge *e) {
    // append geometry
    myGeom.appendWithCrossingPoint(e->myGeom);
    for (unsigned int i=0; i<myLanes.size(); i++) {
        myLanes[i].shape.appendWithCrossingPoint(e->myLanes[i].shape);
    }
    // recompute length
    myLength += e->myLength;
    // copy the connections and the building step if given
    myStep = e->myStep;
    myConnections = e->myConnections;
    myTurnDestination = e->myTurnDestination;
    // set the node
    myTo = e->myTo;
}





bool
NBEdge::hasSignalisedConnectionTo(const NBEdge * const e) const throw() {
    for (std::vector<Connection>::const_iterator i=myConnections.begin(); i!=myConnections.end(); ++i) {
        if ((*i).toEdge==e && (*i).tlID!="") {
            return true;
        }
    }
    return false;
}


NBEdge*
NBEdge::getTurnDestination() const {
    return myTurnDestination;
}


std::string
NBEdge::getLaneID(unsigned int lane) const {
    assert(lane<myLanes.size());
    return myID + "_" + toString(lane);
}


bool
NBEdge::isNearEnough2BeJoined2(NBEdge *e, SUMOReal threshold) const {
    DoubleVector distances = myGeom.distances(e->getGeometry());
    assert(distances.size() > 0);
    return VectorHelper<SUMOReal>::maxValue(distances) < threshold;
}


SUMOReal
NBEdge::getNormedAngle(const NBNode &atNode) const {
    SUMOReal angle = getAngle(atNode);
    if (angle<0) {
        angle = 360 + angle;
    }
    assert(angle>=0&&angle<=360);
    return angle;
}


SUMOReal
NBEdge::getNormedAngle() const {
    SUMOReal angle = myAngle;
    if (angle<0) {
        angle = 360 + angle;
    }
    assert(angle>=0&&angle<360);
    return angle;
}


void
NBEdge::incLaneNo(unsigned int by) {
    unsigned int newLaneNo = (unsigned int) myLanes.size() + by;
    while (myLanes.size()<newLaneNo) {
        Lane l;
        l.speed = mySpeed;
        l.offset = myOffset;
        l.width = myWidth;
        myLanes.push_back(l);
    }
    computeLaneShapes();
    const EdgeVector &incs = myFrom->getIncomingEdges();
    for (EdgeVector::const_iterator i=incs.begin(); i!=incs.end(); ++i) {
        (*i)->invalidateConnections(true);
    }
    invalidateConnections(true);
}


void
NBEdge::decLaneNo(unsigned int by, int dir) {
    unsigned int newLaneNo = (unsigned int) myLanes.size() - by;
    while (myLanes.size()>newLaneNo) {
        myLanes.pop_back();
    }
    computeLaneShapes();
    const EdgeVector &incs = myFrom->getIncomingEdges();
    for (EdgeVector::const_iterator i=incs.begin(); i!=incs.end(); ++i) {
        (*i)->invalidateConnections(true);
    }
    if (dir==0) {
        invalidateConnections(true);
    } else {
        const EdgeVector &outs = myTo->getOutgoingEdges();
        assert(outs.size()==1);
        NBEdge *out = outs[0];
        if (dir<0) {
            removeFromConnections(out, 0);
        } else {
            removeFromConnections(out, (int) myLanes.size());
        }
    }
}


void
NBEdge::copyConnectionsFrom(NBEdge *src) {
    myStep = src->myStep;
    myConnections = src->myConnections;
}


void
NBEdge::markAsInLane2LaneState() {
    assert(myTo->getOutgoingEdges().size()==0);
    myStep = LANES2LANES_DONE;
}


bool
NBEdge::splitGeometry(NBEdgeCont &ec, NBNodeCont &nc) {
    // check whether there any splits to perform
    if (myGeom.size()<3) {
        return false;
    }
    // ok, split
    NBNode *newFrom = myFrom;
    NBNode *myLastNode = myTo;
    NBNode *newTo = 0;
    NBEdge *currentEdge = this;
    for (int i=1; i<(int) myGeom.size()-1; i++) {
        // build the node first
        if (i!=static_cast<int>(myGeom.size()-2)) {
            std::string nodename = myID + "_in_between#" + toString(i);
            if (!nc.insert(nodename, myGeom[i])) {
                throw ProcessError("Error on adding in-between node '" + nodename + "'.");
            }
            newTo = nc.retrieve(nodename);
        } else {
            newTo = myLastNode;
        }
        if (i==1) {
            currentEdge->myTo->removeIncoming(this);
            currentEdge->myTo = newTo;
            newTo->addIncomingEdge(currentEdge);
        } else {
            std::string edgename = myID + "[" + toString(i-1) + "]";
            // @bug lane-specific width, speed, overall offset and restrictions are ignored
            currentEdge = new NBEdge(edgename, newFrom, newTo, myType, mySpeed, (unsigned int) myLanes.size(),
                                     myPriority, myWidth, UNSPECIFIED_OFFSET, 
                                     myStreetName, myLaneSpreadFunction);
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
NBEdge::allowVehicleClass(int lane, SUMOVehicleClass vclass) {
    if (lane<0) {
        // if all lanes are meant...
        for (unsigned int i=0; i<myLanes.size(); i++) {
            // ... do it for each lane
            allowVehicleClass((int) i, vclass);
        }
        return;
    }
    assert(lane<(int) myLanes.size());
    myLanes[lane].allowed.insert(vclass);
}


void
NBEdge::disallowVehicleClass(int lane, SUMOVehicleClass vclass) {
    if (lane<0) {
        // if all lanes are meant...
        for (unsigned int i=0; i<myLanes.size(); i++) {
            // ... do it for each lane
            disallowVehicleClass((int) i, vclass);
        }
        return;
    }
    assert(lane<(int) myLanes.size());
    myLanes[lane].notAllowed.insert(vclass);
}


void
NBEdge::preferVehicleClass(int lane, SUMOVehicleClass vclass) {
    if (lane<0) {
        // if all lanes are meant...
        for (unsigned int i=0; i<myLanes.size(); i++) {
            // ... do it for each lane
            preferVehicleClass((int) i, vclass);
        }
        return;
    }
    assert(lane<(int) myLanes.size());
    myLanes[lane].preferred.insert(vclass);
}


void
NBEdge::setWidth(int lane, SUMOReal width) {
    if (lane<0) { 
        // all lanes are meant...
        myWidth = width;
        for (unsigned int i=0; i<myLanes.size(); i++) {
            // ... do it for each lane
            setWidth((int) i, width);
        }
        return;
    }
    assert(lane<(int) myLanes.size());
    myLanes[lane].width = width;
}


void
NBEdge::setOffset(int lane, SUMOReal offset) {
    if (lane<0) {
        // all lanes are meant...
        myOffset = offset;
        for (unsigned int i=0; i<myLanes.size(); i++) {
            // ... do it for each lane
            setOffset((int) i, offset);
        }
        return;
    }
    assert(lane<(int) myLanes.size());
    myLanes[lane].offset = offset;
}


void
NBEdge::setSpeed(int lane, SUMOReal speed) {
    if (lane<0) {
        // all lanes are meant...
        mySpeed = speed;
        for (unsigned int i=0; i<myLanes.size(); i++) {
            // ... do it for each lane
            setOffset((int) i, speed);
        }
        return;
    }
    assert(lane<(int) myLanes.size());
    myLanes[lane].speed = speed;
}


void
NBEdge::setVehicleClasses(const SUMOVehicleClasses &allowed, const SUMOVehicleClasses &disallowed, int lane) {
    for (SUMOVehicleClasses::const_iterator i=allowed.begin(); i!=allowed.end(); ++i) {
        allowVehicleClass(lane, *i);
    }
    for (SUMOVehicleClasses::const_iterator i=disallowed.begin(); i!=disallowed.end(); ++i) {
        disallowVehicleClass(lane, *i);
    }
}


SUMOVehicleClasses
NBEdge::getAllowedVehicleClasses() const {
    SUMOVehicleClasses ret;
    for (std::vector<Lane>::const_iterator i=myLanes.begin(); i!=myLanes.end(); ++i) {
        const SUMOVehicleClasses &allowed = (*i).allowed;
        ret.insert(allowed.begin(), allowed.end());
    }
    return ret;
}


SUMOVehicleClasses 
NBEdge::getDisallowedVehicleClasses() const {
    SUMOVehicleClasses ret;
    for (std::vector<Lane>::const_iterator i=myLanes.begin(); i!=myLanes.end(); ++i) {
        const SUMOVehicleClasses &notAllowed = (*i).notAllowed;
        ret.insert(notAllowed.begin(), notAllowed.end());
    }
    return ret;
}


SUMOVehicleClasses 
NBEdge::getAllowedVehicleClasses(unsigned int lane) const {
    assert(lane < myLanes.size());
    return myLanes[lane].allowed;
}


SUMOVehicleClasses
NBEdge::getDisallowedVehicleClasses(unsigned int lane) const {
    assert(lane < myLanes.size());
    return myLanes[lane].notAllowed;
}


int
NBEdge::getMinConnectedLane(NBEdge *of) const {
    int ret = -1;
    for (std::vector<Connection>::const_iterator i=myConnections.begin(); i!=myConnections.end(); ++i) {
        if ((*i).toEdge==of && (ret==-1 || (*i).toLane<ret)) {
            ret = (*i).toLane;
        }
    }
    return ret;
}


int
NBEdge::getMaxConnectedLane(NBEdge *of) const {
    int ret = -1;
    for (std::vector<Connection>::const_iterator i=myConnections.begin(); i!=myConnections.end(); ++i) {
        if ((*i).toEdge==of && (*i).toLane>ret) {
            ret = (*i).toLane;
        }
    }
    return ret;
}


void
NBEdge::setLoadedLength(SUMOReal val) {
    myLoadedLength = val;
}


void
NBEdge::dismissVehicleClassInformation() {
    for (std::vector<Lane>::iterator i=myLanes.begin(); i!=myLanes.end(); ++i) {
        (*i).allowed.clear();
        (*i).notAllowed.clear();
        (*i).preferred.clear();
    }
}


/****************************************************************************/
