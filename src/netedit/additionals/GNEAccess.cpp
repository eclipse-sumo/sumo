/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAccess.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2018
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <iostream>
#include <utility>
#include <netbuild/NBEdge.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/GNEViewParent.h>

#include "GNEAccess.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEAccess::GNEAccess(const std::string& id, GNELane* lane, GNEViewNet* viewNet, const std::string& pos, const std::string& length, bool friendlyPos, bool blockMovement) :
    GNEAdditional(id, viewNet, GLO_ACCESS, SUMO_TAG_ACCESS, true, blockMovement),
    myLane(lane),
    myPositionOverLane(pos),
    myLength(length),
    myFriendlyPosition(friendlyPos) {
}


GNEAccess::~GNEAccess() {
}


void 
GNEAccess::moveGeometry(const Position& oldPos, const Position& offset) {
    // Calculate new position using old position
    Position newPosition = oldPos;
    newPosition.add(offset);
    myPositionOverLane = toString(myLane->getShape().nearest_offset_to_point2D(newPosition, false));
    // Update geometry
    updateGeometry();
}


void 
GNEAccess::commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) {
    if (!myBlockMovement) {
        // restore old position before commit new position
        double originalPosOverLane = myLane->getShape().nearest_offset_to_point2D(oldPos, false);
        undoList->p_begin("position of " + toString(getTag()));
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, myPositionOverLane, true, toString(originalPosOverLane)));
        undoList->p_end();
    }
}


void
GNEAccess::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // Get shape of lane parent
    myShape = myLane->getShape();

    // set start position
    double startPosFixed;
    if(!canParse<double>(myPositionOverLane)) {
        startPosFixed = myLane->getParentEdge().getNBEdge()->getFinalLength();
    } else if(parse<double>(myPositionOverLane) < 0) {
        startPosFixed = 0;
    } else if (parse<double>(myPositionOverLane) > myLane->getParentEdge().getNBEdge()->getFinalLength()) {
        startPosFixed = myLane->getParentEdge().getNBEdge()->getFinalLength();
    } else {
        startPosFixed = parse<double>(myPositionOverLane);
    }

    // set end position
    double endPosFixed;
    if(!canParse<double>(myLength)) {
        endPosFixed = myLane->getParentEdge().getNBEdge()->getFinalLength();
    } else if((parse<double>(myPositionOverLane) + parse<double>(myLength)) < 0) {
        endPosFixed = 0;
    } else if ((parse<double>(myPositionOverLane) + parse<double>(myLength)) > myLane->getParentEdge().getNBEdge()->getFinalLength()) {
        endPosFixed = myLane->getParentEdge().getNBEdge()->getFinalLength();
    } else {
        endPosFixed = (parse<double>(myPositionOverLane) + parse<double>(myLength));
    }

    // Cut shape using as delimitators fixed start position and fixed end position
    myShape = myShape.getSubpart(startPosFixed, endPosFixed);
    
    // Get number of parts of the shape
    int numberOfSegments = (int)myShape.size() - 1;

    // If number of segments is more than 0
    if (numberOfSegments >= 0) {

        // Reserve memory (To improve efficiency)
        myShapeRotations.reserve(numberOfSegments);
        myShapeLengths.reserve(numberOfSegments);

        // For every part of the shape
        for (int i = 0; i < numberOfSegments; ++i) {

            // Obtain first position
            const Position& f = myShape[i];

            // Obtain next position
            const Position& s = myShape[i + 1];

            // Save distance between position into myShapeLengths
            myShapeLengths.push_back(f.distanceTo(s));

            // Save rotation (angle) of the vector constructed by points f and s
            myShapeRotations.push_back((double)atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
        }
    }

    // Set block icon position
    myBlockIconPosition = myShape.getLineCenter();

    // Set offset of the block icon
    myBlockIconOffset = Position(-0.75, 0);

    // Set block icon rotation, and using their rotation for draw logo
    setBlockIconRotation(myLane);

    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshElement(this);
}


Position 
GNEAccess::getPositionInView() const {
    if(myPositionOverLane.empty()) {
        return myLane->getShape().positionAtOffset(0);
    } else {
        return myLane->getShape().positionAtOffset(parse<double>(myPositionOverLane));
    }
}


void
GNEAccess::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(getTag());
    writeAttribute(device, SUMO_ATTR_ID);
    writeAttribute(device, SUMO_ATTR_LANE);
    writeAttribute(device, SUMO_ATTR_POSITION);
    writeAttribute(device, SUMO_ATTR_LENGTH);
    writeAttribute(device, SUMO_ATTR_FRIENDLY_POS);
    // Close tag
    device.closeTag();
}


double 
GNEAccess::getLength() const {
    if(myLength.empty()) {
        return myLane->getParentEdge().getNBEdge()->getFinalLength();
    } else {
        return parse<double>(myLength);
    }
}


bool 
GNEAccess::isAccessPositionFixed() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else {
        return (parse<double>(myPositionOverLane)>= 0) && ((parse<double>(myPositionOverLane) + parse<double>(myLength)) <= myLane->getParentEdge().getNBEdge()->getFinalLength());
    }
}


const std::string& 
GNEAccess::getParentName() const {
    return myLane->getID();
}


void
GNEAccess::drawGL(const GUIVisualizationSettings& s) const {
    // Start drawing adding an gl identificator
    glPushName(getGlID());

    // Add a draw matrix
    glPushMatrix();

    // Start with the drawing of the area traslating matrix to origing
    glTranslated(0, 0, getType());

    // Set color of the base
    if (isAttributeCarrierSelected()) {
        GLHelper::setColor(myViewNet->getNet()->selectedAdditionalColor);
    } else {
        GLHelper::setColor(RGBColor(0, 204, 204));
    }

    // Obtain exaggeration of the draw
    const double exaggeration = s.addSize.getExaggeration(s);

    // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, exaggeration);

    // Pop last matrix
    glPopMatrix();

    // Draw name if isn't being drawn for selecting
    if(!s.drawForSelecting) {
        drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    }

    // Pop name
    glPopName();
}


std::string
GNEAccess::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return myLane->getID();
        case SUMO_ATTR_POSITION:
            return toString(myPositionOverLane);
        case SUMO_ATTR_LENGTH:
            return toString(myLength);
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEAccess::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_FRIENDLY_POS:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_SELECTED:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEAccess::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_LANE:
            if (myViewNet->getNet()->retrieveLane(value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_POSITION:
            return canParse<double>(value);
        case SUMO_ATTR_LENGTH:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEAccess::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            myLane = changeLane(myLane, value);
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = value;
            break;
        case SUMO_ATTR_LENGTH:
            myLength = value;
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);;
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
            break;
        case GNE_ATTR_SELECTED:
            if(parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // After setting attribute always update Geometry
    updateGeometry();
}

/****************************************************************************/
