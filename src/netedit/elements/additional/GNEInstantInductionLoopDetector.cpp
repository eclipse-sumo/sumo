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
/// @file    GNEInstantInductionLoopDetector.cpp
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

#include "GNEInstantInductionLoopDetector.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEInstantInductionLoopDetector::GNEInstantInductionLoopDetector(GNENet* net) :
    GNEDetector(net, SUMO_TAG_INSTANT_INDUCTION_LOOP),
    myMoveElementLaneSingle(new GNEMoveElementLaneSingle(this, SUMO_ATTR_POSITION, myPosOverLane, myFriendlyPos,
                            GNEMoveElementLaneSingle::PositionType::SINGLE)) {
}


GNEInstantInductionLoopDetector::GNEInstantInductionLoopDetector(const std::string& id, GNENet* net, const std::string& filename, GNELane* lane,
        const double pos, const std::string& outputFilename, const std::vector<std::string>& vehicleTypes, const std::vector<std::string>& nextEdges,
        const std::string& detectPersons, const std::string& name, const bool friendlyPos, const Parameterised::Map& parameters) :
    GNEDetector(id, net, filename, SUMO_TAG_INSTANT_INDUCTION_LOOP, 0, outputFilename,
                vehicleTypes, nextEdges, detectPersons, name, parameters),
    myPosOverLane(pos),
    myFriendlyPos(friendlyPos),
    myMoveElementLaneSingle(new GNEMoveElementLaneSingle(this, SUMO_ATTR_POSITION, myPosOverLane, myFriendlyPos,
                            GNEMoveElementLaneSingle::PositionType::SINGLE)) {
    // set parents
    setParent<GNELane*>(lane);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEInstantInductionLoopDetector::~GNEInstantInductionLoopDetector() {
    delete myMoveElementLaneSingle;
}


GNEMoveElement*
GNEInstantInductionLoopDetector::getMoveElement() const {
    return myMoveElementLaneSingle;
}


Parameterised*
GNEInstantInductionLoopDetector::getParameters() {
    return this;
}


void
GNEInstantInductionLoopDetector::writeAdditional(OutputDevice& device) const {
    device.openTag(getTagProperty()->getTag());
    // write common additional attributes
    writeAdditionalAttributes(device);
    // write move attributes
    myMoveElementLaneSingle->writeMoveAttributes(device);
    // write common detector parameters
    writeDetectorValues(device);
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


bool
GNEInstantInductionLoopDetector::isAdditionalValid() const {
    // only movement problems
    return myMoveElementLaneSingle->isMoveElementValid();
}


std::string
GNEInstantInductionLoopDetector::getAdditionalProblem() const {
    // only movement problems
    return myMoveElementLaneSingle->getMovingProblem();
}


void
GNEInstantInductionLoopDetector::fixAdditionalProblem() {
    // only movement problems
    myMoveElementLaneSingle->fixMovingProblem();
}


void
GNEInstantInductionLoopDetector::updateGeometry() {
    // update geometry
    myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), myMoveElementLaneSingle->getFixedPositionOverLane(true), myMoveElementLaneSingle->myMovingLateralOffset);
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
            drawInLayer(GLO_E1DETECTOR_INSTANT);
            // draw E1Instant shape
            drawE1Shape(d, E1InstantExaggeration, mainColor, secondColor);
            // draw E1 Logo
            drawE1DetectorLogo(s, d, E1InstantExaggeration, "E1", textColor);
            // pop layer matrix
            GLHelper::popMatrix();
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), myAdditionalGeometry.getShape().getCentroid(),
                    E1InstantExaggeration);
            // Draw additional ID
            drawAdditionalID(s);
            // draw additional name
            drawAdditionalName(s);
            // draw dotted contour
            myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // draw dotted contour
        myAdditionalContour.calculateContourRectangleShape(s, d, this, myAdditionalGeometry.getShape().front(), 2, 1, getType(), 0, 0,
                myAdditionalGeometry.getShapeRotations().front(), E1InstantExaggeration, getParentLanes().front()->getParentEdge());
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


Position
GNEInstantInductionLoopDetector::getAttributePosition(SumoXMLAttr key) const {
    return getDetectorAttributePosition(key);
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
    switch (key) {
        case SUMO_ATTR_LANE:
            replaceAdditionalParentLanes(value);
            break;
        default:
            setDetectorAttribute(key, value);
            break;
    }
}

/****************************************************************************/
