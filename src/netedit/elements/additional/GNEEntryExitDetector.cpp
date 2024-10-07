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
/// @file    GNEEntryExitDetector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
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
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>

#include "GNEEntryExitDetector.h"
#include "GNEAdditionalHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEEntryExitDetector::GNEEntryExitDetector(SumoXMLTag entryExitTag, GNENet* net) :
    GNEDetector("", net, GLO_DET_ENTRY, entryExitTag, GUIIconSubSys::getIcon(GUIIcon::E3ENTRY), 0, 0, {}, "", {}, {}, "", "", false, Parameterised::Map()) {
    // reset default values
    resetDefaultValues();
}


GNEEntryExitDetector::GNEEntryExitDetector(SumoXMLTag entryExitTag, GNENet* net, GNEAdditional* parent, GNELane* lane, const double pos,
        const bool friendlyPos, const Parameterised::Map& parameters) :
    GNEDetector(parent, net, GLO_DET_ENTRY, entryExitTag, GUIIconSubSys::getIcon(GUIIcon::E3ENTRY), pos, 0, {
    lane
}, "", "", friendlyPos, parameters) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEEntryExitDetector::~GNEEntryExitDetector() {}


void
GNEEntryExitDetector::writeAdditional(OutputDevice& device) const {
    device.openTag(getTagProperty().getTag());
    device.writeAttr(SUMO_ATTR_LANE, getParentLanes().front()->getID());
    device.writeAttr(SUMO_ATTR_POSITION, myPositionOverLane);
    // write common detector parameters
    writeDetectorValues(device);
    // write parameters
    writeParams(device);
    device.closeTag();
}


bool
GNEEntryExitDetector::isAdditionalValid() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else {
        return fabs(myPositionOverLane) <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    }
}


std::string
GNEEntryExitDetector::getAdditionalProblem() const {
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
GNEEntryExitDetector::fixAdditionalProblem() {
    // declare new position
    double newPositionOverLane = myPositionOverLane;
    // fix pos and length checkAndFixDetectorPosition
    double length = 0;
    GNEAdditionalHandler::fixLanePosition(newPositionOverLane, length, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
    // set new position
    setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
}


void
GNEEntryExitDetector::updateGeometry() {
    // update geometry
    myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), getGeometryPositionOverLane(), myMoveElementLateralOffset);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


void
GNEEntryExitDetector::drawGL(const GUIVisualizationSettings& s) const {
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals() &&
            !myNet->getViewNet()->selectingDetectorsTLSMode()) {
        // Set initial values
        const double entryExitExaggeration = getExaggeration(s);
        // get detail level
        const auto d = s.getDetailLevel(entryExitExaggeration);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // draw parent and child lines
            drawParentChildLines(s, s.additionalSettings.connectionColor);
            // Push layer matrix
            GLHelper::pushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_DET_ENTRY);
            // Set color
            RGBColor color;
            if (drawUsingSelectColor()) {
                color = s.colorSettings.selectedAdditionalColor;
            } else if (myTagProperty.getTag() == SUMO_TAG_DET_ENTRY) {
                color = s.detectorSettings.E3EntryColor;
            } else if (myTagProperty.getTag() == SUMO_TAG_DET_EXIT) {
                color = s.detectorSettings.E3ExitColor;
            }
            // draw parts
            drawBody(d, color, entryExitExaggeration);
            drawEntryLogo(d, color, entryExitExaggeration);
            drawE3Logo(d, color, entryExitExaggeration);
            // pop layer matrix
            GLHelper::popMatrix();
            // draw additional name
            drawAdditionalName(s);
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), myAdditionalGeometry.getShape().getCentroid(), entryExitExaggeration);
            // draw dotted contour
            myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // calculate contour
        myAdditionalContour.calculateContourRectangleShape(s, d, this, myAdditionalGeometry.getShape().front(), 2.7, 1.6, 2, 0,
                myAdditionalGeometry.getShapeRotations().front(), entryExitExaggeration);
    }
}


std::string
GNEEntryExitDetector::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        default:
            return getDetectorAttribute(key);
    }
}


double
GNEEntryExitDetector::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_POSITION:
            return myPositionOverLane;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEEntryExitDetector::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case GNE_ATTR_PARENT:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setDetectorAttribute(key, value, undoList);
            break;
    }
}


bool
GNEEntryExitDetector::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case GNE_ATTR_PARENT:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_ENTRY_EXIT_DETECTOR, value, false) != nullptr);
        default:
            return isDetectorValid(key, value);
    }
}


