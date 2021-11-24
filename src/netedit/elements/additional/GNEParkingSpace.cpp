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
/// @file    GNEParkingSpace.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
///
// A lane area vehicles can halt at (GNE version)
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEParkingSpace.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEParkingSpace::GNEParkingSpace(GNENet* net) :
    GNEAdditional("", net, GLO_PARKING_SPACE, SUMO_TAG_PARKING_SPACE, "",
        {}, {}, {}, {}, {}, {}, {}, {},
    std::map<std::string, std::string>()),
    mySlope(0) {
    // reset default values
    resetDefaultValues();
}


GNEParkingSpace::GNEParkingSpace(GNENet* net, GNEAdditional* parkingAreaParent, const Position& pos,
                                 const std::string& width, const std::string& length, const std::string& angle, double slope,
                                 const std::string& name, const std::map<std::string, std::string>& parameters) :
    GNEAdditional(net, GLO_PARKING_SPACE, SUMO_TAG_PARKING_SPACE, name,
        {}, {}, {}, {parkingAreaParent}, {}, {}, {}, {},
    parameters),
    myPosition(pos),
    myWidth(width),
    myLength(length),
    myAngle(angle),
    mySlope(slope) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEParkingSpace::~GNEParkingSpace() {}


GNEMoveOperation*
GNEParkingSpace::getMoveOperation() {
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
            (myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE) &&
            myNet->getViewNet()->getMouseButtonKeyPressed().shiftKeyPressed()) {
        // get snap radius
        const double snap_radius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.additionalGeometryPointRadius;
        // get mouse position
        const Position mousePosition = myNet->getViewNet()->getPositionInformation();
        // check if we're editing width or height
        if (myShapeLength.back().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
            // edit lenght
            return new GNEMoveOperation(this, myShapeLength, false, GNEMoveOperation::OperationType::LENGTH);
        } else if (myShapeWidth.front().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
            // edit width
            return new GNEMoveOperation(this, myShapeWidth, true, GNEMoveOperation::OperationType::WIDTH);
        } else if (myShapeWidth.back().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
            // edit width
            return new GNEMoveOperation(this, myShapeWidth, false, GNEMoveOperation::OperationType::WIDTH);
        } else {
            return nullptr;
        }
    } else {
        // move entire space
        return new GNEMoveOperation(this, myPosition);
    }
}


void
GNEParkingSpace::updateGeometry() {
    // get width an lenght
    const double width = getAttributeDouble(SUMO_ATTR_WIDTH) <= 0 ? POSITION_EPS : getAttributeDouble(SUMO_ATTR_WIDTH);
    const double lenght = getAttributeDouble(SUMO_ATTR_LENGTH) <= 0 ? POSITION_EPS : getAttributeDouble(SUMO_ATTR_LENGTH);
    // calculate shape lenght
    myShapeLength.clear();
    myShapeLength.push_back(Position(0, 0));
    myShapeLength.push_back(Position(0, lenght));
    // rotate
    myShapeLength.rotate2D(DEG2RAD(getAttributeDouble(SUMO_ATTR_ANGLE)));
    // move
    myShapeLength.add(myPosition);
    // calculate shape width
    PositionVector leftShape = myShapeLength;
    leftShape.move2side(width * -0.5);
    PositionVector rightShape = myShapeLength;
    rightShape.move2side(width * 0.5);
    myShapeWidth = {leftShape.getCentroid(), rightShape.getCentroid()};
    // update centering boundary
    updateCenteringBoundary(true);
}


Position
GNEParkingSpace::getPositionInView() const {
    return myPosition;
}


