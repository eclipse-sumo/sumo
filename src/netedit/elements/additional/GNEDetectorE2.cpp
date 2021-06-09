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
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEDetectorE2.h"
#include "GNEAdditionalHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE2::GNEDetectorE2(const std::string& id, GNELane* lane, GNENet* net, double pos, double length, const double freq, 
        const std::string& trafficLight, const std::string& filename, const std::vector<std::string>& vehicleTypes, const std::string& name, 
        SUMOTime timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos, 
        const std::map<std::string, std::string> &parameters, bool blockMovement) :
    GNEDetector(id, net, GLO_E2DETECTOR, SUMO_TAG_E2DETECTOR, pos, freq, {lane}, filename, vehicleTypes, name, friendlyPos, parameters, blockMovement),
    myLength(length),
    myEndPositionOverLane(0.),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold),
    myTrafficLight(trafficLight) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEDetectorE2::GNEDetectorE2(const std::string& id, std::vector<GNELane*> lanes, GNENet* net, double pos, double endPos, const double freq, 
        const std::string& trafficLight, const std::string& filename, const std::vector<std::string>& vehicleTypes, const std::string& name, SUMOTime timeThreshold, 
        double speedThreshold, double jamThreshold, bool friendlyPos, const std::map<std::string, std::string> &parameters, bool blockMovement) :
    GNEDetector(id, net, GLO_E2DETECTOR, GNE_TAG_E2DETECTOR_MULTILANE, pos, freq, lanes, filename, vehicleTypes, name, friendlyPos, parameters, blockMovement),
    myLength(0),
    myEndPositionOverLane(endPos),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold),
    myTrafficLight(trafficLight) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
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
        if (areLaneConsecutives()) {
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
        // abort if lanes aren't consecutives
        if (!areLaneConsecutives()) {
            return "lanes aren't consecutives";
        }
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
        GNEAdditionalHandler::fixE2SingleLanePosition(newPositionOverLane, newLength, getParentLanes().at(0)->getParentEdge()->getNBEdge()->getFinalLength());
        // set new position and length
        setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
        setAttribute(SUMO_ATTR_LENGTH, toString(myLength), myNet->getViewNet()->getUndoList());
    } else {
        if (!areLaneConsecutives()) {
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
                    myNet->getViewNet()->getUndoList()->add(new GNEChange_Connection(getParentLanes().at(i)->getParentEdge(), newCon, false, true), true);
                }
                // update lane iterator
                i++;
            }
        } else {
            // declare new positions
            double newPositionOverLane = myPositionOverLane;
            double newEndPositionOverLane = myEndPositionOverLane;
            // fix pos and length checkAndFixDetectorPosition
            GNEAdditionalHandler::fixE2MultiLanePosition(
                newPositionOverLane, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(),
                newEndPositionOverLane, getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength());
            // set new position and endPosition
            setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
            setAttribute(SUMO_ATTR_ENDPOS, toString(newEndPositionOverLane), myNet->getViewNet()->getUndoList());
        }
    }
}


void
GNEDetectorE2::updateGeometry() {
    // check E2 detector
    if (myTagProperty.getTag() == GNE_TAG_E2DETECTOR_MULTILANE) {
        // compute path
        computePathElement();
    } else {
        // declare variables for start and end positions
        double startPosFixed = myPositionOverLane;
        double endPosFixed = myPositionOverLane + myLength;
        // adjust start and end pos
        if (startPosFixed < 0) {
            startPosFixed += myPositionOverLane > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
        }
        if (endPosFixed < 0) {
            endPosFixed += myPositionOverLane > getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength();
        }
        // set start position
        if (myPositionOverLane < 0) {
            startPosFixed = 0;
        } else if (myPositionOverLane > (getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength() - myLength)) {
            startPosFixed = (getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength() - myLength);
        }
        // set end position
        if ((myPositionOverLane + myLength) < 0) {
            endPosFixed = 0;
        } else if ((myPositionOverLane + myLength) > getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength()) {
            endPosFixed = getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength();
        }
        // Cut shape using as delimitators fixed start position and fixed end position
        myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(),
                                            (startPosFixed * getParentLanes().front()->getLengthGeometryFactor()),
                                            (endPosFixed * getParentLanes().back()->getLengthGeometryFactor()),
                                            myMoveElementLateralOffset);
        // update centering boundary without updating grid
        updateCenteringBoundary(false);
    }
}


double
GNEDetectorE2::getLength() const {
    return myLength;
}


