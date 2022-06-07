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

GNEWalkingArea::GNEWalkingArea(GNENet* net) :
    GNENetworkElement(net, "", GLO_CROSSING, SUMO_TAG_CROSSING,
        {}, {}, {}, {}, {}, {}),
    myParentJunction(nullptr),
    myTesselation("", "", RGBColor::GREY, {}, false, true, 0),
    myTemplateNBWalkingArea(new NBNode::WalkingArea("", 1)) {
    // reset default values
    resetDefaultValues();
}

GNEWalkingArea::GNEWalkingArea(GNEJunction* parentJunction, const std::string &ID) :
    GNENetworkElement(parentJunction->getNet(), ID, GLO_CROSSING, SUMO_TAG_WALKINGAREA,
        {}, {}, {}, {}, {}, {}),
    myParentJunction(parentJunction),
    myTesselation(ID, "", RGBColor::GREY, parentJunction->getNBNode()->getWalkingArea(ID).shape, false, true, 0),
    myTemplateNBWalkingArea(nullptr) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEWalkingArea::~GNEWalkingArea() {
    if (myTemplateNBWalkingArea) {
        delete myTemplateNBWalkingArea;
    }
}


const PositionVector&
GNEWalkingArea::getWalkingAreaShape() const {
    return getNBWalkingArea().shape;
}


void
GNEWalkingArea::updateGeometry() {
    // Nothing to update
}


Position
GNEWalkingArea::getPositionInView() const {
    // currently unused
    return Position(0, 0);
}


GNEMoveOperation*
GNEWalkingArea::getMoveOperation() {
    // edit depending if shape is being edited
    if (isShapeEdited()) {
        // calculate move shape operation
        return calculateMoveShapeOperation(getWalkingAreaShape(), myNet->getViewNet()->getPositionInformation(),
                                           myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.crossingGeometryPointRadius, true);
    } else {
        return nullptr;
    }
}


void
GNEWalkingArea::removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) {
    // edit depending if shape is being edited
    if (isShapeEdited()) {
        // get original shape
        PositionVector shape = getWalkingAreaShape();
        // check shape size
        if (shape.size() > 2) {
            // obtain index
            int index = shape.indexOfClosest(clickedPosition);
            // get snap radius
            const double snap_radius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.crossingGeometryPointRadius;
            // check if we have to create a new index
            if ((index != -1) && shape[index].distanceSquaredTo2D(clickedPosition) < (snap_radius * snap_radius)) {
                // remove geometry point
                shape.erase(shape.begin() + index);
                // commit new shape
                undoList->begin(GUIIcon::CROSSING, "remove geometry point of " + getTagStr());
                undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_CUSTOMSHAPE, toString(shape)));
                undoList->end();
            }
        }
    }
}


GNEJunction*
GNEWalkingArea::getParentJunction() const {
    return myParentJunction;
}


const NBNode::WalkingArea&
GNEWalkingArea::getNBWalkingArea() const {
    if (myTemplateNBWalkingArea) {
        return *myTemplateNBWalkingArea;
    } else {
        return myParentJunction->getNBNode()->getWalkingArea(getID());
    }
}


