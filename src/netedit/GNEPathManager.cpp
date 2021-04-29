/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
// Manager for paths in NETEDIT (routes, trips, flows...)
/****************************************************************************/

#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <netedit/GNENet.h>
#include <netedit/elements/network/GNEConnection.h>
#include <utils/router/DijkstraRouter.h>

#include "GNEPathManager.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPathManager::Segment - methods
// ---------------------------------------------------------------------------

GNEPathManager::Segment::Segment(GNEPathManager* pathManager, PathElement* element, const GNELane* lane, 
        const bool firstSegment, const bool lastSegment, const bool valid) :
    myPathManager(pathManager),
    myPathElement(element),
    myFirstSegment(firstSegment),
    myLastSegment(lastSegment),
    myLane(lane),
    myPreviousLane(nullptr),
    myNextLane(nullptr),
    myJunction(nullptr),
    myValid(valid),
    myNextSegment(nullptr) {
    // add segment in laneSegments
    myPathManager->addSegmentInLaneSegments(this, lane);
}


GNEPathManager::Segment::Segment(GNEPathManager* pathManager, PathElement* element, const GNEJunction* junction, 
        const GNELane* previousLane, const GNELane* nextLane, const bool valid) :
    myPathManager(pathManager),
    myPathElement(element),
    myFirstSegment(false),
    myLastSegment(false),
    myLane(nullptr),
    myPreviousLane(previousLane),
    myNextLane(nextLane),
    myJunction(junction),
    myValid(valid),
    myNextSegment(nullptr) {
    // add segment in junctionSegments
    myPathManager->addSegmentInJunctionSegments(this, junction);
}


GNEPathManager::Segment::~Segment() {
    // clear segment from LaneSegments
    myPathManager->clearSegmentFromJunctionAndLaneSegments(this);
}


bool 
GNEPathManager::Segment::isFirstSegment() const {
    if (myLane) {
        return myFirstSegment;
    } else {
        throw ProcessError("Invalid call: Only allowed in lane segments");
    }
}


bool 
GNEPathManager::Segment::isLastSegment() const {
    if (myLane) {
        return myLastSegment;
    } else {
        throw ProcessError("Invalid call: Only allowed in lane segments");
    }
}


GNEPathManager::PathElement*
GNEPathManager::Segment::getPathElement() const {
    return myPathElement;
}


const GNELane* 
GNEPathManager::Segment::getLane() const {
    return myLane;
}


const GNELane* 
GNEPathManager::Segment::getPreviousLane() const {
    if (myJunction) {
        return myPreviousLane;
    } else {
        throw ProcessError("Invalid call: Only allowed in junction segments");
    }
}


const GNELane* 
GNEPathManager::Segment::getNextLane() const {
    if (myJunction) {
        return myNextLane;
    } else {
        throw ProcessError("Invalid call: Only allowed in junction segments");
    }
}


const GNEJunction* 
GNEPathManager::Segment::getJunction() const {
    return myJunction;
}


bool 
GNEPathManager::Segment::isValid() const {
    return myValid;
}


GNEPathManager::Segment*
GNEPathManager::Segment::getNextSegment() const {
    return myNextSegment;
}


void 
GNEPathManager::Segment::setNextSegment(GNEPathManager::Segment *nextSegment) {
    myNextSegment = nextSegment;
}


GNEPathManager::Segment::Segment() :
    myPathManager(nullptr),
    myPathElement(nullptr),
    myFirstSegment(false),
    myLastSegment(false),
    myLane(false),
    myPreviousLane(nullptr),
    myNextLane(nullptr),
    myJunction(false),
    myValid(false),
    myNextSegment(nullptr) {
}

// ---------------------------------------------------------------------------
// GNEPathManager::PathElement - methods
// ---------------------------------------------------------------------------

GNEPathManager::PathElement::PathElement(const Options option) :
    myOption(option) {
}


GNEPathManager::PathElement::~PathElement() {}


bool 
GNEPathManager::PathElement::isNetworkElement() const {
    return (myOption == PathElement::Options::NETWORK_ELEMENT);
}


