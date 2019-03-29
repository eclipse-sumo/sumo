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
#include <netedit/netelements/GNELane.h>
#include <utils/common/StringTokenizer.h>
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
// GNEDemandElement::RouteCalculator - methods
// ---------------------------------------------------------------------------

GNEDemandElement::RouteCalculator::RouteCalculator(GNENet* net) :
    myNet(net) {
    myDijkstraRouter = new DijkstraRouter<NBEdge, NBVehicle, SUMOAbstractRouter<NBEdge, NBVehicle> >(
        myNet->getNetBuilder()->getEdgeCont().getAllEdges(), 
        true, &NBEdge::getTravelTimeStatic, nullptr, true);
}


GNEDemandElement::RouteCalculator::~RouteCalculator() {
    delete myDijkstraRouter;
}


void 
GNEDemandElement::RouteCalculator::updateDijkstraRouter() {
    // simply delete and create myDijkstraRouter again
    if(myDijkstraRouter) {
        delete myDijkstraRouter;
    }
    myDijkstraRouter = new DijkstraRouter<NBEdge, NBVehicle, SUMOAbstractRouter<NBEdge, NBVehicle> >(
        myNet->getNetBuilder()->getEdgeCont().getAllEdges(), 
        true, &NBEdge::getTravelTimeStatic, nullptr, true);
}


std::vector<const NBEdge*> 
GNEDemandElement::RouteCalculator::calculateDijkstraRoute(SUMOVehicleClass vClass, const std::vector<GNEEdge*> &edges) const {
    // declare a solution vector
    std::vector<const NBEdge*> solution;
    // declare temporal vehicle
    NBVehicle tmpVehicle("temporalNBVehicle", vClass);
    // iterate over every selected edges
    for (int i = 1; i < (int)edges.size(); i++) {
        // declare a temporal route in which save route between two last edges
        std::vector<const NBEdge*> partialRoute;
        myDijkstraRouter->compute(edges.at(i-1)->getNBEdge(), edges.at(i)->getNBEdge(), &tmpVehicle, 10, partialRoute);
        // save partial route in solution
        for (const auto &j : partialRoute) {
            solution.push_back(j);
        }
    }
    return solution;
}


std::vector<std::vector<const NBEdge*> >
GNEDemandElement::RouteCalculator::calculateDijkstraPartialRoute(SUMOVehicleClass vClass, const std::vector<GNEEdge*> &edges) const {
    // declare a solution matrix
    std::vector<std::vector<const NBEdge*> > solution;
    // declare temporal vehicle
    NBVehicle tmpVehicle("temporalNBVehicle", vClass);
    // iterate over every selected edges
    for (int i = 1; i < (int)edges.size(); i++) {
        // add a temporal route in which save route between two last edges
        solution.push_back(std::vector<const NBEdge*>());
        myDijkstraRouter->compute(edges.at(i-1)->getNBEdge(), edges.at(i)->getNBEdge(), &tmpVehicle, 10, solution.back());
    }
    // return matrix solution
    return solution;
}


bool 
GNEDemandElement::RouteCalculator::areEdgesConsecutives(SUMOVehicleClass vClass, GNEEdge* from, GNEEdge*to) const {
    // first calculate a route between from and to edge
    std::vector<const NBEdge*> route = calculateDijkstraRoute(vClass, {from, to});
    // if route is empty, return false
    if (route.empty()) {
        return false;
    } else {
        // iterate over route and check if from and to edge are consecutives
        for (int i = 0; i < (int)route.size(); i++) {
            if ((route.at(i)->getID() == from->getID()) && 
                ((i+1) < (int)route.size()) &&
                (route.at(i+1)->getID() == to->getID())) {
                return true;
            }
        }
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEDemandElement - methods
// ---------------------------------------------------------------------------

GNEDemandElement::GNEDemandElement(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag,
        const std::vector<GNEEdge*> &edgeParents, 
        const std::vector<GNELane*> &laneParents, 
        const std::vector<GNEAdditional*>& additionalParents, 
        const std::vector<GNEDemandElement*>& demandElementParents,
        const std::vector<GNEEdge*> &edgeChilds, 
        const std::vector<GNELane*> &laneChilds, 
        const std::vector<GNEAdditional*>& additionalChilds, 
        const std::vector<GNEDemandElement*>& demandElementChilds) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag),
    Parameterised(),
    GNEHierarchicalElementParents(this, edgeParents, laneParents, additionalParents, demandElementParents),
    GNEHierarchicalElementChilds(this, edgeChilds, laneChilds, additionalChilds, demandElementChilds),
    myViewNet(viewNet) {
}


GNEDemandElement::GNEDemandElement(GNEDemandElement* demandElementParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag,
        const std::vector<GNEEdge*> &edgeParents, 
        const std::vector<GNELane*> &laneParents, 
        const std::vector<GNEAdditional*>& additionalParents, 
        const std::vector<GNEDemandElement*>& demandElementParents,
        const std::vector<GNEEdge*> &edgeChilds, 
        const std::vector<GNELane*> &laneChilds, 
        const std::vector<GNEAdditional*>& additionalChilds, 
        const std::vector<GNEDemandElement*>& demandElementChilds) :
    GUIGlObject(type, demandElementParent->generateChildID(tag)),
    GNEAttributeCarrier(tag),
    Parameterised(),
    GNEHierarchicalElementParents(this, edgeParents, laneParents, additionalParents, demandElementParents),
    GNEHierarchicalElementChilds(this, edgeChilds, laneChilds, additionalChilds, demandElementChilds),
    myViewNet(viewNet) {
}


std::string 
GNEDemandElement::generateChildID(SumoXMLTag childTag) {
    int counter = (int)myDemandElementChilds.size();
    while (myViewNet->getNet()->retrieveDemandElement(childTag, getID() + toString(childTag) + toString(counter), false) != nullptr) {
        counter++;
    }
    return (getID() + toString(childTag) + toString(counter));
}


GNEDemandElement::~GNEDemandElement() {}


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


void
GNEDemandElement::startGeometryMoving() {
    // always save original position over view
    myMove.originalViewPosition = getPositionInView();
    // check if position over lane or lanes has to be saved
    if (myTagProperty.hasAttribute(SUMO_ATTR_LANE)) {
        if (myTagProperty.canMaskStartEndPos()) {
            // obtain start and end position
            myMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_STARTPOS);
            myMove.secondOriginalPosition = getAttribute(SUMO_ATTR_ENDPOS);
        } else {
            // obtain position attribute
            myMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_POSITION);
        }
    } else if (myTagProperty.hasAttribute(SUMO_ATTR_LANES)) {
        // obtain start and end position
        myMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_POSITION);
        myMove.secondOriginalPosition = getAttribute(SUMO_ATTR_ENDPOS);
    }
    // save current centering boundary
    myMove.movingGeometryBoundary = getCenteringBoundary();
    // Iterate over demand element childs and start geometry moving
    for (auto i : myDemandElementChilds) {
        i->startGeometryMoving();
    }
}


