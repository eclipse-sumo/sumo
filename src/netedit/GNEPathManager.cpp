/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEPathManager.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2011
///
// Manager for paths in netedit (routes, trips, flows...)
/****************************************************************************/

#include <netbuild/NBNetBuilder.h>
#include <netedit/GNENet.h>
#include <netedit/GNESegment.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/network/GNEConnection.h>
#include <utils/router/DijkstraRouter.h>

#include "GNEPathManager.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPathManager::PathCalculator - methods
// ---------------------------------------------------------------------------

GNEPathManager::PathCalculator::PathCalculator(const GNENet* net) :
    myNet(net),
    myPathCalculatorUpdated(false),
    myDijkstraRouter(nullptr) {
    // create myDijkstraRouter
    myDijkstraRouter = new DijkstraRouter<NBRouterEdge, NBVehicle>(
        myNet->getNetBuilder()->getEdgeCont().getAllRouterEdges(),
        true, &NBRouterEdge::getTravelTimeStatic, nullptr, true);
}


GNEPathManager::PathCalculator::~PathCalculator() {
    delete myDijkstraRouter;
}


void
GNEPathManager::PathCalculator::updatePathCalculator() {
    // simply delete and create myDijkstraRouter again
    if (myDijkstraRouter) {
        delete myDijkstraRouter;
    }
    myDijkstraRouter = new DijkstraRouter<NBRouterEdge, NBVehicle>(
        myNet->getNetBuilder()->getEdgeCont().getAllRouterEdges(),
        true, &NBRouterEdge::getTravelTimeStatic, nullptr, true);
    // update flag
    myPathCalculatorUpdated = true;
}


std::vector<GNEEdge*>
GNEPathManager::PathCalculator::calculateDijkstraPath(const SUMOVehicleClass vClass, const std::vector<GNEEdge*>& edges) const {
    // declare a solution vector
    std::vector<GNEEdge*> solution;
    // check if path calculator is updated
    if (!myPathCalculatorUpdated) {
        // use partialEdges as solution
        solution = edges;
        return solution;
    }
    // calculate route depending of number of partial edges
    if (edges.size() == 0) {
        // partial edges empty, then return a empty vector
        return solution;
    } else if (edges.size() == 1) {
        // if there is only one partialEdges, path has only one edge
        solution.push_back(edges.front());
        return solution;
    } else if ((edges.size() == 2) && (edges.front() == edges.back())) {
        // typical case for stops. Used to avoid unnecesary calls to compute
        solution.push_back(edges.front());
        return solution;
    } else {
        // declare temporal vehicle
        NBVehicle tmpVehicle("temporalNBVehicle", vClass);
        // obtain pointer to GNENet
        GNENet* net = edges.front()->getNet();
        // iterate over every selected myEdges
        for (int i = 1; i < (int)edges.size(); i++) {
            // ignore consecutive edges
            if (edges.at(i - 1)->getNBEdge() != edges.at(i)->getNBEdge()) {
                // declare a temporal route in which save route between two last myEdges
                std::vector<const NBRouterEdge*> partialRoute;
                myDijkstraRouter->compute(edges.at(i - 1)->getNBEdge(), edges.at(i)->getNBEdge(), &tmpVehicle, 10, partialRoute);
                // if partial route is empty, return empty route
                if (partialRoute.empty()) {
                    return {};
                } else {
                    // save partial route in solution
                    for (const auto& edgeID : partialRoute) {
                        solution.push_back(net->getAttributeCarriers()->retrieveEdge(edgeID->getID()));
                    }
                }
            }
        }
    }
    // filter solution
    auto solutionIt = solution.begin();
    // iterate over solution
    while (solutionIt != solution.end()) {
        if ((solutionIt + 1) != solution.end()) {
            // if next edge is the same of current edge, remove it
            if (*solutionIt == *(solutionIt + 1)) {
                solutionIt = solution.erase(solutionIt);
            } else {
                solutionIt++;
            }
        } else {
            solutionIt++;
        }
    }
    return solution;
}


