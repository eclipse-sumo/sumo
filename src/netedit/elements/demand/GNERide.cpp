/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>

#include "GNERide.h"


// ===========================================================================
// method definitions
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355) // mask warning about "this" in initializers
#endif
GNERide::GNERide(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, "", tag, GNEPathElement::Options::DEMAND_ELEMENT),
    GNEDemandElementPlan(this, -1, -1) {
}


GNERide::GNERide(SumoXMLTag tag, GNEDemandElement* personParent, const GNEPlanParents& planParameters,
                 const double arrivalPosition, const std::vector<std::string>& lines, const std::string& group) :
    GNEDemandElement(personParent, tag, GNEPathElement::Options::DEMAND_ELEMENT),
    GNEDemandElementPlan(this, -1, arrivalPosition),
    myLines(lines),
    myGroup(group) {
    // set parents
    setParents<GNEJunction*>(planParameters.getJunctions());
    setParents<GNEEdge*>(planParameters.getEdges());
    setParents<GNEAdditional*>(planParameters.getAdditionalElements());
    setParents<GNEDemandElement*>(planParameters.getDemandElements(personParent));
    // update centering boundary without updating grid
    updatePlanCenteringBoundary(false);
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

GNERide::~GNERide() {}


GNEMoveElement*
GNERide::getMoveElement() const {
    return myMoveElementPlan;
}


Parameterised*
GNERide::getParameters() {
    return nullptr;
}


const Parameterised*
GNERide::getParameters() const {
    return nullptr;
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
GNERide::drawLanePartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
    drawPlanLanePartial(checkDrawPersonPlan(), s, segment, offsetFront, s.widthSettings.rideWidth, s.colorSettings.rideColor, s.colorSettings.selectedPersonPlanColor);
}


void
GNERide::drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
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

/****************************************************************************/
