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
/// @file    GNEChargingStation.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// A class for visualizing chargingStation geometry (adapted from GUILaneWrapper)
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/options/OptionsCont.h>

#include "GNEChargingStation.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChargingStation::GNEChargingStation(GNENet* net) :
    GNEStoppingPlace(net, SUMO_TAG_CHARGING_STATION) {
}


GNEChargingStation::GNEChargingStation(const std::string& id, GNENet* net, const std::string& filename, GNELane* lane,
                                       const double startPos, const double endPos, const std::string& name, const double chargingPower,
                                       const double efficiency, const bool chargeInTransit, const SUMOTime chargeDelay,
                                       const std::string& chargeType, const SUMOTime waitingTime, const std::string& parkingAreaID,
                                       const bool friendlyPosition, const Parameterised::Map& parameters) :
    GNEStoppingPlace(id, net, filename, SUMO_TAG_CHARGING_STATION, lane, startPos, endPos, name, friendlyPosition, RGBColor::INVISIBLE, 0, parameters),
    myChargingPower(chargingPower),
    myEfficiency(efficiency),
    myChargeInTransit(chargeInTransit),
    myChargeDelay(chargeDelay),
    myChargeType(chargeType),
    myWaitingTime(waitingTime),
    myParkingAreaID(parkingAreaID) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEChargingStation::~GNEChargingStation() {}


void
GNEChargingStation::writeAdditional(OutputDevice& device) const {
    device.openTag(getTagProperty()->getTag());
    // write common attributes
    writeStoppingPlaceAttributes(device);
    // write specific attributes
    if (myChargingPower != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_CHARGINGPOWER)) {
        device.writeAttr(SUMO_ATTR_CHARGINGPOWER, toString(myChargingPower));
    }
    if (myEfficiency != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_EFFICIENCY)) {
        device.writeAttr(SUMO_ATTR_EFFICIENCY, myEfficiency);
    }
    if (myChargeInTransit != myTagProperty->getDefaultBoolValue(SUMO_ATTR_CHARGEINTRANSIT)) {
        device.writeAttr(SUMO_ATTR_CHARGEINTRANSIT, myChargeInTransit);
    }
    if (myChargeDelay != myTagProperty->getDefaultTimeValue(SUMO_ATTR_CHARGEDELAY)) {
        device.writeAttr(SUMO_ATTR_CHARGEDELAY, time2string(myChargeDelay));
    }
    if (myChargeType != myTagProperty->getDefaultStringValue(SUMO_ATTR_CHARGETYPE)) {
        device.writeAttr(SUMO_ATTR_CHARGETYPE, myChargeType);
    }
    if (myWaitingTime != myTagProperty->getDefaultTimeValue(SUMO_ATTR_WAITINGTIME)) {
        device.writeAttr(SUMO_ATTR_WAITINGTIME, time2string(myWaitingTime));
    }
    if (myParkingAreaID != myTagProperty->getDefaultStringValue(SUMO_ATTR_PARKING_AREA)) {
        device.writeAttr(SUMO_ATTR_PARKING_AREA, myParkingAreaID);
    }
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


void
GNEChargingStation::updateGeometry() {
    // Get value of option "lefthand"
    const double offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Update common geometry of stopping place
    setStoppingPlaceGeometry(0);

    // Obtain a copy of the shape
    PositionVector tmpShape = myAdditionalGeometry.getShape();

    // Move shape to side
    tmpShape.move2side(myNet->getViewNet()->getVisualisationSettings().stoppingPlaceSettings.stoppingPlaceSignOffset * offsetSign);

    // Get position of the sign
    mySymbolPosition = tmpShape.getLineCenter();
}


