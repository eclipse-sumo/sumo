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

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/moving/GNEMoveElementViewResizable.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <utils/gui/div/GLHelper.h>

#include "GNEParkingSpace.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEParkingSpace::GNEParkingSpace(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_PARKING_SPACE, ""),
    myMoveElementViewResizable(new GNEMoveElementViewResizable(this, GNEMoveElementView::AttributesFormat::CARTESIAN,
                               GNEMoveElementViewResizable::ResizingFormat::WIDTH_LENGTH, SUMO_ATTR_POSITION,
                               myPosOverView, myWidth, myLength)) {
}


GNEParkingSpace::GNEParkingSpace(GNEAdditional* parkingAreaParent, const Position& pos,
                                 const double width, const double length, const double angle,
                                 const double slope, const std::string& name,
                                 const Parameterised::Map& parameters) :
    GNEAdditional(parkingAreaParent, SUMO_TAG_PARKING_SPACE, name),
    Parameterised(parameters),
    myPosOverView(pos),
    myWidth(width),
    myLength(length),
    myMoveElementViewResizable(new GNEMoveElementViewResizable(this, GNEMoveElementView::AttributesFormat::CARTESIAN,
                               GNEMoveElementViewResizable::ResizingFormat::WIDTH_LENGTH, SUMO_ATTR_POSITION,
                               myPosOverView, myWidth, myLength)),
    myAngle(angle),
    mySlope(slope) {
    // set parents
    setParent<GNEAdditional*>(parkingAreaParent);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEParkingSpace::~GNEParkingSpace() {
    delete myMoveElementViewResizable;
}


GNEMoveElement*
GNEParkingSpace::getMoveElement() const {
    return myMoveElementViewResizable;
}


Parameterised*
GNEParkingSpace::getParameters() {
    return this;
}


const Parameterised*
GNEParkingSpace::getParameters() const {
    return this;
}


void
GNEParkingSpace::writeAdditional(OutputDevice& device) const {
    device.openTag(getTagProperty()->getTag());
    // write common additional attributes
    writeAdditionalAttributes(device);
    // write move atributes
    myMoveElementViewResizable->writeMoveAttributes(device);
    // write specific attributes
    if (myWidth != INVALID_DOUBLE) {
        device.writeAttr(SUMO_ATTR_WIDTH, myWidth);
    }
    if (myLength != INVALID_DOUBLE) {
        device.writeAttr(SUMO_ATTR_LENGTH, myLength);
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
    myMoveElementViewResizable->myShapeHeight.clear();
    myMoveElementViewResizable->myShapeHeight.push_back(Position(0, 0));
    myMoveElementViewResizable->myShapeHeight.push_back(Position(0, length));
    // rotate
    myMoveElementViewResizable->myShapeHeight.rotate2D(DEG2RAD(getAttributeDouble(SUMO_ATTR_ANGLE)));
    // move
    myMoveElementViewResizable->myShapeHeight.add(myPosOverView);
    // calculate shape width
    PositionVector leftShape = myMoveElementViewResizable->myShapeHeight;
    leftShape.move2side(width * -0.5);
    PositionVector rightShape = myMoveElementViewResizable->myShapeHeight;
    rightShape.move2side(width * 0.5);
    myMoveElementViewResizable->myShapeWidth = {leftShape.getCentroid(), rightShape.getCentroid()};
    // update centering boundary
    updateCenteringBoundary(true);
}


Position
GNEParkingSpace::getPositionInView() const {
    return myPosOverView;
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
    myAdditionalBoundary.add(myPosOverView);
    // add center
    myAdditionalBoundary.add(myPosOverView);
    // add width
    for (const auto& pos : myMoveElementViewResizable->myShapeWidth) {
        myAdditionalBoundary.add(pos);
    }
    // add length
    for (const auto& pos : myMoveElementViewResizable->myShapeHeight) {
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
        const double parkingSpaceWidth = myMoveElementViewResizable->myShapeWidth.length2D() * 0.5 + (spaceExaggeration * 0.1);
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
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), myMoveElementViewResizable->myShapeHeight.getPolygonCenter(), spaceExaggeration);
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
                if (myMoveElementViewResizable->myShapeHeight.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared) {
                    myMoveElementViewResizable->myMovingContourUp.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                    myMoveElementViewResizable->myMovingContourDown.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                } else if ((myMoveElementViewResizable->myShapeWidth.front().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared) ||
                           (myMoveElementViewResizable->myShapeWidth.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
                    myMoveElementViewResizable->myMovingContourLeft.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                    myMoveElementViewResizable->myMovingContourRight.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
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
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_WIDTH:
            return (myWidth != INVALID_DOUBLE) ? toString(myWidth) : "";
        case SUMO_ATTR_LENGTH:
            return (myLength != INVALID_DOUBLE) ? toString(myLength) : "";
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
            return myMoveElementViewResizable->getMovingAttribute(key);
    }
}


double
GNEParkingSpace::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_WIDTH:
            return (myWidth != INVALID_DOUBLE) ? myWidth : getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_WIDTH);
        case SUMO_ATTR_LENGTH:
            return (myLength != INVALID_DOUBLE) ? myLength : getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_LENGTH);
        case SUMO_ATTR_ANGLE:
            return (myAngle != INVALID_DOUBLE) ? myAngle : getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_ANGLE);
        default:
            return myMoveElementViewResizable->getMovingAttributeDouble(key);
    }
}


