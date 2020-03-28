/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNERide.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
///
// A class for visualizing rides in Netedit
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIGlobalSelection.h>

#include "GNERide.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNERide::GNERide(GNEViewNet* viewNet, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge, const std::vector<GNEEdge*>& via,
                 double arrivalPosition, const std::vector<std::string>& lines) :
    GNEDemandElement(viewNet->getNet()->generateDemandElementID("", SUMO_TAG_RIDE_FROMTO), viewNet, GLO_RIDE, SUMO_TAG_RIDE_FROMTO, 
        {fromEdge, toEdge}, {}, {}, {}, {personParent}, {}, {}, {}, {}, {}, {}, {}),
    Parameterised(),
    myArrivalPosition(arrivalPosition),
    myLines(lines) {
    // set via parameter without updating references
    replaceMiddleParentEdges(this, via, false);
    // compute ride
    computePath();
}


GNERide::GNERide(GNEViewNet* viewNet, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEAdditional* busStop, const std::vector<GNEEdge*>& via,
                 const std::vector<std::string>& lines) :
    GNEDemandElement(viewNet->getNet()->generateDemandElementID("", SUMO_TAG_RIDE_BUSSTOP), viewNet, GLO_RIDE, SUMO_TAG_RIDE_BUSSTOP, 
        {fromEdge}, {}, {}, {busStop}, {personParent}, {}, {}, {}, {}, {}, {}, {}),
    Parameterised(),
    myArrivalPosition(-1),
    myLines(lines) {
    // set via parameter without updating references
    replaceMiddleParentEdges(this, via, false);
    // compute ride
    computePath();
}


GNERide::~GNERide() {}


GUIGLObjectPopupMenu*
GNERide::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
GNERide::writeDemandElement(OutputDevice& device) const {
    // open tag
    device.openTag(SUMO_TAG_RIDE);
    // write attributes depending  of ride type
    if (getParentDemandElements().front()->getChildDemandElements().front() == this) {
        device.writeAttr(SUMO_ATTR_FROM, getParentEdges().front()->getID());
    }
    // check if write busStop or edge to
    if (getParentAdditionals().size() > 0) {
        device.writeAttr(SUMO_ATTR_BUS_STOP, getParentAdditionals().front()->getID());
    } else {
        device.writeAttr(SUMO_ATTR_TO, getParentEdges().back()->getID());
    }
    // only write arrivalPos if is different of -1
    if (myArrivalPosition != -1) {
        device.writeAttr(SUMO_ATTR_ARRIVALPOS, myArrivalPosition);
    }
    // write parameters
    writeParams(device);
    // close tag
    device.closeTag();
}


bool
GNERide::isDemandElementValid() const {
    if ((getParentEdges().size() == 2) && (getParentEdges().at(0) == getParentEdges().at(1))) {
        // from and to are the same edges
        return true;
    } else if (getPathEdges().size() > 0) {
        // if path edges isn't empty, then there is a valid route
        return true;
    } else {
        return false;
    }
}


std::string
GNERide::getDemandElementProblem() const {
    if (getParentEdges().size() == 0) {
        return ("A ride need at least one edge");
    } else {
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)getParentEdges().size(); i++) {
            if (myViewNet->getNet()->getPathCalculator()->consecutiveEdgesConnected(getParentDemandElements().front()->getVClass(), getParentEdges().at((int)i - 1), getParentEdges().at(i)) == false) {
                return ("Edge '" + getParentEdges().at((int)i - 1)->getID() + "' and edge '" + getParentEdges().at(i)->getID() + "' aren't consecutives");
            }
        }
        // there is connections bewteen all edges, then all ok
        return "";
    }
}


void
GNERide::fixDemandElementProblem() {
    // currently the only solution is removing Ride
}


GNEEdge*
GNERide::getFromEdge() const {
    if (getParentDemandElements().size() == 2) {
        // obtain position and rotation of first edge route
        return getParentDemandElements().at(1)->getFromEdge();
    } else {
        return getParentEdges().front();
    }
}


GNEEdge*
GNERide::getToEdge() const {
    if (getParentDemandElements().size() == 2) {
        // obtain position and rotation of first edge route
        return getParentDemandElements().at(1)->getToEdge();
    } else {
        return getParentEdges().back();
    }
}


SUMOVehicleClass
GNERide::getVClass() const {
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNERide::getColor() const {
    return getParentDemandElements().front()->getColor();
}


void
GNERide::startGeometryMoving() {
    // only start geometry moving if arrival position isn't -1
    if (myArrivalPosition != -1) {
        // always save original position over view
        myRideMove.originalViewPosition = getPositionInView();
        // save arrival position
        myRideMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_ARRIVALPOS);
        // save current centering boundary
        myRideMove.movingGeometryBoundary = getCenteringBoundary();
    }
}


