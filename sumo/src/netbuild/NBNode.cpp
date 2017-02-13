/****************************************************************************/
/// @file    NBNode.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// The representation of a single node
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

#include <string>
#include <map>
#include <cassert>
#include <algorithm>
#include <vector>
#include <deque>
#include <set>
#include <cmath>
#include <iterator>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/bezier.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/iodevices/OutputDevice.h>
#include <iomanip>
#include "NBNode.h"
#include "NBAlgorithms.h"
#include "NBNodeCont.h"
#include "NBNodeShapeComputer.h"
#include "NBEdgeCont.h"
#include "NBTypeCont.h"
#include "NBHelpers.h"
#include "NBDistrict.h"
#include "NBContHelper.h"
#include "NBRequest.h"
#include "NBOwnTLDef.h"
#include "NBTrafficLightLogicCont.h"
#include "NBTrafficLightDefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// allow to extend a crossing across multiple edges
#define EXTEND_CROSSING_ANGLE_THRESHOLD 35.0 // degrees
// create intermediate walking areas if either of the following thresholds is exceeded
#define SPLIT_CROSSING_WIDTH_THRESHOLD 1.5 // meters
#define SPLIT_CROSSING_ANGLE_THRESHOLD 5 // degrees

// minimum length for a weaving section at a combined on-off ramp
#define MIN_WEAVE_LENGTH 20.0

//#define DEBUG_SMOOTH_GEOM
#define DEBUGCOND true

// ===========================================================================
// static members
// ===========================================================================
const int NBNode::FORWARD(1);
const int NBNode::BACKWARD(-1);
const SUMOReal NBNode::DEFAULT_CROSSING_WIDTH(4);
const SUMOReal NBNode::UNSPECIFIED_RADIUS = -1;

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * NBNode::ApproachingDivider-methods
 * ----------------------------------------------------------------------- */
NBNode::ApproachingDivider::ApproachingDivider(
    EdgeVector* approaching, NBEdge* currentOutgoing) :
    myApproaching(approaching), myCurrentOutgoing(currentOutgoing) {
    // check whether origin lanes have been given
    assert(myApproaching != 0);
    // collect lanes which are expliclity targeted
    std::set<int> approachedLanes;
    for (EdgeVector::iterator it = myApproaching->begin(); it != myApproaching->end(); ++it) {
        const std::vector<NBEdge::Connection> conns = (*it)->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator it_con = conns.begin(); it_con != conns.end(); ++it_con) {
            if ((*it_con).toEdge == myCurrentOutgoing) {
                approachedLanes.insert((*it_con).toLane);
            }
        }
    }
    // compute the indices of lanes that should be targeted (excluding pedestrian
    // lanes that will be connected from walkingAreas and forbidden lanes)
    // if the lane is targeted by an explicitly set connection we need
    // to make it available anyway
    for (int i = 0; i < currentOutgoing->getNumLanes(); ++i) {
        if ((currentOutgoing->getPermissions(i) == SVC_PEDESTRIAN
                || isForbidden(currentOutgoing->getPermissions(i)))
                && approachedLanes.count(i) == 0) {
            continue;
        }
        myAvailableLanes.push_back((int)i);
    }
}


NBNode::ApproachingDivider::~ApproachingDivider() {}


void
NBNode::ApproachingDivider::execute(const int src, const int dest) {
    assert((int)myApproaching->size() > src);
    // get the origin edge
    NBEdge* incomingEdge = (*myApproaching)[src];
    if (incomingEdge->getStep() == NBEdge::LANES2LANES_DONE || incomingEdge->getStep() == NBEdge::LANES2LANES_USER) {
        return;
    }
    std::vector<int> approachingLanes =
        incomingEdge->getConnectionLanes(myCurrentOutgoing);
    assert(approachingLanes.size() != 0);
    std::deque<int>* approachedLanes = spread(approachingLanes, dest);
    assert(approachedLanes->size() <= myAvailableLanes.size());
    // set lanes
    for (int i = 0; i < (int)approachedLanes->size(); i++) {
        assert((int)approachingLanes.size() > i);
        int approached = myAvailableLanes[(*approachedLanes)[i]];
        incomingEdge->setConnection((int) approachingLanes[i], myCurrentOutgoing,
                                    approached, NBEdge::L2L_COMPUTED);
    }
    delete approachedLanes;
}


std::deque<int>*
NBNode::ApproachingDivider::spread(const std::vector<int>& approachingLanes,
                                   int dest) const {
    std::deque<int>* ret = new std::deque<int>();
    int noLanes = (int) approachingLanes.size();
    // when only one lane is approached, we check, whether the SUMOReal-value
    //  is assigned more to the left or right lane
    if (noLanes == 1) {
        ret->push_back(dest);
        return ret;
    }

    int noOutgoingLanes = (int)myAvailableLanes.size();
    //
    ret->push_back(dest);
    int noSet = 1;
    int roffset = 1;
    int loffset = 1;
    while (noSet < noLanes) {
        // It may be possible, that there are not enough lanes the source
        //  lanes may be divided on
        //  In this case, they remain unset
        //  !!! this is only a hack. It is possible, that this yields in
        //   uncommon divisions
        if (noOutgoingLanes == noSet) {
            return ret;
        }

        // as due to the conversion of SUMOReal->uint the numbers will be lower
        //  than they should be, we try to append to the left side first
        //
        // check whether the left boundary of the approached street has
        //  been overridden; if so, move all lanes to the right
        if (dest + loffset >= noOutgoingLanes) {
            loffset -= 1;
            roffset += 1;
            for (int i = 0; i < (int)ret->size(); i++) {
                (*ret)[i] = (*ret)[i] - 1;
            }
        }
        // append the next lane to the left of all edges
        //  increase the position (destination edge)
        ret->push_back(dest + loffset);
        noSet++;
        loffset += 1;

        // as above
        if (noOutgoingLanes == noSet) {
            return ret;
        }

        // now we try to append the next lane to the right side, when needed
        if (noSet < noLanes) {
            // check whether the right boundary of the approached street has
            //  been overridden; if so, move all lanes to the right
            if (dest < roffset) {
                loffset += 1;
                roffset -= 1;
                for (int i = 0; i < (int)ret->size(); i++) {
                    (*ret)[i] = (*ret)[i] + 1;
                }
            }
            ret->push_front(dest - roffset);
            noSet++;
            roffset += 1;
        }
    }
    return ret;
}


/* -------------------------------------------------------------------------
 * NBNode-methods
 * ----------------------------------------------------------------------- */
NBNode::NBNode(const std::string& id, const Position& position,
               SumoXMLNodeType type) :
    Named(StringUtils::convertUmlaute(id)),
    myPosition(position),
    myType(type),
    myDistrict(0),
    myHaveCustomPoly(false),
    myRequest(0),
    myRadius(OptionsCont::getOptions().isDefault("default.junctions.radius") ? UNSPECIFIED_RADIUS : OptionsCont::getOptions().getFloat("default.junctions.radius")),
    myKeepClear(OptionsCont::getOptions().getBool("default.junctions.keep-clear")),
    myDiscardAllCrossings(false),
    myCrossingsLoadedFromSumoNet(0),
    myDisplacementError(0) {
}


NBNode::NBNode(const std::string& id, const Position& position, NBDistrict* district) :
    Named(StringUtils::convertUmlaute(id)),
    myPosition(position),
    myType(district == 0 ? NODETYPE_UNKNOWN : NODETYPE_DISTRICT),
    myDistrict(district),
    myHaveCustomPoly(false),
    myRequest(0),
    myRadius(OptionsCont::getOptions().isDefault("default.junctions.radius") ? UNSPECIFIED_RADIUS : OptionsCont::getOptions().getFloat("default.junctions.radius")),
    myKeepClear(OptionsCont::getOptions().getBool("default.junctions.keep-clear")),
    myDiscardAllCrossings(false),
    myCrossingsLoadedFromSumoNet(0),
    myDisplacementError(0) {
}


NBNode::~NBNode() {
    delete myRequest;
}


void
NBNode::reinit(const Position& position, SumoXMLNodeType type,
               bool updateEdgeGeometries) {
    myPosition = position;
    // patch type
    myType = type;
    if (!isTrafficLight(myType)) {
        removeTrafficLights();
    }
    if (updateEdgeGeometries) {
        for (EdgeVector::iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
            PositionVector geom = (*i)->getGeometry();
            geom[-1] = myPosition;
            (*i)->setGeometry(geom);
        }
        for (EdgeVector::iterator i = myOutgoingEdges.begin(); i != myOutgoingEdges.end(); i++) {
            PositionVector geom = (*i)->getGeometry();
            geom[0] = myPosition;
            (*i)->setGeometry(geom);
        }
    }
}



// -----------  Applying offset
void
NBNode::reshiftPosition(SUMOReal xoff, SUMOReal yoff) {
    myPosition.add(xoff, yoff, 0);
    myPoly.add(xoff, yoff, 0);
}


void
NBNode::mirrorX() {
    myPosition.mul(1, -1);
    myPoly.mirrorX();
    // mirror pre-computed geometty of crossings and walkingareas
    for (std::vector<Crossing>::iterator it = myCrossings.begin(); it != myCrossings.end(); ++it) {
        (*it).shape.mirrorX();
    }
    for (std::vector<WalkingArea>::iterator it_wa = myWalkingAreas.begin(); it_wa != myWalkingAreas.end(); ++it_wa) {
        (*it_wa).shape.mirrorX();
    }
}


// -----------  Methods for dealing with assigned traffic lights
void
NBNode::addTrafficLight(NBTrafficLightDefinition* tlDef) {
    myTrafficLights.insert(tlDef);
    // rail signals receive a temporary traffic light in order to set connection tl-linkIndex
    if (!isTrafficLight(myType) && myType != NODETYPE_RAIL_SIGNAL && myType != NODETYPE_RAIL_CROSSING) {
        myType = NODETYPE_TRAFFIC_LIGHT;
    }
}


void
NBNode::removeTrafficLight(NBTrafficLightDefinition* tlDef) {
    tlDef->removeNode(this);
    myTrafficLights.erase(tlDef);
}


void
NBNode::removeTrafficLights() {
    std::set<NBTrafficLightDefinition*> trafficLights = myTrafficLights; // make a copy because we will modify the original
    for (std::set<NBTrafficLightDefinition*>::const_iterator i = trafficLights.begin(); i != trafficLights.end(); ++i) {
        removeTrafficLight(*i);
    }
}


bool
NBNode::isJoinedTLSControlled() const {
    if (!isTLControlled()) {
        return false;
    }
    for (std::set<NBTrafficLightDefinition*>::const_iterator i = myTrafficLights.begin(); i != myTrafficLights.end(); ++i) {
        if ((*i)->getID().find("joined") == 0) {
            return true;
        }
    }
    return false;
}


void
NBNode::invalidateTLS(NBTrafficLightLogicCont& tlCont) {
    if (isTLControlled()) {
        std::set<NBTrafficLightDefinition*> oldDefs(myTrafficLights);
        for (std::set<NBTrafficLightDefinition*>::iterator it = oldDefs.begin(); it != oldDefs.end(); ++it) {
            NBTrafficLightDefinition* orig = *it;
            if (dynamic_cast<NBOwnTLDef*>(orig) == 0) {
                NBTrafficLightDefinition* newDef = new NBOwnTLDef(orig->getID(), orig->getOffset(), orig->getType());
                const std::vector<NBNode*>& nodes = orig->getNodes();
                while (!nodes.empty()) {
                    newDef->addNode(nodes.front());
                    nodes.front()->removeTrafficLight(orig);
                }
                tlCont.removeFully(orig->getID());
                tlCont.insert(newDef);
            }
        }
    }
}


void
NBNode::shiftTLConnectionLaneIndex(NBEdge* edge, int offset) {
    for (std::set<NBTrafficLightDefinition*>::iterator it = myTrafficLights.begin(); it != myTrafficLights.end(); ++it) {
        (*it)->shiftTLConnectionLaneIndex(edge, offset);
    }
}

// ----------- Prunning the input
int
NBNode::removeSelfLoops(NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tc) {
    int ret = 0;
    int pos = 0;
    EdgeVector::const_iterator j = myIncomingEdges.begin();
    while (j != myIncomingEdges.end()) {
        // skip edges which are only incoming and not outgoing
        if (find(myOutgoingEdges.begin(), myOutgoingEdges.end(), *j) == myOutgoingEdges.end()) {
            ++j;
            ++pos;
            continue;
        }
        // an edge with both its origin and destination being the current
        //  node should be removed
        NBEdge* dummy = *j;
        WRITE_WARNING(" Removing self-looping edge '" + dummy->getID() + "'");
        // get the list of incoming edges connected to the self-loop
        EdgeVector incomingConnected = dummy->getIncomingEdges();;
        // get the list of outgoing edges connected to the self-loop
        EdgeVector outgoingConnected = dummy->getConnectedEdges();
        // let the self-loop remap its connections
        dummy->remapConnections(incomingConnected);
        remapRemoved(tc, dummy, incomingConnected, outgoingConnected);
        // delete the self-loop
        ec.erase(dc, dummy);
        j = myIncomingEdges.begin() + pos;
        ++ret;
    }
    return ret;
}


// -----------
void
NBNode::addIncomingEdge(NBEdge* edge) {
    assert(edge != 0);
    if (find(myIncomingEdges.begin(), myIncomingEdges.end(), edge) == myIncomingEdges.end()) {
        myIncomingEdges.push_back(edge);
        myAllEdges.push_back(edge);
    }
}


void
NBNode::addOutgoingEdge(NBEdge* edge) {
    assert(edge != 0);
    if (find(myOutgoingEdges.begin(), myOutgoingEdges.end(), edge) == myOutgoingEdges.end()) {
        myOutgoingEdges.push_back(edge);
        myAllEdges.push_back(edge);
    }
}


