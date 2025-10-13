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
/// @file    GNETranship.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2021
///
// A class for visualizing tranships in Netedit
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>

#include "GNETranship.h"


// ===========================================================================
// method definitions
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355) // mask warning about "this" in initializers
#endif
GNETranship::GNETranship(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, "", tag, GNEPathElement::Options::DEMAND_ELEMENT),
    GNEDemandElementPlan(this, -1, -1) {
}


GNETranship::GNETranship(SumoXMLTag tag, GNEDemandElement* containerParent, const GNEPlanParents& planParameters,
                         const double departPosition, const double arrivalPosition, const double speed, const SUMOTime duration) :
    GNEDemandElement(containerParent, tag, GNEPathElement::Options::DEMAND_ELEMENT),
    GNEDemandElementPlan(this, departPosition, arrivalPosition),
    mySpeed(speed),
    myDuration(duration) {
    // set parents
    setParents<GNEJunction*>(planParameters.getJunctions());
    setParents<GNEEdge*>(planParameters.getEdges());
    setParents<GNEAdditional*>(planParameters.getAdditionalElements());
    setParents<GNEDemandElement*>(planParameters.getDemandElements(containerParent));
    // update centering boundary without updating grid
    updatePlanCenteringBoundary(false);
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

GNETranship::~GNETranship() {}


GNEMoveElement*
GNETranship::getMoveElement() const {
    return myMoveElementPlan;
}


Parameterised*
GNETranship::getParameters() {
    return nullptr;
}


const Parameterised*
GNETranship::getParameters() const {
    return nullptr;
}


GUIGLObjectPopupMenu*
GNETranship::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    return getPlanPopUpMenu(app, parent);
}


void
GNETranship::writeDemandElement(OutputDevice& device) const {
    // first write origin stop (if this element starts in a stoppingPlace)
    writeOriginStop(device);
    // write rest of attributes
    device.openTag(SUMO_TAG_TRANSHIP);
    writeLocationAttributes(device);
    // speed
    if (mySpeed != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_SPEED)) {
        device.writeAttr(SUMO_ATTR_SPEED, mySpeed);
    }
    // duration
    if (myDuration != myTagProperty->getDefaultTimeValue(SUMO_ATTR_DURATION)) {
        device.writeAttr(SUMO_ATTR_DURATION, time2string(myDuration));
    }
    device.closeTag();
}


GNEDemandElement::Problem
GNETranship::isDemandElementValid() const {
    return isPlanPersonValid();
}


std::string
GNETranship::getDemandElementProblem() const {
    return getPersonPlanProblem();
}


void
GNETranship::fixDemandElementProblem() {
    // currently the only solution is removing Tranship
}


SUMOVehicleClass
GNETranship::getVClass() const {
    return SVC_IGNORING;
}


const RGBColor&
GNETranship::getColor() const {
    return myNet->getViewNet()->getVisualisationSettings().colorSettings.transhipColor;
}


void
GNETranship::updateGeometry() {
    updatePlanGeometry();
}


Position
GNETranship::getPositionInView() const {
    return getPlanPositionInView();
}


std::string
GNETranship::getParentName() const {
    return getParentDemandElements().front()->getID();
}


Boundary
GNETranship::getCenteringBoundary() const {
    return getPlanCenteringBoundary();
}


void
GNETranship::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList) {
    // only split geometry of TranshipEdges
    if (myTagProperty->getTag() == GNE_TAG_TRANSHIP_EDGES) {
        // obtain new list of tranship edges
        std::string newTranshipEdges = getNewListOfParents(originalElement, newElement);
        // update tranship edges
        if (newTranshipEdges.size() > 0) {
            setAttribute(SUMO_ATTR_EDGES, newTranshipEdges, undoList);
        }
    }
}


void
GNETranship::drawGL(const GUIVisualizationSettings& s) const {
    drawPlanGL(checkDrawContainerPlan(), s, s.colorSettings.transhipColor, s.colorSettings.selectedContainerPlanColor);
}


void
GNETranship::computePathElement() {
    computePlanPathElement();
}


void
GNETranship::drawLanePartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
    drawPlanLanePartial(checkDrawContainerPlan(), s, segment, offsetFront, s.widthSettings.transhipWidth, s.colorSettings.transhipColor, s.colorSettings.selectedContainerPlanColor);
}


void
GNETranship::drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
    drawPlanJunctionPartial(checkDrawContainerPlan(), s, segment, offsetFront, s.widthSettings.transhipWidth, s.colorSettings.transhipColor, s.colorSettings.selectedContainerPlanColor);
}


GNELane*
GNETranship::getFirstPathLane() const {
    return getFirstPlanPathLane();
}


GNELane*
GNETranship::getLastPathLane() const {
    return getLastPlanPathLane();
}


std::string
GNETranship::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_SPEED:
            if (mySpeed == myTagProperty->getDefaultDoubleValue(key)) {
                return "";
            } else {
                return toString(mySpeed);
            }
        case SUMO_ATTR_DURATION:
            if (myDuration == myTagProperty->getDefaultTimeValue(key)) {
                return "";
            } else {
                return time2string(myDuration);
            }
        default:
            return getPlanAttribute(key);
    }
}


double
GNETranship::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_SPEED:
            return mySpeed;
        default:
            return getPlanAttributeDouble(key);
    }
}


Position
GNETranship::getAttributePosition(SumoXMLAttr key) const {
    return getPlanAttributePosition(key);
}


void
GNETranship::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_SPEED:
        case SUMO_ATTR_DURATION:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setPlanAttribute(key, value, undoList);
            break;
    }
}


bool
GNETranship::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_SPEED:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_DURATION:
            return canParse<SUMOTime>(value) && (parse<SUMOTime>(value) >= 0);
        default:
            return isPlanValid(key, value);
    }
}


bool
GNETranship::isAttributeEnabled(SumoXMLAttr key) const {
    return isPlanAttributeEnabled(key);
}


std::string
GNETranship::getPopUpID() const {
    return getTagStr();
}


std::string
GNETranship::getHierarchyName() const {
    return getPlanHierarchyName();
}

// ===========================================================================
// private
// ===========================================================================

void
GNETranship::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_SPEED:
            if (value.empty()) {
                mySpeed = myTagProperty->getDefaultDoubleValue(key);
            } else {
                mySpeed = GNEAttributeCarrier::parse<double>(value);
            }
            break;
        case SUMO_ATTR_DURATION:
            if (value.empty()) {
                myDuration = myTagProperty->getDefaultTimeValue(key);
            } else {
                myDuration = GNEAttributeCarrier::parse<SUMOTime>(value);
            }
            break;
        default:
            setPlanAttribute(key, value);
            break;
    }
}

/****************************************************************************/
