/****************************************************************************/
/// @file    NBNodeShapeComputer.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// This class computes shapes of junctions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
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

#include <algorithm>
#include <iterator>
#include <utils/geom/PositionVector.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/StdDefs.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "NBNode.h"
#include "NBNodeShapeComputer.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NBNodeShapeComputer::NBNodeShapeComputer(const NBNode& node)
    : myNode(node) {}


NBNodeShapeComputer::~NBNodeShapeComputer() {}


PositionVector
NBNodeShapeComputer::compute(bool leftHand) {
    UNUSED_PARAMETER(leftHand);
    PositionVector ret;
    // check whether the node is a dead end node or a node where only turning is possible
    //  in this case, we will use "computeNodeShapeSmall"
    bool singleDirection = false;
    if (myNode.myAllEdges.size() == 1) {
        singleDirection = true;
    }
    if (myNode.myAllEdges.size() == 2 && myNode.getIncomingEdges().size() == 1) {
        if (myNode.getIncomingEdges()[0]->isTurningDirectionAt(myNode.getOutgoingEdges()[0])) {
            singleDirection = true;
        }
    }
    if (singleDirection) {
        return computeNodeShapeSmall();
    }
    // check whether the node is a just something like a geometry
    //  node (one in and one out or two in and two out, pair-wise continuations)
    // also in this case "computeNodeShapeSmall" is used
    bool geometryLike = myNode.isSimpleContinuation();
    if (geometryLike) {
        // additionally, the angle between the edges must not be larger than 45 degrees
        //  (otherwise, we will try to compute the shape in a different way)
        const EdgeVector& incoming = myNode.getIncomingEdges();
        const EdgeVector& outgoing = myNode.getOutgoingEdges();
        SUMOReal maxAngle = SUMOReal(0);
        for (EdgeVector::const_iterator i = incoming.begin(); i != incoming.end(); ++i) {
            SUMOReal ia = (*i)->getAngleAtNode(&myNode);
            for (EdgeVector::const_iterator j = outgoing.begin(); j != outgoing.end(); ++j) {
                SUMOReal oa = (*j)->getAngleAtNode(&myNode);
                SUMOReal ad = GeomHelper::getMinAngleDiff(ia, oa);
                if (22.5 >= ad) {
                    maxAngle = MAX2(ad, maxAngle);
                }
            }
        }
        if (maxAngle > 22.5) {
            return computeNodeShapeSmall();
        }
    }

    //
    ret = computeNodeShapeDefault(geometryLike);
    // fail fall-back: use "computeNodeShapeSmall"
    if (ret.size() < 3) {
        ret = computeNodeShapeSmall();
    }
    return ret;
}


void
computeSameEnd(PositionVector& l1, PositionVector& l2) {
    Line sub(l1.lineAt(0).getPositionAtDistance2D(100), l1[1]);
    Line tmp(sub);
    tmp.rotateAtP1(M_PI / 2);
    tmp.extrapolateBy2D(100);
    if (l1.intersects(tmp.p1(), tmp.p2())) {
        SUMOReal offset1 = l1.intersectsAtLengths2D(tmp)[0];
        Line tl1 = Line(
                       l1.lineAt(0).getPositionAtDistance2D(offset1),
                       l1[1]);
        tl1.extrapolateBy2D(100);
        l1.replaceAt(0, tl1.p1());
    }
    if (l2.intersects(tmp.p1(), tmp.p2())) {
        SUMOReal offset2 = l2.intersectsAtLengths2D(tmp)[0];
        Line tl2 = Line(
                       l2.lineAt(0).getPositionAtDistance2D(offset2),
                       l2[1]);
        tl2.extrapolateBy2D(100);
        l2.replaceAt(0, tl2.p1());
    }
}


