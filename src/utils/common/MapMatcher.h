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
/// @file    MapMatcher.h
/// @author  Jakob Erdmann
/// @date    Thu, 7 March 2024
///
// Utility functions for matching locations to edges (during route parsing)
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/geom/GeoConvHelper.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include "NamedRTree.h"
#include "SUMOVehicleClass.h"
#include "MsgHandler.h"

#define JUNCTION_TAZ_MISSING_HELP "\nSet option '--junction-taz' or load a TAZ-file"

// ===========================================================================
// class declarations
// ===========================================================================

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MapMatcher
 * @brief Provides utility functions for matching locations to edges (during route parsing)
 */

template<class E, class L, class N>
class MapMatcher {
protected:
    MapMatcher(bool matchJunctions, double matchDistance, MsgHandler* errorOutput):
        myLaneTree(nullptr),
        myMapMatchJunctions(matchJunctions),
        myMapMatchingDistance(matchDistance),
        myErrorOutput(errorOutput) {}

    virtual ~MapMatcher() {
        delete myLaneTree;
    }

    void parseGeoEdges(const PositionVector& positions, bool geo, SUMOVehicleClass vClass,
                       std::vector<const E*>& into, const std::string& rid, bool isFrom, bool& ok) {
        if (geo && !GeoConvHelper::getFinal().usingGeoProjection()) {
            WRITE_ERROR(TL("Cannot convert geo-positions because the network has no geo-reference"));
            return;
        }
        for (Position pos : positions) {
            Position orig = pos;
            if (geo) {
                GeoConvHelper::getFinal().x2cartesian_const(pos);
            }
            double dist = MIN2(10.0, myMapMatchingDistance);
            const L* best = getClosestLane(pos, vClass, dist);
            while (best == nullptr && dist < myMapMatchingDistance) {
                dist = MIN2(dist * 2, myMapMatchingDistance);
                best = getClosestLane(pos, vClass, dist);
            }
            if (best == nullptr) {
                myErrorOutput->inform("No edge found near position " + toString(orig, geo ? gPrecisionGeo : gPrecision) + " within the route " + rid + ".");
                ok = false;
            } else {
                const E* bestEdge = &best->getEdge();
                while (bestEdge->isInternal()) {
                    bestEdge = bestEdge->getSuccessors().front();
                }
                if (myMapMatchJunctions) {
                    bestEdge = getJunctionTaz(pos, bestEdge, vClass, isFrom);
                    if (bestEdge != nullptr) {
                        into.push_back(bestEdge);
                    }
                } else {
                    // handle multiple via locations on the same edge without loops
                    if (positions.size() == 1 || into.empty() || into.back() != bestEdge) {
                        into.push_back(bestEdge);
                    }
                }
            }
        }
    }


    /// @brief find closest lane within distance for the given position or nullptr
    const L* getClosestLane(const Position& pos, SUMOVehicleClass vClass, double distance = -1.) {
        NamedRTree* t = getLaneTree();
        Boundary b;
        b.add(pos);
        b.grow(distance < 0 ? myMapMatchingDistance : distance);
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        std::set<const Named*> lanes;
        Named::StoringVisitor sv(lanes);
        t->Search(cmin, cmax, sv);
        // use closest
        double minDist = std::numeric_limits<double>::max();
        const L* best = nullptr;
        for (const Named* o : lanes) {
            const L* cand = static_cast<const L*>(o);
            if (!cand->allowsVehicleClass(vClass)) {
                continue;
            }
            double dist = cand->getShape().distance2D(pos);
            if (dist < minDist) {
                minDist = dist;
                best = cand;
            }
        }
        return best;
    }

    /// @brief find closest junction taz given the closest edge
    const E* getJunctionTaz(const Position& pos, const E* closestEdge, SUMOVehicleClass vClass, bool isFrom) {
        if (closestEdge == nullptr) {
            return nullptr;
        } else {
            const N* fromJunction = closestEdge->getFromJunction();
            const N* toJunction = closestEdge->getToJunction();
            const bool fromCloser = (fromJunction->getPosition().distanceSquaredTo2D(pos) <
                                     toJunction->getPosition().distanceSquaredTo2D(pos));
            const E* fromSource = retrieveEdge(fromJunction->getID() + "-source");
            const E* fromSink = retrieveEdge(fromJunction->getID() + "-sink");
            const E* toSource = retrieveEdge(toJunction->getID() + "-source");
            const E* toSink = retrieveEdge(toJunction->getID() + "-sink");
            if (fromSource == nullptr || fromSink == nullptr) {
                myErrorOutput->inform("Junction-taz '" + fromJunction->getID() + "' not found when mapping position " + toString(pos) + "." + JUNCTION_TAZ_MISSING_HELP);
                return nullptr;
            }
            if (toSource == nullptr || toSink == nullptr) {
                myErrorOutput->inform("Junction-taz '" + toJunction->getID() + "' not found when mapping position " + toString(pos) + "." + JUNCTION_TAZ_MISSING_HELP);
                return nullptr;
            }
            const bool fromPossible = isFrom ? fromSource->getSuccessors(vClass).size() > 0 : fromSink->getPredecessors().size() > 0;
            const bool toPossible = isFrom ? toSource->getSuccessors(vClass).size() > 0 : toSink->getPredecessors().size() > 0;
            //std::cout << "getJunctionTaz pos=" << pos << " isFrom=" << isFrom << " closest=" << closestEdge->getID() << " fromPossible=" << fromPossible << " toPossible=" << toPossible << "\n";
            if (fromCloser && fromPossible) {
                // return closest if possible
                return isFrom ? fromSource : fromSink;
            } else if (!fromCloser && toPossible) {
                // return closest if possible
                return isFrom ? toSource : toSink;
            } else {
                // return possible
                if (fromPossible) {
                    return isFrom ? fromSource : fromSink;
                } else {
                    return isFrom ? toSource : toSink;
                }
            }
        }
    }


    virtual void initLaneTree(NamedRTree* tree) = 0;

    virtual E* retrieveEdge(const std::string& id) = 0;

private:
    /// @brief initialize lane-RTree
    NamedRTree* getLaneTree() {
        if (myLaneTree == nullptr) {
            myLaneTree = new NamedRTree();
            initLaneTree(myLaneTree);
        }
        return myLaneTree;
    }

    /// @brief RTree for finding lanes
    NamedRTree* myLaneTree;
    bool myMapMatchJunctions;
    double myMapMatchingDistance;
    MsgHandler* myErrorOutput;


private:
    /// @brief Invalidated copy constructor
    MapMatcher(const MapMatcher& s) = delete;

    /// @brief Invalidated assignment operator
    MapMatcher& operator=(const MapMatcher& s) = delete;
};
