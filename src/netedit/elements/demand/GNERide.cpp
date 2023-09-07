/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GNERide.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
///
// A class for visualizing rides in Netedit
/****************************************************************************/

#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNERide.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNERide::GNERide(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_RIDE, tag, GUIIconSubSys::getIcon(GUIIcon::RIDE_FROMTO),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}),
    GNEDemandElementPlan(this),
    myArrivalPosition(0) {
    // reset default values
    resetDefaultValues();
}


GNERide::GNERide(GNENet* net, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge,
                 double arrivalPosition, const std::vector<std::string>& lines) :
    GNEDemandElement(personParent, net, GLO_RIDE, GNE_TAG_RIDE_EDGE, GUIIconSubSys::getIcon(GUIIcon::RIDE_FROMTO),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {fromEdge, toEdge}, {}, {}, {personParent}, {}),
    GNEDemandElementPlan(this),
    myArrivalPosition(arrivalPosition),
    myLines(lines) {
}


GNERide::GNERide(bool isTrain, GNENet* net, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEAdditional* toBusStop,
                 double arrivalPosition, const std::vector<std::string>& lines) :
    GNEDemandElement(personParent, net, GLO_RIDE, isTrain ? GNE_TAG_RIDE_TRAINSTOP : GNE_TAG_RIDE_BUSSTOP,
                     GUIIconSubSys::getIcon(isTrain ? GUIIcon::RIDE_TRAINSTOP : GUIIcon::RIDE_BUSSTOP),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {fromEdge}, {}, {toBusStop}, {personParent}, {}),
    GNEDemandElementPlan(this),
    myArrivalPosition(arrivalPosition),
    myLines(lines) {
}


GNERide::~GNERide() {}


GNEMoveOperation*
GNERide::getMoveOperation() {
    // avoid move person plan that ends in busStop
    if (getParentAdditionals().size() > 0) {
        return nullptr;
    }
    // get geometry end pos
    const Position geometryEndPos = getPathElementArrivalPos();
    // calculate circle width squared
    const double circleWidthSquared = myPersonPlanArrivalPositionDiameter * myPersonPlanArrivalPositionDiameter;
    // check if we clicked over a geometry end pos
    if (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(geometryEndPos) <= ((circleWidthSquared + 2))) {
        return new GNEMoveOperation(this, getParentEdges().back()->getLaneByDisallowedVClass(getVClass()), myArrivalPosition, false);
    } else {
        return nullptr;
    }
}


GUIGLObjectPopupMenu*
GNERide::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, app);
    // build menu commands for names
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + getTagStr() + " name to clipboard", nullptr, ret, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + getTagStr() + " typed name to clipboard", nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open demand element dialog
    if (myTagProperty.hasDialog()) {
        GUIDesigns::buildFXMenuCommand(ret, "Open " + getTagStr() + " Dialog", getACIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    GUIDesigns::buildFXMenuCommand(ret, "Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y()), nullptr, nullptr, 0);
    return ret;
}


void
GNERide::writeDemandElement(OutputDevice& device) const {
    // open tag
    device.openTag(SUMO_TAG_RIDE);
    // check if from attribute is enabled
    if (isAttributeEnabled(SUMO_ATTR_FROM)) {
        device.writeAttr(SUMO_ATTR_FROM, getParentEdges().front()->getID());
    }
    // write to depending if personplan ends in a busStop
    if (getParentAdditionals().size() > 0) {
        if (getParentAdditionals().back()->getTagProperty().getTag() == SUMO_TAG_BUS_STOP) {
            device.writeAttr(SUMO_ATTR_BUS_STOP, getParentAdditionals().back()->getID());
        } else {
            device.writeAttr(SUMO_ATTR_TRAIN_STOP, getParentAdditionals().back()->getID());
        }
    } else {
        device.writeAttr(SUMO_ATTR_TO, getParentEdges().back()->getID());
    }
    // avoid write arrival positions in ride to busStop
    if ((myTagProperty.getTag() != GNE_TAG_RIDE_BUSSTOP) && (myTagProperty.getTag() != GNE_TAG_RIDE_TRAINSTOP) &&
            (myArrivalPosition > 0)) {
        device.writeAttr(SUMO_ATTR_ARRIVALPOS, myArrivalPosition);
    }
    // write lines
    if (myLines.empty()) {
        device.writeAttr(SUMO_ATTR_LINES, "ANY");
    } else {
        device.writeAttr(SUMO_ATTR_LINES, myLines);
    }
    // close tag
    device.closeTag();
}


GNEDemandElement::Problem
GNERide::isDemandElementValid() const {
    return isPersonPlanValid();
}


std::string
GNERide::getDemandElementProblem() const {
    return getPersonPlanProblem();
}


void
GNERide::fixDemandElementProblem() {
    // currently the only solution is removing Ride
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
GNERide::updateGeometry() {
    // update child demand elements
    for (const auto& i : getChildDemandElements()) {
        i->updateGeometry();
    }
}


Position
GNERide::getPositionInView() const {
    return getParentEdges().front()->getPositionInView();
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
GNERide::drawGL(const GUIVisualizationSettings& s) const {
    // force draw path
    myNet->getPathManager()->forceDrawPath(s, this);
}


void
GNERide::computePathElement() {
    // get lanes
    const std::vector<GNELane*> lanes = {getFirstPathLane(), getLastPathLane()};
    // calculate path
    myNet->getPathManager()->calculatePathLanes(this, SVC_PASSENGER, lanes);
    // check path (taxis)
    if (!myNet->getPathManager()->isPathValid(this)) {
        myNet->getPathManager()->calculatePathLanes(this, SVC_TAXI, lanes);
    }
    // check path (bus)
    if (!myNet->getPathManager()->isPathValid(this)) {
        myNet->getPathManager()->calculatePathLanes(this, SVC_BUS, lanes);
    }
    // check path (bicycle)
    if (!myNet->getPathManager()->isPathValid(this)) {
        myNet->getPathManager()->calculatePathLanes(this, SVC_BICYCLE, lanes);
    }
    // check path (pedestrian)
    if (!myNet->getPathManager()->isPathValid(this)) {
        myNet->getPathManager()->calculatePathLanes(this, SVC_PEDESTRIAN, lanes);
    }
    // update geometry
    updateGeometry();
}


void
GNERide::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // draw person plan over lane
    drawPersonPlanPartial(drawPersonPlan(), s, lane, segment, offsetFront, s.widthSettings.rideWidth, s.colorSettings.rideColor);
}


void
GNERide::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // draw person plan over junction
    drawPersonPlanPartial(drawPersonPlan(), s, fromLane, toLane, segment, offsetFront, s.widthSettings.rideWidth, s.colorSettings.rideColor);
}


GNELane*
GNERide::getFirstPathLane() const {
    return getParentEdges().front()->getLaneByDisallowedVClass(SVC_PEDESTRIAN);
}


GNELane*
GNERide::getLastPathLane() const {
    // check if personPlan ends in a BusStop
    if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getParentLanes().front();
    } else {
        return getParentEdges().back()->getLaneByDisallowedVClass(SVC_PEDESTRIAN);
    }
}


std::string
GNERide::getAttribute(SumoXMLAttr key) const {
    switch (key) {
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
        default:
            return getPlanAttribute(key);
    }
}


double
GNERide::getAttributeDouble(SumoXMLAttr key) const {
    return getPlanAttributeDouble(key, myArrivalPosition);
}


Position
GNERide::getAttributePosition(SumoXMLAttr key) const {
    return getPlanAttributePosition(key, myArrivalPosition);
}


void
GNERide::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ARRIVALPOS:
        case SUMO_ATTR_LINES:
        case GNE_ATTR_SELECTED:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setPlanAttribute(key, value, undoList);
            break;
    }
}