PositionVector
NBNodeShapeComputer::computeNodeShapeDefault(bool simpleContinuation) {
    // if we have less than two edges, we can not compute the node's shape this way
    if (myNode.myAllEdges.size() < 2) {
        return PositionVector();
    }
    // magic values
    const SUMOReal radius = (myNode.getRadius() == NBNode::UNSPECIFIED_RADIUS ? NBNode::DEFAULT_RADIUS : myNode.getRadius());
    const int cornerDetail = OptionsCont::getOptions().getInt("junctions.corner-detail");

    // initialise
    EdgeVector::const_iterator i;
    // edges located in the value-vector have the same direction as the key edge
    std::map<NBEdge*, std::set<NBEdge*> > same;
    // the counter-clockwise boundary of the edge regarding possible same-direction edges
    GeomsMap geomsCCW;
    // the clockwise boundary of the edge regarding possible same-direction edges
    GeomsMap geomsCW;
    // store relationships
    std::map<NBEdge*, NBEdge*> ccwBoundary;
    std::map<NBEdge*, NBEdge*> cwBoundary;
    for (i = myNode.myAllEdges.begin(); i != myNode.myAllEdges.end(); i++) {
        cwBoundary[*i] = *i;
        ccwBoundary[*i] = *i;
    }
    // check which edges are parallel
    joinSameDirectionEdges(same, geomsCCW, geomsCW);
    // compute unique direction list
    EdgeVector newAll = computeUniqueDirectionList(same, geomsCCW, geomsCW, ccwBoundary, cwBoundary);
    // if we have only two "directions", let's not compute the geometry using this method
    if (newAll.size() < 2) {
        return PositionVector();
    }

    // All geoms are outoing from myNode.
    // for every direction in newAll we compute the offset at which the
    // intersection ends and the edge starts. This value is saved in 'distances'
    // If the geometries need to be extended to get an intersection, this is
    // recorded in 'myExtended'
    std::map<NBEdge*, SUMOReal> distances;
    std::map<NBEdge*, bool> myExtended;

    for (i = newAll.begin(); i != newAll.end(); ++i) {
        EdgeVector::const_iterator cwi = i;
        EdgeVector::const_iterator ccwi = i;
        SUMOReal ccad;
        SUMOReal cad;
        initNeighbors(newAll, i, geomsCW, geomsCCW, cwi, ccwi, cad, ccad);
        assert(geomsCCW.find(*i) != geomsCCW.end());
        assert(geomsCW.find(*ccwi) != geomsCW.end());
        assert(geomsCW.find(*cwi) != geomsCW.end());

        // there are only 2 directions and they are almost parallel
        if (*cwi == *ccwi &&
                (
                    // no change in lane numbers, even low angles still give a good intersection
                    (simpleContinuation && fabs(ccad - cad) < (SUMOReal) 0.1)
                    // lane numbers change, a direct intersection could be far away from the node position
                    // so we use a larger threshold
                    || (!simpleContinuation && fabs(ccad - cad) < DEG2RAD(22.5)))
           ) {
            // compute the mean position between both edges ends ...
            Position p;
            if (myExtended.find(*ccwi) != myExtended.end()) {
                p = geomsCCW[*ccwi][0];
                p.add(geomsCW[*ccwi][0]);
                p.mul(0.5);
            } else {
                p = geomsCCW[*ccwi][0];
                p.add(geomsCW[*ccwi][0]);
                p.add(geomsCCW[*i][0]);
                p.add(geomsCW[*i][0]);
                p.mul(0.25);
            }
            // ... compute the distance to this point ...
            SUMOReal dist = geomsCCW[*i].nearest_offset_to_point2D(p);
            if (dist < 0) {
                // ok, we have the problem that even the extrapolated geometry
                //  does not reach the point
                // in this case, the geometry has to be extenden... too bad ...
                // ... let's append the mean position to the geometry
                PositionVector g = (*i)->getGeometry();
                if (myNode.hasIncoming(*i)) {
                    g.push_back_noDoublePos(p);
                } else {
                    g.push_front_noDoublePos(p);
                }
                (*i)->setGeometry(g);
                // and rebuild previous information
                geomsCCW[*i] = (*i)->getCCWBoundaryLine(myNode);
                geomsCCW[*i].extrapolate(100);
                geomsCW[*i] = (*i)->getCWBoundaryLine(myNode);
                geomsCW[*i].extrapolate(100);
                // the distance is now = zero (the point we have appended)
                distances[*i] = 100;
                myExtended[*i] = true;
            } else {
                if (!simpleContinuation) {
                    // since there are only two (almost parallel) directions, the
                    // concept of a turning radius does not quite fit. Instead we need
                    // to enlarge the intersection to accomodate the change in
                    // the number of lanes
                    // @todo: make this independently configurable
                    dist += radius;
                }
                distances[*i] = dist;
            }

        } else {
            // the angles are different enough to compute the intersection of
            // the outer boundaries directly (or there are more than 2 directions). The "nearer" neighbar causes the furthest distance
            const bool ccwCloser = ccad < cad;
            // the border facing the closer neighbor
            const PositionVector& currGeom = ccwCloser ? geomsCCW[*i] : geomsCW[*i];
            // the border facing the far neighbor
            const PositionVector& currGeom2 = ccwCloser ? geomsCW[*i] : geomsCCW[*i];
            // the border of the closer neighbor
            const PositionVector& neighGeom = ccwCloser ? geomsCW[*ccwi] : geomsCCW[*cwi];
            // the border of the far neighbor
            const PositionVector& neighGeom2 = ccwCloser ? geomsCCW[*cwi] : geomsCW[*ccwi];
            if (!simpleContinuation) {
                if (currGeom.intersects(neighGeom)) {
                    distances[*i] = radius + closestIntersection(currGeom, neighGeom, 100);
                    if (*cwi != *ccwi && currGeom2.intersects(neighGeom2)) {
                        const SUMOReal farAngleDist = ccwCloser ? cad : ccad;
                        SUMOReal a1 = distances[*i];
                        SUMOReal a2 = radius + closestIntersection(currGeom2, neighGeom2, 100);
                        if (ccad > DEG2RAD(90. + 45.) && cad > DEG2RAD(90. + 45.)) {
                            SUMOReal mmin = MIN2(distances[*cwi], distances[*ccwi]);
                            if (mmin > 100) {
                                distances[*i] = (SUMOReal) 5. + (SUMOReal) 100. - (SUMOReal)(mmin - 100); //100 + 1.5;
                            }
                        } else if (fabs(a2 - a1) < 10 || farAngleDist < DEG2RAD(135)) {
                            distances[*i] = MAX2(a1, a2);
                        }
                    }
                } else {
                    if (*cwi != *ccwi && currGeom2.intersects(neighGeom2)) {
                        distances[*i] = radius + currGeom2.intersectsAtLengths2D(neighGeom2)[0];
                    } else {
                        distances[*i] = 100 + radius;
                    }
                }
            } else {
                if (currGeom.intersects(neighGeom)) {
                    distances[*i] = currGeom.intersectsAtLengths2D(neighGeom)[0];
                } else {
                    distances[*i] = (SUMOReal) 100.;
                }
            }
        }
    }

    for (i = newAll.begin(); i != newAll.end(); ++i) {
         if (distances.find(*i) == distances.end()) {
             assert(false);
             distances[*i] = 100;
         }
    }

    // build
    PositionVector ret;
    for (i = newAll.begin(); i != newAll.end(); ++i) {
        const PositionVector& ccwBound = geomsCCW[*i];
        SUMOReal len = ccwBound.length();
        SUMOReal offset = distances[*i];
        if (offset == -1) {
            offset = (SUMOReal) - .1;
        }
        Position p;
        if (len >= offset) {
            p = ccwBound.positionAtOffset2D(offset);
        } else {
            p = ccwBound.positionAtOffset2D(len);
        }
        p.set(p.x(), p.y(), myNode.getPosition().z());
        if (i != newAll.begin()) {
            ret.append(getSmoothCorner(geomsCW[*(i - 1)].reverse(), ccwBound, ret[-1], p, cornerDetail));
        }
        ret.push_back_noDoublePos(p);
        //
        const PositionVector& cwBound = geomsCW[*i];
        len = cwBound.length();
        if (len >= offset) {
            p = cwBound.positionAtOffset2D(offset);
        } else {
            p = cwBound.positionAtOffset2D(len);
        }
        p.set(p.x(), p.y(), myNode.getPosition().z());
        ret.push_back_noDoublePos(p);
    }
    // final curve segment
    ret.append(getSmoothCorner(geomsCW[*(newAll.end() - 1)], geomsCCW[*newAll.begin()], ret[-1], ret[0], cornerDetail));
    return ret;
}