bool
NBNode::isSimpleContinuation(bool checkLaneNumbers) const {
    // one in, one out->continuation
    if (myIncomingEdges.size() == 1 && myOutgoingEdges.size() == 1) {
        // both must have the same number of lanes
        return !checkLaneNumbers || ((*(myIncomingEdges.begin()))->getNumLanes() == (*(myOutgoingEdges.begin()))->getNumLanes());
    }
    // two in and two out and both in reverse direction
    if (myIncomingEdges.size() == 2 && myOutgoingEdges.size() == 2) {
        for (EdgeVector::const_iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
            NBEdge* in = *i;
            EdgeVector::const_iterator opposite = find_if(myOutgoingEdges.begin(), myOutgoingEdges.end(), NBContHelper::opposite_finder(in));
            // must have an opposite edge
            if (opposite == myOutgoingEdges.end()) {
                return false;
            }
            // both must have the same number of lanes
            NBContHelper::nextCW(myOutgoingEdges, opposite);
            if (checkLaneNumbers && in->getNumLanes() != (*opposite)->getNumLanes()) {
                return false;
            }
        }
        return true;
    }
    // nope
    return false;
}


PositionVector
NBNode::computeSmoothShape(const PositionVector& begShape,
                           const PositionVector& endShape,
                           int numPoints,
                           bool isTurnaround,
                           SUMOReal extrapolateBeg,
                           SUMOReal extrapolateEnd,
                           NBNode* recordError) const {

    bool ok = true;
    PositionVector init = bezierControlPoints(begShape, endShape, isTurnaround, extrapolateBeg, extrapolateEnd, ok, recordError);
#ifdef DEBUG_SMOOTH_GEOM
    if (DEBUGCOND) {
        std::cout << "computeSmoothShape node " << getID() << " init=" << init << "\n";
    }
#endif
    if (init.size() == 0) {
        PositionVector ret;
        ret.push_back(begShape.back());
        ret.push_back(endShape.front());
        return ret;
    } else {
        return bezier(init, numPoints).smoothedZFront();
    }
}

PositionVector
NBNode::bezierControlPoints(
    const PositionVector& begShape,
    const PositionVector& endShape,
    bool isTurnaround,
    SUMOReal extrapolateBeg,
    SUMOReal extrapolateEnd,
    bool& ok,
    NBNode* recordError,
    SUMOReal straightThresh) {

    const Position beg = begShape.back();
    const Position end = endShape.front();
    const SUMOReal dist = beg.distanceTo2D(end);
    PositionVector init;
    if (dist < POSITION_EPS || beg.distanceTo2D(begShape[-2]) < POSITION_EPS || end.distanceTo2D(endShape[1]) < POSITION_EPS) {
#ifdef DEBUG_SMOOTH_GEOM
        if (DEBUGCOND) std::cout << "   bezierControlPoints failed beg=" << beg << " end=" << end
                                     << " dist=" << dist
                                     << " distBegLast=" << beg.distanceTo2D(begShape[-2])
                                     << " distEndFirst=" << end.distanceTo2D(endShape[1])
                                     << "\n";
#endif
        // typically, this node a is a simpleContinuation. see also #2539
        return init;
    } else {
        init.push_back(beg);
        if (isTurnaround) {
            // turnarounds:
            //  - end of incoming lane
            //  - position between incoming/outgoing end/begin shifted by the distance orthogonally
            //  - begin of outgoing lane
            Position center = PositionVector::positionAtOffset2D(beg, end, beg.distanceTo2D(end) / (SUMOReal) 2.);
            center.sub(beg.y() - end.y(), end.x() - beg.x());
            init.push_back(center);
        } else {
            const SUMOReal angle = GeomHelper::angleDiff(begShape.angleAt2D(-2), endShape.angleAt2D(0));
            PositionVector endShapeBegLine(endShape[0], endShape[1]);
            PositionVector begShapeEndLineRev(begShape[-1], begShape[-2]);
            endShapeBegLine.extrapolate2D(100, true);
            begShapeEndLineRev.extrapolate2D(100, true);
            if (fabs(angle) < M_PI / 4.) {
                // very low angle: could be an s-shape or a straight line
                const SUMOReal displacementAngle = GeomHelper::angleDiff(begShape.angleAt2D(-2), beg.angleTo2D(end));
                const SUMOReal bendDeg = RAD2DEG(fabs(displacementAngle - angle));
                const SUMOReal halfDistance = dist / 2;
                if (fabs(displacementAngle) <= straightThresh && fabs(angle) <= straightThresh) {
#ifdef DEBUG_SMOOTH_GEOM
                    if (DEBUGCOND) std::cout << "   bezierControlPoints identified straight line beg=" << beg << " end=" << end
                                                 << " angle=" << RAD2DEG(angle) << " displacementAngle=" << RAD2DEG(displacementAngle) << "\n";
#endif
                    return PositionVector();
                } else if (bendDeg > 22.5 && pow(bendDeg / 45, 2) / dist > 0.13) {
                    // do not allow s-curves with extreme bends
                    // (a linear dependency is to restrictive at low displacementAngles and too permisive at high angles)
#ifdef DEBUG_SMOOTH_GEOM
                    if (DEBUGCOND) std::cout << "   bezierControlPoints found extreme s-curve, falling back to straight line beg=" << beg << " end=" << end
                                                 << " angle=" << RAD2DEG(angle) << " displacementAngle=" << RAD2DEG(displacementAngle)
                                                 << " dist=" << dist << " bendDeg=" << bendDeg << " bd2=" << pow(bendDeg / 45, 2)
                                                 << " displacementError=" << sin(displacementAngle) * dist
                                                 << " begShape=" << begShape << " endShape=" << endShape << "\n";
#endif
                    ok = false;
                    if (recordError != 0) {
                        recordError->myDisplacementError = MAX2(recordError->myDisplacementError, (SUMOReal)fabs(sin(displacementAngle) * dist));
                    }
                    return PositionVector();
                } else {
                    const SUMOReal endLength = begShape[-2].distanceTo2D(begShape[-1]);
                    const SUMOReal off1 = endLength + MIN2(extrapolateBeg, halfDistance);
                    init.push_back(PositionVector::positionAtOffset2D(begShapeEndLineRev[1], begShapeEndLineRev[0], off1));
                    const SUMOReal off2 = 100. - MIN2(extrapolateEnd, halfDistance);
                    init.push_back(PositionVector::positionAtOffset2D(endShapeBegLine[0], endShapeBegLine[1], off2));
#ifdef DEBUG_SMOOTH_GEOM
                    if (DEBUGCOND) std::cout << "   bezierControlPoints found s-curve beg=" << beg << " end=" << end
                                                 << " angle=" << RAD2DEG(angle) << " displacementAngle=" << RAD2DEG(displacementAngle)
                                                 << " halfDistance=" << halfDistance << "\n";
#endif
                }
            } else {
                // turning
                //  - end of incoming lane
                //  - intersection of the extrapolated lanes
                //  - begin of outgoing lane
                // attention: if there is no intersection, use a straight line
                Position intersect = endShapeBegLine.intersectionPosition2D(begShapeEndLineRev);
                if (intersect == Position::INVALID) {
#ifdef DEBUG_SMOOTH_GEOM
                    if (DEBUGCOND) {
                        std::cout << "   bezierControlPoints failed beg=" << beg << " end=" << end << " intersect=" << intersect << "\n";
                    }
#endif
                    ok = false;
                    if (recordError != 0) {
                        // it's unclear if this error can be solved via stretching the intersection.
                        recordError->myDisplacementError = MAX2(recordError->myDisplacementError, (SUMOReal)1.0);
                    }
                    return PositionVector();
                }
                const SUMOReal minControlLength = MIN2((SUMOReal)1.0, dist / 2);
                const bool lengthenBeg = intersect.distanceTo2D(beg) <= minControlLength;
                const bool lengthenEnd = intersect.distanceTo2D(end) <= minControlLength;
                if (lengthenBeg && lengthenEnd) {
#ifdef DEBUG_SMOOTH_GEOM
                    if (DEBUGCOND) std::cout << "   bezierControlPoints failed beg=" << beg << " end=" << end << " intersect=" << intersect
                                                 << " dist1=" << intersect.distanceTo2D(beg) << " dist2=" << intersect.distanceTo2D(end) << "\n";
#endif
                    if (recordError != 0) {
                        // This should be fixable with minor stretching
                        recordError->myDisplacementError = MAX2(recordError->myDisplacementError, (SUMOReal)1.0);
                    }
                    ok = false;
                    return PositionVector();
                } else if (lengthenBeg || lengthenEnd) {
                    init.push_back(begShapeEndLineRev.positionAtOffset2D(100 - minControlLength));
                    init.push_back(endShapeBegLine.positionAtOffset2D(100 - minControlLength));
                } else {
                    SUMOReal z;
                    const SUMOReal z1 = begShapeEndLineRev.positionAtOffset2D(begShapeEndLineRev.nearest_offset_to_point2D(intersect)).z();
                    const SUMOReal z2 = endShapeBegLine.positionAtOffset2D(endShapeBegLine.nearest_offset_to_point2D(intersect)).z();
                    const SUMOReal z3 = 0.5 * (beg.z() + end.z());
                    // if z1 and z2 are on the same side in regard to z3 then we
                    // can use their avarage. Otherwise, the intersection in 3D
                    // is not good and we are better of using z3
                    if ((z1 <= z3 && z2 <= z3) || (z1 >= z3 && z2 >= z3)) {
                        z = 0.5 * (z1 + z2);
                    } else {
                        z = z3;
                    }
                    intersect.set(intersect.x(), intersect.y(), z);
                    init.push_back(intersect);
                }
            }
        }
        init.push_back(end);
    }
    return init;
}


PositionVector
NBNode::computeInternalLaneShape(NBEdge* fromE, const NBEdge::Connection& con, int numPoints, NBNode* recordError) const {
    if (con.fromLane >= fromE->getNumLanes()) {
        throw ProcessError("Connection '" + con.getDescription(fromE) + "' starts at a non-existant lane.");
    }
    if (con.toLane >= con.toEdge->getNumLanes()) {
        throw ProcessError("Connection '" + con.getDescription(fromE) + "' targets a non-existant lane.");
    }
    PositionVector ret;
    if (myCustomLaneShapes.size() > 0 && con.internalLaneIndex != NBEdge::UNSPECIFIED_INTERNAL_LANE_INDEX) {
        // this is the second pass (ids and shapes are already set
        assert(con.shape.size() > 0);
        CustomShapeMap::const_iterator it = myCustomLaneShapes.find(con.getInternalLaneID());
        if (it != myCustomLaneShapes.end()) {
            ret = it->second;
        } else {
            ret = con.shape;
        }
        it = myCustomLaneShapes.find(con.viaID + "_0");
        if (it != myCustomLaneShapes.end()) {
            ret.append(it->second);
        } else {
            ret.append(con.viaShape);
        }
        return ret;
    }

    ret = computeSmoothShape(fromE->getLaneShape(con.fromLane), con.toEdge->getLaneShape(con.toLane),
                             numPoints, fromE->getTurnDestination() == con.toEdge,
                             (SUMOReal) 5. * (SUMOReal) fromE->getNumLanes(),
                             (SUMOReal) 5. * (SUMOReal) con.toEdge->getNumLanes(), recordError);
    const NBEdge::Lane& lane = fromE->getLaneStruct(con.fromLane);
    if (lane.endOffset > 0) {
        PositionVector beg = lane.shape.getSubpart(lane.shape.length() - lane.endOffset, lane.shape.length());;
        beg.append(ret);
        ret = beg;
    }
    return ret;
}


bool
NBNode::needsCont(const NBEdge* fromE, const NBEdge* otherFromE,
                  const NBEdge::Connection& c, const NBEdge::Connection& otherC) const {
    const NBEdge* toE = c.toEdge;
    const NBEdge* otherToE = otherC.toEdge;

    if (myType == NODETYPE_RIGHT_BEFORE_LEFT || myType == NODETYPE_ALLWAY_STOP) {
        return false;
    }
    LinkDirection d1 = getDirection(fromE, toE);
    const bool thisRight = (d1 == LINKDIR_RIGHT || d1 == LINKDIR_PARTRIGHT);
    const bool rightTurnConflict = (thisRight &&
                                    NBNode::rightTurnConflict(fromE, toE, c.fromLane, otherFromE, otherToE, otherC.fromLane));
    if (thisRight && !rightTurnConflict) {
        return false;
    }
    if (!(foes(otherFromE, otherToE, fromE, toE) || myRequest == 0 || rightTurnConflict)) {
        // if they do not cross, no waiting place is needed
        return false;
    }
    LinkDirection d2 = getDirection(otherFromE, otherToE);
    if (d2 == LINKDIR_TURN) {
        return false;
    }
    const bool thisLeft = (d1 == LINKDIR_LEFT || d1 == LINKDIR_TURN);
    const bool otherLeft = (d2 == LINKDIR_LEFT || d2 == LINKDIR_TURN);
    const bool bothLeft = thisLeft && otherLeft;
    if (fromE == otherFromE && !thisRight) {
        // ignore same edge links except for right-turns
        return false;
    }
    if (thisRight && d2 != LINKDIR_STRAIGHT) {
        return false;
    }
    if (c.tlID != "" && !bothLeft) {
        assert(myTrafficLights.size() > 0 || myType == NODETYPE_RAIL_CROSSING || myType == NODETYPE_RAIL_SIGNAL);
        for (std::set<NBTrafficLightDefinition*>::const_iterator it = myTrafficLights.begin(); it != myTrafficLights.end(); ++it) {
            if ((*it)->needsCont(fromE, toE, otherFromE, otherToE)) {
                return true;
            }
        }
        return false;
    }
    if (fromE->getJunctionPriority(this) > 0 && otherFromE->getJunctionPriority(this) > 0) {
        return mustBrake(fromE, toE, c.fromLane, c.toLane, false);
    }
    return false;
}


