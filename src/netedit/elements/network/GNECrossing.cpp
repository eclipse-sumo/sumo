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
/// @file    GNECrossing.cpp
/// @author  Jakob Erdmann
/// @date    June 2011
///
// A class for visualizing Inner Lanes (used when editing traffic lights)
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/moving/GNEMoveElementCrossing.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNECrossing.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNECrossing::GNECrossing(GNENet* net) :
    GNENetworkElement(net, "", SUMO_TAG_CROSSING),
    myMoveElementCrossing(new GNEMoveElementCrossing(this)),
    myTemplateNBCrossing(new NBNode::Crossing(nullptr, {}, 0, false, 0, 0, {})) {
}


GNECrossing::GNECrossing(GNEJunction* junction, std::vector<NBEdge*> crossingEdges) :
    GNENetworkElement(junction->getNet(), junction->getNBNode()->getCrossing(crossingEdges)->id, SUMO_TAG_CROSSING),
    myMoveElementCrossing(new GNEMoveElementCrossing(this)),
    myCrossingEdges(crossingEdges),
    myTemplateNBCrossing(nullptr) {
    // set parent
    setParent<GNEJunction*>(junction);
}


GNECrossing::~GNECrossing() {
    if (myTemplateNBCrossing) {
        delete myTemplateNBCrossing;
    }
}


GNEMoveElement*
GNECrossing::getMoveElement() const {
    return myMoveElementCrossing;
}


Parameterised*
GNECrossing::getParameters() {
    return myTemplateNBCrossing;
}


const Parameterised*
GNECrossing::getParameters() const {
    return myTemplateNBCrossing;
}


bool
GNECrossing::isNetworkElementValid() const {
    return getNBCrossing()->valid;
}


std::string
GNECrossing::getNetworkElementProblem() const {
    return TL("Crossing's edges don't support pedestrians");
}


const PositionVector&
GNECrossing::getCrossingShape() const {
    const auto crossing = getNBCrossing();
    return (crossing->customShape.size() > 0) ? crossing->customShape : crossing->shape;
}


void
GNECrossing::updateGeometry() {
    const auto crossing = getNBCrossing();
    // update crossing geometry
    myCrossingGeometry.updateGeometry(crossing->customShape.size() > 0 ?  crossing->customShape : crossing->shape);
}


Position
GNECrossing::getPositionInView() const {
    // currently unused
    return Position(0, 0);
}


bool
GNECrossing::checkDrawFromContour() const {
    return false;
}


bool
GNECrossing::checkDrawToContour() const {
    return false;
}


bool
GNECrossing::checkDrawRelatedContour() const {
    // check opened popup
    if (myNet->getViewNet()->getPopup()) {
        return myNet->getViewNet()->getPopup()->getGLObject() == this;
    }
    return false;
}


bool
GNECrossing::checkDrawOverContour() const {
    return false;
}


bool
GNECrossing::checkDrawDeleteContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in select mode
    if (editModes.isCurrentSupermodeNetwork() && (editModes.networkEditMode == NetworkEditMode::NETWORK_DELETE)) {
        return myNet->getViewNet()->checkOverLockedElement(this, mySelected);
    } else {
        return false;
    }
}


bool
GNECrossing::checkDrawDeleteContourSmall() const {
    return false;
}


bool
GNECrossing::checkDrawSelectContour() const {
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
GNECrossing::checkDrawMoveContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in move mode
    if (!myNet->getViewNet()->isCurrentlyMovingElements() && editModes.isCurrentSupermodeNetwork() &&
            (editModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) && myNet->getViewNet()->checkOverLockedElement(this, mySelected)) {
        // check if we're editing this network element
        const GNENetworkElement* editedNetworkElement = myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement();
        if (editedNetworkElement) {
            return editedNetworkElement == this;
        } else {
            // only move the first element
            return myNet->getViewNet()->getViewObjectsSelector().getGUIGlObjectFront() == this;
        }
    } else {
        return false;
    }
}


const std::vector<NBEdge*>&
GNECrossing::getCrossingEdges() const {
    return myCrossingEdges;
}