void
GNEWalkingArea::drawGL(const GUIVisualizationSettings& s) const {
    // check if boundary has to be drawn
    if (s.drawBoundaries) {
        GLHelper::drawBoundary(getCenteringBoundary());
    }
    // check if draw start und end
    const bool drawExtremeSymbols = myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
        myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE;
    // declare variables
    const Position mousePosition = myNet->getViewNet()->getPositionInformation();
    const double walkingAreaExaggeration = getExaggeration(s);
    // only continue if exaggeration is greather than 0
    if ((getWalkingAreaShape().size() > 0) && s.drawCrossingsAndWalkingareas) {
        // push junction name
        GLHelper::pushName(getGlID());
        // push layer matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_JUNCTION);
        // set shape color
        const RGBColor junctionShapeColor = setColor(s);
        // recognize full transparency and simply don't draw
        if (junctionShapeColor.alpha() != 0) {
            // set color
            GLHelper::setColor(junctionShapeColor);
            // adjust shape to exaggeration
            if (((walkingAreaExaggeration > 1) || (myExaggeration > 1)) && (walkingAreaExaggeration != myExaggeration)) {
                myExaggeration = walkingAreaExaggeration;
                myTesselation.setShape(getWalkingAreaShape());
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
            // draw shape points only in Network supemode
            if (myShapeEdited && s.drawMovingGeometryPoint(walkingAreaExaggeration, s.neteditSizeSettings.junctionGeometryPointRadius) && myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
                // set color
                const RGBColor darkerColor = junctionShapeColor.changedBrightness(-32);
                // calculate geometry
                GUIGeometry junctionGeometry;
                // obtain junction Shape
                PositionVector junctionOpenShape = getWalkingAreaShape();
                // adjust shape to exaggeration
                if (walkingAreaExaggeration > 1) {
                    junctionOpenShape.scaleRelative(walkingAreaExaggeration);
                }
                // update geometry
                junctionGeometry.updateGeometry(junctionOpenShape);
                // set color
                GLHelper::setColor(darkerColor);
                // draw shape
                GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), junctionGeometry, s.neteditSizeSettings.junctionGeometryPointRadius * 0.5);
                // draw geometry points
                GUIGeometry::drawGeometryPoints(s, myNet->getViewNet()->getPositionInformation(), junctionOpenShape, darkerColor, RGBColor::BLACK,
                    s.neteditSizeSettings.junctionGeometryPointRadius, walkingAreaExaggeration, false, drawExtremeSymbols);
                // draw moving hint
                if (myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE) {
                    GUIGeometry::drawMovingHint(s, myNet->getViewNet()->getPositionInformation(), junctionOpenShape, darkerColor,
                        s.neteditSizeSettings.junctionGeometryPointRadius, walkingAreaExaggeration);
                }
            }
        }
        // pop layer Matrix
        GLHelper::popMatrix();
        // pop junction name
        GLHelper::popName();

        // draw lock icon
        // GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), 1);
        // check if dotted contour has to be drawn
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            GUIDottedGeometry::drawDottedContourClosedShape(GUIDottedGeometry::DottedContourType::INSPECT, s, getWalkingAreaShape(),
                (walkingAreaExaggeration >= 1) ? walkingAreaExaggeration : 1);
        }
        // check if dotted contour has to be drawn
        if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
            GUIDottedGeometry::drawDottedContourClosedShape(GUIDottedGeometry::DottedContourType::FRONT, s, getWalkingAreaShape(),
                (walkingAreaExaggeration >= 1) ? walkingAreaExaggeration : 1);
        }
    }
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
    myBoundary = myParentJunction->getCenteringBoundary();
}


std::string
GNEWalkingArea::getAttribute(SumoXMLAttr key) const {
    const auto walkingArea = getNBWalkingArea();
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
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
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case SUMO_ATTR_WIDTH:
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
        case SUMO_ATTR_ID:
            // id isn't editable
            return false;
        default:
            return true;
    }
}


bool
GNEWalkingArea::isAttributeComputed(SumoXMLAttr /*key*/) const {
    return false;
}


bool
GNEWalkingArea::isValid(SumoXMLAttr key, const std::string& value) {
    const auto walkingArea = getNBWalkingArea();
    switch (key) {
        case SUMO_ATTR_ID:
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


bool
GNEWalkingArea::checkEdgeBelong(GNEEdge* edge) const {
/*
    const auto walkingArea = getNBWalkingArea();
    if (std::find(crossing->edges.begin(), crossing->edges.end(), edge->getNBEdge()) !=  crossing->edges.end()) {
        return true;
    } else {
        return false;
    }
*/
    return false;
}


bool
GNEWalkingArea::checkEdgeBelong(const std::vector<GNEEdge*>& edges) const {
    for (auto i : edges) {
        if (checkEdgeBelong(i)) {
            return true;
        }
    }
    return false;
}

// ===========================================================================
// private
// ===========================================================================

void
GNEWalkingArea::setAttribute(SumoXMLAttr key, const std::string& value) {
    const auto crossing = getNBWalkingArea();
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEWalkingArea::setMoveShape(const GNEMoveResult& moveResult) {
    // set custom shape
/*
    getNBWalkingArea()->customShape = moveResult.shapeToUpdate;
*/
    // update geometry
    updateGeometry();
}


void
GNEWalkingArea::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // commit new shape
/*
    undoList->begin(GUIIcon::CROSSING, "moving " + toString(SUMO_ATTR_CUSTOMSHAPE) + " of " + getTagStr());
    undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_CUSTOMSHAPE, toString(moveResult.shapeToUpdate)));
    undoList->end();
*/
}


RGBColor
GNEWalkingArea::setColor(const GUIVisualizationSettings& /*s*/) const {
    // return color
    return RGBColor::GREY;
}

/****************************************************************************/