void
GNERide::endGeometryMoving() {
    // check that myArrivalPosition isn't -1 and endGeometryMoving was called only once
    if ((myArrivalPosition != -1) && myRideMove.movingGeometryBoundary.isInitialised()) {
        // reset myMovingGeometryBoundary
        myRideMove.movingGeometryBoundary.reset();
    }
}


void
GNERide::moveGeometry(const Position& offset) {
    // only move if myArrivalPosition isn't -1
    if (myArrivalPosition != -1) {
        // Calculate new position using old position
        Position newPosition = myRideMove.originalViewPosition;
        newPosition.add(offset);
        // filtern position using snap to active grid
        newPosition = myViewNet->snapToActiveGrid(newPosition);
        // obtain lane shape (to improve code legibility)
        const PositionVector& laneShape = getParentEdges().back()->getLanes().front()->getLaneShape();
        // calculate offset lane
        double offsetLane = laneShape.nearest_offset_to_point2D(newPosition, false) - laneShape.nearest_offset_to_point2D(myRideMove.originalViewPosition, false);
        std::cout << offsetLane << std::endl;
        // Update arrival Position
        myArrivalPosition = parse<double>(myRideMove.firstOriginalLanePosition) + offsetLane;
        // Update geometry
        updateGeometry();
    }
}


void
GNERide::commitGeometryMoving(GNEUndoList* undoList) {
    // only commit geometry moving if myArrivalPosition isn't -1
    if (myArrivalPosition != -1) {
        undoList->p_begin("arrivalPos of " + getTagStr());
        undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_ARRIVALPOS, toString(myArrivalPosition), true, myRideMove.firstOriginalLanePosition));
        undoList->p_end();
    }
}


void
GNERide::updateGeometry() {
    // declare depart and arrival pos lane
    double departPosLane = -1;
    double arrivalPosLane = -1;
    // declare start and end positions
    Position startPos = Position::INVALID;
    Position endPos = Position::INVALID;
    // calculate person plan start and end lanepositions
    calculatePersonPlanLaneStartEndPos(departPosLane, arrivalPosLane);
    // calculate person plan start and end positions
    calculatePersonPlanPositionStartEndPos(startPos, endPos);
    // calculate geometry path
    if (getPathEdges().size() > 0) {
        GNEGeometry::calculateEdgeGeometricPath(this, myDemandElementSegmentGeometry, getPathEdges(), getVClass(),
                                                getFirstAllowedVehicleLane(), getLastAllowedVehicleLane(), departPosLane, arrivalPosLane, startPos, endPos);
    } else {
        GNEGeometry::calculateEdgeGeometricPath(this, myDemandElementSegmentGeometry, getParentEdges(), getVClass(),
                                                getFirstAllowedVehicleLane(), getLastAllowedVehicleLane(), departPosLane, arrivalPosLane, startPos, endPos);
    }
    // update child demand elementss
    for (const auto& i : getChildDemandElements()) {
        i->updateGeometry();
    }
}

void
GNERide::updateDottedContour() {
    //
}


void
GNERide::updatePartialGeometry(const GNEEdge* edge) {
    // declare depart and arrival pos lane
    double departPosLane = -1;
    double arrivalPosLane = -1;
    // declare start and end positions
    Position startPos = Position::INVALID;
    Position endPos = Position::INVALID;
    // calculate person plan start and end lanepositions
    calculatePersonPlanLaneStartEndPos(departPosLane, arrivalPosLane);
    // calculate person plan start and end positions
    calculatePersonPlanPositionStartEndPos(startPos, endPos);
    // calculate geometry path
    GNEGeometry::updateGeometricPath(myDemandElementSegmentGeometry, edge, departPosLane, arrivalPosLane, startPos, endPos);
    // update child demand elementss
    for (const auto& i : getChildDemandElements()) {
        i->updatePartialGeometry(edge);
    }
}


void
GNERide::computePath() {
    if (myTagProperty.getTag() == SUMO_TAG_RIDE_FROMTO) {
        // calculate route and update routeEdges
        replacePathEdges(this, myViewNet->getNet()->getPathCalculator()->calculatePath(getParentDemandElements().at(0)->getVClass(), getParentEdges()));
    } else if (myTagProperty.getTag() == SUMO_TAG_RIDE_BUSSTOP) {
        // declare a from-via-busStop edges vector
        std::vector<GNEEdge*> fromViaBusStopEdges = getParentEdges();
        // add busStop edge
        fromViaBusStopEdges.push_back(getParentAdditionals().front()->getParentLanes().front()->getParentEdge());
        // calculate route and update routeEdges
        replacePathEdges(this, myViewNet->getNet()->getPathCalculator()->calculatePath(getParentDemandElements().at(0)->getVClass(), fromViaBusStopEdges));
    }
    // update geometry
    updateGeometry();
}