NBNode::Crossing*
GNECrossing::getNBCrossing() const {
    if (myTemplateNBCrossing) {
        return myTemplateNBCrossing;
    } else {
        return getParentJunctions().front()->getNBNode()->getCrossing(myCrossingEdges);
    }
}


void
GNECrossing::drawGL(const GUIVisualizationSettings& s) const {
    // continue depending of drawCrossing flag
    if (checkDrawCrossing(s)) {
        // get NBCrossing
        const auto NBCrossing = getNBCrossing();
        // get scaling depending if attribute carrier is selected
        const double crossingExaggeration = isAttributeCarrierSelected() ? s.selectorFrameScale : 1;
        // get width
        const double crossingWidth = NBCrossing->width * 0.5 * crossingExaggeration;
        // get detail level
        const auto d = s.getDetailLevel(crossingExaggeration);
        // check if draw geometry
        if (!s.drawForViewObjectsHandler) {
            // draw crossing
            drawCrossing(s, d, NBCrossing, crossingWidth, crossingExaggeration);
            // draw TLS Links No
            drawTLSLinkNo(s, NBCrossing);
            // draw crossing name
            if (s.cwaEdgeName.show(this)) {
                drawName(myCrossingGeometry.getShape().getCentroid(), s.scale, s.edgeName, 0, true);
            }
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), getPositionInView(), 1);
            // draw dotted contour depending if we're editing the custom shape
            const GNENetworkElement* editedNetworkElement = myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement();
            if (editedNetworkElement && (editedNetworkElement == this)) {
                // draw dotted contour geometry points
                myNetworkElementContour.drawDottedContourGeometryPoints(s, d, this, myCrossingGeometry.getShape(), s.neteditSizeSettings.crossingGeometryPointRadius,
                        crossingExaggeration, s.dottedContourSettings.segmentWidthSmall);
            } else {
                // draw dotted contour
                myNetworkElementContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
            }
        }
        // calculate contour
        calculateCrossingContour(s, d, crossingWidth, crossingExaggeration);
    }
}


void GNECrossing::deleteGLObject() {
    myNet->deleteNetworkElement(this, myNet->getViewNet()->getUndoList());
}


void
GNECrossing::updateGLObject() {
    updateGeometry();
}


void
GNECrossing::drawTLSLinkNo(const GUIVisualizationSettings& s, const NBNode::Crossing* crossing) const {
    // check if draw
    if (s.drawLinkTLIndex.show(getParentJunctions().front())) {
        // push matrix
        GLHelper::pushMatrix();
        // move to GLO_Crossing
        glTranslated(0, 0, GLO_CROSSING + 0.5);
        // make a copy of shape
        PositionVector shape = crossing->shape;
        // extrapolate
        shape.extrapolate(0.5); // draw on top of the walking area
        // get link indexes
        const int linkNo = crossing->tlLinkIndex;
        const int linkNo2 = crossing->tlLinkIndex2 > 0 ? crossing->tlLinkIndex2 : linkNo;
        // draw link indexes
        GLHelper::drawTextAtEnd(toString(linkNo2), shape, 0, s.drawLinkTLIndex, s.scale);
        GLHelper::drawTextAtEnd(toString(linkNo), shape.reverse(), 0, s.drawLinkTLIndex, s.scale);
        // push matrix
        GLHelper::popMatrix();
    }
}


GUIGLObjectPopupMenu*
GNECrossing::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    if (myShapeEdited) {
        return getShapeEditedPopUpMenu(app, parent, getNBCrossing()->customShape);
    } else {
        GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, this);
        // build common options
        buildPopUpMenuCommonOptions(ret, app, myNet->getViewNet(), myTagProperty->getTag(), mySelected);
        // check if we're in supermode network
        if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
            // create menu commands
            FXMenuCommand* mcCustomShape = GUIDesigns::buildFXMenuCommand(ret, TL("Set custom crossing shape"), nullptr, &parent, MID_GNE_CROSSING_EDIT_SHAPE);
            // check if menu commands has to be disabled
            NetworkEditMode editMode = myNet->getViewNet()->getEditModes().networkEditMode;
            if ((editMode == NetworkEditMode::NETWORK_CONNECT) || (editMode == NetworkEditMode::NETWORK_TLS) || (editMode == NetworkEditMode::NETWORK_CREATE_EDGE)) {
                mcCustomShape->disable();
            }
        }
        return ret;
    }
}


