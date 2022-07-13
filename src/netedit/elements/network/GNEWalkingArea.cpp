/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEWalkingArea.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// A class for visualizing and editing WalkingAreas
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEWalkingArea.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEWalkingArea::GNEWalkingArea(GNEJunction* parentJunction, const std::string& ID) :
    GNENetworkElement(parentJunction->getNet(), ID, GLO_WALKINGAREA, SUMO_TAG_WALKINGAREA,
{}, {}, {}, {}, {}, {}),
myParentJunction(parentJunction),
myTesselation(ID, "", RGBColor::GREY, parentJunction->getNBNode()->getWalkingArea(ID).shape, false, true, 0) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEWalkingArea::~GNEWalkingArea() {
}


void
GNEWalkingArea::updateGeometry() {
    // Nothing to update
}


Position
GNEWalkingArea::getPositionInView() const {
    return myParentJunction->getPositionInView();
}


GNEMoveOperation*
GNEWalkingArea::getMoveOperation() {
    return nullptr;
}


void
GNEWalkingArea::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}


GNEJunction*
GNEWalkingArea::getParentJunction() const {
    return myParentJunction;
}


void
GNEWalkingArea::drawGL(const GUIVisualizationSettings& s) const {
    // check if boundary has to be drawn
    if (s.drawBoundaries) {
        GLHelper::drawBoundary(getCenteringBoundary());
    }
    // declare variables
    const Position mousePosition = myNet->getViewNet()->getPositionInformation();
    const double walkingAreaExaggeration = getExaggeration(s);
    // get walking area shape
    const auto& walkingAreaShape = myParentJunction->getNBNode()->getWalkingArea(getID()).shape;
    // only continue if exaggeration is greather than 0
    if ((walkingAreaShape.size() > 0) && s.drawCrossingsAndWalkingareas) {
        // push junction name
        GLHelper::pushName(getGlID());
        // push layer matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_WALKINGAREA);
        // set shape color
        const RGBColor walkingAreaColor = isAttributeCarrierSelected() ? RGBColor::BLUE : RGBColor::GREY;
        // recognize full transparency and simply don't draw
        if (walkingAreaColor.alpha() != 0) {
            // set color
            GLHelper::setColor(walkingAreaColor);
            // adjust shape to exaggeration
            if (((walkingAreaExaggeration > 1) || (myExaggeration > 1)) && (walkingAreaExaggeration != myExaggeration)) {
                myExaggeration = walkingAreaExaggeration;
                myTesselation.setShape(walkingAreaShape);
                myTesselation.getShapeRef().closePolygon();
                myTesselation.getShapeRef().scaleRelative(walkingAreaExaggeration);
                myTesselation.myTesselation.clear();
            }
            // first check if inner junction polygon can be drawn
            if (s.drawForPositionSelection) {
                // only draw a point if mouse is around shape
                if (myTesselation.getShape().around(mousePosition)) {
                    // push matrix
                    GLHelper::pushMatrix();
                    // move to mouse position
                    glTranslated(mousePosition.x(), mousePosition.y(), 0.1);
                    // draw a simple circle
                    GLHelper::drawFilledCircle(1, s.getCircleResolution());
                    // pop matrix
                    GLHelper::popMatrix();
                }
            } else if ((s.scale * walkingAreaExaggeration * myParentJunction->getMaxDrawingSize()) >= 40) {
                // draw shape with high detail
                myTesselation.drawTesselation(myTesselation.getShape());
            } else {
                // draw shape
                GLHelper::drawFilledPoly(myTesselation.getShape(), true);
            }
        }
        // pop layer Matrix
        GLHelper::popMatrix();
        // pop junction name
        GLHelper::popName();
        // check if dotted contour has to be drawn
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            GUIDottedGeometry::drawDottedContourClosedShape(GUIDottedGeometry::DottedContourType::INSPECT, s, walkingAreaShape,
                    (walkingAreaExaggeration >= 1) ? walkingAreaExaggeration : 1);
        }
        // check if dotted contour has to be drawn
        if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
            GUIDottedGeometry::drawDottedContourClosedShape(GUIDottedGeometry::DottedContourType::FRONT, s, walkingAreaShape,
                    (walkingAreaExaggeration >= 1) ? walkingAreaExaggeration : 1);
        }
    }
}


void
GNEWalkingArea::updateGLObject() {
    updateGeometry();
}


GUIGLObjectPopupMenu*
GNEWalkingArea::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // build position copy entry
    buildPositionCopyEntry(ret, app);
    // check if we're in supermode network
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        // create menu commands
        FXMenuCommand* mcCustomShape = GUIDesigns::buildFXMenuCommand(ret, "Set custom crossing shape", nullptr, &parent, MID_GNE_CROSSING_EDIT_SHAPE);
        // check if menu commands has to be disabled
        NetworkEditMode editMode = myNet->getViewNet()->getEditModes().networkEditMode;
        if ((editMode == NetworkEditMode::NETWORK_CONNECT) || (editMode == NetworkEditMode::NETWORK_TLS) || (editMode == NetworkEditMode::NETWORK_CREATE_EDGE)) {
            mcCustomShape->disable();
        }
    }
    return ret;
}


double
GNEWalkingArea::getExaggeration(const GUIVisualizationSettings& /*s*/) const {
    return 1;
}


void
GNEWalkingArea::updateCenteringBoundary(const bool /*updateGrid*/) {
    // in other case use boundary of parent junction
    const PositionVector& shape = myParentJunction->getNBNode()->getWalkingArea(getID()).shape;
    if (shape.size() == 0) {
        myBoundary = myParentJunction->getCenteringBoundary();
    } else {
        myBoundary = shape.getBoxBoundary();
        myBoundary.grow(10);
    }
}


std::string
GNEWalkingArea::getAttribute(SumoXMLAttr key) const {
    if (key == SUMO_ATTR_ID) {
        // for security purposes, avoid get WalkingArea if we want only the ID
        return getMicrosimID();
    }
    const auto& walkingArea = myParentJunction->getNBNode()->getWalkingArea(getMicrosimID());
    switch (key) {
        case SUMO_ATTR_WIDTH:
            return toString(walkingArea.width);
        case SUMO_ATTR_LENGTH:
            return toString(walkingArea.length);
        case SUMO_ATTR_CUSTOMSHAPE:
            if (walkingArea.hasCustomShape) {
                return toString(walkingArea.shape);
            }
            return "";
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEWalkingArea::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_CUSTOMSHAPE:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case GNE_ATTR_SELECTED:
            undoList->add(new GNEChange_Attribute(this, key, value), true);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEWalkingArea::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case GNE_ATTR_SELECTED:
            return true;
        default:
            return false;
    }
}


bool
GNEWalkingArea::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_CUSTOMSHAPE:
            return false;
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


const Parameterised::Map&
GNEWalkingArea::getACParametersMap() const {
    return GNEAttributeCarrier::PARAMETERS_EMPTY;
}

// ===========================================================================
// private
// ===========================================================================

void
GNEWalkingArea::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_CUSTOMSHAPE:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEWalkingArea::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void
GNEWalkingArea::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to commet
}

/****************************************************************************/
