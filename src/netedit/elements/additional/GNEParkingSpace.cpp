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
/// @file    GNEParkingSpace.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
///
// A lane area vehicles can halt at (GNE version)
/****************************************************************************/
#include <config.h>

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/moving/GNEMoveElementView.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEParkingSpace.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEParkingSpace::GNEParkingSpace(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_PARKING_SPACE, ""),
    myMoveElementView(new GNEMoveElementView(this)) {
}


GNEParkingSpace::GNEParkingSpace(GNEAdditional* parkingAreaParent, const Position& pos,
                                 const double width, const double length, const double angle,
                                 const double slope, const std::string& name,
                                 const Parameterised::Map& parameters) :
    GNEAdditional(parkingAreaParent, SUMO_TAG_PARKING_SPACE, name),
    Parameterised(parameters),
    myMoveElementView(new GNEMoveElementView(this, GNEMoveElementView::AttributesFormat::CARTESIAN, pos, width, 0, length)),
    myAngle(angle),
    mySlope(slope) {
    // set parents
    setParent<GNEAdditional*>(parkingAreaParent);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEParkingSpace::~GNEParkingSpace() {
    delete myMoveElementView;
}


GNEMoveElement*
GNEParkingSpace::getMoveElement() const {
    return myMoveElementView;
}


void
GNEParkingSpace::writeAdditional(OutputDevice& device) const {
    device.openTag(getTagProperty()->getTag());
    // write common additional attributes
    writeAdditionalAttributes(device);
    // write move atributes
    myMoveElementView->writeMoveAttributes(device);
    // write specific attributes
    if (myMoveElementView->myWidth != INVALID_DOUBLE) {
        device.writeAttr(SUMO_ATTR_WIDTH, myMoveElementView->myWidth);
    }
    if (myMoveElementView->myLength != INVALID_DOUBLE) {
        device.writeAttr(SUMO_ATTR_LENGTH, myMoveElementView->myLength);
    }
    if (myAngle != INVALID_DOUBLE) {
        device.writeAttr(SUMO_ATTR_ANGLE, myAngle);
    }
    if (mySlope != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_SLOPE)) {
        device.writeAttr(SUMO_ATTR_SLOPE, mySlope);
    }
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


bool
GNEParkingSpace::isAdditionalValid() const {
    return true;
}


std::string
GNEParkingSpace::getAdditionalProblem() const {
    return "";
}


void
GNEParkingSpace::fixAdditionalProblem() {
    // nothing to fix
}


bool
GNEParkingSpace::checkDrawMoveContour() const {
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


void
GNEParkingSpace::updateGeometry() {
    // get width an length
    const double width = getAttributeDouble(SUMO_ATTR_WIDTH) <= 0 ? POSITION_EPS : getAttributeDouble(SUMO_ATTR_WIDTH);
    const double length = getAttributeDouble(SUMO_ATTR_LENGTH) <= 0 ? POSITION_EPS : getAttributeDouble(SUMO_ATTR_LENGTH);
    // calculate shape length
    myMoveElementView->myShapeLength.clear();
    myMoveElementView->myShapeLength.push_back(Position(0, 0));
    myMoveElementView->myShapeLength.push_back(Position(0, length));
    // rotate
    myMoveElementView->myShapeLength.rotate2D(DEG2RAD(getAttributeDouble(SUMO_ATTR_ANGLE)));
    // move
    myMoveElementView->myShapeLength.add(myMoveElementView->myPosOverView);
    // calculate shape width
    PositionVector leftShape = myMoveElementView->myShapeLength;
    leftShape.move2side(width * -0.5);
    PositionVector rightShape = myMoveElementView->myShapeLength;
    rightShape.move2side(width * 0.5);
    myMoveElementView->myShapeWidth = {leftShape.getCentroid(), rightShape.getCentroid()};
    // update centering boundary
    updateCenteringBoundary(true);
}


Position
GNEParkingSpace::getPositionInView() const {
    return myMoveElementView->myPosOverView;
}


void
GNEParkingSpace::updateCenteringBoundary(const bool updateGrid) {
    // remove additional from grid
    if (updateGrid) {
        myNet->removeGLObjectFromGrid(this);
    }
    // first reset boundary
    myAdditionalBoundary.reset();
    // add position
    myAdditionalBoundary.add(myMoveElementView->myPosOverView);
    // add center
    myAdditionalBoundary.add(myMoveElementView->myPosOverView);
    // add width
    for (const auto& pos : myMoveElementView->myShapeWidth) {
        myAdditionalBoundary.add(pos);
    }
    // add length
    for (const auto& pos : myMoveElementView->myShapeLength) {
        myAdditionalBoundary.add(pos);
    }
    // grow
    myAdditionalBoundary.grow(5);
    // add additional into RTREE again
    if (updateGrid) {
        myNet->addGLObjectIntoGrid(this);
    }
}


void
GNEParkingSpace::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNEParkingSpace::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNEParkingSpace::drawGL(const GUIVisualizationSettings& s) const {
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // draw boundaries
        GLHelper::drawBoundary(s, getCenteringBoundary());
        // get exaggeration
        const double spaceExaggeration = getExaggeration(s);
        // get witdh
        const double parkingSpaceWidth = myMoveElementView->myShapeWidth.length2D() * 0.5 + (spaceExaggeration * 0.1);
        // get detail level
        const auto d = s.getDetailLevel(spaceExaggeration);
        // check if draw moving geometry points
        const bool movingGeometryPoints = drawMovingGeometryPoints();
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // draw space
            drawSpace(s, d, parkingSpaceWidth, movingGeometryPoints);
            // draw parent and child lines
            drawParentChildLines(s, s.additionalSettings.connectionColor);
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), myMoveElementView->myShapeLength.getPolygonCenter(), spaceExaggeration);
            // Draw additional ID
            drawAdditionalID(s);
            // draw additional name
            drawAdditionalName(s);
            // draw dotted contours
            if (movingGeometryPoints) {
                // get snap radius
                const double snapRadius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.additionalGeometryPointRadius;
                const double snapRadiusSquared = snapRadius * snapRadius;
                // get mouse position
                const Position mousePosition = myNet->getViewNet()->getPositionInformation();
                // check if we're editing width or height
                if (myMoveElementView->myShapeLength.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared) {
                    myMoveElementView->myMovingContourUp.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                    myMoveElementView->myMovingContourDown.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                } else if ((myMoveElementView->myShapeWidth.front().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared) ||
                           (myMoveElementView->myShapeWidth.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
                    myMoveElementView->myMovingContourLeft.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                    myMoveElementView->myMovingContourRight.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                }
            } else {
                myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
            }

        }
        // calculate contour
        calculateSpaceContour(s, d, parkingSpaceWidth, spaceExaggeration, movingGeometryPoints);
    }
}


