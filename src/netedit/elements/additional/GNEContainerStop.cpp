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
/// @file    GNEContainerStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
///
// A lane area vehicles can halt at (GNE version)
/****************************************************************************/
#include <foreign/fontstash/fontstash.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/vehicle/SUMORouteHandler.h>

#include "GNEContainerStop.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEContainerStop::GNEContainerStop(const std::string& id, GNELane* lane, GNENet* net, const double startPos, const double endPos,
                                   const std::string& name, const std::vector<std::string>& lines, int containerCapacity, double parkingLength, const RGBColor& color,
                                   bool friendlyPosition, const std::map<std::string, std::string>& parameters) :
    GNEStoppingPlace(id, net, GLO_CONTAINER_STOP, SUMO_TAG_CONTAINER_STOP, lane, startPos, endPos, name, friendlyPosition, parameters),
    myLines(lines),
    myContainerCapacity(containerCapacity),
    myParkingLength(parkingLength),
    myColor(color) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEContainerStop::~GNEContainerStop() {}


void
GNEContainerStop::writeAdditional(OutputDevice& device) const {
    // use write additional of gneAdditional
    GNEAdditional::writeAdditional(device);
}


void
GNEContainerStop::updateGeometry() {
    // Get value of option "lefthand"
    const double offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Update common geometry of stopping place
    setStoppingPlaceGeometry(getParentLanes().front()->getParentEdge()->getNBEdge()->getLaneWidth(getParentLanes().front()->getIndex()) / 2);

    // Obtain a copy of the shape
    PositionVector tmpShape = myAdditionalGeometry.getShape();

    // Move shape to side
    tmpShape.move2side(myNet->getViewNet()->getVisualisationSettings().stoppingPlaceSettings.stoppingPlaceSignOffset * offsetSign);

    // Get position of the sign
    mySignPos = tmpShape.getLineCenter();
}


void
GNEContainerStop::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double containerStopExaggeration = s.addSize.getExaggeration(s, this);
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // check exaggeration
        if (s.drawAdditionals(containerStopExaggeration)) {
            // declare colors
            RGBColor baseColor, signColor;
            // set colors
            if (mySpecialColor) {
                baseColor = *mySpecialColor;
                signColor = baseColor.changedBrightness(-32);
            } else if (drawUsingSelectColor()) {
                baseColor = s.colorSettings.selectedAdditionalColor;
                signColor = baseColor.changedBrightness(-32);
            } else {
                baseColor = myColor;
                signColor = s.stoppingPlaceSettings.containerStopColorSign;
            }
            // Start drawing adding an gl identificator
            GLHelper::pushName(getGlID());
            // Add a draw matrix
            GLHelper::pushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_CONTAINER_STOP);
            // set base color
            GLHelper::setColor(baseColor);
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GNEGeometry::drawGeometry(myNet->getViewNet(), myAdditionalGeometry, s.stoppingPlaceSettings.containerStopWidth * containerStopExaggeration);
            // draw detail
            if (s.drawDetail(s.detailSettings.stoppingPlaceDetails, containerStopExaggeration)) {
                // draw lines
                drawLines(s, myLines, baseColor);
                // draw sign
                drawSign(s, containerStopExaggeration, baseColor, signColor, "C");
                // draw lock icon
                GNEViewNetHelper::LockIcon::drawLockIcon(getType(), this, myAdditionalGeometry.getShape().getCentroid(), containerStopExaggeration);
            }
            // pop draw matrix
            GLHelper::popMatrix();
            // Pop name
            GLHelper::popName();
            // check if dotted contours has to be drawn
            if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GNEGeometry::drawDottedContourShape(GNEGeometry::DottedContourType::INSPECT, s, myAdditionalGeometry.getShape(), s.stoppingPlaceSettings.containerStopWidth, containerStopExaggeration);
            }
            if (s.drawDottedContour() || myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                GNEGeometry::drawDottedContourShape(GNEGeometry::DottedContourType::FRONT, s, myAdditionalGeometry.getShape(), s.stoppingPlaceSettings.containerStopWidth, containerStopExaggeration);
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
GNEContainerStop::getAttribute(SumoXMLAttr key) const {
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
        case SUMO_ATTR_CONTAINER_CAPACITY:
            return toString(myContainerCapacity);
        case SUMO_ATTR_PARKING_LENGTH:
            return toString(myParkingLength);
        case SUMO_ATTR_COLOR:
            if (myColor == myNet->getViewNet()->getVisualisationSettings().stoppingPlaceSettings.containerStopColor) {
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
GNEContainerStop::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
        case SUMO_ATTR_CONTAINER_CAPACITY:
        case SUMO_ATTR_PARKING_LENGTH:
        case SUMO_ATTR_COLOR:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEContainerStop::isValid(SumoXMLAttr key, const std::string& value) {
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
        case SUMO_ATTR_CONTAINER_CAPACITY:
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
GNEContainerStop::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myNet->getAttributeCarriers()->updateID(this, value);
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
        case SUMO_ATTR_CONTAINER_CAPACITY:
            myContainerCapacity = GNEAttributeCarrier::parse<int>(value);
            break;
        case SUMO_ATTR_PARKING_LENGTH:
            myParkingLength = GNEAttributeCarrier::parse<double>(value);
            break;
        case SUMO_ATTR_COLOR:
            if (value.empty()) {
                myColor = myNet->getViewNet()->getVisualisationSettings().stoppingPlaceSettings.busStopColor;
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


/****************************************************************************/
