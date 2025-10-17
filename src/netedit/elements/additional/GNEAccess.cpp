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

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/moving/GNEMoveElementLaneSingle.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEAccess.h"
#include "GNEAdditionalHandler.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAccess::GNEAccess(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_ACCESS, ""),
    myMoveElementLaneSingle(new GNEMoveElementLaneSingle(this, SUMO_ATTR_POSITION, myPosOverLane, myFriendlyPos,
                            GNEMoveElementLaneSingle::PositionType::SINGLE)) {
}


GNEAccess::GNEAccess(GNEAdditional* busStop, GNELane* lane, const double pos, const std::string& specialPos,
                     const bool friendlyPos, const double length, const Parameterised::Map& parameters) :
    GNEAdditional(busStop, SUMO_TAG_ACCESS, ""),
    Parameterised(parameters),
    myPosOverLane(pos),
    myFriendlyPos(friendlyPos),
    myMoveElementLaneSingle(new GNEMoveElementLaneSingle(this, SUMO_ATTR_POSITION, myPosOverLane, myFriendlyPos,
                            GNEMoveElementLaneSingle::PositionType::SINGLE)),
    mySpecialPosition(specialPos),
    myLength(length) {
    // set parents
    setParent<GNELane*>(lane);
    setParent<GNEAdditional*>(busStop);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEAccess::~GNEAccess() {
    delete myMoveElementLaneSingle;
}


GNEMoveElement*
GNEAccess::getMoveElement() const {
    return myMoveElementLaneSingle;
}


Parameterised*
GNEAccess::getParameters() {
    return this;
}


const Parameterised*
GNEAccess::getParameters() const {
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
    myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), fixedPositionOverLane * getParentLanes().front()->getLengthGeometryFactor(), myMoveElementLaneSingle->myMovingLateralOffset);
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
    // write common additional attributes
    writeAdditionalAttributes(device);
    // write move attributes
    myMoveElementLaneSingle->writeMoveAttributes(device);
    // write specific attributes
    if (myLength != -1) {
        device.writeAttr(SUMO_ATTR_LENGTH, myLength);
    }
    device.closeTag();
}


bool
GNEAccess::isAdditionalValid() const {
    // only movement problems
    return myMoveElementLaneSingle->isMoveElementValid();
}


std::string
GNEAccess::getAdditionalProblem() const {
    // only movement problems
    return myMoveElementLaneSingle->getMovingProblem();
}


void
GNEAccess::fixAdditionalProblem() {
    // only movement problems
    myMoveElementLaneSingle->fixMovingProblem();
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
        case SUMO_ATTR_POSITION:
            if (myPosOverLane == INVALID_DOUBLE) {
                return mySpecialPosition;
            } else {
                return myMoveElementLaneSingle->getMovingAttribute(key);
            }
        case SUMO_ATTR_LENGTH:
            if (myLength == -1) {
                return "";
            } else {
                return toString(myLength);
            }
        case GNE_ATTR_PARENT:
            if (isTemplate()) {
                return "";
            } else {
                return getParentAdditionals().at(0)->getID();
            }
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            return myMoveElementLaneSingle->getMovingAttribute(key);
    }
}


double
GNEAccess::getAttributeDouble(SumoXMLAttr key) const {
    return myMoveElementLaneSingle->getMovingAttributeDouble(key);
}


Position
GNEAccess::getAttributePosition(SumoXMLAttr key) const {
    return myMoveElementLaneSingle->getMovingAttributePosition(key);
}


PositionVector
GNEAccess::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


void
GNEAccess::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_LENGTH:
        case GNE_ATTR_PARENT:
        case GNE_ATTR_SHIFTLANEINDEX:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            myMoveElementLaneSingle->setMovingAttribute(key, value, undoList);
            break;
    }
}


bool
GNEAccess::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_LANE: {
            // check lane
            if (!myMoveElementLaneSingle->isMovingAttributeValid(key, value)) {
                return false;
            } else {
                // check if exist another access for the same parent in the given edge
                const auto lane = myNet->getAttributeCarriers()->retrieveLane(value);
                return GNEAdditionalHandler::accessExists(getParentAdditionals().at(0), lane->getParentEdge());
            }
        }
        case SUMO_ATTR_POSITION:
            if (value.empty() || (value == "random") || (value == "doors") || (value == "carriage")) {
                return true;
            } else {
                return myMoveElementLaneSingle->isMovingAttributeValid(key, value);
            }
        case SUMO_ATTR_LENGTH:
            if (canParse<double>(value)) {
                const double valueDouble = parse<double>(value);
                return (valueDouble == -1) || (valueDouble >= 0);
            } else {
                return false;
            }
        case GNE_ATTR_PARENT:
            return (myNet->getAttributeCarriers()->retrieveAdditionals(NamespaceIDs::busStops, value, false) != nullptr);
        default:
            return myMoveElementLaneSingle->isMovingAttributeValid(key, value);
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
                myMoveElementLaneSingle->setMovingAttribute(key, value);
            }
            break;
        case SUMO_ATTR_LENGTH:
            if (value.empty()) {
                myLength = myTagProperty->getDefaultDoubleValue(key);
            } else {
                myLength = parse<double>(value);
            }
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
            myMoveElementLaneSingle->setMovingAttribute(key, value);
            break;
    }
}

/****************************************************************************/
