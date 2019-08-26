/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEWalk.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
/// @version $Id$
///
// A class for visualizing walks in Netedit
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIGlobalSelection.h>

#include "GNEWalk.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEWalk::GNEWalk(GNEViewNet* viewNet, GNEDemandElement* personParent, SumoXMLTag walkTag, const std::vector<GNEEdge*>& edges, double arrivalPosition) :
    GNEDemandElement(viewNet->getNet()->generateDemandElementID("", walkTag), viewNet, GLO_WALK, walkTag,
                     edges, {}, {}, {}, {personParent}, {}, {}, {}, {}, {}),
                        Parameterised(),
myArrivalPosition(arrivalPosition) {
}


GNEWalk::GNEWalk(GNEViewNet* viewNet, GNEDemandElement* personParent, const std::vector<GNEEdge*>& edges, GNEAdditional* busStop) :
    GNEDemandElement(viewNet->getNet()->generateDemandElementID("", SUMO_TAG_WALK_BUSSTOP), viewNet, GLO_WALK, SUMO_TAG_WALK_BUSSTOP,
                     edges, {}, {}, {busStop}, {personParent}, {}, {}, {}, {}, {}),
Parameterised(),
myArrivalPosition(-1) {
}


GNEWalk::GNEWalk(GNEViewNet* viewNet, GNEDemandElement* personParent, GNEDemandElement* routeParent, double arrivalPosition) :
    GNEDemandElement(viewNet->getNet()->generateDemandElementID("", SUMO_TAG_WALK_ROUTE), viewNet, GLO_WALK, SUMO_TAG_WALK_ROUTE,
{}, {}, {}, {}, {personParent, routeParent}, {}, {}, {}, {}, {}),
Parameterised(),
myArrivalPosition(arrivalPosition) {
}


GNEWalk::~GNEWalk() {}


GUIGLObjectPopupMenu*
GNEWalk::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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


void
GNEWalk::writeDemandElement(OutputDevice& device) const {
    // open tag
    device.openTag(SUMO_TAG_WALK);
    // write attributes depending  of walk type
    if (myTagProperty.getTag() == SUMO_TAG_WALK_ROUTE) {
        device.writeAttr(SUMO_ATTR_ROUTE, getDemandElementParents().at(1)->getID());
    } else if (myTagProperty.getTag() == SUMO_TAG_WALK_EDGES) {
        device.writeAttr(SUMO_ATTR_EDGES, parseIDs(getEdgeParents()));
    } else {
        // only write From attribute if this is the first Person Plan
        if (getDemandElementParents().front()->getDemandElementChildren().front() == this) {
            device.writeAttr(SUMO_ATTR_FROM, getEdgeParents().front()->getID());
        }
        // check if write busStop or edge to
        if (getAdditionalParents().size() > 0) {
            device.writeAttr(SUMO_ATTR_BUS_STOP, getAdditionalParents().front()->getID());
        } else {
            device.writeAttr(SUMO_ATTR_TO, getEdgeParents().back()->getID());
        }
    }
    // only write arrivalPos if is different of -1
    if (myArrivalPosition != -1) {
        device.writeAttr(SUMO_ATTR_ARRIVALPOS, myArrivalPosition);
    }
    // close tag
    device.closeTag();
}


bool
GNEWalk::isDemandElementValid() const {
    if (myTagProperty.getTag() == SUMO_TAG_WALK_ROUTE) {
        return true;
    } else if (getEdgeParents().size() == 0) {
        return false;
    } else if (getEdgeParents().size() == 1) {
        return true;
    } else {
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)getEdgeParents().size(); i++) {
            if (getRouteCalculatorInstance()->areEdgesConsecutives(getDemandElementParents().front()->getVClass(), getEdgeParents().at((int)i - 1), getEdgeParents().at(i)) == false) {
                return false;
            }
        }
        // there is connections bewteen all edges, then return true
        return true;
    }
}


std::string
GNEWalk::getDemandElementProblem() const {
    if (myTagProperty.getTag() == SUMO_TAG_WALK_ROUTE) {
        return "";
    } else if (getEdgeParents().size() == 0) {
        return ("A walk need at least one edge");
    } else {
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)getEdgeParents().size(); i++) {
            if (getRouteCalculatorInstance()->areEdgesConsecutives(getDemandElementParents().front()->getVClass(), getEdgeParents().at((int)i - 1), getEdgeParents().at(i)) == false) {
                return ("Edge '" + getEdgeParents().at((int)i - 1)->getID() + "' and edge '" + getEdgeParents().at(i)->getID() + "' aren't consecutives");
            }
        }
        // there is connections bewteen all edges, then all ok
        return "";
    }
}


void
GNEWalk::fixDemandElementProblem() {
    // currently the only solution is removing Walk
}


