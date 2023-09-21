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

GNEWalk*
GNEWalk::buildWalk(GNENet* net, GNEDemandElement* personParent, 
        GNEEdge* fromEdge, GNEAdditional* fromTAZ, GNEJunction* fromJunction,
        GNEEdge* toEdge, GNEAdditional* toTAZ, GNEJunction* toJunction, GNEAdditional* toBusStop, GNEAdditional* toTrainStop,
        double arrivalPosition, std::vector<GNEEdge*> edgeList, GNEDemandElement* route) {
    // declare icon an tag
    SumoXMLTag tag = SUMO_TAG_NOTHING;
    GUIIcon icon = GUIIcon::PERSON;
    // declare containers
    std::vector<GNEDemandElement*> demandElements = {personParent};
    std::vector<GNEJunction*> junctions;
    std::vector<GNEEdge*> edges;
    std::vector<GNEAdditional*> additionals;
    // continue depending of input parameters
    if (edgeList.size() > 0) {
        edges = edgeList;
        tag = GNE_TAG_WALK_EDGES;
        icon = GUIIcon::WALK_EDGES;
    } else if (route) {
        demandElements.push_back(route);
        tag = GNE_TAG_WALK_ROUTE;
        icon = GUIIcon::WALK_ROUTE;
    } else if (fromEdge) {
        edges.push_back(fromEdge);
        if (toEdge) {
            edges.push_back(toEdge);
            tag = GNE_TAG_WALK_EDGE_EDGE;
            icon = GUIIcon::WALK_EDGE;
        } else if (toTAZ) {
            additionals.push_back(toTAZ);
            tag = GNE_TAG_WALK_EDGE_TAZ;
            icon = GUIIcon::WALK_TAZ;
        } else if (toBusStop) {
            additionals.push_back(toBusStop);
            tag = GNE_TAG_WALK_EDGE_BUSSTOP;
            icon = GUIIcon::WALK_BUSSTOP;
        } else if (toTrainStop) {
            additionals.push_back(toTrainStop);
            tag = GNE_TAG_WALK_EDGE_TRAINSTOP;
            icon = GUIIcon::WALK_TRAINSTOP;
        }
    } else if (fromTAZ) {
        additionals.push_back(fromTAZ);
        if (toEdge) {
            edges.push_back(toEdge);
            tag = GNE_TAG_WALK_TAZ_EDGE;
            icon = GUIIcon::WALK_EDGE;
        } else if (toTAZ) {
            additionals.push_back(toTAZ);
            tag = GNE_TAG_WALK_TAZ_TAZ;
            icon = GUIIcon::WALK_TAZ;
        } else if (toBusStop) {
            additionals.push_back(toBusStop);
            tag = GNE_TAG_WALK_TAZ_BUSSTOP;
            icon = GUIIcon::WALK_BUSSTOP;
        } else if (toTrainStop) {
            additionals.push_back(toTrainStop);
            tag = GNE_TAG_WALK_TAZ_TRAINSTOP;
            icon = GUIIcon::WALK_TRAINSTOP;
        }
    } else if (fromJunction) {
        junctions.push_back(fromJunction);
        if (toJunction) {
            junctions.push_back(toJunction);
            tag = GNE_TAG_WALK_JUNCTION_JUNCTION;
            icon = GUIIcon::WALK_JUNCTION;
        }
    }
    // check if combination was correct
    if (tag == SUMO_TAG_NOTHING) {
        throw ProcessError("Invalid walk input combination");
    } else {
        return new GNEWalk(net, tag, icon, demandElements, junctions, edges, additionals,arrivalPosition);
    }
}


GNEWalk::GNEWalk(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_WALK, tag, GUIIconSubSys::getIcon(GUIIcon::WALK_EDGE),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}),
    GNEDemandElementPlan(this, -1) {
    // reset default values
    resetDefaultValues();
}


GNEWalk::~GNEWalk() {}


GNEMoveOperation*
GNEWalk::getMoveOperation() {
    // only move personTrips defined over edges
    if ((myTagProperty.getTag() == GNE_TAG_WALK_EDGE_EDGE) || (myTagProperty.getTag() == GNE_TAG_WALK_EDGES)) {
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
    drawPlanGL(checkDrawPersonPlan(), s, s.colorSettings.walkColor, s.colorSettings.selectedPersonPlanColor);
}


void
GNEWalk::computePathElement() {
    computePlanPathElement();
}


void
GNEWalk::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    drawPlanPartial(checkDrawPersonPlan(), s, lane, segment, offsetFront, s.widthSettings.walkWidth, s.colorSettings.walkColor, s.colorSettings.selectedPersonPlanColor);
}


void
GNEWalk::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    drawPlanPartial(checkDrawPersonPlan(), s, fromLane, toLane, segment, offsetFront, s.widthSettings.walkWidth, s.colorSettings.walkColor, s.colorSettings.selectedPersonPlanColor);
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


GNEWalk::GNEWalk(GNENet* net, SumoXMLTag tag, GUIIcon icon, std::vector<GNEDemandElement*> &parents, const std::vector<GNEJunction*> &junctions,
                             const std::vector<GNEEdge*> &edges, const std::vector<GNEAdditional*> &additionals, double arrivalPosition) :
    GNEDemandElement(parents.front(), net, GLO_PERSONTRIP, tag, GUIIconSubSys::getIcon(icon),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, junctions, edges, {}, additionals, parents, {}),
    GNEDemandElementPlan(this, arrivalPosition) {
}

/****************************************************************************/
