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
/// @file    GNEStoppingPlace.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// A abstract class to define common parameters of lane area in which vehicles can halt (GNE version)
/****************************************************************************/
#include <config.h>

#include <foreign/fontstash/fontstash.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMORouteHandler.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEStoppingPlace.h"
#include "GNEAdditionalHandler.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEStoppingPlace::GNEStoppingPlace(GNENet* net, SumoXMLTag tag) :
    GNEAdditional("", net, "", tag, "") {
}


GNEStoppingPlace::GNEStoppingPlace(const std::string& id, GNENet* net, const std::string& filename,
                                   SumoXMLTag tag, GNELane* lane, const double startPos, const double endPos,
                                   const std::string& name, bool friendlyPosition, const RGBColor& color,
                                   const double angle, const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, tag, name),
    GNELaneMovableElement(this, lane, startPos, endPos, friendlyPosition),
    Parameterised(parameters),
    myColor(color),
    myAngle(angle) {
}


GNEStoppingPlace::~GNEStoppingPlace() {}


GNEMoveOperation*
GNEStoppingPlace::getMoveOperation() {
    // get allow change lane
    const bool allowChangeLane = myNet->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeLane();
    // fist check if we're moving only extremes
    if (drawMovingGeometryPoints()) {
        // get geometry points under cursor
        const auto geometryPoints = gViewObjectsHandler.getSelectedGeometryPoints(this);
        // continue depending of moved element
        if (geometryPoints.empty()) {
            return nullptr;
        } else if (geometryPoints.front() == 0) {
            // move start position
            return new GNEMoveOperation(this, getParentLanes().front(), myStartPosition, getParentLanes().front()->getLaneShape().length2D() - POSITION_EPS,
                                        allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST);
        } else {
            // move end position
            return new GNEMoveOperation(this, getParentLanes().front(), 0, myEndPosition,
                                        allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST);
        }
    } else if ((myStartPosition != INVALID_DOUBLE) && (myEndPosition != INVALID_DOUBLE)) {
        // move both start and end positions
        return new GNEMoveOperation(this, getParentLanes().front(), myStartPosition, myEndPosition,
                                    allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_BOTH);
    } else if (myStartPosition != INVALID_DOUBLE) {
        // move only start position
        return new GNEMoveOperation(this, getParentLanes().front(), myStartPosition, getParentLanes().front()->getLaneShape().length2D() - POSITION_EPS,
                                    allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST);
    } else if (myEndPosition != INVALID_DOUBLE) {
        // move only end position
        return new GNEMoveOperation(this, getParentLanes().front(), 0, myEndPosition,
                                    allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST);
    } else {
        // start and end positions undefined, then nothing to move
        return nullptr;
    }
}


bool
GNEStoppingPlace::isAdditionalValid() const {
    return GNEAdditionalHandler::checkLaneDoublePosition(getAttributeDouble(SUMO_ATTR_STARTPOS), getAttributeDouble(SUMO_ATTR_ENDPOS),
            getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), myFriendlyPosition);
}


std::string
GNEStoppingPlace::getAdditionalProblem() const {
    // obtain lane length
    double laneLength = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength() * getParentLanes().front()->getLengthGeometryFactor();
    // calculate start and end positions
    double startPos = getAttributeDouble(SUMO_ATTR_STARTPOS);
    double endPos = getAttributeDouble(SUMO_ATTR_ENDPOS);
    // check if position has to be fixed
    if (startPos < 0) {
        startPos += laneLength;
    }
    if (endPos < 0) {
        endPos += laneLength;
    }
    // declare variables
    std::string errorStart, separator, errorEnd;
    // check positions over lane
    if (startPos < 0) {
        errorStart = (toString(SUMO_ATTR_STARTPOS) + " < 0");
    } else if (startPos > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
        errorStart = (toString(SUMO_ATTR_STARTPOS) + TL(" > lanes's length"));
    }
    if (endPos < 0) {
        errorEnd = (toString(SUMO_ATTR_ENDPOS) + " < 0");
    } else if (endPos > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
        errorEnd = (toString(SUMO_ATTR_ENDPOS) + TL(" > lanes's length"));
    }
    // check separator
    if ((errorStart.size() > 0) && (errorEnd.size() > 0)) {
        separator = TL(" and ");
    }
    return errorStart + separator + errorEnd;
}