bool 
GNEPathManager::PathElement::isAdditionalElement() const {
    return (myOption == PathElement::Options::ADDITIONAL_ELEMENT);
}


bool
GNEPathManager::PathElement::isDemandElement() const {
    return (myOption == PathElement::Options::DEMAND_ELEMENT);
}


bool 
GNEPathManager::PathElement::isDataElement() const {
    return (myOption == PathElement::Options::DATA_ELEMENT);
}


GNEPathManager::PathElement::PathElement() :
    myOption(PathElement::Options::NETWORK_ELEMENT) {
}

// ---------------------------------------------------------------------------
// GNEPathManager::PathCalculator - methods
// ---------------------------------------------------------------------------

GNEPathManager::PathCalculator::PathCalculator(const GNENet* net) :
    myNet(net),
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
}


std::vector<GNEEdge*>
GNEPathManager::PathCalculator::calculatePath(const SUMOVehicleClass vClass, const std::vector<GNEEdge*>& partialEdges) const {
    // declare a solution vector
    std::vector<GNEEdge*> solution;
    // calculate route depending of number of partial myEdges
    if (partialEdges.size() == 0) {
        // partial edges empty, then return a empty vector
        return solution;
    }
    if (partialEdges.size() == 1) {
        // if there is only one partialEdges, route has only one edge
        solution.push_back(partialEdges.front());
    }
    else {
        // declare temporal vehicle
        NBVehicle tmpVehicle("temporalNBVehicle", vClass);
        // obtain pointer to GNENet
        GNENet* net = partialEdges.front()->getNet();
        // iterate over every selected myEdges
        for (int i = 1; i < (int)partialEdges.size(); i++) {
            // declare a temporal route in which save route between two last myEdges
            std::vector<const NBRouterEdge*> partialRoute;
            myDijkstraRouter->compute(partialEdges.at(i - 1)->getNBEdge(), partialEdges.at(i)->getNBEdge(), &tmpVehicle, 10, partialRoute);
            // save partial route in solution
            for (const auto& edgeID : partialRoute) {
                solution.push_back(net->retrieveEdge(edgeID->getID()));
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


void
GNEPathManager::PathCalculator::calculateReachability(const SUMOVehicleClass vClass, GNEEdge* originEdge) {
    // first reset reachability of all lanes
    for (const auto& edge : originEdge->getNet()->getAttributeCarriers()->getEdges()) {
        for (const auto& lane : edge.second->getLanes()) {
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
        for (const auto& lane : edge->getLanes()) {
            if ((edge->getNBEdge()->getLaneStruct(lane->getIndex()).permissions & vClass) == vClass) {
                lane->setReachability(traveltime);
            }
        }
        // update traveltime
        traveltime += edge->getNBEdge()->getLength() / MIN2(edge->getNBEdge()->getSpeed(), defaultMaxSpeed);
        std::vector<GNEEdge*> sucessors;
        // get sucessor edges
        for (const auto& sucessorEdge : edge->getParentJunctions().back()->getGNEOutgoingEdges()) {
            // check if edge is connected with sucessor edge
            if (consecutiveEdgesConnected(vClass, edge, sucessorEdge)) {
                sucessors.push_back(sucessorEdge);
            }
        }
        // add sucessors to check vector
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
    }
    else if (from == to) {
        // the same edge cannot be consecutive of itself
        return false;
    }
    else if (vClass == SVC_PEDESTRIAN) {
        // for pedestrians consecutive myEdges are always connected
        return true;
    }
    else {
        // iterate over connections of from edge
        for (const auto& fromLane : from->getLanes()) {
            for (const auto& fromConnection : from->getGNEConnections()) {
                // within from loop, iterate ove to lanes
                for (const auto& toLane : to->getLanes()) {
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
    if (busStop->getTagProperty().getTag() != SUMO_TAG_BUS_STOP) {
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
    for (int laneIndex = 0; laneIndex < (int)edge->getLanes().size(); laneIndex++) {
        if ((edge->getNBEdge()->getLaneStruct(laneIndex).permissions & SVC_PEDESTRIAN) != 0) {
            pedestrianLanes.push_back(edge->getLanes().at(laneIndex));
        }
    }
    // check if exist an access between busStop and pedestrian lanes
    for (const auto& access : busStop->getChildAdditionals()) {
        // check that child is an access
        if (access->getTagProperty().getTag() == SUMO_TAG_ACCESS) {
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

// ---------------------------------------------------------------------------
// GNEPathManager::PathCalculator - methods
// ---------------------------------------------------------------------------

GNEPathManager::GNEPathManager(const GNENet* net) :
    myPathCalculator(new PathCalculator(net)) {
}


GNEPathManager::~GNEPathManager() {
    // clear paths
    clearSegments();
    // delete route calculator Instance
    delete myPathCalculator;
}


GNEPathManager::PathCalculator*
GNEPathManager::getPathCalculator() {
    return myPathCalculator;
}


bool 
GNEPathManager::isPathValid(PathElement* pathElement) const {
    // first check if path element exist
    if (myPaths.count(pathElement) > 0) {
        // check if path hat more than one element
        if (myPaths.at(pathElement).size() > 0) {
            // if there is a next segment, then is invalid
            return (myPaths.at(pathElement).front()->getNextSegment() == nullptr);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


const GNELane* 
GNEPathManager::getFirstLane(const PathElement* pathElement) const {
    if ((myPaths.count(pathElement) > 0) && (myPaths.at(pathElement).size() > 0)) {
        return myPaths.at(pathElement).front()->getLane();
    } else {
        return nullptr;
    }
}


void
GNEPathManager::calculateEdgesPath(PathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEEdge*> edges) {
    // check if path element exist already in myPaths
    if (myPaths.find(pathElement) != myPaths.end()) {
        // delete segments
        for (const auto& segment : myPaths.at(pathElement)) {
            delete segment;
        }
        // remove path element from myPaths
        myPaths.erase(pathElement);
    }
    // continue depending of number of edges
    if (edges.size() > 0) {
        // declare segment vector
        std::vector<Segment*> segments;
        // calculate path
        const std::vector<GNEEdge*> path = myPathCalculator->calculatePath(vClass, edges);
        // continue if path isn't empty
        if (path.size() > 0) {
            for (int i = 0; i < (int)path.size(); i++) {
                // get first and last segment flags
                const bool firstSegment = (i == 0);
                const bool lastSegment = (i == ((int)path.size() - 1));
                // get first allowed lane
                const GNELane* lane = path.at(i)->getLaneByAllowedVClass(vClass);
                // create segments
                Segment* laneSegment = new Segment(this, pathElement, lane, firstSegment, lastSegment, true);
                // add it into segment vector
                segments.push_back(laneSegment);
                // continue if this isn't the last edge
                if (path.at(i) != path.back()) {
                    // obtain next lane
                    const GNELane* nextLane = path.at(i + 1)->getLaneByAllowedVClass(vClass);
                    // create junction segments
                    Segment* junctionSegment = new Segment(this, pathElement, path.at(i)->getParentJunctions().at(1), lane, nextLane, true);
                    // add it into segment vector
                    segments.push_back(junctionSegment);
                }
            }
        } else {
            // create first segment
            Segment* firstSegment = new Segment(this, pathElement, edges.front()->getLaneByAllowedVClass(vClass), true, false, true);
            // add to segments
            segments.push_back(firstSegment);
            // create last segment
            Segment* lastSegment = new Segment(this, pathElement, edges.back()->getLaneByAllowedVClass(vClass), false, true, false);
            // add to segments
            segments.push_back(lastSegment);
            // set next segment for vinculating first and last segment with a red line
            firstSegment->setNextSegment(lastSegment);
        }
        // add segment in path
        myPaths[pathElement] = segments;
    }
}


void 
GNEPathManager::calculateLanesPath(PathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNELane*> lanes) {
    // declare edges
    std::vector<GNEEdge*> edges;
    // reserve edges
    edges.reserve(lanes.size());
    // get parent of every lane
    for (const auto& lane : lanes) {
        edges.push_back(lane->getParentEdge());
    }
    // calculate edges path
    calculateEdgesPath(pathElement, vClass, edges);
}


void 
GNEPathManager::removePath(PathElement* pathElement) {
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
GNEPathManager::drawLanePathElements(const GUIVisualizationSettings& s, const GNELane* lane) {
    if (myLaneSegments.count(lane) > 0) {
        for (const auto &segment: myLaneSegments.at(lane)) {
            // draw segment
            segment->getPathElement()->drawPartialGL(s, lane, segment, 0);
        }
    }
}


void
GNEPathManager::drawJunctionPathElements(const GUIVisualizationSettings& s, const GNEJunction* junction) {
    if (myJunctionSegments.count(junction) > 0) {
        for (const auto& segment : myJunctionSegments.at(junction)) {
            segment->getPathElement()->drawPartialGL(s, segment->getPreviousLane(), segment->getNextLane(), segment, 0);
        }
    }
}


void
GNEPathManager::invalidatePath(const GNELane* lane) {
    // declare vector for path elements to compute
    std::vector<PathElement*> pathElementsToCompute;
    // check lane in laneSegments
    if (myLaneSegments.count(lane) > 0) {
        // obtain affected path elements
        for (const auto &segment: myLaneSegments.at(lane)) {
            pathElementsToCompute.push_back(segment->getPathElement());
        }
    }
    // compute path elements
    for (const auto &pathElement : pathElementsToCompute) {
        pathElement->computePathElement();
    }
}


void 
GNEPathManager::clearDemandPaths() {
    // declare iterator
    auto it = myPaths.begin();
    // iterate over paths
    while (it != myPaths.end()) {
        if (it->first->isDemandElement()) {
            // delete all segments
            for (const auto &segment : it->second) {
                delete segment;
            }
            // remove path
            it = myPaths.erase(it);
        } else {
            it++;
        }
    }
}


void 
GNEPathManager::addSegmentInLaneSegments(Segment* segment, const GNELane* lane) {
    myLaneSegments[lane].insert(segment);
}


void
GNEPathManager::addSegmentInJunctionSegments(Segment* segment, const GNEJunction* junction) {
    myJunctionSegments[junction].insert(segment);
}


void
GNEPathManager::clearSegmentFromJunctionAndLaneSegments(Segment* segment) {
    // first declare vector with lanes to clear
    std::vector<const GNELane*> lanesToClear;
    // now iterate over laneSegments
    for (auto& laneSegment : myLaneSegments) {
        // remove segment from segment sets
        if (laneSegment.second.find(segment) != laneSegment.second.end()) {
            laneSegment.second.erase(segment);
        }
        // now check if lane doesn't have segments
        if (laneSegment.second.empty()) {
            lanesToClear.push_back(laneSegment.first);
        }
    }
    // finally clear empty lanes from myLaneSegments
    for (const auto& laneToClear : lanesToClear) {
        myLaneSegments.erase(laneToClear);
    }
    // first declare vector with junctions to clear
    std::vector<const GNEJunction*> junctionsToClear;
    // now iterate over junctionSegments
    for (auto& junctionSegment : myJunctionSegments) {
        // remove segment from segment sets
        if (junctionSegment.second.find(segment) != junctionSegment.second.end()) {
            junctionSegment.second.erase(segment);
        }
        // now check if junction doesn't have segments
        if (junctionSegment.second.empty()) {
            junctionsToClear.push_back(junctionSegment.first);
        }
    }
    // finally clear empty junctions from myJunctionSegments
    for (const auto& junctionToClear : junctionsToClear) {
        myJunctionSegments.erase(junctionToClear);
    }
}


void 
GNEPathManager::clearSegments() {
    // first iterate over paths
    for (const auto& path : myPaths) {
        // delete all segments
        for (const auto& segment : path.second) {
            delete segment;
        }
    }
    // clear paths
    myPaths.clear();
}

/****************************************************************************/