SUMOReal 
NBNodeShapeComputer::closestIntersection(const PositionVector& geom1, const PositionVector& geom2, SUMOReal offset) {
    std::vector<SUMOReal> intersections = geom1.intersectsAtLengths2D(geom2);
    SUMOReal result = intersections[0];
    for (std::vector<SUMOReal>::iterator it = intersections.begin() + 1; it != intersections.end(); ++it) {
        if (fabs(*it - offset) < fabs(result - offset)) {
            result = *it;
        }
    }
    return result;
}


PositionVector
NBNodeShapeComputer::getSmoothCorner(PositionVector begShape, PositionVector endShape,
                                     const Position& begPoint, const Position& endPoint, int cornerDetail) {
    PositionVector ret;
    if (cornerDetail > 0) {
        begShape = begShape.reverse();
        begShape[-1] = begPoint;
        endShape[0] = endPoint;
        PositionVector curve = myNode.computeSmoothShape(begShape, endShape, cornerDetail + 2, false, 25, 25);
        if (curve.size() > 2) {
            curve.eraseAt(0);
            curve.eraseAt(-1);
            ret = curve;
        }
    }
    return ret;
}

void
NBNodeShapeComputer::joinSameDirectionEdges(std::map<NBEdge*, std::set<NBEdge*> >& same,
        GeomsMap& geomsCCW,
        GeomsMap& geomsCW) {
    EdgeVector::const_iterator i, j;
    // compute boundary lines and extend it by 100m
    for (i = myNode.myAllEdges.begin(); i != myNode.myAllEdges.end() - 1; i++) {
        // store current edge's boundary as current ccw/cw boundary
        try {
            geomsCCW[*i] = (*i)->getCCWBoundaryLine(myNode);
        } catch (InvalidArgument& e) {
            WRITE_WARNING(std::string("While computing intersection geometry: ") + std::string(e.what()));
            geomsCCW[*i] = (*i)->getGeometry();
        }
        try {
            geomsCW[*i] = (*i)->getCWBoundaryLine(myNode);
        } catch (InvalidArgument& e) {
            WRITE_WARNING(std::string("While computing intersection geometry: ") + std::string(e.what()));
            geomsCW[*i] = (*i)->getGeometry();
        }
        // extend the boundary by extroplating it by 100m
        PositionVector g1 =
            myNode.hasIncoming(*i)
            ? (*i)->getCCWBoundaryLine(myNode)
            : (*i)->getCWBoundaryLine(myNode);
        Line l1 = g1.lineAt(0);
        Line tmp = geomsCCW[*i].lineAt(0);
        tmp.extrapolateBy2D(100);
        geomsCCW[*i].replaceAt(0, tmp.p1());
        tmp = geomsCW[*i].lineAt(0);
        tmp.extrapolateBy2D(100);
        geomsCW[*i].replaceAt(0, tmp.p1());
        //
        for (j = i + 1; j != myNode.myAllEdges.end(); j++) {
            geomsCCW[*j] = (*j)->getCCWBoundaryLine(myNode);
            geomsCW[*j] = (*j)->getCWBoundaryLine(myNode);
            PositionVector g2 =
                myNode.hasIncoming(*j)
                ? (*j)->getCCWBoundaryLine(myNode)
                : (*j)->getCWBoundaryLine(myNode);
            Line l2 = g2.lineAt(0);
            tmp = geomsCCW[*j].lineAt(0);
            tmp.extrapolateBy2D(100);
            geomsCCW[*j].replaceAt(0, tmp.p1());
            tmp = geomsCW[*j].lineAt(0);
            tmp.extrapolateBy2D(100);
            geomsCW[*j].replaceAt(0, tmp.p1());
        }
    }
    // compute same (edges where an intersection doesn't work well
    // (always check an edge and its cw neightbor)
    // distance to look ahead for a misleading angle
    const SUMOReal angleChangeLookahead = 35;
    EdgeSet foundOpposite;
    for (i = myNode.myAllEdges.begin(); i != myNode.myAllEdges.end() - 1; i++) {
        EdgeVector::const_iterator j = i + 1;
        if (j == myNode.myAllEdges.end()) {
            j = myNode.myAllEdges.begin();
        }
        const bool incoming = (*i)->getToNode() == &myNode;
        const bool incoming2 = (*j)->getToNode() == &myNode;
        const Position positionAtNode = (*i)->getGeometry()[incoming ? -1 : 0];
        const Position positionAtNode2 = (*j)->getGeometry()[incoming2 ? -1 : 0];
        PositionVector g1 = incoming ? (*i)->getCCWBoundaryLine(myNode) : (*i)->getCWBoundaryLine(myNode);
        PositionVector g2 = incoming ? (*j)->getCCWBoundaryLine(myNode) : (*j)->getCWBoundaryLine(myNode);
        Line l1 = g1.lineAt(0);
        Line l2 = g2.lineAt(0);
        const SUMOReal angle1further = (g1.size() > 2 && l1.length2D() < angleChangeLookahead ?
                g1.lineAt(1).atan2DegreeAngle() : l1.atan2DegreeAngle());
        const SUMOReal angle2further = (g2.size() > 2 && l2.length2D() < angleChangeLookahead ?
                g2.lineAt(1).atan2DegreeAngle() : l2.atan2DegreeAngle());
        const SUMOReal angleDiff = NBHelpers::relAngle(l1.atan2DegreeAngle(), l2.atan2DegreeAngle());
        const SUMOReal angleDiffFurther = NBHelpers::relAngle(angle1further, angle2further);
        const bool ambiguousGeometry = ((angleDiff > 0 && angleDiffFurther < 0) || (angleDiff < 0 && angleDiffFurther > 0));
        const bool differentDirs = (incoming != incoming2);
        //if (ambiguousGeometry) {
        //    @todo: this warning would be helpful in many cases. However, if angle and angleFurther jump between 179 and -179 it is misleading
        //    WRITE_WARNING("Ambigous angles at node '" + myNode.getID() + "' for edges '" + (*i)->getID() + "' and '" + (*j)->getID() + "'.");
        //}
        if (fabs(angleDiff) < 20) {
            const bool isOpposite = differentDirs && foundOpposite.count(*i) == 0;
            if (isOpposite) {
                foundOpposite.insert(*i);
                foundOpposite.insert(*j);
            }
            if (isOpposite || ambiguousGeometry || badIntersection(*i, *j, geomsCW[*i], geomsCCW[*j], fabs(angleDiff), 100)) {
                // maintain equivalence relation for all members of the equivalence class
                for (std::set<NBEdge*>::iterator k = same[*i].begin(); k != same[*i].end(); ++k) {
                    if (*j != *k) {
                        same[*k].insert(*j);
                        same[*j].insert(*k);
                    }
                }
                for (std::set<NBEdge*>::iterator k = same[*j].begin(); k != same[*j].end(); ++k) {
                    if (*i != *k) {
                        same[*k].insert(*i);
                        same[*i].insert(*k);
                    }
                }
                same[*i].insert(*j);
                same[*j].insert(*i);
            }
        }
    }
}


