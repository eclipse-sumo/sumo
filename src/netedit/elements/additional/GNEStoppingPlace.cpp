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
    GNEAdditional("", net, "", tag, ""),
    GNEMoveElementLaneDouble(this) {
}


GNEStoppingPlace::GNEStoppingPlace(const std::string& id, GNENet* net, const std::string& filename,
                                   SumoXMLTag tag, GNELane* lane, const double startPos, const double endPos,
                                   const std::string& name, bool friendlyPosition, const RGBColor& color,
                                   const double angle, const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, tag, name),
    GNEMoveElementLaneDouble(this, GNEMoveElementLaneDouble::PosAttributes::STARTPOS_ENDPOS, lane, startPos, endPos, friendlyPosition),
    Parameterised(parameters),
    myColor(color),
    myAngle(angle) {
}


GNEStoppingPlace::~GNEStoppingPlace() {}


GNEMoveElement*
GNEStoppingPlace::getMoveElement() {
    return this;
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
    // make a copy of start and end positions over lane
    double startPos = myStartPosOverLane;
    double endPos = myEndPosPosOverLane;
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
        if ((myStartPosOverLane != INVALID_DOUBLE) && (myEndPosPosOverLane != INVALID_DOUBLE)) {
            // calculate middle position
            const double middlePosition = ((myEndPosPosOverLane - myStartPosOverLane) / 2.0) + myStartPosOverLane;
            //  four cases:
            if (splitPosition < myStartPosOverLane) {
                // change lane
                setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
                // now adjust start and end position
                setAttribute(SUMO_ATTR_STARTPOS, toString(myStartPosOverLane - splitPosition), undoList);
                setAttribute(SUMO_ATTR_ENDPOS, toString(myEndPosPosOverLane - splitPosition), undoList);
            } else if ((splitPosition > myStartPosOverLane) && (splitPosition < middlePosition)) {
                // change lane
                setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
                // now adjust start and end position
                setAttribute(SUMO_ATTR_STARTPOS, "0", undoList);
                setAttribute(SUMO_ATTR_ENDPOS, toString(myEndPosPosOverLane - splitPosition), undoList);
            } else if ((splitPosition > middlePosition) && (splitPosition < myEndPosPosOverLane)) {
                // only adjust end position
                setAttribute(SUMO_ATTR_ENDPOS, toString(splitPosition), undoList);
            } else if ((splitPosition > myEndPosPosOverLane)) {
                // nothing to do
            }
        } else if ((myStartPosOverLane != INVALID_DOUBLE) && (splitPosition < myStartPosOverLane)) {
            // change lane
            setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
            // now adjust start position
            setAttribute(SUMO_ATTR_STARTPOS, toString(myEndPosPosOverLane - splitPosition), undoList);
        } else if ((myEndPosPosOverLane != INVALID_DOUBLE) && (splitPosition < myEndPosPosOverLane)) {
            // change lane
            setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
            // now adjust end position
            setAttribute(SUMO_ATTR_ENDPOS, toString(myEndPosPosOverLane - splitPosition), undoList);
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
    // write move atributes
    writeMoveAttributes(device);
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
            if (myStartPosOverLane != INVALID_DOUBLE) {
                return toString(myStartPosOverLane);
            } else {
                return LANE_START;
            }
        case SUMO_ATTR_ENDPOS:
            if (myEndPosPosOverLane != INVALID_DOUBLE) {
                return toString(myEndPosPosOverLane);
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
            } else if ((myStartPosOverLane != INVALID_DOUBLE) && (myEndPosPosOverLane != INVALID_DOUBLE)) {
                return toString(myEndPosPosOverLane - myStartPosOverLane);
            } else if (myStartPosOverLane != INVALID_DOUBLE) {
                return toString(getParentLanes().front()->getLaneShapeLength() - myStartPosOverLane);
            } else if (myEndPosPosOverLane != INVALID_DOUBLE) {
                return toString(myEndPosPosOverLane);
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
            if (myStartPosOverLane != INVALID_DOUBLE) {
                return myStartPosOverLane;
            } else {
                return 0;
            }
        case SUMO_ATTR_ENDPOS:
            if (myEndPosPosOverLane != INVALID_DOUBLE) {
                return myEndPosPosOverLane;
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
            adjustLaneMovableLength(parse<double>(value), undoList);
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
                myStartPosOverLane = INVALID_DOUBLE;
            } else {
                myStartPosOverLane = parse<double>(value);
            }
            break;
        case SUMO_ATTR_ENDPOS:
            if (value.empty() || (value == LANE_END)) {
                myEndPosPosOverLane = INVALID_DOUBLE;
            } else {
                myEndPosPosOverLane = parse<double>(value);
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
    myAdditionalGeometry.updateGeometry(laneShape, getStartFixedPositionOverLane(), getEndFixedPositionOverLane(), myMovingLateralOffset);
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
                return (myStartPosOverLane != INVALID_DOUBLE) && (myEndPosPosOverLane != INVALID_DOUBLE);
            }
        case GNE_ATTR_SIZE:
            if (isTemplate()) {
                return true;
            } else {
                return (myStartPosOverLane != INVALID_DOUBLE) || (myEndPosPosOverLane != INVALID_DOUBLE);
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
        if (myStartPosOverLane != INVALID_DOUBLE) {
            myAdditionalContour.calculateContourFirstGeometryPoint(s, d, this, myAdditionalGeometry.getShape(),
                    getType(), s.neteditSizeSettings.additionalGeometryPointRadius, 1);
        }
        if (movingGeometryPoints && (myEndPosPosOverLane != INVALID_DOUBLE)) {
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

/****************************************************************************/
