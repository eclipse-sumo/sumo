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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
#include <utils/geom/Line.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/bezier.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/iodevices/OutputDevice.h>
#include <iomanip>
#include "NBNode.h"
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


// ===========================================================================
// static members
// ===========================================================================

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
}


NBNode::ApproachingDivider::~ApproachingDivider() {}


void
NBNode::ApproachingDivider::execute(const unsigned int src, const unsigned int dest) {
    assert(myApproaching->size() > src);
    // get the origin edge
    NBEdge* incomingEdge = (*myApproaching)[src];
    if (incomingEdge->getStep() == NBEdge::LANES2LANES_DONE || incomingEdge->getStep() == NBEdge::LANES2LANES_USER) {
        return;
    }
    std::vector<int> approachingLanes =
        incomingEdge->getConnectionLanes(myCurrentOutgoing);
    assert(approachingLanes.size() != 0);
    std::deque<int>* approachedLanes = spread(approachingLanes, dest);
    assert(approachedLanes->size() <= myCurrentOutgoing->getNumLanes());
    // set lanes
    for (unsigned int i = 0; i < approachedLanes->size(); i++) {
        unsigned int approached = (*approachedLanes)[i];
        assert(approachedLanes->size() > i);
        assert(approachingLanes.size() > i);
        incomingEdge->setConnection((unsigned int) approachingLanes[i], myCurrentOutgoing,
                                    approached, NBEdge::L2L_COMPUTED);
    }
    delete approachedLanes;
}


