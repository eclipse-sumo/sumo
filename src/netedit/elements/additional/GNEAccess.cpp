/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEAccess.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2018
///
//
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEAccess.h"
#include "GNEAdditionalHandler.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAccess::GNEAccess(GNENet* net) :
    GNEAdditional("", net, GLO_ACCESS, SUMO_TAG_ACCESS, "",
        {}, {}, {}, {}, {}, {}, {}, {},
    std::map<std::string, std::string>()),
    myPositionOverLane(0),
    myLength(0),
    myFriendlyPosition(false) {
    // reset default values
    resetDefaultValues();
}


GNEAccess::GNEAccess(GNEAdditional* busStop, GNELane* lane, GNENet* net, double pos, const double length, bool friendlyPos,
                     const std::map<std::string, std::string>& parameters) :
    GNEAdditional(net, GLO_ACCESS, SUMO_TAG_ACCESS, "",
        {}, {}, {lane}, {busStop}, {}, {}, {}, {},
    parameters),
    myPositionOverLane(pos),
    myLength(length),
    myFriendlyPosition(friendlyPos) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEAccess::~GNEAccess() {
}


GNEMoveOperation*
GNEAccess::getMoveOperation() {
    // return move operation for additional placed over shape
    return new GNEMoveOperation(this, getParentLanes().front(), myPositionOverLane,
                                myNet->getViewNet()->getViewParent()->getMoveFrame()->getCommonModeOptions()->getAllowChangeLane());
}


void
GNEAccess::updateGeometry() {
    // set start position
    double fixedPositionOverLane;
    if (myPositionOverLane == -1) {
        fixedPositionOverLane = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    } else if (myPositionOverLane < 0) {
        fixedPositionOverLane = 0;
    } else if (myPositionOverLane > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
        fixedPositionOverLane = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    } else {
        fixedPositionOverLane = myPositionOverLane;
    }
    // update geometry
    myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), fixedPositionOverLane * getParentLanes().front()->getLengthGeometryFactor(), myMoveElementLateralOffset);
}


Position
GNEAccess::getPositionInView() const {
    return myAdditionalGeometry.getShape().getPolygonCenter();
}


void
GNEAccess::updateCenteringBoundary(const bool /*updateGrid*/) {
    myAdditionalBoundary.reset();
    // add center
    myAdditionalBoundary.add(getPositionInView());
    // grow
    myAdditionalBoundary.grow(10);
}


void
GNEAccess::splitEdgeGeometry(const double splitPosition, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* newElement, GNEUndoList* undoList) {
    if (splitPosition < myPositionOverLane) {
        // change lane
        setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
        // now adjust start position
        setAttribute(SUMO_ATTR_POSITION, toString(myPositionOverLane - splitPosition), undoList);
    }
}


bool
GNEAccess::isAccessPositionFixed() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else {
        if (myPositionOverLane != -1) {
            return (myPositionOverLane >= 0) && (myPositionOverLane <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
        } else {
            return false;
        }
    }
}


void
GNEAccess::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_ACCESS);
    device.writeAttr(SUMO_ATTR_LANE, getParentLanes().front()->getID());
    device.writeAttr(SUMO_ATTR_POSITION, myPositionOverLane);
    if (myLength != -1) {
        device.writeAttr(SUMO_ATTR_LENGTH, myLength);
    }
    if (myFriendlyPosition) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, true);
    }
    device.closeTag();
}


GNEEdge*
GNEAccess::getEdge() const {
    return getParentLanes().front()->getParentEdge();
}


