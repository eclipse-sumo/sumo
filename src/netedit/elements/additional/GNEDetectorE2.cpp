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

GNEDetectorE2::GNEDetectorE2(SumoXMLTag tag, GNENet* net) :
    GNEDetector("", net, GLO_E2DETECTOR, tag, 0, 0, {}, "", {}, "", false, std::map<std::string, std::string>()),
    myEndPositionOverLane(0),
    myTimeThreshold(0),
    mySpeedThreshold(0),
    myJamThreshold(0) {
    // reset default values
    resetDefaultValues();
}


GNEDetectorE2::GNEDetectorE2(const std::string& id, GNELane* lane, GNENet* net, double pos, double length, const SUMOTime freq,
                             const std::string& trafficLight, const std::string& filename, const std::vector<std::string>& vehicleTypes, const std::string& name,
                             SUMOTime timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos,
                             const std::map<std::string, std::string>& parameters) :
    GNEDetector(id, net, GLO_E2DETECTOR, SUMO_TAG_E2DETECTOR, pos, freq, {lane}, filename, vehicleTypes, name, friendlyPos, parameters),
    myEndPositionOverLane(pos + length),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold),
    myTrafficLight(trafficLight) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEDetectorE2::GNEDetectorE2(const std::string& id, std::vector<GNELane*> lanes, GNENet* net, double pos, double endPos, const SUMOTime freq,
                             const std::string& trafficLight, const std::string& filename, const std::vector<std::string>& vehicleTypes, const std::string& name, SUMOTime timeThreshold,
                             double speedThreshold, double jamThreshold, bool friendlyPos, const std::map<std::string, std::string>& parameters) :
    GNEDetector(id, net, GLO_E2DETECTOR, GNE_TAG_E2DETECTOR_MULTILANE, pos, freq, lanes, filename, vehicleTypes, name, friendlyPos, parameters),
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


void 
GNEDetectorE2::writeAdditional(OutputDevice& device) const {
    device.openTag(getTagProperty().getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    if (!myAdditionalName.empty()) {
        device.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(myAdditionalName));
    }
    // continue depending of E2 type
    if (myTagProperty.getTag() == SUMO_TAG_E2DETECTOR) {
        device.writeAttr(SUMO_ATTR_LANE, getParentLanes().front()->getID());
        device.writeAttr(SUMO_ATTR_POSITION, myPositionOverLane);
        device.writeAttr(SUMO_ATTR_LENGTH, toString(myEndPositionOverLane - myPositionOverLane));
    } else {
        device.writeAttr(SUMO_ATTR_LANES, getAttribute(SUMO_ATTR_LANES));
        device.writeAttr(SUMO_ATTR_POSITION, myPositionOverLane);
        device.writeAttr(SUMO_ATTR_ENDPOS, myEndPositionOverLane);
    }
    if (myTrafficLight.size() > 0) {
        device.writeAttr(SUMO_ATTR_TLID, myTrafficLight);
    } else {
        device.writeAttr(SUMO_ATTR_FREQUENCY, time2string(myFreq));
    }
    if (myFilename.size() > 0) {
        device.writeAttr(SUMO_ATTR_FILE, myFilename);
    }
    if (myVehicleTypes.size() > 0) {
        device.writeAttr(SUMO_ATTR_VTYPES, myVehicleTypes);
    }
    if (getAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD) != myTagProperty.getDefaultValue(SUMO_ATTR_HALTING_TIME_THRESHOLD)) {
        device.writeAttr(SUMO_ATTR_HALTING_TIME_THRESHOLD, mySpeedThreshold);
    }
    if (getAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD) != myTagProperty.getDefaultValue(SUMO_ATTR_HALTING_SPEED_THRESHOLD)) {
        device.writeAttr(SUMO_ATTR_HALTING_SPEED_THRESHOLD, mySpeedThreshold);
    }
    if (getAttribute(SUMO_ATTR_JAM_DIST_THRESHOLD) != myTagProperty.getDefaultValue(SUMO_ATTR_JAM_DIST_THRESHOLD)) {
        device.writeAttr(SUMO_ATTR_JAM_DIST_THRESHOLD, mySpeedThreshold);
    }
    if (myFriendlyPosition) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, true);
    }
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


