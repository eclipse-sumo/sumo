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
/// @file    GNEBusStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// A lane area vehicles can halt at (GNE version)
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/options/OptionsCont.h>

#include "GNEBusStop.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEBusStop*
GNEBusStop::buildBusStop(GNENet* net) {
    return new GNEBusStop(SUMO_TAG_BUS_STOP, net);
}


GNEBusStop*
GNEBusStop::buildTrainStop(GNENet* net) {
    return new GNEBusStop(SUMO_TAG_TRAIN_STOP, net);
}


GNEBusStop*
GNEBusStop::buildBusStop(const std::string& id, GNENet* net, const std::string& filename, GNELane* lane,
                         const double startPos, const double endPos, const std::string& name, const std::vector<std::string>& lines,
                         const int personCapacity, const double parkingLength, const RGBColor& color, const bool friendlyPosition,
                         const double angle, const Parameterised::Map& parameters) {
    return new GNEBusStop(SUMO_TAG_BUS_STOP, id, net, filename, lane, startPos, endPos, name, lines,
                          personCapacity, parkingLength, color, friendlyPosition, angle, parameters);
}


GNEBusStop*
GNEBusStop::buildTrainStop(const std::string& id, GNENet* net, const std::string& filename, GNELane* lane,
                           const double startPos, const double endPos, const std::string& name, const std::vector<std::string>& lines,
                           const int personCapacity, const double parkingLength, const RGBColor& color, const bool friendlyPosition,
                           const double angle, const Parameterised::Map& parameters) {
    return new GNEBusStop(SUMO_TAG_TRAIN_STOP, id, net, filename, lane, startPos, endPos, name, lines,
                          personCapacity, parkingLength, color, friendlyPosition, angle, parameters);
}


GNEBusStop::~GNEBusStop() {}


void
GNEBusStop::writeAdditional(OutputDevice& device) const {
    device.openTag(getTagProperty()->getTag());
    // write common attributes
    writeStoppingPlaceAttributes(device);
    // write specific attributes
    if (getAttribute(SUMO_ATTR_LINES) != myTagProperty->getDefaultStringValue(SUMO_ATTR_LINES)) {
        device.writeAttr(SUMO_ATTR_LINES, toString(myLines));
    }
    if (myPersonCapacity != myTagProperty->getDefaultIntValue(SUMO_ATTR_PERSON_CAPACITY)) {
        device.writeAttr(SUMO_ATTR_PERSON_CAPACITY, myPersonCapacity);
    }
    if (myParkingLength != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_PARKING_LENGTH)) {
        device.writeAttr(SUMO_ATTR_PARKING_LENGTH, myParkingLength);
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
        const bool movingGeometryPoints = drawMovingGeometryPoints();
        // get width
        const double stopWidth = (myTagProperty->getTag() == SUMO_TAG_BUS_STOP) ? s.stoppingPlaceSettings.busStopWidth : s.stoppingPlaceSettings.trainStopWidth;
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
            } else if (myTagProperty->getTag() == SUMO_TAG_TRAIN_STOP) {
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
            drawInLayer(GLO_BUS_STOP);
            // set base color
            GLHelper::setColor(baseColor);
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GUIGeometry::drawGeometry(d, myAdditionalGeometry, stopWidth * MIN2(1.0, busStopExaggeration));
            // draw lines
            drawLines(d, myLines, baseColor);
            // draw sign
            drawSign(s, d, busStopExaggeration, baseColor, signColor, (myTagProperty->getTag() == SUMO_TAG_BUS_STOP) ? "H" : "T");
            // draw geometry points
            if (movingGeometryPoints && (myStartPosOverLane != INVALID_DOUBLE)) {
                drawLeftGeometryPoint(s, d, myAdditionalGeometry.getShape().front(), myAdditionalGeometry.getShapeRotations().front(), baseColor);
            }
            if (movingGeometryPoints && (myEndPosPosOverLane != INVALID_DOUBLE)) {
                drawRightGeometryPoint(s, d, myAdditionalGeometry.getShape().back(), myAdditionalGeometry.getShapeRotations().back(), baseColor);
            }
            // pop layer matrix
            GLHelper::popMatrix();
            // draw lock icon
            if (myTagProperty->getTag() == SUMO_TAG_BUS_STOP) {
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
        case SUMO_ATTR_LINES:
            return joinToString(myLines, " ");
        case SUMO_ATTR_PERSON_CAPACITY:
            return toString(myPersonCapacity);
        case SUMO_ATTR_PARKING_LENGTH:
            return toString(myParkingLength);
        default:
            return getStoppingPlaceAttribute(key);
    }
}


double
GNEBusStop::getAttributeDouble(SumoXMLAttr key) const {
    return getStoppingPlaceAttributeDouble(key);
}


void
GNEBusStop::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_LINES:
        case SUMO_ATTR_PERSON_CAPACITY:
        case SUMO_ATTR_PARKING_LENGTH:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setStoppingPlaceAttribute(key, value, undoList);
            break;
    }
}


bool
GNEBusStop::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_LINES:
            return true;
        case SUMO_ATTR_PERSON_CAPACITY:
            return canParse<int>(value) && (parse<int>(value) > 0 || parse<int>(value) == -1);
        case SUMO_ATTR_PARKING_LENGTH:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        default:
            return isStoppingPlaceValid(key, value);
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEBusStop::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_LINES:
            myLines = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_PERSON_CAPACITY:
            myPersonCapacity = GNEAttributeCarrier::parse<int>(value);
            break;
        case SUMO_ATTR_PARKING_LENGTH:
            myParkingLength = GNEAttributeCarrier::parse<double>(value);
            break;
        default:
            setStoppingPlaceAttribute(key, value);
            break;
    }
}


GNEBusStop::GNEBusStop(SumoXMLTag tag, GNENet* net) :
    GNEStoppingPlace(net, tag) {
}


GNEBusStop::GNEBusStop(SumoXMLTag tag, const std::string& id, GNENet* net, const std::string& filename,
                       GNELane* lane, const double startPos, const double endPos, const std::string& name,
                       const std::vector<std::string>& lines, const int personCapacity, const double parkingLength,
                       const RGBColor& color, const bool friendlyPosition, const double angle,
                       const Parameterised::Map& parameters) :
    GNEStoppingPlace(id, net, filename, tag, lane, startPos, endPos, name, friendlyPosition, color, angle, parameters),
    myLines(lines),
    myPersonCapacity(personCapacity),
    myParkingLength(parkingLength) {
}

/****************************************************************************/