Boundary
GNECrossing::getCenteringBoundary() const {
    return myNetworkElementContour.getContourBoundary();
}


void
GNECrossing::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to update
}


std::string
GNECrossing::getAttribute(SumoXMLAttr key) const {
    const auto crossing = getNBCrossing();
    switch (key) {
        case SUMO_ATTR_ID:
            // get attribute requires a special case
            if (crossing) {
                return crossing->id;
            } else {
                return "Temporal Unreferenced";
            }
        case SUMO_ATTR_WIDTH:
            return toString(crossing->customWidth);
        case SUMO_ATTR_PRIORITY:
            return crossing->priority ? "true" : "false";
        case SUMO_ATTR_EDGES:
            return toString(crossing->edges);
        case SUMO_ATTR_TLLINKINDEX:
            return toString(crossing->customTLIndex < 0 ? crossing->tlLinkIndex : crossing->customTLIndex);
        case SUMO_ATTR_TLLINKINDEX2:
            return toString(crossing->customTLIndex2 < 0 ? crossing->tlLinkIndex2 : crossing->customTLIndex2);
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_CUSTOMSHAPE:
            return toString(crossing->customShape);
        default:
            return getCommonAttribute(key);
    }
}


double
GNECrossing::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


Position
GNECrossing::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNECrossing::getAttributePositionVector(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_CUSTOMSHAPE:
            return getNBCrossing()->customShape;
        default:
            return getCommonAttributePositionVector(key);
    }
}


void
GNECrossing::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_PRIORITY:
        case SUMO_ATTR_TLLINKINDEX:
        case SUMO_ATTR_TLLINKINDEX2:
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_CUSTOMSHAPE:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList, true);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNECrossing::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            // id isn't editable
            return false;
        case SUMO_ATTR_TLLINKINDEX:
        case SUMO_ATTR_TLLINKINDEX2:
            return (getNBCrossing()->tlID != "");
        default:
            return true;
    }
}


bool
GNECrossing::isValid(SumoXMLAttr key, const std::string& value) {
    const auto crossing = getNBCrossing();
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        case SUMO_ATTR_EDGES:
            if (canParse<std::vector<GNEEdge*> >(myNet, value, false)) {
                // parse edges and save their IDs in a set
                std::vector<GNEEdge*> parsedEdges = parse<std::vector<GNEEdge*> >(myNet, value);
                EdgeVector nbEdges;
                for (const auto& edge : parsedEdges) {
                    nbEdges.push_back(edge->getNBEdge());
                }
                std::sort(nbEdges.begin(), nbEdges.end());
                //
                EdgeVector originalEdges = crossing->edges;
                std::sort(originalEdges.begin(), originalEdges.end());
                // return true if we're setting the same edges
                if (toString(nbEdges) == toString(originalEdges)) {
                    return true;
                } else {
                    return !getParentJunctions().front()->getNBNode()->checkCrossingDuplicated(nbEdges);
                }
            } else {
                return false;
            }
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value) && ((parse<double>(value) > 0) || (parse<double>(value) == -1)); // can not be 0, or -1 (it means default)
        case SUMO_ATTR_PRIORITY:
            return canParse<bool>(value);
        case SUMO_ATTR_TLLINKINDEX:
        case SUMO_ATTR_TLLINKINDEX2:
            // -1 means that tlLinkIndex2 takes on the same value as tlLinkIndex when setting indices
            return (isAttributeEnabled(key) &&
                    canParse<int>(value)
                    && (parse<double>(value) >= 0 || parse<double>(value) == -1)
                    && getParentJunctions().front()->getNBNode()->getControllingTLS().size() > 0
                    && (*getParentJunctions().front()->getNBNode()->getControllingTLS().begin())->getMaxValidIndex() >= parse<int>(value));
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_CUSTOMSHAPE:
            // empty shapes are allowed
            return canParse<PositionVector>(value);
        default:
            return isValid(key, value);
    }
}


