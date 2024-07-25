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
/// @file    GNEParkingArea.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
///
// A lane area vehicles can park at (GNE version)
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/vehicle/SUMORouteHandler.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>

#include "GNEParkingArea.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEParkingArea::GNEParkingArea(GNENet* net) :
    GNEStoppingPlace("", net, GLO_PARKING_AREA, SUMO_TAG_PARKING_AREA, GUIIconSubSys::getIcon(GUIIcon::PARKINGAREA),
                     nullptr, 0, 0, "", false, Parameterised::Map()),
    myRoadSideCapacity(0),
    myOnRoad(false),
    myWidth(0),
    myLength(0),
    myAngle(0),
    myLefthand(false) {
    // reset default values
    resetDefaultValues();
}


GNEParkingArea::GNEParkingArea(const std::string& id, GNELane* lane, GNENet* net, const double startPos, const double endPos,
                               const std::string& departPos, const std::string& name, const std::vector<std::string>& badges,
                               const bool friendlyPosition, const int roadSideCapacity, const bool onRoad, const double width,
                               const double length, const double angle, const bool lefthand, const Parameterised::Map& parameters) :
    GNEStoppingPlace(id, net, GLO_PARKING_AREA, SUMO_TAG_PARKING_AREA, GUIIconSubSys::getIcon(GUIIcon::PARKINGAREA),
                     lane, startPos, endPos, name, friendlyPosition, parameters),
    myDepartPos(departPos),
    myRoadSideCapacity(roadSideCapacity),
    myOnRoad(onRoad),
    myWidth(width),
    myLength(length),
    myAngle(angle),
    myLefthand(lefthand),
    myAcceptedBadges(badges) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEParkingArea::~GNEParkingArea() {}