void
GNEStoppingPlace::fixAdditionalProblem() {
    // calculate start and end positions
    double startPos = getAttributeDouble(SUMO_ATTR_STARTPOS);
    double endPos = getAttributeDouble(SUMO_ATTR_ENDPOS);
    // fix start and end positions using fixLaneDoublePosition
    GNEAdditionalHandler::fixLaneDoublePosition(startPos, endPos, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
    // set new start and end positions
    setAttribute(SUMO_ATTR_STARTPOS, toString(startPos), myNet->getViewNet()->getUndoList());
    setAttribute(SUMO_ATTR_ENDPOS, toString(endPos), myNet->getViewNet()->getUndoList());
}


bool
GNEStoppingPlace::checkDrawMoveContour() const {
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


Position
GNEStoppingPlace::getPositionInView() const {
    return myAdditionalGeometry.getShape().getPolygonCenter();
}


void
GNEStoppingPlace::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to do
}


void
GNEStoppingPlace::splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList) {
    // first check tat both network elements are lanes and originalElement correspond to stoppingPlace lane
    if ((originalElement->getTagProperty()->getTag() == SUMO_TAG_LANE) &&
            (newElement->getTagProperty()->getTag() == SUMO_TAG_LANE) &&
            (getParentLanes().front() == originalElement)) {
        // check if we have to change additional lane depending of split position
        if ((myStartPosition != INVALID_DOUBLE) && (myEndPosition != INVALID_DOUBLE)) {
            // calculate middle position
            const double middlePosition = ((myEndPosition - myStartPosition) / 2.0) + myStartPosition;
            //  four cases:
            if (splitPosition < myStartPosition) {
                // change lane
                setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
                // now adjust start and end position
                setAttribute(SUMO_ATTR_STARTPOS, toString(myStartPosition - splitPosition), undoList);
                setAttribute(SUMO_ATTR_ENDPOS, toString(myEndPosition - splitPosition), undoList);
            } else if ((splitPosition > myStartPosition) && (splitPosition < middlePosition)) {
                // change lane
                setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
                // now adjust start and end position
                setAttribute(SUMO_ATTR_STARTPOS, "0", undoList);
                setAttribute(SUMO_ATTR_ENDPOS, toString(myEndPosition - splitPosition), undoList);
            } else if ((splitPosition > middlePosition) && (splitPosition < myEndPosition)) {
                // only adjust end position
                setAttribute(SUMO_ATTR_ENDPOS, toString(splitPosition), undoList);
            } else if ((splitPosition > myEndPosition)) {
                // nothing to do
            }
        } else if ((myStartPosition != INVALID_DOUBLE) && (splitPosition < myStartPosition)) {
            // change lane
            setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
            // now adjust start position
            setAttribute(SUMO_ATTR_STARTPOS, toString(myEndPosition - splitPosition), undoList);
        } else if ((myEndPosition != INVALID_DOUBLE) && (splitPosition < myEndPosition)) {
            // change lane
            setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
            // now adjust end position
            setAttribute(SUMO_ATTR_ENDPOS, toString(myEndPosition - splitPosition), undoList);
        }
    }
}


std::string
GNEStoppingPlace::getParentName() const {
    return getParentLanes().front()->getID();
}


void
GNEStoppingPlace::writeStoppingPlaceAttributes(OutputDevice& device) const {
    // id
    device.writeAttr(SUMO_ATTR_ID, getID());
    // name (if defined)
    if (!myAdditionalName.empty()) {
        device.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(myAdditionalName));
    }
    // lane
    device.writeAttr(SUMO_ATTR_LANE, getParentLanes().front()->getID());
    // start and end positions
    if (myStartPosition != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_STARTPOS)) {
        device.writeAttr(SUMO_ATTR_STARTPOS, myStartPosition);
    }
    if (myEndPosition != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_ENDPOS)) {
        device.writeAttr(SUMO_ATTR_ENDPOS, myEndPosition);
    }
    // friendly position (only if true)
    if (myFriendlyPosition) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, myFriendlyPosition);
    }
    // color (if defined)
    if (getAttribute(SUMO_ATTR_COLOR).size() > 0) {
        device.writeAttr(SUMO_ATTR_COLOR, myColor);
    }
    // angle (if defined)
    if (myAngle != 0) {
        device.writeAttr(SUMO_ATTR_ANGLE, myAngle);
    }
}