Position
GNEParkingSpace::getAttributePosition(SumoXMLAttr key) const {
    return myMoveElementViewResizable->getMovingAttributePosition(key);
}


PositionVector
GNEParkingSpace::getAttributePositionVector(SumoXMLAttr key) const {
    return myMoveElementViewResizable->getMovingAttributePositionVector(key);
}


void
GNEParkingSpace::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_ANGLE:
        case SUMO_ATTR_SLOPE:
        case GNE_ATTR_PARENT:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            myMoveElementViewResizable->setMovingAttribute(key, value, undoList);
            break;
    }
}


bool
GNEParkingSpace::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
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
            return myMoveElementViewResizable->isMovingAttributeValid(key, value);
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
    GLHelper::drawBoxLines(myMoveElementViewResizable->myShapeHeight, width);
    // make a copy of myShapeLength and scale
    PositionVector shapeLengthInner = myMoveElementViewResizable->myShapeHeight;
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
        if (myMoveElementViewResizable->myShapeHeight.size() > 0) {
            drawUpGeometryPoint(s, d, myMoveElementViewResizable->myShapeHeight.back(), angle, RGBColor::ORANGE);
        }
        if (myMoveElementViewResizable->myShapeWidth.size() > 0) {
            drawLeftGeometryPoint(s, d, myMoveElementViewResizable->myShapeWidth.back(), angle - 90, RGBColor::ORANGE);
            drawRightGeometryPoint(s, d, myMoveElementViewResizable->myShapeWidth.front(), angle - 90, RGBColor::ORANGE);
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
        myMoveElementViewResizable->myMovingContourUp.calculateContourCircleShape(s, d, this, myMoveElementViewResizable->myShapeHeight.back(), s.neteditSizeSettings.additionalGeometryPointRadius,
                getType(), exaggeration, nullptr);
        myMoveElementViewResizable->myMovingContourLeft.calculateContourCircleShape(s, d, this, myMoveElementViewResizable->myShapeWidth.front(), s.neteditSizeSettings.additionalGeometryPointRadius, getType(),
                exaggeration, nullptr);
        myMoveElementViewResizable->myMovingContourRight.calculateContourCircleShape(s, d, this, myMoveElementViewResizable->myShapeWidth.back(), s.neteditSizeSettings.additionalGeometryPointRadius, getType(),
                exaggeration, nullptr);
    } else {
        myAdditionalContour.calculateContourExtrudedShape(s, d, this, myMoveElementViewResizable->myShapeHeight, getType(), width, exaggeration, true, true, 0, nullptr, nullptr);
    }
}


void
GNEParkingSpace::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_WIDTH:
            if (value.empty()) {
                myWidth = INVALID_DOUBLE;
            } else {
                myWidth = parse<double>(value);
            }
            break;
        case SUMO_ATTR_LENGTH:
            if (value.empty()) {
                myLength = INVALID_DOUBLE;
            } else {
                myLength = parse<double>(value);
            }
            break;
        case SUMO_ATTR_ANGLE:
            if (value.empty()) {
                myAngle = INVALID_DOUBLE;
            } else {
                myAngle = parse<double>(value);
            }
            break;
        case SUMO_ATTR_SLOPE:
            mySlope = parse<double>(value);
            break;
        case GNE_ATTR_PARENT:
            replaceAdditionalParent(SUMO_TAG_PARKING_AREA, value, 0);
            break;
        default:
            myMoveElementViewResizable->setMovingAttribute(key, value);
            break;
    }
    // update geometry (except for template)
    if (getParentAdditionals().size() > 0) {
        updateGeometry();
    }
}

/****************************************************************************/
