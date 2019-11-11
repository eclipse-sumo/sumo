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
        // For every part of the shape
        for (int i = 0; i < numberOfSegments; ++i) {
            // Obtain first position
            const Position& f = shape[i];
            // Obtain next position
            const Position& s = shape[i + 1];
            // Save distance between position into myShapeLengths
            shapeLengths.push_back(f.distanceTo(s));
            // Save rotation (angle) of the vector constructed by points f and s
            shapeRotations.push_back((double)atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
        }
    }
}

// ---------------------------------------------------------------------------
// GNEDemandElement::DemandElementSegmentGeometry::Segment - methods
// ---------------------------------------------------------------------------

GNEDemandElement::DemandElementSegmentGeometry::Segment::Segment(const GNEDemandElement* _element, const GNEEdge* _edge, 
        const Position& pos, const double rotation, const double length, const bool _visible, const bool _valid) :
    element(_element),
    edge(_edge),
    junction(nullptr),
    visible(_visible),
    valid(_valid),
    shape({pos}),
    shapeRotations({rotation}),
    shapeLengths({length}) {
}


GNEDemandElement::DemandElementSegmentGeometry::Segment::Segment(const GNEDemandElement* _element, const GNEEdge* _edge, 
        const PositionVector& _shape, const std::vector<double> &_shapeRotations, const std::vector<double> &_shapeLengths, 
        const bool _visible, const bool _valid) :
    element(_element),
    edge(_edge),
    junction(nullptr),
    visible(_visible),
    valid(_valid),
    shape(_shape),
    shapeRotations(_shapeRotations),
    shapeLengths(_shapeLengths) {
}


GNEDemandElement::DemandElementSegmentGeometry::Segment::Segment(const GNEDemandElement* _element, const GNEJunction* _junction, 
        const PositionVector& _shape, const std::vector<double> &_shapeRotations, const std::vector<double> &_shapeLengths, 
        const bool _visible, const bool _valid) :
    element(_element),
    edge(nullptr),
    junction(_junction),
    visible(_visible),
    valid(_valid),
    shape(_shape),
    shapeRotations(_shapeRotations),
    shapeLengths(_shapeLengths) {
}

// ---------------------------------------------------------------------------
// GNEDemandElement::DemandElementGeometry - methods
// ---------------------------------------------------------------------------

GNEDemandElement::DemandElementSegmentGeometry::DemandElementSegmentGeometry() :
    geometryDeprecated(true) {
}


void
GNEDemandElement::DemandElementSegmentGeometry::insertPartialEdgeSegment(const GNEDemandElement* element, const GNEEdge* edge, 
    const Position pos, double rotation, double length, const bool visible, const bool valid) {
    // check if we have to insert a new segment or update last inserted element
    if (myShapeSegments.empty() || (myShapeSegments.back().edge != edge)) {
        // add new segment
        myShapeSegments.push_back(Segment(element, edge, pos, rotation, length, visible, valid));
    } else {
        // update last shape segment
        myShapeSegments.back().shape.push_back(pos);
        myShapeSegments.back().shapeRotations.push_back(rotation);
        myShapeSegments.back().shapeLengths.push_back(length);
    }
}


void 
GNEDemandElement::DemandElementSegmentGeometry::insertEdgeSegment(const GNEDemandElement* element, const GNEEdge* edge, 
    const PositionVector& laneShape, const std::vector<double> &laneShapeRotations, const std::vector<double> &laneShapeLengths, const bool visible, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(element, edge, laneShape, laneShapeRotations, laneShapeLengths, visible, valid));
}


void
GNEDemandElement::DemandElementSegmentGeometry::insertLane2LaneSegment(const GNEDemandElement* element, const GNEJunction* junction, 
    const PositionVector& laneShape, const std::vector<double> &laneShapeRotations, const std::vector<double> &laneShapeLengths, const bool visible, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(element, junction, laneShape, laneShapeRotations, laneShapeLengths, visible, valid));
}


void
GNEDemandElement::DemandElementSegmentGeometry::clearDemandElementSegmentGeometry() {
    // clear segments
    myShapeSegments.clear();
}