std::string
GNEParkingSpace::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_POSITION:
            return toString(myMoveElementView->myPosOverView);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_WIDTH:
            return (myMoveElementView->myWidth != INVALID_DOUBLE) ? toString(myMoveElementView->myWidth) : "";
        case SUMO_ATTR_LENGTH:
            return (myMoveElementView->myLength != INVALID_DOUBLE) ? toString(myMoveElementView->myLength) : "";
        case SUMO_ATTR_ANGLE:
            return (myAngle != INVALID_DOUBLE) ? toString(myAngle) : "";;
        case SUMO_ATTR_SLOPE:
            return toString(mySlope);
        case GNE_ATTR_PARENT:
            if (isTemplate()) {
                return "";
            } else {
                return getParentAdditionals().at(0)->getID();
            }
        default:
            return getCommonAttribute(this, key);
    }
}


double
GNEParkingSpace::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_WIDTH:
            return (myMoveElementView->myWidth != INVALID_DOUBLE) ? myMoveElementView->myWidth : getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_WIDTH);
        case SUMO_ATTR_LENGTH:
            return (myMoveElementView->myLength != INVALID_DOUBLE) ? myMoveElementView->myLength : getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_LENGTH);
        case SUMO_ATTR_ANGLE:
            return (myAngle != INVALID_DOUBLE) ? myAngle : getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_ANGLE);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


const Parameterised::Map&
GNEParkingSpace::getACParametersMap() const {
    return getParametersMap();
}


void
GNEParkingSpace::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_ANGLE:
        case SUMO_ATTR_SLOPE:
        case GNE_ATTR_PARENT:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNEParkingSpace::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_POSITION:
            return canParse<Position>(value);
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_WIDTH:
            return value.empty() || (canParse<double>(value) && (parse<double>(value) > 0));
        case SUMO_ATTR_LENGTH:
            return value.empty() || (canParse<double>(value) && (parse<double>(value) > 0));
        case SUMO_ATTR_ANGLE:
            return value.empty() || canParse<double>(value);
        case SUMO_ATTR_SLOPE:
            return canParse<double>(value);
        case GNE_ATTR_PARENT:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_PARKING_AREA, value, false) != nullptr);
        default:
            return isCommonValid(key, value);
    }
}


std::string
GNEParkingSpace::getPopUpID() const {
    return getTagStr();
}