std::string
GNEStoppingPlace::getStoppingPlaceAttribute(const Parameterised* parameterised, SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANE:
            return getParentLanes().front()->getID();
        case SUMO_ATTR_STARTPOS:
            if (myStartPosition != INVALID_DOUBLE) {
                return toString(myStartPosition);
            } else {
                return LANE_START;
            }
        case SUMO_ATTR_ENDPOS:
            if (myEndPosition != INVALID_DOUBLE) {
                return toString(myEndPosition);
            } else {
                return LANE_END;
            }
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case SUMO_ATTR_COLOR:
            if (myColor == RGBColor::INVISIBLE) {
                return "";
            } else {
                return toString(myColor);
            }
        case SUMO_ATTR_ANGLE:
            return toString(myAngle);
        // special attributes used during creation or edition
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        case GNE_ATTR_SIZE:
            if (isTemplate()) {
                return toString(mySize);
            } else if ((myStartPosition != INVALID_DOUBLE) && (myEndPosition != INVALID_DOUBLE)) {
                return toString(myEndPosition - myStartPosition);
            } else if (myStartPosition != INVALID_DOUBLE) {
                return toString(getParentLanes().front()->getLaneShapeLength() - myStartPosition);
            } else if (myEndPosition != INVALID_DOUBLE) {
                return toString(myEndPosition);
            } else if (getParentLanes().size() > 0) {
                return toString(getParentLanes().front()->getLaneShapeLength());
            } else {
                return "10";
            }
        case GNE_ATTR_FORCESIZE:
            return toString(myForceSize);
        case GNE_ATTR_REFERENCE:
            return SUMOXMLDefinitions::ReferencePositions.getString(myReferencePosition);
        default:
            return getCommonAttribute(parameterised, key);
    }
}


double
GNEStoppingPlace::getStoppingPlaceAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_STARTPOS:
            if (myStartPosition != INVALID_DOUBLE) {
                return myStartPosition;
            } else {
                return 0;
            }
        case SUMO_ATTR_ENDPOS:
            if (myEndPosition != INVALID_DOUBLE) {
                return myEndPosition;
            } else {
                return getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
            }
        case SUMO_ATTR_CENTER:
            return ((getStoppingPlaceAttributeDouble(SUMO_ATTR_ENDPOS) - getStoppingPlaceAttributeDouble(SUMO_ATTR_STARTPOS)) * 0.5) + getStoppingPlaceAttributeDouble(SUMO_ATTR_STARTPOS);
        case SUMO_ATTR_ANGLE:
            return myAngle;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


