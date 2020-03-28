/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEDetectorE2.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Connection.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEDetectorE2.h"
#include "GNEAdditionalHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE2::GNEDetectorE2(const std::string& id, GNELane* lane, GNEViewNet* viewNet, double pos, double length, SUMOTime freq, const std::string& filename, const std::string& vehicleTypes,
        const std::string& name, SUMOTime timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos, bool blockMovement) :
    GNEDetector(id, viewNet, GLO_E2DETECTOR, SUMO_TAG_E2DETECTOR, pos, freq, filename, vehicleTypes, name, friendlyPos, blockMovement, {lane}),
    myLength(length),
    myEndPositionOverLane(0.),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold),
    myE2valid(true) {
}


GNEDetectorE2::GNEDetectorE2(const std::string& id, std::vector<GNELane*> lanes, GNEViewNet* viewNet, double pos, double endPos, SUMOTime freq, const std::string& filename, const std::string& vehicleTypes,
        const std::string& name, SUMOTime timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos, bool blockMovement) :
    GNEDetector(id, viewNet, GLO_E2DETECTOR, SUMO_TAG_E2DETECTOR_MULTILANE, pos, freq, filename, vehicleTypes, name, friendlyPos, blockMovement, lanes),
    myLength(0),
    myEndPositionOverLane(endPos),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold),
    myE2valid(true) {
}


GNEDetectorE2::~GNEDetectorE2() {
}


bool
GNEDetectorE2::isAdditionalValid() const {
    if (getParentLanes().size() == 1) {
        // with friendly position enabled position are "always fixed"
        if (myFriendlyPosition) {
            return true;
        } else {
            return (myPositionOverLane >= 0) && ((myPositionOverLane + myLength) <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
        }
    } else {
        // first check if there is connection between all consecutive lanes
        if (myE2valid) {
            // with friendly position enabled position are "always fixed"
            if (myFriendlyPosition) {
                return true;
            } else {
                return (myPositionOverLane >= 0) && ((myPositionOverLane) <= getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength() &&
                                                     myEndPositionOverLane >= 0) && ((myEndPositionOverLane) <= getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength());
            }
        } else {
            return false;
        }
    }
}


std::string
GNEDetectorE2::getAdditionalProblem() const {
    // declare variable for error position
    std::string errorFirstLanePosition, separator, errorLastLanePosition;
    if (getParentLanes().size() == 1) {
        // check positions over lane
        if (myPositionOverLane < 0) {
            errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + " < 0");
        }
        if (myPositionOverLane > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
            errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + " > lanes's length");
        }
        if ((myPositionOverLane + myLength) > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
            errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + " + " + toString(SUMO_ATTR_LENGTH) + " > lanes's length");
        }
    } else {
        if (myE2valid) {
            // check positions over first lane
            if (myPositionOverLane < 0) {
                errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + " < 0");
            }
            if (myPositionOverLane > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
                errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + " > lanes's length");
            }
            // check positions over last lane
            if (myEndPositionOverLane < 0) {
                errorLastLanePosition = (toString(SUMO_ATTR_ENDPOS) + " < 0");
            }
            if (myEndPositionOverLane > getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength()) {
                errorLastLanePosition = (toString(SUMO_ATTR_ENDPOS) + " > lanes's length");
            }
        } else {
            errorFirstLanePosition = "lanes aren't consecutives";
        }
    }
    // check separator
    if ((errorFirstLanePosition.size() > 0) && (errorLastLanePosition.size() > 0)) {
        separator = " and ";
    }
    // return error message
    return errorFirstLanePosition + separator + errorLastLanePosition;
}