void
NBNode::computeLogic(const NBEdgeCont& ec, OptionsCont& oc) {
    delete myRequest; // possibly recomputation step
    myRequest = 0;
    if (myIncomingEdges.size() == 0 || myOutgoingEdges.size() == 0) {
        // no logic if nothing happens here
        myType = NODETYPE_NOJUNCTION;
        std::set<NBTrafficLightDefinition*> trafficLights = myTrafficLights; // make a copy because we will modify the original
        for (std::set<NBTrafficLightDefinition*>::const_iterator i = trafficLights.begin(); i != trafficLights.end(); ++i) {
            // if this is the only controlled node we keep the tlDef as it is to generate a warning later
            if ((*i)->getNodes().size() > 1) {
                myTrafficLights.erase(*i);
                (*i)->removeNode(this);
                (*i)->setParticipantsInformation();
                (*i)->setTLControllingInformation();
            }
        }
        return;
    }
    // check whether the node was set to be unregulated by the user
    if (oc.getBool("keep-nodes-unregulated") || oc.isInStringVector("keep-nodes-unregulated.explicit", getID())
            || (oc.getBool("keep-nodes-unregulated.district-nodes") && (isNearDistrict() || isDistrict()))) {
        myType = NODETYPE_NOJUNCTION;
        return;
    }
    // compute the logic if necessary or split the junction
    if (myType != NODETYPE_NOJUNCTION && myType != NODETYPE_DISTRICT && myType != NODETYPE_TRAFFIC_LIGHT_NOJUNCTION) {
        // build the request
        myRequest = new NBRequest(ec, this, myAllEdges, myIncomingEdges, myOutgoingEdges, myBlockedConnections);
        // check whether it is not too large
        int numConnections = numNormalConnections();
        if (numConnections >= SUMO_MAX_CONNECTIONS) {
            // yep -> make it untcontrolled, warn
            delete myRequest;
            myRequest = 0;
            if (myType == NODETYPE_TRAFFIC_LIGHT) {
                myType = NODETYPE_TRAFFIC_LIGHT_NOJUNCTION;
            } else {
                myType = NODETYPE_NOJUNCTION;
            }
            WRITE_WARNING("Junction '" + getID() + "' is too complicated (" + toString(numConnections)
                          + " connections, max " + toString(SUMO_MAX_CONNECTIONS) + "); will be set to " + toString(myType));
        } else if (numConnections == 0) {
            delete myRequest;
            myRequest = 0;
            myType = NODETYPE_DEAD_END;
        } else {
            myRequest->buildBitfieldLogic();
        }
    }
}


bool
NBNode::writeLogic(OutputDevice& into, const bool checkLaneFoes) const {
    if (myRequest) {
        myRequest->writeLogic(myID, into, checkLaneFoes);
        return true;
    }
    return false;
}


void
NBNode::computeNodeShape(SUMOReal mismatchThreshold) {
    if (myHaveCustomPoly) {
        return;
    }
    if (myIncomingEdges.size() == 0 && myOutgoingEdges.size() == 0) {
        // may be an intermediate step during network editing
        myPoly.clear();
        myPoly.push_back(myPosition);
        return;
    }
    try {
        NBNodeShapeComputer computer(*this);
        myPoly = computer.compute();
        if (myPoly.size() > 0) {
            PositionVector tmp = myPoly;
            tmp.push_back_noDoublePos(tmp[0]); // need closed shape
            if (mismatchThreshold >= 0
                    && !tmp.around(myPosition)
                    && tmp.distance2D(myPosition) > mismatchThreshold) {
                WRITE_WARNING("Shape for junction '" + myID + "' has distance " + toString(tmp.distance2D(myPosition)) + " to its given position");
            }
        }
    } catch (InvalidArgument&) {
        WRITE_WARNING("For junction '" + getID() + "': could not compute shape.");
        // make sure our shape is not empty because our XML schema forbids empty attributes
        myPoly.clear();
        myPoly.push_back(myPosition);
    }
}


void
NBNode::computeLanes2Lanes() {
    // special case a):
    //  one in, one out, the outgoing has one lane more
    if (myIncomingEdges.size() == 1 && myOutgoingEdges.size() == 1) {
        NBEdge* in = myIncomingEdges[0];
        NBEdge* out = myOutgoingEdges[0];
        // check if it's not the turnaround
        if (in->getTurnDestination() == out) {
            // will be added later or not...
            return;
        }
        const int inOffset = MAX2(0, in->getFirstNonPedestrianLaneIndex(FORWARD, true));
        const int outOffset = MAX2(0, out->getFirstNonPedestrianLaneIndex(FORWARD, true));
        if (in->getStep() <= NBEdge::LANES2EDGES
                && in->getNumLanes() - inOffset == out->getNumLanes() - outOffset - 1
                && in != out
                && in->isConnectedTo(out)) {
            for (int i = inOffset; i < in->getNumLanes(); ++i) {
                in->setConnection(i, out, i - inOffset + outOffset + 1, NBEdge::L2L_COMPUTED);
            }
            in->setConnection(inOffset, out, outOffset, NBEdge::L2L_COMPUTED);
            return;
        }
    }
    // special case b):
    //  two in, one out, the outgoing has the same number of lanes as the sum of the incoming
    //  --> highway on-ramp
    if (myIncomingEdges.size() == 2 && myOutgoingEdges.size() == 1) {
        NBEdge* out = myOutgoingEdges[0];
        NBEdge* in1 = myIncomingEdges[0];
        NBEdge* in2 = myIncomingEdges[1];
        const int outOffset = MAX2(0, out->getFirstNonPedestrianLaneIndex(FORWARD, true));
        int in1Offset = MAX2(0, in1->getFirstNonPedestrianLaneIndex(FORWARD, true));
        int in2Offset = MAX2(0, in2->getFirstNonPedestrianLaneIndex(FORWARD, true));
        if (in1->getNumLanes() + in2->getNumLanes() - in1Offset - in2Offset == out->getNumLanes() - outOffset
                && (in1->getStep() <= NBEdge::LANES2EDGES)
                && (in2->getStep() <= NBEdge::LANES2EDGES)
                && in1 != out
                && in2 != out
                && in1->isConnectedTo(out)
                && in2->isConnectedTo(out)
                && isLongEnough(out, MIN_WEAVE_LENGTH)) {
            // for internal: check which one is the rightmost
            SUMOReal a1 = in1->getAngleAtNode(this);
            SUMOReal a2 = in2->getAngleAtNode(this);
            SUMOReal ccw = GeomHelper::getCCWAngleDiff(a1, a2);
            SUMOReal cw = GeomHelper::getCWAngleDiff(a1, a2);
            if (ccw > cw) {
                std::swap(in1, in2);
                std::swap(in1Offset, in2Offset);
            }
            in1->addLane2LaneConnections(in1Offset, out, outOffset, in1->getNumLanes() - in1Offset, NBEdge::L2L_VALIDATED, true);
            in2->addLane2LaneConnections(in2Offset, out, in1->getNumLanes() + outOffset - in1Offset, in2->getNumLanes() - in2Offset, NBEdge::L2L_VALIDATED, true);
            return;
        }
    }
    // special case c):
    //  one in, two out, the incoming has the same number of lanes or only 1 lane less than the sum of the outgoing lanes
    //  --> highway off-ramp
    if (myIncomingEdges.size() == 1 && myOutgoingEdges.size() == 2) {
        NBEdge* in = myIncomingEdges[0];
        NBEdge* out1 = myOutgoingEdges[0];
        NBEdge* out2 = myOutgoingEdges[1];
        const int inOffset = MAX2(0, in->getFirstNonPedestrianLaneIndex(FORWARD, true));
        int out1Offset = MAX2(0, out1->getFirstNonPedestrianLaneIndex(FORWARD, true));
        int out2Offset = MAX2(0, out2->getFirstNonPedestrianLaneIndex(FORWARD, true));
        const int deltaLaneSum = (out2->getNumLanes() + out1->getNumLanes() - out1Offset - out2Offset) - (in->getNumLanes() - inOffset);
        if ((deltaLaneSum == 0 || (deltaLaneSum == 1 && in->getPermissionVariants(inOffset, in->getNumLanes()).size() == 1))
                && (in->getStep() <= NBEdge::LANES2EDGES)
                && in != out1
                && in != out2
                && in->isConnectedTo(out1)
                && in->isConnectedTo(out2)
                && !in->isTurningDirectionAt(out1)
                && !in->isTurningDirectionAt(out2)
           ) {
            // for internal: check which one is the rightmost
            if (NBContHelper::relative_outgoing_edge_sorter(in)(out2, out1)) {
                std::swap(out1, out2);
                std::swap(out1Offset, out2Offset);
            }
            in->addLane2LaneConnections(inOffset, out1, out1Offset, out1->getNumLanes() - out1Offset, NBEdge::L2L_VALIDATED, true);
            in->addLane2LaneConnections(out1->getNumLanes() + inOffset - out1Offset - deltaLaneSum, out2, out2Offset, out2->getNumLanes() - out2Offset, NBEdge::L2L_VALIDATED, false);
            return;
        }
    }
    // special case d):
    //  one in, one out, the outgoing has one lane less and node has type 'zipper'
    if (myIncomingEdges.size() == 1 && myOutgoingEdges.size() == 1 && myType == NODETYPE_ZIPPER) {
        NBEdge* in = myIncomingEdges[0];
        NBEdge* out = myOutgoingEdges[0];
        // check if it's not the turnaround
        if (in->getTurnDestination() == out) {
            // will be added later or not...
            return;
        }
        const int inOffset = MAX2(0, in->getFirstNonPedestrianLaneIndex(FORWARD, true));
        const int outOffset = MAX2(0, out->getFirstNonPedestrianLaneIndex(FORWARD, true));
        if (in->getStep() <= NBEdge::LANES2EDGES
                && in->getNumLanes() - inOffset == out->getNumLanes() - outOffset + 1
                && in != out
                && in->isConnectedTo(out)) {
            for (int i = inOffset; i < in->getNumLanes(); ++i) {
                in->setConnection(i, out, MIN2(outOffset + i, out->getNumLanes() - 1), NBEdge::L2L_COMPUTED, true);
            }
            return;
        }
    }
    // special case f):
    //  one in, one out, out has reduced or same number of lanes
    if (myIncomingEdges.size() == 1 && myOutgoingEdges.size() == 1) {
        NBEdge* in = myIncomingEdges[0];
        NBEdge* out = myOutgoingEdges[0];
        // check if it's not the turnaround
        if (in->getTurnDestination() == out) {
            // will be added later or not...
            return;
        }
        int inOffset = MAX2(0, in->getFirstNonPedestrianLaneIndex(FORWARD, true));
        const int outOffset = MAX2(0, out->getFirstNonPedestrianLaneIndex(FORWARD, true));
        const int reduction = (in->getNumLanes() - inOffset) - (out->getNumLanes() - outOffset);
        if (in->getStep() <= NBEdge::LANES2EDGES
                && reduction >= 0
                && in != out
                && in->isConnectedTo(out)) {
            // in case of reduced lane number, let the rightmost lanse end
            inOffset += reduction;
            for (int i = outOffset; i < out->getNumLanes(); ++i) {
                in->setConnection(i + inOffset - outOffset, out, i, NBEdge::L2L_COMPUTED);
            }
            //std::cout << " special case f at node=" << getID() << " inOffset=" << inOffset << " outOffset=" << outOffset << "\n";
            return;
        }
    }

    // go through this node's outgoing edges
    //  for every outgoing edge, compute the distribution of the node's
    //  incoming edges on this edge when approaching this edge
    // the incoming edges' steps will then also be marked as LANE2LANE_RECHECK...
    EdgeVector::reverse_iterator i;
    for (i = myOutgoingEdges.rbegin(); i != myOutgoingEdges.rend(); i++) {
        NBEdge* currentOutgoing = *i;
        // get the information about edges that do approach this edge
        EdgeVector* approaching = getEdgesThatApproach(currentOutgoing);
        const int numApproaching = (int)approaching->size();
        if (numApproaching != 0) {
            ApproachingDivider divider(approaching, currentOutgoing);
            Bresenham::compute(&divider, numApproaching, divider.numAvailableLanes());
        }
        delete approaching;

        // ensure that all modes have a connection if possible
        for (EdgeVector::const_iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
            NBEdge* incoming = *i;
            if (incoming->getConnectionLanes(currentOutgoing).size() > 0 && incoming->getStep() <= NBEdge::LANES2LANES_DONE) {
                // no connections are needed for pedestrians during this step
                // no satisfaction is possible if the outgoing edge disallows
                SVCPermissions unsatisfied = incoming->getPermissions() & currentOutgoing->getPermissions() & ~SVC_PEDESTRIAN;
                //std::cout << "initial unsatisfied modes from edge=" << incoming->getID() << " toEdge=" << currentOutgoing->getID() << " deadModes=" << getVehicleClassNames(unsatisfied) << "\n";
                const std::vector<NBEdge::Connection>& elv = incoming->getConnections();
                for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
                    const NBEdge::Connection& c = *k;
                    if (c.toEdge == currentOutgoing) {
                        const SVCPermissions satisfied = (incoming->getPermissions(c.fromLane) & c.toEdge->getPermissions(c.toLane));
                        //std::cout << "  from=" << c.fromLane << " to=" << c.toEdge->getID() << "_" << c.toLane << " satisfied=" << getVehicleClassNames(satisfied) << "\n";
                        unsatisfied &= ~satisfied;
                    }
                }
                if (unsatisfied != 0) {
                    //std::cout << " unsatisfied modes from edge=" << incoming->getID() << " toEdge=" << currentOutgoing->getID() << " deadModes=" << getVehicleClassNames(unsatisfied) << "\n";
                    int fromLane = 0;
                    while (unsatisfied != 0 && fromLane < incoming->getNumLanes()) {
                        if ((incoming->getPermissions(fromLane) & unsatisfied) != 0) {
                            for (int toLane = 0; toLane < currentOutgoing->getNumLanes(); ++toLane) {
                                const SVCPermissions satisfied = incoming->getPermissions(fromLane) & currentOutgoing->getPermissions(toLane) & unsatisfied;
                                if (satisfied != 0) {
                                    incoming->setConnection((int)fromLane, currentOutgoing, toLane, NBEdge::L2L_COMPUTED);
                                    //std::cout << "  new connection from=" << fromLane << " to=" << currentOutgoing->getID() << "_" << toLane << " satisfies=" << getVehicleClassNames(satisfied) << "\n";
                                    unsatisfied &= ~satisfied;
                                }
                            }
                        }
                        fromLane++;
                    }
                    //if (unsatisfied != 0) {
                    //    std::cout << "     still unsatisfied modes from edge=" << incoming->getID() << " toEdge=" << currentOutgoing->getID() << " deadModes=" << getVehicleClassNames(unsatisfied) << "\n";
                    //}
                }
            }
        }
    }
    // special case e): rail_crossing
    // there should only be straight connections here
    if (myType == NODETYPE_RAIL_CROSSING) {
        for (EdgeVector::const_iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
            const std::vector<NBEdge::Connection> cons = (*i)->getConnections();
            for (std::vector<NBEdge::Connection>::const_iterator k = cons.begin(); k != cons.end(); ++k) {
                if (getDirection(*i, (*k).toEdge) == LINKDIR_TURN) {
                    (*i)->removeFromConnections((*k).toEdge);
                }
            }
        }
    }

    // ... but we may have the case that there are no outgoing edges
    //  In this case, we have to mark the incoming edges as being in state
    //   LANE2LANE( not RECHECK) by hand
    if (myOutgoingEdges.size() == 0) {
        for (i = myIncomingEdges.rbegin(); i != myIncomingEdges.rend(); i++) {
            (*i)->markAsInLane2LaneState();
        }
    }

    // DEBUG
    //std::cout << "connections at " << getID() << "\n";
    //for (i = myIncomingEdges.rbegin(); i != myIncomingEdges.rend(); i++) {
    //    const std::vector<NBEdge::Connection>& elv = (*i)->getConnections();
    //    for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
    //        std::cout << "  " << (*i)->getID() << "_" << (*k).fromLane << " -> " << (*k).toEdge->getID() << "_" << (*k).toLane << "\n";
    //    }
    //}
}

