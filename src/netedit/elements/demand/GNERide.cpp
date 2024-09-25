/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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

GNERide::GNERide(GNENet* net, SumoXMLTag tag, GUIIcon icon, GNEDemandElement* personParent, const GNEPlanParents& planParameters,
                 const double arrivalPosition, const std::vector<std::string>& lines, const std::string& group) :
    GNEDemandElement(personParent, net, GLO_PERSONTRIP, tag, GUIIconSubSys::getIcon(icon),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT,
                     planParameters.getJunctions(), planParameters.getEdges(), {},
planParameters.getAdditionalElements(), planParameters.getDemandElements(personParent), {}),
                                     GNEDemandElementPlan(this, -1, arrivalPosition),
                                     myLines(lines),
myGroup(group) {
}


GNERide::GNERide(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_RIDE, tag, GUIIconSubSys::getIcon(GUIIcon::RIDE_EDGE),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}),
GNEDemandElementPlan(this, -1, -1) {
    // reset default values
    resetDefaultValues();
}


GNERide::~GNERide() {}


GNEMoveOperation*
GNERide::getMoveOperation() {
    return getPlanMoveOperation();
}


GUIGLObjectPopupMenu*
GNERide::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    return getPlanPopUpMenu(app, parent);
}


void
GNERide::writeDemandElement(OutputDevice& device) const {
    // first write origin stop (if this element starts in a stoppingPlace)
    writeOriginStop(device);
    // write rest of attributes
    device.openTag(SUMO_TAG_RIDE);
    writeLocationAttributes(device);
    // lines
    if (myLines.size() > 0) {
        device.writeAttr(SUMO_ATTR_LINES, myLines);
    }
    // group
    if (myGroup.size() > 0) {
        device.writeAttr(SUMO_ATTR_GROUP, myGroup);
    }
    device.closeTag();
}


GNEDemandElement::Problem
GNERide::isDemandElementValid() const {
    return isPlanPersonValid();
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
    return SVC_PEDESTRIAN;
}


const RGBColor&
GNERide::getColor() const {
    return myNet->getViewNet()->getVisualisationSettings().colorSettings.rideColor;
}


void
GNERide::updateGeometry() {
    updatePlanGeometry();
}


Position
GNERide::getPositionInView() const {
    return getPlanPositionInView();
}


std::string
GNERide::getParentName() const {
    return getParentDemandElements().front()->getID();
}


Boundary
GNERide::getCenteringBoundary() const {
    return getPlanCenteringBoundary();
}


void
GNERide::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNERide::drawGL(const GUIVisualizationSettings& s) const {
    drawPlanGL(checkDrawPersonPlan(), s, s.colorSettings.rideColor, s.colorSettings.selectedPersonPlanColor);
}


void
GNERide::computePathElement() {
    computePlanPathElement();
}


void
GNERide::drawLanePartialGL(const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment, const double offsetFront) const {
    drawPlanLanePartial(checkDrawPersonPlan(), s, segment, offsetFront, s.widthSettings.rideWidth, s.colorSettings.rideColor, s.colorSettings.selectedPersonPlanColor);
}


void
GNERide::drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment, const double offsetFront) const {
    drawPlanJunctionPartial(checkDrawPersonPlan(), s, segment, offsetFront, s.widthSettings.rideWidth, s.colorSettings.rideColor, s.colorSettings.selectedPersonPlanColor);
}


GNELane*
GNERide::getFirstPathLane() const {
    return getFirstPlanPathLane();
}


GNELane*
GNERide::getLastPathLane() const {
    return getLastPlanPathLane();
}


std::string
GNERide::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_LINES:
            return joinToString(myLines, " ");
        case SUMO_ATTR_GROUP:
            return myGroup;
        default:
            return getPlanAttribute(key);
    }
}


double
GNERide::getAttributeDouble(SumoXMLAttr key) const {
    return getPlanAttributeDouble(key);
}


Position
GNERide::getAttributePosition(SumoXMLAttr key) const {
    return getPlanAttributePosition(key);
}


void
GNERide::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_LINES:
        case SUMO_ATTR_GROUP:
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
        case SUMO_ATTR_GROUP:
            return true;
        default:
            return isPlanValid(key, value);
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
        case SUMO_ATTR_LINES:
            myLines = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_GROUP:
            myGroup = value;
            break;
        default:
            setPlanAttribute(key, value);
            break;
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
