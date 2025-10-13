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
/// @file    GNEParkingArea.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
///
// A lane area vehicles can park at (GNE version)
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/options/OptionsCont.h>

#include "GNEParkingArea.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEParkingArea::GNEParkingArea(GNENet* net) :
    GNEStoppingPlace(net, SUMO_TAG_PARKING_AREA) {
}


GNEParkingArea::GNEParkingArea(const std::string& id, GNENet* net, const std::string& filename, GNELane* lane, const double startPos, const double endPos,
                               const std::string& departPos, const std::string& name, const std::vector<std::string>& badges,
                               const bool friendlyPosition, const int roadSideCapacity, const bool onRoad, const double width,
                               const double length, const double angle, const bool lefthand, const Parameterised::Map& parameters) :
    GNEStoppingPlace(id, net, filename, SUMO_TAG_PARKING_AREA, lane, startPos,
                     endPos, name, friendlyPosition, RGBColor::INVISIBLE, angle, parameters),
    myDepartPos(departPos),
    myRoadSideCapacity(roadSideCapacity),
    myOnRoad(onRoad),
    myWidth(width),
    myLength(length),
    myLefthand(lefthand),
    myAcceptedBadges(badges) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEParkingArea::~GNEParkingArea() {}


void
GNEParkingArea::writeAdditional(OutputDevice& device) const {
    device.openTag(getTagProperty()->getTag());
    // write common attributes
    writeStoppingPlaceAttributes(device);
    // write specific attributes
    if (myRoadSideCapacity != myTagProperty->getDefaultIntValue(SUMO_ATTR_ROADSIDE_CAPACITY)) {
        device.writeAttr(SUMO_ATTR_ROADSIDE_CAPACITY, myRoadSideCapacity);
    }
    if (myOnRoad != myTagProperty->getDefaultBoolValue(SUMO_ATTR_ONROAD)) {
        device.writeAttr(SUMO_ATTR_ONROAD, myOnRoad);
    }
    if (myWidth != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_WIDTH)) {
        device.writeAttr(SUMO_ATTR_WIDTH, myWidth);
    }
    if (myLength != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_LENGTH)) {
        device.writeAttr(SUMO_ATTR_LENGTH, myLength);
    }
    if (myDepartPos != myTagProperty->getDefaultStringValue(SUMO_ATTR_DEPARTPOS)) {
        device.writeAttr(SUMO_ATTR_DEPARTPOS, myDepartPos);
    }
    if (myLefthand != myTagProperty->getDefaultBoolValue(SUMO_ATTR_LEFTHAND)) {
        device.writeAttr(SUMO_ATTR_LEFTHAND, myLefthand);
    }
    if (getAttribute(SUMO_ATTR_ACCEPTED_BADGES) != myTagProperty->getDefaultStringValue(SUMO_ATTR_ACCEPTED_BADGES)) {
        device.writeAttr(SUMO_ATTR_ACCEPTED_BADGES, toString(myAcceptedBadges));
    }
    // write all parking spaces
    for (const auto& space : getChildAdditionals()) {
        if (space->getTagProperty()->getTag() == SUMO_TAG_PARKING_SPACE) {
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
    mySymbolPosition = tmpShape.getLineCenter();
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
        const bool movingGeometryPoints = drawMovingGeometryPoints();
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
            drawInLayer(GLO_PARKING_AREA);
            // set base color
            GLHelper::setColor(baseColor);
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GUIGeometry::drawGeometry(d, myAdditionalGeometry, myWidth * 0.5 * MIN2(1.0, parkingAreaExaggeration));
            // draw sign
            drawSign(s, d, parkingAreaExaggeration, baseColor, signColor, "P");
            // Traslate to front
            glTranslated(0, 0, 0.1);
            // draw lotSpaceDefinitions
            if (d <= GUIVisualizationSettings::Detail::AdditionalDetails) {
                for (const auto& lsd : myLotSpaceDefinitions) {
                    GLHelper::drawSpaceOccupancies(parkingAreaExaggeration, lsd.position, lsd.rotation, lsd.width, lsd.length, true);
                }
            }
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
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), getPositionInView(), parkingAreaExaggeration);
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
        calculateStoppingPlaceContour(s, d, myWidth * 0.5, parkingAreaExaggeration, movingGeometryPoints);
    }
}


std::string
GNEParkingArea::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPARTPOS:
            return myDepartPos;
        case SUMO_ATTR_ACCEPTED_BADGES:
            return joinToString(myAcceptedBadges, " ");
        case SUMO_ATTR_ROADSIDE_CAPACITY:
            return toString(myRoadSideCapacity);
        case SUMO_ATTR_ONROAD:
            return toString(myOnRoad);
        case SUMO_ATTR_WIDTH:
            return toString(myWidth);
        case SUMO_ATTR_LENGTH:
            return toString(myLength);
        case SUMO_ATTR_LEFTHAND:
            return toString(myLefthand);
        default:
            return getStoppingPlaceAttribute(key);
    }
}


double
GNEParkingArea::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_WIDTH:
            return myWidth;
        case SUMO_ATTR_LENGTH: {
            // calculate spaceDim
            const double spaceDim = myRoadSideCapacity > 0 ? (getAttributeDouble(SUMO_ATTR_ENDPOS) - getAttributeDouble(SUMO_ATTR_STARTPOS)) / myRoadSideCapacity * getParentLanes().front()->getLengthGeometryFactor() : 7.5;
            return (myLength > 0) ? myLength : spaceDim;
        }
        default:
            return getStoppingPlaceAttributeDouble(key);
    }
}


void
GNEParkingArea::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_DEPARTPOS:
        case SUMO_ATTR_ACCEPTED_BADGES:
        case SUMO_ATTR_ROADSIDE_CAPACITY:
        case SUMO_ATTR_ONROAD:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_LEFTHAND:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setStoppingPlaceAttribute(key, value, undoList);
            break;
    }
}


bool
GNEParkingArea::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
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
        case SUMO_ATTR_ACCEPTED_BADGES:
            return true;
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
        case SUMO_ATTR_LEFTHAND:
            return canParse<bool>(value);
        default:
            return isStoppingPlaceValid(key, value);
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
        case SUMO_ATTR_DEPARTPOS:
            myDepartPos = value;
            break;
        case SUMO_ATTR_ACCEPTED_BADGES:
            myAcceptedBadges = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
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
        case SUMO_ATTR_LEFTHAND:
            myLefthand = parse<bool>(value);
            if (!isTemplate()) {
                updateGeometry();
            }
            break;
        default:
            setStoppingPlaceAttribute(key, value);
            break;
    }
}


/****************************************************************************/