std::vector<GNEEdge*>
GNEPathManager::PathCalculator::calculateDijkstraPath(const SUMOVehicleClass vClass, GNEEdge* fromEdge, GNEEdge* toEdge) const {
    return calculateDijkstraPath(vClass, {fromEdge, toEdge});
}


std::vector<GNEEdge*>
GNEPathManager::PathCalculator::calculateDijkstraPath(const SUMOVehicleClass vClass, GNEEdge* fromEdge, GNEJunction* toJunction) const {
    std::vector<GNEEdge*> edges;
    // get from and to edges
    const auto toEdges = toJunction->getGNEIncomingEdges();
    // try to find a path
    for (const auto& toEdge : toEdges) {
        edges = calculateDijkstraPath(vClass, fromEdge, toEdge);
        // if a path was found, clean it
        if (edges.size() > 0) {
            return optimizeJunctionPath(edges);
        }
    }
    return {};
}


std::vector<GNEEdge*>
GNEPathManager::PathCalculator::calculateDijkstraPath(const SUMOVehicleClass vClass, GNEJunction* fromJunction, GNEEdge* toEdge) const {
    std::vector<GNEEdge*> edges;
    // get from and to edges
    const auto fromEdges = fromJunction->getGNEOutgoingEdges();
    // try to find a path
    for (const auto& fromEdge : fromEdges) {
        edges = calculateDijkstraPath(vClass, fromEdge, toEdge);
        // if a path was found, clean it
        if (edges.size() > 0) {
            return optimizeJunctionPath(edges);
        }
    }
    return {};
}


std::vector<GNEEdge*>
GNEPathManager::PathCalculator::calculateDijkstraPath(const SUMOVehicleClass vClass, GNEJunction* fromJunction, GNEJunction* toJunction) const {
    std::vector<GNEEdge*> edges;
    // get from and to edges
    const auto fromEdges = fromJunction->getGNEOutgoingEdges();
    const auto toEdges = toJunction->getGNEIncomingEdges();
    // try to find a path
    for (const auto& fromEdge : fromEdges) {
        for (const auto& toEdge : toEdges) {
            edges = calculateDijkstraPath(vClass, fromEdge, toEdge);
            // if a path was found, clean it
            if (edges.size() > 0) {
                return optimizeJunctionPath(edges);
            }
        }
    }
    return {};
}


void
GNEPathManager::PathCalculator::calculateReachability(const SUMOVehicleClass vClass, GNEEdge* originEdge) {
    // first reset reachability of all lanes
    for (const auto& edge : originEdge->getNet()->getAttributeCarriers()->getEdges()) {
        for (const auto& lane : edge.second->getChildLanes()) {
            lane->resetReachability();
        }
    }
    // get max speed
    const double defaultMaxSpeed = SUMOVTypeParameter::VClassDefaultValues(vClass).maxSpeed;
    // declare map for reachable edges
    std::map<GNEEdge*, double> reachableEdges;
    // init first edge
    reachableEdges[originEdge] = 0;
    // declare a vector for checked edges
    std::vector<GNEEdge*> check;
    // add first edge
    check.push_back(originEdge);
    // continue while there is edges to check
    while (check.size() > 0) {
        GNEEdge* edge = check.front();
        check.erase(check.begin());
        double traveltime = reachableEdges[edge];
        for (const auto& lane : edge->getChildLanes()) {
            if ((edge->getNBEdge()->getLaneStruct(lane->getIndex()).permissions & vClass) == vClass) {
                lane->setReachability(traveltime);
            }
        }
        // update traveltime
        traveltime += edge->getNBEdge()->getLength() / MIN2(edge->getNBEdge()->getSpeed(), defaultMaxSpeed);
        std::vector<GNEEdge*> sucessors;
        // get successor edges
        for (const auto& sucessorEdge : edge->getToJunction()->getGNEOutgoingEdges()) {
            // check if edge is connected with successor edge
            if (consecutiveEdgesConnected(vClass, edge, sucessorEdge)) {
                sucessors.push_back(sucessorEdge);
            }
        }
        // add successors to check vector
        for (const auto& nextEdge : sucessors) {
            // revisit edge via faster path
            if ((reachableEdges.count(nextEdge) == 0) || (reachableEdges[nextEdge] > traveltime)) {
                reachableEdges[nextEdge] = traveltime;
                check.push_back(nextEdge);
            }
        }
    }
}