void
GNEStoppingPlace::setStoppingPlaceAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FRIENDLY_POS:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_ANGLE:
        // special attributes used during creation or edition
        case GNE_ATTR_SHIFTLANEINDEX:
        case GNE_ATTR_REFERENCE:
        case GNE_ATTR_FORCESIZE:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        case GNE_ATTR_SIZE:
            adjustLength(parse<double>(value), undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNEStoppingPlace::isStoppingPlaceValid(SumoXMLAttr key, const std::string& value) const {
    switch (key) {
        case SUMO_ATTR_ID:
            if (std::find(NamespaceIDs::busStops.begin(), NamespaceIDs::busStops.end(), myTagProperty->getTag()) != NamespaceIDs::busStops.end()) {
                return isValidAdditionalID(NamespaceIDs::busStops, value);
            } else {
                return isValidAdditionalID(value);
            }
        case SUMO_ATTR_LANE:
            if (myNet->getAttributeCarriers()->retrieveLane(value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_STARTPOS:
            if (value.empty() || (value == LANE_START)) {
                return true;
            } else if (canParse<double>(value)) {
                return SUMORouteHandler::isStopPosValid(parse<double>(value), getAttributeDouble(SUMO_ATTR_ENDPOS), getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), POSITION_EPS, myFriendlyPosition);
            } else {
                return false;
            }
        case SUMO_ATTR_ENDPOS:
            if (value.empty() || (value == LANE_END)) {
                return true;
            } else if (canParse<double>(value)) {
                return SUMORouteHandler::isStopPosValid(getAttributeDouble(SUMO_ATTR_STARTPOS), parse<double>(value), getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), POSITION_EPS, myFriendlyPosition);
            } else {
                return false;
            }
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case SUMO_ATTR_COLOR:
            if (value.empty()) {
                return true;
            } else {
                return canParse<RGBColor>(value);
            }
        case SUMO_ATTR_ANGLE:
            if (value.empty()) {
                return true;
            } else {
                return canParse<double>(value);
            }
        // special attributes used during creation or edition
        case GNE_ATTR_SHIFTLANEINDEX:
            return true;
        case GNE_ATTR_SIZE:
            if (value.empty()) {
                return true;
            } else {
                return canParse<double>(value) && parse<double>(value) >= POSITION_EPS;
            }
        case GNE_ATTR_FORCESIZE:
            return canParse<bool>(value);
        case GNE_ATTR_REFERENCE:
            return SUMOXMLDefinitions::ReferencePositions.hasString(value);
        default:
            return isCommonValid(key, value);
    }
}


void
GNEStoppingPlace::setStoppingPlaceAttribute(Parameterised* parameterised, SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            replaceAdditionalParentLanes(value);
            break;
        case SUMO_ATTR_STARTPOS:
            if (value.empty() || (value == LANE_START)) {
                myStartPosition = INVALID_DOUBLE;
            } else {
                myStartPosition = parse<double>(value);
            }
            break;
        case SUMO_ATTR_ENDPOS:
            if (value.empty() || (value == LANE_END)) {
                myEndPosition = INVALID_DOUBLE;
            } else {
                myEndPosition = parse<double>(value);
            }
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        case SUMO_ATTR_COLOR:
            if (value.empty()) {
                myColor = RGBColor::INVISIBLE;
            } else {
                myColor = GNEAttributeCarrier::parse<RGBColor>(value);
            }
            break;
        case SUMO_ATTR_ANGLE:
            if (value.empty()) {
                myAngle = 0;
            } else {
                myAngle = GNEAttributeCarrier::parse<double>(value);
            }
            break;
        // special attributes used during creation or edition
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        case GNE_ATTR_SIZE:
            if (value.empty()) {
                mySize = 10;
            } else {
                mySize = parse<double>(value);
            }
            break;
        case GNE_ATTR_FORCESIZE:
            myForceSize = parse<bool>(value);
            break;
        case GNE_ATTR_REFERENCE:
            myReferencePosition = SUMOXMLDefinitions::ReferencePositions.get(value);
            break;
        default:
            setCommonAttribute(parameterised, key, value);
            break;
    }
}


void
GNEStoppingPlace::setStoppingPlaceGeometry(double movingToSide) {
    if (getParentLanes().empty() || getParentLanes().front() == nullptr) {
        // may happen during initialization
        return;
    }
    // Get value of option "lefthand"
    const bool lefthandAttr = hasAttribute(SUMO_ATTR_LEFTHAND) && parse<bool>(getAttribute(SUMO_ATTR_LEFTHAND));
    const double offsetSign = OptionsCont::getOptions().getBool("lefthand") != lefthandAttr  ? -1 : 1;

    // obtain laneShape
    PositionVector laneShape = getParentLanes().front()->getLaneShape();

    // Move shape to side
    laneShape.move2side(movingToSide * offsetSign);

    // Cut shape using as delimitators fixed start position and fixed end position
    myAdditionalGeometry.updateGeometry(laneShape, getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane(), myMoveElementLateralOffset);
}


const Parameterised::Map&
GNEStoppingPlace::getACParametersMap() const {
    return getParametersMap();
}


bool
GNEStoppingPlace::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case GNE_ATTR_REFERENCE:
            if (isTemplate()) {
                return true;
            } else {
                return (myStartPosition != INVALID_DOUBLE) && (myEndPosition != INVALID_DOUBLE);
            }
        case GNE_ATTR_SIZE:
            if (isTemplate()) {
                return true;
            } else {
                return (myStartPosition != INVALID_DOUBLE) || (myEndPosition != INVALID_DOUBLE);
            }
        default:
            return true;
    }
}


std::string
GNEStoppingPlace::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNEStoppingPlace::getHierarchyName() const {
    return getTagStr();
}


void
GNEStoppingPlace::drawLines(const GUIVisualizationSettings::Detail d, const std::vector<std::string>& lines, const RGBColor& color) const {
    // only draw in level 1
    if (d <= GUIVisualizationSettings::Detail::Text) {
        // calculate middle point
        const double middlePoint = (myAdditionalGeometry.getShape().length2D() * 0.5);
        // calculate rotation
        const double rot = (myAdditionalGeometry.getShape().size() <= 1) ? 0 : myAdditionalGeometry.getShape().rotationDegreeAtOffset(middlePoint);
        // Iterate over every line
        for (int i = 0; i < (int)lines.size(); ++i) {
            // push a new matrix for every line
            GLHelper::pushMatrix();
            // translate
            glTranslated(mySymbolPosition.x(), mySymbolPosition.y(), 0);
            // rotate over lane
            GUIGeometry::rotateOverLane(rot);
            // draw line with a color depending of the selection status
            if (drawUsingSelectColor()) {
                GLHelper::drawText(lines[i].c_str(), Position(1.2, (double)i), .1, 1.f, color, 0, FONS_ALIGN_LEFT);
            } else {
                GLHelper::drawText(lines[i].c_str(), Position(1.2, (double)i), .1, 1.f, color, 0, FONS_ALIGN_LEFT);
            }
            // pop matrix for every line
            GLHelper::popMatrix();
        }
    }
}