void
GNEDetectorE2::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double E2Exaggeration = s.addSize.getExaggeration(s, this);
    // first check if additional has to be drawn
    if ((myTagProperty.getTag() == SUMO_TAG_E2DETECTOR) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // check exaggeration
        if (s.drawAdditionals(E2Exaggeration)) {
            // declare color
            RGBColor E2Color, textColor;
            // set color
            if (drawUsingSelectColor()) {
                E2Color = s.colorSettings.selectedAdditionalColor;
                textColor = E2Color.changedBrightness(-32);
            } else if (areLaneConsecutives()) {
                E2Color = s.detectorSettings.E2Color;
                textColor = RGBColor::BLACK;
            }
            // Start drawing adding an gl identificator
            glPushName(getGlID());
            // push layer matrix
            glPushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_E2DETECTOR);
            // set color
            GLHelper::setColor(E2Color);
            // draw geometry
            GNEGeometry::drawGeometry(myNet->getViewNet(), myAdditionalGeometry, s.detectorSettings.E2Width * E2Exaggeration);
            // Check if the distance is enought to draw details
            if (s.drawDetail(s.detailSettings.detectorDetails, E2Exaggeration)) {
                // draw E2 Logo
                drawDetectorLogo(s, E2Exaggeration, "E2", textColor);
                // draw lock icon
                GNEViewNetHelper::LockIcon::drawLockIcon(this, myAdditionalGeometry, E2Exaggeration, -1, 0, true);
            }
            // pop layer matrix
            glPopMatrix();
            // Pop name
            glPopName();
            // check if dotted contours has to be drawn
            if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GNEGeometry::drawDottedContourShape(GNEGeometry::DottedContourType::INSPECT, s, myAdditionalGeometry.getShape(), s.detectorSettings.E2Width, E2Exaggeration);
            }
            if (s.drawDottedContour() || myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                GNEGeometry::drawDottedContourShape(GNEGeometry::DottedContourType::FRONT, s, myAdditionalGeometry.getShape(), s.detectorSettings.E2Width, E2Exaggeration);
            }
        }
        // Draw additional ID
        drawAdditionalID(s);
        // draw additional name
        drawAdditionalName(s);
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
            return toString(myFreq);
        case SUMO_ATTR_TLID:
            return myTrafficLight;
        case SUMO_ATTR_LENGTH:
            return toString(myLength);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_VTYPES:
            return toString(myVehicleTypes);
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


double
GNEDetectorE2::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_POSITION:
            return myPositionOverLane;
        case SUMO_ATTR_ENDPOS:
            return myEndPositionOverLane;
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
        case SUMO_ATTR_TLID:
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
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorE2::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            if (isValidDetectorID(value)) {
                if (myTagProperty.getTag() == SUMO_TAG_E2DETECTOR) {
                    return (myNet->retrieveAdditional(GNE_TAG_E2DETECTOR_MULTILANE, value, false) == nullptr);
                } else {
                    return (myNet->retrieveAdditional(SUMO_TAG_E2DETECTOR, value, false) == nullptr);
                }
            } else {
                return false;
            }
        case SUMO_ATTR_LANE:
            if (value.empty()) {
                return false;
            } else {
                return canParse<std::vector<GNELane*> >(myNet, value, false);
            }
        case SUMO_ATTR_LANES:
            if (value.empty()) {
                return false;
            } else if (canParse<std::vector<GNELane*> >(myNet, value, false)) {
                // check if lanes are consecutives
                return lanesConsecutives(parse<std::vector<GNELane*> >(myNet, value));
            } else {
                return false;
            }
        case SUMO_ATTR_POSITION:
            return canParse<double>(value);
        case SUMO_ATTR_ENDPOS:
            return canParse<double>(value);
        case SUMO_ATTR_FREQUENCY:
            return value.empty() || (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_TLID:
            /* temporal */
            return true;
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
            myNet->getAttributeCarriers()->updateID(this, value);
            break;
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_LANES:
            replaceAdditionalParentLanes(value);
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = parse<double>(value);
            break;
        case SUMO_ATTR_ENDPOS:
            myEndPositionOverLane = parse<double>(value);
            break;
        case SUMO_ATTR_FREQUENCY:
            myFreq = parse<double>(value);
            break;
        case SUMO_ATTR_TLID:
            myTrafficLight = value;
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
            myVehicleTypes = parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            myTimeThreshold = TIME2STEPS(parse<double>(value));
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


bool
GNEDetectorE2::areLaneConsecutives() const {
    // declare lane iterator
    int i = 0;
    // iterate over all lanes, and stop if myE2valid is false
    while (i < ((int)getParentLanes().size() - 1)) {
        // we assume that E2 is invalid
        bool connectionFound = false;
        // if there is a connection betwen "from" lane and "to" lane of connection, change connectionFound to true
        for (auto j : getParentLanes().at(i)->getParentEdge()->getGNEConnections()) {
            if (j->getLaneFrom() == getParentLanes().at(i) && j->getLaneTo() == getParentLanes().at(i + 1)) {
                connectionFound = true;
            }
        }
        // abort if connectionFound is false
        if (!connectionFound) {
            return false;
        }
        // update iterator
        i++;
    }
    // there are connections between all lanes, then return true
    return true;
}


/****************************************************************************/
