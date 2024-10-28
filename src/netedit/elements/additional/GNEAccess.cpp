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
/// @file    GNEAccess.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2018
///
//
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEAccess.h"
#include "GNEAdditionalHandler.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAccess::GNEAccess(GNENet* net) :
    GNEAdditional("", net, GLO_ACCESS, SUMO_TAG_ACCESS, GUIIconSubSys::getIcon(GUIIcon::ACCESS), "", {}, {}, {}, {}, {}, {}),
              myPositionOverLane(0),
              myLength(0),
myFriendlyPosition(false) {
    // reset default values
    resetDefaultValues();
}


GNEAccess::GNEAccess(GNEAdditional* busStop, GNELane* lane, GNENet* net, const double pos, const std::string& specialPos,
                     const bool friendlyPos, const double length, const Parameterised::Map& parameters) :
    GNEAdditional(net, GLO_ACCESS, SUMO_TAG_ACCESS, GUIIconSubSys::getIcon(GUIIcon::ACCESS), "", {}, {}, {lane}, {busStop}, {}, {}),
Parameterised(parameters),
myPositionOverLane(pos),
mySpecialPosition(specialPos),
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
    if (myPositionOverLane < 0) {
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
    // nothing to update
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
        if (myPositionOverLane != INVALID_DOUBLE) {
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
    device.writeAttr(SUMO_ATTR_POSITION, getAttribute(SUMO_ATTR_POSITION));
    if (myLength != -1) {
        device.writeAttr(SUMO_ATTR_LENGTH, myLength);
    }
    if (myFriendlyPosition) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, true);
    }
    device.closeTag();
}


bool
GNEAccess::isAdditionalValid() const {
    // with friendly position enabled position is "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else if (myPositionOverLane == INVALID_DOUBLE) {
        return true;
    } else {
        return fabs(myPositionOverLane) <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    }
}


std::string GNEAccess::getAdditionalProblem() const {
    // obtain final length
    const double len = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    // check if detector has a problem
    if (GNEAdditionalHandler::checkLanePosition(myPositionOverLane, 0, len, myFriendlyPosition)) {
        return "";
    } else {
        // declare variable for error position
        std::string errorPosition;
        // check positions over lane
        if (myPositionOverLane < 0) {
            errorPosition = (toString(SUMO_ATTR_POSITION) + " < 0");
        }
        if (myPositionOverLane > len) {
            errorPosition = (toString(SUMO_ATTR_POSITION) + TL(" > lanes's length"));
        }
        return errorPosition;
    }
}


void GNEAccess::fixAdditionalProblem() {
    // declare new position
    double newPositionOverLane = myPositionOverLane;
    // declare new length (but unsed in this context)
    double length = 0;
    // fix pos and length with fixLanePosition
    GNEAdditionalHandler::fixLanePosition(newPositionOverLane, length, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
    // set new position
    setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
}


bool
GNEAccess::checkDrawMoveContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in move mode
    if (!myNet->getViewNet()->isCurrentlyMovingElements() && editModes.isCurrentSupermodeNetwork() &&
            !myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement() &&
            (editModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) && myNet->getViewNet()->checkOverLockedElement(this, mySelected)) {
        // only move the first element
        return myNet->getViewNet()->getViewObjectsSelector().getGUIGlObjectFront() == this;
    } else {
        return false;
    }
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
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // Obtain exaggeration
        const double accessExaggeration = getExaggeration(s);
        // get detail level
        const auto d = s.getDetailLevel(1);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // radius depends if mouse is over element
            const double radius = gViewObjectsHandler.isElementSelected(this) ? 1 : 0.5;
            // get color
            RGBColor accessColor;
            if (drawUsingSelectColor()) {
                accessColor = s.colorSettings.selectedAdditionalColor;
            } else if (!getParentAdditionals().front()->getAttribute(SUMO_ATTR_COLOR).empty()) {
                accessColor = parse<RGBColor>(getParentAdditionals().front()->getAttribute(SUMO_ATTR_COLOR));
            } else {
                accessColor = s.colorSettings.busStopColor;
            }
            // draw parent and child lines
            drawParentChildLines(s, accessColor);
            // push layer matrix
            GLHelper::pushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_ACCESS);
            // set color
            GLHelper::setColor(accessColor);
            // translate to geometry position
            glTranslated(myAdditionalGeometry.getShape().front().x(), myAdditionalGeometry.getShape().front().y(), 0);
            // draw circle
            GLHelper::drawFilledCircleDetailled(d, radius);
            // pop layer matrix
            GLHelper::popMatrix();
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), myAdditionalGeometry.getShape().front(), accessExaggeration, 0.3);
            // draw dotted contour
            myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
        }
        // calculate contour
        myAdditionalContour.calculateContourCircleShape(s, d, this, myAdditionalGeometry.getShape().front(), 1, getType(), accessExaggeration);
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
            if (myPositionOverLane == INVALID_DOUBLE) {
                return mySpecialPosition;
            } else {
                return toString(myPositionOverLane);
            }
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


const Parameterised::Map&
GNEAccess::getACParametersMap() const {
    return getParametersMap();
}


void
GNEAccess::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_FRIENDLY_POS:
        case GNE_ATTR_PARENT:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
        case GNE_ATTR_SHIFTLANEINDEX:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
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
            if (value.empty() || value == "random" || value == "doors" || value == "carriage") {
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
        case GNE_ATTR_PARENT:
            return (myNet->getAttributeCarriers()->retrieveAdditionals(NamespaceIDs::busStops, value, false) != nullptr);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
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
            if (value.empty()) {
                myPositionOverLane = 0;
            } else if (value == "random" || value == "doors" || value == "carriage") {
                myPositionOverLane = INVALID_DOUBLE;
                mySpecialPosition = value;
            } else {
                myPositionOverLane = parse<double>(value);
            }
            break;
        case SUMO_ATTR_LENGTH:
            myLength = parse<double>(value);
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        case GNE_ATTR_PARENT:
            if (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr) {
                replaceAdditionalParent(SUMO_TAG_BUS_STOP, value, 0);
            } else {
                replaceAdditionalParent(SUMO_TAG_TRAIN_STOP, value, 0);
            }
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
    undoList->begin(this, "position of " + getTagStr());
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
