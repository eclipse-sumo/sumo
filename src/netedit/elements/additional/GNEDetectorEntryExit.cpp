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
/// @file    GNEDetectorEntryExit.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEDetectorEntryExit.h"
#include "GNEAdditionalHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorEntryExit::GNEDetectorEntryExit(SumoXMLTag entryExitTag, GNENet* net) :
    GNEDetector("", net, GLO_DET_ENTRY, entryExitTag, 0, 0, {}, "", {}, "", false, std::map<std::string, std::string>()) {
    // reset default values
    resetDefaultValues();
}


GNEDetectorEntryExit::GNEDetectorEntryExit(SumoXMLTag entryExitTag, GNENet* net, GNEAdditional* parent, GNELane* lane, const double pos,
        const bool friendlyPos, const std::map<std::string, std::string>& parameters) :
    GNEDetector(parent, net, GLO_DET_ENTRY, entryExitTag, pos, 0, {lane}, "", "", friendlyPos, parameters) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEDetectorEntryExit::~GNEDetectorEntryExit() {}


bool
GNEDetectorEntryExit::isAdditionalValid() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else {
        return fabs(myPositionOverLane) <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    }
}


std::string
GNEDetectorEntryExit::getAdditionalProblem() const {
    // obtain final lenght
    const double len = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    // check if detector has a problem
    if (GNEAdditionalHandler::checkSinglePositionOverLane(myPositionOverLane, len, myFriendlyPosition)) {
        return "";
    } else {
        // declare variable for error position
        std::string errorPosition;
        // check positions over lane
        if (myPositionOverLane < 0) {
            errorPosition = (toString(SUMO_ATTR_POSITION) + " < 0");
        }
        if (myPositionOverLane > len) {
            errorPosition = (toString(SUMO_ATTR_POSITION) + " > lanes's length");
        }
        return errorPosition;
    }
}


void
GNEDetectorEntryExit::fixAdditionalProblem() {
    // declare new position
    double newPositionOverLane = myPositionOverLane;
    // fix pos and length checkAndFixDetectorPosition
    GNEAdditionalHandler::fixSinglePositionOverLane(newPositionOverLane, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
    // set new position
    setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
}


void
GNEDetectorEntryExit::updateGeometry() {
    // update geometry
    myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), getGeometryPositionOverLane(), myMoveElementLateralOffset);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


