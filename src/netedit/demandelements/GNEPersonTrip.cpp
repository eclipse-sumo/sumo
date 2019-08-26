/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPersonTrip.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
/// @version $Id$
///
// A class for visualizing personTrips in Netedit
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/vehicle/SUMOVehicleParameter.h>

#include "GNEPersonTrip.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEPersonTrip::GNEPersonTrip(GNEViewNet* viewNet, GNEDemandElement* personParent, const std::vector<GNEEdge*>& edges, const std::vector<std::string>& types,
                             const std::vector<std::string>& modes, double arrivalPosition) :
    GNEDemandElement(viewNet->getNet()->generateDemandElementID("", SUMO_TAG_PERSONTRIP_FROMTO), viewNet, GLO_PERSONTRIP, SUMO_TAG_PERSONTRIP_FROMTO,
                     edges, {}, {}, {}, {personParent}, {}, {}, {}, {}, {}),
                        Parameterised(),
                        myVTypes(types),
                        myModes(modes),
myArrivalPosition(arrivalPosition) {
}


GNEPersonTrip::GNEPersonTrip(GNEViewNet* viewNet, GNEDemandElement* personParent, const std::vector<GNEEdge*>& edges, GNEAdditional* busStop,
                             const std::vector<std::string>& types, const std::vector<std::string>& modes) :
    GNEDemandElement(viewNet->getNet()->generateDemandElementID("", SUMO_TAG_PERSONTRIP_BUSSTOP), viewNet, GLO_PERSONTRIP, SUMO_TAG_PERSONTRIP_BUSSTOP,
                     edges, {}, {}, {busStop}, {personParent}, {}, {}, {}, {}, {}),
Parameterised(),
myVTypes(types),
myModes(modes),
myArrivalPosition(-1) {
}


GNEPersonTrip::~GNEPersonTrip() {}


GUIGLObjectPopupMenu*
GNEPersonTrip::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
GNEPersonTrip::writeDemandElement(OutputDevice& device) const {
    // open tag
    device.openTag(SUMO_TAG_PERSONTRIP);
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
    // write modes
    if (myModes.size() > 0) {
        device.writeAttr(SUMO_ATTR_MODES, myModes);
    }
    // write vTypes
    if (myVTypes.size() > 0) {
        device.writeAttr(SUMO_ATTR_VTYPES, myVTypes);
    }
    // only write arrivalPos if is different of -1
    if (myArrivalPosition != -1) {
        device.writeAttr(SUMO_ATTR_ARRIVALPOS, myArrivalPosition);
    }
    // close tag
    device.closeTag();
}