GNEEdge*
GNEWalk::getFromEdge() const {
    if (getDemandElementParents().size() == 2) {
        // obtain position and rotation of first edge route
        return getDemandElementParents().at(1)->getFromEdge();
    } else {
        return getEdgeParents().front();
    }
}


GNEEdge*
GNEWalk::getToEdge() const {
    if (getDemandElementParents().size() == 2) {
        // obtain position and rotation of first edge route
        return getDemandElementParents().at(1)->getToEdge();
    } else {
        return getEdgeParents().back();
    }
}


SUMOVehicleClass
GNEWalk::getVClass() const {
    return getDemandElementParents().front()->getVClass();
}


const RGBColor&
GNEWalk::getColor() const {
    return getDemandElementParents().front()->getColor();
}


void
GNEWalk::compute() {
    // Nothing to compute
}


void
GNEWalk::startGeometryMoving() {
    // only start geometry moving if arrival position isn't -1
    if (myArrivalPosition != -1) {
        // always save original position over view
        myWalkMove.originalViewPosition = getPositionInView();
        // save arrival position
        myWalkMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_ARRIVALPOS);
        // save current centering boundary
        myWalkMove.movingGeometryBoundary = getCenteringBoundary();
    }
}


void
GNEWalk::endGeometryMoving() {
    // check that myArrivalPosition isn't -1 and endGeometryMoving was called only once
    if ((myArrivalPosition != -1) && myWalkMove.movingGeometryBoundary.isInitialised()) {
        // reset myMovingGeometryBoundary
        myWalkMove.movingGeometryBoundary.reset();
    }
}


void
GNEWalk::moveGeometry(const Position& offset) {
    // only move if myArrivalPosition isn't -1
    if (myArrivalPosition != -1) {
        // Calculate new position using old position
        Position newPosition = myWalkMove.originalViewPosition;
        newPosition.add(offset);
        // filtern position using snap to active grid
        newPosition = myViewNet->snapToActiveGrid(newPosition);
        // obtain lane shape (to improve code legibility)
        const PositionVector& laneShape = getEdgeParents().back()->getLanes().front()->getGeometry().shape;
        // calculate offset lane
        double offsetLane = laneShape.nearest_offset_to_point2D(newPosition, false) - laneShape.nearest_offset_to_point2D(myWalkMove.originalViewPosition, false);
        std::cout << offsetLane << std::endl;
        // Update arrival Position
        myArrivalPosition = parse<double>(myWalkMove.firstOriginalLanePosition) + offsetLane;
        // Update geometry
        updateGeometry();
    }
}


void
GNEWalk::commitGeometryMoving(GNEUndoList* undoList) {
    // only commit geometry moving if myArrivalPosition isn't -1
    if (myArrivalPosition != -1) {
        undoList->p_begin("arrivalPos of " + getTagStr());
        undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_ARRIVALPOS, toString(myArrivalPosition), true, myWalkMove.firstOriginalLanePosition));
        undoList->p_end();
    }
}


void
GNEWalk::updateGeometry() {
    // update person parent
    getDemandElementParents().front()->updateGeometry();
    // update demand element childs
    for (const auto& i : getDemandElementChildren()) {
        i->updateGeometry();
    }
}


Position
GNEWalk::getPositionInView() const {
    return Position();
}


std::string
GNEWalk::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


Boundary
GNEWalk::getCenteringBoundary() const {
    Boundary walkBoundary;
    // return the combination of all edge parents's boundaries
    for (const auto& i : getEdgeParents()) {
        walkBoundary.add(i->getCenteringBoundary());
    }
    // check if is valid
    if (walkBoundary.isInitialised()) {
        return walkBoundary;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNEWalk::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // Walks are drawn in GNEEdges
}


void
GNEWalk::selectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.select(getGlID());
        // add object of list into selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(getType());
        if (changeFlag) {
            mySelected = true;
        }
    }
}


void
GNEWalk::unselectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.deselect(getGlID());
        // remove object of list of selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(getType());
        if (changeFlag) {
            mySelected = false;

        }
    }
}