void
GNEDetectorE2::fixAdditionalProblem() {
    if (getParentLanes().size() == 1) {
        // obtain position and length
        double newPositionOverLane = myPositionOverLane;
        double newLength = myLength;
        // fix pos and length using fixE2DetectorPosition
        GNEAdditionalHandler::fixE2DetectorPosition(newPositionOverLane, newLength, getParentLanes().at(0)->getParentEdge()->getNBEdge()->getFinalLength(), true);
        // set new position and length
        setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myViewNet->getUndoList());
        setAttribute(SUMO_ATTR_LENGTH, toString(myLength), myViewNet->getUndoList());
    } else {
        if (!myE2valid) {
            // build connections between all consecutive lanes
            bool foundConnection = true;
            int i = 0;
            // iterate over all lanes, and stop if myE2valid is false
            while (i < ((int)getParentLanes().size() - 1)) {
                // change foundConnection to false
                foundConnection = false;
                // if a connection betwen "from" lane and "to" lane of connection is found, change myE2valid to true again
                for (auto j : getParentLanes().at(i)->getParentEdge()->getGNEConnections()) {
                    if (j->getLaneFrom() == getParentLanes().at(i) && j->getLaneTo() == getParentLanes().at(i + 1)) {
                        foundConnection = true;
                    }
                }
                // if connection wasn't found
                if (!foundConnection) {
                    // create new connection manually
                    NBEdge::Connection newCon(getParentLanes().at(i)->getIndex(), getParentLanes().at(i + 1)->getParentEdge()->getNBEdge(), getParentLanes().at(i + 1)->getIndex());
                    // allow to undo creation of new lane
                    myViewNet->getUndoList()->add(new GNEChange_Connection(getParentLanes().at(i)->getParentEdge(), newCon, false, true), true);
                }
                // update lane iterator
                i++;
            }
        } else {
            // declare new position
            double newPositionOverLane = myPositionOverLane;
            // fix pos and length  checkAndFixDetectorPosition
            GNEAdditionalHandler::checkAndFixDetectorPosition(newPositionOverLane, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), true);
            // set new position
            setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myViewNet->getUndoList());
            // declare new end position
            double newEndPositionOverLane = myEndPositionOverLane;
            // fix pos and length  checkAndFixDetectorPosition
            GNEAdditionalHandler::checkAndFixDetectorPosition(newEndPositionOverLane, getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength(), true);
            // set new position
            setAttribute(SUMO_ATTR_ENDPOS, toString(newEndPositionOverLane), myViewNet->getUndoList());
        }
    }
}


void
GNEDetectorE2::moveGeometry(const Position& offset) {
    // Calculate new position using old position
    Position newPosition = myMove.originalViewPosition;
    newPosition.add(offset);
    // filtern position using snap to active grid
    newPosition = myViewNet->snapToActiveGrid(newPosition);
    double offsetLane = getParentLanes().front()->getLaneShape().nearest_offset_to_point2D(newPosition, false) - getParentLanes().front()->getLaneShape().nearest_offset_to_point2D(myMove.originalViewPosition, false);
    // move geometry depending of number of lanes
    if (getParentLanes().size() == 1) {
        // calculate new position over lane
        double newPositionOverLane = parse<double>(myMove.firstOriginalLanePosition) + offsetLane;
        // obtain lane length
        double laneLength = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength() * getLane()->getLengthGeometryFactor();
        if (newPositionOverLane < 0) {
            myPositionOverLane = 0;
        } else if (newPositionOverLane + myLength > laneLength) {
            myPositionOverLane = laneLength - myLength;
        } else {
            myPositionOverLane = newPositionOverLane;
        }
    } else {
        // calculate new start and end positions
        double newStartPosition = parse<double>(myMove.firstOriginalLanePosition) + offsetLane;
        double newEndPosition = parse<double>(myMove.secondOriginalPosition) + offsetLane;
        // change start and end position of E2 detector ONLY if both extremes aren't overpassed
        if ((newStartPosition >= 0) && (newStartPosition <= getParentLanes().front()->getLaneShapeLength()) &&
                (newEndPosition >= 0) && (newEndPosition <= getParentLanes().back()->getLaneShapeLength())) {
            myPositionOverLane = newStartPosition;
            myEndPositionOverLane = newEndPosition;
        }
    }
    // Update geometry
    updateGeometry();
}


void
GNEDetectorE2::commitGeometryMoving(GNEUndoList* undoList) {
    // commit geometry moving depending of number of lanes
    if (getParentLanes().size() == 1) {
        // commit new position allowing undo/redo
        undoList->p_begin("position of " + getTagStr());
        undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_POSITION, toString(myPositionOverLane), true, myMove.firstOriginalLanePosition));
        undoList->p_end();
    } else {
        undoList->p_begin("position of " + getTagStr());
        undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_POSITION, toString(myPositionOverLane), true, myMove.firstOriginalLanePosition));
        undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_ENDPOS, toString(myEndPositionOverLane), true, myMove.secondOriginalPosition));
        undoList->p_end();
    }
}