void 
GNEDemandElement::DemandElementSegmentGeometry::closePartialEdgeSegment() {
    if (myShapeSegments.size() > 0) {
        if ((myShapeSegments.back().shape.size() > 0) &&
            (myShapeSegments.back().shape.size() == myShapeSegments.back().shapeRotations.size()) &&
            (myShapeSegments.back().shape.size() == myShapeSegments.back().shapeLengths.size())) {
            myShapeSegments.back().shape.push_back(myShapeSegments.back().shape.back());
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


void
GNEDemandElement::markSegmentGeometryDeprecated() {
    myDemandElementSegmentGeometry.geometryDeprecated = true;
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
GNEDemandElement::calculateGeometricPath(double startPos, double endPos) {
    // clear geometry
    myDemandElementSegmentGeometry.clearDemandElementSegmentGeometry();
    // first check that there is edge parents
    if (getEdgeParents().size() > 0) {
        // calculate depending if both from and to edges are the same
        if (getEdgeParents().size() == 1) {
            // obtain first (and single) Lane
            GNELane* singleLane = getFirstVehicleLane();
            // if obtained lane is null, then force to use first lane
            if (singleLane == nullptr) {
                singleLane = getEdgeParents().front()->getLanes().front();
            }
            // filter start and end pos
            adjustStartPosGeometricPath(startPos, singleLane, endPos, singleLane);
            // set geometry depending of start and end positions
            if ((startPos == -1) && (endPos == -1)) {
                // add lane geometry
                for (int i = 0; i < ((int)singleLane->getLaneShape().size() - 1); i++) {
                    myDemandElementSegmentGeometry.insertPartialEdgeSegment(this, getEdgeParents().at(0),
                        singleLane->getLaneShape()[i], 
                        singleLane->getShapeRotations()[i], 
                        singleLane->getShapeLengths()[i], true, true);
                }
                // close partial edge segment
                myDemandElementSegmentGeometry.closePartialEdgeSegment();
            } else {
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
                // calculate shape rotations and lenghts
                subLane.calculateShapeRotationsAndLengths();
                // add sublane geometry
                for (int i = 0; i < ((int)subLane.shape.size() - 1); i++) {
                    myDemandElementSegmentGeometry.insertPartialEdgeSegment(this, getEdgeParents().at(0),
                        subLane.shape[i], 
                        subLane.shapeRotations[i], 
                        subLane.shapeLengths[i], true, true);
                }
                // close partial edge segment
                myDemandElementSegmentGeometry.closePartialEdgeSegment();
            }
        } else {
            // declare vector of lanes
            std::vector<GNELane*> lanes;
            // reserve space
            lanes.reserve(getEdgeParents().size());
            // obtain lanes by VClass
            for (auto edgeParent = getEdgeParents().begin(); edgeParent != getEdgeParents().end(); edgeParent++) {
                GNELane* allowedLane = nullptr;
                if (edgeParent == getEdgeParents().begin()) {
                    allowedLane = getFirstVehicleLane();
                } else if (edgeParent == getEdgeParents().begin()) {
                    allowedLane = getLastVehicleLane();
                } else {
                    // obtain first allowed lane
                    allowedLane = (*edgeParent)->getLaneByVClass(getVClass());
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
                // check if first or last lane must be splitted
                if ((lane == lanes.front()) && (startPos != -1)) {
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
                    // calculate shape rotations and lenghts
                    subLane.calculateShapeRotationsAndLengths();
                    // add sublane geometry
                    for (int j = 0; j < ((int)subLane.shape.size() - 1); j++) {
                        myDemandElementSegmentGeometry.insertPartialEdgeSegment(this, &lane->getParentEdge(),
                            subLane.shape[j], 
                            subLane.shapeRotations[j], 
                            subLane.shapeLengths[j], true, true);
                    }
                    // close partial edge segment
                    myDemandElementSegmentGeometry.closePartialEdgeSegment();
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
                    // calculate shape rotations and lenghts
                    subLane.calculateShapeRotationsAndLengths();
                    // add sublane geometry
                    for (int j = 0; j < ((int)subLane.shape.size() - 1); j++) {
                        myDemandElementSegmentGeometry.insertPartialEdgeSegment(this, &lane->getParentEdge(),
                            subLane.shape[j], 
                            subLane.shapeRotations[j], 
                            subLane.shapeLengths[j], true, true);
                    }
                    // close partial edge segment
                    myDemandElementSegmentGeometry.closePartialEdgeSegment();
                } else {
                    // add lane geometry
                    for (int j = 0; j < ((int)lane->getLaneShape().size() - 1); j++) {
                        myDemandElementSegmentGeometry.insertEdgeSegment(this, &lane->getParentEdge(),
                            lane->getLaneShape(), 
                            lane->getShapeRotations(), 
                            lane->getShapeLengths(), true, true);
                    }
                }
                // now continue with connection
                if ((i+1) < (int)lanes.size()) {
                    // obtain next lane
                    const GNELane* const nextLane = lanes.at(i+1);
                    // check that next lane exist
                    if (lane->getLane2laneConnections().shape.count(nextLane) > 0) {
                        // add lane2laneConnection shape
                        myDemandElementSegmentGeometry.insertLane2LaneSegment(this, lane->getParentEdge().getGNEJunctionDestiny(), 
                            lane->getLane2laneConnections().shape.at(nextLane), 
                            lane->getLane2laneConnections().shapeRotations.at(nextLane), 
                            lane->getLane2laneConnections().shapeLengths.at(nextLane), true, true);
                    }
                }
            }
        }
    }
}


GNELane* 
GNEDemandElement::getFirstVehicleLane() const {
    // first check if current demand element has edge parents
    if (getEdgeParents().size() > 0) {
        // obtain Lane depending of attribute "departLane"
        if (myTagProperty.hasAttribute(SUMO_ATTR_DEPARTLANE)) {
            // obtain depart lane
            std::string departLane = getAttribute(SUMO_ATTR_DEPARTLANE);
            //  check depart lane
            if ((departLane == "random") || (departLane == "free") ||(departLane == "allowed") ||(departLane == "best") || (departLane == "first")) {
                return getEdgeParents().front()->getLaneByVClass(getVClass());
            }
            // obtain index
            const int departLaneIndex = parse<int>(getAttribute(SUMO_ATTR_DEPARTLANE));
            // if index is correct, return lane. In other case, return nullptr;
            if ((departLaneIndex >= 0) && (departLaneIndex < getEdgeParents().front()->getNBEdge()->getNumLanes())) {
                return getEdgeParents().front()->getLanes().at(departLaneIndex);
            } else {
                return nullptr;
            }
        } else {
            // in other case, always return the first allowed
            return getEdgeParents().front()->getLaneByVClass(getVClass());
        }
    } else {
        return nullptr;
    }
}


GNELane* 
GNEDemandElement::getLastVehicleLane() const {
    // first check if current demand element has edge parents
    if (getEdgeParents().size() > 0) {
        // obtain Lane depending of attribute "arrivalLane"
        if (myTagProperty.hasAttribute(SUMO_ATTR_ARRIVALLANE)) {
            // obtain arrival lane
            std::string arrivalLane = getAttribute(SUMO_ATTR_ARRIVALLANE);
            //  check depart lane
            if (arrivalLane == "current") {
                return getEdgeParents().back()->getLaneByVClass(getVClass());;
            }
            // obtain index
            const int arrivalLaneIndex = parse<int>(getAttribute(SUMO_ATTR_ARRIVALLANE));
            // if index is correct, return lane. In other case, return nullptr;
            if ((arrivalLaneIndex >= 0) && (arrivalLaneIndex < getEdgeParents().back()->getNBEdge()->getNumLanes())) {
                return getEdgeParents().back()->getLanes().at(arrivalLaneIndex);
            } else {
                return nullptr;
            }
        } else {
            // in other case, always return the first allowed
            return getEdgeParents().back()->getLaneByVClass(getVClass());
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
