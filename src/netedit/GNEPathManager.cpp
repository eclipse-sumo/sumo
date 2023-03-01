/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
#include <netedit/GNEViewNet.h>
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
                                 const bool firstSegment, const bool lastSegment) :
    myPathManager(pathManager),
    myPathElement(element),
    myFirstSegment(firstSegment),
    myLastSegment(lastSegment),
    myLane(lane),
    myPreviousLane(nullptr),
    myNextLane(nullptr),
    myJunction(nullptr),
    myNextSegment(nullptr),
    myPreviousSegment(nullptr),
    myLabelSegment(false) {
    // add segment in laneSegments
    myPathManager->addSegmentInLaneSegments(this, lane);
}


GNEPathManager::Segment::Segment(GNEPathManager* pathManager, PathElement* element, const GNEJunction* junction,
                                 const GNELane* previousLane, const GNELane* nextLane) :
    myPathManager(pathManager),
    myPathElement(element),
    myFirstSegment(false),
    myLastSegment(false),
    myLane(nullptr),
    myPreviousLane(previousLane),
    myNextLane(nextLane),
    myJunction(junction),
    myNextSegment(nullptr),
    myPreviousSegment(nullptr),
    myLabelSegment(false) {
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


GNEPathManager::Segment*
GNEPathManager::Segment::getNextSegment() const {
    return myNextSegment;
}


void
GNEPathManager::Segment::setNextSegment(GNEPathManager::Segment* nextSegment) {
    myNextSegment = nextSegment;
}


GNEPathManager::Segment*
GNEPathManager::Segment::getPreviousSegment() const {
    return myPreviousSegment;
}


void
GNEPathManager::Segment::setPreviousSegment(GNEPathManager::Segment* previousSegment) {
    myPreviousSegment = previousSegment;
}


bool
GNEPathManager::Segment::isLabelSegment() const {
    return myLabelSegment;
}


void
GNEPathManager::Segment::markSegmentLabel() {
    myLabelSegment = true;
}


GNEPathManager::Segment::Segment() :
    myPathManager(nullptr),
    myPathElement(nullptr),
    myFirstSegment(false),
    myLastSegment(false),
    myLane(nullptr),
    myPreviousLane(nullptr),
    myNextLane(nullptr),
    myJunction(nullptr),
    myNextSegment(nullptr),
    myPreviousSegment(nullptr),
    myLabelSegment(false) {
}

// ---------------------------------------------------------------------------
// GNEPathManager::PathElement - methods
// ---------------------------------------------------------------------------

GNEPathManager::PathElement::PathElement(GUIGlObjectType type, const std::string& microsimID, FXIcon* icon, const int options) :
    GUIGlObject(type, microsimID, icon),
    myOption(options) {
}


GNEPathManager::PathElement::~PathElement() {}


bool
GNEPathManager::PathElement::isNetworkElement() const {
    return (myOption & PathElement::Options::NETWORK_ELEMENT) != 0;
}


bool
GNEPathManager::PathElement::isAdditionalElement() const {
    return (myOption & PathElement::Options::ADDITIONAL_ELEMENT) != 0;
}


bool
GNEPathManager::PathElement::isDemandElement() const {
    return (myOption & PathElement::Options::DEMAND_ELEMENT) != 0;
}


bool
GNEPathManager::PathElement::isDataElement() const {
    return (myOption & PathElement::Options::DATA_ELEMENT) != 0;
}


bool
GNEPathManager::PathElement::isRoute() const {
    return (myOption & PathElement::Options::ROUTE) != 0;
}

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
GNEPathManager::PathCalculator::calculateDijkstraPath(const SUMOVehicleClass vClass, const std::vector<GNEEdge*>& partialEdges) const {
    // declare a solution vector
    std::vector<GNEEdge*> solution;
    // calculate route depending of number of partial myEdges
    if (partialEdges.size() == 0) {
        // partial edges empty, then return a empty vector
        return solution;
    }
    // check if path calculator is updated
    if (!myPathCalculatorUpdated) {
        // use partialEdges as solution
        solution = partialEdges;
        return solution;
    }
    if (partialEdges.size() == 1) {
        // if there is only one partialEdges, route has only one edge
        solution.push_back(partialEdges.front());
        return solution;
    } else {
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
                solution.push_back(net->getAttributeCarriers()->retrieveEdge(edgeID->getID()));
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
GNEPathManager::PathCalculator::calculateDijkstraPath(const SUMOVehicleClass vClass, const GNEJunction* fromJunction, const GNEJunction* toJunction) const {
    std::vector<GNEEdge*> edges;
    // get from and to edges
    const auto fromEdges = fromJunction->getGNEOutgoingEdges();
    const auto toEdges = toJunction->getGNEIncomingEdges();
    // try to find a path
    for (const auto& fromEdge : fromEdges) {
        for (const auto& toEdge : toEdges) {
            edges = calculateDijkstraPath(vClass, {fromEdge, toEdge});
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
GNEPathManager::PathDraw::drawPathGeometry(const bool dottedElement, const GNELane* lane, SumoXMLTag tag) {
    // check conditions
    if (dottedElement) {
        return true;
    } else if (lane->getNet()->getViewNet()->getVisualisationSettings().drawForPositionSelection ||
               lane->getNet()->getViewNet()->getVisualisationSettings().drawForRectangleSelection) {
        return true;
    } else if (myLaneDrawedElements.count(lane) > 0) {
        // check tag
        if (myLaneDrawedElements.at(lane).count(tag) > 0) {
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
GNEPathManager::PathDraw::drawPathGeometry(const bool dottedElement, const GNELane* fromLane, const GNELane* toLane, SumoXMLTag tag) {
    // check conditions
    if (dottedElement) {
        return true;
    } else if (fromLane->getNet()->getViewNet()->getVisualisationSettings().drawForPositionSelection ||
               fromLane->getNet()->getViewNet()->getVisualisationSettings().drawForRectangleSelection) {
        return true;
    } else {
        // declare lane2lane
        const std::pair<const GNELane*, const GNELane*> lane2lane(fromLane, toLane);
        // check lane2lane
        if (myLane2laneDrawedElements.count(lane2lane) > 0) {
            // check tag
            if (myLane2laneDrawedElements.at(lane2lane).count(tag) > 0) {
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


const GNEPathManager::PathElement*
GNEPathManager::getPathElement(const GUIGlObject* GLObject) const {
    // first parse pathElement
    const auto pathElement = dynamic_cast<const GNEPathManager::PathElement*>(GLObject);
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


const std::vector<GNEPathManager::Segment*>&
GNEPathManager::getPathElementSegments(GNEPathManager::PathElement* pathElement) const {
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
GNEPathManager::isPathValid(const PathElement* pathElement) const {
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
GNEPathManager::calculatePathEdges(PathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEEdge*> edges) {
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
        // declare lane segments
        std::vector<Segment*> laneSegments;
        // calculate Dijkstra path
        const std::vector<GNEEdge*> path = myPathCalculator->calculateDijkstraPath(vClass, edges);
        // continue if path isn't empty
        if (path.size() > 0) {
            // reserve
            segments.reserve(2 * (int)path.size() - 1);
            laneSegments.reserve(path.size());
            // iterate over path
            for (int i = 0; i < (int)path.size(); i++) {
                // get first and last segment flags
                const bool firstSegment = (i == 0);
                const bool lastSegment = (i == ((int)path.size() - 1));
                // get first allowed lane
                const GNELane* lane = path.at(i)->getLaneByAllowedVClass(vClass);
                // create segments
                Segment* laneSegment = new Segment(this, pathElement, lane, firstSegment, lastSegment);
                // add it into segment and laneSegment vectors
                segments.push_back(laneSegment);
                laneSegments.push_back(laneSegment);
                // continue if this isn't the last edge
                if (!lastSegment) {
                    // obtain next lane
                    const GNELane* nextLane = path.at(i + 1)->getLaneByAllowedVClass(vClass);
                    // create junction segments
                    Segment* junctionSegment = new Segment(this, pathElement, path.at(i)->getParentJunctions().at(1), lane, nextLane);
                    // add it into segment vector
                    segments.push_back(junctionSegment);
                }
            }
            // get lane segment index
            const int laneSegmentIndex = (int)((double)laneSegments.size() * 0.5);
            // mark middle label as label segment
            laneSegments.at(laneSegmentIndex)->markSegmentLabel();
        } else {
            // create first segment
            Segment* firstSegment = new Segment(this, pathElement, edges.front()->getLaneByAllowedVClass(vClass), true, false);
            // mark segment as label segment
            firstSegment->markSegmentLabel();
            // add to segments
            segments.push_back(firstSegment);
            // create last segment
            Segment* lastSegment = new Segment(this, pathElement, edges.back()->getLaneByAllowedVClass(vClass), false, true);
            // add to segments
            segments.push_back(lastSegment);
            // set previous and next segment for vinculating first and last segment with a red line
            firstSegment->setNextSegment(lastSegment);
            lastSegment->setPreviousSegment(firstSegment);
        }
        // add segment in path
        myPaths[pathElement] = segments;
    }
}


void
GNEPathManager::calculatePathLanes(PathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNELane*> lanes) {
    // declare edges
    std::vector<GNEEdge*> edges;
    // reserve edges
    edges.reserve(lanes.size());
    // get parent of every lane
    for (const auto& lane : lanes) {
        edges.push_back(lane->getParentEdge());
    }
    // calculate path edges
    calculatePathEdges(pathElement, vClass, edges);
}


void
GNEPathManager::calculatePathJunctions(PathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEJunction*> junctions) {
    // first calculate edge path between both elements
    const auto junctionPath = myPathCalculator->calculateDijkstraPath(vClass, junctions.front(), junctions.back());
    // if exist, then calculate edgePath between the first edges
    if (junctionPath.size() > 0) {
        // calculate path edges
        calculatePathEdges(pathElement, vClass, {junctionPath.front(), junctionPath.back()});
    }
}


void
GNEPathManager::calculateConsecutivePathEdges(PathElement* pathElement, SUMOVehicleClass vClass, const std::vector<GNEEdge*> edges) {
    // declare lane vector
    std::vector<GNELane*> lanes;
    // reserve lanes
    lanes.reserve(edges.size());
    // get first allowed lane of every edge
    for (const auto& edge : edges) {
        lanes.push_back(edge->getLaneByAllowedVClass(vClass));
    }
    // calculate consecutive path lanes
    calculateConsecutivePathLanes(pathElement, lanes);
}


void
GNEPathManager::calculateConsecutivePathLanes(PathElement* pathElement, const std::vector<GNELane*> lanes) {
    // check if path element exist already in myPaths
    if (myPaths.find(pathElement) != myPaths.end()) {
        // delete segments
        for (const auto& segment : myPaths.at(pathElement)) {
            delete segment;
        }
        // remove path element from myPaths
        myPaths.erase(pathElement);
    }
    // continue depending of number of lanes
    if (lanes.size() > 0) {
        // declare segment vector
        std::vector<Segment*> segments;
        // declare lane segments
        std::vector<Segment*> laneSegments;
        // reserve
        segments.reserve(2 * (int)lanes.size() - 1);
        laneSegments.reserve(lanes.size());
        // iterate over lanes
        for (int i = 0; i < (int)lanes.size(); i++) {
            // get first and last segment flags
            const bool firstSegment = (i == 0);
            const bool lastSegment = (i == ((int)lanes.size() - 1));
            // create segments
            Segment* laneSegment = new Segment(this, pathElement, lanes.at(i), firstSegment, lastSegment);
            // add it into segment vector
            segments.push_back(laneSegment);
            laneSegments.push_back(laneSegment);
            // continue if this isn't the last lane
            if (!lastSegment) {
                // obtain next lane
                const GNELane* nextLane = lanes.at(i + 1);
                // create junction segments
                Segment* junctionSegment = new Segment(this, pathElement, lanes.at(i)->getParentEdge()->getParentJunctions().at(1), lanes.at(i), nextLane);
                // add it into segment vector
                segments.push_back(junctionSegment);
            }
        }
        // get lane segment index
        const int laneSegmentIndex = (int)((double)laneSegments.size() * 0.5);
        // mark middle label as label segment
        laneSegments.at(laneSegmentIndex)->markSegmentLabel();
        // add segment in path
        myPaths[pathElement] = segments;
    }
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
        int numRoutes = 0;
        // first draw selected elements (for drawing over other elements)
        for (const auto& segment : myLaneSegments.at(lane)) {
            if (segment->getPathElement()->isPathElementSelected()) {
                // draw segment
                segment->getPathElement()->drawPartialGL(s, lane, segment, 2);
                // check if path element is a route
                if (segment->getPathElement()->isRoute()) {
                    numRoutes++;
                }
            }
        }
        // now draw non selected elements
        for (const auto& segment : myLaneSegments.at(lane)) {
            if (!segment->getPathElement()->isPathElementSelected()) {
                // draw segment
                segment->getPathElement()->drawPartialGL(s, lane, segment, 0);
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
GNEPathManager::drawJunctionPathElements(const GUIVisualizationSettings& s, const GNEJunction* junction) {
    if (myJunctionSegments.count(junction) > 0) {
        // first draw selected elements (for drawing over other elements)
        for (const auto& segment : myJunctionSegments.at(junction)) {
            if (segment->getPathElement()->isPathElementSelected()) {
                segment->getPathElement()->drawPartialGL(s, segment->getPreviousLane(), segment->getNextLane(), segment, 0);
            }
        }
        // now draw non selected elements
        for (const auto& segment : myJunctionSegments.at(junction)) {
            if (!segment->getPathElement()->isPathElementSelected()) {
                segment->getPathElement()->drawPartialGL(s, segment->getPreviousLane(), segment->getNextLane(), segment, 0);
            }
        }
    }
}


void
GNEPathManager::forceDrawPath(const GUIVisualizationSettings& s, const PathElement* pathElement) const {
    // draw all lane segments
    for (const auto& laneSegment : myLaneSegments) {
        for (const auto& segment : laneSegment.second) {
            if (segment->getPathElement() == pathElement) {
                pathElement->drawPartialGL(s, laneSegment.first, segment, 0);
            }
        }
    }
    // draw all junction segments
    for (const auto& junctionSegment : myJunctionSegments) {
        for (const auto& segment : junctionSegment.second) {
            if (segment->getPathElement() == pathElement) {
                segment->getPathElement()->drawPartialGL(s, segment->getPreviousLane(), segment->getNextLane(), segment, 0);
            }
        }
    }
}


void
GNEPathManager::invalidateLanePath(const GNELane* lane) {
    // declare vector for path elements to compute
    std::vector<PathElement*> pathElementsToCompute;
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
    std::vector<PathElement*> pathElementsToCompute;
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
GNEPathManager::clearDemandPaths() {
    // declare iterator
    auto it = myPaths.begin();
    // iterate over paths
    while (it != myPaths.end()) {
        if (it->first->isDemandElement()) {
            // delete all segments
            for (const auto& segment : it->second) {
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
    // check if segment has a lane
    if (segment->getLane()) {
        // remove segment from segments associated with lane
        if (myLaneSegments.at(segment->getLane()).find(segment) != myLaneSegments.at(segment->getLane()).end()) {
            myLaneSegments.at(segment->getLane()).erase(segment);
        }
        // clear lane if doesn't have more segments
        if (myLaneSegments.at(segment->getLane()).empty()) {
            myLaneSegments.erase(segment->getLane());
        }
    }
    if (segment->getJunction()) {
        // remove segment from segments associated with junction
        if (myJunctionSegments.at(segment->getJunction()).find(segment) != myJunctionSegments.at(segment->getJunction()).end()) {
            myJunctionSegments.at(segment->getJunction()).erase(segment);
        }
        // clear junction if doesn't have more segments
        if (myJunctionSegments.at(segment->getJunction()).empty()) {
            myJunctionSegments.erase(segment->getJunction());
        }
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

/****************************************************************************/