void
GNEDetectorEntryExit::drawGL(const GUIVisualizationSettings& s) const {
    // Set initial values
    const double entryExitExaggeration = getExaggeration(s);
    // first check if additional has to be drawn
    if (s.drawAdditionals(entryExitExaggeration) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // draw parent and child lines
        drawParentChildLines(s, s.additionalSettings.connectionColor);
        // Start drawing adding gl identificator
        GLHelper::pushName(getGlID());
        // Push layer matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_DET_ENTRY);
        // Set color
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
        } else if (myTagProperty.getTag() == SUMO_TAG_DET_ENTRY) {
            GLHelper::setColor(s.detectorSettings.E3EntryColor);
        } else if (myTagProperty.getTag() == SUMO_TAG_DET_EXIT) {
            GLHelper::setColor(s.detectorSettings.E3ExitColor);
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // Push polygon matrix
        GLHelper::pushMatrix();
        glTranslated(myAdditionalGeometry.getShape().front().x(), myAdditionalGeometry.getShape().front().y(), 0);
        // rotate over lane
        GUIGeometry::rotateOverLane(myAdditionalGeometry.getShapeRotations().front() + 90);
        // scale
        glScaled(entryExitExaggeration, entryExitExaggeration, 1);
        // draw details if isn't being drawn for selecting
        if (!s.drawForRectangleSelection) {
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
        // Check if the distance is enought to draw details
        if (!s.drawForRectangleSelection && s.drawDetail(s.detailSettings.detectorDetails, entryExitExaggeration)) {
            // Push matrix
            GLHelper::pushMatrix();
            // Traslate to center of detector
            glTranslated(myAdditionalGeometry.getShape().front().x(), myAdditionalGeometry.getShape().front().y(), getType() + 0.1);
            // rotate over lane
            GUIGeometry::rotateOverLane(myAdditionalGeometry.getShapeRotations().front());
            //move to logo position
            glTranslated(1.9, 0, 0);
            // scale
            glScaled(entryExitExaggeration, entryExitExaggeration, 1);
            // draw Entry or Exit logo if isn't being drawn for selecting
            if (s.drawForRectangleSelection || s.drawForPositionSelection) {
                GLHelper::setColor(s.detectorSettings.E3EntryColor);
                GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
            } else if (drawUsingSelectColor()) {
                GLHelper::drawText("E3", Position(), .1, 2.8, s.colorSettings.selectedAdditionalColor, 180);
            } else if (myTagProperty.getTag() == SUMO_TAG_DET_ENTRY) {
                GLHelper::drawText("E3", Position(), .1, 2.8, s.detectorSettings.E3EntryColor, 180);
            } else if (myTagProperty.getTag() == SUMO_TAG_DET_EXIT) {
                GLHelper::drawText("E3", Position(), .1, 2.8, s.detectorSettings.E3ExitColor, 180);
            }
            //move to logo position
            glTranslated(1.7, 0, 0);
            // rotate 90 degrees lane
            glRotated(90, 0, 0, 1);
            // draw Entry or Exit text if isn't being drawn for selecting
            if (s.drawForRectangleSelection || s.drawForPositionSelection) {
                GLHelper::setColor(s.detectorSettings.E3EntryColor);
                GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
            } else if (drawUsingSelectColor()) {
                if (myTagProperty.getTag() == SUMO_TAG_DET_ENTRY) {
                    GLHelper::drawText("Entry", Position(), .1, 1, s.colorSettings.selectedAdditionalColor, 180);
                } else if (myTagProperty.getTag() == SUMO_TAG_DET_EXIT) {
                    GLHelper::drawText("Exit", Position(), .1, 1, s.colorSettings.selectedAdditionalColor, 180);
                }
            } else {
                if (myTagProperty.getTag() == SUMO_TAG_DET_ENTRY) {
                    GLHelper::drawText("Entry", Position(), .1, 1, s.detectorSettings.E3EntryColor, 180);
                } else if (myTagProperty.getTag() == SUMO_TAG_DET_EXIT) {
                    GLHelper::drawText("Exit", Position(), .1, 1, s.detectorSettings.E3ExitColor, 180);
                }
            }
            // pop matrix
            GLHelper::popMatrix();
        }
        // Pop layer matrix
        GLHelper::popMatrix();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), myAdditionalGeometry.getShape().getCentroid(), entryExitExaggeration);
        // check if dotted contour has to be drawn
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            GUIDottedGeometry::drawDottedSquaredShape(GUIDottedGeometry::DottedContourType::INSPECT, s,
                    myAdditionalGeometry.getShape().front(), 2.7, 1.6, 2, 0,
                    myAdditionalGeometry.getShapeRotations().front(), entryExitExaggeration);
        }
        if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
            GUIDottedGeometry::drawDottedSquaredShape(GUIDottedGeometry::DottedContourType::FRONT, s,
                    myAdditionalGeometry.getShape().front(), 2.7, 1.6, 2, 0,
                    myAdditionalGeometry.getShapeRotations().front(), entryExitExaggeration);
        }
        // pop gl identificator
        GLHelper::popName();
        // draw additional name
        drawAdditionalName(s);
    }
}


std::string
GNEDetectorEntryExit::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getParentAdditionals().front()->getID();
        case SUMO_ATTR_LANE:
            return getParentLanes().front()->getID();
        case SUMO_ATTR_POSITION:
            return toString(myPositionOverLane);
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
GNEDetectorEntryExit::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_POSITION:
            return myPositionOverLane;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDetectorEntryExit::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FRIENDLY_POS:
        case GNE_ATTR_PARENT:
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
GNEDetectorEntryExit::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_LANE:
            return (myNet->getAttributeCarriers()->retrieveLane(value, false) != nullptr);
        case SUMO_ATTR_POSITION:
            return canParse<double>(value) && fabs(parse<double>(value)) < getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case GNE_ATTR_PARENT:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_E3DETECTOR, value, false) != nullptr);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorEntryExit::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}


void
GNEDetectorEntryExit::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_LANE:
            replaceAdditionalParentLanes(value);
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = parse<double>(value);
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        case GNE_ATTR_PARENT:
            replaceAdditionalParent(SUMO_TAG_E3DETECTOR, value, 0);
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
GNEDetectorEntryExit::setMoveShape(const GNEMoveResult& moveResult) {
    // change position
    myPositionOverLane = moveResult.newFirstPos;
    // set lateral offset
    myMoveElementLateralOffset = moveResult.firstLaneOffset;
    // update geometry
    updateGeometry();
}


void
GNEDetectorEntryExit::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // reset lateral offset
    myMoveElementLateralOffset = 0;
    // begin change attribute
    undoList->begin(myTagProperty.getGUIIcon(), "position of " + getTagStr());
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