std::string
GNEParkingSpace::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_POSITION);
}

// ===========================================================================
// private
// ===========================================================================

void
GNEParkingSpace::drawSpace(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                           const double width, const bool movingGeometryPoints) const {
    // get angle
    const double angle = getAttributeDouble(SUMO_ATTR_ANGLE);
    // get contour color
    RGBColor contourColor = s.colorSettings.parkingSpaceColorContour;
    if (drawUsingSelectColor()) {
        contourColor = s.colorSettings.selectedAdditionalColor;
    } else if (d <= GUIVisualizationSettings::Detail::AdditionalDetails) {
        contourColor = s.colorSettings.parkingSpaceColorContour;
    }
    // push later matrix
    GLHelper::pushMatrix();
    // translate to front
    drawInLayer(GLO_PARKING_SPACE);
    // set contour color
    GLHelper::setColor(contourColor);
    // draw extern
    GLHelper::drawBoxLines(myMoveElementView->myShapeLength, width);
    // make a copy of myShapeLength and scale
    PositionVector shapeLengthInner = myMoveElementView->myShapeLength;
    shapeLengthInner.scaleAbsolute(-0.1);
    // draw intern
    if (d <= GUIVisualizationSettings::Detail::AdditionalDetails) {
        // Traslate to front
        glTranslated(0, 0, 0.1);
        // set base color
        GLHelper::setColor(drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.colorSettings.parkingSpaceColor);
        //draw intern
        GLHelper::drawBoxLines(shapeLengthInner, width - 0.1);
    }
    // draw geometry points
    if (movingGeometryPoints) {
        if (myMoveElementView->myShapeLength.size() > 0) {
            drawUpGeometryPoint(s, d, myMoveElementView->myShapeLength.back(), angle, RGBColor::ORANGE);
        }
        if (myMoveElementView->myShapeWidth.size() > 0) {
            drawLeftGeometryPoint(s, d, myMoveElementView->myShapeWidth.back(), angle - 90, RGBColor::ORANGE);
            drawRightGeometryPoint(s, d, myMoveElementView->myShapeWidth.front(), angle - 90, RGBColor::ORANGE);
        }
    }
    // pop layer matrix
    GLHelper::popMatrix();
}


void
GNEParkingSpace::calculateSpaceContour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                       const double width, const double exaggeration, const bool movingGeometryPoints) const {
    // check if we're calculating the contour or the moving geometry points
    if (movingGeometryPoints) {
        myMoveElementView->myMovingContourUp.calculateContourCircleShape(s, d, this, myMoveElementView->myShapeLength.back(), s.neteditSizeSettings.additionalGeometryPointRadius,
                getType(), exaggeration, nullptr);
        myMoveElementView->myMovingContourLeft.calculateContourCircleShape(s, d, this, myMoveElementView->myShapeWidth.front(), s.neteditSizeSettings.additionalGeometryPointRadius, getType(),
                exaggeration, nullptr);
        myMoveElementView->myMovingContourRight.calculateContourCircleShape(s, d, this, myMoveElementView->myShapeWidth.back(), s.neteditSizeSettings.additionalGeometryPointRadius, getType(),
                exaggeration, nullptr);
    } else {
        myAdditionalContour.calculateContourExtrudedShape(s, d, this, myMoveElementView->myShapeLength, getType(), width, exaggeration, true, true, 0, nullptr, nullptr);
    }
}


void
GNEParkingSpace::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_POSITION:
            myMoveElementView->myPosOverView = parse<Position>(value);
            // update geometry
            updateGeometry();
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_WIDTH:
            if (value.empty()) {
                myMoveElementView->myWidth = INVALID_DOUBLE;
            } else {
                myMoveElementView->myWidth = parse<double>(value);
            }
            // update geometry (except for template)
            if (getParentAdditionals().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_LENGTH:
            if (value.empty()) {
                myMoveElementView->myLength = INVALID_DOUBLE;
            } else {
                myMoveElementView->myLength = parse<double>(value);
            }
            // update geometry (except for template)
            if (getParentAdditionals().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_ANGLE:
            if (value.empty()) {
                myAngle = INVALID_DOUBLE;
            } else {
                myAngle = parse<double>(value);
            }
            // update geometry (except for template)
            if (getParentAdditionals().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_SLOPE:
            mySlope = parse<double>(value);
            break;
        case GNE_ATTR_PARENT:
            replaceAdditionalParent(SUMO_TAG_PARKING_AREA, value, 0);
            break;
        default:
            setCommonAttribute(this, key, value);
            break;
    }
}

/****************************************************************************/