void
GNEParkingSpace::updateCenteringBoundary(const bool /*updateGrid*/) {
    // first reset boundary
    myAdditionalBoundary.reset();
    // add position
    myAdditionalBoundary.add(myPosition);
    // grow width and lenght
    myAdditionalBoundary.grow(myShapeLength.length2D());
    myAdditionalBoundary.grow(myShapeWidth.length2D());
    // grow
    myAdditionalBoundary.grow(10);
    // update centering boundary of parent
    getParentAdditionals().front()->updateCenteringBoundary(true);
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
    // Set initial values
    const double parkingAreaExaggeration = getExaggeration(s);
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals() && s.drawAdditionals(parkingAreaExaggeration)) {
        // obtain  values
        const double width = myShapeWidth.length2D() * 0.5 + (parkingAreaExaggeration * 0.1);
        const double angle = getAttributeDouble(SUMO_ATTR_ANGLE);
        // get colors
        const RGBColor baseColor = drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.colorSettings.parkingSpaceColor;
        const RGBColor contourColor = drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.colorSettings.parkingSpaceColorContour;
        // draw parent and child lines
        drawParentChildLines(s, s.additionalSettings.connectionColor);
        // push name
        GLHelper::pushName(getGlID());
        // push later matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_PARKING_SPACE);
        // set contour color
        GLHelper::setColor(contourColor);
        // draw extern
        GLHelper::drawBoxLines(myShapeLength, width);
        // make a copy of myShapeLength and scale
        PositionVector shapeLengthInner = myShapeLength;
        shapeLengthInner.scaleAbsolute(-0.1);
        // draw intern
        if (!s.drawForRectangleSelection) {
            // Traslate to front
            glTranslated(0, 0, 0.1);
            // set base color
            GLHelper::setColor(baseColor);
            //draw intern
            GLHelper::drawBoxLines(shapeLengthInner, width - 0.1);
        }
        // draw geometry points
        drawUpGeometryPoint(myNet->getViewNet(), myShapeLength.back(), angle, contourColor);
        drawLeftGeometryPoint(myNet->getViewNet(), myShapeWidth.back(), angle - 90, contourColor);
        drawRightGeometryPoint(myNet->getViewNet(), myShapeWidth.front(), angle - 90, contourColor);
        // pop layer matrix
        GLHelper::popMatrix();
        // pop name
        GLHelper::popName();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), myShapeLength.getPolygonCenter(), parkingAreaExaggeration);
        // check if dotted contours has to be drawn
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            // draw using drawDottedContourClosedShape
            GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, myShapeLength, width, parkingAreaExaggeration, true, true);
        }
        if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
            // draw using drawDottedContourClosedShape
            GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, myShapeLength, width, parkingAreaExaggeration, true, true);
        }
        // Draw additional ID
        drawAdditionalID(s);
        // draw additional name
        drawAdditionalName(s);
    }
}


std::string
GNEParkingSpace::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_WIDTH:
            return myWidth;
        case SUMO_ATTR_LENGTH:
            return myLength;
        case SUMO_ATTR_ANGLE:
            return myAngle;
        case SUMO_ATTR_SLOPE:
            return toString(mySlope);
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEParkingSpace::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_WIDTH:
            return myWidth.empty() ? getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_WIDTH) : parse<double>(myWidth);
        case SUMO_ATTR_LENGTH:
            return myLength.empty() ? getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_LENGTH) : parse<double>(myLength);
        case SUMO_ATTR_ANGLE:
            return myAngle.empty() ? getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_ANGLE) : parse<double>(myAngle);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
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
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEParkingSpace::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
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
GNEParkingSpace::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_POSITION:
            myPosition = parse<Position>(value);
            // update geometry
            updateGeometry();
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_WIDTH:
            myWidth = value;
            // update geometry (except for template)
            if (getParentAdditionals().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_LENGTH:
            myLength = value;
            // update geometry (except for template)
            if (getParentAdditionals().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_ANGLE:
            myAngle = value;
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
GNEParkingSpace::setMoveShape(const GNEMoveResult& moveResult) {
    // check what are being updated
    if (moveResult.operationType == GNEMoveOperation::OperationType::LENGTH) {
        myShapeLength[1] = moveResult.shapeToUpdate[1];
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::WIDTH) {
        myShapeWidth = moveResult.shapeToUpdate;
    } else {
        myPosition = moveResult.shapeToUpdate.front();
        // update geometry
        updateGeometry();
    }
}


void
GNEParkingSpace::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // check what are being updated
    if (moveResult.operationType == GNEMoveOperation::OperationType::LENGTH) {
        undoList->begin(myTagProperty.getGUIIcon(), "length of " + getTagStr());
        setAttribute(SUMO_ATTR_LENGTH, toString(myShapeLength[0].distanceTo2D(moveResult.shapeToUpdate[1])), undoList);
        undoList->end();
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::WIDTH) {
        undoList->begin(myTagProperty.getGUIIcon(), "width of " + getTagStr());
        setAttribute(SUMO_ATTR_WIDTH, toString(moveResult.shapeToUpdate.length2D()), undoList);
        undoList->end();
    } else {
        undoList->begin(myTagProperty.getGUIIcon(), "position of " + getTagStr());
        setAttribute(SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front()), undoList);
        undoList->end();
    }
}

/****************************************************************************/
