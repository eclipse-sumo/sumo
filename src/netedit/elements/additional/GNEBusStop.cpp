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
/// @file    GNEBusStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// A lane area vehicles can halt at (GNE version)
/****************************************************************************/
#include <config.h>

#include <foreign/fontstash/fontstash.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/vehicle/SUMORouteHandler.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEBusStop.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEBusStop*
GNEBusStop::buildBusStop(GNENet* net) {
    return new GNEBusStop(SUMO_TAG_BUS_STOP, GLO_BUS_STOP, GUIIcon::BUSSTOP, net);
}


GNEBusStop*
GNEBusStop::buildTrainStop(GNENet* net) {
    return new GNEBusStop(SUMO_TAG_TRAIN_STOP, GLO_TRAIN_STOP, GUIIcon::TRAINSTOP, net);
}


GNEBusStop*
GNEBusStop::buildBusStop(const std::string& id, GNELane* lane, GNENet* net,
                         const double startPos, const double endPos, const std::string& name, const std::vector<std::string>& lines,
                         int personCapacity, double parkingLength, const RGBColor& color, bool friendlyPosition,
                         const Parameterised::Map& parameters) {
    return new GNEBusStop(SUMO_TAG_BUS_STOP, GLO_BUS_STOP, GUIIcon::BUSSTOP, id, lane, net, startPos, endPos, name, lines,
                          personCapacity, parkingLength, color, friendlyPosition, parameters);
}


GNEBusStop*
GNEBusStop::buildTrainStop(const std::string& id, GNELane* lane, GNENet* net,
                           const double startPos, const double endPos, const std::string& name, const std::vector<std::string>& lines,
                           int personCapacity, double parkingLength, const RGBColor& color, bool friendlyPosition,
                           const Parameterised::Map& parameters) {
    return new GNEBusStop(SUMO_TAG_TRAIN_STOP, GLO_TRAIN_STOP, GUIIcon::TRAINSTOP, id, lane, net, startPos, endPos, name, lines,
                          personCapacity, parkingLength, color, friendlyPosition, parameters);
}


GNEBusStop::~GNEBusStop() {}


void
GNEBusStop::writeAdditional(OutputDevice& device) const {
    device.openTag(getTagProperty().getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    if (!myAdditionalName.empty()) {
        device.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(myAdditionalName));
    }
    device.writeAttr(SUMO_ATTR_LANE, getParentLanes().front()->getID());
    if (myStartPosition != INVALID_DOUBLE) {
        device.writeAttr(SUMO_ATTR_STARTPOS, myStartPosition);
    }
    if (myEndPosition != INVALID_DOUBLE) {
        device.writeAttr(SUMO_ATTR_ENDPOS, myEndPosition);
    }
    if (myFriendlyPosition) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, "true");
    }
    if (getAttribute(SUMO_ATTR_LINES) != myTagProperty.getDefaultValue(SUMO_ATTR_LINES)) {
        device.writeAttr(SUMO_ATTR_LINES, toString(myLines));
    }
    if (getAttribute(SUMO_ATTR_PERSON_CAPACITY) != myTagProperty.getDefaultValue(SUMO_ATTR_PERSON_CAPACITY)) {
        device.writeAttr(SUMO_ATTR_PERSON_CAPACITY, myPersonCapacity);
    }
    if (getAttribute(SUMO_ATTR_PARKING_LENGTH) != myTagProperty.getDefaultValue(SUMO_ATTR_PARKING_LENGTH)) {
        device.writeAttr(SUMO_ATTR_PARKING_LENGTH, myParkingLength);
    }
    if (getAttribute(SUMO_ATTR_COLOR).size() > 0) {
        device.writeAttr(SUMO_ATTR_COLOR, myColor);
    }
    // write all access
    for (const auto& access : getChildAdditionals()) {
        access->writeAdditional(device);
    }
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