bool
NBNode::isLongEnough(NBEdge* out, SUMOReal minLength) {
    SUMOReal seen = out->getLoadedLength();
    while (seen < minLength) {
        // advance along trivial continuations
        if (out->getToNode()->getOutgoingEdges().size() != 1
                || out->getToNode()->getIncomingEdges().size() != 1) {
            return false;
        } else {
            out = out->getToNode()->getOutgoingEdges()[0];
            seen += out->getLoadedLength();
        }
    }
    return true;
}

EdgeVector*
NBNode::getEdgesThatApproach(NBEdge* currentOutgoing) {
    // get the position of the node to get the approaching nodes of
    EdgeVector::const_iterator i = find(myAllEdges.begin(),
                                        myAllEdges.end(), currentOutgoing);
    // get the first possible approaching edge
    NBContHelper::nextCW(myAllEdges, i);
    // go through the list of edges clockwise and add the edges
    EdgeVector* approaching = new EdgeVector();
    for (; *i != currentOutgoing;) {
        // check only incoming edges
        if ((*i)->getToNode() == this && (*i)->getTurnDestination() != currentOutgoing) {
            std::vector<int> connLanes = (*i)->getConnectionLanes(currentOutgoing);
            if (connLanes.size() != 0) {
                approaching->push_back(*i);
            }
        }
        NBContHelper::nextCW(myAllEdges, i);
    }
    return approaching;
}


void
NBNode::replaceOutgoing(NBEdge* which, NBEdge* by, int laneOff) {
    // replace the edge in the list of outgoing nodes
    EdgeVector::iterator i = find(myOutgoingEdges.begin(), myOutgoingEdges.end(), which);
    if (i != myOutgoingEdges.end()) {
        (*i) = by;
        i = find(myAllEdges.begin(), myAllEdges.end(), which);
        (*i) = by;
    }
    // replace the edge in connections of incoming edges
    for (i = myIncomingEdges.begin(); i != myIncomingEdges.end(); ++i) {
        (*i)->replaceInConnections(which, by, laneOff);
    }
    // replace within the connetion prohibition dependencies
    replaceInConnectionProhibitions(which, by, 0, laneOff);
}


void
NBNode::replaceOutgoing(const EdgeVector& which, NBEdge* by) {
    // replace edges
    int laneOff = 0;
    for (EdgeVector::const_iterator i = which.begin(); i != which.end(); i++) {
        replaceOutgoing(*i, by, laneOff);
        laneOff += (*i)->getNumLanes();
    }
    // removed SUMOReal occurences
    removeDoubleEdges();
    // check whether this node belongs to a district and the edges
    //  must here be also remapped
    if (myDistrict != 0) {
        myDistrict->replaceOutgoing(which, by);
    }
}


void
NBNode::replaceIncoming(NBEdge* which, NBEdge* by, int laneOff) {
    // replace the edge in the list of incoming nodes
    EdgeVector::iterator i = find(myIncomingEdges.begin(), myIncomingEdges.end(), which);
    if (i != myIncomingEdges.end()) {
        (*i) = by;
        i = find(myAllEdges.begin(), myAllEdges.end(), which);
        (*i) = by;
    }
    // replace within the connetion prohibition dependencies
    replaceInConnectionProhibitions(which, by, laneOff, 0);
}


void
NBNode::replaceIncoming(const EdgeVector& which, NBEdge* by) {
    // replace edges
    int laneOff = 0;
    for (EdgeVector::const_iterator i = which.begin(); i != which.end(); i++) {
        replaceIncoming(*i, by, laneOff);
        laneOff += (*i)->getNumLanes();
    }
    // removed SUMOReal occurences
    removeDoubleEdges();
    // check whether this node belongs to a district and the edges
    //  must here be also remapped
    if (myDistrict != 0) {
        myDistrict->replaceIncoming(which, by);
    }
}



void
NBNode::replaceInConnectionProhibitions(NBEdge* which, NBEdge* by,
                                        int whichLaneOff, int byLaneOff) {
    // replace in keys
    NBConnectionProhibits::iterator j = myBlockedConnections.begin();
    while (j != myBlockedConnections.end()) {
        bool changed = false;
        NBConnection c = (*j).first;
        if (c.replaceFrom(which, whichLaneOff, by, byLaneOff)) {
            changed = true;
        }
        if (c.replaceTo(which, whichLaneOff, by, byLaneOff)) {
            changed = true;
        }
        if (changed) {
            myBlockedConnections[c] = (*j).second;
            myBlockedConnections.erase(j);
            j = myBlockedConnections.begin();
        } else {
            j++;
        }
    }
    // replace in values
    for (j = myBlockedConnections.begin(); j != myBlockedConnections.end(); j++) {
        NBConnectionVector& prohibiting = (*j).second;
        for (NBConnectionVector::iterator k = prohibiting.begin(); k != prohibiting.end(); k++) {
            NBConnection& sprohibiting = *k;
            sprohibiting.replaceFrom(which, whichLaneOff, by, byLaneOff);
            sprohibiting.replaceTo(which, whichLaneOff, by, byLaneOff);
        }
    }
}



void
NBNode::removeDoubleEdges() {
    // check incoming
    for (int i = 0; myIncomingEdges.size() > 0 && i < (int)myIncomingEdges.size() - 1; i++) {
        int j = i + 1;
        while (j < (int)myIncomingEdges.size()) {
            if (myIncomingEdges[i] == myIncomingEdges[j]) {
                myIncomingEdges.erase(myIncomingEdges.begin() + j);
            } else {
                j++;
            }
        }
    }
    // check outgoing
    for (int i = 0; myOutgoingEdges.size() > 0 && i < (int)myOutgoingEdges.size() - 1; i++) {
        int j = i + 1;
        while (j < (int)myOutgoingEdges.size()) {
            if (myOutgoingEdges[i] == myOutgoingEdges[j]) {
                myOutgoingEdges.erase(myOutgoingEdges.begin() + j);
            } else {
                j++;
            }
        }
    }
    // check all
    for (int i = 0; myAllEdges.size() > 0 && i < (int)myAllEdges.size() - 1; i++) {
        int j = i + 1;
        while (j < (int)myAllEdges.size()) {
            if (myAllEdges[i] == myAllEdges[j]) {
                myAllEdges.erase(myAllEdges.begin() + j);
            } else {
                j++;
            }
        }
    }
}


bool
NBNode::hasIncoming(const NBEdge* const e) const {
    return find(myIncomingEdges.begin(), myIncomingEdges.end(), e) != myIncomingEdges.end();
}


bool
NBNode::hasOutgoing(const NBEdge* const e) const {
    return find(myOutgoingEdges.begin(), myOutgoingEdges.end(), e) != myOutgoingEdges.end();
}


NBEdge*
NBNode::getOppositeIncoming(NBEdge* e) const {
    EdgeVector edges = myIncomingEdges;
    if (find(edges.begin(), edges.end(), e) != edges.end()) {
        edges.erase(find(edges.begin(), edges.end(), e));
    }
    if (edges.size() == 0) {
        return 0;
    }
    if (e->getToNode() == this) {
        sort(edges.begin(), edges.end(), NBContHelper::edge_opposite_direction_sorter(e, this));
    } else {
        sort(edges.begin(), edges.end(), NBContHelper::edge_similar_direction_sorter(e));
    }
    return edges[0];
}


void
NBNode::addSortedLinkFoes(const NBConnection& mayDrive,
                          const NBConnection& mustStop) {
    if (mayDrive.getFrom() == 0 ||
            mayDrive.getTo() == 0 ||
            mustStop.getFrom() == 0 ||
            mustStop.getTo() == 0) {

        WRITE_WARNING("Something went wrong during the building of a connection...");
        return; // !!! mark to recompute connections
    }
    NBConnectionVector conn = myBlockedConnections[mustStop];
    conn.push_back(mayDrive);
    myBlockedConnections[mustStop] = conn;
}


NBEdge*
NBNode::getPossiblySplittedIncoming(const std::string& edgeid) {
    int size = (int) edgeid.length();
    for (EdgeVector::iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
        std::string id = (*i)->getID();
        if (id.substr(0, size) == edgeid) {
            return *i;
        }
    }
    return 0;
}


NBEdge*
NBNode::getPossiblySplittedOutgoing(const std::string& edgeid) {
    int size = (int) edgeid.length();
    for (EdgeVector::iterator i = myOutgoingEdges.begin(); i != myOutgoingEdges.end(); i++) {
        std::string id = (*i)->getID();
        if (id.substr(0, size) == edgeid) {
            return *i;
        }
    }
    return 0;
}


void
NBNode::removeEdge(NBEdge* edge, bool removeFromConnections) {
    EdgeVector::iterator i = find(myAllEdges.begin(), myAllEdges.end(), edge);
    if (i != myAllEdges.end()) {
        myAllEdges.erase(i);
        i = find(myOutgoingEdges.begin(), myOutgoingEdges.end(), edge);
        if (i != myOutgoingEdges.end()) {
            myOutgoingEdges.erase(i);
        } else {
            i = find(myIncomingEdges.begin(), myIncomingEdges.end(), edge);
            if (i != myIncomingEdges.end()) {
                myIncomingEdges.erase(i);
            } else {
                // edge must have been either incoming or outgoing
                assert(false);
            }
        }
        if (removeFromConnections) {
            for (i = myAllEdges.begin(); i != myAllEdges.end(); ++i) {
                (*i)->removeFromConnections(edge);
            }
        }
        // invalidate controlled connections for loaded traffic light plans
        for (std::set<NBTrafficLightDefinition*>::iterator i = myTrafficLights.begin(); i != myTrafficLights.end(); ++i) {
            (*i)->replaceRemoved(edge, -1, 0, -1);
        }
    }
}


Position
NBNode::getEmptyDir() const {
    Position pos(0, 0);
    EdgeVector::const_iterator i;
    for (i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
        NBNode* conn = (*i)->getFromNode();
        Position toAdd = conn->getPosition();
        toAdd.sub(myPosition);
        toAdd.mul((SUMOReal) 1.0 / sqrt(toAdd.x()*toAdd.x() + toAdd.y()*toAdd.y()));
        pos.add(toAdd);
    }
    for (i = myOutgoingEdges.begin(); i != myOutgoingEdges.end(); i++) {
        NBNode* conn = (*i)->getToNode();
        Position toAdd = conn->getPosition();
        toAdd.sub(myPosition);
        toAdd.mul((SUMOReal) 1.0 / sqrt(toAdd.x()*toAdd.x() + toAdd.y()*toAdd.y()));
        pos.add(toAdd);
    }
    pos.mul((SUMOReal) - 1.0 / (myIncomingEdges.size() + myOutgoingEdges.size()));
    if (pos.x() == 0 && pos.y() == 0) {
        pos = Position(1, 0);
    }
    pos.norm2d();
    return pos;
}



void
NBNode::invalidateIncomingConnections() {
    for (EdgeVector::const_iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
        (*i)->invalidateConnections();
    }
}


void
NBNode::invalidateOutgoingConnections() {
    for (EdgeVector::const_iterator i = myOutgoingEdges.begin(); i != myOutgoingEdges.end(); i++) {
        (*i)->invalidateConnections();
    }
}


bool
NBNode::mustBrake(const NBEdge* const from, const NBEdge* const to, int fromLane, int toLane, bool includePedCrossings) const {
    // unregulated->does not need to brake
    if (myRequest == 0) {
        return false;
    }
    // vehicles which do not have a following lane must always decelerate to the end
    if (to == 0) {
        return true;
    }
    // check whether any other connection on this node prohibits this connection
    return myRequest->mustBrake(from, to, fromLane, toLane, includePedCrossings);
}

bool
NBNode::mustBrakeForCrossing(const NBEdge* const from, const NBEdge* const to, const NBNode::Crossing& crossing) const {
    return NBRequest::mustBrakeForCrossing(this, from, to, crossing);
}


bool
NBNode::rightTurnConflict(const NBEdge* from, const NBEdge* to, int fromLane,
                          const NBEdge* prohibitorFrom, const NBEdge* prohibitorTo, int prohibitorFromLane,
                          bool lefthand) {
    if (from != prohibitorFrom) {
        return false;
    }
    if (from->isTurningDirectionAt(to)
            || prohibitorFrom->isTurningDirectionAt(prohibitorTo)) {
        // XXX should warn if there are any non-turning connections left of this
        return false;
    }
    // conflict if to is between prohibitorTo and from when going clockwise
    if (to->getStartAngle() == prohibitorTo->getStartAngle()) {
        // reduce rounding errors
        return false;
    }
    const LinkDirection d1 = from->getToNode()->getDirection(from, to);
    // must be a right turn to qualify as rightTurnConflict
    if (d1 == LINKDIR_STRAIGHT) {
        // no conflict for straight going connections
        // XXX actually this should check the main direction (which could also
        // be a turn)
        return false;
    } else {
        const LinkDirection d2 = prohibitorFrom->getToNode()->getDirection(prohibitorFrom, prohibitorTo);
        if (d1 == LINKDIR_LEFT || d1 == LINKDIR_PARTLEFT) {
            // check for leftTurnConflicht
            lefthand = !lefthand;
            if (d2 == LINKDIR_RIGHT || d1 == LINKDIR_PARTRIGHT) {
                // assume that the left-turning bicycle goes straight at first
                // and thus gets precedence over a right turning vehicle
                return false;
            }
        }
        if ((!lefthand && fromLane <= prohibitorFromLane) ||
                (lefthand && fromLane >= prohibitorFromLane)) {
            return false;
        }
        const SUMOReal toAngleAtNode = fmod(to->getStartAngle() + 180, (SUMOReal)360.0);
        const SUMOReal prohibitorToAngleAtNode = fmod(prohibitorTo->getStartAngle() + 180, (SUMOReal)360.0);
        return (lefthand != (GeomHelper::getCWAngleDiff(from->getEndAngle(), toAngleAtNode) <
                             GeomHelper::getCWAngleDiff(from->getEndAngle(), prohibitorToAngleAtNode)));
    }
}