bool
GNEDetectorE2::isAdditionalValid() const {
    if (getParentLanes().size() == 1) {
        // with friendly position enabled position are "always fixed"
        if (myFriendlyPosition) {
            return true;
        } else {
            return (myPositionOverLane >= 0) && (myEndPositionOverLane <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
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
        if ((myEndPositionOverLane) > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
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
        double newLength = (myEndPositionOverLane - myPositionOverLane);
        // fix pos and length using fixE2DetectorPosition
        GNEAdditionalHandler::fixE2SingleLanePosition(newPositionOverLane, newLength, getParentLanes().at(0)->getParentEdge()->getNBEdge()->getFinalLength());
        // set new position and length
        setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
        setAttribute(SUMO_ATTR_LENGTH, toString(newLength), myNet->getViewNet()->getUndoList());
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
                for (const auto& connection : getParentLanes().at(i)->getParentEdge()->getGNEConnections()) {
                    if ((connection->getLaneFrom() == getParentLanes().at(i)) && (connection->getLaneTo() == getParentLanes().at(i + 1))) {
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
        // Cut shape using as delimitators fixed start position and fixed end position
        myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane(), myMoveElementLateralOffset);
        // update centering boundary without updating grid
        updateCenteringBoundary(false);
    }
}


void
GNEDetectorE2::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double E2Exaggeration = getExaggeration(s);
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
            // draw parent and child lines
            drawParentChildLines(s, s.additionalSettings.connectionColor);
            // Start drawing adding an gl identificator
            GLHelper::pushName(getGlID());
            // push layer matrix
            GLHelper::pushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_E2DETECTOR);
            // set color
            GLHelper::setColor(E2Color);
            // draw geometry
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myAdditionalGeometry, s.detectorSettings.E2Width * E2Exaggeration);
            // Check if the distance is enought to draw details
            if (s.drawDetail(s.detailSettings.detectorDetails, E2Exaggeration)) {
                // draw E2 Logo
                drawDetectorLogo(s, E2Exaggeration, "E2", textColor);
            }
            // draw geometry points
            drawLeftGeometryPoint(myNet->getViewNet(), myAdditionalGeometry.getShape().front(), myAdditionalGeometry.getShapeRotations().front(), E2Color);
            drawRightGeometryPoint(myNet->getViewNet(), myAdditionalGeometry.getShape().back(), myAdditionalGeometry.getShapeRotations().back(), E2Color);
            // pop layer matrix
            GLHelper::popMatrix();
            // Pop name
            GLHelper::popName();
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), myAdditionalGeometry.getShape().getCentroid(), E2Exaggeration);
            // check if dotted contours has to be drawn
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, myAdditionalGeometry.getShape(), s.detectorSettings.E2Width,
                        E2Exaggeration, true, true);
            }
            if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::FRONT, s, myAdditionalGeometry.getShape(), s.detectorSettings.E2Width,
                        E2Exaggeration, true, true);
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
            return time2string(myFreq);
        case SUMO_ATTR_TLID:
            return myTrafficLight;
        case SUMO_ATTR_LENGTH:
            return toString(myEndPositionOverLane - myPositionOverLane);
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
GNEDetectorE2::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_POSITION:
            return myPositionOverLane;
        case SUMO_ATTR_LENGTH:
            return (myEndPositionOverLane - myPositionOverLane);
        case SUMO_ATTR_ENDPOS:
            return myEndPositionOverLane;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDetectorE2::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