bool
GNEPersonTrip::isDemandElementValid() const {
    if (getEdgeParents().size() == 0) {
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
GNEPersonTrip::getDemandElementProblem() const {
    if (getEdgeParents().size() == 0) {
        return ("A personTrip need at least one edge");
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
GNEPersonTrip::fixDemandElementProblem() {
    // currently the only solution is removing PersonTrip
}


GNEEdge*
GNEPersonTrip::getFromEdge() const {
    return getEdgeParents().front();
}


GNEEdge*
GNEPersonTrip::getToEdge() const {
    return getEdgeParents().back();
}


SUMOVehicleClass
GNEPersonTrip::getVClass() const {
    return getDemandElementParents().front()->getVClass();
}


const RGBColor&
GNEPersonTrip::getColor() const {
    return getDemandElementParents().front()->getColor();
}


void
GNEPersonTrip::compute() {
    // Nothing to compute
}


void
GNEPersonTrip::startGeometryMoving() {
    // only start geometry moving if arrival position isn't -1
    if (myArrivalPosition != -1) {
        // always save original position over view
        myPersonTripMove.originalViewPosition = getPositionInView();
        // save arrival position
        myPersonTripMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_ARRIVALPOS);
        // save current centering boundary
        myPersonTripMove.movingGeometryBoundary = getCenteringBoundary();
    }
}


void
GNEPersonTrip::endGeometryMoving() {
    // check that myArrivalPosition isn't -1 and endGeometryMoving was called only once
    if ((myArrivalPosition != -1) && myPersonTripMove.movingGeometryBoundary.isInitialised()) {
        // reset myMovingGeometryBoundary
        myPersonTripMove.movingGeometryBoundary.reset();
    }
}


void
GNEPersonTrip::moveGeometry(const Position& offset) {
    // only move if myArrivalPosition isn't -1
    if (myArrivalPosition != -1) {
        // Calculate new position using old position
        Position newPosition = myPersonTripMove.originalViewPosition;
        newPosition.add(offset);
        // filtern position using snap to active grid
        newPosition = myViewNet->snapToActiveGrid(newPosition);
        // obtain lane shape (to improve code legibility)
        const PositionVector& laneShape = getEdgeParents().back()->getLanes().front()->getGeometry().shape;
        // calculate offset lane
        double offsetLane = laneShape.nearest_offset_to_point2D(newPosition, false) - laneShape.nearest_offset_to_point2D(myPersonTripMove.originalViewPosition, false);        // Update arrival Position
        myArrivalPosition = parse<double>(myPersonTripMove.firstOriginalLanePosition) + offsetLane;
        // Update geometry
        updateGeometry();
    }
}


void
GNEPersonTrip::commitGeometryMoving(GNEUndoList* undoList) {
    // only commit geometry moving if myArrivalPosition isn't -1
    if (myArrivalPosition != -1) {
        undoList->p_begin("arrivalPos of " + getTagStr());
        undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_ARRIVALPOS, toString(myArrivalPosition), true, myPersonTripMove.firstOriginalLanePosition));
        undoList->p_end();
    }
}

void
GNEPersonTrip::updateGeometry() {
    getDemandElementParents().front()->updateGeometry();
    // update demand element childs
    for (const auto& i : getDemandElementChildren()) {
        i->updateGeometry();
    }
}


Position
GNEPersonTrip::getPositionInView() const {
    return Position();
}


std::string
GNEPersonTrip::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


Boundary
GNEPersonTrip::getCenteringBoundary() const {
    Boundary personTripBoundary;
    // return the combination of all edge parents's boundaries
    for (const auto& i : getEdgeParents()) {
        personTripBoundary.add(i->getCenteringBoundary());
    }
    // check if is valid
    if (personTripBoundary.isInitialised()) {
        return personTripBoundary;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNEPersonTrip::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // PersonTrips are drawn in GNEEdges
}


void
GNEPersonTrip::selectAttributeCarrier(bool changeFlag) {
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
GNEPersonTrip::unselectAttributeCarrier(bool changeFlag) {
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
GNEPersonTrip::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getDemandElementID();
        case SUMO_ATTR_FROM:
            return getEdgeParents().front()->getID();
        case SUMO_ATTR_TO:
            return getEdgeParents().back()->getID();
        case SUMO_ATTR_BUS_STOP:
            return getAdditionalParents().front()->getID();
        case SUMO_ATTR_MODES:
            return joinToString(myModes, " ");
        case SUMO_ATTR_VTYPES:
            return joinToString(myVTypes, " ");
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
GNEPersonTrip::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ARRIVALPOS:
            return myArrivalPosition;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEPersonTrip::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_MODES:
        case SUMO_ATTR_VTYPES:
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
GNEPersonTrip::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) && (myViewNet->getNet()->retrieveEdge(value, false) != nullptr);
        case SUMO_ATTR_BUS_STOP:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr);
        case SUMO_ATTR_MODES: {
            SVCPermissions dummyModeSet;
            std::string dummyError;
            return SUMOVehicleParameter::parsePersonModes(value, myTagProperty.getTagStr(), getID(), dummyModeSet, dummyError);
        }
        case SUMO_ATTR_VTYPES:
            return canParse<std::vector<std::string> >(value);
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
GNEPersonTrip::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


bool
GNEPersonTrip::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return true;
}


std::string
GNEPersonTrip::getPopUpID() const {
    return getTagStr();
}


std::string
GNEPersonTrip::getHierarchyName() const {
    if (myTagProperty.getTag() == SUMO_TAG_PERSONTRIP_FROMTO) {
        return "personTrip: " + getEdgeParents().front()->getID() + " -> " + getEdgeParents().back()->getID();
    } else {
        return "personTrip: " + getEdgeParents().front()->getID() + " -> " + getAdditionalParents().front()->getID();
    }
}



std::string
GNEPersonTrip::getGenericParametersStr() const {
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
GNEPersonTrip::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNEPersonTrip::setGenericParametersStr(const std::string& value) {
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
GNEPersonTrip::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_FROM: {
            // declare a from-via-to edges vector
            std::vector<std::string> FromViaToEdges;
            // add from edge
            FromViaToEdges.push_back(value);
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
            // add to edge
            FromViaToEdges.push_back(value);
            // calculate route
            std::vector<GNEEdge*> route = getRouteCalculatorInstance()->calculateDijkstraRoute(myViewNet->getNet(), getDemandElementParents().at(0)->getVClass(), FromViaToEdges);
            // change edge parents
            changeEdgeParents(this, toString(route));
            break;
        }
        case SUMO_ATTR_BUS_STOP:
            changeAdditionalParent(this, value, 0);
            break;
        case SUMO_ATTR_MODES:
            myModes = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_VTYPES:
            myVTypes = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
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
GNEPersonTrip::setEnabledAttribute(const int /*enabledAttributes*/) {
    //
}

/****************************************************************************/
