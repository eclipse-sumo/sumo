/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Helper.cpp
/// @author  Laura Bieker-Walz
/// @author  Robert Hilbrich
/// @date    15.09.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/geom/GeomHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSTransportableControl.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSTransportable.h>
#include <microsim/pedestrians/MSPerson.h>
#include <libsumo/TraCIDefs.h>
#include <libsumo/InductionLoop.h>
#include <libsumo/Junction.h>
#include <libsumo/POI.h>
#include <libsumo/Polygon.h>
#include <traci-server/TraCIConstants.h>
#include "Helper.h"

#define FAR_AWAY 1000.0

//#define DEBUG_MOVEXY
//#define DEBUG_MOVEXY_ANGLE


void
LaneStoringVisitor::add(const MSLane* const l) const {
    switch (myDomain) {
        case CMD_GET_VEHICLE_VARIABLE: {
            const MSLane::VehCont& vehs = l->getVehiclesSecure();
            for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
                if (myShape.distance2D((*j)->getPosition()) <= myRange) {
                    myIDs.insert((*j)->getID());
                }
            }
            l->releaseVehicles();
        }
        break;
        case CMD_GET_PERSON_VARIABLE: {
            l->getVehiclesSecure();
            std::vector<MSTransportable*> persons = l->getEdge().getSortedPersons(MSNet::getInstance()->getCurrentTimeStep(), true);
            for (auto p : persons) {
                if (myShape.distance2D(p->getPosition()) <= myRange) {
                    myIDs.insert(p->getID());
                }
            }
            l->releaseVehicles();
        }
        break;
        case CMD_GET_EDGE_VARIABLE: {
            if (myShape.size() != 1 || l->getShape().distance2D(myShape[0]) <= myRange) {
                myIDs.insert(l->getEdge().getID());
            }
        }
        break;
        case CMD_GET_LANE_VARIABLE: {
            if (myShape.size() != 1 || l->getShape().distance2D(myShape[0]) <= myRange) {
                myIDs.insert(l->getID());
            }
        }
        break;
        default:
            break;

    }
}

