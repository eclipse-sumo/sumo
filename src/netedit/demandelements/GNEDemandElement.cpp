/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDemandElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
/// @version $Id$
///
// A abstract class for demand elements
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netbuild/NBNetBuilder.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEJunction.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/router/DijkstraRouter.h>

#include "GNEDemandElement.h"

// ===========================================================================
// static members
// ===========================================================================

GNEDemandElement::RouteCalculator* GNEDemandElement::myRouteCalculatorInstance = nullptr;

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDemandElement::DemandElementGeometry - methods
// ---------------------------------------------------------------------------

GNEDemandElement::DemandElementGeometry::DemandElementGeometry() {}


void
GNEDemandElement::DemandElementGeometry::clearGeometry() {
    shape.clear();
    shapeRotations.clear();
    shapeLengths.clear();
}


void
GNEDemandElement::DemandElementGeometry::calculateShapeRotationsAndLengths() {
    // Get number of parts of the shape
    int numberOfSegments = (int)shape.size() - 1;
    // If number of segments is more than 0
    if (numberOfSegments >= 0) {
        // Reserve memory (To improve efficiency)
        shapeRotations.reserve(numberOfSegments);
        shapeLengths.reserve(numberOfSegments);
        // Calculate lengths and rotations for every shape
        for (int i = 0; i < numberOfSegments; i++) {
            shapeRotations.push_back(calculateRotation(shape[i], shape[i + 1]));
            shapeLengths.push_back(calculateLength(shape[i], shape[i + 1]));
        }
    }
}

// ---------------------------------------------------------------------------
// GNEDemandElement::DemandElementSegmentGeometry::Segment - methods
// ---------------------------------------------------------------------------

GNEDemandElement::DemandElementSegmentGeometry::Segment::Segment(const GNEDemandElement* _element, const GNEEdge* _edge, const GNELane* _lane,
        const Position& pos, const double rotation, const double length, const bool _valid) :
    element(_element),
    edge(_edge),
    lane(_lane),
    junction(nullptr),
    valid(_valid),
    shape({pos}),
    shapeRotations({rotation}),
    shapeLengths({length}) {
}


GNEDemandElement::DemandElementSegmentGeometry::Segment::Segment(const GNEDemandElement* _element, const GNEEdge* _edge, const GNELane* _lane,
        const PositionVector& _shape, const std::vector<double> &_shapeRotations, const std::vector<double> &_shapeLengths, const bool _valid) :
    element(_element),
    edge(_edge),
    lane(_lane),
    junction(nullptr),
    valid(_valid),
    shape(_shape),
    shapeRotations(_shapeRotations),
    shapeLengths(_shapeLengths) {
}


GNEDemandElement::DemandElementSegmentGeometry::Segment::Segment(const GNEDemandElement* _element, const GNEJunction* _junction, const GNELane* _lane,
        const PositionVector& _shape, const std::vector<double> &_shapeRotations, const std::vector<double> &_shapeLengths, const bool _valid) :
    element(_element),
    edge(nullptr),
    lane(_lane),
    junction(_junction),
    valid(_valid),
    shape(_shape),
    shapeRotations(_shapeRotations),
    shapeLengths(_shapeLengths) {
}

// ---------------------------------------------------------------------------
// GNEDemandElement::DemandElementGeometry - methods
// ---------------------------------------------------------------------------

GNEDemandElement::DemandElementSegmentGeometry::DemandElementSegmentGeometry() {}


void
GNEDemandElement::DemandElementSegmentGeometry::insertPartialEdgeSegment(const GNEDemandElement* element, const GNEEdge* edge, const GNELane* lane,
    const Position pos, double rotation, double length, const bool valid) {
    // check if we have to insert a new segment or update last inserted element
    if (myShapeSegments.empty() || (myShapeSegments.back().edge != edge)) {
        // add new segment
        myShapeSegments.push_back(Segment(element, edge, lane, pos, rotation, length, valid));
    } else {
        // update last shape segment
        myShapeSegments.back().shape.push_back(pos);
        myShapeSegments.back().shapeRotations.push_back(rotation);
        myShapeSegments.back().shapeLengths.push_back(length);
    }
}


void 
GNEDemandElement::DemandElementSegmentGeometry::insertEdgeSegment(const GNEDemandElement* element, const GNEEdge* edge, const GNELane* lane,
    const PositionVector& laneShape, const std::vector<double> &laneShapeRotations, const std::vector<double> &laneShapeLengths, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(element, edge, lane, laneShape, laneShapeRotations, laneShapeLengths, valid));
}


void
GNEDemandElement::DemandElementSegmentGeometry::insertLane2LaneSegment(const GNEDemandElement* element, const GNEJunction* junction, const GNELane* lane,
    const PositionVector& laneShape, const std::vector<double> &laneShapeRotations, const std::vector<double> &laneShapeLengths, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(element, junction, lane, laneShape, laneShapeRotations, laneShapeLengths, valid));
}



void 
GNEDemandElement::DemandElementSegmentGeometry::updatePartialEdgeSegment(const int segmentIndex, const int shapeIndex, const Position newPos, double newRotation, double newLength) {
    if ((myShapeSegments.at(segmentIndex).shape.size() <= shapeIndex) || 
        (myShapeSegments.at(segmentIndex).shapeRotations.size() <= shapeIndex) || 
        (myShapeSegments.at(segmentIndex).shapeLengths.size() <= shapeIndex)) {
        throw ProcessError("Invalid index");
    } else {
        myShapeSegments.at(segmentIndex).shape[shapeIndex] = newPos;
        myShapeSegments.at(segmentIndex).shapeRotations[shapeIndex] = newRotation;
        myShapeSegments.at(segmentIndex).shapeLengths[shapeIndex] = newLength;
    }
}


