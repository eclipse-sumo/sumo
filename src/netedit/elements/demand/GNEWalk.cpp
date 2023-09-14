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
/// @file    GNEWalk.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
///
// A class for visualizing walks in Netedit
/****************************************************************************/

#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GLHelper.h>

#include "GNEWalk.h"
#include "GNERoute.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEWalk::GNEWalk(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_WALK, tag, GUIIconSubSys::getIcon(GUIIcon::WALK_FROMTO),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}),
    GNEDemandElementPlan(this, -1) {
    // reset default values
    resetDefaultValues();
}


GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge, double arrivalPosition) :
    GNEDemandElement(personParent, net, GLO_WALK, GNE_TAG_WALK_EDGE, GUIIconSubSys::getIcon(GUIIcon::WALK_FROMTO),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {fromEdge, toEdge}, {}, {}, {personParent}, {}),
    GNEDemandElementPlan(this, arrivalPosition) {
}


GNEWalk::GNEWalk(const bool isTrain, GNENet* net, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEAdditional* toAdditional, double arrivalPosition) :
    GNEDemandElement(personParent, net, GLO_WALK, isTrain ? GNE_TAG_WALK_TRAINSTOP : GNE_TAG_WALK_BUSSTOP, GUIIconSubSys::getIcon(isTrain ? GUIIcon::WALK_TRAINSTOP : GUIIcon::WALK_BUSSTOP),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {fromEdge}, {}, {toAdditional}, {personParent}, {}),
    GNEDemandElementPlan(this, arrivalPosition) {
}


GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, std::vector<GNEEdge*> edges, double arrivalPosition) :
    GNEDemandElement(personParent, net, GLO_WALK, GNE_TAG_WALK_EDGES, GUIIconSubSys::getIcon(GUIIcon::WALK_EDGES),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {edges}, {}, {}, {personParent}, {}),
    GNEDemandElementPlan(this, arrivalPosition) {
}


GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, GNEDemandElement* route, double arrivalPosition) :
    GNEDemandElement(personParent, net, GLO_WALK, GNE_TAG_WALK_ROUTE, GUIIconSubSys::getIcon(GUIIcon::WALK_ROUTE),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {personParent, route}, {}),
    GNEDemandElementPlan(this, arrivalPosition) {
}


GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, GNEJunction* fromJunction, GNEJunction* toJunction, double arrivalPosition) :
    GNEDemandElement(personParent, net, GLO_WALK, GNE_TAG_WALK_JUNCTIONS, GUIIconSubSys::getIcon(GUIIcon::WALK_JUNCTIONS),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {fromJunction, toJunction}, {}, {}, {}, {personParent}, {}),
    GNEDemandElementPlan(this, arrivalPosition) {
}


GNEWalk::GNEWalk(GNENet* net, GNEDemandElement* personParent, GNEAdditional* fromTAZ, GNEAdditional* toTAZ, double arrivalPosition) :
    GNEDemandElement(personParent, net, GLO_WALK, GNE_TAG_WALK_TAZS, GUIIconSubSys::getIcon(GUIIcon::WALK_TAZS),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {fromTAZ, toTAZ}, {personParent}, {}),
    GNEDemandElementPlan(this, arrivalPosition) {
}


GNEWalk::~GNEWalk() {}


GNEMoveOperation*
GNEWalk::getMoveOperation() {
    // avoid move person plan that ends in busStop or junction
    if ((getParentAdditionals().size() > 0) || (getParentJunctions().size() > 0)) {
        return nullptr;
    }
    // get geometry end pos
    const Position geometryEndPos = getPlanAttributePosition(GNE_ATTR_PLAN_GEOMETRY_ENDPOS);
    // calculate circle width squared
    const double circleWidthSquared = myPersonPlanArrivalPositionDiameter * myPersonPlanArrivalPositionDiameter;
    // check if we clicked over a geometry end pos
    if (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(geometryEndPos) <= ((circleWidthSquared + 2))) {
        // continue depending of parent edges
        if (getParentEdges().size() > 0) {
            return new GNEMoveOperation(this, getParentEdges().back()->getLaneByAllowedVClass(getVClass()), myArrivalPosition, false);
        } else {
            return new GNEMoveOperation(this, getParentDemandElements().at(1)->getParentEdges().back()->getLaneByAllowedVClass(getVClass()), myArrivalPosition, false);
        }
    } else {
        return nullptr;
    }
}