bool
NBNode::isLeftMover(const NBEdge* const from, const NBEdge* const to) const {
    // when the junction has only one incoming edge, there are no
    //  problems caused by left blockings
    if (myIncomingEdges.size() == 1 || myOutgoingEdges.size() == 1) {
        return false;
    }
    SUMOReal fromAngle = from->getAngleAtNode(this);
    SUMOReal toAngle = to->getAngleAtNode(this);
    SUMOReal cw = GeomHelper::getCWAngleDiff(fromAngle, toAngle);
    SUMOReal ccw = GeomHelper::getCCWAngleDiff(fromAngle, toAngle);
    std::vector<NBEdge*>::const_iterator i = std::find(myAllEdges.begin(), myAllEdges.end(), from);
    do {
        NBContHelper::nextCW(myAllEdges, i);
    } while ((!hasOutgoing(*i) || from->isTurningDirectionAt(*i)) && *i != from);
    return cw < ccw && (*i) == to && myOutgoingEdges.size() > 2;
}


bool
NBNode::forbids(const NBEdge* const possProhibitorFrom, const NBEdge* const possProhibitorTo,
                const NBEdge* const possProhibitedFrom, const NBEdge* const possProhibitedTo,
                bool regardNonSignalisedLowerPriority) const {
    return myRequest != 0 && myRequest->forbids(possProhibitorFrom, possProhibitorTo,
            possProhibitedFrom, possProhibitedTo,
            regardNonSignalisedLowerPriority);
}


bool
NBNode::foes(const NBEdge* const from1, const NBEdge* const to1,
             const NBEdge* const from2, const NBEdge* const to2) const {
    return myRequest != 0 && myRequest->foes(from1, to1, from2, to2);
}


void
NBNode::remapRemoved(NBTrafficLightLogicCont& tc,
                     NBEdge* removed, const EdgeVector& incoming,
                     const EdgeVector& outgoing) {
    assert(find(incoming.begin(), incoming.end(), removed) == incoming.end());
    bool changed = true;
    while (changed) {
        changed = false;
        NBConnectionProhibits blockedConnectionsTmp = myBlockedConnections;
        NBConnectionProhibits blockedConnectionsNew;
        // remap in connections
        for (NBConnectionProhibits::iterator i = blockedConnectionsTmp.begin(); i != blockedConnectionsTmp.end(); i++) {
            const NBConnection& blocker = (*i).first;
            const NBConnectionVector& blocked = (*i).second;
            // check the blocked connections first
            // check whether any of the blocked must be changed
            bool blockedChanged = false;
            NBConnectionVector newBlocked;
            NBConnectionVector::const_iterator j;
            for (j = blocked.begin(); j != blocked.end(); j++) {
                const NBConnection& sblocked = *j;
                if (sblocked.getFrom() == removed || sblocked.getTo() == removed) {
                    blockedChanged = true;
                }
            }
            // adapt changes if so
            for (j = blocked.begin(); blockedChanged && j != blocked.end(); j++) {
                const NBConnection& sblocked = *j;
                if (sblocked.getFrom() == removed && sblocked.getTo() == removed) {
                    /*                    for(EdgeVector::const_iterator k=incoming.begin(); k!=incoming.end(); k++) {
                    !!!                        newBlocked.push_back(NBConnection(*k, *k));
                                        }*/
                } else if (sblocked.getFrom() == removed) {
                    assert(sblocked.getTo() != removed);
                    for (EdgeVector::const_iterator k = incoming.begin(); k != incoming.end(); k++) {
                        newBlocked.push_back(NBConnection(*k, sblocked.getTo()));
                    }
                } else if (sblocked.getTo() == removed) {
                    assert(sblocked.getFrom() != removed);
                    for (EdgeVector::const_iterator k = outgoing.begin(); k != outgoing.end(); k++) {
                        newBlocked.push_back(NBConnection(sblocked.getFrom(), *k));
                    }
                } else {
                    newBlocked.push_back(NBConnection(sblocked.getFrom(), sblocked.getTo()));
                }
            }
            if (blockedChanged) {
                blockedConnectionsNew[blocker] = newBlocked;
                changed = true;
            }
            // if the blocked were kept
            else {
                if (blocker.getFrom() == removed && blocker.getTo() == removed) {
                    changed = true;
                    /*                    for(EdgeVector::const_iterator k=incoming.begin(); k!=incoming.end(); k++) {
                    !!!                        blockedConnectionsNew[NBConnection(*k, *k)] = blocked;
                                        }*/
                } else if (blocker.getFrom() == removed) {
                    assert(blocker.getTo() != removed);
                    changed = true;
                    for (EdgeVector::const_iterator k = incoming.begin(); k != incoming.end(); k++) {
                        blockedConnectionsNew[NBConnection(*k, blocker.getTo())] = blocked;
                    }
                } else if (blocker.getTo() == removed) {
                    assert(blocker.getFrom() != removed);
                    changed = true;
                    for (EdgeVector::const_iterator k = outgoing.begin(); k != outgoing.end(); k++) {
                        blockedConnectionsNew[NBConnection(blocker.getFrom(), *k)] = blocked;
                    }
                } else {
                    blockedConnectionsNew[blocker] = blocked;
                }
            }
        }
        myBlockedConnections = blockedConnectionsNew;
    }
    // remap in traffic lights
    tc.remapRemoved(removed, incoming, outgoing);
}


LinkDirection
NBNode::getDirection(const NBEdge* const incoming, const NBEdge* const outgoing, bool leftHand) const {
    // ok, no connection at all -> dead end
    if (outgoing == 0) {
        return LINKDIR_NODIR;
    }
    // turning direction
    if (incoming->isTurningDirectionAt(outgoing)) {
        return leftHand ? LINKDIR_TURN_LEFTHAND : LINKDIR_TURN;
    }
    // get the angle between incoming/outgoing at the junction
    SUMOReal angle =
        NBHelpers::normRelAngle(incoming->getAngleAtNode(this), outgoing->getAngleAtNode(this));
    // ok, should be a straight connection
    if (abs((int) angle) + 1 < 45) {
        return LINKDIR_STRAIGHT;
    }

    // check for left and right, first
    if (angle > 0) {
        // check whether any other edge goes further to the right
        EdgeVector::const_iterator i =
            find(myAllEdges.begin(), myAllEdges.end(), outgoing);
        if (leftHand) {
            NBContHelper::nextCCW(myAllEdges, i);
        } else {
            NBContHelper::nextCW(myAllEdges, i);
        }
        while ((*i) != incoming) {
            if ((*i)->getFromNode() == this && !incoming->isTurningDirectionAt(*i)) {
                //std::cout << incoming->getID() << " -> " << outgoing->getID() << " partRight because auf " << (*i)->getID() << "\n";
                return LINKDIR_PARTRIGHT;
            }
            if (leftHand) {
                NBContHelper::nextCCW(myAllEdges, i);
            } else {
                NBContHelper::nextCW(myAllEdges, i);
            }
        }
        return LINKDIR_RIGHT;
    }
    // check whether any other edge goes further to the left
    EdgeVector::const_iterator i =
        find(myAllEdges.begin(), myAllEdges.end(), outgoing);
    if (leftHand) {
        NBContHelper::nextCW(myAllEdges, i);
    } else {
        NBContHelper::nextCCW(myAllEdges, i);
    }
    while ((*i) != incoming) {
        if ((*i)->getFromNode() == this && !incoming->isTurningDirectionAt(*i)) {
            //std::cout << incoming->getID() << " -> " << outgoing->getID() << " partLeft because auf " << (*i)->getID() << "\n";
            return LINKDIR_PARTLEFT;
        }
        if (leftHand) {
            NBContHelper::nextCW(myAllEdges, i);
        } else {
            NBContHelper::nextCCW(myAllEdges, i);
        }
    }
    return LINKDIR_LEFT;
}


LinkState
NBNode::getLinkState(const NBEdge* incoming, NBEdge* outgoing, int fromlane, int toLane,
                     bool mayDefinitelyPass, const std::string& tlID) const {
    if (myType == NODETYPE_RAIL_CROSSING && isRailway(incoming->getPermissions())) {
        return LINKSTATE_MAJOR; // the trains must run on time
    }
    if (tlID != "") {
        return LINKSTATE_TL_OFF_BLINKING;
    }
    if (outgoing == 0) { // always off
        return LINKSTATE_TL_OFF_NOSIGNAL;
    }
    if (myType == NODETYPE_RIGHT_BEFORE_LEFT) {
        return LINKSTATE_EQUAL; // all the same
    }
    if (myType == NODETYPE_ALLWAY_STOP) {
        return LINKSTATE_ALLWAY_STOP; // all drive, first one to arrive may drive first
    }
    if (myType == NODETYPE_ZIPPER && mustBrake(incoming, outgoing, fromlane, toLane, false)) {
        return LINKSTATE_ZIPPER;
    }
    if ((!incoming->isInnerEdge() && mustBrake(incoming, outgoing, fromlane, toLane, true)) && !mayDefinitelyPass) {
        return myType == NODETYPE_PRIORITY_STOP ? LINKSTATE_STOP : LINKSTATE_MINOR; // minor road
    }
    // traffic lights are not regarded here
    return LINKSTATE_MAJOR;
}


bool
NBNode::checkIsRemovable() const {
    // check whether this node is included in a traffic light or crossing
    if (myTrafficLights.size() != 0 || myCrossings.size() != 0) {
        return false;
    }
    EdgeVector::const_iterator i;
    // one in, one out -> just a geometry ...
    if (myOutgoingEdges.size() == 1 && myIncomingEdges.size() == 1) {
        // ... if types match ...
        if (!myIncomingEdges[0]->expandableBy(myOutgoingEdges[0])) {
            return false;
        }
        //
        return myIncomingEdges[0]->getTurnDestination(true) != myOutgoingEdges[0];
    }
    // two in, two out -> may be something else
    if (myOutgoingEdges.size() == 2 && myIncomingEdges.size() == 2) {
        // check whether the origin nodes of the incoming edges differ
        std::set<NBNode*> origSet;
        for (i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
            origSet.insert((*i)->getFromNode());
        }
        if (origSet.size() < 2) {
            return false;
        }
        // check whether this node is an intermediate node of
        //  a two-directional street
        for (i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
            // each of the edges must have an opposite direction edge
            NBEdge* opposite = (*i)->getTurnDestination(true);
            if (opposite != 0) {
                // the other outgoing edges must be the continuation of the current
                NBEdge* continuation = opposite == myOutgoingEdges.front() ? myOutgoingEdges.back() : myOutgoingEdges.front();
                // check whether the types allow joining
                if (!(*i)->expandableBy(continuation)) {
                    return false;
                }
            } else {
                // ok, at least one outgoing edge is not an opposite
                //  of an incoming one
                return false;
            }
        }
        return true;
    }
    // ok, a real node
    return false;
}


std::vector<std::pair<NBEdge*, NBEdge*> >
NBNode::getEdgesToJoin() const {
    assert(checkIsRemovable());
    std::vector<std::pair<NBEdge*, NBEdge*> > ret;
    // one in, one out-case
    if (myOutgoingEdges.size() == 1 && myIncomingEdges.size() == 1) {
        ret.push_back(
            std::pair<NBEdge*, NBEdge*>(
                myIncomingEdges[0], myOutgoingEdges[0]));
        return ret;
    }
    // two in, two out-case
    for (EdgeVector::const_iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
        // join with the edge that is not a turning direction
        NBEdge* opposite = (*i)->getTurnDestination(true);
        assert(opposite != 0);
        NBEdge* continuation = opposite == myOutgoingEdges.front() ? myOutgoingEdges.back() : myOutgoingEdges.front();
        ret.push_back(std::pair<NBEdge*, NBEdge*>(*i, continuation));
    }
    return ret;
}


const PositionVector&
NBNode::getShape() const {
    return myPoly;
}


void
NBNode::setCustomShape(const PositionVector& shape) {
    myPoly = shape;
    myHaveCustomPoly = (myPoly.size() > 1);
}


void
NBNode::setCustomLaneShape(const std::string& laneID, const PositionVector& shape) {
    if (shape.size() > 1) {
        myCustomLaneShapes[laneID] = shape;
    } else {
        myCustomLaneShapes.erase(laneID);
    }
}


NBEdge*
NBNode::getConnectionTo(NBNode* n) const {
    for (EdgeVector::const_iterator i = myOutgoingEdges.begin(); i != myOutgoingEdges.end(); i++) {
        if ((*i)->getToNode() == n) {
            return (*i);
        }
    }
    return 0;
}


bool
NBNode::isNearDistrict() const {
    if (isDistrict()) {
        return false;
    }
    EdgeVector edges;
    copy(getIncomingEdges().begin(), getIncomingEdges().end(),
         back_inserter(edges));
    copy(getOutgoingEdges().begin(), getOutgoingEdges().end(),
         back_inserter(edges));
    for (EdgeVector::const_iterator j = edges.begin(); j != edges.end(); ++j) {
        NBEdge* t = *j;
        NBNode* other = 0;
        if (t->getToNode() == this) {
            other = t->getFromNode();
        } else {
            other = t->getToNode();
        }
        EdgeVector edges2;
        copy(other->getIncomingEdges().begin(), other->getIncomingEdges().end(), back_inserter(edges2));
        copy(other->getOutgoingEdges().begin(), other->getOutgoingEdges().end(), back_inserter(edges2));
        for (EdgeVector::const_iterator k = edges2.begin(); k != edges2.end(); ++k) {
            if ((*k)->getFromNode()->isDistrict() || (*k)->getToNode()->isDistrict()) {
                return true;
            }
        }
    }
    return false;
}


bool
NBNode::isDistrict() const {
    return myType == NODETYPE_DISTRICT;
}