void
GNEDetectorE2::updateGeometry() {
    // declare variables for start and end positions
    double startPosFixed, endPosFixed;

    // calculate start and end positions dependin of number of lanes
    if (getParentLanes().size() == 1) {

        // set start position
        if (myPositionOverLane < 0) {
            startPosFixed = 0;
        } else if (myPositionOverLane > getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength()) {
            startPosFixed = getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength();
        } else {
            startPosFixed = myPositionOverLane;
        }

        // set end position
        if ((myPositionOverLane + myLength) < 0) {
            endPosFixed = 0;
        } else if ((myPositionOverLane + myLength) > getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength()) {
            endPosFixed = getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength();
        } else {
            endPosFixed = (myPositionOverLane + myLength);
        }

        // Cut shape using as delimitators fixed start position and fixed end position
        myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), startPosFixed * getParentLanes().front()->getLengthGeometryFactor(), endPosFixed * getParentLanes().back()->getLengthGeometryFactor());

        // Set block icon position
        myBlockIcon.position = myAdditionalGeometry.getShape().getLineCenter();

    } else if (getParentLanes().size() > 1) {
        GNEGeometry::calculateLaneGeometricPath(this, mySegmentGeometry, getParentLanes(), myPositionOverLane, myEndPositionOverLane);
        /*
        // Set block icon position
        myBlockIcon.position = myAdditionalGeometry.multiShape.front().getLineCenter();

        // check integrity
        checkE2MultilaneIntegrity();
        */
    }

    // Set offset of the block icon
    myBlockIcon.offset = Position(-0.75, 0);

    // Set block icon rotation, and using their rotation for draw logo
    myBlockIcon.setRotation(getParentLanes().front());

    // mark dotted geometry deprecated
    myDottedGeometry.markDottedGeometryDeprecated();
}


void
GNEDetectorE2::updateDottedContour() {
    myDottedGeometry.updateDottedGeometry(myViewNet->getVisualisationSettings(),
                                          myAdditionalGeometry.getShape(),
                                          myViewNet->getVisualisationSettings().detectorSettings.E2Width);
}


double
GNEDetectorE2::getLength() const {
    return myLength;
}


void
GNEDetectorE2::checkE2MultilaneIntegrity() {
    // we assume that E2 is valid
    myE2valid = true;
    int i = 0;
    // iterate over all lanes, and stop if myE2valid is false
    while (i < ((int)getParentLanes().size() - 1) && myE2valid) {
        // set myE2valid to false
        myE2valid = false;
        // if a connection betwen "from" lane and "to" lane of connection is found, change myE2valid to true again
        for (auto j : getParentLanes().at(i)->getParentEdge()->getGNEConnections()) {
            if (j->getLaneFrom() == getParentLanes().at(i) && j->getLaneTo() == getParentLanes().at(i + 1)) {
                myE2valid = true;
            }
        }
        // update iterator
        i++;
    }
}