void
GNEBusStop::updateGeometry() {
    // Get value of option "lefthand"
    double offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;
    // Update common geometry of stopping place
    setStoppingPlaceGeometry(getParentLanes().front()->getParentEdge()->getNBEdge()->getLaneWidth(getParentLanes().front()->getIndex()) * 0.5);
    // Obtain a copy of the shape
    PositionVector tmpShape = myAdditionalGeometry.getShape();
    // Move shape to side
    tmpShape.move2side(myNet->getViewNet()->getVisualisationSettings().stoppingPlaceSettings.stoppingPlaceSignOffset * offsetSign);
    // Get position of the sign
    mySymbolPosition = tmpShape.getLineCenter();
    // update demand element children
    for (const auto& demandElement : getChildDemandElements()) {
        demandElement->updateGeometry();
    }
}


void
GNEBusStop::drawGL(const GUIVisualizationSettings& s) const {
    // check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // Obtain exaggeration of the draw
        const double busStopExaggeration = getExaggeration(s);
        // check if draw moving geometry points
        const bool movingGeometryPoints = drawMovingGeometryPoints(false);
        // get width
        const double stopWidth = (myTagProperty.getTag() == SUMO_TAG_BUS_STOP) ? s.stoppingPlaceSettings.busStopWidth : s.stoppingPlaceSettings.trainStopWidth;
        // get detail level
        const auto d = s.getDetailLevel(busStopExaggeration);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // declare colors
            RGBColor baseColor, signColor;
            // set colors
            if (mySpecialColor) {
                baseColor = *mySpecialColor;
                signColor = baseColor.changedBrightness(-32);
            } else if (drawUsingSelectColor()) {
                baseColor = s.colorSettings.selectedAdditionalColor;
                signColor = baseColor.changedBrightness(-32);
            } else if (myColor != RGBColor::INVISIBLE) {
                baseColor = myColor;
                signColor = s.colorSettings.busStopColorSign;
            } else if (myTagProperty.getTag() == SUMO_TAG_TRAIN_STOP) {
                baseColor = s.colorSettings.trainStopColor;
                signColor = s.colorSettings.trainStopColorSign;
            } else {
                baseColor = s.colorSettings.busStopColor;
                signColor = s.colorSettings.busStopColorSign;
            }
            // draw parent and child lines
            drawParentChildLines(s, baseColor);
            // Add layer matrix
            GLHelper::pushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_BUS_STOP);
            // set base color
            GLHelper::setColor(baseColor);
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GUIGeometry::drawGeometry(d, myAdditionalGeometry, stopWidth * MIN2(1.0, busStopExaggeration));
            // draw lines
            drawLines(d, myLines, baseColor);
            // draw sign
            drawSign(s, d, busStopExaggeration, baseColor, signColor, (myTagProperty.getTag() == SUMO_TAG_BUS_STOP) ? "H" : "T");
            // draw geometry points
            if (movingGeometryPoints && (myStartPosition != INVALID_DOUBLE)) {
                drawLeftGeometryPoint(s, d, myAdditionalGeometry.getShape().front(), myAdditionalGeometry.getShapeRotations().front(), baseColor);
            }
            if (movingGeometryPoints && (myEndPosition != INVALID_DOUBLE)) {
                drawRightGeometryPoint(s, d, myAdditionalGeometry.getShape().back(), myAdditionalGeometry.getShapeRotations().back(), baseColor);
            }
            // pop layer matrix
            GLHelper::popMatrix();
            // draw lock icon
            if (myTagProperty.getTag() == SUMO_TAG_BUS_STOP) {
                GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), myAdditionalGeometry.getShape().getCentroid(), busStopExaggeration, 0.5);
            } else {
                GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), myAdditionalGeometry.getShape().getCentroid(), busStopExaggeration, 0.25);
            }
            // Draw additional ID
            drawAdditionalID(s);
            // draw additional name
            drawAdditionalName(s);
            // draw dotted contours
            if (movingGeometryPoints) {
                myAdditionalContour.drawDottedContourGeometryPoints(s, d, this, myAdditionalGeometry.getShape(), s.neteditSizeSettings.additionalGeometryPointRadius,
                        1, s.dottedContourSettings.segmentWidthSmall);
            } else {
                myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
                mySymbolContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
            }
        }
        // draw demand element children
        drawDemandElementChildren(s);
        // calculate contours
        calculateStoppingPlaceContour(s, d, stopWidth, busStopExaggeration, movingGeometryPoints);
    }
}