int
NBNode::guessCrossings() {
    //gDebugFlag1 = getID() == DEBUGID;
    int numGuessed = 0;
    if (myCrossings.size() > 0 || myDiscardAllCrossings) {
        // user supplied crossings, do not guess
        return numGuessed;
    }
    if (gDebugFlag1) {
        std::cout << "guess crossings for " << getID() << "\n";
    }
    EdgeVector allEdges = getEdgesSortedByAngleAtNodeCenter();
    // check for pedestrial lanes going clockwise around the node
    std::vector<std::pair<NBEdge*, bool> > normalizedLanes;
    for (EdgeVector::const_iterator it = allEdges.begin(); it != allEdges.end(); ++it) {
        NBEdge* edge = *it;
        const std::vector<NBEdge::Lane>& lanes = edge->getLanes();
        if (edge->getFromNode() == this) {
            for (std::vector<NBEdge::Lane>::const_reverse_iterator it_l = lanes.rbegin(); it_l != lanes.rend(); ++it_l) {
                normalizedLanes.push_back(std::make_pair(edge, ((*it_l).permissions & SVC_PEDESTRIAN) != 0));
            }
        } else {
            for (std::vector<NBEdge::Lane>::const_iterator it_l = lanes.begin(); it_l != lanes.end(); ++it_l) {
                normalizedLanes.push_back(std::make_pair(edge, ((*it_l).permissions & SVC_PEDESTRIAN) != 0));
            }
        }
    }
    // do we even have a pedestrian lane?
    int firstSidewalk = -1;
    for (int i = 0; i < (int)normalizedLanes.size(); ++i) {
        if (normalizedLanes[i].second) {
            firstSidewalk = i;
            break;
        }
    }
    int hadCandidates = 0;
    std::vector<int> connectedCandidates; // number of crossings that were built for each connected candidate
    if (firstSidewalk != -1) {
        // rotate lanes to ensure that the first one allows pedestrians
        std::vector<std::pair<NBEdge*, bool> > tmp;
        copy(normalizedLanes.begin() + firstSidewalk, normalizedLanes.end(), std::back_inserter(tmp));
        copy(normalizedLanes.begin(), normalizedLanes.begin() + firstSidewalk, std::back_inserter(tmp));
        normalizedLanes = tmp;
        // find candidates
        EdgeVector candidates;
        for (int i = 0; i < (int)normalizedLanes.size(); ++i) {
            NBEdge* edge = normalizedLanes[i].first;
            const bool allowsPed = normalizedLanes[i].second;
            if (gDebugFlag1) {
                std::cout << "  cands=" << toString(candidates) << "  edge=" << edge->getID() << " allowsPed=" << allowsPed << "\n";
            }
            if (!allowsPed && (candidates.size() == 0 || candidates.back() != edge)) {
                candidates.push_back(edge);
            } else if (allowsPed) {
                if (candidates.size() > 0) {
                    if (hadCandidates > 0 || forbidsPedestriansAfter(normalizedLanes, i)) {
                        hadCandidates++;
                        const int n = checkCrossing(candidates);
                        numGuessed += n;
                        if (n > 0) {
                            connectedCandidates.push_back(n);
                        }
                    }
                    candidates.clear();
                }
            }
        }
        if (hadCandidates > 0 && candidates.size() > 0) {
            // avoid wrapping around to the same sidewalk
            hadCandidates++;
            const int n = checkCrossing(candidates);
            numGuessed += n;
            if (n > 0) {
                connectedCandidates.push_back(n);
            }
        }
    }
    // Avoid duplicate crossing between the same pair of walkingareas
    if (gDebugFlag1) {
        std::cout << "  hadCandidates=" << hadCandidates << "  connectedCandidates=" << toString(connectedCandidates) << "\n";
    }
    if (hadCandidates == 2 && connectedCandidates.size() == 2) {
        // One or both of them might be split: remove the one with less splits
        if (connectedCandidates.back() <= connectedCandidates.front()) {
            numGuessed -= connectedCandidates.back();
            myCrossings.erase(myCrossings.end() - connectedCandidates.back(), myCrossings.end());
        } else {
            numGuessed -= connectedCandidates.front();
            myCrossings.erase(myCrossings.begin(), myCrossings.begin() + connectedCandidates.front());
        }
    }
    std::sort(myCrossings.begin(), myCrossings.end(), NBNodesEdgesSorter::crossing_by_junction_angle_sorter(this, myAllEdges));
    if (gDebugFlag1) {
        std::cout << "guessedCrossings:\n";
        for (std::vector<Crossing>::iterator it = myCrossings.begin(); it != myCrossings.end(); it++) {
            std::cout << "  edges=" << toString((*it).edges) << "\n";
        }
    }
    return numGuessed;
}


int
NBNode::checkCrossing(EdgeVector candidates) {
    if (gDebugFlag1) {
        std::cout << "checkCrossing candidates=" << toString(candidates) << "\n";
    }
    if (candidates.size() == 0) {
        if (gDebugFlag1) {
            std::cout << "no crossing added (numCandidates=" << candidates.size() << ")\n";
        }
        return 0;
    } else {
        // check whether the edges may be part of a common crossing due to having similar angle
        SUMOReal prevAngle = -100000; // dummy
        for (int i = 0; i < (int)candidates.size(); ++i) {
            NBEdge* edge = candidates[i];
            SUMOReal angle = edge->getCrossingAngle(this);
            // edges should be sorted by angle but this only holds true approximately
            if (i > 0 && fabs(angle - prevAngle) > EXTEND_CROSSING_ANGLE_THRESHOLD) {
                if (gDebugFlag1) {
                    std::cout << "no crossing added (found angle difference of " << fabs(angle - prevAngle) << " at i=" << i << "\n";
                }
                return 0;
            }
            if (!isTLControlled() && myType != NODETYPE_RAIL_CROSSING && edge->getSpeed() > OptionsCont::getOptions().getFloat("crossings.guess.speed-threshold")) {
                if (gDebugFlag1) {
                    std::cout << "no crossing added (uncontrolled, edge with speed > " << edge->getSpeed() << ")\n";
                }
                return 0;
            }
            prevAngle = angle;
        }
        if (candidates.size() == 1) {
            addCrossing(candidates, DEFAULT_CROSSING_WIDTH, isTLControlled());
            if (gDebugFlag1) {
                std::cout << "adding crossing: " << toString(candidates) << "\n";
            }
            return 1;
        } else {
            // check for intermediate walking areas
            SUMOReal prevAngle = -100000; // dummy
            for (EdgeVector::iterator it = candidates.begin(); it != candidates.end(); ++it) {
                SUMOReal angle = (*it)->getCrossingAngle(this);
                if (it != candidates.begin()) {
                    NBEdge* prev = *(it - 1);
                    NBEdge* curr = *it;
                    Position prevPos, currPos;
                    int laneI;
                    // compute distance between candiate edges
                    SUMOReal intermediateWidth = 0;
                    if (prev->getToNode() == this) {
                        laneI = prev->getNumLanes() - 1;
                        prevPos = prev->getLanes()[laneI].shape[-1];
                    } else {
                        laneI = 0;
                        prevPos = prev->getLanes()[laneI].shape[0];
                    }
                    intermediateWidth -= 0.5 * prev->getLaneWidth(laneI);
                    if (curr->getFromNode() == this) {
                        laneI = curr->getNumLanes() - 1;
                        currPos = curr->getLanes()[laneI].shape[0];
                    } else {
                        laneI = 0;
                        currPos = curr->getLanes()[laneI].shape[-1];
                    }
                    intermediateWidth -= 0.5 * curr->getLaneWidth(laneI);
                    intermediateWidth += currPos.distanceTo2D(prevPos);
                    if (gDebugFlag1) {
                        std::cout
                                << " prevAngle=" << prevAngle
                                << " angle=" << angle
                                << " intermediateWidth=" << intermediateWidth
                                << "\n";
                    }
                    if (fabs(prevAngle - angle) > SPLIT_CROSSING_ANGLE_THRESHOLD
                            || (intermediateWidth > SPLIT_CROSSING_WIDTH_THRESHOLD)) {
                        return checkCrossing(EdgeVector(candidates.begin(), it))
                               + checkCrossing(EdgeVector(it, candidates.end()));
                    }
                }
                prevAngle = angle;
            }
            addCrossing(candidates, DEFAULT_CROSSING_WIDTH, isTLControlled());
            if (gDebugFlag1) {
                std::cout << "adding crossing: " << toString(candidates) << "\n";
            }
            return 1;
        }
    }
}


bool
NBNode::checkCrossingDuplicated(EdgeVector edges) {
    // sort edge vector
    std::sort(edges.begin(), edges.end());
    // iterate over crossing to find a crossing with the same edges
    for (std::vector<Crossing>::iterator it = myCrossings.begin(); it != myCrossings.end(); it++) {
        // sort edges of crossing before compare
        EdgeVector edgesOfCrossing = it->edges;
        std::sort(edgesOfCrossing.begin(), edgesOfCrossing.end());
        if (edgesOfCrossing == edges) {
            return true;
        }
    }
    return false;
}


bool
NBNode::forbidsPedestriansAfter(std::vector<std::pair<NBEdge*, bool> > normalizedLanes, int startIndex) {
    for (int i = startIndex; i < (int)normalizedLanes.size(); ++i) {
        if (!normalizedLanes[i].second) {
            return true;
        }
    }
    return false;
}


void
NBNode::buildCrossingsAndWalkingAreas(bool discardInvalid) {
    buildCrossings();
    buildWalkingAreas(OptionsCont::getOptions().getInt("junctions.corner-detail"));
    // ensure that all crossings are properly connected
    if (discardInvalid) {
        for (std::vector<Crossing>::iterator it = myCrossings.begin(); it != myCrossings.end();) {
            if ((*it).prevWalkingArea == "" || (*it).nextWalkingArea == "") {
                WRITE_WARNING("Discarding invalid crossing '" + (*it).id + "' at junction '" + getID() + "' with edges '" + toString((*it).edges) + "'.");
                for (std::vector<WalkingArea>::iterator it_wa = myWalkingAreas.begin(); it_wa != myWalkingAreas.end(); it_wa++) {
                    if ((*it_wa).nextCrossing == (*it).id) {
                        (*it_wa).nextCrossing = "";
                    }
                }
                it = myCrossings.erase(it);
            } else {
                ++it;
            }
        }
    }
}


void
NBNode::buildInnerEdges() {
    // myDisplacementError is computed during this operation. reset first
    myDisplacementError = 0;
    // build inner edges for vehicle movements across the junction
    int noInternalNoSplits = 0;
    for (EdgeVector::const_iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
        const std::vector<NBEdge::Connection>& elv = (*i)->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
            if ((*k).toEdge == 0) {
                continue;
            }
            noInternalNoSplits++;
        }
    }
    int lno = 0;
    int splitNo = 0;
    for (EdgeVector::const_iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
        (*i)->buildInnerEdges(*this, noInternalNoSplits, lno, splitNo);
    }
    // if there are custom lane shapes we need to built twice:
    // first to set the ids then to build intersections with the custom geometries
    if (myCustomLaneShapes.size() > 0) {
        int lno = 0;
        int splitNo = 0;
        for (EdgeVector::const_iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
            (*i)->buildInnerEdges(*this, noInternalNoSplits, lno, splitNo);
        }
    }
}


int
NBNode::buildCrossings() {
    //gDebugFlag1 = getID() == DEBUGID;
    if (gDebugFlag1) {
        std::cout << "build crossings for " << getID() << ":\n";
    }
    if (myDiscardAllCrossings) {
        myCrossings.clear();
    }
    int index = 0;
    for (std::vector<Crossing>::iterator it = myCrossings.begin(); it != myCrossings.end();) {
        (*it).id = ":" + getID() + "_c" + toString(index++);
        // reset fields, so repeated computation (Netedit) will sucessfully perform the checks
        // in buildWalkingAreas (split crossings) and buildInnerEdges (sanity check)
        (*it).nextWalkingArea = "";
        (*it).prevWalkingArea = "";
        EdgeVector& edges = (*it).edges;
        if (gDebugFlag1) {
            std::cout << "  crossing=" << (*it).id << " edges=" << toString(edges);
        }
        // sorting the edges in the right way is imperative. We want to sort
        // them by getAngleAtNodeToCenter() but need to be extra carefull to avoid wrapping around 0 somewhere in between
        std::sort(edges.begin(), edges.end(), NBContHelper::edge_by_angle_to_nodeShapeCentroid_sorter(this));
        if (gDebugFlag1) {
            std::cout << " sortedEdges=" << toString(edges) << "\n";
        };
        // rotate the edges so that the largest relative angle difference comes at the end
        SUMOReal maxAngleDiff = 0;
        int maxAngleDiffIndex = 0; // index before maxDist
        for (int i = 0; i < (int) edges.size(); i++) {
            SUMOReal diff = NBHelpers::relAngle(edges[i]->getAngleAtNodeToCenter(this),
                                                edges[(i + 1) % edges.size()]->getAngleAtNodeToCenter(this));
            if (diff < 0) {
                diff += 360;
            }
            if (gDebugFlag1) {
                std::cout << "   i=" << i << " a1=" << edges[i]->getAngleAtNodeToCenter(this) << " a2=" << edges[(i + 1) % edges.size()]->getAngleAtNodeToCenter(this) << " diff=" << diff << "\n";
            }
            if (diff > maxAngleDiff) {
                maxAngleDiff = diff;
                maxAngleDiffIndex = i;
            }
        }
        if (maxAngleDiff > 2 && maxAngleDiff < 360 - 2) {
            // if the angle differences is too small, we better not rotate
            std::rotate(edges.begin(), edges.begin() + (maxAngleDiffIndex + 1) % edges.size(), edges.end());
            if (gDebugFlag1) {
                std::cout << " rotatedEdges=" << toString(edges);
            }
        }
        // reverse to get them in CCW order (walking direction around the node)
        std::reverse(edges.begin(), edges.end());
        if (gDebugFlag1) {
            std::cout << " finalEdges=" << toString(edges) << "\n";
        }
        // compute shape
        (*it).shape.clear();
        const int begDir = (edges.front()->getFromNode() == this ? FORWARD : BACKWARD);
        const int endDir = (edges.back()->getToNode() == this ? FORWARD : BACKWARD);
        if (edges.front()->getFirstNonPedestrianLaneIndex(begDir) < 0
                || edges.back()->getFirstNonPedestrianLaneIndex(endDir) < 0) {
            // invalid crossing
            WRITE_WARNING("Discarding invalid crossing '" + (*it).id + "' at junction '" + getID() + "' with edges '" + toString((*it).edges) + "'.");
            it = myCrossings.erase(it);
        } else {
            NBEdge::Lane crossingBeg = edges.front()->getFirstNonPedestrianLane(begDir);
            NBEdge::Lane crossingEnd = edges.back()->getFirstNonPedestrianLane(endDir);
            crossingBeg.width = (crossingBeg.width == NBEdge::UNSPECIFIED_WIDTH ? SUMO_const_laneWidth : crossingBeg.width);
            crossingEnd.width = (crossingEnd.width == NBEdge::UNSPECIFIED_WIDTH ? SUMO_const_laneWidth : crossingEnd.width);
            crossingBeg.shape.move2side(begDir * crossingBeg.width / 2);
            crossingEnd.shape.move2side(endDir * crossingEnd.width / 2);
            crossingBeg.shape.extrapolate((*it).width / 2);
            crossingEnd.shape.extrapolate((*it).width / 2);
            (*it).shape.push_back(crossingBeg.shape[begDir == FORWARD ? 0 : -1]);
            (*it).shape.push_back(crossingEnd.shape[endDir == FORWARD ? -1 : 0]);
            ++it;
        }
    }
    return index;
}