namespace libsumo {
// ===========================================================================
// static member definitions
// ===========================================================================
std::map<int, NamedRTree*> Helper::myObjects;
LANE_RTREE_QUAL* Helper::myLaneTree;
std::map<std::string, MSVehicle*> Helper::myRemoteControlledVehicles;
std::map<std::string, MSPerson*> Helper::myRemoteControlledPersons;

// ===========================================================================
// member definitions
// ===========================================================================
TraCIPositionVector
Helper::makeTraCIPositionVector(const PositionVector& positionVector) {
    TraCIPositionVector tp;
    for (int i = 0; i < (int)positionVector.size(); ++i) {
        tp.push_back(makeTraCIPosition(positionVector[i]));
    }
    return tp;
}


PositionVector
Helper::makePositionVector(const TraCIPositionVector& vector) {
    PositionVector pv;
    for (int i = 0; i < (int)vector.size(); i++) {
        pv.push_back(Position(vector[i].x, vector[i].y));
    }
    return pv;
}


TraCIColor
Helper::makeTraCIColor(const RGBColor& color) {
    TraCIColor tc;
    tc.a = color.alpha();
    tc.b = color.blue();
    tc.g = color.green();
    tc.r = color.red();
    return tc;
}


RGBColor
Helper::makeRGBColor(const TraCIColor& c) {
    return RGBColor((unsigned char)c.r, (unsigned char)c.g, (unsigned char)c.b, (unsigned char)c.a);
}


TraCIPosition
Helper::makeTraCIPosition(const Position& position) {
    TraCIPosition p;
    p.x = position.x();
    p.y = position.y();
    p.z = position.z();
    return p;
}


Position
Helper::makePosition(const TraCIPosition& tpos) {
    Position p;
    p.set(tpos.x, tpos.y, tpos.z);
    return p;
}


MSEdge*
Helper::getEdge(const std::string& edgeID) {
    MSEdge* edge = MSEdge::dictionary(edgeID);
    if (edge == 0) {
        throw TraCIException("Referenced edge '" + edgeID + "' is not known.");
    }
    return edge;
}


const MSLane*
Helper::getLaneChecking(const std::string& edgeID, int laneIndex, double pos) {
    const MSEdge* edge = MSEdge::dictionary(edgeID);
    if (edge == 0) {
        throw TraCIException("Unknown edge " + edgeID);
    }
    if (laneIndex < 0 || laneIndex >= (int)edge->getLanes().size()) {
        throw TraCIException("Invalid lane index for " + edgeID);
    }
    const MSLane* lane = edge->getLanes()[laneIndex];
    if (pos < 0 || pos > lane->getLength()) {
        throw TraCIException("Position on lane invalid");
    }
    return lane;
}


std::pair<MSLane*, double>
Helper::convertCartesianToRoadMap(Position pos) {
    /// XXX use rtree instead
    std::pair<MSLane*, double> result;
    std::vector<std::string> allEdgeIds;
    double minDistance = std::numeric_limits<double>::max();

    allEdgeIds = MSNet::getInstance()->getEdgeControl().getEdgeNames();
    for (std::vector<std::string>::iterator itId = allEdgeIds.begin(); itId != allEdgeIds.end(); itId++) {
        const std::vector<MSLane*>& allLanes = MSEdge::dictionary((*itId))->getLanes();
        for (std::vector<MSLane*>::const_iterator itLane = allLanes.begin(); itLane != allLanes.end(); itLane++) {
            const double newDistance = (*itLane)->getShape().distance2D(pos);
            if (newDistance < minDistance) {
                minDistance = newDistance;
                result.first = (*itLane);
            }
        }
    }
    // @todo this may be a place where 3D is required but 2D is delivered
    result.second = result.first->getShape().nearest_offset_to_point2D(pos, false);
    return result;
}

void
Helper::cleanup() {
    for (std::map<int, NamedRTree*>::const_iterator i = myObjects.begin(); i != myObjects.end(); ++i) {
        delete(*i).second;
    }
    myObjects.clear();
    delete myLaneTree;
    myLaneTree = 0;
}


void
Helper::collectObjectsInRange(int domain, const PositionVector& shape, double range, std::set<std::string>& into) {
    // build the look-up tree if not yet existing
    if (myObjects.find(domain) == myObjects.end()) {
        switch (domain) {
            case CMD_GET_INDUCTIONLOOP_VARIABLE:
                myObjects[CMD_GET_INDUCTIONLOOP_VARIABLE] = InductionLoop::getTree();
                break;
            case CMD_GET_EDGE_VARIABLE:
            case CMD_GET_LANE_VARIABLE:
            case CMD_GET_PERSON_VARIABLE:
            case CMD_GET_VEHICLE_VARIABLE:
                myObjects[CMD_GET_EDGE_VARIABLE] = 0;
                myObjects[CMD_GET_LANE_VARIABLE] = 0;
                myObjects[CMD_GET_PERSON_VARIABLE] = 0;
                myObjects[CMD_GET_VEHICLE_VARIABLE] = 0;
                myLaneTree = new LANE_RTREE_QUAL(&MSLane::visit);
                MSLane::fill(*myLaneTree);
                break;
            case CMD_GET_POI_VARIABLE:
                myObjects[CMD_GET_POI_VARIABLE] = POI::getTree();
                break;
            case CMD_GET_POLYGON_VARIABLE:
                myObjects[CMD_GET_POLYGON_VARIABLE] = Polygon::getTree();
                break;
            case CMD_GET_JUNCTION_VARIABLE:
                myObjects[CMD_GET_JUNCTION_VARIABLE] = Junction::getTree();
                break;
            default:
                break;
        }
    }
    const Boundary b = shape.getBoxBoundary().grow(range);
    const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
    const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
    switch (domain) {
        case CMD_GET_INDUCTIONLOOP_VARIABLE:
        case CMD_GET_POI_VARIABLE:
        case CMD_GET_POLYGON_VARIABLE:
        case CMD_GET_JUNCTION_VARIABLE: {
            Named::StoringVisitor sv(into);
            myObjects[domain]->Search(cmin, cmax, sv);
        }
        break;
        case CMD_GET_EDGE_VARIABLE:
        case CMD_GET_LANE_VARIABLE:
        case CMD_GET_PERSON_VARIABLE:
        case CMD_GET_VEHICLE_VARIABLE: {
            LaneStoringVisitor sv(into, shape, range, domain);
            myLaneTree->Search(cmin, cmax, sv);
        }
        break;
        default:
            break;
    }
}

void
Helper::setRemoteControlled(MSVehicle* v, Position xyPos, MSLane* l, double pos, double posLat, double angle,
                            int edgeOffset, ConstMSEdgeVector route, SUMOTime t) {
    myRemoteControlledVehicles[v->getID()] = v;
    v->getInfluencer().setRemoteControlled(xyPos, l, pos, posLat, angle, edgeOffset, route, t);
}

void
Helper::setRemoteControlled(MSPerson* p, Position xyPos, MSLane* l, double pos, double posLat, double angle,
                            int edgeOffset, ConstMSEdgeVector route, SUMOTime t) {
    myRemoteControlledPersons[p->getID()] = p;
    p->getInfluencer().setRemoteControlled(xyPos, l, pos, posLat, angle, edgeOffset, route, t);
}


void
Helper::postProcessRemoteControl() {
    for (auto& controlled : myRemoteControlledVehicles) {
        if (MSNet::getInstance()->getVehicleControl().getVehicle(controlled.first) != 0) {
            controlled.second->getInfluencer().postProcessRemoteControl(controlled.second);
        } else {
            WRITE_WARNING("Vehicle '" + controlled.first + "' was removed though being controlled by TraCI");
        }
    }
    myRemoteControlledVehicles.clear();
    for (auto& controlled : myRemoteControlledPersons) {
        if (MSNet::getInstance()->getPersonControl().get(controlled.first) != 0) {
            controlled.second->getInfluencer().postProcessRemoteControl(controlled.second);
        } else {
            WRITE_WARNING("Person '" + controlled.first + "' was removed though being controlled by TraCI");
        }
    }
    myRemoteControlledPersons.clear();
}


bool
Helper::moveToXYMap(const Position& pos, double maxRouteDistance, bool mayLeaveNetwork, const std::string& origID, const double angle,
                    double speed, const ConstMSEdgeVector& currentRoute, const int routePosition, MSLane* currentLane, double currentLanePos, bool onRoad,
                    double& bestDistance, MSLane** lane, double& lanePos, int& routeOffset, ConstMSEdgeVector& edges) {
    // collect edges around the vehicle/person
    const MSEdge* const currentRouteEdge = currentRoute[routePosition];
    std::set<std::string> into;
    PositionVector shape;
    shape.push_back(pos);
    collectObjectsInRange(CMD_GET_EDGE_VARIABLE, shape, maxRouteDistance, into);
    double maxDist = 0;
    std::map<MSLane*, LaneUtility> lane2utility;
    // compute utility for all candidate edges
    for (std::set<std::string>::const_iterator j = into.begin(); j != into.end(); ++j) {
        const MSEdge* const e = MSEdge::dictionary(*j);
        const MSEdge* prevEdge = 0;
        const MSEdge* nextEdge = 0;
        bool onRoute = false;
        // the next if/the clause sets "onRoute", "prevEdge", and "nextEdge", depending on
        //  whether the currently seen edge is an internal one or a normal one
        if (!e->isInternal()) {
#ifdef DEBUG_MOVEXY_ANGLE
            std::cout << "Ego on normal" << std::endl;
#endif
            // a normal edge
            //
            // check whether the currently seen edge is in the vehicle's route
            //  - either the one it's on or one of the next edges
            ConstMSEdgeVector::const_iterator searchStart = currentRoute.begin() + routePosition;
            if (onRoad && currentLane->getEdge().isInternal()) {
                ++searchStart;
            }
            ConstMSEdgeVector::const_iterator edgePos = std::find(searchStart, currentRoute.end(), e);
            onRoute = edgePos != currentRoute.end(); // no? -> onRoute is false
            if (edgePos == currentRoute.end() - 1 && currentRouteEdge == e) {
                // onRoute is false as well if the vehicle is beyond the edge
                onRoute &= currentRouteEdge->getLength() > currentLanePos + SPEED2DIST(speed);
            }
            // save prior and next edges
            prevEdge = e;
            nextEdge = !onRoute || edgePos == currentRoute.end() - 1 ? 0 : *(edgePos + 1);
#ifdef DEBUG_MOVEXY_ANGLE
            std::cout << "normal:" << e->getID() << " prev:" << prevEdge->getID() << " next:";
            if (nextEdge != 0) {
                std::cout << nextEdge->getID();
            }
            std::cout << std::endl;
#endif
        } else {
#ifdef DEBUG_MOVEXY_ANGLE
            std::cout << "Ego on internal" << std::endl;
#endif
            // an internal edge
            // get the previous edge
            prevEdge = e;
            while (prevEdge != 0 && prevEdge->isInternal()) {
                MSLane* l = prevEdge->getLanes()[0];
                l = l->getLogicalPredecessorLane();
                prevEdge = l == 0 ? 0 : &l->getEdge();
            }
            // check whether the previous edge is on the route (was on the route)
            ConstMSEdgeVector::const_iterator prevEdgePos = std::find(currentRoute.begin() + routePosition, currentRoute.end(), prevEdge);
            nextEdge = e;
            while (nextEdge != 0 && nextEdge->isInternal()) {
                nextEdge = nextEdge->getSuccessors()[0]; // should be only one for an internal edge
            }
            if (prevEdgePos != currentRoute.end() && (prevEdgePos + 1) != currentRoute.end()) {
                onRoute = *(prevEdgePos + 1) == nextEdge;
            }
#ifdef DEBUG_MOVEXY_ANGLE
            std::cout << "internal:" << e->getID() << " prev:" << prevEdge->getID() << " next:" << nextEdge->getID() << std::endl;
#endif
        }


        // weight the lanes...
        const std::vector<MSLane*>& lanes = e->getLanes();
        const bool perpendicular = false;
        for (std::vector<MSLane*>::const_iterator k = lanes.begin(); k != lanes.end(); ++k) {
            MSLane* lane = *k;
            double langle = 180.;
            double dist = FAR_AWAY;
            double perpendicularDist = FAR_AWAY;
            double off = lane->getShape().nearest_offset_to_point2D(pos, true);
            if (off != GeomHelper::INVALID_OFFSET) {
                perpendicularDist = lane->getShape().distance2D(pos, true);
            }
            off = lane->getShape().nearest_offset_to_point2D(pos, perpendicular);
            if (off != GeomHelper::INVALID_OFFSET) {
                dist = lane->getShape().distance2D(pos, perpendicular);
                langle = GeomHelper::naviDegree(lane->getShape().rotationAtOffset(off));
            }
            bool sameEdge = onRoad && &lane->getEdge() == &currentLane->getEdge() && currentRouteEdge->getLength() > currentLanePos + SPEED2DIST(speed);
            /*
            const MSEdge* rNextEdge = nextEdge;
            while(rNextEdge==0&&lane->getEdge().getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
                MSLane* next = lane->getLinkCont()[0]->getLane();
                rNextEdge = next == 0 ? 0 : &next->getEdge();
            }
            */
            double dist2 = dist;
            if (mayLeaveNetwork && dist != perpendicularDist) {
                // ambiguous mapping. Don't trust this
                dist2 = FAR_AWAY;
            }
            const double angleDiff = (angle == INVALID_DOUBLE_VALUE ? 0 : GeomHelper::getMinAngleDiff(angle, langle));
#ifdef DEBUG_MOVEXY_ANGLE
            std::cout << lane->getID() << " lAngle:" << langle << " lLength=" << lane->getLength()
                      << " angleDiff:" << angleDiff
                      << " off:" << off
                      << " pDist=" << perpendicularDist
                      << " dist=" << dist
                      << " dist2=" << dist2
                      << "\n";
            std::cout << lane->getID() << " param=" << lane->getParameter(SUMO_PARAM_ORIGID, lane->getID()) << " origID='" << origID << "\n";
#endif
            lane2utility[lane] = LaneUtility(
                                     dist2, perpendicularDist, off, angleDiff,
                                     lane->getParameter(SUMO_PARAM_ORIGID, lane->getID()) == origID,
                                     onRoute, sameEdge, prevEdge, nextEdge);
            // update scaling value
            maxDist = MAX2(maxDist, MIN2(dist, SUMO_const_laneWidth));

        }
    }

    // get the best lane given the previously computed values
    double bestValue = 0;
    MSLane* bestLane = 0;
    for (std::map<MSLane*, LaneUtility>::iterator i = lane2utility.begin(); i != lane2utility.end(); ++i) {
        MSLane* l = (*i).first;
        const LaneUtility& u = (*i).second;
        double distN = u.dist > 999 ? -10 : 1. - (u.dist / maxDist);
        double angleDiffN = 1. - (u.angleDiff / 180.);
        double idN = u.ID ? 1 : 0;
        double onRouteN = u.onRoute ? 1 : 0;
        double sameEdgeN = u.sameEdge ? MIN2(currentRouteEdge->getLength() / speed, (double)1.) : 0;
        double value = (distN * .5 // distance is more important than angle because the vehicle might be driving in the opposite direction
                        + angleDiffN * 0.35 /*.5 */
                        + idN * 1
                        + onRouteN * 0.1
                        + sameEdgeN * 0.1);
#ifdef DEBUG_MOVEXY
        std::cout << " x; l:" << l->getID() << " d:" << u.dist << " dN:" << distN << " aD:" << angleDiffN <<
                  " ID:" << idN << " oRN:" << onRouteN << " sEN:" << sameEdgeN << " value:" << value << std::endl;
#endif
        if (value > bestValue || bestLane == 0) {
            bestValue = value;
            if (u.dist == FAR_AWAY) {
                bestLane = 0;
            } else {
                bestLane = l;
            }
        }
    }
    // no best lane found, return
    if (bestLane == 0) {
        return false;
    }
    const LaneUtility& u = lane2utility.find(bestLane)->second;
    bestDistance = u.dist;
    *lane = bestLane;
    lanePos = bestLane->getShape().nearest_offset_to_point2D(pos, false);
    const MSEdge* prevEdge = u.prevEdge;
    if (u.onRoute) {
        ConstMSEdgeVector::const_iterator prevEdgePos = std::find(currentRoute.begin(), currentRoute.end(), prevEdge);
        routeOffset = (int)std::distance(currentRoute.begin(), prevEdgePos);
        //std::cout << SIMTIME << "moveToXYMap vehicle=" << veh.getID() << " currLane=" << veh.getLane()->getID() << " routeOffset=" << routeOffset << " edges=" << toString(ev) << " bestLane=" << bestLane->getID() << " prevEdge=" << prevEdge->getID() << "\n";
    } else {
        edges.push_back(u.prevEdge);
        /*
           if(bestLane->getEdge().getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
           edges.push_back(&bestLane->getEdge());
           }
        */
        if (u.nextEdge != 0) {
            edges.push_back(u.nextEdge);
        }
        routeOffset = 0;
#ifdef DEBUG_MOVEXY_ANGLE
        std::cout << "internal2: lane=" << bestLane->getID() << " prev=" << Named::getIDSecure(u.prevEdge) << " next=" << Named::getIDSecure(u.nextEdge) << "\n";;
#endif
    }
    return true;
}


bool
Helper::findCloserLane(const MSEdge* edge, const Position& pos, double& bestDistance, MSLane** lane) {
    if (edge == 0) {
        return false;
    }
    const std::vector<MSLane*>& lanes = edge->getLanes();
    bool newBest = false;
    for (std::vector<MSLane*>::const_iterator k = lanes.begin(); k != lanes.end() && bestDistance > POSITION_EPS; ++k) {
        MSLane* candidateLane = *k;
        const double dist = candidateLane->getShape().distance2D(pos); // get distance
#ifdef DEBUG_MOVEXY
        std::cout << "   b at lane " << candidateLane->getID() << " dist:" << dist << " best:" << bestDistance << std::endl;
#endif
        if (dist < bestDistance) {
            // is the new distance the best one? keep then...
            bestDistance = dist;
            *lane = candidateLane;
            newBest = true;
        }
    }
    return newBest;
}

bool
Helper::moveToXYMap_matchingRoutePosition(const Position& pos, const std::string& origID,
        const ConstMSEdgeVector& currentRoute, int routeIndex,
        double& bestDistance, MSLane** lane, double& lanePos, int& routeOffset) {
    routeOffset = 0;
    // routes may be looped which makes routeOffset ambiguous. We first try to
    // find the closest upcoming edge on the route and then look for closer passed edges

    // look forward along the route
    const MSEdge* prev = 0;
    for (int i = routeIndex; i < (int)currentRoute.size(); ++i) {
        const MSEdge* cand = currentRoute[i];
        while (prev != 0) {
            // check internal edge(s)
            const MSEdge* internalCand = prev->getInternalFollowingEdge(cand);
            findCloserLane(internalCand, pos, bestDistance, lane);
            prev = internalCand;
        }
        if (findCloserLane(cand, pos, bestDistance, lane)) {
            routeOffset = i;
        }
        prev = cand;
    }
    // look backward along the route
    const MSEdge* next = currentRoute[routeIndex];
    for (int i = routeIndex; i > 0; --i) {
        const MSEdge* cand = currentRoute[i];
        prev = cand;
        while (prev != 0) {
            // check internal edge(s)
            const MSEdge* internalCand = prev->getInternalFollowingEdge(next);
            findCloserLane(internalCand, pos, bestDistance, lane);
            prev = internalCand;
        }
        if (findCloserLane(cand, pos, bestDistance, lane)) {
            routeOffset = i;
        }
        next = cand;
    }

    assert(lane != 0);
    // quit if no solution was found, reporting a failure
    if (lane == 0) {
#ifdef DEBUG_MOVEXY
        std::cout << "  b failed - no best route lane" << std::endl;
#endif
        return false;
    }


    // position may be inaccurate; let's checkt the given index, too
    // @note: this is enabled for non-internal lanes only, as otherwise the position information may ambiguous
    if (!(*lane)->getEdge().isInternal()) {
        const std::vector<MSLane*>& lanes = (*lane)->getEdge().getLanes();
        for (std::vector<MSLane*>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
            if ((*i)->getParameter(SUMO_PARAM_ORIGID, (*i)->getID()) == origID) {
                *lane = *i;
                break;
            }
        }
    }
    // check position, stuff, we should have the best lane along the route
    lanePos = MAX2(0., MIN2(double((*lane)->getLength() - POSITION_EPS),
                            (*lane)->interpolateGeometryPosToLanePos(
                                (*lane)->getShape().nearest_offset_to_point2D(pos, false))));
    //std::cout << SIMTIME << " moveToXYMap_matchingRoutePosition vehicle=" << veh.getID() << " currLane=" << veh.getLane()->getID() << " routeOffset=" << routeOffset << " edges=" << toString(edges) << " lane=" << (*lane)->getID() << "\n";
#ifdef DEBUG_MOVEXY
    std::cout << "  b ok lane " << (*lane)->getID() << " lanePos:" << lanePos << std::endl;
#endif
    return true;
}

}


/****************************************************************************/
