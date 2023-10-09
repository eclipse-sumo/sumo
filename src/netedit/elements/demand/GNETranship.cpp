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
/// @file    GNETranship.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2021
///
// A class for visualizing tranships in Netedit
/****************************************************************************/

#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNETranship.h"
#include "GNERoute.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNETranship*
GNETranship::buildTranship(GNENet* net, GNEDemandElement* containerParent, GNEEdge* fromEdge,
        GNEAdditional* fromContainerStop, GNEEdge* toEdge, GNEAdditional* toContainerStop,
        std::vector<GNEEdge*> edgeList, double arrivalPosition) {
    // declare icon an tag
    const auto iconTag = getTagIconTranship(edgeList, fromEdge, toEdge, fromContainerStop, toContainerStop);
    // declare containers
    std::vector<GNEEdge*> edges;
    std::vector<GNEAdditional*> additionals;
    // continue depending of input parameters
    if (edgeList.size() > 0) {
        edges = edgeList;
    } else {
        if (fromEdge) {
            edges.push_back(fromEdge);
        } else if (fromContainerStop) {
            additionals.push_back(fromContainerStop);
        }
        if (toEdge) {
            edges.push_back(toEdge);
        } else if (toContainerStop) {
            additionals.push_back(toContainerStop);
        }
    }
    return new GNETranship(net, iconTag.first, iconTag.second, containerParent, edges, additionals, arrivalPosition);
}


GNETranship::GNETranship(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_TRANSHIP, tag, GUIIconSubSys::getIcon(GUIIcon::TRANSHIP_EDGE),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}),
    GNEDemandElementPlan(this, -1),
    mySpeed(0),
    myDepartPosition(0) {
    // reset default values
    resetDefaultValues();
}


GNETranship::~GNETranship() {}


GNEMoveOperation*
GNETranship::getMoveOperation() {
    // only move tranships defined over edges
    if (myTagProperty.planToEdge() || (myTagProperty.getTag() == GNE_TAG_TRANSHIP_EDGES)) {
        // get geometry end pos
        const Position geometryEndPos = getPlanAttributePosition(GNE_ATTR_PLAN_GEOMETRY_ENDPOS);
        // calculate circle width squared
        const double circleWidthSquared = myArrivalPositionDiameter * myArrivalPositionDiameter;
        // check if we clicked over a geometry end pos
        if (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(geometryEndPos) <= ((circleWidthSquared + 2))) {
            // continue depending of parent edges
            if (getParentEdges().size() > 0) {
                return new GNEMoveOperation(this, getParentEdges().back()->getLaneByAllowedVClass(getVClass()), myArrivalPosition, false);
            } else {
                return new GNEMoveOperation(this, getParentDemandElements().at(1)->getParentEdges().back()->getLaneByAllowedVClass(getVClass()), myArrivalPosition, false);
            }
        }
    }
    return nullptr;
}


GUIGLObjectPopupMenu*
GNETranship::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    return getPlanPopUpMenu(app, parent);
}


void
GNETranship::writeDemandElement(OutputDevice& device) const {
    // open tag
    device.openTag(SUMO_TAG_WALK);
    // write plan attributes
    writePlanAttributes(device);
    // only write departPos if is different of -1
    if (myDepartPosition != -1) {
        device.writeAttr(SUMO_ATTR_DEPARTPOS, myDepartPosition);
    }
    // close tag
    device.closeTag();
}


GNEDemandElement::Problem
GNETranship::isDemandElementValid() const {
    return isPersonPlanValid();
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
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNETranship::getColor() const {
    return getParentDemandElements().front()->getColor();
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
    if (myTagProperty.getTag() == GNE_TAG_TRANSHIP_EDGES) {
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
    drawPlanGL(checkDrawPersonPlan(), s, s.colorSettings.walkColor, s.colorSettings.selectedPersonPlanColor);
}


void
GNETranship::computePathElement() {
    computePlanPathElement();
}


void
GNETranship::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    drawPlanPartial(checkDrawContainerPlan(), s, lane, segment, offsetFront, s.widthSettings.transhipWidth, s.colorSettings.transhipColor, s.colorSettings.selectedContainerPlanColor);
}


void
GNETranship::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    drawPlanPartial(checkDrawContainerPlan(), s, fromLane, toLane, segment, offsetFront, s.widthSettings.transhipWidth, s.colorSettings.transhipColor, s.colorSettings.selectedContainerPlanColor);
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
        // Common attributes
        case SUMO_ATTR_DEPARTPOS:
            if (myDepartPosition == -1) {
                return "";
            } else {
                return toString(myDepartPosition);
            }
        case SUMO_ATTR_SPEED:
            return toString(mySpeed);
        default:
            return getPlanAttribute(key);
    }
}


double
GNETranship::getAttributeDouble(SumoXMLAttr key) const {
    return getPlanAttributeDouble(key);
}


Position
GNETranship::getAttributePosition(SumoXMLAttr key) const {
    return getPlanAttributePosition(key);
}


void
GNETranship::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        // Common attributes
        case SUMO_ATTR_DEPARTPOS:
        case SUMO_ATTR_SPEED:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setPlanAttribute(key, value, undoList);
            break;
    }
}


bool
GNETranship::isValid(SumoXMLAttr key, const std::string& value) {
    return isPlanValid(key, value);
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


const Parameterised::Map&
GNETranship::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// private
// ===========================================================================

void
GNETranship::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        // Common attributes
        case SUMO_ATTR_DEPARTPOS:
        case SUMO_ATTR_SPEED:
            break;
        default:
            setPlanAttribute(key, value);
            break;
    }
}


void
GNETranship::setMoveShape(const GNEMoveResult& moveResult) {
    // change both position
    myArrivalPosition = moveResult.newFirstPos;
    // update geometry
    updateGeometry();
}


void
GNETranship::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(this, "arrivalPos of " + getTagStr());
    // now adjust start position
    setAttribute(SUMO_ATTR_ARRIVALPOS, toString(moveResult.newFirstPos), undoList);
    undoList->end();
}


GNETranship::GNETranship(GNENet* net, SumoXMLTag tag, GUIIcon icon, GNEDemandElement* containerParent,
                         const std::vector<GNEEdge*> &edges, const std::vector<GNEAdditional*> &additionals,
                         double arrivalPosition) :
    GNEDemandElement(containerParent, net, GLO_TRANSHIP, tag, GUIIconSubSys::getIcon(icon),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, edges, {}, additionals, {containerParent}, {}),
    GNEDemandElementPlan(this, arrivalPosition) {
}

/****************************************************************************/