void 
GNEDemandElement::DemandElementSegmentGeometry::updateLastPartialEdgeSegment(const int segmentIndex, const Position newPos) {
    myShapeSegments.at(segmentIndex).shape[myShapeSegments.at(segmentIndex).shape.size()-1] = newPos;
}


void 
GNEDemandElement::DemandElementSegmentGeometry::updateSegment(const int segmentIndex, const PositionVector& newLaneShape, const std::vector<double> &newLaneShapeRotations, const std::vector<double> &newLaneShapeLengths) {
    myShapeSegments.at(segmentIndex).shape = newLaneShape;
    myShapeSegments.at(segmentIndex).shapeRotations = newLaneShapeRotations;
    myShapeSegments.at(segmentIndex).shapeLengths = newLaneShapeLengths;
}


void
GNEDemandElement::DemandElementSegmentGeometry::clearDemandElementSegmentGeometry() {
    // clear segments
    myShapeSegments.clear();
}


void 
GNEDemandElement::DemandElementSegmentGeometry::closePartialEdgeSegment(const Position &lastPosition) {
    // first check that there are shape segments
    if (myShapeSegments.size() > 0) {
        // check that all containers have exactly the same size (because shape must be +1 greather than lenghts and rotations)
        if ((myShapeSegments.back().shape.size() > 0) &&
            (myShapeSegments.back().shape.size() == myShapeSegments.back().shapeRotations.size()) &&
            (myShapeSegments.back().shape.size() == myShapeSegments.back().shapeLengths.size())) {
            // add last position
            myShapeSegments.back().shape.push_back(lastPosition);
        }
    }
}


const Position &
GNEDemandElement::DemandElementSegmentGeometry::getFirstPosition() const {
    if ((myShapeSegments.size() > 0) && (myShapeSegments.front().shape.size() > 0)) {
        return myShapeSegments.front().shape.front();
    } else {
        return Position::INVALID;
    }
}

const Position &
GNEDemandElement::DemandElementSegmentGeometry::getLastPosition() const {
    if ((myShapeSegments.size() > 0) && (myShapeSegments.back().shape.size() > 0)) {
        return myShapeSegments.back().shape.back();
    } else {
        return Position::INVALID;
    }
}


double 
GNEDemandElement::DemandElementSegmentGeometry::getFirstRotation() const {
    if ((myShapeSegments.size() > 0) && (myShapeSegments.front().shapeRotations.size() > 0)) {
        return myShapeSegments.front().shapeRotations.front();
    } else {
        return 0;
    }
}


std::vector<GNEDemandElement::DemandElementSegmentGeometry::Segment>::const_iterator
GNEDemandElement::DemandElementSegmentGeometry::begin() const {
    return myShapeSegments.cbegin();
}


std::vector<GNEDemandElement::DemandElementSegmentGeometry::Segment>::const_iterator
GNEDemandElement::DemandElementSegmentGeometry::end() const {
    return myShapeSegments.cend();
}


const GNEDemandElement::DemandElementSegmentGeometry::Segment&
GNEDemandElement::DemandElementSegmentGeometry::front() const {
    return myShapeSegments.front();
}


const GNEDemandElement::DemandElementSegmentGeometry::Segment&
GNEDemandElement::DemandElementSegmentGeometry::back() const {
    return myShapeSegments.back();
}


int 
GNEDemandElement::DemandElementSegmentGeometry::size() const {
    return (int)myShapeSegments.size();
}

// ---------------------------------------------------------------------------
// GNEDemandElement::RouteCalculator - methods
// ---------------------------------------------------------------------------

GNEDemandElement::RouteCalculator::RouteCalculator(GNENet* net) :
    myNet(net) {
    myDijkstraRouter = new DijkstraRouter<NBRouterEdge, NBVehicle, SUMOAbstractRouter<NBRouterEdge, NBVehicle> >(
        myNet->getNetBuilder()->getEdgeCont().getAllRouterEdges(),
        true, &NBRouterEdge::getTravelTimeStatic, nullptr, true);
}


GNEDemandElement::RouteCalculator::~RouteCalculator() {
    delete myDijkstraRouter;
}


void
GNEDemandElement::RouteCalculator::updateDijkstraRouter() {
    // simply delete and create myDijkstraRouter again
    if (myDijkstraRouter) {
        delete myDijkstraRouter;
    }
    myDijkstraRouter = new DijkstraRouter<NBRouterEdge, NBVehicle, SUMOAbstractRouter<NBRouterEdge, NBVehicle> >(
        myNet->getNetBuilder()->getEdgeCont().getAllRouterEdges(),
        true, &NBRouterEdge::getTravelTimeStatic, nullptr, true);
}