bool
NBNodeShapeComputer::badIntersection(const NBEdge* e1, const NBEdge* e2, 
        const PositionVector& e1cw, const PositionVector& e2ccw, 
        SUMOReal absAngleDiff, SUMOReal distance) {
    // check whether the two edges are on top of each other. In that case they should be joined
    // also, if they never touch along their common length
    const SUMOReal commonLength = MIN3(distance, e1->getGeometry().length(), e2->getGeometry().length());
    PositionVector geom1 = e1->getGeometry();
    PositionVector geom2 = e2->getGeometry();
    // shift to make geom the centerline of the edge regardless of spreadtype
    if (e1->getLaneSpreadFunction() == LANESPREAD_RIGHT) {
        geom1.move2side(e1->getTotalWidth() / 2);
    }
    if (e2->getLaneSpreadFunction() == LANESPREAD_RIGHT) {
        geom2.move2side(e2->getTotalWidth() / 2);
    }
    // always let geometry start at myNode
    if (e1->getToNode() == &myNode) {
        geom1 = geom1.reverse();
    }
    if (e2->getToNode() == &myNode) {
        geom2 = geom2.reverse();
    }
    geom1 = geom1.getSubpart2D(0, commonLength);
    geom2 = geom2.getSubpart2D(0, commonLength);
    std::vector<SUMOReal> distances = geom1.distances(geom2, true);
    const SUMOReal minDistanceThreshold = (e1->getTotalWidth() + e2->getTotalWidth()) / 2 + POSITION_EPS;
    const SUMOReal minDist = VectorHelper<SUMOReal>::minValue(distances);
    const SUMOReal maxDist = VectorHelper<SUMOReal>::maxValue(distances);
    const bool onTop = maxDist - POSITION_EPS < minDistanceThreshold;
    const bool curvingTowards = geom1[0].distanceTo2D(geom2[0]) > minDistanceThreshold && minDist < minDistanceThreshold;
    const bool intersects = e1cw.intersects(e2ccw);
    return onTop || curvingTowards || !intersects;
}