void
GNEDetectorE2::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double E2Exaggeration = s.addSize.getExaggeration(s, this);
    // first check if additional has to be drawn
    if ((myTagProperty.getTag() == SUMO_TAG_E2DETECTOR) && s.drawAdditionals(E2Exaggeration) && myViewNet->getDataViewOptions().showAdditionals()) {
        // Start drawing adding an gl identificator
        glPushName(getGlID());
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType());
        // Set color of the base
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
        } else {
            // set color depending if is or isn't valid
            if (myE2valid) {
                GLHelper::setColor(s.detectorSettings.E2Color);
            } else {
                GLHelper::setColor(RGBColor::RED);
            }
        }
        // check if we have to drawn a E2 single lane or a E2 multiLane
        if (myAdditionalGeometry.getShape().size() > 0) {
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GNEGeometry::drawGeometry(myViewNet, myAdditionalGeometry, s.detectorSettings.E2Width * E2Exaggeration);
        }
        // Pop last matrix
        glPopMatrix();
        // Check if the distance is enougth to draw details and isn't being drawn for selecting
        if ((s.drawDetail(s.detailSettings.detectorDetails, E2Exaggeration)) && !s.drawForRectangleSelection && !s.drawForPositionSelection) {
            // draw logo depending if this is an Multilane E2 detector
            if (myTagProperty.getTag() == SUMO_TAG_E2DETECTOR) {
                // Push matrix
                glPushMatrix();
                // Traslate to center of detector
                glTranslated(myAdditionalGeometry.getShape().getLineCenter().x(), myAdditionalGeometry.getShape().getLineCenter().y(), getType() + 0.1);
                // Rotate depending of myBlockIcon.rotation
                glRotated(myBlockIcon.rotation, 0, 0, -1);
                // move to logo position
                glTranslated(-0.75, 0, 0);
                // scale text
                glScaled(E2Exaggeration, E2Exaggeration, 1);
                // draw E2 logo
                if (drawUsingSelectColor()) {
                    GLHelper::drawText("E2", Position(), .1, 1.5, s.colorSettings.selectionColor);
                } else {
                    GLHelper::drawText("E2", Position(), .1, 1.5, RGBColor::BLACK);
                }
            } else {
                // Push matrix
                glPushMatrix();
                // Traslate to center of detector
                glTranslated(myBlockIcon.position.x(), myBlockIcon.position.y(), getType() + 0.1);
                // Rotate depending of myBlockIcon.rotation
                glRotated(myBlockIcon.rotation, 0, 0, -1);
                //move to logo position
                glTranslated(-1.5, 0, 0);
                // scale text
                glScaled(E2Exaggeration, E2Exaggeration, 1);
                // draw E2 logo
                if (drawUsingSelectColor()) {
                    GLHelper::drawText("E2", Position(), .1, 1.5, s.colorSettings.selectionColor);
                } else {
                    GLHelper::drawText("E2", Position(), .1, 1.5, RGBColor::BLACK);
                }
                //move to logo position
                glTranslated(1.2, 0, 0);
                // Rotate depending of myBlockIcon.rotation
                glRotated(90, 0, 0, 1);
                if (drawUsingSelectColor()) {
                    GLHelper::drawText("multi", Position(), .1, 0.9, s.colorSettings.selectedAdditionalColor);
                } else {
                    GLHelper::drawText("multi", Position(), .1, 0.9, RGBColor::BLACK);
                }
            }
            // pop matrix
            glPopMatrix();
            // Show Lock icon depending of the Edit mode
            myBlockIcon.drawIcon(s, E2Exaggeration);
        }
        // Draw name if isn't being drawn for selecting
        if (!s.drawForRectangleSelection) {
            drawName(getPositionInView(), s.scale, s.addName);
        }
        // check if dotted contour has to be drawn
        if (myViewNet->getDottedAC() == this) {
            if (myAdditionalGeometry.getShape().size() > 0) {
                GNEGeometry::drawShapeDottedContour(s, getType(), E2Exaggeration, myDottedGeometry);
            }
        }
        // Pop name
        glPopName();
    }
}


std::string
GNEDetectorE2::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_LANES:
            return parseIDs(getParentLanes());
        case SUMO_ATTR_POSITION:
            return toString(myPositionOverLane);
        case SUMO_ATTR_ENDPOS:
            return toString(myEndPositionOverLane);
        case SUMO_ATTR_FREQUENCY:
            return time2string(myFreq);
        case SUMO_ATTR_LENGTH:
            return toString(myLength);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_VTYPES:
            return myVehicleTypes;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return time2string(myTimeThreshold);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return toString(mySpeedThreshold);
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return toString(myJamThreshold);
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDetectorE2::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_LANES:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
        case SUMO_ATTR_FRIENDLY_POS:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorE2::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDetectorID(value);
        case SUMO_ATTR_LANE:
            if (value.empty()) {
                return false;
            } else {
                return canParse<std::vector<GNELane*> >(myViewNet->getNet(), value, false);
            }
        case SUMO_ATTR_LANES:
            if (value.empty()) {
                return false;
            } else if (canParse<std::vector<GNELane*> >(myViewNet->getNet(), value, false)) {
                // check if lanes are consecutives
                return lanesConsecutives(parse<std::vector<GNELane*> >(myViewNet->getNet(), value));
            } else {
                return false;
            }
        case SUMO_ATTR_POSITION:
            return canParse<double>(value);
        case SUMO_ATTR_ENDPOS:
            return canParse<double>(value);
        case SUMO_ATTR_FREQUENCY:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_LENGTH:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_FILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_VTYPES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfTypeID(value);
            }
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return canParse<SUMOTime>(value);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
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
GNEDetectorE2::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}

// ===========================================================================
// private
// ===========================================================================

void
GNEDetectorE2::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myViewNet->getNet()->getAttributeCarriers()->updateID(this, value);
            break;
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_LANES:
            replaceParentLanes(this, value);
            checkE2MultilaneIntegrity();
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = parse<double>(value);
            break;
        case SUMO_ATTR_ENDPOS:
            myEndPositionOverLane = parse<double>(value);
            break;
        case SUMO_ATTR_FREQUENCY:
            myFreq = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_LENGTH:
            myLength = parse<double>(value);
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_VTYPES:
            myVehicleTypes = value;
            break;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            myTimeThreshold = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            mySpeedThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            myJamThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
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
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/