bool
GNEPathManager::PathCalculator::consecutiveEdgesConnected(const SUMOVehicleClass vClass, const GNEEdge* from, const GNEEdge* to) const {
    // check conditions
    if ((from == nullptr) || (to == nullptr)) {
        // myEdges cannot be null
        return false;
    } else if (from == to) {
        // the same edge cannot be consecutive of itself
        return false;
    } else if (vClass == SVC_PEDESTRIAN) {
        // for pedestrians consecutive myEdges are always connected
        return true;
    } else {
        // iterate over connections of from edge
        for (const auto& fromLane : from->getChildLanes()) {
            for (const auto& fromConnection : from->getGNEConnections()) {
                // within from loop, iterate ove to lanes
                for (const auto& toLane : to->getChildLanes()) {
                    if (fromConnection->getLaneTo() == toLane) {
                        // get lane structs for both lanes
                        const NBEdge::Lane NBFromLane = from->getNBEdge()->getLaneStruct(fromLane->getIndex());
                        const NBEdge::Lane NBToLane = to->getNBEdge()->getLaneStruct(toLane->getIndex());
                        // check vClass
                        if (((NBFromLane.permissions & vClass) == vClass) &&
                                ((NBToLane.permissions & vClass) == vClass)) {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
}


bool
GNEPathManager::PathCalculator::busStopConnected(const GNEAdditional* busStop, const GNEEdge* edge) const {
    if (busStop->getTagProperty()->getTag() != SUMO_TAG_BUS_STOP) {
        return false;
    }
    // check if busstop is placed over a pedestrian lane
    if ((busStop->getParentLanes().front()->getParentEdge() == edge) &&
            (edge->getNBEdge()->getLaneStruct(busStop->getParentLanes().front()->getIndex()).permissions & SVC_PEDESTRIAN) != 0) {
        // busStop is placed over an lane that supports pedestrians, then return true
        return true;
    }
    // obtain a list with all edge lanes that supports pedestrians
    std::vector<GNELane*> pedestrianLanes;
    for (int laneIndex = 0; laneIndex < (int)edge->getChildLanes().size(); laneIndex++) {
        if ((edge->getNBEdge()->getLaneStruct(laneIndex).permissions & SVC_PEDESTRIAN) != 0) {
            pedestrianLanes.push_back(edge->getChildLanes().at(laneIndex));
        }
    }
    // check if exist an access between busStop and pedestrian lanes
    for (const auto& access : busStop->getChildAdditionals()) {
        // check that child is an access
        if (access->getTagProperty()->getTag() == SUMO_TAG_ACCESS) {
            for (const auto& lane : pedestrianLanes) {
                if (access->getParentLanes().front() == lane) {
                    // found, then return true
                    return true;
                }
            }
        }
    }
    // There isn't a valid access, then return false
    return false;
}


bool
GNEPathManager::PathCalculator::isPathCalculatorUpdated() const {
    return myPathCalculatorUpdated;
}


void
GNEPathManager::PathCalculator::invalidatePathCalculator() {
    myPathCalculatorUpdated = false;
}


std::vector<GNEEdge*>
GNEPathManager::PathCalculator::optimizeJunctionPath(const std::vector<GNEEdge*>& edges) const {
    bool stop = false;
    std::vector<GNEEdge*> solutionA, solutionB;
    // get from and to junctions
    const auto fromJunction = edges.front()->getFromJunction();
    const auto toJunction = edges.back()->getToJunction();
    // first optimize from Junction
    for (auto it = edges.rbegin(); (it != edges.rend()) && !stop; it++) {
        solutionA.insert(solutionA.begin(), *it);
        if ((*it)->getFromJunction() == fromJunction) {
            stop = true;
        }
    }
    // optimize to edge
    stop = false;
    for (auto it = solutionA.begin(); (it != solutionA.end()) && !stop; it++) {
        solutionB.push_back(*it);
        if ((*it)->getToJunction() == toJunction) {
            stop = true;
        }
    }
    return solutionB;
}

// ---------------------------------------------------------------------------
// GNEPathManager::PathDraw - methods
// ---------------------------------------------------------------------------

GNEPathManager::PathDraw::PathDraw() {}


GNEPathManager::PathDraw::~PathDraw() {}


void
GNEPathManager::PathDraw::clearPathDraw() {
    // just clear myDrawedElements
    myLaneDrawedElements.clear();
    myLane2laneDrawedElements.clear();
}


bool
GNEPathManager::PathDraw::checkDrawPathGeometry(const GUIVisualizationSettings& s, const GNELane* lane,
        const SumoXMLTag tag, const bool isPlan) {
    // check conditions
    if (s.drawForViewObjectsHandler) {
        return true;
    } else if (myLaneDrawedElements.count(lane) > 0) {
        // check tag
        if (!isPlan && myLaneDrawedElements.at(lane).count(tag) > 0) {
            // element type was already inserted, then don't draw geometry
            return false;
        } else {
            // insert tag for the given lane
            myLaneDrawedElements.at(lane).insert(tag);
            // draw geometry
            return true;
        }
    } else {
        // insert lane and tag
        myLaneDrawedElements[lane].insert(tag);
        // draw geometry
        return true;
    }
}


bool
GNEPathManager::PathDraw::checkDrawPathGeometry(const GUIVisualizationSettings& s, const GNESegment* segment,
        const SumoXMLTag tag, const bool isPlan) {
    // check conditions
    if (s.drawForViewObjectsHandler) {
        return true;
    } else {
        // declare lane2lane
        const std::pair<const GNELane*, const GNELane*> lane2lane(segment->getPreviousLane(), segment->getNextLane());
        // check lane2lane
        if (myLane2laneDrawedElements.count(lane2lane) > 0) {
            // check tag
            if (!isPlan && myLane2laneDrawedElements.at(lane2lane).count(tag) > 0) {
                // element type was already inserted, then don't draw geometry
                return false;
            } else {
                // insert tag for the given lane2lane
                myLane2laneDrawedElements.at(lane2lane).insert(tag);
                // draw geometry
                return true;
            }
        } else {
            // insert lane2lane and tag
            myLane2laneDrawedElements[lane2lane].insert(tag);
            // draw geometry
            return true;
        }
    }
}

// ---------------------------------------------------------------------------
// GNEPathManager - methods
// ---------------------------------------------------------------------------

GNEPathManager::GNEPathManager(const GNENet* net) :
    myPathCalculator(new PathCalculator(net)),
    myPathDraw(new PathDraw()) {
}


GNEPathManager::~GNEPathManager() {
    // clear paths
    clearSegments();
    // delete route calculator Instance
    delete myPathCalculator;
    // delete path draw
    delete myPathDraw;
}


GNEPathManager::PathCalculator*
GNEPathManager::getPathCalculator() {
    return myPathCalculator;
}


const GNEPathElement*
GNEPathManager::getPathElement(const GUIGlObject* GLObject) const {
    // first parse pathElement
    const auto pathElement = dynamic_cast<const GNEPathElement*>(GLObject);
    if (pathElement == nullptr) {
        return nullptr;
    } else {
        // find it in paths
        auto it = myPaths.find(pathElement);
        if (it == myPaths.end()) {
            return nullptr;
        } else {
            return it->first;
        }
    }
}


const std::vector<GNESegment*>&
GNEPathManager::getPathElementSegments(GNEPathElement* pathElement) const {
    if (myPaths.count(pathElement) > 0) {
        return myPaths.at(pathElement);
    } else {
        return myEmptySegments;
    }
}


GNEPathManager::PathDraw*
GNEPathManager::getPathDraw() {
    return myPathDraw;
}


bool
GNEPathManager::isPathValid(const GNEPathElement* pathElement) const {
    // first check if path element exist
    if (myPaths.count(pathElement) > 0) {
        // iterate over all segments
        for (const auto& segment : myPaths.at(pathElement)) {
            // if we have two consecutive lane segments, then path isn't valid
            if (segment->getLane() && segment->getNextLane()) {
                return false;
            }
        }
        // all ok, then return true
        return true;
    } else {
        return false;
    }
}


const GNELane*
GNEPathManager::getFirstLane(const GNEPathElement* pathElement) const {
    if ((myPaths.count(pathElement) > 0) && (myPaths.at(pathElement).size() > 0)) {
        return myPaths.at(pathElement).front()->getLane();
    } else {
        return nullptr;
    }
}



void
GNEPathManager::calculatePath(GNEPathElement* pathElement, SUMOVehicleClass vClass, GNELane* fromLane, GNELane* toLane) {
    // build path
    buildPath(pathElement, vClass, myPathCalculator->calculateDijkstraPath(vClass, fromLane->getParentEdge(), toLane->getParentEdge()),
              fromLane, nullptr, toLane, nullptr);
}


void
GNEPathManager::calculatePath(GNEPathElement* pathElement, SUMOVehicleClass vClass, GNELane* fromLane, GNEJunction* toJunction) {
    // build path
    buildPath(pathElement, vClass, myPathCalculator->calculateDijkstraPath(vClass, fromLane->getParentEdge(), toJunction),
              fromLane, nullptr, nullptr, toJunction);
}


void
GNEPathManager::calculatePath(GNEPathElement* pathElement, SUMOVehicleClass vClass, GNEJunction* fromJunction, GNELane* toLane) {
    // build path
    buildPath(pathElement, vClass, myPathCalculator->calculateDijkstraPath(vClass, fromJunction, toLane->getParentEdge()),
              nullptr, fromJunction, toLane, nullptr);
}


void
GNEPathManager::calculatePath(GNEPathElement* pathElement, SUMOVehicleClass vClass, GNEJunction* fromJunction, GNEJunction* toJunction) {
    // build path
    buildPath(pathElement, vClass, myPathCalculator->calculateDijkstraPath(vClass, fromJunction, toJunction), nullptr, fromJunction, nullptr, toJunction);
}


void
GNEPathManager::calculatePath(GNEPathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEEdge*>& edges) {
    // build path
    if (edges.size() > 0) {
        buildPath(pathElement, vClass, myPathCalculator->calculateDijkstraPath(vClass, edges),
                  edges.front()->getLaneByAllowedVClass(vClass), nullptr, edges.back()->getLaneByAllowedVClass(vClass), nullptr);
    } else {
        removePath(pathElement);
    }
}


void
GNEPathManager::calculateConsecutivePathEdges(GNEPathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEEdge*>& edges,
        const int firstLaneIndex, const int lastLaneIndex) {
    // declare lane vector
    std::vector<GNELane*> lanes;
    if (edges.size() > 0) {
        lanes.reserve(edges.size());
        // add first lane
        if ((firstLaneIndex >= 0) && (firstLaneIndex < (int)edges.front()->getChildLanes().size())) {
            lanes.push_back(edges.front()->getChildLanes().at(firstLaneIndex));
        } else {
            lanes.push_back(edges.front()->getLaneByAllowedVClass(vClass));
        }
        // add rest of lanes
        if (edges.size() > 1) {
            // add middle lanes
            for (int i = 1; i < ((int)edges.size() - 1); i++) {
                lanes.push_back(edges[i]->getLaneByAllowedVClass(vClass));
            }
            // add last lane
            if ((lastLaneIndex >= 0) && (lastLaneIndex < (int)edges.back()->getChildLanes().size())) {
                lanes.push_back(edges.back()->getChildLanes().at(lastLaneIndex));
            } else {
                lanes.push_back(edges.back()->getLaneByAllowedVClass(vClass));
            }
        }
    }
    // calculate consecutive path lanes
    calculateConsecutivePathLanes(pathElement, lanes);
}


void
GNEPathManager::calculateConsecutivePathLanes(GNEPathElement* pathElement, const std::vector<GNELane*>& lanes) {
    // first remove path element from paths
    removePath(pathElement);
    // continue depending of number of lanes
    if (lanes.size() > 0) {
        // declare segment vector
        std::vector<GNESegment*> segments;
        // declare last index
        const int lastIndex = ((int)lanes.size() - 1);
        // reserve segments
        segments.reserve(2 * lanes.size());
        // iterate over lanes
        for (int i = 0; i < (int)lanes.size(); i++) {
            // create lane segment
            new GNESegment(this, pathElement, lanes.at(i), segments);
            // continue if this isn't the last lane
            if (i != lastIndex) {
                // create junction segments
                new GNESegment(this, pathElement, lanes.at(i)->getParentEdge()->getToJunction(), segments);
            }
        }
        // mark label segment
        markLabelSegment(segments);
        // add segments in paths
        myPaths[pathElement] = segments;
    }
}


void
GNEPathManager::removePath(GNEPathElement* pathElement) {
    // check if path element exist already in myPaths
    if (myPaths.find(pathElement) != myPaths.end()) {
        // delete segments
        for (const auto& segment : myPaths.at(pathElement)) {
            delete segment;
        }
        // remove path element from myPaths
        myPaths.erase(pathElement);
    }
}


void
GNEPathManager::drawLanePathElements(const GUIVisualizationSettings& s, const GNELane* lane) const {
    // check detail level and lane segments
    if (myLaneSegments.count(lane) > 0) {
        int numRoutes = 0;
        // first draw the elements marked for redraw
        for (const auto& segment : myLaneSegments.at(lane)) {
            if (gViewObjectsHandler.isPathElementMarkForRedraw(segment->getPathElement())) {
                segment->getPathElement()->drawLanePartialGL(s, segment, 2);
                // check if path element is a route
                if (segment->getPathElement()->isRoute()) {
                    numRoutes++;
                }
            }
        }
        // now draw the rest of segments
        for (const auto& segment : myLaneSegments.at(lane)) {
            if (!gViewObjectsHandler.isPathElementMarkForRedraw(segment->getPathElement())) {
                segment->getPathElement()->drawLanePartialGL(s, segment, 0);
                // check if path element is a route
                if (segment->getPathElement()->isRoute()) {
                    numRoutes++;
                }
            }
        }
        // check if draw overlapped routes
        if ((numRoutes > 1) && lane->getNet()->getViewNet()->getDemandViewOptions().showOverlappedRoutes()) {
            lane->drawOverlappedRoutes(numRoutes);
        }
    }
}


void
GNEPathManager::drawJunctionPathElements(const GUIVisualizationSettings& s, const GNEJunction* junction) const {
    // check detail level and junction segments
    if (myJunctionSegments.count(junction) > 0) {
        // first draw the elements marked for redraw
        for (const auto& segment : myJunctionSegments.at(junction)) {
            if (gViewObjectsHandler.isPathElementMarkForRedraw(segment->getPathElement())) {
                segment->getPathElement()->drawJunctionPartialGL(s, segment, 2);
            }
        }
        // now draw the rest of segments
        for (const auto& segment : myJunctionSegments.at(junction)) {
            if (!gViewObjectsHandler.isPathElementMarkForRedraw(segment->getPathElement())) {
                segment->getPathElement()->drawJunctionPartialGL(s, segment, 0);
            }
        }
    }
}


void
GNEPathManager::redrawPathElements(const GUIVisualizationSettings& s) const {
    // draw every segment partial
    for (const auto& pathElementToRedraw : gViewObjectsHandler.getRedrawPathElements()) {
        const auto it = myPaths.find(pathElementToRedraw);
        if (it != myPaths.end()) {
            for (const auto& segment : it->second) {
                if (segment->getLane()) {
                    // draw with high offset
                    it->first->drawLanePartialGL(s, segment, 3);
                } else if (segment->getJunction()) {
                    // draw with high offset
                    it->first->drawJunctionPartialGL(s, segment, 3);
                }
            }
        }
    }
}


void
GNEPathManager::invalidateLanePath(const GNELane* lane) {
    // declare vector for path elements to compute
    std::vector<GNEPathElement*> pathElementsToCompute;
    // check lane in laneSegments
    if (myLaneSegments.count(lane) > 0) {
        // obtain affected path elements
        for (const auto& segment : myLaneSegments.at(lane)) {
            pathElementsToCompute.push_back(segment->getPathElement());
        }
    }
    // compute path elements
    for (const auto& pathElement : pathElementsToCompute) {
        pathElement->computePathElement();
    }
}


void
GNEPathManager::invalidateJunctionPath(const GNEJunction* junction) {
    // declare vector for path elements to compute
    std::vector<GNEPathElement*> pathElementsToCompute;
    // check junction in junctionSegments
    if (myJunctionSegments.count(junction) > 0) {
        // obtain affected path elements
        for (const auto& segment : myJunctionSegments.at(junction)) {
            pathElementsToCompute.push_back(segment->getPathElement());
        }
    }
    // compute path elements
    for (const auto& pathElement : pathElementsToCompute) {
        pathElement->computePathElement();
    }
}


void
GNEPathManager::clearSegments() {
    // clear segments quickly
    myCleaningSegments = true;
    // clear all segments
    for (const auto& path : myPaths) {
        // delete all segments
        for (const auto& segment : path.second) {
            delete segment;
        }
    }
    // clear containers
    myPaths.clear();
    myJunctionSegments.clear();
    myLaneSegments.clear();
    // restore flag
    myCleaningSegments = false;
}


void
GNEPathManager::addSegmentInLaneSegments(GNESegment* segment, const GNELane* lane) {
    myLaneSegments[lane].insert(segment);
}


void
GNEPathManager::addSegmentInJunctionSegments(GNESegment* segment, const GNEJunction* junction) {
    myJunctionSegments[junction].insert(segment);
}


void
GNEPathManager::clearSegmentFromJunctionAndLaneSegments(GNESegment* segment) {
    // check if segment has a lane
    if (segment->getLane()) {
        auto lane = segment->getLane();
        myLaneSegments[lane].erase(segment);
        // clear lane if doesn't have more segments
        if (myLaneSegments.at(lane).empty()) {
            myLaneSegments.erase(lane);
        }
    }
    if (segment->getJunction()) {
        auto junction = segment->getJunction();
        myJunctionSegments[junction].erase(segment);
        // clear junction if doesn't have more segments
        if (myJunctionSegments.at(junction).empty()) {
            myJunctionSegments.erase(junction);
        }
    }
}


bool
GNEPathManager::connectedLanes(const GNELane* fromLane, const GNELane* toLane) const {
    // get from and to NBEdges
    NBEdge* fromNBEdge = fromLane->getParentEdge()->getNBEdge();
    NBEdge* toNBEdge = toLane->getParentEdge()->getNBEdge();
    // get connections vinculated with from Lane
    const std::vector<NBEdge::Connection> connections = fromNBEdge->getConnectionsFromLane(fromLane->getIndex());
    // find connection
    std::vector<NBEdge::Connection>::const_iterator con_it = find_if(
                connections.begin(), connections.end(),
                NBEdge::connections_finder(fromLane->getIndex(), toNBEdge, toLane->getIndex()));
    // check if connection was found
    return (con_it != connections.end());
}


void
GNEPathManager::buildPath(GNEPathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEEdge*> path,
                          GNELane* fromLane, GNEJunction* fromJunction, GNELane* toLane, GNEJunction* toJunction) {
    // first remove path element from paths
    removePath(pathElement);
    // declare segment vector
    std::vector<GNESegment*> segments;
    // continue if path isn't empty
    if (path.size() > 0) {
        // declare last index
        const int lastIndex = ((int)path.size() - 1);
        // reserve segments
        segments.reserve(2 * path.size());
        if (fromJunction) {
            // create lane segment using fromJunction
            new GNESegment(this, pathElement, fromJunction, segments);
        }
        // iterate over path
        for (int i = 0; i < (int)path.size(); i++) {
            if ((i == 0) && fromLane) {
                // create lane segment using fromLane
                new GNESegment(this, pathElement, fromLane, segments);
                // continue if this isn't the last path edge
                if (i != lastIndex) {
                    // create junction segment using to junction
                    new GNESegment(this, pathElement, path.at(i)->getToJunction(), segments);
                }
            } else if ((i == lastIndex) && toLane) {
                // create lane segment using toLane
                new GNESegment(this, pathElement, toLane, segments);
            } else {
                // get first allowed lane
                const GNELane* lane = path.at(i)->getLaneByAllowedVClass(vClass);
                // create lane segment
                new GNESegment(this, pathElement, lane, segments);
                // continue if this isn't the last path edge
                if (i != lastIndex) {
                    // create junction segment using to junction
                    new GNESegment(this, pathElement, path.at(i)->getToJunction(), segments);
                }
            }
        }
        if (toJunction) {
            // create lane segment using toJunction
            new GNESegment(this, pathElement, toJunction, segments);
        }
        // mark label segment
        markLabelSegment(segments);
        // add segments in paths
        myPaths[pathElement] = segments;
    } else {
        // create first segment
        GNESegment* firstSegment = nullptr;
        GNESegment* lastSegment = nullptr;
        // continue depending of from-to elements
        if (fromLane) {
            firstSegment = new GNESegment(this, pathElement, fromLane, segments);
        } else if (fromJunction) {
            firstSegment = new GNESegment(this, pathElement, fromJunction, segments);
        }
        if (toLane) {
            lastSegment = new GNESegment(this, pathElement, toLane, segments);
        } else if (toJunction) {
            lastSegment = new GNESegment(this, pathElement, toJunction, segments);
        }
        // continue depending of segments
        if (firstSegment && lastSegment) {
            // mark segment as label segment
            firstSegment->markSegmentLabel();
            // add segments in path
            myPaths[pathElement] = segments;
        } else {
            delete firstSegment;
            delete lastSegment;
        }
    }
}


void
GNEPathManager::markLabelSegment(const std::vector<GNESegment*>& segments) const {
    // separate junction segments and lane segments
    std::vector<GNESegment*> laneSegments;
    laneSegments.reserve(segments.size());
    for (const auto& segment : segments) {
        if (segment->getLane()) {
            laneSegments.push_back(segment);
        }
    }
    // get lane segment index
    const int laneSegmentIndex = (int)((double)laneSegments.size() * 0.5);
    // mark middle label as label segment
    laneSegments.at(laneSegmentIndex)->markSegmentLabel();
}

/****************************************************************************/
