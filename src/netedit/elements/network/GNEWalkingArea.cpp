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
/// @file    GNEWalkingArea.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// A class for visualizing and editing WalkingAreas
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEWalkingArea.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEWalkingArea::GNEWalkingArea(GNEJunction* junction, const std::string& ID) :
    GNENetworkElement(junction->getNet(), ID, SUMO_TAG_WALKINGAREA),
    myTesselation(ID, "", RGBColor::GREY, junction->getNBNode()->getWalkingArea(ID).shape, false, true, 0) {
    // set parent
    setParent<GNEJunction*>(junction);
}


GNEWalkingArea::~GNEWalkingArea() {
}


GNEMoveElement*
GNEWalkingArea::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNEWalkingArea::getParameters() {
    return nullptr;
}


const Parameterised*
GNEWalkingArea::getParameters() const {
    return nullptr;
}


void
GNEWalkingArea::updateGeometry() {
    // Nothing to update
}


Position
GNEWalkingArea::getPositionInView() const {
    return getParentJunctions().front()->getPositionInView();
}


bool
GNEWalkingArea::checkDrawFromContour() const {
    return false;
}


bool
GNEWalkingArea::checkDrawToContour() const {
    return false;
}


bool
GNEWalkingArea::checkDrawRelatedContour() const {
    // check opened popup
    if (myNet->getViewNet()->getPopup()) {
        return myNet->getViewNet()->getPopup()->getGLObject() == this;
    }
    return false;
}


bool
GNEWalkingArea::checkDrawOverContour() const {
    return false;
}


bool
GNEWalkingArea::checkDrawDeleteContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in delete mode
    if (editModes.isCurrentSupermodeNetwork() && (editModes.networkEditMode == NetworkEditMode::NETWORK_DELETE)) {
        return myNet->getViewNet()->checkOverLockedElement(this, mySelected);
    } else {
        return false;
    }
}


bool
GNEWalkingArea::checkDrawDeleteContourSmall() const {
    return false;
}


bool
GNEWalkingArea::checkDrawSelectContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in select mode
    if (editModes.isCurrentSupermodeNetwork() && (editModes.networkEditMode == NetworkEditMode::NETWORK_SELECT)) {
        return myNet->getViewNet()->checkOverLockedElement(this, mySelected);
    } else {
        return false;
    }
}


bool
GNEWalkingArea::checkDrawMoveContour() const {
    return false;
}


NBNode::WalkingArea&
GNEWalkingArea::getNBWalkingArea() const {
    return getParentJunctions().front()->getNBNode()->getWalkingArea(getMicrosimID());
}


void
GNEWalkingArea::drawGL(const GUIVisualizationSettings& s) const {
    // declare variables
    const double walkingAreaExaggeration = getExaggeration(s);
    // get walking area shape
    const auto& walkingAreaShape = getParentJunctions().front()->getNBNode()->getWalkingArea(getID()).shape;
    // only continue if exaggeration is greater than 0 and junction's shape is greater than 4
    if ((getParentJunctions().front()->getNBNode()->getShape().area() > 4) &&
            (walkingAreaShape.size() > 0) && s.drawCrossingsAndWalkingareas) {
        // don't draw this walking area if we're editing their junction parent
        const GNENetworkElement* editedNetworkElement = myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement();
        if (!editedNetworkElement || (editedNetworkElement != getParentJunctions().front())) {
            const auto contourMode = drawInContourMode();
            // get detail level
            const auto d = s.getDetailLevel(walkingAreaExaggeration);
            // draw geometry only if we'rent in drawForObjectUnderCursor mode
            if (!s.drawForViewObjectsHandler) {
                // draw walking area
                if (!contourMode) {
                    drawWalkingArea(s, d, walkingAreaShape, walkingAreaExaggeration);
                }
                // draw walkingArea name
                if (s.cwaEdgeName.show(this)) {
                    drawName(walkingAreaShape.getCentroid(), s.scale, s.edgeName, 0, true);
                }
                // draw dotted contour
                if (contourMode) {
                    myNetworkElementContour.drawDottedContour(s, GUIDottedGeometry::DottedContourType::WALKINGAREA, s.dottedContourSettings.segmentWidth, false);
                } else {
                    myNetworkElementContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
                }
            }
            // draw dotted contour (except in contour mode) checking if junction parent was inserted with full boundary
            myNetworkElementContour.calculateContourClosedShape(s, d, this, walkingAreaShape, getType(),
                    walkingAreaExaggeration, getParentJunctions().front(), !contourMode);
        }
    }
}


void
GNEWalkingArea::deleteGLObject() {
    // currently WalkingAreas cannot be removed
}


void
GNEWalkingArea::updateGLObject() {
    updateGeometry();
}


GUIGLObjectPopupMenu*
GNEWalkingArea::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // create popup
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, this);
    // build common options
    buildPopUpMenuCommonOptions(ret, app, myNet->getViewNet(), myTagProperty->getTag(), mySelected);
    // check if we're in supermode network
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        // create menu commands
        FXMenuCommand* mcCustomShape = GUIDesigns::buildFXMenuCommand(ret, "Set custom WalkingArea shape", nullptr, &parent, MID_GNE_WALKINGAREA_EDIT_SHAPE);
        // check if menu commands has to be disabled
        NetworkEditMode editMode = myNet->getViewNet()->getEditModes().networkEditMode;
        if ((editMode == NetworkEditMode::NETWORK_CONNECT) || (editMode == NetworkEditMode::NETWORK_TLS) || (editMode == NetworkEditMode::NETWORK_CREATE_EDGE)) {
            mcCustomShape->disable();
        }
        // disabled for release 1.15
        mcCustomShape->disable();
    }
    return ret;
}


