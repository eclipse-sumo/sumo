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
/// @file    GNEBusStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// A lane area vehicles can halt at (GNE version)
/****************************************************************************/
#include <foreign/fontstash/fontstash.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/vehicle/SUMORouteHandler.h>

#include "GNEBusStop.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEBusStop::GNEBusStop(SumoXMLTag tag, const std::string& id, GNELane* lane, GNENet* net, const double startPos, const double endPos,
                       const std::string& name, const std::vector<std::string>& lines, int personCapacity, double parkingLength, const RGBColor& color,
                       bool friendlyPosition, const std::map<std::string, std::string>& parameters) :
    GNEStoppingPlace(id, net, GLO_BUS_STOP, tag, lane, startPos, endPos, name, friendlyPosition, parameters),
    myLines(lines),
    myPersonCapacity(personCapacity),
    myParkingLength(parkingLength),
    myColor(color) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
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
    if (myLines.size() > 0) {
        device.writeAttr(SUMO_ATTR_LINES, toString(myLines));
    }
    if (myParkingLength > 0) {
        device.writeAttr(SUMO_ATTR_PARKING_LENGTH, myParkingLength);
    }
    if (myColor.isValid()) {
        device.writeAttr(SUMO_ATTR_COLOR, myColor);
    }
    // write all access
    for (const auto &access : getChildAdditionals()) {
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
    mySignPos = tmpShape.getLineCenter();
    // update demand element children
    for (const auto& demandElement : getChildDemandElements()) {
        demandElement->updateGeometry();
    }
}


void
GNEBusStop::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double busStopExaggeration = s.addSize.getExaggeration(s, this);
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // check exaggeration
        if (s.drawAdditionals(busStopExaggeration)) {
            // get width
            const double stopWidth = (myTagProperty.getTag() == SUMO_TAG_BUS_STOP) ? s.stoppingPlaceSettings.busStopWidth : s.stoppingPlaceSettings.trainStopWidth;
            // declare colors
            RGBColor baseColor, signColor;
            // set colors
            if (mySpecialColor) {
                baseColor = *mySpecialColor;
                signColor = baseColor.changedBrightness(-32);
            } else if (drawUsingSelectColor()) {
                baseColor = s.colorSettings.selectedAdditionalColor;
                signColor = baseColor.changedBrightness(-32);
            } else if (myColor.isValid()){
                baseColor = myColor;
                signColor = s.stoppingPlaceSettings.busStopColorSign;
            } else {
                baseColor = myNet->getViewNet()->getVisualisationSettings().stoppingPlaceSettings.busStopColor;
                signColor = s.stoppingPlaceSettings.busStopColorSign;
            }
            // Start drawing adding an gl identificator
            GLHelper::pushName(getGlID());
            // Add a draw matrix
            GLHelper::pushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_BUS_STOP);
            // set base color
            GLHelper::setColor(baseColor);
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GNEGeometry::drawGeometry(myNet->getViewNet(), myAdditionalGeometry, stopWidth * busStopExaggeration);
            // draw detail
            if (s.drawDetail(s.detailSettings.stoppingPlaceDetails, busStopExaggeration)) {
                // draw lines
                drawLines(s, myLines, baseColor);
                // draw sign
                drawSign(s, busStopExaggeration, baseColor, signColor, (myTagProperty.getTag() == SUMO_TAG_BUS_STOP) ? "H" : "T");
                // draw lock icon
                GNEViewNetHelper::LockIcon::drawLockIcon(getType(), this, myAdditionalGeometry.getShape().getCentroid(), busStopExaggeration, (myTagProperty.getTag() == SUMO_TAG_BUS_STOP) ? 0.5 : 0.25);
            }
            // pop draw matrix
            GLHelper::popMatrix();
            // Pop name
            GLHelper::popName();
            // draw connection betwen access
            drawConnectionAccess(s, baseColor);
            // check if dotted contours has to be drawn
            if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GNEGeometry::drawDottedContourShape(GNEGeometry::DottedContourType::INSPECT, s, myAdditionalGeometry.getShape(), stopWidth, busStopExaggeration);
            }
            if (s.drawDottedContour() || myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                GNEGeometry::drawDottedContourShape(GNEGeometry::DottedContourType::FRONT, s, myAdditionalGeometry.getShape(), stopWidth, busStopExaggeration);
            }
            // draw child demand elements
            for (const auto& demandElement : getChildDemandElements()) {
                if (!demandElement->getTagProperty().isPlacedInRTree()) {
                    demandElement->drawGL(s);
                }
            }
        }
        // Draw additional ID
        drawAdditionalID(s);
        // draw additional name
        drawAdditionalName(s);
    }
}


std::string
GNEBusStop::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
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
            if (!myColor.isValid()) {
                return "";
            } else {
                return toString(myColor);
            }
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEBusStop::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
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
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEBusStop::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_LANE:
            if (myNet->retrieveLane(value, false) != nullptr) {
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
            return Parameterised::areParametersValid(value);
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
            myNet->getAttributeCarriers()->updateID(this, value);
            // Change IDs of all access children
            for (const auto& access : getChildAdditionals()) {
                access->setMicrosimID(getID());
            }
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
                myColor.setValid(false);
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
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEBusStop::drawConnectionAccess(const GUIVisualizationSettings& s, const RGBColor& color) const {
    if (!s.drawForPositionSelection && !s.drawForRectangleSelection) {
        // Add a draw matrix for details
        GLHelper::pushMatrix();
        // move to GLO_BUS_STOP
        glTranslated(0, 0, GLO_BUS_STOP);
        // set color
        GLHelper::setColor(color);
        // draw lines between BusStops and Access
        for (const auto& access : getChildAdditionals()) {
            // get busStop center
            const Position busStopCenter = myAdditionalGeometry.getShape().getLineCenter();
            // get access center
            const Position accessCenter = access->getAdditionalGeometry().getShape().front();
            GLHelper::drawBoxLine(accessCenter,
                                  RAD2DEG(busStopCenter.angleTo2D(accessCenter)) - 90,
                                  busStopCenter.distanceTo2D(accessCenter), .05);
        }
        // pop draw matrix
        GLHelper::popMatrix();
    }
}

/****************************************************************************/