void
GNEParkingArea::writeAdditional(OutputDevice& device) const {
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
    if (getAttribute(SUMO_ATTR_ROADSIDE_CAPACITY) != myTagProperty.getDefaultValue(SUMO_ATTR_ROADSIDE_CAPACITY)) {
        device.writeAttr(SUMO_ATTR_ROADSIDE_CAPACITY, toString(myRoadSideCapacity));
    }
    if (getAttribute(SUMO_ATTR_ONROAD) != myTagProperty.getDefaultValue(SUMO_ATTR_ONROAD)) {
        device.writeAttr(SUMO_ATTR_ONROAD, myOnRoad);
    }
    if (getAttribute(SUMO_ATTR_WIDTH) != myTagProperty.getDefaultValue(SUMO_ATTR_WIDTH)) {
        device.writeAttr(SUMO_ATTR_WIDTH, myWidth);
    }
    if (getAttribute(SUMO_ATTR_LENGTH) != myTagProperty.getDefaultValue(SUMO_ATTR_LENGTH)) {
        device.writeAttr(SUMO_ATTR_LENGTH, myLength);
    }
    if (getAttribute(SUMO_ATTR_ANGLE) != myTagProperty.getDefaultValue(SUMO_ATTR_ANGLE)) {
        device.writeAttr(SUMO_ATTR_ANGLE, myAngle);
    }
    if (getAttribute(SUMO_ATTR_DEPARTPOS) != myTagProperty.getDefaultValue(SUMO_ATTR_DEPARTPOS)) {
        device.writeAttr(SUMO_ATTR_DEPARTPOS, myDepartPos);
    }
    if (getAttribute(SUMO_ATTR_LEFTHAND) != myTagProperty.getDefaultValue(SUMO_ATTR_LEFTHAND)) {
        device.writeAttr(SUMO_ATTR_LEFTHAND, myLefthand);
    }
    if (getAttribute(SUMO_ATTR_ACCEPTED_BADGES) != myTagProperty.getDefaultValue(SUMO_ATTR_ACCEPTED_BADGES)) {
        device.writeAttr(SUMO_ATTR_ACCEPTED_BADGES, toString(myAcceptedBadges));
    }
    // write all parking spaces
    for (const auto& space : getChildAdditionals()) {
        if (space->getTagProperty().getTag() == SUMO_TAG_PARKING_SPACE) {
            space->writeAdditional(device);
        }
    }
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


void
GNEParkingArea::updateGeometry() {
    // Get value of option "lefthand"
    const double offsetSign = OptionsCont::getOptions().getBool("lefthand") != myLefthand ? -1 : 1;
    // calculate spaceDim
    const double spaceDim = myRoadSideCapacity > 0 ? (getAttributeDouble(SUMO_ATTR_ENDPOS) - getAttributeDouble(SUMO_ATTR_STARTPOS)) / myRoadSideCapacity * getParentLanes().front()->getLengthGeometryFactor() : 7.5;
    // calculate length
    const double length = (myLength > 0) ? myLength : spaceDim;
    // Update common geometry of stopping place
    setStoppingPlaceGeometry(myWidth);
    // Obtain a copy of the shape
    PositionVector tmpShape = myAdditionalGeometry.getShape();
    // Move shape to side
    tmpShape.move2side(1.5 * offsetSign + myWidth);
    // Get position of the sign
    mySignPos = tmpShape.getLineCenter();
    // clear LotSpaceDefinitions
    myLotSpaceDefinitions.clear();
    // iterate over
    for (int i = 0; i < myRoadSideCapacity; ++i) {
        // calculate pos
        const Position pos = GeomHelper::calculateLotSpacePosition(myAdditionalGeometry.getShape(), i, spaceDim, myAngle, myWidth, length);
        // calculate angle
        const double angle = GeomHelper::calculateLotSpaceAngle(myAdditionalGeometry.getShape(), i, spaceDim, myAngle);
        // add GNElotEntry
        myLotSpaceDefinitions.push_back(GNELotSpaceDefinition(pos.x(), pos.y(), pos.z(), angle, myWidth, length));
    }
}


void
GNEParkingArea::drawGL(const GUIVisualizationSettings& s) const {
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // Obtain exaggeration of the draw
        const double parkingAreaExaggeration = getExaggeration(s);
        // check if draw moving geometry points
        const bool movingGeometryPoints = drawMovingGeometryPoints(false);
        // get detail level
        const auto d = s.getDetailLevel(parkingAreaExaggeration);
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
                baseColor = s.colorSettings.parkingAreaColor;
                signColor = s.colorSettings.parkingAreaColorSign;
            }
            // draw parent and child lines
            drawParentChildLines(s, s.additionalSettings.connectionColor);
            // Add a layer matrix
            GLHelper::pushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_PARKING_AREA);
            // set base color
            GLHelper::setColor(baseColor);
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GUIGeometry::drawGeometry(d, myAdditionalGeometry, myWidth * 0.5 * MIN2(1.0, parkingAreaExaggeration));
            // draw sign
            drawSign(d, parkingAreaExaggeration, baseColor, signColor, "P");
            // Traslate to front
            glTranslated(0, 0, 0.1);
            // draw lotSpaceDefinitions
            if (d <= GUIVisualizationSettings::Detail::AdditionalDetails) {
                for (const auto& lsd : myLotSpaceDefinitions) {
                    GLHelper::drawSpaceOccupancies(parkingAreaExaggeration, lsd.position, lsd.rotation, lsd.width, lsd.length, true);
                }
            }
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
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), getPositionInView(), parkingAreaExaggeration);
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
        calculateStoppingPlaceContour(s, d, myWidth * 0.5, parkingAreaExaggeration, movingGeometryPoints);
    }
}