bool
GNERide::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_LINES:
            return canParse<std::vector<std::string> >(value);
        case SUMO_ATTR_ARRIVALPOS:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                if (isTemplate()) {
                    return true;
                }
                const double parsedValue = canParse<double>(value);
                if ((parsedValue < 0) || (parsedValue > getLastPathLane()->getLaneShape().length())) {
                    return false;
                } else {
                    return true;
                }
            } else {
                return false;
            }
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        default:
            return isPlanValid(key, value, {SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW});
    }
}


bool
GNERide::isAttributeEnabled(SumoXMLAttr key) const {
    return isPlanAttributeEnabled(key);
}


std::string
GNERide::getPopUpID() const {
    return getTagStr();
}


std::string
GNERide::getHierarchyName() const {
    return getPlanHierarchyName();
}


const Parameterised::Map&
GNERide::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// private
// ===========================================================================

void
GNERide::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // Common person plan attributes
        case SUMO_ATTR_FROM:
            // change first edge
            replaceFirstParentEdge(value);
            // compute ride
            computePathElement();
            break;
        case SUMO_ATTR_TO:
            // change last edge
            replaceLastParentEdge(value);
            // compute ride
            computePathElement();
            break;
        case GNE_ATTR_TO_BUSSTOP:
            replaceFirstParentAdditional(SUMO_TAG_BUS_STOP, value);
            // compute ride
            computePathElement();
            break;
        case GNE_ATTR_TO_TRAINSTOP:
            replaceFirstParentAdditional(SUMO_TAG_TRAIN_STOP, value);
            // compute ride
            computePathElement();
            break;
        // specific person plan attributes
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
        case GNE_ATTR_PARENT:
            if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_PERSON, value, false) != nullptr) {
                replaceDemandElementParent(SUMO_TAG_PERSON, value, 0);
            } else if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_PERSONFLOW, value, false) != nullptr) {
                replaceDemandElementParent(SUMO_TAG_PERSONFLOW, value, 0);
            }
            updateGeometry();
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERide::setMoveShape(const GNEMoveResult& moveResult) {
    // change both position
    myArrivalPosition = moveResult.newSecondPos;
    // update geometry
    updateGeometry();
}


void
GNERide::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(this, "arrivalPos of " + getTagStr());
    // now adjust start position
    setAttribute(SUMO_ATTR_ARRIVALPOS, toString(moveResult.newFirstPos), undoList);
    undoList->end();
}

/****************************************************************************/
