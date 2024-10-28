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
/// @file    GNETransport.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2021
///
// A class for visualizing transports in Netedit
/****************************************************************************/

#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNETransport.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNETransport::GNETransport(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_TRANSPORT, tag, GUIIconSubSys::getIcon(GUIIcon::TRANSHIP_EDGE),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}),
GNEDemandElementPlan(this, -1, -1) {
    // reset default values
    resetDefaultValues();
}


GNETransport::GNETransport(GNENet* net, SumoXMLTag tag, GUIIcon icon, GNEDemandElement* containerParent, const GNEPlanParents& planParameters,
                           const double arrivalPosition, const std::vector<std::string>& lines, const std::string& group) :
    GNEDemandElement(containerParent, net, GLO_TRANSPORT, tag, GUIIconSubSys::getIcon(icon),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT,
                     planParameters.getJunctions(), planParameters.getEdges(), {},
planParameters.getAdditionalElements(), planParameters.getDemandElements(containerParent), {}),
GNEDemandElementPlan(this, -1, arrivalPosition),
myLines(lines),
myGroup(group) {
}


GNETransport::~GNETransport() {}


GNEMoveOperation*
GNETransport::getMoveOperation() {
    return getPlanMoveOperation();
}


GUIGLObjectPopupMenu*
GNETransport::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    return getPlanPopUpMenu(app, parent);
}


void
GNETransport::writeDemandElement(OutputDevice& device) const {
    // first write origin stop (if this element starts in a stoppingPlace)
    writeOriginStop(device);
    // write rest of attributes
    device.openTag(SUMO_TAG_TRANSPORT);
    writeLocationAttributes(device);
    if (myLines.size() > 0) {
        device.writeAttr(SUMO_ATTR_LINES, myLines);
    }
    if (myGroup.size() > 0) {
        device.writeAttr(SUMO_ATTR_GROUP, myGroup);
    }
    device.closeTag();
}


GNEDemandElement::Problem
GNETransport::isDemandElementValid() const {
    return isPlanPersonValid();
}


std::string
GNETransport::getDemandElementProblem() const {
    return getPersonPlanProblem();
}


void
GNETransport::fixDemandElementProblem() {
    // currently the only solution is removing Transport
}


SUMOVehicleClass
GNETransport::getVClass() const {
    return SVC_IGNORING;
}


const RGBColor&
GNETransport::getColor() const {
    return myNet->getViewNet()->getVisualisationSettings().colorSettings.transportColor;
}


void
GNETransport::updateGeometry() {
    updatePlanGeometry();
}


Position
GNETransport::getPositionInView() const {
    return getPlanPositionInView();
}


std::string
GNETransport::getParentName() const {
    return getParentDemandElements().front()->getID();
}


Boundary
GNETransport::getCenteringBoundary() const {
    return getPlanCenteringBoundary();
}


void
GNETransport::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // Nothing to do
}


void
GNETransport::drawGL(const GUIVisualizationSettings& s) const {
    drawPlanGL(checkDrawContainerPlan(), s, s.colorSettings.transportColor, s.colorSettings.selectedContainerPlanColor);
}


void
GNETransport::computePathElement() {
    computePlanPathElement();
}


void
GNETransport::drawLanePartialGL(const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment, const double offsetFront) const {
    drawPlanLanePartial(checkDrawContainerPlan(), s, segment, offsetFront, s.widthSettings.transportWidth, s.colorSettings.transportColor, s.colorSettings.selectedContainerPlanColor);
}


void
GNETransport::drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment, const double offsetFront) const {
    drawPlanJunctionPartial(checkDrawContainerPlan(), s, segment, offsetFront, s.widthSettings.transportWidth, s.colorSettings.transportColor, s.colorSettings.selectedContainerPlanColor);
}


GNELane*
GNETransport::getFirstPathLane() const {
    return getFirstPlanPathLane();
}


GNELane*
GNETransport::getLastPathLane() const {
    return getLastPlanPathLane();
}


std::string
GNETransport::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        // specific person plan attributes
        case SUMO_ATTR_LINES:
            return joinToString(myLines, " ");
        case SUMO_ATTR_GROUP:
            return myGroup;
        default:
            return getPlanAttribute(key);
    }
}


double
GNETransport::getAttributeDouble(SumoXMLAttr key) const {
    return getPlanAttributeDouble(key);
}


Position
GNETransport::getAttributePosition(SumoXMLAttr key) const {
    return getPlanAttributePosition(key);
}


void
GNETransport::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
GNETransport::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // specific person plan attributes
        case SUMO_ATTR_LINES:
            return canParse<std::vector<std::string> >(value);
        case SUMO_ATTR_GROUP:
            return true;
        default:
            return isPlanValid(key, value);
    }
}


bool
GNETransport::isAttributeEnabled(SumoXMLAttr key) const {
    return isPlanAttributeEnabled(key);
}


std::string
GNETransport::getPopUpID() const {
    return getTagStr();
}


std::string
GNETransport::getHierarchyName() const {
    return getPlanHierarchyName();
}


const Parameterised::Map&
GNETransport::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// private
// ===========================================================================

void
GNETransport::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // specific person plan attributes
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
GNETransport::setMoveShape(const GNEMoveResult& moveResult) {
    // change both position
    myArrivalPosition = moveResult.newFirstPos;
    // update geometry
    updateGeometry();
}


void
GNETransport::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(this, "arrivalPos of " + getTagStr());
    // now adjust start position
    setAttribute(SUMO_ATTR_ARRIVALPOS, toString(moveResult.newFirstPos), undoList);
    undoList->end();
}

/****************************************************************************/