void
NBNode::buildWalkingAreas(int cornerDetail) {
    //gDebugFlag1 = getID() == DEBUGID;
    int index = 0;
    myWalkingAreas.clear();
    if (gDebugFlag1) {
        std::cout << "build walkingAreas for " << getID() << ":\n";
    }
    if (myAllEdges.size() == 0) {
        return;
    }
    EdgeVector allEdges = getEdgesSortedByAngleAtNodeCenter();
    // shapes are all pointing away from the intersection
    std::vector<std::pair<NBEdge*, NBEdge::Lane> > normalizedLanes;
    for (EdgeVector::const_iterator it = allEdges.begin(); it != allEdges.end(); ++it) {
        NBEdge* edge = *it;
        const std::vector<NBEdge::Lane>& lanes = edge->getLanes();
        if (edge->getFromNode() == this) {
            for (std::vector<NBEdge::Lane>::const_reverse_iterator it_l = lanes.rbegin(); it_l != lanes.rend(); ++it_l) {
                NBEdge::Lane l = *it_l;
                l.shape = l.shape.getSubpartByIndex(0, 2);
                l.width = (l.width == NBEdge::UNSPECIFIED_WIDTH ? SUMO_const_laneWidth : l.width);
                normalizedLanes.push_back(std::make_pair(edge, l));
            }
        } else {
            for (std::vector<NBEdge::Lane>::const_iterator it_l = lanes.begin(); it_l != lanes.end(); ++it_l) {
                NBEdge::Lane l = *it_l;
                l.shape = l.shape.reverse();
                l.shape = l.shape.getSubpartByIndex(0, 2);
                l.width = (l.width == NBEdge::UNSPECIFIED_WIDTH ? SUMO_const_laneWidth : l.width);
                normalizedLanes.push_back(std::make_pair(edge, l));
            }
        }
    }
    //if (gDebugFlag1) std::cout << "  normalizedLanes=" << normalizedLanes.size() << "\n";
    // collect [start,count[ indices in normalizedLanes that belong to a walkingArea
    std::vector<std::pair<int, int> > waIndices;
    int start = -1;
    NBEdge* prevEdge = normalizedLanes.back().first;
    for (int i = 0; i < (int)normalizedLanes.size(); ++i) {
        NBEdge* edge = normalizedLanes[i].first;
        NBEdge::Lane& l = normalizedLanes[i].second;
        if (start == -1) {
            if ((l.permissions & SVC_PEDESTRIAN) != 0) {
                start = i;
            }
        } else {
            if ((l.permissions & SVC_PEDESTRIAN) == 0 || crossingBetween(edge, prevEdge)) {
                waIndices.push_back(std::make_pair(start, i - start));
                if ((l.permissions & SVC_PEDESTRIAN) != 0) {
                    start = i;
                } else {
                    start = -1;
                }

            }
        }
        if (gDebugFlag1) std::cout << "     i=" << i << " edge=" << edge->getID() << " start=" << start << " ped=" << ((l.permissions & SVC_PEDESTRIAN) != 0)
                                       << " waI=" << waIndices.size() << " crossingBetween=" << crossingBetween(edge, prevEdge) << "\n";
        prevEdge = edge;
    }
    // deal with wrap-around issues
    if (start != - 1) {
        const int waNumLanes = (int)normalizedLanes.size() - start;
        if (waIndices.size() == 0) {
            waIndices.push_back(std::make_pair(start, waNumLanes));
            if (gDebugFlag1) {
                std::cout << "  single wa, end at wrap-around\n";
            }
        } else {
            if (waIndices.front().first == 0) {
                NBEdge* edge = normalizedLanes.front().first;
                NBEdge* prevEdge = normalizedLanes.back().first;
                if (crossingBetween(edge, prevEdge)) {
                    // do not wrap-around if there is a crossing in between
                    waIndices.push_back(std::make_pair(start, waNumLanes));
                    if (gDebugFlag1) {
                        std::cout << "  do not wrap around, turn-around in between\n";
                    }
                } else {
                    // first walkingArea wraps around
                    waIndices.front().first = start;
                    waIndices.front().second = waNumLanes + waIndices.front().second;
                    if (gDebugFlag1) {
                        std::cout << "  wrapping around\n";
                    }
                }
            } else {
                // last walkingArea ends at the wrap-around
                waIndices.push_back(std::make_pair(start, waNumLanes));
                if (gDebugFlag1) {
                    std::cout << "  end at wrap-around\n";
                }
            }
        }
    }
    if (gDebugFlag1) {
        std::cout << "  normalizedLanes=" << normalizedLanes.size() << " waIndices:\n";
        for (int i = 0; i < (int)waIndices.size(); ++i) {
            std::cout << "   " << waIndices[i].first << ", " << waIndices[i].second << "\n";
        }
    }
    // build walking areas connected to a sidewalk
    for (int i = 0; i < (int)waIndices.size(); ++i) {
        const bool buildExtensions = waIndices[i].second != (int)normalizedLanes.size();
        const int start = waIndices[i].first;
        const int prev = start > 0 ? start - 1 : (int)normalizedLanes.size() - 1;
        const int count = waIndices[i].second;
        const int end = (start + count) % normalizedLanes.size();

        WalkingArea wa(":" + getID() + "_w" + toString(index++), 1);
        if (gDebugFlag1) {
            std::cout << "build walkingArea " << wa.id << " start=" << start << " end=" << end << " count=" << count << " prev=" << prev << ":\n";
        }
        SUMOReal endCrossingWidth = 0;
        SUMOReal startCrossingWidth = 0;
        PositionVector endCrossingShape;
        PositionVector startCrossingShape;
        // check for connected crossings
        bool connectsCrossing = false;
        std::vector<Position> connectedPoints;
        for (std::vector<Crossing>::iterator it = myCrossings.begin(); it != myCrossings.end(); ++it) {
            if (gDebugFlag1) {
                std::cout << "  crossing=" << (*it).id << " sortedEdges=" << toString((*it).edges) << "\n";
            }
            if ((*it).edges.back() == normalizedLanes[end].first
                    && (normalizedLanes[end].second.permissions & SVC_PEDESTRIAN) == 0) {
                // crossing ends
                if ((*it).nextWalkingArea != "") {
                    WRITE_WARNING("Invalid pedestrian topology at junction '" + getID()
                                  + "'; crossing '" + (*it).id
                                  + "' targets '" + (*it).nextWalkingArea
                                  + "' and '" + wa.id + "'.");
                }
                (*it).nextWalkingArea = wa.id;
                endCrossingWidth = (*it).width;
                endCrossingShape = (*it).shape;
                wa.width = MAX2(wa.width, endCrossingWidth);
                connectsCrossing = true;
                connectedPoints.push_back((*it).shape[-1]);
                if (gDebugFlag1) {
                    std::cout << "    crossing " << (*it).id << " ends\n";
                }
            }
            if ((*it).edges.front() == normalizedLanes[prev].first
                    && (normalizedLanes[prev].second.permissions & SVC_PEDESTRIAN) == 0) {
                // crossing starts
                if ((*it).prevWalkingArea != "") {
                    WRITE_WARNING("Invalid pedestrian topology at junction '" + getID()
                                  + "'; crossing '" + (*it).id
                                  + "' is targeted by '" + (*it).prevWalkingArea
                                  + "' and '" + wa.id + "'.");
                }
                (*it).prevWalkingArea = wa.id;
                wa.nextCrossing = (*it).id;
                startCrossingWidth = (*it).width;
                startCrossingShape = (*it).shape;
                wa.width = MAX2(wa.width, startCrossingWidth);
                connectsCrossing = true;
                connectedPoints.push_back((*it).shape[0]);
                if (gDebugFlag1) {
                    std::cout << "    crossing " << (*it).id << " starts\n";
                }
            }
            if (gDebugFlag1) std::cout << "  check connections to crossing " << (*it).id
                                           << " cFront=" << (*it).edges.front()->getID() << " cBack=" << (*it).edges.back()->getID()
                                           << " wEnd=" << normalizedLanes[end].first->getID() << " wStart=" << normalizedLanes[start].first->getID()
                                           << " wStartPrev=" << normalizedLanes[prev].first->getID()
                                           << "\n";
        }
        if (count < 2 && !connectsCrossing) {
            // not relevant for walking
            if (gDebugFlag1) {
                std::cout << "    not relevant for walking: count=" << count << " connectsCrossing=" << connectsCrossing << "\n";
            }
            continue;
        }
        // build shape and connections
        std::set<NBEdge*> connected;
        for (int j = 0; j < count; ++j) {
            const int nlI = (start + j) % normalizedLanes.size();
            NBEdge* edge = normalizedLanes[nlI].first;
            NBEdge::Lane l = normalizedLanes[nlI].second;
            wa.width = MAX2(wa.width, l.width);
            if (connected.count(edge) == 0) {
                if (edge->getFromNode() == this) {
                    wa.nextSidewalks.push_back(edge->getID());
                    connectedPoints.push_back(edge->getLaneShape(0)[0]);
                } else {
                    wa.prevSidewalks.push_back(edge->getID());
                    connectedPoints.push_back(edge->getLaneShape(0)[-1]);
                }
                connected.insert(edge);
            }
            l.shape.move2side(-l.width / 2);
            wa.shape.push_back(l.shape[0]);
            l.shape.move2side(l.width);
            wa.shape.push_back(l.shape[0]);
        }
        if (buildExtensions) {
            // extension at starting crossing
            if (startCrossingShape.size() > 0) {
                if (gDebugFlag1) {
                    std::cout << "  extension at startCrossing shape=" << startCrossingShape << "\n";
                }
                startCrossingShape.move2side(startCrossingWidth / 2);
                wa.shape.push_front_noDoublePos(startCrossingShape[0]); // right corner
                startCrossingShape.move2side(-startCrossingWidth);
                wa.shape.push_front_noDoublePos(startCrossingShape[0]); // left corner goes first
            }
            // extension at ending crossing
            if (endCrossingShape.size() > 0) {
                if (gDebugFlag1) {
                    std::cout << "  extension at endCrossing shape=" << endCrossingShape << "\n";
                }
                endCrossingShape.move2side(endCrossingWidth / 2);
                wa.shape.push_back_noDoublePos(endCrossingShape[-1]);
                endCrossingShape.move2side(-endCrossingWidth);
                wa.shape.push_back_noDoublePos(endCrossingShape[-1]);
            }
        }
        if (connected.size() == 2 && !connectsCrossing && wa.nextSidewalks.size() == 1 && wa.prevSidewalks.size() == 1
                && normalizedLanes.size() == 2) {
            // do not build a walkingArea since a normal connection exists
            NBEdge* e1 = *connected.begin();
            NBEdge* e2 = *(++connected.begin());
            if (e1->hasConnectionTo(e2, 0, 0) || e2->hasConnectionTo(e1, 0, 0)) {
                if (gDebugFlag1) {
                    std::cout << "    not building a walkingarea since normal connections exist\n";
                }
                continue;
            }
        }
        // build smooth inner curve (optional)
        if (cornerDetail > 0) {
            int smoothEnd = end;
            int smoothPrev = prev;
            // extend to green verge
            if (endCrossingWidth > 0 && normalizedLanes[smoothEnd].second.permissions == 0) {
                smoothEnd = (smoothEnd + 1) % normalizedLanes.size();
            }
            if (startCrossingWidth > 0 && normalizedLanes[smoothPrev].second.permissions == 0) {
                if (smoothPrev == 0) {
                    smoothPrev = (int)normalizedLanes.size() - 1;
                } else {
                    smoothPrev--;
                }
            }
            PositionVector begShape = normalizedLanes[smoothEnd].second.shape;
            begShape = begShape.reverse();
            //begShape.extrapolate(endCrossingWidth);
            begShape.move2side(normalizedLanes[smoothEnd].second.width / 2);
            PositionVector endShape = normalizedLanes[smoothPrev].second.shape;
            endShape.move2side(normalizedLanes[smoothPrev].second.width / 2);
            //endShape.extrapolate(startCrossingWidth);
            PositionVector curve = computeSmoothShape(begShape, endShape, cornerDetail + 2, false, 25, 25);
            if (gDebugFlag1) std::cout
                        << " end=" << smoothEnd << " prev=" << smoothPrev
                        << " endCrossingWidth=" << endCrossingWidth << " startCrossingWidth=" << startCrossingWidth
                        << "  begShape=" << begShape << " endShape=" << endShape << " smooth curve=" << curve << "\n";
            if (curve.size() > 2) {
                curve.erase(curve.begin());
                curve.pop_back();
                if (endCrossingWidth > 0) {
                    wa.shape.pop_back();
                }
                if (startCrossingWidth > 0) {
                    wa.shape.erase(wa.shape.begin());
                }
                wa.shape.append(curve, 0);
            }
        }
        // determine length (average of all possible connections)
        SUMOReal lengthSum = 0;
        int combinations = 0;
        for (std::vector<Position>::const_iterator it1 = connectedPoints.begin(); it1 != connectedPoints.end(); ++it1) {
            for (std::vector<Position>::const_iterator it2 = connectedPoints.begin(); it2 != connectedPoints.end(); ++it2) {
                const Position& p1 = *it1;
                const Position& p2 = *it2;
                if (p1 != p2) {
                    lengthSum += p1.distanceTo2D(p2);
                    combinations += 1;
                }
            }
        }
        if (gDebugFlag1) {
            std::cout << "  combinations=" << combinations << " connectedPoints=" << connectedPoints << "\n";
        }
        wa.length = POSITION_EPS;
        if (combinations > 0) {
            wa.length = MAX2(POSITION_EPS, lengthSum / combinations);
        }
        myWalkingAreas.push_back(wa);
    }
    // build walkingAreas between split crossings
    for (std::vector<Crossing>::iterator it = myCrossings.begin(); it != myCrossings.end(); ++it) {
        Crossing& prev = *it;
        Crossing& next = (it !=  myCrossings.begin() ? * (it - 1) : * (myCrossings.end() - 1));
        if (gDebugFlag1) {
            std::cout << "  checkIntermediate: prev=" << prev.id << " next=" << next.id << " prev.nextWA=" << prev.nextWalkingArea << "\n";
        }
        if (prev.nextWalkingArea == "") {
            if (next.prevWalkingArea != "") {
                WRITE_WARNING("Invalid pedestrian topology: crossing '" + prev.id + "' has no target.");
                continue;
            }
            WalkingArea wa(":" + getID() + "_w" + toString(index++), prev.width);
            prev.nextWalkingArea = wa.id;
            wa.nextCrossing = next.id;
            next.prevWalkingArea = wa.id;
            // back of previous crossing
            PositionVector tmp = prev.shape;
            tmp.move2side(-prev.width / 2);
            wa.shape.push_back(tmp[-1]);
            tmp.move2side(prev.width);
            wa.shape.push_back(tmp[-1]);
            // front of next crossing
            tmp = next.shape;
            tmp.move2side(prev.width / 2);
            wa.shape.push_back(tmp[0]);
            tmp.move2side(-prev.width);
            wa.shape.push_back(tmp[0]);
            // length (special case)
            wa.length = MAX2(POSITION_EPS, prev.shape.back().distanceTo2D(next.shape.front()));
            myWalkingAreas.push_back(wa);
            if (gDebugFlag1) {
                std::cout << "     build wa=" << wa.id << "\n";
            }
        }
    }
}