void
GNEStoppingPlace::drawSign(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const double exaggeration,
                           const RGBColor& baseColor, const RGBColor& signColor, const std::string& word) const {
    // only draw in level 2
    if (d <= GUIVisualizationSettings::Detail::AdditionalDetails) {
        // calculate middle point
        const double middlePoint = (myAdditionalGeometry.getShape().length2D() * 0.5);
        // calculate rotation
        const double rotSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;
        const double rot = (myAdditionalGeometry.getShape().size() <= 1) ? 0 : myAdditionalGeometry.getShape().rotationDegreeAtOffset(middlePoint) - 180. * rotSign;
        // push matrix
        GLHelper::pushMatrix();
        // Start drawing sign traslating matrix to signal position
        glTranslated(mySymbolPosition.x(), mySymbolPosition.y(), 0);
        // rotate over lane
        GUIGeometry::rotateOverLane(rot);
        // scale matrix depending of the exaggeration
        glScaled(exaggeration, exaggeration, 1);
        // set color
        GLHelper::setColor(baseColor);
        // Draw circle
        GLHelper::drawFilledCircleDetailled(d, s.stoppingPlaceSettings.symbolExternalRadius);
        // continue depending of rectangle selection
        if (d <= GUIVisualizationSettings::Detail::Text) {
            // Traslate to front
            glTranslated(0, 0, .1);
            // set color
            GLHelper::setColor(signColor);
            // draw another circle in the same position, but a little bit more small
            GLHelper::drawFilledCircleDetailled(d, s.stoppingPlaceSettings.symbolInternalRadius);
            // draw H depending of detailSettings
            GLHelper::drawText(word, Position(), .1, s.stoppingPlaceSettings.symbolInternalTextSize, baseColor);
        }
        // pop draw matrix
        GLHelper::popMatrix();
    }
}


void
GNEStoppingPlace::calculateStoppingPlaceContour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const double width, const double exaggeration, const bool movingGeometryPoints) const {
    // check if we're calculating the contour or the moving geometry points
    if (movingGeometryPoints) {
        if (myStartPosition != INVALID_DOUBLE) {
            myAdditionalContour.calculateContourFirstGeometryPoint(s, d, this, myAdditionalGeometry.getShape(),
                    getType(), s.neteditSizeSettings.additionalGeometryPointRadius, 1);
        }
        if (movingGeometryPoints && (myEndPosition != INVALID_DOUBLE)) {
            myAdditionalContour.calculateContourLastGeometryPoint(s, d, this, myAdditionalGeometry.getShape(),
                    getType(), s.neteditSizeSettings.additionalGeometryPointRadius, 1);
        }
    } else {
        // don't exaggerate contour
        myAdditionalContour.calculateContourExtrudedShape(s, d, this, myAdditionalGeometry.getShape(), getType(), width, 1, true, true, 0,
                nullptr, getParentLanes().front()->getParentEdge());
        mySymbolContour.calculateContourCircleShape(s, d, this, mySymbolPosition, s.stoppingPlaceSettings.symbolExternalRadius, getType(),
                exaggeration, getParentLanes().front()->getParentEdge());
    }
}


double
GNEStoppingPlace::getStartGeometryPositionOverLane() const {
    if (myStartPosition != INVALID_DOUBLE) {
        // get lane final and shape length
        const double laneLength = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
        // get startPosition
        double fixedPos = myStartPosition;
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
    } else {
        return 0;
    }
}