GUIGLObjectPopupMenu*
GNEWalk::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    return getPlanPopUpMenu(app, parent);
}


void
GNEWalk::writeDemandElement(OutputDevice& device) const {
    // open tag
    device.openTag(SUMO_TAG_WALK);
    // write plan attributes
    writePlanAttributes(device);
    // close tag
    device.closeTag();
}


GNEDemandElement::Problem
GNEWalk::isDemandElementValid() const {
    return isPersonPlanValid();
}


std::string
GNEWalk::getDemandElementProblem() const {
    return getPersonPlanProblem();
}


void
GNEWalk::fixDemandElementProblem() {
    // currently the only solution is removing Walk
}


SUMOVehicleClass
GNEWalk::getVClass() const {
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNEWalk::getColor() const {
    return getParentDemandElements().front()->getColor();
}


void
GNEWalk::updateGeometry() {
    updatePlanGeometry();
}


Position
GNEWalk::getPositionInView() const {
    return getPlanPositionInView();
}


std::string
GNEWalk::getParentName() const {
    return getParentDemandElements().front()->getID();
}


Boundary
GNEWalk::getCenteringBoundary() const {
    return getPlanCenteringBoundary();
}


void
GNEWalk::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList) {
    // only split geometry of WalkEdges
    if (myTagProperty.getTag() == GNE_TAG_WALK_EDGES) {
        // obtain new list of walk edges
        std::string newWalkEdges = getNewListOfParents(originalElement, newElement);
        // update walk edges
        if (newWalkEdges.size() > 0) {
            setAttribute(SUMO_ATTR_EDGES, newWalkEdges, undoList);
        }
    }
}


void
GNEWalk::drawGL(const GUIVisualizationSettings& s) const {
    drawPlanGL(s, s.colorSettings.walkColor);
}


void
GNEWalk::computePathElement() {
    computePlanPathElement();
}


void
GNEWalk::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // draw person plan over lane
    drawPlanPartial(drawPersonPlan(), s, lane, segment, offsetFront, s.widthSettings.walkWidth, s.colorSettings.walkColor);
}


void
GNEWalk::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // draw person plan over junction
    drawPlanPartial(drawPersonPlan(), s, fromLane, toLane, segment, offsetFront, s.widthSettings.walkWidth, s.colorSettings.walkColor);
}


GNELane*
GNEWalk::getFirstPathLane() const {
    return getFirstPlanPathLane();
}


GNELane*
GNEWalk::getLastPathLane() const {
    return getLastPlanPathLane();
}


std::string
GNEWalk::getAttribute(SumoXMLAttr key) const {
    return getPlanAttribute(key);
}


double
GNEWalk::getAttributeDouble(SumoXMLAttr key) const {
    return getPlanAttributeDouble(key);
}


Position
GNEWalk::getAttributePosition(SumoXMLAttr key) const {
    return getPlanAttributePosition(key);
}


void
GNEWalk::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    setPlanAttribute(key, value, undoList);
}


bool
GNEWalk::isValid(SumoXMLAttr key, const std::string& value) {
    return isPlanValid(key, value);
}


bool
GNEWalk::isAttributeEnabled(SumoXMLAttr key) const {
    return isPlanAttributeEnabled(key);
}


std::string
GNEWalk::getPopUpID() const {
    return getTagStr();
}


std::string
GNEWalk::getHierarchyName() const {
    return getPlanHierarchyName();
}


const Parameterised::Map&
GNEWalk::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEWalk::setAttribute(SumoXMLAttr key, const std::string& value) {
    setPlanAttribute(key, value);
}


void
GNEWalk::setMoveShape(const GNEMoveResult& moveResult) {
    // change both position
    myArrivalPosition = moveResult.newFirstPos;
    // update geometry
    updateGeometry();
}


void
GNEWalk::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(this, "arrivalPos of " + getTagStr());
    // now adjust start position
    setAttribute(SUMO_ATTR_ARRIVALPOS, toString(moveResult.newFirstPos), undoList);
    undoList->end();
}

/****************************************************************************/
