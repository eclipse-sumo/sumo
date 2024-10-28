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
    GNEDemandElement("", net, GLO_WALK, tag, GUIIconSubSys::getIcon(GUIIcon::WALK_EDGE),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}),
GNEDemandElementPlan(this, -1, -1) {
    // reset default values
    resetDefaultValues();
}


GNEWalk::GNEWalk(GNENet* net, SumoXMLTag tag, GUIIcon icon, GNEDemandElement* personParent, const GNEPlanParents& planParameters,
                 const double arrivalPosition, const double speed, const SUMOTime duration) :
    GNEDemandElement(personParent, net, GLO_WALK, tag, GUIIconSubSys::getIcon(icon),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT,
                     planParameters.getJunctions(), planParameters.getEdges(), {},
planParameters.getAdditionalElements(), planParameters.getDemandElements(personParent), {}),
GNEDemandElementPlan(this, -1, arrivalPosition),
mySpeed(speed),
myDuration(duration) {
}


GNEWalk::~GNEWalk() {}


GNEMoveOperation*
GNEWalk::getMoveOperation() {
    return getPlanMoveOperation();
}


GUIGLObjectPopupMenu*
GNEWalk::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    return getPlanPopUpMenu(app, parent);
}


void
GNEWalk::writeDemandElement(OutputDevice& device) const {
    // first write origin stop (if this element starts in a stoppingPlace)
    writeOriginStop(device);
    // write rest of attributes
    device.openTag(SUMO_TAG_WALK);
    writeLocationAttributes(device);
    // speed
    if ((mySpeed > 0) && (toString(mySpeed) != myTagProperty.getDefaultValue(SUMO_ATTR_SPEED))) {
        device.writeAttr(SUMO_ATTR_SPEED, mySpeed);
    }
    // duration
    if (toString(myDuration) != myTagProperty.getDefaultValue(SUMO_ATTR_DURATION)) {
        device.writeAttr(SUMO_ATTR_DURATION, time2string(myDuration));
    }
    device.closeTag();
}


GNEDemandElement::Problem
GNEWalk::isDemandElementValid() const {
    return isPlanPersonValid();
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
    return SVC_PEDESTRIAN;
}


const RGBColor&
GNEWalk::getColor() const {
    return myNet->getViewNet()->getVisualisationSettings().colorSettings.walkColor;
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
GNEWalk::drawLanePartialGL(const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment, const double offsetFront) const {
    drawPlanLanePartial(checkDrawPersonPlan(), s, segment, offsetFront, s.widthSettings.walkWidth, s.colorSettings.walkColor, s.colorSettings.selectedPersonPlanColor);
}


void
GNEWalk::drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment, const double offsetFront) const {
    drawPlanJunctionPartial(checkDrawPersonPlan(), s, segment, offsetFront, s.widthSettings.walkWidth, s.colorSettings.walkColor, s.colorSettings.selectedPersonPlanColor);
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
    switch (key) {
        case SUMO_ATTR_SPEED:
            if (mySpeed == 0) {
                return "";
            } else {
                return toString(mySpeed);
            }
        case SUMO_ATTR_DURATION:
            if (myDuration == 0) {
                return "";
            } else {
                return time2string(myDuration);
            }
        default:
            return getPlanAttribute(key);
    }
}


double
GNEWalk::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_SPEED:
            return mySpeed;
        default:
            return getPlanAttributeDouble(key);
    }
}


Position
GNEWalk::getAttributePosition(SumoXMLAttr key) const {
    return getPlanAttributePosition(key);
}


void
GNEWalk::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
GNEWalk::isValid(SumoXMLAttr key, const std::string& value) {
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
    switch (key) {
        case SUMO_ATTR_SPEED:
            if (value.empty()) {
                mySpeed = 0;
            } else {
                mySpeed = GNEAttributeCarrier::parse<double>(value);
            }
            break;
        case SUMO_ATTR_DURATION:
            if (value.empty()) {
                mySpeed = 0;
            } else {
                myDuration = GNEAttributeCarrier::parse<SUMOTime>(value);
            }
            break;
        default:
            setPlanAttribute(key, value);
            break;
    }
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