EdgeVector
NBNodeShapeComputer::computeUniqueDirectionList(
    std::map<NBEdge*, std::set<NBEdge*> >& same,
    GeomsMap& geomsCCW,
    GeomsMap& geomsCW,
    std::map<NBEdge*, NBEdge*>& ccwBoundary,
    std::map<NBEdge*, NBEdge*>& cwBoundary) {
    EdgeVector newAll = myNode.myAllEdges;
    std::set<NBEdge*>::const_iterator j;
    EdgeVector::iterator i2;
    std::map<NBEdge*, EdgeVector >::iterator k;
    bool changed = true;
    while (changed) {
        changed = false;
        for (i2 = newAll.begin(); !changed && i2 != newAll.end();) {
            std::set<NBEdge*> other = same[*i2];
            for (j = other.begin(); j != other.end(); ++j) {
                EdgeVector::iterator k = find(newAll.begin(), newAll.end(), *j);
                if (k != newAll.end()) {
                    if (myNode.hasIncoming(*i2)) {
                        if (myNode.hasIncoming(*j)) {} else {
                            geomsCW[*i2] = geomsCW[*j];
                            cwBoundary[*i2] = *j;
                            computeSameEnd(geomsCW[*i2], geomsCCW[*i2]);
                        }
                    } else {
                        if (myNode.hasIncoming(*j)) {
                            ccwBoundary[*i2] = *j;
                            geomsCCW[*i2] = geomsCCW[*j];
                            computeSameEnd(geomsCW[*i2], geomsCCW[*i2]);
                        } else {}
                    }
                    newAll.erase(k);
                    changed = true;
                }
            }
            if (!changed) {
                ++i2;
            }
        }
    }
    return newAll;
}