bool
GNECrossing::checkEdgeBelong(GNEEdge* edge) const {
    const auto crossing = getNBCrossing();
    if (std::find(crossing->edges.begin(), crossing->edges.end(), edge->getNBEdge()) !=  crossing->edges.end()) {
        return true;
    } else {
        return false;
    }
}


bool
GNECrossing::checkEdgeBelong(const std::vector<GNEEdge*>& edges) const {
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

bool
GNECrossing::checkDrawCrossing(const GUIVisualizationSettings& s) const {
    // don't draw in supermode data
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        return false;
    }
    // check shape rotations
    if (myCrossingGeometry.getShapeRotations().empty()) {
        return false;
    }
    // check shape lengths
    if (myCrossingGeometry.getShapeLengths().empty()) {
        return false;
    }
    return s.drawCrossingsAndWalkingareas;
}


void
GNECrossing::drawCrossing(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                          const NBNode::Crossing* crossing, const double width, const double exaggeration) const {
    // don't draw crossing in TLS Mode
    if (myNet->getViewNet()->getEditModes().networkEditMode != NetworkEditMode::NETWORK_TLS) {
        // get color
        RGBColor crossingColor = getCrossingColor(s, crossing);
        // push layer matrix
        GLHelper::pushMatrix();
        // translate to front
        drawInLayer(GLO_CROSSING);
        // set color
        GLHelper::setColor(crossingColor);
        // draw depending of level of detail
        if (d <= GUIVisualizationSettings::Detail::JunctionElementDetails) {
            drawCrossingDetailed(width, exaggeration);
        } else {
            GUIGeometry::drawGeometry(d, myCrossingGeometry, width);
        }
        // draw shape points only in Network supermode
        if (myShapeEdited && myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
                s.drawMovingGeometryPoint(exaggeration, s.neteditSizeSettings.crossingGeometryPointRadius)) {
            // color
            const RGBColor darkerColor = crossingColor.changedBrightness(-32);
            // draw on top of of the white area between the rails
            glTranslated(0, 0, 0.2);
            // set color
            GLHelper::setColor(darkerColor);
            // draw shape
            GUIGeometry::drawGeometry(d, myCrossingGeometry.getShape(), s.neteditSizeSettings.crossingGeometryPointRadius * 0.4);
            // draw geometry points
            GUIGeometry::drawGeometryPoints(d, myCrossingGeometry.getShape(), darkerColor,
                                            s.neteditSizeSettings.crossingGeometryPointRadius, exaggeration,
                                            myNet->getViewNet()->getNetworkViewOptions().editingElevation());
        }
        // pop layer matrix
        GLHelper::popMatrix();
    }
}


RGBColor
GNECrossing::getCrossingColor(const GUIVisualizationSettings& s, const NBNode::Crossing* crossing) const {
    if (myShapeEdited) {
        return s.colorSettings.editShapeColor;
    } else if (drawUsingSelectColor()) {
        return s.colorSettings.selectedCrossingColor;
    } else if (!crossing->valid) {
        return s.colorSettings.crossingInvalidColor;
    } else if (crossing->priority) {
        return s.colorSettings.crossingPriorityColor;
    } else if (myNet->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        return s.laneColorer.getSchemes()[0].getColor(8);
    } else {
        return s.colorSettings.crossingColor;
    }
}