bool
NBNode::crossingBetween(const NBEdge* e1, const NBEdge* e2) const {
    if (e1 == e2) {
        return false;
    }
    for (std::vector<Crossing>::const_iterator it = myCrossings.begin(); it != myCrossings.end(); ++it) {
        const EdgeVector& edges = (*it).edges;
        EdgeVector::const_iterator it1 = find(edges.begin(), edges.end(), e1);
        EdgeVector::const_iterator it2 = find(edges.begin(), edges.end(), e2);
        if (it1 != edges.end() && it2 != edges.end()) {
            return true;
        }
    }
    return false;
}


EdgeVector
NBNode::edgesBetween(const NBEdge* e1, const NBEdge* e2) const {
    EdgeVector result;
    EdgeVector::const_iterator it = find(myAllEdges.begin(), myAllEdges.end(), e1);
    assert(it != myAllEdges.end());
    NBContHelper::nextCW(myAllEdges, it);
    EdgeVector::const_iterator it_end = find(myAllEdges.begin(), myAllEdges.end(), e2);
    assert(it_end != myAllEdges.end());
    while (it != it_end) {
        result.push_back(*it);
        NBContHelper::nextCW(myAllEdges, it);
    }
    return result;
}


bool
NBNode::geometryLike() const {
    if (myIncomingEdges.size() == 1 && myOutgoingEdges.size() == 1) {
        return true;
    }
    if (myIncomingEdges.size() == 2 && myOutgoingEdges.size() == 2) {
        // check whether the incoming and outgoing edges are pairwise (near) parallel and
        // thus the only cross-connections could be turn-arounds
        NBEdge* out0 = myOutgoingEdges[0];
        NBEdge* out1 = myOutgoingEdges[1];
        for (EdgeVector::const_iterator it = myIncomingEdges.begin(); it != myIncomingEdges.end(); ++it) {
            NBEdge* inEdge = *it;
            SUMOReal angle0 = fabs(NBHelpers::relAngle(inEdge->getAngleAtNode(this), out0->getAngleAtNode(this)));
            SUMOReal angle1 = fabs(NBHelpers::relAngle(inEdge->getAngleAtNode(this), out1->getAngleAtNode(this)));
            if (MAX2(angle0, angle1) <= 160) {
                // neither of the outgoing edges is parallel to inEdge
                return false;
            }
        }
        return true;
    }
    return false;
}


void
NBNode::setRoundabout() {
    if (myType == NODETYPE_RIGHT_BEFORE_LEFT) {
        myType = NODETYPE_PRIORITY;
    }
}


void
NBNode::addCrossing(EdgeVector edges, SUMOReal width, bool priority, bool fromSumoNet) {
    myCrossings.push_back(Crossing(this, edges, width, priority));
    if (fromSumoNet) {
        myCrossingsLoadedFromSumoNet += 1;
    }
}


void
NBNode::removeCrossing(const EdgeVector& edges) {
    EdgeSet edgeSet(edges.begin(), edges.end());
    for (std::vector<Crossing>::iterator it = myCrossings.begin(); it != myCrossings.end();) {
        EdgeSet edgeSet2((*it).edges.begin(), (*it).edges.end());
        if (edgeSet == edgeSet2) {
            it = myCrossings.erase(it);
        } else {
            ++it;
        }
    }
}


const NBNode::Crossing&
NBNode::getCrossing(const std::string& id) const {
    for (std::vector<Crossing>::const_iterator it = myCrossings.begin(); it != myCrossings.end(); ++it) {
        if ((*it).id == id) {
            return *it;
        }
    }
    throw ProcessError("Request for unknown crossing '" + id + "'");
}

NBNode::Crossing&
NBNode::getCrossingRef(const std::string& id) {
    for (std::vector<Crossing>::iterator it = myCrossings.begin(); it != myCrossings.end(); ++it) {
        if ((*it).id == id) {
            return *it;
        }
    }
    throw ProcessError("Request for unknown crossing '" + id + "'");
}


void
NBNode::setCrossingTLIndices(const std::string& tlID, int startIndex) {
    for (std::vector<Crossing>::iterator it = myCrossings.begin(); it != myCrossings.end(); ++it) {
        (*it).tlLinkNo = startIndex++;
        (*it).tlID = tlID;
    }
}


int
NBNode::numNormalConnections() const {
    return myRequest->getSizes().second;
}

Position
NBNode::getCenter() const {
    /* Conceptually, the center point would be identical with myPosition.
    * However, if the shape is influenced by custom geometry endpoints of the adjoining edges,
    * myPosition may fall outside the shape. In this case it is better to use
    * the center of the shape
    **/
    PositionVector tmp = myPoly;
    tmp.closePolygon();
    //std::cout << getID() << " around=" << tmp.around(myPosition) << " dist=" << tmp.distance2D(myPosition) << "\n";
    if (tmp.size() < 3 || tmp.around(myPosition) || tmp.distance2D(myPosition) < POSITION_EPS) {
        return myPosition;
    } else {
        return myPoly.getPolygonCenter();
    }
}


EdgeVector
NBNode::getEdgesSortedByAngleAtNodeCenter() const {
    EdgeVector result = myAllEdges;
    if (gDebugFlag1) {
        std::cout << "  angles:\n";
        for (EdgeVector::const_iterator it = result.begin(); it != result.end(); ++it) {
            std::cout << "    edge=" << (*it)->getID() << " edgeAngle=" << (*it)->getAngleAtNode(this) << " angleToShape=" << (*it)->getAngleAtNodeToCenter(this) << "\n";
        }
        std::cout << "  allEdges before: " << toString(result) << "\n";
    }
    sort(result.begin(), result.end(), NBContHelper::edge_by_angle_to_nodeShapeCentroid_sorter(this));
    // let the first edge in myAllEdges remain the first
    if (gDebugFlag1) {
        std::cout << "  allEdges sorted: " << toString(result) << "\n";
    }
    rotate(result.begin(), std::find(result.begin(), result.end(), *myAllEdges.begin()), result.end());
    if (gDebugFlag1) {
        std::cout << "  allEdges rotated: " << toString(result) << "\n";
    }
    return result;
}


std::string
NBNode::getNodeIDFromInternalLane(const std::string id) {
    // this relies on the fact that internal ids always have the form
    // :<nodeID>_<part1>_<part2>
    // i.e. :C_3_0, :C_c1_0 :C_w0_0
    assert(id[0] == ':');
    std::string::size_type sep_index = id.rfind('_');
    if (sep_index == std::string::npos) {
        WRITE_ERROR("Invalid lane id '" + id + "' (missing '_').");
        return "";
    }
    sep_index = id.substr(0, sep_index).rfind('_');
    if (sep_index == std::string::npos) {
        WRITE_ERROR("Invalid lane id '" + id + "' (missing '_').");
        return "";
    }
    return id.substr(1, sep_index - 1);
}


void
NBNode::avoidOverlap() {
    // simple case: edges with LANESPREAD_CENTER and a (possible) turndirection at the same node
    for (EdgeVector::iterator it = myIncomingEdges.begin(); it != myIncomingEdges.end(); it++) {
        NBEdge* edge = *it;
        NBEdge* turnDest = edge->getTurnDestination(true);
        if (turnDest != 0) {
            edge->shiftPositionAtNode(this, turnDest);
            turnDest->shiftPositionAtNode(this, edge);
        }
    }
    // @todo: edges in the same direction with sharp angles starting/ending at the same position
}


bool
NBNode::isTrafficLight(SumoXMLNodeType type) {
    return type == NODETYPE_TRAFFIC_LIGHT
           || type == NODETYPE_TRAFFIC_LIGHT_NOJUNCTION
           || type == NODETYPE_TRAFFIC_LIGHT_RIGHT_ON_RED;
}


bool
NBNode::rightOnRedConflict(int index, int foeIndex) const {
    if (myType == NODETYPE_TRAFFIC_LIGHT_RIGHT_ON_RED) {
        for (std::set<NBTrafficLightDefinition*>::const_iterator i = myTrafficLights.begin(); i != myTrafficLights.end(); ++i) {
            if ((*i)->rightOnRedConflict(index, foeIndex)) {
                return true;
            }
        }
    }
    return false;
}


void
NBNode::sortEdges(bool useNodeShape) {
    if (myAllEdges.size() == 0) {
        return;
    }
    EdgeVector& allEdges = myAllEdges;
    EdgeVector& incoming = myIncomingEdges;
    EdgeVector& outgoing = myOutgoingEdges;
    std::vector<NBNode::Crossing>& crossings = myCrossings;
    if (!useNodeShape || getShape().area() < 1) {
        // if the area is too small (i.e. for simple-continuation nodes) we better not use it
        // sort by the angle of the adjoining line segment of the edge geometry
        // sort the edges
        std::sort(allEdges.begin(), allEdges.end(), NBNodesEdgesSorter::edge_by_junction_angle_sorter(this));
        std::sort(incoming.begin(), incoming.end(), NBNodesEdgesSorter::edge_by_junction_angle_sorter(this));
        std::sort(outgoing.begin(), outgoing.end(), NBNodesEdgesSorter::edge_by_junction_angle_sorter(this));
        std::vector<NBEdge*>::iterator j;
        for (j = allEdges.begin(); j != allEdges.end() - 1 && j != allEdges.end(); ++j) {
            NBNodesEdgesSorter::swapWhenReversed(this, j, j + 1);
        }
        if (allEdges.size() > 1 && j != allEdges.end()) {
            NBNodesEdgesSorter::swapWhenReversed(this, allEdges.end() - 1, allEdges.begin());
        }
    } else {
        NBEdge* firstOfAll = allEdges.front();
        NBEdge* firstOfIncoming = incoming.size() > 0 ? incoming.front() : 0;
        NBEdge* firstOfOutgoing = outgoing.size() > 0 ? outgoing.front() : 0;
        // sort by the angle between the node shape center and the point where the edge meets the node shape
        sort(allEdges.begin(), allEdges.end(), NBContHelper::edge_by_angle_to_nodeShapeCentroid_sorter(this));
        sort(incoming.begin(), incoming.end(), NBContHelper::edge_by_angle_to_nodeShapeCentroid_sorter(this));
        sort(outgoing.begin(), outgoing.end(), NBContHelper::edge_by_angle_to_nodeShapeCentroid_sorter(this));
        // let the first edge remain the first
        rotate(allEdges.begin(), std::find(allEdges.begin(), allEdges.end(), firstOfAll), allEdges.end());
        if (firstOfIncoming != 0) {
            rotate(incoming.begin(), std::find(incoming.begin(), incoming.end(), firstOfIncoming), incoming.end());
        }
        if (firstOfOutgoing != 0) {
            rotate(outgoing.begin(), std::find(outgoing.begin(), outgoing.end(), firstOfOutgoing), outgoing.end());
        }
    }
    // fixing some pathological all edges orderings
    // if every of the edges a,b,c has a turning edge a',b',c' the all edges ordering should be a,a',b,b',c,c'
    if (incoming.size() == outgoing.size() && incoming.front() == allEdges.front()) {
        std::vector<NBEdge*>::const_iterator in, out;
        std::vector<NBEdge*> allTmp;
        for (in = incoming.begin(), out = outgoing.begin(); in != incoming.end(); ++in, ++out) {
            if ((*in)->isTurningDirectionAt(*out)) {
                allTmp.push_back(*in);
                allTmp.push_back(*out);
            } else {
                break;
            }
        }
        if (allTmp.size() == allEdges.size()) {
            allEdges = allTmp;
        }
    }
    // sort the crossings
    std::sort(crossings.begin(), crossings.end(), NBNodesEdgesSorter::crossing_by_junction_angle_sorter(this, allEdges));
    // DEBUG
    //if (getID() == "cluster_492462300_671564296") {
    //    if (crossings.size() > 0) {
    //        std::cout << " crossings at " << getID() << "\n";
    //        for (std::vector<NBNode::Crossing>::iterator it = crossings.begin(); it != crossings.end(); ++it) {
    //            std::cout << "  " << toString((*it).edges) << "\n";
    //        }
    //    }
    //}
}

/****************************************************************************/