void
NBNodeShapeComputer::initNeighbors(const EdgeVector& edges, const EdgeVector::const_iterator& current,
                                   GeomsMap& geomsCW,
                                   GeomsMap& geomsCCW,
                                   EdgeVector::const_iterator& cwi,
                                   EdgeVector::const_iterator& ccwi,
                                   SUMOReal& cad,
                                   SUMOReal& ccad) {
    const SUMOReal twoPI = (SUMOReal)(2 * M_PI);
    cwi = current;
    cwi++;
    if (cwi == edges.end()) {
        std::advance(cwi, -((int)edges.size())); // set to edges.begin();
    }
    ccwi = current;
    if (ccwi == edges.begin()) {
        std::advance(ccwi, edges.size() - 1); // set to edges.end() - 1;
    } else {
        ccwi--;
    }

    const SUMOReal angleCurCCW = geomsCCW[*current].lineAt(0).atan2PositiveAngle();
    const SUMOReal angleCurCW = geomsCW[*current].lineAt(0).atan2PositiveAngle();
    const SUMOReal angleCCW = geomsCW[*ccwi].lineAt(0).atan2PositiveAngle();
    const SUMOReal angleCW = geomsCCW[*cwi].lineAt(0).atan2PositiveAngle();
    if (angleCurCCW > angleCCW) {
        ccad = angleCurCCW - angleCCW;
    } else {
        ccad = twoPI - (angleCCW - angleCurCCW);
    }

    if (angleCurCW > angleCW) {
        cad = twoPI - (angleCurCW - angleCW);
    } else {
        cad = angleCW - angleCurCW;
    }

    if (ccad < 0) {
        ccad += twoPI;
    }
    if (ccad > twoPI) {
        ccad -= twoPI;
    }
    if (cad < 0) {
        cad += twoPI;
    }
    if (cad > twoPI) {
        cad -= twoPI;
    }
}



PositionVector
NBNodeShapeComputer::computeNodeShapeSmall() {
    PositionVector ret;
    EdgeVector::const_iterator i;
    for (i = myNode.myAllEdges.begin(); i != myNode.myAllEdges.end(); i++) {
        // compute crossing with normal
        Line edgebound1 = (*i)->getCCWBoundaryLine(myNode).lineAt(0);
        Line edgebound2 = (*i)->getCWBoundaryLine(myNode).lineAt(0);
        Line cross(edgebound1);
        cross.rotateAtP1(M_PI / 2.);
        cross.add(myNode.getPosition() - cross.p1());
        cross.extrapolateBy2D(500);
        edgebound1.extrapolateBy2D(500);
        edgebound2.extrapolateBy2D(500);
        if (cross.intersects(edgebound1)) {
            Position np = cross.intersectsAt(edgebound1);
            np.set(np.x(), np.y(), myNode.getPosition().z());
            ret.push_back_noDoublePos(np);
        }
        if (cross.intersects(edgebound2)) {
            Position np = cross.intersectsAt(edgebound2);
            np.set(np.x(), np.y(), myNode.getPosition().z());
            ret.push_back_noDoublePos(np);
        }
    }
    return ret;
}



/****************************************************************************/