void
GNERide::invalidatePath() {
    if (myTagProperty.getTag() == SUMO_TAG_RIDE_FROMTO) {
        // calculate route and update routeEdges
        replacePathEdges(this, getParentEdges());
    } else if (myTagProperty.getTag() == SUMO_TAG_RIDE_BUSSTOP) {
        // declare a from-via-busStop edges vector
        std::vector<GNEEdge*> fromViaBusStopEdges = getParentEdges();
        // add busStop edge
        fromViaBusStopEdges.push_back(getParentAdditionals().front()->getParentLanes().front()->getParentEdge());
        // calculate route and update routeEdges
        replacePathEdges(this, fromViaBusStopEdges);
    }
    // update geometry
    updateGeometry();
}


Position
GNERide::getPositionInView() const {
    return Position();
}


std::string
GNERide::getParentName() const {
    return getParentDemandElements().front()->getID();
}


Boundary
GNERide::getCenteringBoundary() const {
    Boundary rideBoundary;
    // return the combination of all parent edges's boundaries
    for (const auto& i : getParentEdges()) {
        rideBoundary.add(i->getCenteringBoundary());
    }
    // check if is valid
    if (rideBoundary.isInitialised()) {
        return rideBoundary;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNERide::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNERide::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // Rides are drawn in GNEEdges
}


void
GNERide::selectAttributeCarrier(bool changeFlag) {
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
GNERide::unselectAttributeCarrier(bool changeFlag) {
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
GNERide::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_FROM:
            return getParentEdges().front()->getID();
        case SUMO_ATTR_TO:
            return getParentEdges().back()->getID();
        case SUMO_ATTR_VIA:
            return toString(getMiddleParentEdges());
        case SUMO_ATTR_BUS_STOP:
            return getParentAdditionals().front()->getID();
        case SUMO_ATTR_LINES:
            return joinToString(myLines, " ");
        case SUMO_ATTR_ARRIVALPOS:
            if (myArrivalPosition == -1) {
                return "";
            } else {
                return toString(myArrivalPosition);
            }
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        case GNE_ATTR_PARENT:
            return getParentDemandElements().front()->getID();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNERide::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ARRIVALPOS:
            if (myArrivalPosition != -1) {
                return myArrivalPosition;
            } else {
                return (getLastAllowedVehicleLane()->getLaneShape().length() - POSITION_EPS);
            }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERide::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case SUMO_ATTR_VIA:
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_LINES:
        case SUMO_ATTR_ARRIVALPOS:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERide::isValid(SumoXMLAttr key, const std::string& value) {
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
        case SUMO_ATTR_LINES:
            return canParse<std::vector<std::string> >(value);
        case SUMO_ATTR_ARRIVALPOS:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                const double parsedValue = canParse<double>(value);
                if ((parsedValue < 0) || (parsedValue > getLastAllowedVehicleLane()->getLaneShape().length())) {
                    return false;
                } else {
                    return true;
                }
            } else {
                return false;
            }
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERide::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


void
GNERide::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


bool
GNERide::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return true;
}


std::string
GNERide::getPopUpID() const {
    return getTagStr();
}


std::string
GNERide::getHierarchyName() const {
    if (myTagProperty.getTag() == SUMO_TAG_RIDE_FROMTO) {
        return "ride: " + getParentEdges().front()->getID() + " -> " + getParentEdges().back()->getID();
    } else {
        return "ride: " + getParentEdges().front()->getID() + " -> " + getParentAdditionals().front()->getID();
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNERide::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // Specific of Trips and flow
        case SUMO_ATTR_FROM: {
            // change first edge
            replaceFirstParentEdge(this, myViewNet->getNet()->retrieveEdge(value));
            // compute ride
            computePath();
            break;
        }
        case SUMO_ATTR_TO: {
            // change last edge
            replaceLastParentEdge(this, myViewNet->getNet()->retrieveEdge(value));
            // compute ride
            computePath();
            break;
        }
        case SUMO_ATTR_VIA: {
            // update via
            replaceMiddleParentEdges(this, parse<std::vector<GNEEdge*> >(myViewNet->getNet(), value), true);
            // compute ride
            computePath();
            break;
        }
        case SUMO_ATTR_BUS_STOP:
            replaceParentAdditional(this, value, 0);
            // compute ride
            computePath();
            break;
        case SUMO_ATTR_LINES:
            myLines = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_ARRIVALPOS:
            if (value.empty()) {
                myArrivalPosition = -1;
            } else {
                myArrivalPosition = parse<double>(value);
            }
            updateGeometry();
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERide::setEnabledAttribute(const int /*enabledAttributes*/) {
    //
}


/****************************************************************************/