Boundary
GNEWalkingArea::getCenteringBoundary() const {
    return myNetworkElementContour.getContourBoundary();
}


void
GNEWalkingArea::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to update
}


std::string
GNEWalkingArea::getAttribute(SumoXMLAttr key) const {
    if (key == SUMO_ATTR_ID) {
        // for security purposes, avoid get WalkingArea if we want only the ID
        return getMicrosimID();
    }
    const auto& walkingArea = getNBWalkingArea();
    switch (key) {
        case SUMO_ATTR_WIDTH:
            return toString(walkingArea.width);
        case SUMO_ATTR_LENGTH:
            return toString(walkingArea.length);
        case SUMO_ATTR_SHAPE:
            return toString(walkingArea.shape);
        default:
            return getCommonAttribute(key);
    }
}


double
GNEWalkingArea::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


Position
GNEWalkingArea::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNEWalkingArea::getAttributePositionVector(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_SHAPE:
            return getNBWalkingArea().shape;
        default:
            return getCommonAttributePositionVector(key);
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
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_SHAPE:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList, true);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
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
            return false;
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_LENGTH:
            return canParse<double>(value) && (parse<double>(value) > 0);
        case SUMO_ATTR_SHAPE:
            if (canParse<PositionVector>(value)) {
                return parse<PositionVector>(value).size() > 0;
            } else {
                return false;
            }
        default:
            return isCommonAttributeValid(key, value);
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEWalkingArea::drawWalkingArea(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                const PositionVector& shape, const double exaggeration) const {
    // adjust shape to exaggeration
    if (((exaggeration > 1) || (myExaggeration > 1)) && (exaggeration != myExaggeration)) {
        myExaggeration = exaggeration;
        myTesselation.setShape(shape);
        myTesselation.getShapeRef().closePolygon();
        myTesselation.getShapeRef().scaleRelative(exaggeration);
        myTesselation.myTesselation.clear();
    }
    // push layer matrix
    GLHelper::pushMatrix();
    // translate to front
    drawInLayer(GLO_WALKINGAREA, 0.1);
    // set color
    if (myShapeEdited) {
        GLHelper::setColor(s.colorSettings.editShapeColor);
    } else if (isAttributeCarrierSelected()) {
        GLHelper::setColor(RGBColor::BLUE);
    } else {
        GLHelper::setColor(s.junctionColorer.getScheme().getColor(6));
    }
    // check if draw walking area tesselated or contour
    if (drawInContourMode()) {
        myInnenContour.drawInnenContourClosed(s, d, shape, exaggeration, s.dottedContourSettings.segmentWidth);
    } else {
        drawTesselatedWalkingArea(s, d);
    }
    // pop layer Matrix
    GLHelper::popMatrix();
}


bool
GNEWalkingArea::drawInContourMode() const {
    const auto& modes = myNet->getViewNet()->getEditModes();
    // check modes
    if (!modes.isCurrentSupermodeNetwork()) {
        return true;
    } else if (modes.networkEditMode == NetworkEditMode::NETWORK_MOVE) {
        return true;
    } else if (modes.networkEditMode == NetworkEditMode::NETWORK_DELETE) {
        return true;
    } else if (modes.networkEditMode == NetworkEditMode::NETWORK_CONNECT) {
        return true;
    } else {
        return false;
    }
}


void
GNEWalkingArea::drawTesselatedWalkingArea(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d) const {
    // check if draw polygon or tesselation
    if (d <= GUIVisualizationSettings::Detail::JunctionElementDetails) {
        // draw shape with high detail
        myTesselation.drawTesselation(myTesselation.getShape());
    } else {
        // draw shape
        GLHelper::drawFilledPoly(myTesselation.getShape(), true);
    }
    // draw shape points only in Network supemode
    if (myShapeEdited && s.drawMovingGeometryPoint(1, s.neteditSizeSettings.junctionGeometryPointRadius)) {
        // draw geometry points
        GUIGeometry::drawGeometryPoints(d, myTesselation.getShape(), GLHelper::getColor().changedBrightness(-32),
                                        s.neteditSizeSettings.crossingGeometryPointRadius, 1,
                                        myNet->getViewNet()->getNetworkViewOptions().editingElevation());
    }
}


void
GNEWalkingArea::setAttribute(SumoXMLAttr key, const std::string& value) {
    auto& walkingArea = getNBWalkingArea();
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case SUMO_ATTR_WIDTH:
            walkingArea.width = parse<double>(value);
            break;
        case SUMO_ATTR_LENGTH:
            walkingArea.length = parse<double>(value);
            break;
        case SUMO_ATTR_SHAPE:
            walkingArea.shape = parse<PositionVector>(value);
            walkingArea.hasCustomShape = true;
            break;
        default:
            setCommonAttribute(key, value);
            break;
    }
}

/****************************************************************************/