std::string
GNEWalk::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getDemandElementID();
        case SUMO_ATTR_FROM:
            return getEdgeParents().front()->getID();
        case SUMO_ATTR_TO:
            return getEdgeParents().back()->getID();
        case SUMO_ATTR_ROUTE:
            return getDemandElementParents().at(1)->getID();
        case SUMO_ATTR_VIA:
            return toString(myVia);
        case SUMO_ATTR_BUS_STOP:
            return getAdditionalParents().front()->getID();
        case SUMO_ATTR_ARRIVALPOS:
            return toString(myArrivalPosition);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        case GNE_ATTR_PARENT:
            return getDemandElementParents().front()->getID();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEWalk::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ARRIVALPOS:
            return myArrivalPosition;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEWalk::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case SUMO_ATTR_VIA:
        case SUMO_ATTR_ROUTE:
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_ARRIVALPOS:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEWalk::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) && (myViewNet->getNet()->retrieveEdge(value, false) != nullptr);
        case SUMO_ATTR_VIA:
            if (value.empty()) {
                return true;
            } else {
                return canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), value, false);
            }
        case SUMO_ATTR_BUS_STOP:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr);
        case SUMO_ATTR_ROUTE:
            return (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, value, false) != nullptr);
        case SUMO_ATTR_ARRIVALPOS:
            if (canParse<double>(value)) {
                double parsedValue = canParse<double>(value);
                // a arrival pos with value -1 means that it will be ignored
                if (parsedValue == -1) {
                    return true;
                } else {
                    return parsedValue >= 0;
                }
            } else {
                return false;
            }
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEWalk::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


bool
GNEWalk::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return true;
}


std::string
GNEWalk::getPopUpID() const {
    return getTagStr();
}


std::string
GNEWalk::getHierarchyName() const {
    if ((myTagProperty.getTag() == SUMO_TAG_WALK_FROMTO) || (myTagProperty.getTag() == SUMO_TAG_WALK_EDGES)) {
        return "walk: " + getEdgeParents().front()->getID() + " -> " + getEdgeParents().back()->getID();
    } else  if (myTagProperty.getTag() == SUMO_TAG_WALK_BUSSTOP) {
        return "walk: " + getEdgeParents().front()->getID() + " -> " + getAdditionalParents().front()->getID();
    } else {
        return "walk: " + getDemandElementParents().at(1)->getID();
    }
}


std::string
GNEWalk::getGenericParametersStr() const {
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
GNEWalk::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNEWalk::setGenericParametersStr(const std::string& value) {
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

// ===========================================================================
// private
// ===========================================================================

void
GNEWalk::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_FROM: {
            // declare a from-via-to edges vector
            std::vector<std::string> FromViaToEdges;
            // add from edge
            FromViaToEdges.push_back(value);
            // add via edges
            FromViaToEdges.insert(FromViaToEdges.end(), myVia.begin(), myVia.end());
            // add to edge
            FromViaToEdges.push_back(getEdgeParents().back()->getID());
            // calculate route
            std::vector<GNEEdge*> route = getRouteCalculatorInstance()->calculateDijkstraRoute(myViewNet->getNet(), getDemandElementParents().at(0)->getVClass(), FromViaToEdges);
            // change edge parents
            changeEdgeParents(this, toString(route));
            break;
        }
        case SUMO_ATTR_TO: {
            // declare a from-via-to edges vector
            std::vector<std::string> FromViaToEdges;
            // add from edge
            FromViaToEdges.push_back(getEdgeParents().front()->getID());
            // add via edges
            FromViaToEdges.insert(FromViaToEdges.end(), myVia.begin(), myVia.end());
            // add to edge
            FromViaToEdges.push_back(value);
            // calculate route
            std::vector<GNEEdge*> route = getRouteCalculatorInstance()->calculateDijkstraRoute(myViewNet->getNet(), getDemandElementParents().at(0)->getVClass(), FromViaToEdges);
            // change edge parents
            changeEdgeParents(this, toString(route));
            break;
        }
        case SUMO_ATTR_VIA: {
            if (!value.empty()) {
                // set new via edges
                myVia = parse< std::vector<std::string> >(value);
            } else {
                // clear via
                myVia.clear();
            }
            // declare a from-via-to edges vector
            std::vector<std::string> FromViaToEdges;
            // add from edge
            FromViaToEdges.push_back(getEdgeParents().front()->getID());
            // add via edges
            FromViaToEdges.insert(FromViaToEdges.end(), myVia.begin(), myVia.end());
            // add to edge
            FromViaToEdges.push_back(getEdgeParents().back()->getID());
            // calculate route
            std::vector<GNEEdge*> route = getRouteCalculatorInstance()->calculateDijkstraRoute(myViewNet->getNet(), getDemandElementParents().at(0)->getVClass(), FromViaToEdges);
            // change edge parents
            changeEdgeParents(this, toString(route));
            break;
        }
        case SUMO_ATTR_ROUTE:
            changeDemandElementParent(this, value, 1);
            break;
        case SUMO_ATTR_BUS_STOP:
            changeAdditionalParent(this, value, 0);
            break;
        case SUMO_ATTR_ARRIVALPOS:
            myArrivalPosition = parse<double>(value);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEWalk::setEnabledAttribute(const int /*enabledAttributes*/) {
    //
}

/****************************************************************************/
