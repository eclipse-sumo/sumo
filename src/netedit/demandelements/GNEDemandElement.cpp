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
#include <netedit/netelements/GNEEdge.h>
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

GNEDemandElement::DemandElementSegmentGeometry::Segment::Segment(const GNEDemandElement* _element, const GNEEdge* _edge, const Position _pos, const bool _visible, const bool _valid) :
    element(_element),
    edge(_edge),
    junction(nullptr),
    pos(_pos),
    visible(_visible),
    valid(_valid),
    length(-1),
    rotation(0) {
}


GNEDemandElement::DemandElementSegmentGeometry::Segment::Segment(const GNEDemandElement* _element, const GNEEdge* _edge, const Position _pos, double _length, double _rotation, const bool _visible, const bool _valid) :
    element(_element),
    edge(_edge),
    junction(nullptr),
    pos(_pos),
    visible(_visible),
    valid(_valid),
    length(_length),
    rotation(_rotation) {
}


GNEDemandElement::DemandElementSegmentGeometry::Segment::Segment(const GNEDemandElement* _element, const GNEJunction* _junction, const Position _pos, const bool _visible, const bool _valid) :
    element(_element),
    edge(nullptr),
    junction(_junction),
    pos(_pos),
    visible(_visible),
    valid(_valid),
    length(-1),
    rotation(0) {
}


// ---------------------------------------------------------------------------
// GNEDemandElement::DemandElementGeometry - methods
// ---------------------------------------------------------------------------

GNEDemandElement::DemandElementSegmentGeometry::DemandElementSegmentGeometry() :
    geometryDeprecated(true) {
}


void
GNEDemandElement::DemandElementSegmentGeometry::insertEdgeSegment(const GNEDemandElement* element, const GNEEdge* edge, const Position pos, const bool visible, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(element, edge, pos, visible, valid));
}


void
GNEDemandElement::DemandElementSegmentGeometry::insertEdgeLengthRotSegment(const GNEDemandElement* element, const GNEEdge* edge, const Position pos, double length, double rotation, const bool visible, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(element, edge, pos, length, rotation, visible, valid));
}


void
GNEDemandElement::DemandElementSegmentGeometry::insertJunctionSegment(const GNEDemandElement* element, const GNEJunction* junction, const Position pos, const bool visible, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(element, junction, pos, visible, valid));
}


void
GNEDemandElement::DemandElementSegmentGeometry::clearDemandElementSegmentGeometry() {
    // clear segments
    myShapeSegments.clear();
}


void
GNEDemandElement::DemandElementSegmentGeometry::calculatePartialShapeRotationsAndLengths() {
    // Get number of parts of the shapeSegments
    int numberOfSegments = (int)myShapeSegments.size() - 1;
    // If number of segments is more than 0
    if (numberOfSegments >= 0) {
        // Iterate over every segment
        for (int i = 0; i < numberOfSegments; ++i) {
            // check if length and rotation has to be calculated
            if (myShapeSegments[i].length == -1) {
                // Obtain first position
                const Position& f = myShapeSegments[i].pos;
                // Obtain next position
                const Position& s = myShapeSegments[i + 1].pos;
                // Save distance between position into myShapeLengths
                myShapeSegments[i].length = f.distanceTo2D(s);
                // Save rotation (angle) of the vector constructed by points f and s
                myShapeSegments[i].rotation = ((double)atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
            }
        }
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