void
GNECrossing::drawCrossingDetailed(const double width, const double exaggeration) const {
    // get length and spacing
    const double length = 0.5 * exaggeration;
    const double spacing = 1.0 * exaggeration;
    // push rail matrix
    GLHelper::pushMatrix();
    // draw on top of of the white area between the rails
    glTranslated(0, 0, 0.1);
    for (int i = 0; i < (int)myCrossingGeometry.getShape().size() - 1; i++) {
        // push draw matrix
        GLHelper::pushMatrix();
        // translate and rotate
        glTranslated(myCrossingGeometry.getShape()[i].x(), myCrossingGeometry.getShape()[i].y(), 0.0);
        glRotated(myCrossingGeometry.getShapeRotations()[i], 0, 0, 1);
        // draw crossing depending if isn't being drawn for selecting
        for (double t = 0; t < myCrossingGeometry.getShapeLengths()[i]; t += spacing) {
            glBegin(GL_QUADS);
            glVertex2d(-width, -t);
            glVertex2d(-width, -t - length);
            glVertex2d(width, -t - length);
            glVertex2d(width, -t);
            glEnd();
        }
        // pop draw matrix
        GLHelper::popMatrix();
    }
    // pop rail matrix
    GLHelper::popMatrix();
}


void
GNECrossing::calculateCrossingContour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                      const double width, const double exaggeration) const {
    // first check if junction parent was inserted with full boundary
    if (!gViewObjectsHandler.checkBoundaryParentObject(this, getType(), getParentJunctions().front())) {
        // check if calculate contour for geometry points
        if (myShapeEdited) {
            myNetworkElementContour.calculateContourAllGeometryPoints(s, d, this, myCrossingGeometry.getShape(),
                    getType(), s.neteditSizeSettings.crossingGeometryPointRadius, exaggeration, true);
        } else {
            // in move mode, add to selected object if this is the edited element
            const auto& editModes = myNet->getViewNet()->getEditModes();
            const bool addToSelectedObjects = (editModes.isCurrentSupermodeNetwork() && editModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) ?
                                              (myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement() == this) : true;
            // calculate contour and
            myNetworkElementContour.calculateContourExtrudedShape(s, d, this, myCrossingGeometry.getShape(), getType(),
                    width, exaggeration, true, true, 0, nullptr, getParentJunctions().front(), addToSelectedObjects);
        }
    }
}


void
GNECrossing::setAttribute(SumoXMLAttr key, const std::string& value) {
    const auto crossing = getNBCrossing();
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case SUMO_ATTR_EDGES: {
            // obtain GNEEdges
            std::vector<GNEEdge*> edges = parse<std::vector<GNEEdge*> >(myNet, value);
            // remove NBEdges of crossing
            crossing->edges.clear();
            // set NBEdge of every GNEEdge into Crossing Edges
            for (auto i : edges) {
                crossing->edges.push_back(i->getNBEdge());
            }
            // sort new edges
            std::sort(crossing->edges.begin(), crossing->edges.end());
            // change myCrossingEdges by the new edges
            myCrossingEdges = crossing->edges;
            // update geometry of parent junction
            getParentJunctions().front()->updateGeometry();
            break;
        }
        case SUMO_ATTR_WIDTH:
            // Change width an refresh element
            crossing->customWidth = parse<double>(value);
            break;
        case SUMO_ATTR_PRIORITY:
            crossing->priority = parse<bool>(value);
            break;
        case SUMO_ATTR_TLLINKINDEX:
            crossing->customTLIndex = parse<int>(value);
            // make new value visible immediately
            crossing->tlLinkIndex = crossing->customTLIndex;
            break;
        case SUMO_ATTR_TLLINKINDEX2:
            crossing->customTLIndex2 = parse<int>(value);
            // make new value visible immediately
            crossing->tlLinkIndex2 = crossing->customTLIndex2;
            break;
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_CUSTOMSHAPE:
            // set custom shape
            crossing->customShape = parse<PositionVector>(value);
            break;
        default:
            setCommonAttribute(key, value);
            break;
    }
    // Crossing are a special case and we need ot update geometry of junction instead of crossing
    if ((getParentJunctions().size() > 0) && (key != SUMO_ATTR_ID) && (key != GNE_ATTR_PARAMETERS) && (key != GNE_ATTR_SELECTED)) {
        getParentJunctions().front()->updateGeometry();
    }
    // invalidate demand path calculator
    myNet->getDemandPathManager()->getPathCalculator()->invalidatePathCalculator();
}

/****************************************************************************/