double
GNEStoppingPlace::getEndGeometryPositionOverLane() const {
    if (myEndPosition != INVALID_DOUBLE) {
        // get lane final and shape length
        const double laneLength = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
        // get endPosition
        double fixedPos = myEndPosition;
        // adjust fixedPos
        if (fixedPos < 0) {
            fixedPos += laneLength;
        }
        fixedPos *= getParentLanes().front()->getLengthGeometryFactor();
        // return depending of fixedPos
        if (fixedPos < POSITION_EPS) {
            return POSITION_EPS;
        } else if (fixedPos > getParentLanes().front()->getLaneShapeLength()) {
            return getParentLanes().front()->getLaneShapeLength();
        } else {
            return fixedPos;
        }
    } else {
        return getParentLanes().front()->getLaneShapeLength();
    }
}


void
GNEStoppingPlace::setMoveShape(const GNEMoveResult& moveResult) {
    if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST) {
        // change only start position
        myStartPosition = moveResult.newFirstPos;
        // adjust startPos
        if (myStartPosition > (getAttributeDouble(SUMO_ATTR_ENDPOS) - POSITION_EPS)) {
            myStartPosition = (getAttributeDouble(SUMO_ATTR_ENDPOS) - POSITION_EPS);
        }
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST) {
        // change only end position
        myEndPosition = moveResult.newFirstPos;
        // adjust endPos
        if (myEndPosition < (getAttributeDouble(SUMO_ATTR_STARTPOS) + POSITION_EPS)) {
            myEndPosition = (getAttributeDouble(SUMO_ATTR_STARTPOS) + POSITION_EPS);
        }
    } else {
        // change both position
        myStartPosition = moveResult.newFirstPos;
        myEndPosition = moveResult.newLastPos;
        // set lateral offset
        myMoveElementLateralOffset = moveResult.firstLaneOffset;
    }
    // update geometry
    updateGeometry();
}


void
GNEStoppingPlace::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // begin change attribute
    undoList->begin(this, "position of " + getTagStr());
    // set attributes depending of operation type
    if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST) {
        // set only start position
        setAttribute(SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos), undoList);
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST) {
        // set only end position
        setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newFirstPos), undoList);
    } else {
        // set both
        setAttribute(SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos), undoList);
        setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newLastPos), undoList);
        // check if lane has to be changed
        if (moveResult.newFirstLane) {
            // set new lane
            setAttribute(SUMO_ATTR_LANE, moveResult.newFirstLane->getID(), undoList);
        }
    }
    // end change attribute
    undoList->end();
}


void
GNEStoppingPlace::adjustLength(const double newLength, GNEUndoList* undoList) {
    const auto laneLength = getParentLanes().front()->getLaneShapeLength();
    auto newStartPos = myStartPosition;
    auto newEndPos = myEndPosition;
    if ((newStartPos != INVALID_DOUBLE) && (newEndPos != INVALID_DOUBLE)) {
        // get middle lengths
        const auto middleLength = (newEndPos - newStartPos) * 0.5;
        const auto middleNewLength = newLength * 0.5;
        // set new start and end positions
        newStartPos -= (middleNewLength - middleLength);
        newEndPos += (middleNewLength - middleLength);
        // now adjust both
        if (newStartPos < 0) {
            newEndPos += (newStartPos * -1);
            newStartPos = 0;
            if ((newStartPos < 0) && (newEndPos > laneLength)) {
                newStartPos = 0;
                newEndPos = laneLength;
            }
        } else if (newEndPos > laneLength) {
            newStartPos -= (newEndPos - laneLength);
            newStartPos = laneLength;
            if ((newStartPos < 0) && (newEndPos > laneLength)) {
                newStartPos = 0;
                newEndPos = laneLength;
            }
        }
        // set new start and end positions
        undoList->begin(this, TLF(" %'s length", myTagProperty->getTagStr()));
        GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_STARTPOS, toString(newStartPos), undoList);
        GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_ENDPOS, toString(newEndPos), undoList);
        undoList->end();
    } else if (newStartPos != INVALID_DOUBLE) {
        newStartPos = laneLength - newLength;
        if (newStartPos < 0) {
            newStartPos = 0;
        }
        undoList->begin(this, TLF(" %'s length", myTagProperty->getTagStr()));
        GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_STARTPOS, toString(newStartPos), undoList);
        undoList->end();
    } else if (newEndPos != INVALID_DOUBLE) {
        newEndPos = newLength;
        if (newEndPos > laneLength) {
            newEndPos = laneLength;
        }
        undoList->begin(this, TLF(" %'s length", myTagProperty->getTagStr()));
        GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_ENDPOS, toString(newEndPos), undoList);
        undoList->end();
    }
}

/****************************************************************************/