std::vector<GNEEdge*>
GNEDemandElement::RouteCalculator::calculateDijkstraRoute(SUMOVehicleClass vClass, const std::vector<GNEEdge*>& partialEdges) const {
    // declare a solution vector
    std::vector<GNEEdge*> solution;
    // calculate route depending of number of partial edges
    if (partialEdges.size() == 1) {
        // if there is only one partialEdges, route has only one edge
        solution.push_back(partialEdges.front());
    } else {
        // declare temporal vehicle
        NBVehicle tmpVehicle("temporalNBVehicle", vClass);
        // obtain pointer to GNENet
        GNENet* net = partialEdges.front()->getNet();
        // iterate over every selected edges
        for (int i = 1; i < (int)partialEdges.size(); i++) {
            // declare a temporal route in which save route between two last edges
            std::vector<const NBRouterEdge*> partialRoute;
            myDijkstraRouter->compute(partialEdges.at(i - 1)->getNBEdge(), partialEdges.at(i)->getNBEdge(), &tmpVehicle, 10, partialRoute);
            // save partial route in solution
            for (const auto& j : partialRoute) {
                solution.push_back(net->retrieveEdge(j->getID()));
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
GNEDemandElement::RouteCalculator::calculateDijkstraRoute(GNENet* net, SUMOVehicleClass vClass, const std::vector<std::string>& partialEdgesStr) const {
    // declare a vector of GNEEdges
    std::vector<GNEEdge*> partialEdges;
    partialEdges.reserve(partialEdgesStr.size());
    // convert to vector of GNEEdges
    for (const auto& i : partialEdgesStr) {
        partialEdges.push_back(net->retrieveEdge(i));
    }
    // calculate DijkstraRoute using partialEdges
    return calculateDijkstraRoute(vClass, partialEdges);
}


bool
GNEDemandElement::RouteCalculator::areEdgesConsecutives(SUMOVehicleClass vClass, GNEEdge* from, GNEEdge* to) const {
    // the same edge cannot be consecutive of itself
    if (from == to) {
        return false;
    }
    // for pedestrian edges are always consecutives
    if (vClass == SVC_PEDESTRIAN) {
        return true;
    }
    // obtain NBEdges from both edges
    NBEdge* nbFrom = from->getNBEdge();
    NBEdge* nbTo = to->getNBEdge();
    // iterate over all connections of NBFrom
    for (NBEdge::Connection c : nbFrom->getConnectionsFromLane(-1, nbTo, -1)) {
        //check if given VClass is allowed for from and to lanes
        if ((nbFrom->getPermissions(c.fromLane) & nbTo->getPermissions(c.toLane) & vClass) == vClass) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// GNEDemandElement - methods
// ---------------------------------------------------------------------------

GNEDemandElement::GNEDemandElement(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag,
                                   const std::vector<GNEEdge*>& edgeParents,
                                   const std::vector<GNELane*>& laneParents,
                                   const std::vector<GNEShape*>& shapeParents,
                                   const std::vector<GNEAdditional*>& additionalParents,
                                   const std::vector<GNEDemandElement*>& demandElementParents,
                                   const std::vector<GNEEdge*>& edgeChildren,
                                   const std::vector<GNELane*>& laneChildren,
                                   const std::vector<GNEShape*>& shapeChildren,
                                   const std::vector<GNEAdditional*>& additionalChildren,
                                   const std::vector<GNEDemandElement*>& demandElementChildren) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag),
    GNEHierarchicalElementParents(this, edgeParents, laneParents, shapeParents, additionalParents, demandElementParents),
    GNEHierarchicalElementChildren(this, edgeChildren, laneChildren, shapeChildren, additionalChildren, demandElementChildren),
    myViewNet(viewNet) {
}


GNEDemandElement::GNEDemandElement(GNEDemandElement* demandElementParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag,
                                   const std::vector<GNEEdge*>& edgeParents,
                                   const std::vector<GNELane*>& laneParents,
                                   const std::vector<GNEShape*>& shapeParents,
                                   const std::vector<GNEAdditional*>& additionalParents,
                                   const std::vector<GNEDemandElement*>& demandElementParents,
                                   const std::vector<GNEEdge*>& edgeChildren,
                                   const std::vector<GNELane*>& laneChildren,
                                   const std::vector<GNEShape*>& shapeChildren,
                                   const std::vector<GNEAdditional*>& additionalChildren,
                                   const std::vector<GNEDemandElement*>& demandElementChildren) :
    GUIGlObject(type, demandElementParent->generateChildID(tag)),
    GNEAttributeCarrier(tag),
    GNEHierarchicalElementParents(this, edgeParents, laneParents, shapeParents, additionalParents, demandElementParents),
    GNEHierarchicalElementChildren(this, edgeChildren, laneChildren, shapeChildren, additionalChildren, demandElementChildren),
    myViewNet(viewNet) {
}


std::string
GNEDemandElement::generateChildID(SumoXMLTag childTag) {
    int counter = (int)getDemandElementChildren().size();
    while (myViewNet->getNet()->retrieveDemandElement(childTag, getID() + toString(childTag) + toString(counter), false) != nullptr) {
        counter++;
    }
    return (getID() + toString(childTag) + toString(counter));
}


GNEDemandElement::~GNEDemandElement() {}


const GNEDemandElement::DemandElementGeometry&
GNEDemandElement::getDemandElementGeometry() const {
    return myDemandElementGeometry;
}


const GNEDemandElement::DemandElementSegmentGeometry&
GNEDemandElement::getDemandElementSegmentGeometry() const {
    return myDemandElementSegmentGeometry;
}


bool
GNEDemandElement::isDemandElementValid() const {
    return true;
}


std::string
GNEDemandElement::getDemandElementProblem() const {
    return "";
}


void
GNEDemandElement::fixDemandElementProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


void
GNEDemandElement::openDemandElementDialog() {
    throw InvalidArgument(getTagStr() + " doesn't have an demand element dialog");
}


std::string
GNEDemandElement::getBegin() const {
    throw InvalidArgument(getTagStr() + " doesn't have an begin time");
}


GNEViewNet*
GNEDemandElement::getViewNet() const {
    return myViewNet;
}


void
GNEDemandElement::createRouteCalculatorInstance(GNENet* net) {
    if (myRouteCalculatorInstance == nullptr) {
        myRouteCalculatorInstance = new RouteCalculator(net);
    } else {
        throw ProcessError("Instance already created");
    }
}


void
GNEDemandElement::deleteRouteCalculatorInstance() {
    if (myRouteCalculatorInstance) {
        delete myRouteCalculatorInstance;
        myRouteCalculatorInstance = nullptr;
    } else {
        throw ProcessError("Instance wasn't created");
    }
}


GNEDemandElement::RouteCalculator*
GNEDemandElement::getRouteCalculatorInstance() {
    if (myRouteCalculatorInstance) {
        return myRouteCalculatorInstance;
    } else {
        throw ProcessError("Instance wasn't created");
    }
}


GUIGLObjectPopupMenu*
GNEDemandElement::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    // buld menu commands for names
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " name to clipboard").c_str(), nullptr, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " typed name to clipboard").c_str(), nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myViewNet->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open demand element dialog
    if (myTagProperty.hasDialog()) {
        new FXMenuCommand(ret, ("Open " + getTagStr() + " Dialog").c_str(), getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    new FXMenuCommand(ret, ("Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y())).c_str(), nullptr, nullptr, 0);
    return ret;
}


GUIParameterTableWindow*
GNEDemandElement::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, myTagProperty.getNumberOfAttributes());
    // Iterate over attributes
    for (const auto& i : myTagProperty) {
        // Add attribute and set it dynamic if aren't unique
        if (i.isUnique()) {
            ret->mkItem(i.getAttrStr().c_str(), false, getAttribute(i.getAttr()));
        } else {
            ret->mkItem(i.getAttrStr().c_str(), true, getAttribute(i.getAttr()));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


bool
GNEDemandElement::isRouteValid(const std::vector<GNEEdge*>& edges, bool report) {
    if (edges.size() == 0) {
        // routes cannot be empty
        return false;
    } else if (edges.size() == 1) {
        // routes with a single edge are valid
        return true;
    } else {
        // iterate over edges to check that compounds a chain
        auto it = edges.begin();
        while (it != edges.end() - 1) {
            GNEEdge* currentEdge = *it;
            GNEEdge* nextEdge = *(it + 1);
            // consecutive edges aren't allowed
            if (currentEdge->getID() == nextEdge->getID()) {
                return false;
            }
            // make sure that edges are consecutives
            if (std::find(currentEdge->getGNEJunctionDestiny()->getGNEOutgoingEdges().begin(),
                          currentEdge->getGNEJunctionDestiny()->getGNEOutgoingEdges().end(),
                          nextEdge) == currentEdge->getGNEJunctionDestiny()->getGNEOutgoingEdges().end()) {
                if (report) {
                    WRITE_WARNING("Parameter 'Route' invalid. " + currentEdge->getTagStr() + " '" + currentEdge->getID() +
                                  "' ins't consecutive to " + nextEdge->getTagStr() + " '" + nextEdge->getID() + "'");
                }
                return false;
            }
            it++;
        }
    }
    return true;
}


const std::string&
GNEDemandElement::getDemandElementID() const {
    return getMicrosimID();
}


bool
GNEDemandElement::isValidDemandElementID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidVehicleID(newID) && (myViewNet->getNet()->retrieveDemandElement(myTagProperty.getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


void
GNEDemandElement::changeDemandElementID(const std::string& newID) {
    if (myViewNet->getNet()->retrieveDemandElement(myTagProperty.getTag(), newID, false) != nullptr) {
        throw InvalidArgument("An DemandElement with tag " + getTagStr() + " and ID = " + newID + " already exists");
    } else {
        // Save old ID
        std::string oldID = getMicrosimID();
        // set New ID
        setMicrosimID(newID);
        // update demand element ID in the container of net
        myViewNet->getNet()->updateDemandElementID(oldID, this);
    }
}


void 
GNEDemandElement::calculateGeometricPath(const std::vector<GNEEdge*> &edges, double startPos, double endPos, 
    const Position &extraFirstPosition, const Position &extraLastPosition) {
    // clear geometry
    myDemandElementSegmentGeometry.clearDemandElementSegmentGeometry();
    // first check that there is edge parents
    if (edges.size() > 0) {
        // calculate depending if both from and to edges are the same
        if (edges.size() == 1) {
            // obtain first (and single) Lane
            GNELane* singleLane = getFirstAllowedVehicleLane();
            // if obtained lane is null, then force to use first lane
            if (singleLane == nullptr) {
                singleLane = edges.front()->getLanes().front();
            }
            // filter start and end pos
            adjustStartPosGeometricPath(startPos, singleLane, endPos, singleLane);
            // set geometry depending of start and end positions
            if ((startPos == -1) && (endPos == -1)) {
                // check if both extra positions are invalid
                if ((extraFirstPosition == Position::INVALID) && (extraLastPosition == Position::INVALID)) {
                    // add entire lane geometry geometry
                    myDemandElementSegmentGeometry.insertEdgeSegment(this, edges.at(0), singleLane,
                        singleLane->getLaneShape(), 
                        singleLane->getShapeRotations(), 
                        singleLane->getShapeLengths(), true);
                } else if (singleLane->getLaneShape().size() > 0) {
                    // declare a Net Element Geometry
                    GNENetElement::NetElementGeometry subLane;
                    // set shape lane
                    subLane.shape = singleLane->getLaneShape();
                    // check if we have to add an extra first position
                    if (extraFirstPosition != Position::INVALID) {
                        subLane.shape.push_front(extraFirstPosition);
                    }
                     // check if we have to add an extra last position
                    if (extraLastPosition != Position::INVALID) {
                        subLane.shape.push_back(extraLastPosition);
                    }
                    // calculate shape rotations and lenghts
                    subLane.calculateShapeRotationsAndLengths();
                    // add sublane geometry
                    for (int i = 0; i < ((int)subLane.shape.size() - 1); i++) {
                        myDemandElementSegmentGeometry.insertPartialEdgeSegment(this, edges.at(0), singleLane,
                            subLane.shape[i], 
                            subLane.shapeRotations[i], 
                            subLane.shapeLengths[i], true);
                    }
                    // close partial edge segment
                    myDemandElementSegmentGeometry.closePartialEdgeSegment(subLane.shape.back());
                }
            } else if (singleLane->getLaneShape().size() > 0) {
                // declare a Net Element Geometry
                GNENetElement::NetElementGeometry subLane;
                // set shape lane
                subLane.shape = singleLane->getLaneShape();
                if (subLane.shape.length() > (2*POSITION_EPS)) {
                    if (startPos && endPos) {
                        // split lane
                        subLane.shape = subLane.shape.getSubpart(startPos, endPos);
                    } else if (startPos) {
                        // split lane
                        subLane.shape = subLane.shape.splitAt(startPos).second;
                    } else if (endPos) {
                        // split lane
                        subLane.shape = subLane.shape.splitAt(endPos).first;
                    }
                }
                // check if we have to add an extra first position
                if (extraFirstPosition != Position::INVALID) {
                    subLane.shape.push_front(extraFirstPosition);
                }
                    // check if we have to add an extra last position
                if (extraLastPosition != Position::INVALID) {
                    subLane.shape.push_back(extraLastPosition);
                }
                // calculate shape rotations and lenghts
                subLane.calculateShapeRotationsAndLengths();
                // add sublane geometry
                for (int i = 0; i < ((int)subLane.shape.size() - 1); i++) {
                    myDemandElementSegmentGeometry.insertPartialEdgeSegment(this, edges.at(0), singleLane,
                        subLane.shape[i], 
                        subLane.shapeRotations[i], 
                        subLane.shapeLengths[i], true);
                }
                // close partial edge segment
                myDemandElementSegmentGeometry.closePartialEdgeSegment(subLane.shape.back());
            }
        } else {
            // declare vector of lanes
            std::vector<GNELane*> lanes;
            // reserve space
            lanes.reserve(edges.size());
            // obtain lanes by VClass
            for (auto edgeParent = edges.begin(); edgeParent != edges.end(); edgeParent++) {
                GNELane* allowedLane = nullptr;
                if (edgeParent == edges.begin()) {
                    allowedLane = getFirstAllowedVehicleLane();
                } else if (edgeParent == (edges.end() - 1)) {
                    allowedLane = getLastAllowedVehicleLane();
                } else if (myTagProperty.isRide()) {
                    // obtain first disallowed lane (special case for rides)
                    allowedLane = (*edgeParent)->getLaneByDisallowedVClass(getVClass());
                } else {
                    // obtain first allowed lane
                    allowedLane = (*edgeParent)->getLaneByAllowedVClass(getVClass());
                }
                // if there isn't allowed lane, then use first lane
                if (allowedLane == nullptr) {
                    allowedLane = (*edgeParent)->getLanes().front();
                }
                // add it to lanes
                lanes.push_back(allowedLane);
            }
            // iterate over obtained lanes
            for (int i = 0; i < (int)lanes.size(); i++) {
                // get lane (only for code readability)
                const GNELane* lane = lanes.at(i);
                // first check that lane shape isn't empty
                if (lane->getLaneShape().size() > 0) {
                    // check if first or last lane must be splitted
                    if ((lanes.at(i) == lanes.front()) && (startPos != -1)) {
                        // filter start position
                        adjustStartPosGeometricPath(startPos, lanes.at(i), endPos, nullptr);
                        // declare a Net Element Geometry
                        GNENetElement::NetElementGeometry subLane;
                        // set shape lane
                        subLane.shape = lanes.at(i)->getLaneShape();
                        // split lane
                        if (lanes.at(i)->getLaneShape().length() > (2*POSITION_EPS)) {
                            subLane.shape = subLane.shape.splitAt(startPos).second;
                        }
                        // check if we have to add an extra first position
                        if (extraFirstPosition != Position::INVALID) {
                            subLane.shape.push_front(extraFirstPosition);
                        }
                        // calculate shape rotations and lenghts
                        subLane.calculateShapeRotationsAndLengths();
                        // add sublane geometry
                        for (int j = 0; j < ((int)subLane.shape.size() - 1); j++) {
                            myDemandElementSegmentGeometry.insertPartialEdgeSegment(this, &lanes.at(i)->getParentEdge(), lanes.at(i),
                                subLane.shape[j], 
                                subLane.shapeRotations[j], 
                                subLane.shapeLengths[j], true);
                        }
                        // close partial edge segment
                        myDemandElementSegmentGeometry.closePartialEdgeSegment(subLane.shape.back());
                    } else if ((lane == lanes.back()) && (endPos != -1)) {
                        // filter end position
                        adjustStartPosGeometricPath(startPos, nullptr, endPos, lane);
                        // declare a Net Element Geometry
                        GNENetElement::NetElementGeometry subLane;
                        // set shape lane
                        subLane.shape = lane->getLaneShape();
                        // split lane
                        if (lane->getLaneShape().length() > (2*POSITION_EPS)) {
                            subLane.shape = subLane.shape.splitAt(endPos).first;
                        }
                        // check if we have to add an extra last position
                        if (extraLastPosition != Position::INVALID) {
                            subLane.shape.push_back(extraLastPosition);
                        }
                        // calculate shape rotations and lenghts
                        subLane.calculateShapeRotationsAndLengths();
                        // add sublane geometry
                        for (int j = 0; j < ((int)subLane.shape.size() - 1); j++) {
                            myDemandElementSegmentGeometry.insertPartialEdgeSegment(this, &lane->getParentEdge(), lanes.at(i),
                                subLane.shape[j], 
                                subLane.shapeRotations[j], 
                                subLane.shapeLengths[j], true);
                        }
                        // close partial edge segment
                        myDemandElementSegmentGeometry.closePartialEdgeSegment(subLane.shape.back());
                    } else {
                        // add entire lane geometry
                        myDemandElementSegmentGeometry.insertEdgeSegment(this, &lane->getParentEdge(), lanes.at(i),
                            lane->getLaneShape(), 
                            lane->getShapeRotations(), 
                            lane->getShapeLengths(), true);
                    }
                }
                // now continue with connection
                if ((i+1) < (int)lanes.size()) {
                    // obtain next lane
                    const GNELane* const nextLane = lanes.at(i+1);
                    // check that next lane exist
                    if (lane->getLane2laneConnections().shape.count(nextLane) > 0) {
                        // add lane2laneConnection shape
                        myDemandElementSegmentGeometry.insertLane2LaneSegment(this, lane->getParentEdge().getGNEJunctionDestiny(), lane,
                            lane->getLane2laneConnections().shape.at(nextLane), 
                            lane->getLane2laneConnections().shapeRotations.at(nextLane), 
                            lane->getLane2laneConnections().shapeLengths.at(nextLane), true);
                    }
                }
            }
        }
    }
}


void 
GNEDemandElement::updateGeometricPath(const GNEEdge* edge, double startPos, double endPos, 
    const Position &extraFirstPosition, const Position &extraLastPosition) {
    // calculate depending if both from and to edges are the same
    if ((myDemandElementSegmentGeometry.size() == 1) && (myDemandElementSegmentGeometry.front().edge == edge)) {
        // obtain first (and single) Lane
        GNELane* singleLane = getFirstAllowedVehicleLane();
        // if obtained lane is null, then force to use first lane
        if (singleLane == nullptr) {
            singleLane = edge->getLanes().front();
        }
        // filter start and end pos
        adjustStartPosGeometricPath(startPos, singleLane, endPos, singleLane);
        // set geometry depending of start and end positions
        if ((startPos == -1) && (endPos == -1)) {
            // check if both extra positions are invalid
            if ((extraFirstPosition == Position::INVALID) && (extraLastPosition == Position::INVALID)) {
                // add entire lane geometry geometry
                myDemandElementSegmentGeometry.updateSegment(0,
                    singleLane->getLaneShape(), 
                    singleLane->getShapeRotations(), 
                    singleLane->getShapeLengths());
            } else if (singleLane->getLaneShape().size() > 0) {
                // declare a Net Element Geometry
                GNENetElement::NetElementGeometry subLane;
                // set shape lane
                subLane.shape = singleLane->getLaneShape();
                // check if we have to add an extra first position
                if (extraFirstPosition != Position::INVALID) {
                    subLane.shape.push_front(extraFirstPosition);
                }
                    // check if we have to add an extra last position
                if (extraLastPosition != Position::INVALID) {
                    subLane.shape.push_back(extraLastPosition);
                }
                // calculate shape rotations and lenghts
                subLane.calculateShapeRotationsAndLengths();
                // add sublane geometry
                for (int i = 0; i < ((int)subLane.shape.size() - 1); i++) {
                    myDemandElementSegmentGeometry.updatePartialEdgeSegment(0, i,
                        subLane.shape[i], 
                        subLane.shapeRotations[i], 
                        subLane.shapeLengths[i]);
                }
                // close partial edge segment
                myDemandElementSegmentGeometry.updateLastPartialEdgeSegment(0, subLane.shape.back());
            }
        } else if (singleLane->getLaneShape().size() > 0) {
            // declare a Net Element Geometry
            GNENetElement::NetElementGeometry subLane;
            // set shape lane
            subLane.shape = singleLane->getLaneShape();
            if (subLane.shape.length() > (2*POSITION_EPS)) {
                if (startPos && endPos) {
                    // split lane
                    subLane.shape = subLane.shape.getSubpart(startPos, endPos);
                } else if (startPos) {
                    // split lane
                    subLane.shape = subLane.shape.splitAt(startPos).second;
                } else if (endPos) {
                    // split lane
                    subLane.shape = subLane.shape.splitAt(endPos).first;
                }
            }
            // check if we have to add an extra first position
            if (extraFirstPosition != Position::INVALID) {
                subLane.shape.push_front(extraFirstPosition);
            }
            // check if we have to add an extra last position
            if (extraLastPosition != Position::INVALID) {
                subLane.shape.push_back(extraLastPosition);
            }
            // calculate shape rotations and lenghts
            subLane.calculateShapeRotationsAndLengths();
            // add sublane geometry
            for (int i = 0; i < ((int)subLane.shape.size() - 1); i++) {
                myDemandElementSegmentGeometry.updatePartialEdgeSegment(0, i,
                    subLane.shape[i], 
                    subLane.shapeRotations[i], 
                    subLane.shapeLengths[i]);
            }
            // close partial edge segment
            myDemandElementSegmentGeometry.updateLastPartialEdgeSegment(0, subLane.shape.back());
        }
    } else {
        // obtain edges to be updated
        std::vector<std::tuple <const int, const GNELane*, const GNELane*> > segmentsToUpdate;
        for (auto segment = myDemandElementSegmentGeometry.begin(); segment != myDemandElementSegmentGeometry.end(); segment++) {
            if (segment->edge == edge) {
                const int index = std::distance(myDemandElementSegmentGeometry.begin(), segment);
                const GNELane* nextLane = ((segment + 1) != myDemandElementSegmentGeometry.end())? (segment + 1)->lane : nullptr;
                segmentsToUpdate.push_back(std::make_tuple(index, segment->lane, nextLane));
            }
        }
        // iterate over segments to update
        for (auto i : segmentsToUpdate) {
            // get lane (only for code readability)
            const int index = std::get<0>(i);
            const GNELane* lane = std::get<1>(i);
            const GNELane* nextLane = std::get<2>(i);
            // first check that lane shape isn't empty
            if (lane->getLaneShape().size() > 0) {
                // check if first or last lane must be splitted
                if ((index == 0) && (startPos != -1)) {
                    // filter start position
                    adjustStartPosGeometricPath(startPos, lane, endPos, nullptr);
                    // declare a Net Element Geometry
                    GNENetElement::NetElementGeometry subLane;
                    // set shape lane
                    subLane.shape = lane->getLaneShape();
                    // split lane
                    if (lane->getLaneShape().length() > (2*POSITION_EPS)) {
                        subLane.shape = subLane.shape.splitAt(startPos).second;
                    }
                    // check if we have to add an extra first position
                    if (extraFirstPosition != Position::INVALID) {
                        subLane.shape.push_front(extraFirstPosition);
                    }
                    // calculate shape rotations and lenghts
                    subLane.calculateShapeRotationsAndLengths();
                    // add sublane geometry
                    for (int j = 0; j < ((int)subLane.shape.size() - 1); j++) {
                        myDemandElementSegmentGeometry.updatePartialEdgeSegment(index, j,
                            subLane.shape[j], 
                            subLane.shapeRotations[j], 
                            subLane.shapeLengths[j]);
                    }
                    // close partial edge segment
                    myDemandElementSegmentGeometry.updateLastPartialEdgeSegment(index, subLane.shape.back());
                } else if ((index == (myDemandElementSegmentGeometry.size() -1)) && (endPos != -1)) {
                    // filter end position
                    adjustStartPosGeometricPath(startPos, nullptr, endPos, lane);
                    // declare a Net Element Geometry
                    GNENetElement::NetElementGeometry subLane;
                    // set shape lane
                    subLane.shape = lane->getLaneShape();
                    // split lane
                    if (lane->getLaneShape().length() > (2*POSITION_EPS)) {
                        subLane.shape = subLane.shape.splitAt(endPos).first;
                    }
                    // check if we have to add an extra last position
                    if (extraLastPosition != Position::INVALID) {
                        subLane.shape.push_back(extraLastPosition);
                    }
                    // calculate shape rotations and lenghts
                    subLane.calculateShapeRotationsAndLengths();
                    // add sublane geometry
                    for (int j = 0; j < ((int)subLane.shape.size() - 1); j++) {
                        myDemandElementSegmentGeometry.updatePartialEdgeSegment(index, j,
                            subLane.shape[j], 
                            subLane.shapeRotations[j], 
                            subLane.shapeLengths[j]);
                    }
                    // close partial edge segment
                    myDemandElementSegmentGeometry.updateLastPartialEdgeSegment(index, subLane.shape.back());
                } else {
                    // add entire lane geometry
                    myDemandElementSegmentGeometry.updateSegment(index,
                        lane->getLaneShape(), 
                        lane->getShapeRotations(), 
                        lane->getShapeLengths());
                }
            }
            // now continue with connection
            if (nextLane) {
                // check that next lane exist
                if (lane->getLane2laneConnections().shape.count(nextLane) > 0) {
                    // add lane2laneConnection shape
                    myDemandElementSegmentGeometry.updateSegment(index+1, 
                        lane->getLane2laneConnections().shape.at(nextLane), 
                        lane->getLane2laneConnections().shapeRotations.at(nextLane), 
                        lane->getLane2laneConnections().shapeLengths.at(nextLane));
                }
            }
        }
    }
}


void 
GNEDemandElement::calculatePersonPlanLaneStartEndPos(double &startPos, double &endPos) const {
    // obtain pointer to current busStop
    GNEAdditional* busStop = getAdditionalParents().size() > 0? getAdditionalParents().front() : nullptr;
    // declare pointers for previous elements
    GNEAdditional* previousBusStop = nullptr;
    GNEDemandElement *previousPersonPlan = getDemandElementParents().at(0)->getPreviousemandElement(this);
    // declare pointer to next person plan
    GNEDemandElement *nextPersonPlan = getDemandElementParents().at(0)->getNextDemandElement(this);
    // obtain departlane throught previous element
    if (previousPersonPlan && (previousPersonPlan->getAdditionalParents().size() > 0)) {
        // set previous busStop
        previousBusStop = previousPersonPlan->getAdditionalParents().front();
    }
    // adjust startPos depending of previous busStop
    if (previousBusStop) {
        startPos = previousBusStop->getAttributeDouble(SUMO_ATTR_ENDPOS);
    } else if (previousPersonPlan) {
        // check if previous element is a stop or another person plan (walk, ride, trip...)
        if (previousPersonPlan->getTagProperty().isPersonStop()) {
            startPos = previousPersonPlan->getAttributeDouble(SUMO_ATTR_ENDPOS);
        } else {
            startPos = previousPersonPlan->getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
        }
    } else {
        // if this is the first person plan, use departPos of pedestrian
        startPos = getDemandElementParents().front()->getAttributeDouble(SUMO_ATTR_DEPARTPOS);
    }
    // adjust endPos depending of next busStop
    if (busStop) {
        endPos = busStop->getAttributeDouble(SUMO_ATTR_STARTPOS);
    } else if (nextPersonPlan) {
        // check if next element is a stop or another person plan (walk, ride, trip...)
        if (nextPersonPlan->getTagProperty().isPersonStop()) {
            endPos = nextPersonPlan->getAttributeDouble(SUMO_ATTR_STARTPOS);
        } else {
            endPos = nextPersonPlan->getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
        }
    } else {
        // if this is the last element, simply use arrival position
        endPos = getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
    }
}


void 
GNEDemandElement::calculatePersonPlanPositionStartEndPos(Position &startPos, Position &endPos) const {
    // obtain previous demand element
    GNEDemandElement *previousDemandElmement = getDemandElementParents().front()->getPreviousemandElement(this);
    if (previousDemandElmement) {
        // update startPos
        if ((previousDemandElmement->getAdditionalParents().size() > 0) && 
            (previousDemandElmement->getAdditionalParents().front()->getAdditionalGeometry().shape.size() > 0)) {
            // Previous demand element ends in an busStop
            startPos = previousDemandElmement->getAdditionalParents().front()->getAdditionalGeometry().shape.back();
        } else if (previousDemandElmement->getTagProperty().isPersonStop() && (previousDemandElmement->getDemandElementGeometry().shape.size() > 0)) {
            // Previous demand element ends in an Stop
            startPos = previousDemandElmement->getDemandElementGeometry().shape.back();
        } else if ((previousDemandElmement->getDemandElementSegmentGeometry().size() > 0) && 
                   (previousDemandElmement->getDemandElementSegmentGeometry().back().shape.size() > 0)) {
            // add last shape segment of previous segment geometry
            startPos = previousDemandElmement->getDemandElementSegmentGeometry().back().shape.back();
        }
    }
    // check if demand element ends in an busStop
    if ((getAdditionalParents().size() > 0) && (getAdditionalParents().front()->getAdditionalGeometry().shape.size() > 0)) {
        endPos = getAdditionalParents().front()->getAdditionalGeometry().shape.front();
    } else {
        // obtain next demand element
        GNEDemandElement *nextDemandElmement = getDemandElementParents().front()->getNextDemandElement(this);
        if (nextDemandElmement) {
            // update end pos
            if (nextDemandElmement->getTagProperty().isPersonStop() && (nextDemandElmement->getDemandElementGeometry().shape.size() > 0)) {
                // previous demand element ends in an Stop
                endPos = nextDemandElmement->getDemandElementGeometry().shape.front();
            }
        }
    }
}


GNELane* 
GNEDemandElement::getFirstAllowedVehicleLane() const {
    // first check if current demand element has edge parents
    if (getEdgeParents().size() > 0) {
        // obtain Lane depending of attribute "departLane"
        if (myTagProperty.hasAttribute(SUMO_ATTR_DEPARTLANE)) {
            // obtain depart lane
            std::string departLane = getAttribute(SUMO_ATTR_DEPARTLANE);
            //  check depart lane
            if ((departLane == "random") || (departLane == "free") ||(departLane == "allowed") ||(departLane == "best") || (departLane == "first")) {
                return getEdgeParents().front()->getLaneByAllowedVClass(getVClass());
            }
            // obtain index
            const int departLaneIndex = parse<int>(getAttribute(SUMO_ATTR_DEPARTLANE));
            // if index is correct, return lane. In other case, return nullptr;
            if ((departLaneIndex >= 0) && (departLaneIndex < getEdgeParents().front()->getNBEdge()->getNumLanes())) {
                return getEdgeParents().front()->getLanes().at(departLaneIndex);
            } else {
                return nullptr;
            }
        } else if (myTagProperty.isRide()) {
            // special case for rides
            return getEdgeParents().front()->getLaneByDisallowedVClass(getVClass());
        } else {
            // in other case, always return the first allowed
            return getEdgeParents().front()->getLaneByAllowedVClass(getVClass());
        }
    } else {
        return nullptr;
    }
}


GNELane* 
GNEDemandElement::getLastAllowedVehicleLane() const {
    // first check if current demand element has edge parents
    if (getEdgeParents().size() > 0) {
        // obtain Lane depending of attribute "arrivalLane"
        if (myTagProperty.hasAttribute(SUMO_ATTR_ARRIVALLANE)) {
            // obtain arrival lane
            std::string arrivalLane = getAttribute(SUMO_ATTR_ARRIVALLANE);
            //  check depart lane
            if (arrivalLane == "current") {
                return getEdgeParents().back()->getLaneByAllowedVClass(getVClass());
            }
            // obtain index
            const int arrivalLaneIndex = parse<int>(getAttribute(SUMO_ATTR_ARRIVALLANE));
            // if index is correct, return lane. In other case, return nullptr;
            if ((arrivalLaneIndex >= 0) && (arrivalLaneIndex < getEdgeParents().back()->getNBEdge()->getNumLanes())) {
                return getEdgeParents().back()->getLanes().at(arrivalLaneIndex);
            } else {
                return nullptr;
            }
        } else if (myTagProperty.isRide()) {
            // special case for rides
            return getEdgeParents().back()->getLaneByDisallowedVClass(getVClass());
        } else {
            // in other case, always return the first allowed
            return getEdgeParents().back()->getLaneByAllowedVClass(getVClass());
        }
    } else {
        return nullptr;
    }
}


void 
GNEDemandElement::adjustStartPosGeometricPath(double &startPos, const GNELane* startLane, double &endPos, const GNELane* endLane) const {
    // adjust startPos
    if ((startPos != -1) && startLane) {
        if (startPos < (2 * POSITION_EPS)) {
            startPos = (2 * POSITION_EPS);
        }
        if (startPos > (startLane->getLaneShape().length() - (2 * POSITION_EPS))) {
            startPos = (startLane->getLaneShape().length() - (2 * POSITION_EPS));
        }
    }
    // adjust endPos
    if ((endPos != -1) && endLane) {
        if (endPos < (2 * POSITION_EPS)) {
            endPos = (2 * POSITION_EPS);
        }
        if (endPos > (endLane->getLaneShape().length() - (2 * POSITION_EPS))) {
            endPos = (endLane->getLaneShape().length() - (2 * POSITION_EPS));
        }
    }
    // adjust both, if start and end lane are the same
    if (startLane && endLane && (startLane == endLane) && (startPos != -1) && (endPos != -1)) {
        if (startPos < endPos) {
            endPos = startPos;
        }
    }
}


bool
GNEDemandElement::isAttributeCarrierSelected() const {
    return mySelected;
}


bool
GNEDemandElement::drawUsingSelectColor() const {
    if (mySelected && (myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND)) {
        return true;
    } else {
        return false;
    }
}


bool
GNEDemandElement::checkDemandElementChildRestriction() const {
    // throw exception because this function mus be implemented in child (see GNEE3Detector)
    throw ProcessError("Calling non-implemented function checkDemandElementChildRestriction during saving of " + getTagStr() + ". It muss be reimplemented in child class");
}

/****************************************************************************/