void
GNEDemandElement::endGeometryMoving() {
    // check that endGeometryMoving was called only once
    if (myMove.movingGeometryBoundary.isInitialised()) {
        // Remove object from net
        myViewNet->getNet()->removeGLObjectFromGrid(this);
        // reset myMovingGeometryBoundary
        myMove.movingGeometryBoundary.reset();
        // update geometry without updating grid
        updateGeometry(false);
        // Iterate over demand element childs and end geometry moving
        for (auto i : myDemandElementChilds) {
            i->endGeometryMoving();
        }
        // add object into grid again (using the new centering boundary)
        myViewNet->getNet()->addGLObjectIntoGrid(this);
    }
}


GNEViewNet*
GNEDemandElement::getViewNet() const {
    return myViewNet;
}


void 
GNEDemandElement::createRouteCalculatorInstance(GNENet *net) {
    if(myRouteCalculatorInstance == nullptr) {
        myRouteCalculatorInstance = new RouteCalculator(net);
    } else {
        throw ProcessError("Instance already created");
    }
}


void 
GNEDemandElement::deleteRouteCalculatorInstance() {
    if(myRouteCalculatorInstance) {
        delete myRouteCalculatorInstance;
        myRouteCalculatorInstance = nullptr;
    } else {
        throw ProcessError("Instance wasn't created");
    }
}


GNEDemandElement::RouteCalculator* 
GNEDemandElement::getRouteCalculatorInstance() {
    if(myRouteCalculatorInstance) {
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
        if (i.second.isUnique()) {
            ret->mkItem(toString(i.first).c_str(), false, getAttribute(i.first));
        } else {
            ret->mkItem(toString(i.first).c_str(), true, getAttribute(i.first));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GNEDemandElement::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (myMove.movingGeometryBoundary.isInitialised()) {
        return myMove.movingGeometryBoundary;
    } else if (myGeometry.shape.size() > 0) {
        Boundary b = myGeometry.shape.getBoxBoundary();
        b.grow(20);
        return b;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
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


const std::string&
GNEDemandElement::getDemandElementID() const {
    return getMicrosimID();
}


bool
GNEDemandElement::isValidDemandElementID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidNetID(newID) && (myViewNet->getNet()->retrieveDemandElement(myTagProperty.getTag(), newID, false) == nullptr)) {
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


std::string
GNEDemandElement::getGenericParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i : getParametersMap()) {
        result += i.first + "=" + i.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNEDemandElement::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNEDemandElement::setGenericParametersStr(const std::string& value) {
    // clear parameters
    clearParameter();
    // separate value in a vector of string using | as separator
    std::vector<std::string> parsedValues;
    StringTokenizer stValues(value, "|", true);
    while (stValues.hasNext()) {
        parsedValues.push_back(stValues.next());
    }
    // check that parsed values (A=B)can be parsed in generic parameters
    for (auto i : parsedValues) {
        std::vector<std::string> parsedParameters;
        StringTokenizer stParam(i, "=", true);
        while (stParam.hasNext()) {
            parsedParameters.push_back(stParam.next());
        }
        // Check that parsed parameters are exactly two and contains valid chracters
        if (parsedParameters.size() == 2 && SUMOXMLDefinitions::isValidGenericParameterKey(parsedParameters.front()) && SUMOXMLDefinitions::isValidGenericParameterValue(parsedParameters.back())) {
            setParameter(parsedParameters.front(), parsedParameters.back());
        }
    }
}

/****************************************************************************/