void
GNEChargingStation::drawGL(const GUIVisualizationSettings& s) const {
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // Obtain exaggeration of the draw
        const double chargingStationExaggeration = getExaggeration(s);
        // check if draw moving geometry points
        const bool movingGeometryPoints = drawMovingGeometryPoints();
        // get detail level
        const auto d = s.getDetailLevel(chargingStationExaggeration);
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
            } else {
                baseColor = s.colorSettings.chargingStationColor;
                signColor = s.colorSettings.chargingStationColorSign;
            }
            // draw parent and child lines
            drawParentChildLines(s, s.additionalSettings.connectionColor);
            // Add a layer matrix
            GLHelper::pushMatrix();
            // translate to front
            drawInLayer(GLO_CHARGING_STATION);
            // set base color
            GLHelper::setColor(baseColor);
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GUIGeometry::drawGeometry(d, myAdditionalGeometry, s.stoppingPlaceSettings.chargingStationWidth * MIN2(1.0, chargingStationExaggeration));
            // draw charging power and efficiency
            drawLines(d, {toString(myChargingPower)}, baseColor);
            // draw sign
            drawSign(s, d, chargingStationExaggeration, baseColor, signColor, "C");
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
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), myAdditionalGeometry.getShape().getCentroid(), chargingStationExaggeration);
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
        calculateStoppingPlaceContour(s, d, s.stoppingPlaceSettings.chargingStationWidth, chargingStationExaggeration, movingGeometryPoints);
    }
}


std::string
GNEChargingStation::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_CHARGINGPOWER:
            return toString(myChargingPower);
        case SUMO_ATTR_EFFICIENCY:
            return toString(myEfficiency);
        case SUMO_ATTR_CHARGEINTRANSIT:
            return toString(myChargeInTransit);
        case SUMO_ATTR_CHARGEDELAY:
            return time2string(myChargeDelay);
        case SUMO_ATTR_CHARGETYPE:
            return myChargeType;
        case SUMO_ATTR_WAITINGTIME:
            return time2string(myWaitingTime);
        case SUMO_ATTR_PARKING_AREA:
            return myParkingAreaID;
        default:
            return getStoppingPlaceAttribute(key);
    }
}


double
GNEChargingStation::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_CHARGINGPOWER:
            return myChargingPower;
        case SUMO_ATTR_EFFICIENCY:
            return myEfficiency;
        default:
            return getStoppingPlaceAttributeDouble(key);
    }
}


void
GNEChargingStation::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_CHARGINGPOWER:
        case SUMO_ATTR_EFFICIENCY:
        case SUMO_ATTR_CHARGEINTRANSIT:
        case SUMO_ATTR_CHARGEDELAY:
        case SUMO_ATTR_CHARGETYPE:
        case SUMO_ATTR_WAITINGTIME:
        case SUMO_ATTR_PARKING_AREA:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setStoppingPlaceAttribute(key, value, undoList);
            break;
    }
}


bool
GNEChargingStation::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_CHARGINGPOWER:
            return (canParse<double>(value) && parse<double>(value) >= 0);
        case SUMO_ATTR_EFFICIENCY:
            if (canParse<double>(value)) {
                const double efficiency = parse<double>(value);
                return (efficiency >= 0) && (efficiency <= 1);
            } else {
                return false;
            }
        case SUMO_ATTR_CHARGEINTRANSIT:
            return canParse<bool>(value);
        case SUMO_ATTR_CHARGEDELAY:
            return canParse<SUMOTime>(value) && parse<SUMOTime>(value) >= 0;
        case SUMO_ATTR_CHARGETYPE: {
            return SUMOXMLDefinitions::ChargeTypes.hasString(value);
        }
        case SUMO_ATTR_WAITINGTIME:
            return canParse<SUMOTime>(value) && parse<SUMOTime>(value) >= 0;
        case SUMO_ATTR_PARKING_AREA:
            return isValidAdditionalID(value);
        default:
            return isStoppingPlaceValid(key, value);
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEChargingStation::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_CHARGINGPOWER:
            myChargingPower = parse<double>(value);
            break;
        case SUMO_ATTR_EFFICIENCY:
            myEfficiency = parse<double>(value);
            break;
        case SUMO_ATTR_CHARGEINTRANSIT:
            myChargeInTransit = parse<bool>(value);
            break;
        case SUMO_ATTR_CHARGEDELAY:
            myChargeDelay = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_CHARGETYPE:
            myChargeType = value;
            break;
        case SUMO_ATTR_WAITINGTIME:
            myWaitingTime = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_PARKING_AREA:
            myParkingAreaID = value;
            break;
        default:
            setStoppingPlaceAttribute(key, value);
            break;
    }
}

/****************************************************************************/