std::string
GNEAccess::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNEAccess::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration
    const double accessExaggeration = getExaggeration(s);
    // declare width
    const double radius = 0.5;
    // first check if additional has to be drawn
    if (s.drawAdditionals(accessExaggeration) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // get color
        RGBColor color;
        if (drawUsingSelectColor()) {
            color = s.colorSettings.selectedAdditionalColor;
        } else if (!getParentAdditionals().front()->getAttribute(SUMO_ATTR_COLOR).empty()) {
            color = parse<RGBColor>(getParentAdditionals().front()->getAttribute(SUMO_ATTR_COLOR));
        } else {
            color = s.colorSettings.busStopColor;
        }
        // draw parent and child lines
        drawParentChildLines(s, color);
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // push layer matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_ACCESS);
        // set color
        GLHelper::setColor(color);
        // translate to geometry position
        glTranslated(myAdditionalGeometry.getShape().front().x(), myAdditionalGeometry.getShape().front().y(), 0);
        // draw circle
        if (s.drawForRectangleSelection) {
            GLHelper::drawFilledCircle(radius * accessExaggeration, 8);
        } else {
            GLHelper::drawFilledCircle(radius * accessExaggeration, 16);
        }
        // pop layer matrix
        GLHelper::popMatrix();
        // pop gl identficador
        GLHelper::popName();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), myAdditionalGeometry.getShape().front(), accessExaggeration, 0.3);
        // check if dotted contours has to be drawn
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            GUIDottedGeometry::drawDottedContourCircle(GUIDottedGeometry::DottedContourType::INSPECT, s, myAdditionalGeometry.getShape().front(), 0.5, accessExaggeration);
        }
        if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
            GUIDottedGeometry::drawDottedContourCircle(GUIDottedGeometry::DottedContourType::FRONT, s, myAdditionalGeometry.getShape().front(), 0.5, accessExaggeration);
        }
    }
}


std::string
GNEAccess::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getParentAdditionals().front()->getID();
        case SUMO_ATTR_LANE:
            return getParentLanes().front()->getID();
        case SUMO_ATTR_POSITION:
            return toString(myPositionOverLane);
        case SUMO_ATTR_LENGTH:
            return toString(myLength);
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEAccess::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


void
GNEAccess::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_FRIENDLY_POS:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
        case GNE_ATTR_SHIFTLANEINDEX:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEAccess::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_LANE: {
            GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(value, false);
            if (lane != nullptr) {
                if (getParentLanes().front()->getParentEdge()->getID() != lane->getParentEdge()->getID()) {
                    return GNEAdditionalHandler::accessCanBeCreated(getParentAdditionals().at(0), lane->getParentEdge());
                } else {
                    return true;
                }
            } else {
                return false;
            }
        }
        case SUMO_ATTR_POSITION:
            if (value.empty()) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_LENGTH:
            if (canParse<double>(value)) {
                const double valueDouble = parse<double>(value);
                return (valueDouble == -1) || (valueDouble >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEAccess::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}


std::string
GNEAccess::getPopUpID() const {
    return getTagStr();
}


std::string
GNEAccess::getHierarchyName() const {
    return getTagStr() + ": " + getParentLanes().front()->getParentEdge()->getID();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEAccess::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_LANE:
            replaceAdditionalParentLanes(value);
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = parse<double>(value);
            break;
        case SUMO_ATTR_LENGTH:
            myLength = parse<double>(value);
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
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
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEAccess::setMoveShape(const GNEMoveResult& moveResult) {
    // change both position
    myPositionOverLane = moveResult.newFirstPos;
    // set lateral offset
    myMoveElementLateralOffset = moveResult.firstLaneOffset;
    // update geometry
    updateGeometry();
}


void
GNEAccess::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // reset lateral offset
    myMoveElementLateralOffset = 0;
    undoList->begin(GUIIcon::ACCESS, "position of " + getTagStr());
    // now adjust start position
    setAttribute(SUMO_ATTR_POSITION, toString(moveResult.newFirstPos), undoList);
    // check if lane has to be changed
    if (moveResult.newFirstLane) {
        // set new lane
        setAttribute(SUMO_ATTR_LANE, moveResult.newFirstLane->getID(), undoList);
    }
    // end change attribute
    undoList->end();
}


/****************************************************************************/