void
GNEEntryExitDetector::drawBody(const GUIVisualizationSettings::Detail d,
                               const RGBColor& color, const double exaggeration) const {
    // check detail level
    if (d <= GUIVisualizationSettings::Detail::Additionals) {
        // Push polygon matrix
        GLHelper::pushMatrix();
        // set color
        GLHelper::setColor(color);
        // set polygon mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // move to position
        glTranslated(myAdditionalGeometry.getShape().front().x(), myAdditionalGeometry.getShape().front().y(), 0);
        // rotate over lane
        GUIGeometry::rotateOverLane(myAdditionalGeometry.getShapeRotations().front() + 90);
        // scale
        glScaled(exaggeration, exaggeration, 1);
        // check detail level
        if (d <= GUIVisualizationSettings::Detail::AdditionalDetails) {
            // Draw polygon
            glBegin(GL_LINES);
            glVertex2d(1.7, 0);
            glVertex2d(-1.7, 0);
            glEnd();
            glBegin(GL_QUADS);
            glVertex2d(-1.7, .5);
            glVertex2d(-1.7, -.5);
            glVertex2d(1.7, -.5);
            glVertex2d(1.7, .5);
            glEnd();
            // first Arrow
            glTranslated(1.5, 0, 0);
            GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
            GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (double) 1, (double) .25);
            // second Arrow
            glTranslated(-3, 0, 0);
            GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
            GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (double) 1, (double) .25);
        } else {
            // Draw square in drawy for selecting mode
            glBegin(GL_QUADS);
            glVertex2d(-1.7, 4.3);
            glVertex2d(-1.7, -.5);
            glVertex2d(1.7, -.5);
            glVertex2d(1.7, 4.3);
            glEnd();
        }
        // Pop polygon matrix
        GLHelper::popMatrix();
    }
}


void
GNEEntryExitDetector::drawEntryLogo(const GUIVisualizationSettings::Detail d,
                                    const RGBColor& color, const double exaggeration) const {
    // check detail level
    if (d <= GUIVisualizationSettings::Detail::AdditionalDetails) {
        // Push matrix
        GLHelper::pushMatrix();
        // set color
        GLHelper::setColor(color);
        // Traslate to center of detector
        glTranslated(myAdditionalGeometry.getShape().front().x(), myAdditionalGeometry.getShape().front().y(), getType() + 0.1);
        // rotate over lane
        GUIGeometry::rotateOverLane(myAdditionalGeometry.getShapeRotations().front());
        //move to logo position
        glTranslated(1.9, 0, 0);
        // scale
        glScaled(exaggeration, exaggeration, 1);
        //move to logo position
        glTranslated(1.7, 0, 0);
        // rotate 90 degrees lane
        glRotated(90, 0, 0, 1);
        // draw Entry or Exit text if isn't being drawn for selecting
        if (d <= GUIVisualizationSettings::Detail::Text) {
            if (myTagProperty.getTag() == SUMO_TAG_DET_ENTRY) {
                GLHelper::drawText("Entry", Position(), .1, 1, color, 180);
            } else if (myTagProperty.getTag() == SUMO_TAG_DET_EXIT) {
                GLHelper::drawText("Exit", Position(), .1, 1, color, 180);
            }
        } else {
            GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
        }
        // pop matrix
        GLHelper::popMatrix();
    }
}


void
GNEEntryExitDetector::drawE3Logo(const GUIVisualizationSettings::Detail d,
                                 const RGBColor& color, const double exaggeration) const {
    // check detail level
    if (d <= GUIVisualizationSettings::Detail::Text) {
        // Push matrix
        GLHelper::pushMatrix();
        // set color
        GLHelper::setColor(color);
        // Traslate to center of detector
        glTranslated(myAdditionalGeometry.getShape().front().x(), myAdditionalGeometry.getShape().front().y(), getType() + 0.1);
        // rotate over lane
        GUIGeometry::rotateOverLane(myAdditionalGeometry.getShapeRotations().front());
        //move to logo position
        glTranslated(1.9, 0, 0);
        // scale
        glScaled(exaggeration, exaggeration, 1);
        // draw E3 logo
        GLHelper::drawText("E3", Position(0, 0), .1, 2.8, color);
        // pop matrix
        GLHelper::popMatrix();
    }
}


void
GNEEntryExitDetector::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case GNE_ATTR_PARENT:
            replaceAdditionalParent(SUMO_TAG_ENTRY_EXIT_DETECTOR, value, 0);
            break;
        default:
            setDetectorAttribute(key, value);
            break;
    }
}


void
GNEEntryExitDetector::setMoveShape(const GNEMoveResult& moveResult) {
    // change position
    myPositionOverLane = moveResult.newFirstPos;
    // set lateral offset
    myMoveElementLateralOffset = moveResult.firstLaneOffset;
    // update geometry
    updateGeometry();
}


void
GNEEntryExitDetector::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
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