GNEDetectorE2::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            if (isValidDetectorID(value)) {
                if (myTagProperty.getTag() == SUMO_TAG_E2DETECTOR) {
                    return (myNet->getAttributeCarriers()->retrieveAdditional(GNE_TAG_E2DETECTOR_MULTILANE, value, false) == nullptr);
                } else {
                    return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_E2DETECTOR, value, false) == nullptr);
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
            // update microsimID
            setMicrosimID(value);
            break;
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_LANES:
            replaceAdditionalParentLanes(value);
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = parse<double>(value);
            // update geometry (except for template)
            if (getParentLanes().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_ENDPOS:
            myEndPositionOverLane = parse<double>(value);
            // update geometry (except for template)
            if (getParentLanes().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_FREQUENCY:
            myFreq = string2time(value);
            break;
        case SUMO_ATTR_TLID:
            myTrafficLight = value;
            break;
        case SUMO_ATTR_LENGTH:
            myEndPositionOverLane = (myPositionOverLane + parse<double>(value));
            // update geometry (except for template)
            if (getParentLanes().size() > 0) {
                updateGeometry();
            }
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
GNEDetectorE2::setMoveShape(const GNEMoveResult& moveResult) {
    if ((moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST) ||
            (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVEFIRST)) {
        // change only start position
        myPositionOverLane = moveResult.newFirstPos;
    } else if ((moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND) ||
               (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVESECOND)) {
        // change only end position
        myEndPositionOverLane = moveResult.newFirstPos;
    } else {
        // change both position
        myPositionOverLane = moveResult.newFirstPos;
        myEndPositionOverLane = moveResult.newSecondPos;
        // set lateral offset
        myMoveElementLateralOffset = moveResult.firstLaneOffset;
    }
    // update geometry
    updateGeometry();
}


void
GNEDetectorE2::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // begin change attribute
    undoList->begin(myTagProperty.getGUIIcon(), "position of " + getTagStr());
    // set attributes depending of operation type
    if ((moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST) ||
            (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVEFIRST)) {
        // set only start position
        setAttribute(SUMO_ATTR_POSITION, toString(moveResult.newFirstPos), undoList);
    } else if ((moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND) ||
               (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVESECOND)) {
        // set only end position
        setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newFirstPos), undoList);
    } else {
        // set both
        setAttribute(SUMO_ATTR_POSITION, toString(moveResult.newFirstPos), undoList);
        setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newSecondPos), undoList);
        // check if lane has to be changed
        if (moveResult.newFirstLane) {
            // set new lane
            setAttribute(SUMO_ATTR_LANE, moveResult.newFirstLane->getID(), undoList);
        }
    }
    // end change attribute
    undoList->end();
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


double
GNEDetectorE2::getStartGeometryPositionOverLane() const {
    // get lane final and shape length
    const double laneLength = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    // get startPosition
    double fixedPos = myPositionOverLane;
    // adjust fixedPos
    if (fixedPos < 0) {
        fixedPos += laneLength;
    }
    fixedPos *= getParentLanes().front()->getLengthGeometryFactor();
    // return depending of fixedPos
    if (fixedPos < 0) {
        return 0;
    } else if (fixedPos > (getParentLanes().front()->getLaneShapeLength() - POSITION_EPS)) {
        return (getParentLanes().front()->getLaneShapeLength() - POSITION_EPS);
    } else {
        return fixedPos;
    }
}


double
GNEDetectorE2::getEndGeometryPositionOverLane() const {
    // get lane final and shape length
    const double laneLength = getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength();
    // get endPosition
    double fixedPos = myEndPositionOverLane;
    // adjust fixedPos
    if (fixedPos < 0) {
        fixedPos += laneLength;
    }
    fixedPos *= getParentLanes().back()->getLengthGeometryFactor();
    // return depending of fixedPos
    if (fixedPos < POSITION_EPS) {
        return POSITION_EPS;
    } else if (fixedPos > getParentLanes().back()->getLaneShapeLength()) {
        return getParentLanes().back()->getLaneShapeLength();
    } else {
        return fixedPos;
    }
}

/****************************************************************************/
