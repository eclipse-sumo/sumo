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
/// @file    GNEChargingStation.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// A class for visualizing chargingStation geometry (adapted from GUILaneWrapper)
/****************************************************************************/
#include <config.h>

#include <foreign/fontstash/fontstash.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMORouteHandler.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>

#include "GNEChargingStation.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChargingStation::GNEChargingStation(GNENet* net) :
    GNEStoppingPlace("", net, GLO_CHARGING_STATION, SUMO_TAG_CHARGING_STATION, GUIIconSubSys::getIcon(GUIIcon::CHARGINGSTATION),
                     nullptr, 0, 0, "", false, Parameterised::Map()) {
    // reset default values
    resetDefaultValues();
}


GNEChargingStation::GNEChargingStation(const std::string& id, GNELane* lane, GNENet* net, const double startPos, const double endPos,
                                       const std::string& name, double chargingPower, double efficiency, bool chargeInTransit, SUMOTime chargeDelay,
                                       const std::string& chargeType, const SUMOTime waitingTime, bool friendlyPosition, const Parameterised::Map& parameters) :
    GNEStoppingPlace(id, net, GLO_CHARGING_STATION, SUMO_TAG_CHARGING_STATION, GUIIconSubSys::getIcon(GUIIcon::CHARGINGSTATION),
                     lane, startPos, endPos, name, friendlyPosition, parameters),
    myChargingPower(chargingPower),
    myEfficiency(efficiency),
    myChargeInTransit(chargeInTransit),
    myChargeDelay(chargeDelay),
    myChargeType(chargeType),
    myWaitingTime(waitingTime) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEChargingStation::~GNEChargingStation() {}


void
GNEChargingStation::writeAdditional(OutputDevice& device) const {
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
    if (getAttribute(SUMO_ATTR_CHARGINGPOWER) != myTagProperty.getDefaultValue(SUMO_ATTR_CHARGINGPOWER)) {
        device.writeAttr(SUMO_ATTR_CHARGINGPOWER, toString(myChargingPower));
    }
    if (getAttribute(SUMO_ATTR_EFFICIENCY) != myTagProperty.getDefaultValue(SUMO_ATTR_EFFICIENCY)) {
        device.writeAttr(SUMO_ATTR_EFFICIENCY, myEfficiency);
    }
    if (getAttribute(SUMO_ATTR_CHARGEINTRANSIT) != myTagProperty.getDefaultValue(SUMO_ATTR_CHARGEINTRANSIT)) {
        device.writeAttr(SUMO_ATTR_CHARGEINTRANSIT, myChargeInTransit);
    }
    if (getAttribute(SUMO_ATTR_CHARGEDELAY) != myTagProperty.getDefaultValue(SUMO_ATTR_CHARGEDELAY)) {
        device.writeAttr(SUMO_ATTR_CHARGEDELAY, myChargeDelay);
    }
    if (getAttribute(SUMO_ATTR_CHARGETYPE) != myTagProperty.getDefaultValue(SUMO_ATTR_CHARGETYPE)) {
        device.writeAttr(SUMO_ATTR_CHARGETYPE, myChargeType);
    }
    if (string2time(myTagProperty.getDefaultValue(SUMO_ATTR_WAITINGTIME)) != myWaitingTime) {
        device.writeAttr(SUMO_ATTR_WAITINGTIME, time2string(myWaitingTime));
    }
    if (myTagProperty.getDefaultValue(SUMO_ATTR_PARKING_AREA) != myTagProperty.getDefaultValue(SUMO_ATTR_PARKING_AREA)) {
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
    mySignPos = tmpShape.getLineCenter();
}


void
GNEChargingStation::drawGL(const GUIVisualizationSettings& s) const {
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // Obtain exaggeration of the draw
        const double chargingStationExaggeration = getExaggeration(s);
        // check if draw moving geometry points
        const bool movingGeometryPoints = drawMovingGeometryPoints(false);
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
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_CHARGING_STATION);
            // set base color
            GLHelper::setColor(baseColor);
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GUIGeometry::drawGeometry(d, myAdditionalGeometry, s.stoppingPlaceSettings.chargingStationWidth * MIN2(1.0, chargingStationExaggeration));
            // draw charging power and efficiency
            drawLines(d, {toString(myChargingPower)}, baseColor);
            // draw sign
            drawSign(d, chargingStationExaggeration, baseColor, signColor, "C");
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
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), myAdditionalGeometry.getShape().getCentroid(), chargingStationExaggeration);
            // Draw additional ID
            drawAdditionalID(s);
            // draw additional name
            drawAdditionalName(s);
            // draw dotted contour
            myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
            myAdditionalContourAuxiliary.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
            // draw dotted contours for geometry points
            myAdditionalContour.drawDottedContourGeometryPoints(s, d, this, myAdditionalGeometry.getShape(), s.neteditSizeSettings.additionalGeometryPointRadius,
                    1, s.dottedContourSettings.segmentWidthSmall);
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
GNEChargingStation::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FRIENDLY_POS:
        case SUMO_ATTR_CHARGINGPOWER:
        case SUMO_ATTR_EFFICIENCY:
        case SUMO_ATTR_CHARGEINTRANSIT:
        case SUMO_ATTR_CHARGEDELAY:
        case SUMO_ATTR_CHARGETYPE:
        case SUMO_ATTR_WAITINGTIME:
        case SUMO_ATTR_PARKING_AREA:
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
GNEChargingStation::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
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
            const auto validValues = myTagProperty.getAttributeProperties(key).getDiscreteValues();
            return std::find(validValues.begin(), validValues.end(), value) != validValues.end();
        }
        case SUMO_ATTR_WAITINGTIME:
            return canParse<SUMOTime>(value) && parse<SUMOTime>(value) >= 0;
        case SUMO_ATTR_PARKING_AREA:
            return isValidAdditionalID(value);
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
GNEChargingStation::setAttribute(SumoXMLAttr key, const std::string& value) {
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
            throw InvalidArgument(getTagStr() + "attribute '" + toString(key) + "' not allowed");
    }
}


/****************************************************************************/