std::string
GNEParkingArea::getAttribute(SumoXMLAttr key) const {
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
        case SUMO_ATTR_DEPARTPOS:
            return myDepartPos;
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_ACCEPTED_BADGES:
            return joinToString(myAcceptedBadges, " ");
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case SUMO_ATTR_ROADSIDE_CAPACITY:
            return toString(myRoadSideCapacity);
        case SUMO_ATTR_ONROAD:
            return toString(myOnRoad);
        case SUMO_ATTR_WIDTH:
            return toString(myWidth);
        case SUMO_ATTR_LENGTH:
            return toString(myLength);
        case SUMO_ATTR_ANGLE:
            return toString(myAngle);
        case SUMO_ATTR_LEFTHAND:
            return toString(myLefthand);
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
GNEParkingArea::getAttributeDouble(SumoXMLAttr key) const {
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
            return ((getAttributeDouble(SUMO_ATTR_ENDPOS) - getAttributeDouble(SUMO_ATTR_STARTPOS)) * 0.5) + getAttributeDouble(SUMO_ATTR_STARTPOS);
        case SUMO_ATTR_WIDTH:
            return myWidth;
        case SUMO_ATTR_LENGTH: {
            // calculate spaceDim
            const double spaceDim = myRoadSideCapacity > 0 ? (getAttributeDouble(SUMO_ATTR_ENDPOS) - getAttributeDouble(SUMO_ATTR_STARTPOS)) / myRoadSideCapacity * getParentLanes().front()->getLengthGeometryFactor() : 7.5;
            return (myLength > 0) ? myLength : spaceDim;
        }
        case SUMO_ATTR_ANGLE:
            return myAngle;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


void
GNEParkingArea::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_DEPARTPOS:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_ACCEPTED_BADGES:
        case SUMO_ATTR_FRIENDLY_POS:
        case SUMO_ATTR_ROADSIDE_CAPACITY:
        case SUMO_ATTR_ONROAD:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_ANGLE:
        case SUMO_ATTR_LEFTHAND:
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
GNEParkingArea::isValid(SumoXMLAttr key, const std::string& value) {
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
        case SUMO_ATTR_DEPARTPOS:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                // parse value
                const double departPos = parse<double>(value);
                if (departPos >= 0) {
                    if (isTemplate()) {
                        return true;
                    } else {
                        return (departPos <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_ACCEPTED_BADGES:
            return canParse<std::vector<std::string> >(value);
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case SUMO_ATTR_ROADSIDE_CAPACITY:
            return canParse<int>(value) && (parse<int>(value) >= 0);
        case SUMO_ATTR_ONROAD:
            return canParse<bool>(value);
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value) && (parse<double>(value) > 0);
        case SUMO_ATTR_LENGTH:
            if (value.empty()) {
                return true;
            } else {
                return canParse<double>(value) && (parse<double>(value) > 0);
            }
        case SUMO_ATTR_ANGLE:
            return canParse<double>(value);
        case SUMO_ATTR_LEFTHAND:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// protected
// ===========================================================================

GNEParkingArea::GNELotSpaceDefinition::GNELotSpaceDefinition() :
    rotation(0),
    width(0),
    length(0) {
}


GNEParkingArea::GNELotSpaceDefinition::GNELotSpaceDefinition(double x, double y, double z, double rotation_, double width_, double length_) :
    position(Position(x, y, z)),
    rotation(rotation_),
    width(width_),
    length(length_) {
}

// ===========================================================================
// private
// ===========================================================================

void
GNEParkingArea::setAttribute(SumoXMLAttr key, const std::string& value) {
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
            updateCenteringBoundary(false);
            break;
        case SUMO_ATTR_ENDPOS:
            if (value == "") {
                myEndPosition = INVALID_DOUBLE;
            } else {
                myEndPosition = parse<double>(value);
            }
            updateCenteringBoundary(false);
            break;
        case SUMO_ATTR_DEPARTPOS:
            myDepartPos = value;
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_ACCEPTED_BADGES:
            myAcceptedBadges = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        case SUMO_ATTR_ROADSIDE_CAPACITY:
            myRoadSideCapacity = parse<int>(value);
            updateCenteringBoundary(false);
            break;
        case SUMO_ATTR_ONROAD:
            myOnRoad = parse<bool>(value);
            break;
        case SUMO_ATTR_WIDTH:
            myWidth = parse<double>(value);
            // update geometry of all spaces
            for (const auto& space : getChildAdditionals()) {
                space->updateGeometry();
            }
            updateCenteringBoundary(false);
            if (!isTemplate()) {
                getParentLanes().front()->getParentEdge()->updateCenteringBoundary(true);
            }
            break;
        case SUMO_ATTR_LENGTH:
            if (value.empty()) {
                myLength = 0;
            } else {
                myLength = parse<double>(value);
            }
            // update geometry of all spaces
            for (const auto& space : getChildAdditionals()) {
                space->updateGeometry();
            }
            break;
        case SUMO_ATTR_ANGLE:
            myAngle = parse<double>(value);
            break;
        case SUMO_ATTR_LEFTHAND:
            myLefthand = parse<bool>(value);
            if (!isTemplate()) {
                updateGeometry();
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


/****************************************************************************/
