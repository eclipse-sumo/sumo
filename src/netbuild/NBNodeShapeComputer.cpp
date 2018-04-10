/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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

//#define DEBUG_NODE_SHAPE
#define DEBUGCOND (myNode.getID() == "disabled")

// ===========================================================================
// method definitions
// ===========================================================================
NBNodeShapeComputer::NBNodeShapeComputer(const NBNode& node)
    : myNode(node) {}


NBNodeShapeComputer::~NBNodeShapeComputer() {}


PositionVector
NBNodeShapeComputer::compute() {
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
        double maxAngle = 0.;
        for (EdgeVector::const_iterator i = incoming.begin(); i != incoming.end(); ++i) {
            double ia = (*i)->getAngleAtNode(&myNode);
            for (EdgeVector::const_iterator j = outgoing.begin(); j != outgoing.end(); ++j) {
                double oa = (*j)->getAngleAtNode(&myNode);
                double ad = GeomHelper::getMinAngleDiff(ia, oa);
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
    assert(l1[0].distanceTo2D(l1[1]) >= 100.);
    assert(l2[0].distanceTo2D(l2[1]) >= 100.);
    PositionVector tmp;
    tmp.push_back(PositionVector::positionAtOffset2D(l1[0], l1[1], 100));
    tmp.push_back(l1[1]);
    tmp[1].sub(tmp[0]);
    tmp[1].set(-tmp[1].y(), tmp[1].x());
    tmp[1].add(tmp[0]);
    tmp.extrapolate2D(100);
    if (l2.intersects(tmp[0], tmp[1])) {
        const double offset = l2.intersectsAtLengths2D(tmp)[0];
        if (l2.length2D() - offset > POSITION_EPS) {
            PositionVector tl2 = l2.getSubpart2D(offset, l2.length2D());
            tl2.extrapolate2D(100);
            l2.erase(l2.begin(), l2.begin() + (l2.size() - tl2.size()));
            l2[0] = tl2[0];
        }
    }
}


PositionVector
NBNodeShapeComputer::computeNodeShapeDefault(bool simpleContinuation) {
    // if we have less than two edges, we can not compute the node's shape this way
    if (myNode.myAllEdges.size() < 2) {
        return PositionVector();
    }
    // magic values
    const OptionsCont& oc = OptionsCont::getOptions();
    const bool defaultRadius = myNode.getRadius() == NBNode::UNSPECIFIED_RADIUS;
    const double radius = (defaultRadius ? oc.getFloat("default.junctions.radius") : myNode.getRadius());
    const int cornerDetail = oc.getInt("junctions.corner-detail");
    const double sCurveStretch = oc.getFloat("junctions.scurve-stretch");
    const bool rectangularCut = oc.getBool("rectangular-lane-cut");
    const bool openDriveOutput = oc.isSet("opendrive-output");

    // Extend geometries to move the stop line forward.
    // In OpenDrive the junction starts whenever the geometry changes. Stop
    // line information is not given or ambiguous (sign positions at most)
    // In SUMO, stop lines are where the junction starts. This is computed
    // heuristically from intersecting the junctions roads geometries.
    const double advanceStopLine = oc.exists("opendrive-files") && oc.isSet("opendrive-files") ? oc.getFloat("opendrive.advance-stopline") : 0;


#ifdef DEBUG_NODE_SHAPE
    if (DEBUGCOND) {
        std::cout << "\ncomputeNodeShapeDefault node " << myNode.getID() << " simple=" << simpleContinuation << " radius=" << radius << "\n";
    }
#endif

    // initialise
    EdgeVector::const_iterator i;
    // edges located in the value-vector have the same direction as the key edge
    std::map<NBEdge*, std::set<NBEdge*> > same;
    // the counter-clockwise boundary of the edge regarding possible same-direction edges
    GeomsMap geomsCCW;
    // the clockwise boundary of the edge regarding possible same-direction edges
    GeomsMap geomsCW;
    // check which edges are parallel
    joinSameDirectionEdges(same, geomsCCW, geomsCW);
    // compute unique direction list
    EdgeVector newAll = computeUniqueDirectionList(same, geomsCCW, geomsCW);
    // if we have only two "directions", let's not compute the geometry using this method
    if (newAll.size() < 2) {
        return PositionVector();
    }

    // All geoms are outoing from myNode.
    // for every direction in newAll we compute the offset at which the
    // intersection ends and the edge starts. This value is saved in 'distances'
    // If the geometries need to be extended to get an intersection, this is
    // recorded in 'myExtended'
    std::map<NBEdge*, double> distances;
    std::map<NBEdge*, bool> myExtended;

    for (i = newAll.begin(); i != newAll.end(); ++i) {
        EdgeVector::const_iterator cwi = i;
        EdgeVector::const_iterator ccwi = i;
        double ccad;
        double cad;
        initNeighbors(newAll, i, geomsCW, geomsCCW, cwi, ccwi, cad, ccad);
        assert(geomsCCW.find(*i) != geomsCCW.end());
        assert(geomsCW.find(*ccwi) != geomsCW.end());
        assert(geomsCW.find(*cwi) != geomsCW.end());

        // there are only 2 directions and they are almost parallel
        if (*cwi == *ccwi &&
                (
                    // no change in lane numbers, even low angles still give a good intersection
                    (simpleContinuation && fabs(ccad - cad) < (double) 0.1)
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
#ifdef DEBUG_NODE_SHAPE
                if (DEBUGCOND) {
                    std::cout << " extended: p=" << p << " angle=" << (ccad - cad) << "\n";
                }
#endif
            } else {
                p = geomsCCW[*ccwi][0];
                p.add(geomsCW[*ccwi][0]);
                p.add(geomsCCW[*i][0]);
                p.add(geomsCW[*i][0]);
                p.mul(0.25);
#ifdef DEBUG_NODE_SHAPE
                if (DEBUGCOND) {
                    std::cout << " unextended: p=" << p << " angle=" << (ccad - cad) << "\n";
                }
#endif
            }
            // ... compute the distance to this point ...
            double dist = MAX2(
                              geomsCCW[*i].nearest_offset_to_point2D(p),
                              geomsCW[*i].nearest_offset_to_point2D(p));
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
#ifdef DEBUG_NODE_SHAPE
                if (DEBUGCOND) {
                    std::cout << " extending (dist=" << dist << ")\n";
                }
#endif
            } else {
                if (!simpleContinuation) {
                    dist += radius;
                } else {
                    // if the angles change, junction should have some size to avoid degenerate shape
                    double radius2 = fabs(ccad - cad) * (*i)->getNumLanes();
                    if (radius2 > NUMERICAL_EPS || openDriveOutput) {
                        radius2 = MAX2(0.15, radius2);
                    }
                    dist += radius2;
#ifdef DEBUG_NODE_SHAPE
                    if (DEBUGCOND) {
                        std::cout << " using radius=" << fabs(ccad - cad) * (*i)->getNumLanes() << " ccad=" << ccad << " cad=" << cad << "\n";
                    }
#endif
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
#ifdef DEBUG_NODE_SHAPE
            if (DEBUGCOND) {
                std::cout << " i=" << (*i)->getID() << " neigh=" << (*ccwi)->getID() << " neigh2=" << (*cwi)->getID() << "\n";
            }
#endif
            if (!simpleContinuation) {
                if (currGeom.intersects(neighGeom)) {
                    distances[*i] = radius + closestIntersection(currGeom, neighGeom, 100);
#ifdef DEBUG_NODE_SHAPE
                    if (DEBUGCOND) {
                        std::cout << "   neigh intersects dist=" << distances[*i] << " currGeom=" << currGeom << " neighGeom=" << neighGeom << "\n";
                    }
#endif
                    if (*cwi != *ccwi && currGeom2.intersects(neighGeom2)) {
                        // also use the second intersection point
                        // but prevent very large node shapes
                        const double farAngleDist = ccwCloser ? cad : ccad;
                        double a1 = distances[*i];
                        double a2 = radius + closestIntersection(currGeom2, neighGeom2, 100);
#ifdef DEBUG_NODE_SHAPE
                        if (DEBUGCOND) {
                            std::cout << "      neigh2 also intersects a1=" << a1 << " a2=" << a2 << " ccad=" << RAD2DEG(ccad) << " cad=" << RAD2DEG(cad) << " dist[cwi]=" << distances[*cwi] << " dist[ccwi]=" << distances[*ccwi] << " farAngleDist=" << RAD2DEG(farAngleDist) << " currGeom2=" << currGeom2 << " neighGeom2=" << neighGeom2 << "\n";
                        }
#endif
                        //if (RAD2DEG(farAngleDist) < 175) {
                        //    distances[*i] = MAX2(a1, MIN2(a2, a1 + 180 - RAD2DEG(farAngleDist)));
                        //}
                        if (ccad > DEG2RAD(90. + 45.) && cad > DEG2RAD(90. + 45.)) {
                            // do nothing.
                        } else if (farAngleDist < DEG2RAD(135) || (fabs(RAD2DEG(farAngleDist) - 180) > 1 && fabs(a2 - a1) < 10)) {
                            distances[*i] = MAX2(a1, a2);
                        }
#ifdef DEBUG_NODE_SHAPE
                        if (DEBUGCOND) {
                            std::cout << "   a1=" << a1 << " a2=" << a2 << " dist=" << distances[*i] << "\n";
                        }
#endif
                    }
                } else {
                    if (*cwi != *ccwi && currGeom2.intersects(neighGeom2)) {
                        distances[*i] = radius + currGeom2.intersectsAtLengths2D(neighGeom2)[0];
#ifdef DEBUG_NODE_SHAPE
                        if (DEBUGCOND) {
                            std::cout << "   neigh2 intersects dist=" << distances[*i] << " currGeom2=" << currGeom2 << " neighGeom2=" << neighGeom2 << "\n";
                        }
#endif
                    } else {
                        distances[*i] = 100 + radius;
#ifdef DEBUG_NODE_SHAPE
                        if (DEBUGCOND) {
                            std::cout << "   no intersects dist=" << distances[*i]  << " currGeom=" << currGeom << " neighGeom=" << neighGeom << " currGeom2=" << currGeom2 << " neighGeom2=" << neighGeom2 << "\n";
                        }
#endif
                    }
                }
            } else {
                if (currGeom.intersects(neighGeom)) {
                    distances[*i] = currGeom.intersectsAtLengths2D(neighGeom)[0];
                } else {
                    distances[*i] = (double) 100.;
                }
            }
        }
        if (defaultRadius && sCurveStretch > 0) {
            double sCurveWidth = myNode.getDisplacementError();
            if (sCurveWidth > 0) {
                const double sCurveRadius = radius + sCurveWidth / SUMO_const_laneWidth * sCurveStretch * pow((*i)->getSpeed(), 2 + sCurveStretch) / 1000;
                const double stretch = 100 + sCurveRadius - distances[*i];
                if (stretch > 0) {
                    distances[*i] += stretch;
                    // fixate extended geometry for repeated computation
                    const double shorten = distances[*i] - 100;
                    (*i)->shortenGeometryAtNode(&myNode, shorten);
                    for (std::set<NBEdge*>::iterator k = same[*i].begin(); k != same[*i].end(); ++k) {
                        (*k)->shortenGeometryAtNode(&myNode, shorten);
                    }
#ifdef DEBUG_NODE_SHAPE
                    if (DEBUGCOND) {
                        std::cout << "   stretching junction: sCurveWidth=" << sCurveWidth << " sCurveRadius=" << sCurveRadius << " stretch=" << stretch << " dist=" << distances[*i]  << "\n";
                    }
#endif
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
    // prevent inverted node shapes
    // (may happen with near-parallel edges)
    const double minDistSum = 2 * (100 + radius);
    for (i = newAll.begin(); i != newAll.end(); ++i) {
        if (distances[*i] < 100 && (*i)->hasDefaultGeometryEndpointAtNode(&myNode)) {
            for (EdgeVector::const_iterator j = newAll.begin(); j != newAll.end(); ++j) {
                if (distances[*j] > 100 && (*j)->hasDefaultGeometryEndpointAtNode(&myNode) && distances[*i] + distances[*j] < minDistSum) {
                    const double angleDiff = fabs(NBHelpers::relAngle((*i)->getAngleAtNode(&myNode), (*j)->getAngleAtNode(&myNode)));
                    if (angleDiff > 160 || angleDiff < 20) {
#ifdef DEBUG_NODE_SHAPE
                        if (DEBUGCOND) {
                            std::cout << "   increasing dist for i=" << (*i)->getID() << " because of j=" << (*j)->getID() << " jDist=" << distances[*j]
                                      << "  oldI=" << distances[*i] << " newI=" << minDistSum - distances[*j]
                                      << " angleDiff=" << angleDiff
                                      << " geomI=" << (*i)->getGeometry() << " geomJ=" << (*j)->getGeometry() << "\n";
                        }
#endif
                        distances[*i] = minDistSum - distances[*j];
                    }
                }
            }
        }
    }


    // build
    PositionVector ret;
    for (i = newAll.begin(); i != newAll.end(); ++i) {
        const PositionVector& ccwBound = geomsCCW[*i];
        const PositionVector& cwBound = geomsCW[*i];
        //double offset = MIN3(distances[*i], cwBound.length2D() - POSITION_EPS, ccwBound.length2D() - POSITION_EPS);
        double offset = distances[*i];
        if (!(*i)->hasDefaultGeometryEndpointAtNode(&myNode)) {
            // for non geometry-endpoints, only shorten but never extend the geometry
            if (advanceStopLine > 0 && offset < 100) {
#ifdef DEBUG_NODE_SHAPE
                std::cout << " i=" << (*i)->getID() << " offset=" << offset << " advanceStopLine=" << advanceStopLine << "\n";
#endif
                // fixate extended geometry for repeated computation
                (*i)->extendGeometryAtNode(&myNode, advanceStopLine);
                for (std::set<NBEdge*>::iterator k = same[*i].begin(); k != same[*i].end(); ++k) {
                    (*k)->extendGeometryAtNode(&myNode, advanceStopLine);
                }
            }
            offset = MAX2(100.0 - advanceStopLine, offset);
        }
        if (offset == -1) {
            WRITE_WARNING("Fixing offset for edge '" + (*i)->getID() + "' at node '" + myNode.getID() + ".");
            offset = (double) - .1;
        }
        Position p = ccwBound.positionAtOffset2D(offset);
        p.setz(myNode.getPosition().z());
        if (i != newAll.begin()) {
            ret.append(getSmoothCorner(geomsCW[*(i - 1)].reverse(), ccwBound, ret[-1], p, cornerDetail));
        }
        ret.push_back_noDoublePos(p);
        //
        Position p2 = cwBound.positionAtOffset2D(offset);
        p2.setz(myNode.getPosition().z());
        ret.push_back_noDoublePos(p2);
#ifdef DEBUG_NODE_SHAPE
        if (DEBUGCOND) {
            std::cout << "   build stopLine for i=" << (*i)->getID() << " offset=" << offset << " dist=" << distances[*i] << " cwLength=" << cwBound.length2D() << " ccwLength=" << ccwBound.length2D() << " p=" << p << " p2=" << p2 << " ccwBound=" <<  ccwBound << " cwBound=" << cwBound << "\n";
        }
#endif
        (*i)->setNodeBorder(&myNode, p, p2, rectangularCut);
        for (std::set<NBEdge*>::iterator k = same[*i].begin(); k != same[*i].end(); ++k) {
            (*k)->setNodeBorder(&myNode, p, p2, rectangularCut);
        }
    }
    // final curve segment
    ret.append(getSmoothCorner(geomsCW[*(newAll.end() - 1)], geomsCCW[*newAll.begin()], ret[-1], ret[0], cornerDetail));
    return ret;
}


double
NBNodeShapeComputer::closestIntersection(const PositionVector& geom1, const PositionVector& geom2, double offset) {
    std::vector<double> intersections = geom1.intersectsAtLengths2D(geom2);
    double result = intersections[0];
    for (std::vector<double>::iterator it = intersections.begin() + 1; it != intersections.end(); ++it) {
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
            curve.erase(curve.begin());
            curve.pop_back();
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
        geomsCCW[*i].extrapolate2D(100, true);
        geomsCW[*i].extrapolate2D(100, true);
        //
        for (j = i + 1; j != myNode.myAllEdges.end(); j++) {
            geomsCCW[*j] = (*j)->getCCWBoundaryLine(myNode);
            geomsCW[*j] = (*j)->getCWBoundaryLine(myNode);
            PositionVector g2 =
                myNode.hasIncoming(*j)
                ? (*j)->getCCWBoundaryLine(myNode)
                : (*j)->getCWBoundaryLine(myNode);
            geomsCCW[*j].extrapolate2D(100, true);
            geomsCW[*j].extrapolate2D(100, true);
        }
    }
    // compute same (edges where an intersection doesn't work well
    // (always check an edge and its cw neightbor)
    // distance to look ahead for a misleading angle
    const double angleChangeLookahead = 35;
    EdgeSet foundOpposite;
    for (i = myNode.myAllEdges.begin(); i != myNode.myAllEdges.end(); i++) {
        EdgeVector::const_iterator j;
        if (i == myNode.myAllEdges.end() - 1) {
            j = myNode.myAllEdges.begin();
        } else {
            j = i + 1;
        }
        const bool incoming = (*i)->getToNode() == &myNode;
        const bool incoming2 = (*j)->getToNode() == &myNode;
        const Position positionAtNode = (*i)->getGeometry()[incoming ? -1 : 0];
        const Position positionAtNode2 = (*j)->getGeometry()[incoming2 ? -1 : 0];
        const PositionVector g1 = incoming ? (*i)->getCCWBoundaryLine(myNode) : (*i)->getCWBoundaryLine(myNode);
        const PositionVector g2 = incoming ? (*j)->getCCWBoundaryLine(myNode) : (*j)->getCWBoundaryLine(myNode);
        const double angle1further = (g1.size() > 2 && g1[0].distanceTo2D(g1[1]) < angleChangeLookahead ?
                                      g1.angleAt2D(1) : g1.angleAt2D(0));
        const double angle2further = (g2.size() > 2 && g2[0].distanceTo2D(g2[1]) < angleChangeLookahead ?
                                      g2.angleAt2D(1) : g2.angleAt2D(0));
        const double angleDiff = GeomHelper::angleDiff(g1.angleAt2D(0), g2.angleAt2D(0));
        const double angleDiffFurther = GeomHelper::angleDiff(angle1further, angle2further);
        const bool ambiguousGeometry = ((angleDiff > 0 && angleDiffFurther < 0) || (angleDiff < 0 && angleDiffFurther > 0));
        const bool differentDirs = (incoming != incoming2);
        //if (ambiguousGeometry) {
        //    @todo: this warning would be helpful in many cases. However, if angle and angleFurther jump between 179 and -179 it is misleading
        //    WRITE_WARNING("Ambigous angles at junction '" + myNode.getID() + "' for edges '" + (*i)->getID() + "' and '" + (*j)->getID() + "'.");
        //}
        if (fabs(angleDiff) < DEG2RAD(20)) {
            const bool isOpposite = differentDirs && foundOpposite.count(*i) == 0;
            if (isOpposite) {
                foundOpposite.insert(*i);
                foundOpposite.insert(*j);
            }
            if (isOpposite || ambiguousGeometry || badIntersection(*i, *j, geomsCW[*i], geomsCCW[*j], 100)) {
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
#ifdef DEBUG_NODE_SHAPE
                if (DEBUGCOND) {
                    std::cout << "   joinedSameDirectionEdges " << (*i)->getID() << "   " << (*j)->getID() << " isOpposite=" << isOpposite << " ambiguousGeometry=" << ambiguousGeometry << "\n";
                }
#endif
            }
        }
    }
}


bool
NBNodeShapeComputer::badIntersection(const NBEdge* e1, const NBEdge* e2,
                                     const PositionVector& e1cw, const PositionVector& e2ccw,
                                     double distance) {
    // check whether the two edges are on top of each other. In that case they should be joined
    // also, if they never touch along their common length
    const double commonLength = MIN3(distance, e1->getGeometry().length(), e2->getGeometry().length());
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
    std::vector<double> distances = geom1.distances(geom2, true);
    const double minDistanceThreshold = (e1->getTotalWidth() + e2->getTotalWidth()) / 2 + POSITION_EPS;
    const double minDist = VectorHelper<double>::minValue(distances);
    const double maxDist = VectorHelper<double>::maxValue(distances);
    const bool onTop = maxDist - POSITION_EPS < minDistanceThreshold;
    const bool curvingTowards = geom1[0].distanceTo2D(geom2[0]) > minDistanceThreshold && minDist < minDistanceThreshold;
    const bool intersects = e1cw.intersects(e2ccw);
    return onTop || curvingTowards || !intersects;
}


EdgeVector
NBNodeShapeComputer::computeUniqueDirectionList(
    std::map<NBEdge*, std::set<NBEdge*> >& same,
    GeomsMap& geomsCCW,
    GeomsMap& geomsCW) {
    // store relationships
    EdgeVector newAll = myNode.myAllEdges;
    bool changed = true;
    while (changed) {
        changed = false;
        for (EdgeVector::iterator i2 = newAll.begin(); i2 != newAll.end(); ++i2) {
            std::set<NBEdge*> other = same[*i2];
            for (std::set<NBEdge*>::const_iterator j = other.begin(); j != other.end(); ++j) {
                EdgeVector::iterator k = find(newAll.begin(), newAll.end(), *j);
                if (k != newAll.end()) {
                    if (myNode.hasIncoming(*i2)) {
                        if (!myNode.hasIncoming(*j)) {
                            geomsCW[*i2] = geomsCW[*j];
                            computeSameEnd(geomsCW[*i2], geomsCCW[*i2]);
                        }
                    } else {
                        if (myNode.hasIncoming(*j)) {
                            geomsCCW[*i2] = geomsCCW[*j];
                            computeSameEnd(geomsCW[*i2], geomsCCW[*i2]);
                        }
                    }
                    newAll.erase(k);
                    changed = true;
                }
            }
            if (changed) {
                break;
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
                                   double& cad,
                                   double& ccad) {
    const double twoPI = (double)(2 * M_PI);
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

    const double angleCurCCW = geomsCCW[*current].angleAt2D(0);
    const double angleCurCW = geomsCW[*current].angleAt2D(0);
    const double angleCCW = geomsCW[*ccwi].angleAt2D(0);
    const double angleCW = geomsCCW[*cwi].angleAt2D(0);
    ccad = angleCCW - angleCurCCW;
    while (ccad < 0.) {
        ccad += twoPI;
    }
    cad = angleCurCW - angleCW;
    while (cad < 0.) {
        cad += twoPI;
    }
}



PositionVector
NBNodeShapeComputer::computeNodeShapeSmall() {
#ifdef DEBUG_NODE_SHAPE
    if (DEBUGCOND) {
        std::cout << "computeNodeShapeSmall node=" << myNode.getID() << "\n";
    }
#endif
    PositionVector ret;
    EdgeVector::const_iterator i;
    for (i = myNode.myAllEdges.begin(); i != myNode.myAllEdges.end(); i++) {
        // compute crossing with normal
        PositionVector edgebound1 = (*i)->getCCWBoundaryLine(myNode).getSubpartByIndex(0, 2);
        PositionVector edgebound2 = (*i)->getCWBoundaryLine(myNode).getSubpartByIndex(0, 2);
        Position delta = edgebound1[1] - edgebound1[0];
        delta.set(-delta.y(), delta.x()); // rotate 90 degrees
        PositionVector cross(myNode.getPosition(), myNode.getPosition() + delta);
        cross.extrapolate2D(500);
        edgebound1.extrapolate2D(500);
        edgebound2.extrapolate2D(500);
        if (cross.intersects(edgebound1)) {
            Position np = cross.intersectionPosition2D(edgebound1);
            np.set(np.x(), np.y(), myNode.getPosition().z());
            ret.push_back_noDoublePos(np);
        }
        if (cross.intersects(edgebound2)) {
            Position np = cross.intersectionPosition2D(edgebound2);
            np.set(np.x(), np.y(), myNode.getPosition().z());
            ret.push_back_noDoublePos(np);
        }
        (*i)->resetNodeBorder(&myNode);
    }
    return ret;
}



/****************************************************************************/
