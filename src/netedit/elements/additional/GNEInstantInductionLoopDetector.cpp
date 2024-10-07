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
/// @file    GNEInstantInductionLoopDetector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2018
///
//
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>

#include "GNEInstantInductionLoopDetector.h"
#include "GNEAdditionalHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEInstantInductionLoopDetector::GNEInstantInductionLoopDetector(GNENet* net) :
    GNEDetector("", net, GLO_E1DETECTOR_INSTANT, SUMO_TAG_INSTANT_INDUCTION_LOOP, GUIIconSubSys::getIcon(GUIIcon::E1INSTANT),
                0, 0, {}, "", {}, {}, "", "", false, Parameterised::Map()) {
    // reset default values
    resetDefaultValues();
}


GNEInstantInductionLoopDetector::GNEInstantInductionLoopDetector(const std::string& id, GNELane* lane, GNENet* net,
        const double pos, const std::string& filename, const std::vector<std::string>& vehicleTypes,
        const std::vector<std::string>& nextEdges, const std::string& detectPersons, const std::string& name,
        const bool friendlyPos, const Parameterised::Map& parameters) :
    GNEDetector(id, net, GLO_E1DETECTOR_INSTANT, SUMO_TAG_INSTANT_INDUCTION_LOOP, GUIIconSubSys::getIcon(GUIIcon::E1INSTANT),
                pos, 0, {
    lane
}, filename, vehicleTypes, nextEdges, detectPersons, name, friendlyPos, parameters) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEInstantInductionLoopDetector::~GNEInstantInductionLoopDetector() {
}


void
GNEInstantInductionLoopDetector::writeAdditional(OutputDevice& device) const {
    device.openTag(getTagProperty().getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANE, getParentLanes().front()->getID());
    device.writeAttr(SUMO_ATTR_POSITION, myPositionOverLane);
    // write common detector parameters
    writeDetectorValues(device);
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


bool
GNEInstantInductionLoopDetector::isAdditionalValid() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else {
        return fabs(myPositionOverLane) <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    }
}


std::string
GNEInstantInductionLoopDetector::getAdditionalProblem() const {
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


void
GNEInstantInductionLoopDetector::fixAdditionalProblem() {
    // declare new position
    double newPositionOverLane = myPositionOverLane;
    // fix pos and length checkAndFixDetectorPosition
    double length = 0;
    GNEAdditionalHandler::fixLanePosition(newPositionOverLane, length, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
    // set new position
    setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
}


void
GNEInstantInductionLoopDetector::updateGeometry() {
    // update geometry
    myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), getGeometryPositionOverLane(), myMoveElementLateralOffset);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


void
GNEInstantInductionLoopDetector::drawGL(const GUIVisualizationSettings& s) const {
    // check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals() &&
            !myNet->getViewNet()->selectingDetectorsTLSMode()) {
        // Obtain exaggeration of the draw
        const double E1InstantExaggeration = getExaggeration(s);
        // get detail level
        const auto d = s.getDetailLevel(E1InstantExaggeration);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // declare colors
            RGBColor mainColor, secondColor, textColor;
            // set color
            if (drawUsingSelectColor()) {
                mainColor = s.colorSettings.selectedAdditionalColor;
                secondColor = mainColor.changedBrightness(-32);
                textColor = mainColor.changedBrightness(32);
            } else {
                mainColor = s.detectorSettings.E1InstantColor;
                secondColor = RGBColor::WHITE;
                textColor = RGBColor::BLACK;
            }
            // draw parent and child lines
            drawParentChildLines(s, s.additionalSettings.connectionColor);
            // push layer matrix
            GLHelper::pushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_E1DETECTOR_INSTANT);
            // draw E1Instant shape
            drawE1Shape(d, E1InstantExaggeration, mainColor, secondColor);
            // draw E1 Logo
            drawE1DetectorLogo(s, d, E1InstantExaggeration, "E1", textColor);
            // pop layer matrix
            GLHelper::popMatrix();
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), myAdditionalGeometry.getShape().getCentroid(), E1InstantExaggeration);
            // Draw additional ID
            drawAdditionalID(s);
            // draw additional name
            drawAdditionalName(s);
            // draw dotted contour
            myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // draw dotted contour
        myAdditionalContour.calculateContourRectangleShape(s, d, this, myAdditionalGeometry.getShape().front(), 2, 1, 0, 0,
                myAdditionalGeometry.getShapeRotations().front(), E1InstantExaggeration);
    }
}


std::string
GNEInstantInductionLoopDetector::getAttribute(SumoXMLAttr key) const {
    return getDetectorAttribute(key);
}


double
GNEInstantInductionLoopDetector::getAttributeDouble(SumoXMLAttr key) const {
    return getDetectorAttributeDouble(key);
}


void
GNEInstantInductionLoopDetector::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    setDetectorAttribute(key, value, undoList);
}


bool
GNEInstantInductionLoopDetector::isValid(SumoXMLAttr key, const std::string& value) {
    return isDetectorValid(key, value);
}

// ===========================================================================
// private
// ===========================================================================

void
GNEInstantInductionLoopDetector::setAttribute(SumoXMLAttr key, const std::string& value) {
    setDetectorAttribute(key, value);
}


void
GNEInstantInductionLoopDetector::setMoveShape(const GNEMoveResult& moveResult) {
    // change position
    myPositionOverLane = moveResult.newFirstPos;
    // set lateral offset
    myMoveElementLateralOffset = moveResult.firstLaneOffset;
    // update geometry
    updateGeometry();
}


void
GNEInstantInductionLoopDetector::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // reset lateral offset
    myMoveElementLateralOffset = 0;
    // begin change attribute
    undoList->begin(this, "position of " + getTagStr());
    // set startPosition
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