std::deque<int>*
NBNode::ApproachingDivider::spread(const std::vector<int>& approachingLanes,
                                   int dest) const {
    std::deque<int>* ret = new std::deque<int>();
    unsigned int noLanes = (unsigned int) approachingLanes.size();
    // when only one lane is approached, we check, whether the SUMOReal-value
    //  is assigned more to the left or right lane
    if (noLanes == 1) {
        ret->push_back(dest);
        return ret;
    }

    unsigned int noOutgoingLanes = myCurrentOutgoing->getNumLanes();
    //
    ret->push_back(dest);
    unsigned int noSet = 1;
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
        if (dest + loffset >= static_cast<int>(noOutgoingLanes)) {
            loffset -= 1;
            roffset += 1;
            for (unsigned int i = 0; i < ret->size(); i++) {
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
                for (unsigned int i = 0; i < ret->size(); i++) {
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
    myType(type), myDistrict(0), myRequest(0)
{ }


NBNode::NBNode(const std::string& id, const Position& position, NBDistrict* district) :
    Named(StringUtils::convertUmlaute(id)),
    myPosition(position),
    myType(district == 0 ? NODETYPE_UNKNOWN : NODETYPE_DISTRICT), myDistrict(district), myRequest(0)
{ }


NBNode::~NBNode() {
    delete myRequest;
}


void
NBNode::reinit(const Position& position, SumoXMLNodeType type,
               bool updateEdgeGeometries) {
    myPosition = position;
    // patch type
    myType = type;
    if (myType != NODETYPE_TRAFFIC_LIGHT && myType != NODETYPE_TRAFFIC_LIGHT_NOJUNCTION) {
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


// -----------  Methods for dealing with assigned traffic lights
void
NBNode::addTrafficLight(NBTrafficLightDefinition* tlDef) {
    myTrafficLights.insert(tlDef);
    if (myType != NODETYPE_TRAFFIC_LIGHT_NOJUNCTION) {
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
                    nodes.front()->removeTrafficLight(orig);
                    newDef->addNode(nodes.front());
                }
                tlCont.removeFully(orig->getID());
                tlCont.insert(newDef);
            }
        }
    }
}


// ----------- Prunning the input
unsigned int
NBNode::removeSelfLoops(NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tc) {
    unsigned int ret = 0;
    unsigned int pos = 0;
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
        EdgeVector incomingConnected;
        for (EdgeVector::const_iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
            if ((*i)->isConnectedTo(dummy) && *i != dummy) {
                incomingConnected.push_back(*i);
            }
        }
        // get the list of outgoing edges connected to the self-loop
        EdgeVector outgoingConnected;
        for (EdgeVector::const_iterator i = myOutgoingEdges.begin(); i != myOutgoingEdges.end(); i++) {
            if (dummy->isConnectedTo(*i) && *i != dummy) {
                outgoingConnected.push_back(*i);
            }
        }
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
NBNode::isSimpleContinuation() const {
    // one in, one out->continuation
    if (myIncomingEdges.size() == 1 && myOutgoingEdges.size() == 1) {
        // both must have the same number of lanes
        return (*(myIncomingEdges.begin()))->getNumLanes() == (*(myOutgoingEdges.begin()))->getNumLanes();
    }
    // two in and two out and both in reverse direction
    if (myIncomingEdges.size() == 2 && myOutgoingEdges.size() == 2) {
        for (EdgeVector::const_iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
            NBEdge* in = *i;
            EdgeVector::const_iterator opposite = find_if(myOutgoingEdges.begin(), myOutgoingEdges.end(), NBContHelper::opposite_finder(in, this));
            // must have an opposite edge
            if (opposite == myOutgoingEdges.end()) {
                return false;
            }
            // both must have the same number of lanes
            NBContHelper::nextCW(myOutgoingEdges, opposite);
            if (in->getNumLanes() != (*opposite)->getNumLanes()) {
                return false;
            }
        }
        return true;
    }
    // nope
    return false;
}


PositionVector
NBNode::computeInternalLaneShape(NBEdge* fromE, int fromL,
                                 NBEdge* toE, int toL, int numPoints) const {
    if (fromL >= (int) fromE->getNumLanes()) {
        throw ProcessError("Connection '" + fromE->getID() + "_" + toString(fromL) + "->" + toE->getID() + "_" + toString(toL) + "' starts at a not existing lane.");
    }
    if (toL >= (int) toE->getNumLanes()) {
        throw ProcessError("Connection '" + fromE->getID() + "_" + toString(fromL) + "->" + toE->getID() + "_" + toString(toL) + "' yields in a not existing lane.");
    }
    bool noSpline = false;
    PositionVector ret;
    PositionVector init;
    Position beg = fromE->getLaneShape(fromL).back();
    Position end = toE->getLaneShape(toL).front();
    Position intersection;
    unsigned int noInitialPoints = 0;
    if (beg.distanceTo(end) <= POSITION_EPS) {
        noSpline = true;
    } else {
        if (fromE->getTurnDestination() == toE) {
            // turnarounds:
            //  - end of incoming lane
            //  - position between incoming/outgoing end/begin shifted by the distance orthogonally
            //  - begin of outgoing lane
            noInitialPoints = 3;
            init.push_back(beg);
            Line straightConn(fromE->getLaneShape(fromL)[-1], toE->getLaneShape(toL)[0]);
            Position straightCenter = straightConn.getPositionAtDistance((SUMOReal) straightConn.length() / (SUMOReal) 2.);
            Position center = straightCenter;//.add(straightCenter);
            Line cross(straightConn);
            cross.sub(cross.p1().x(), cross.p1().y());
            cross.rotateAtP1(M_PI / 2);
            center.sub(cross.p2());
            init.push_back(center);
            init.push_back(end);
        } else {
            const SUMOReal angle = fabs(fromE->getLaneShape(fromL).getEndLine().atan2Angle() - toE->getLaneShape(toL).getBegLine().atan2Angle());
            if (angle < M_PI / 4. || angle > 7. / 4.*M_PI) {
                // very low angle: almost straight
                noInitialPoints = 4;
                init.push_back(beg);
                Line begL = fromE->getLaneShape(fromL).getEndLine();
                begL.extrapolateSecondBy(100);
                Line endL = toE->getLaneShape(toL).getBegLine();
                endL.extrapolateFirstBy(100);
                SUMOReal distance = beg.distanceTo(end);
                if (distance > 10) {
                    {
                        SUMOReal off1 = fromE->getLaneShape(fromL).getEndLine().length() + (SUMOReal) 5. * (SUMOReal) fromE->getNumLanes();
                        off1 = MIN2(off1, (SUMOReal)(fromE->getLaneShape(fromL).getEndLine().length() + distance / 2.));
                        Position tmp = begL.getPositionAtDistance(off1);
                        init.push_back(tmp);
                    }
                    {
                        SUMOReal off1 = (SUMOReal) 100. - (SUMOReal) 5. * (SUMOReal) toE->getNumLanes();
                        off1 = MAX2(off1, (SUMOReal)(100. - distance / 2.));
                        Position tmp = endL.getPositionAtDistance(off1);
                        init.push_back(tmp);
                    }
                } else {
                    noSpline = true;
                }
                init.push_back(end);
            } else {
                // turning
                //  - end of incoming lane
                //  - intersection of the extrapolated lanes
                //  - begin of outgoing lane
                // attention: if there is no intersection, use a straight line
                noInitialPoints = 3;
                init.push_back(beg);
                Line begL = fromE->getLaneShape(fromL).getEndLine();
                Line endL = toE->getLaneShape(toL).getBegLine();
                bool check = !begL.p1().almostSame(begL.p2()) && !endL.p1().almostSame(endL.p2());
                if (check) {
                    begL.extrapolateSecondBy(100);
                    endL.extrapolateFirstBy(100);
                } else {
                    WRITE_WARNING("Could not use edge geometry for internal lane, node '" + getID() + "'.");
                }
                if (!check || !begL.intersects(endL)) {
                    noSpline = true;
                } else {
                    init.push_back(begL.intersectsAt(endL));
                }
                init.push_back(end);
            }
        }
    }
    //
    if (noSpline) {
        ret.push_back(fromE->getLaneShape(fromL).back());
        ret.push_back(toE->getLaneShape(toL).front());
    } else {
        SUMOReal* def = new SUMOReal[1 + noInitialPoints * 3];
        for (int i = 0; i < (int) init.size(); ++i) {
            // starts at index 1
            def[i * 3 + 1] = init[i].x();
            def[i * 3 + 2] = 0;
            def[i * 3 + 3] = init[i].y();
        }
        SUMOReal* ret_buf = new SUMOReal[numPoints * 3 + 1];
        bezier(noInitialPoints, def, numPoints, ret_buf);
        delete[] def;
        Position prev;
        for (int i = 0; i < (int) numPoints; i++) {
            Position current(ret_buf[i * 3 + 1], ret_buf[i * 3 + 3]);
            if (prev != current) {
                ret.push_back(current);
            }
            prev = current;
        }
        delete[] ret_buf;
    }
    const NBEdge::Lane& lane = fromE->getLaneStruct(fromL);
    if (lane.offset > 0) {
        PositionVector beg = lane.shape.getSubpart(lane.shape.length() - lane.offset, lane.shape.length());;
        beg.append(ret);
        ret = beg;
    }
    return ret;
}


bool
NBNode::needsCont(NBEdge* fromE, NBEdge* toE, NBEdge* otherFromE, NBEdge* otherToE, const NBEdge::Connection& c) const {
    if (myType == NODETYPE_RIGHT_BEFORE_LEFT) {
        return false;
    }
    if (fromE == otherFromE) {
        // ignore same edge links
        return false;
    }
    if (!foes(otherFromE, otherToE, fromE, toE)) {
        // if they do not cross, no waiting place is needed
        return false;
    }
    LinkDirection d1 = getDirection(fromE, toE);
    LinkDirection d2 = getDirection(otherFromE, otherToE);
    bool thisLeft = (d1 == LINKDIR_LEFT || d1 == LINKDIR_TURN);
    bool otherLeft = (d2 == LINKDIR_LEFT || d2 == LINKDIR_TURN);
    bool bothLeft = thisLeft && otherLeft;
    if (c.tlID != "" && !bothLeft) {
        // tls-controlled links will have space
        return true;
    }
    if (fromE->getJunctionPriority(this) > 0 && otherFromE->getJunctionPriority(this) > 0) {
        return mustBrake(fromE, toE, c.toLane);
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
        myRequest = new NBRequest(ec, this,
                                  myAllEdges, myIncomingEdges, myOutgoingEdges, myBlockedConnections);
        // check whether it is not too large
        unsigned int numConnections = myRequest->getSizes().second;
        if (numConnections >= 64) {
            // yep -> make it untcontrolled, warn
            WRITE_WARNING("Junction '" + getID() + "' is too complicated (#links>64); will be set to unregulated.");
            delete myRequest;
            myRequest = 0;
            myType = NODETYPE_NOJUNCTION;
        } else if (numConnections == 0) {
            delete myRequest;
            myRequest = 0;
            myType = NODETYPE_DEAD_END;
        } else {
            myRequest->buildBitfieldLogic(ec.isLeftHanded());
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
NBNode::computeNodeShape(bool leftHand, SUMOReal mismatchThreshold) {
    if (myIncomingEdges.size() == 0 && myOutgoingEdges.size() == 0) {
        return;
    }
    try {
        NBNodeShapeComputer computer(*this);
        myPoly = computer.compute(leftHand);
        if (myPoly.size() > 0) {
            PositionVector tmp = myPoly;
            tmp.push_back_noDoublePos(tmp[0]); // need closed shape
            if (mismatchThreshold >= 0
                    && !tmp.around(myPosition)  
                    && tmp.distance(myPosition) > mismatchThreshold) {
                WRITE_WARNING("Junction shape for '" + myID + "' has distance " + toString(tmp.distance(myPosition)) + " to its given position");
            }
        }
    } catch (InvalidArgument&) {
        WRITE_WARNING("For node '" + getID() + "': could not compute shape.");
        // make sure our shape is not empty because our XML schema forbids empty attributes
        myPoly.clear();
        myPoly.push_back(myPosition);
    }
}


void
NBNode::computeLanes2Lanes() {
    // special case a):
    //  one in, one out, the outgoing has one lane more
    if (myIncomingEdges.size() == 1 && myOutgoingEdges.size() == 1
            && myIncomingEdges[0]->getNumLanes() == myOutgoingEdges[0]->getNumLanes() - 1
            && myIncomingEdges[0] != myOutgoingEdges[0]
            && myIncomingEdges[0]->isConnectedTo(myOutgoingEdges[0])) {

        NBEdge* incoming = myIncomingEdges[0];
        NBEdge* outgoing = myOutgoingEdges[0];
        // check if it's not the turnaround
        if (incoming->getTurnDestination() == outgoing) {
            // will be added later or not...
            return;
        }
        for (int i = 0; i < (int) incoming->getNumLanes(); ++i) {
            incoming->setConnection(i, outgoing, i + 1, NBEdge::L2L_COMPUTED);
        }
        incoming->setConnection(0, outgoing, 0, NBEdge::L2L_COMPUTED);
        return;
    }
    // special case b):
    //  two in, one out, the outgoing has the same number of lanes as the sum of the incoming
    //  --> highway on-ramp
    bool check = false;
    if (myIncomingEdges.size() == 2 && myOutgoingEdges.size() == 1) {
        check = myIncomingEdges[0]->getNumLanes() + myIncomingEdges[1]->getNumLanes() == myOutgoingEdges[0]->getNumLanes();
        check &= (myIncomingEdges[0]->getStep() <= NBEdge::LANES2EDGES);
        check &= (myIncomingEdges[1]->getStep() <= NBEdge::LANES2EDGES);
        check &= myIncomingEdges[0] != myOutgoingEdges[0];
        check &= myIncomingEdges[1] != myOutgoingEdges[0];
        check &= myIncomingEdges[0]->isConnectedTo(myOutgoingEdges[0]);
        check &= myIncomingEdges[1]->isConnectedTo(myOutgoingEdges[0]);
    }
    if (check) {
        NBEdge* inc1 = myIncomingEdges[0];
        NBEdge* inc2 = myIncomingEdges[1];
        // for internal: check which one is the rightmost
        SUMOReal a1 = inc1->getAngleAtNode(this);
        SUMOReal a2 = inc2->getAngleAtNode(this);
        SUMOReal ccw = GeomHelper::getCCWAngleDiff(a1, a2);
        SUMOReal cw = GeomHelper::getCWAngleDiff(a1, a2);
        if (ccw > cw) {
            std::swap(inc1, inc2);
        }
        inc1->addLane2LaneConnections(0, myOutgoingEdges[0], 0, inc1->getNumLanes(), NBEdge::L2L_VALIDATED, true, true);
        inc2->addLane2LaneConnections(0, myOutgoingEdges[0], inc1->getNumLanes(), inc2->getNumLanes(), NBEdge::L2L_VALIDATED, true, true);
        return;
    }
    // special case c):
    //  one in, two out, the incoming has the same number of lanes as the sum of the outgoing
    //  --> highway off-ramp
    check = false;
    if (myIncomingEdges.size() == 1 && myOutgoingEdges.size() == 2) {
        check = myIncomingEdges[0]->getNumLanes() == myOutgoingEdges[1]->getNumLanes() + myOutgoingEdges[0]->getNumLanes();
        check &= (myIncomingEdges[0]->getStep() <= NBEdge::LANES2EDGES);
        check &= myIncomingEdges[0] != myOutgoingEdges[0];
        check &= myIncomingEdges[0] != myOutgoingEdges[1];
        check &= myIncomingEdges[0]->isConnectedTo(myOutgoingEdges[0]);
        check &= myIncomingEdges[0]->isConnectedTo(myOutgoingEdges[1]);
    }
    if (check) {
        NBEdge* out1 = myOutgoingEdges[0];
        NBEdge* out2 = myOutgoingEdges[1];
        // for internal: check which one is the rightmost
        if (NBContHelper::relative_outgoing_edge_sorter(myIncomingEdges[0])(out2, out1)) {
            std::swap(out1, out2);
        }
        myIncomingEdges[0]->addLane2LaneConnections(0, out1, 0, out1->getNumLanes(), NBEdge::L2L_VALIDATED, true, true);
        myIncomingEdges[0]->addLane2LaneConnections(out1->getNumLanes(), out2, 0, out2->getNumLanes(), NBEdge::L2L_VALIDATED, false, true);
        return;
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
        if (approaching->size() != 0) {
            ApproachingDivider divider(approaching, currentOutgoing);
            Bresenham::compute(&divider, static_cast<unsigned int>(approaching->size()),
                               currentOutgoing->getNumLanes());
        }
        delete approaching;
    }
    // ... but we may have the case that there are no outgoing edges
    //  In this case, we have to mark the incoming edges as being in state
    //   LANE2LANE( not RECHECK) by hand
    if (myOutgoingEdges.size() == 0) {
        for (i = myIncomingEdges.rbegin(); i != myIncomingEdges.rend(); i++) {
            (*i)->markAsInLane2LaneState();
        }
    }
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
NBNode::replaceOutgoing(NBEdge* which, NBEdge* by, unsigned int laneOff) {
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
    unsigned int laneOff = 0;
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
NBNode::replaceIncoming(NBEdge* which, NBEdge* by, unsigned int laneOff) {
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
    unsigned int laneOff = 0;
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
                                        unsigned int whichLaneOff, unsigned int byLaneOff) {
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
    unsigned int i, j;
    // check incoming
    for (i = 0; myIncomingEdges.size() > 0 && i < myIncomingEdges.size() - 1; i++) {
        j = i + 1;
        while (j < myIncomingEdges.size()) {
            if (myIncomingEdges[i] == myIncomingEdges[j]) {
                myIncomingEdges.erase(myIncomingEdges.begin() + j);
            } else {
                j++;
            }
        }
    }
    // check outgoing
    for (i = 0; myOutgoingEdges.size() > 0 && i < myOutgoingEdges.size() - 1; i++) {
        j = i + 1;
        while (j < myOutgoingEdges.size()) {
            if (myOutgoingEdges[i] == myOutgoingEdges[j]) {
                myOutgoingEdges.erase(myOutgoingEdges.begin() + j);
            } else {
                j++;
            }
        }
    }
    // check all
    for (i = 0; myAllEdges.size() > 0 && i < myAllEdges.size() - 1; i++) {
        j = i + 1;
        while (j < myAllEdges.size()) {
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
    unsigned int size = (unsigned int) edgeid.length();
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
    unsigned int size = (unsigned int) edgeid.length();
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
NBNode::mustBrake(const NBEdge* const from, const NBEdge* const to, int /* toLane */) const {
    // check whether it is participant to a traffic light
    //  - controlled links are set by the traffic lights, not the normal
    //    right-of-way rules
    //  - uncontrolled participants (spip lanes etc.) should always break
    if (myTrafficLights.size() != 0) {
        // ok, we have a traffic light, return true by now, it will be later
        //  controlled by the tls
        return true;
    }
    // unregulated->does not need to brake
    if (myRequest == 0) {
        return false;
    }
    // vehicles which do not have a following lane must always decelerate to the end
    if (to == 0) {
        return true;
    }
    // check whether any other connection on this node prohibits this connection
    return myRequest->mustBrake(from, to);
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
    } while ((!hasOutgoing(*i) || from->isTurningDirectionAt(this, *i)) && *i != from);
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
NBNode::getDirection(const NBEdge* const incoming, const NBEdge* const outgoing) const {
    // ok, no connection at all -> dead end
    if (outgoing == 0) {
        return LINKDIR_NODIR;
    }
    // turning direction
    if (incoming->isTurningDirectionAt(this, outgoing)) {
        return LINKDIR_TURN;
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
        NBContHelper::nextCW(myAllEdges, i);
        while ((*i) != incoming) {
            if ((*i)->getFromNode() == this) {
                return LINKDIR_PARTRIGHT;
            }
            NBContHelper::nextCW(myAllEdges, i);
        }
        return LINKDIR_RIGHT;
    }
    // check whether any other edge goes further to the left
    EdgeVector::const_iterator i =
        find(myAllEdges.begin(), myAllEdges.end(), outgoing);
    NBContHelper::nextCCW(myAllEdges, i);
    while ((*i) != incoming) {
        if ((*i)->getFromNode() == this && !incoming->isTurningDirectionAt(this, *i)) {
            return LINKDIR_PARTLEFT;
        }
        NBContHelper::nextCCW(myAllEdges, i);
    }
    return LINKDIR_LEFT;
}


LinkState
NBNode::getLinkState(const NBEdge* incoming, NBEdge* outgoing, int fromlane,
                     bool mayDefinitelyPass, const std::string& tlID) const {
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
    if ((!incoming->isInnerEdge() && mustBrake(incoming, outgoing, fromlane)) && !mayDefinitelyPass) {
        return myType == NODETYPE_PRIORITY_STOP ? LINKSTATE_STOP : LINKSTATE_MINOR; // minor road
    }
    // traffic lights are not regarded here
    return LINKSTATE_MAJOR;
}


bool
NBNode::checkIsRemovable() const {
    // check whether this node is included in a traffic light
    if (myTrafficLights.size() != 0) {
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
        return myIncomingEdges[0]->getFromNode() != myOutgoingEdges[0]->getToNode();
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
            // try to find the opposite direction
            NBNode* origin = (*i)->getFromNode();
            // find the back direction of the current edge
            EdgeVector::const_iterator j =
                find_if(myOutgoingEdges.begin(), myOutgoingEdges.end(),
                        NBContHelper::edge_with_destination_finder(origin));
            // check whether the back direction exists
            if (j != myOutgoingEdges.end()) {
                // check whether the edge from the backdirection (must be
                //  the counter-clockwise one) may be joined with the current
                NBContHelper::nextCCW(myOutgoingEdges, j);
                // check whether the types allow joining
                if (!(*i)->expandableBy(*j)) {
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
        NBNode* origin = (*i)->getFromNode();
        EdgeVector::const_iterator j =
            find_if(myOutgoingEdges.begin(), myOutgoingEdges.end(),
                    NBContHelper::edge_with_destination_finder(origin));
        NBContHelper::nextCCW(myOutgoingEdges, j);
        ret.push_back(std::pair<NBEdge*, NBEdge*>(*i, *j));
    }
    return ret;
}


const PositionVector&
NBNode::getShape() const {
    return myPoly;
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


void
NBNode::buildInnerEdges() {
    unsigned int noInternalNoSplits = 0;
    for (EdgeVector::const_iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
        const std::vector<NBEdge::Connection>& elv = (*i)->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
            if ((*k).toEdge == 0) {
                continue;
            }
            noInternalNoSplits++;
        }
    }
    unsigned int lno = 0;
    unsigned int splitNo = 0;
    for (EdgeVector::const_iterator i = myIncomingEdges.begin(); i != myIncomingEdges.end(); i++) {
        (*i)->buildInnerEdges(*this, noInternalNoSplits, lno, splitNo);
    }
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


Position
NBNode::getCenter() const {
    /* Conceptually, the center point would be identical with myPosition.
    * However, if the shape is influenced by custom geometry endpoints of the adjoining edges,
    * myPosition may fall outside the shape. In this case it is better to use
    * the center of the shape
    **/
    PositionVector tmp = myPoly;
    tmp.closePolygon();
    //std::cout << getID() << " around=" << tmp.around(myPosition) << " dist=" << tmp.distance(myPosition) << "\n";
    if (tmp.size() < 3 || tmp.around(myPosition) || tmp.distance(myPosition) < POSITION_EPS) {
        return myPosition;
    } else {
        return myPoly.getPolygonCenter();
    }
}

/****************************************************************************/

