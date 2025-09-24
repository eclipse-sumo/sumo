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
/// @file    GNEAccess.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2018
///
//
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
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
    GNEAdditional("", net, "", SUMO_TAG_ACCESS, ""),
    GNEMoveElementLaneSingle(this) {
}


GNEAccess::GNEAccess(GNEAdditional* busStop, GNELane* lane, const double pos, const std::string& specialPos,
                     const bool friendlyPos, const double length, const Parameterised::Map& parameters) :
    GNEAdditional(busStop, SUMO_TAG_ACCESS, ""),
    GNEMoveElementLaneSingle(this, lane, pos, friendlyPos),
    Parameterised(parameters),
    mySpecialPosition(specialPos),
    myLength(length) {
    // set parents
    setParent<GNEAdditional*>(busStop);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEAccess::~GNEAccess() {
}

/// @brief get GNEMoveElement associated with this AttributeCarrier
inline GNEMoveElement* GNEAccess::getMoveElement() {
    return this;
}


void
GNEAccess::updateGeometry() {
    // set start position
    double fixedPositionOverLane;
    if (myPosOverLane < 0) {
        fixedPositionOverLane = 0;
    } else if (myPosOverLane > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
        fixedPositionOverLane = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    } else {
        fixedPositionOverLane = myPosOverLane;
    }
    // update geometry
    myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), fixedPositionOverLane * getParentLanes().front()->getLengthGeometryFactor(), myMovingLateralOffset);
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
    if (splitPosition < myPosOverLane) {
        // change lane
        setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
        // now adjust start position
        setAttribute(SUMO_ATTR_POSITION, toString(myPosOverLane - splitPosition), undoList);
    }
}


bool
GNEAccess::isAccessPositionFixed() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPos) {
        return true;
    } else {
        if (myPosOverLane != INVALID_DOUBLE) {
            return (myPosOverLane >= 0) && (myPosOverLane <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
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
    if (myFriendlyPos) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, myFriendlyPos);
    }
    device.closeTag();
}


bool
GNEAccess::isAdditionalValid() const {
    // with friendly position enabled position is "always fixed"
    if (myFriendlyPos) {
        return true;
    } else if (myPosOverLane == INVALID_DOUBLE) {
        return true;
    } else {
        return fabs(myPosOverLane) <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    }
}


std::string GNEAccess::getAdditionalProblem() const {
    // obtain final length
    const double len = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    // check if detector has a problem
    if (GNEAdditionalHandler::checkLanePosition(myPosOverLane, 0, len, myFriendlyPos)) {
        return "";
    } else {
        // declare variable for error position
        std::string errorPosition;
        // check positions over lane
        if (myPosOverLane < 0) {
            errorPosition = (toString(SUMO_ATTR_POSITION) + " < 0");
        }
        if (myPosOverLane > len) {
            errorPosition = (toString(SUMO_ATTR_POSITION) + TL(" > lanes's length"));
        }
        return errorPosition;
    }
}


void GNEAccess::fixAdditionalProblem() {
    // set fixed position
    setAttribute(SUMO_ATTR_POSITION, toString(getFixedPositionOverLane()), myNet->getViewNet()->getUndoList());
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
        // adjust radius depending of mode and distance to mouse position
        double radius = 0.5;
        if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
                myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(myAdditionalGeometry.getShape().front()) < 1) {
            radius = 1;
        }
        // get detail level
        const auto d = s.getDetailLevel(1);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // get color
            RGBColor accessColor;
            if (drawUsingSelectColor()) {
                accessColor = s.colorSettings.selectedAdditionalColor;
            } else if (!getParentAdditionals().front()->getAttribute(SUMO_ATTR_COLOR).empty()) {
                accessColor = parse<RGBColor>(getParentAdditionals().front()->getAttribute(SUMO_ATTR_COLOR));
            } else if (getParentAdditionals().front()->getTagProperty()->getTag() == SUMO_TAG_CONTAINER_STOP) {
                accessColor = s.colorSettings.containerStopColor;
            } else {
                accessColor = s.colorSettings.busStopColor;
            }
            // draw parent and child lines
            drawParentChildLines(s, accessColor);
            // push layer matrix
            GLHelper::pushMatrix();
            // translate to front
            drawInLayer(GLO_ACCESS);
            // set color
            GLHelper::setColor(accessColor);
            // translate to geometry position
            glTranslated(myAdditionalGeometry.getShape().front().x(), myAdditionalGeometry.getShape().front().y(), 0);
            // draw circle
            GLHelper::drawFilledCircleDetailled(d, radius * accessExaggeration);
            // pop layer matrix
            GLHelper::popMatrix();
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), myAdditionalGeometry.getShape().front(), accessExaggeration, 0.3);
            // draw dotted contour
            myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
        }
        // calculate contour
        myAdditionalContour.calculateContourCircleShape(s, d, this, myAdditionalGeometry.getShape().front(), radius, getType(),
                accessExaggeration, getParentLanes().front()->getParentEdge());
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
            if (myPosOverLane == INVALID_DOUBLE) {
                return mySpecialPosition;
            } else {
                return toString(myPosOverLane);
            }
        case SUMO_ATTR_LENGTH:
            if (myLength == -1) {
                return "";
            } else {
                return toString(myLength);
            }
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPos);
        case GNE_ATTR_PARENT:
            if (isTemplate()) {
                return "";
            } else {
                return getParentAdditionals().at(0)->getID();
            }
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            return getCommonAttribute(this, key);
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
        case GNE_ATTR_SHIFTLANEINDEX:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
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
        default:
            return isCommonValid(key, value);
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
                myPosOverLane = 0;
            } else if (value == "random" || value == "doors" || value == "carriage") {
                myPosOverLane = INVALID_DOUBLE;
                mySpecialPosition = value;
            } else {
                myPosOverLane = parse<double>(value);
            }
            break;
        case SUMO_ATTR_LENGTH:
            if (value.empty()) {
                myLength = myTagProperty->getDefaultDoubleValue(key);
            } else {
                myLength = parse<double>(value);
            }
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPos = parse<bool>(value);
            break;
        case GNE_ATTR_PARENT:
            if (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr) {
                replaceAdditionalParent(SUMO_TAG_BUS_STOP, value, 0);
            } else if (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, value, false) != nullptr) {
                replaceAdditionalParent(SUMO_TAG_TRAIN_STOP, value, 0);
            } else {
                replaceAdditionalParent(SUMO_TAG_CONTAINER_STOP, value, 0);
            }
            break;
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        default:
            setCommonAttribute(this, key, value);
            break;
    }
}

/****************************************************************************/