std::string
GNEBusStop::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANE:
            return getParentLanes().front()->getID();
        case SUMO_ATTR_STARTPOS:
            if (myStartPosition != INVALID_DOUBLE) {
                return toString(myStartPosition);
            } else {
                return "";
            }
        case SUMO_ATTR_ENDPOS:
            if (myEndPosition != INVALID_DOUBLE) {
                return toString(myEndPosition);
            } else {
                return "";
            }
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case SUMO_ATTR_LINES:
            return joinToString(myLines, " ");
        case SUMO_ATTR_PERSON_CAPACITY:
            return toString(myPersonCapacity);
        case SUMO_ATTR_PARKING_LENGTH:
            return toString(myParkingLength);
        case SUMO_ATTR_COLOR:
            if (myColor == RGBColor::INVISIBLE) {
                return "";
            } else {
                return toString(myColor);
            }
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


void
GNEBusStop::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FRIENDLY_POS:
        case SUMO_ATTR_LINES:
        case SUMO_ATTR_PERSON_CAPACITY:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_PARKING_LENGTH:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
        case GNE_ATTR_SHIFTLANEINDEX:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEBusStop::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(NamespaceIDs::busStops, value);
        case SUMO_ATTR_LANE:
            if (myNet->getAttributeCarriers()->retrieveLane(value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_STARTPOS:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return SUMORouteHandler::isStopPosValid(parse<double>(value), getAttributeDouble(SUMO_ATTR_ENDPOS), getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), POSITION_EPS, myFriendlyPosition);
            } else {
                return false;
            }
        case SUMO_ATTR_ENDPOS:
            if (value.empty()) {
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
        case SUMO_ATTR_LINES:
            return canParse<std::vector<std::string> >(value);
        case SUMO_ATTR_PERSON_CAPACITY:
            return canParse<int>(value) && (parse<int>(value) > 0 || parse<int>(value) == -1);
        case SUMO_ATTR_PARKING_LENGTH:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_COLOR:
            if (value.empty()) {
                return true;
            } else {
                return canParse<RGBColor>(value);
            }
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEBusStop::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            replaceAdditionalParentLanes(value);
            break;
        case SUMO_ATTR_STARTPOS:
            if (value == "") {
                myStartPosition = INVALID_DOUBLE;
            } else {
                myStartPosition = parse<double>(value);
            }
            break;
        case SUMO_ATTR_ENDPOS:
            if (value == "") {
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
        case SUMO_ATTR_LINES:
            myLines = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_PERSON_CAPACITY:
            myPersonCapacity = GNEAttributeCarrier::parse<int>(value);
            break;
        case SUMO_ATTR_PARKING_LENGTH:
            myParkingLength = GNEAttributeCarrier::parse<double>(value);
            break;
        case SUMO_ATTR_COLOR:
            if (value.empty()) {
                myColor = RGBColor::INVISIBLE;
            } else {
                myColor = GNEAttributeCarrier::parse<RGBColor>(value);
            }
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


GNEBusStop::GNEBusStop(SumoXMLTag tag, GUIGlObjectType type, GUIIcon icon, GNENet* net) :
    GNEStoppingPlace("", net, type, tag, GUIIconSubSys::getIcon(icon), nullptr, 0, 0, "", false, Parameterised::Map()),
    myPersonCapacity(0),
    myParkingLength(0),
    myColor(RGBColor::BLACK) {
    // reset default values
    resetDefaultValues();
}


GNEBusStop::GNEBusStop(SumoXMLTag tag, GUIGlObjectType type, GUIIcon icon, const std::string& id, GNELane* lane, GNENet* net,
                       const double startPos, const double endPos, const std::string& name, const std::vector<std::string>& lines,
                       int personCapacity, double parkingLength, const RGBColor& color, bool friendlyPosition, const Parameterised::Map& parameters) :
    GNEStoppingPlace(id, net, type, tag, GUIIconSubSys::getIcon(icon), lane, startPos, endPos, name, friendlyPosition, parameters),
    myLines(lines),
    myPersonCapacity(personCapacity),
    myParkingLength(parkingLength),
    myColor(color) {
}

/****************************************************************************/
