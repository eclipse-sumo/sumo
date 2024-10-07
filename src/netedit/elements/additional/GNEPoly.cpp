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
/// @file    GNEPoly.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
///
// A class for visualizing and editing POIS in netedit (adapted from
// GUIPolygon and NLHandler)
/****************************************************************************/
#include <config.h>

#include <string>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GLHelper.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEPoly.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEPoly::GNEPoly(SumoXMLTag tag, GNENet* net) :
    TesselatedPolygon("", "", RGBColor::BLACK, {}, false, false, 0, 0, 0, "", false, "", Parameterised::Map()),
                  GNEAdditional("", net, GLO_POLYGON, tag, GUIIconSubSys::getIcon(GUIIcon::POLY), "", {}, {}, {}, {}, {}, {}),
mySimplifiedShape(false) {
    // reset default values
    resetDefaultValues();
}


GNEPoly::GNEPoly(GNENet* net, const std::string& id, const std::string& type, const PositionVector& shape, bool geo, bool fill, double lineWidth,
                 const RGBColor& color, double layer, double angle, const std::string& imgFile, bool relativePath, const std::string& name,
                 const Parameterised::Map& parameters) :
    TesselatedPolygon(id, type, color, shape, geo, fill, lineWidth, layer, angle, imgFile, relativePath, name, parameters),
    GNEAdditional(id, net, GLO_POLYGON, SUMO_TAG_POLY, GUIIconSubSys::getIcon(GUIIcon::POLY), "", {}, {}, {}, {}, {}, {}),
mySimplifiedShape(false) {
    // check if imgFile is valid
    if (!imgFile.empty() && GUITexturesHelper::getTextureID(imgFile) == -1) {
        setShapeImgFile("");
    }
    // set GEO shape
    myGeoShape = myShape;
    if (geo) {
        for (int i = 0; i < (int) myGeoShape.size(); i++) {
            GeoConvHelper::getFinal().x2cartesian_const(myShape[i]);
        }
    } else {
        for (int i = 0; i < (int) myGeoShape.size(); i++) {
            GeoConvHelper::getFinal().cartesian2geo(myGeoShape[i]);
        }
    }
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
    // update geometry
    updateGeometry();
}


GNEPoly::GNEPoly(SumoXMLTag tag, GNENet* net, const std::string& id, const PositionVector& shape, bool geo, const std::string& name,
                 const Parameterised::Map& parameters) :
    TesselatedPolygon(id, getJuPedSimType(tag), getJuPedSimColor(tag), shape, geo, getJuPedSimFill(tag), 1,
                      getJuPedSimLayer(tag), 0, "", false, name, parameters),
    GNEAdditional(id, net, getJuPedSimGLO(tag), tag, getJuPedSimIcon(tag), "", {}, {}, {}, {}, {}, {}),
mySimplifiedShape(false) {
    // set GEO shape
    myGeoShape = myShape;
    if (geo) {
        for (int i = 0; i < (int) myGeoShape.size(); i++) {
            GeoConvHelper::getFinal().x2cartesian_const(myShape[i]);
        }
    } else {
        for (int i = 0; i < (int) myGeoShape.size(); i++) {
            GeoConvHelper::getFinal().cartesian2geo(myGeoShape[i]);
        }
    }
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
    // update geometry
    updateGeometry();
}


GNEPoly::~GNEPoly() {}


GNEMoveOperation*
GNEPoly::getMoveOperation() {
    // edit depending if shape is blocked
    if (myNet->getViewNet()->getViewParent()->getMoveFrame()->getNetworkModeOptions()->getMoveWholePolygons()) {
        // move entire shape
        return new GNEMoveOperation(this, myShape);
    } else {
        // continue depending of tag
        switch (getTagProperty().getTag()) {
            case GNE_TAG_JPS_WALKABLEAREA:
            case GNE_TAG_JPS_OBSTACLE:
                // calculate move shape operation maintain shape closed
                return calculateMoveShapeOperation(this, myShape, true);
            default:
                // calculate move shape operation
                return calculateMoveShapeOperation(this, myShape, false);
        }
    }
}


void
GNEPoly::removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) {
    // get original shape
    PositionVector shape = myShape;
    // check shape size
    if (shape.size() > 2) {
        // obtain index
        int index = shape.indexOfClosest(clickedPosition);
        // get snap radius
        const double snap_radius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.polygonGeometryPointRadius;
        // check if we have to create a new index
        if ((index != -1) && shape[index].distanceSquaredTo2D(clickedPosition) < (snap_radius * snap_radius)) {
            // remove geometry point
            shape.erase(shape.begin() + index);
            // commit new shape
            undoList->begin(this, "remove geometry point of " + getTagStr());
            GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_SHAPE, toString(shape), undoList);
            undoList->end();
        }
    }
}


std::string
GNEPoly::generateChildID(SumoXMLTag /*childTag*/) {
    return "";
}


void
GNEPoly::updateGeometry() {
    // just update polygon geometry
    myAdditionalGeometry.updateGeometry(myShape);
    myTesselation.clear();
}


Position
GNEPoly::getPositionInView() const {
    return myAdditionalBoundary.getCenter();
}


double
GNEPoly::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.polySize.getExaggeration(s, this);
}


void
GNEPoly::updateCenteringBoundary(const bool updateGrid) {
    // Remove object from net
    if (updateGrid) {
        myNet->removeGLObjectFromGrid(this);
    }
    // use shape as boundary
    myAdditionalBoundary = myShape.getBoxBoundary();
    // grow boundary
    myAdditionalBoundary.grow(5);
    // add object into net
    if (updateGrid) {
        myNet->addGLObjectIntoGrid(this);
    }
}


void
GNEPoly::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // nothing to split
}


void
GNEPoly::writeAdditional(OutputDevice& device) const {
    writeXML(device, myGEO);
}


bool
GNEPoly::isAdditionalValid() const {
    return true;
}


std::string
GNEPoly::getAdditionalProblem() const {
    return "";
}


void
GNEPoly::fixAdditionalProblem() {
    // nothing to fix
}


GUIGlID
GNEPoly::getGlID() const {
    return GUIGlObject::getGlID();
}


bool
GNEPoly::checkDrawMoveContour() const {
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


std::string
GNEPoly::getParentName() const {
    return myNet->getMicrosimID();
}


GUIGLObjectPopupMenu*
GNEPoly::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, app);
    new FXMenuSeparator(ret);
    FXMenuCommand* simplifyShape = GUIDesigns::buildFXMenuCommand(ret, TL("Simplify Shape"), TL("Replace current shape with a rectangle"), nullptr, &parent, MID_GNE_POLYGON_SIMPLIFY_SHAPE);
    // disable simplify shape if polygon was already simplified
    if (mySimplifiedShape || myShape.size() <= 2) {
        simplifyShape->disable();
    }
    // only allow open/close for non juPedSim polygons
    if (!myTagProperty.isJuPedSimElement()) {
        if (myShape.isClosed()) {
            GUIDesigns::buildFXMenuCommand(ret, TL("Open shape"), TL("Open polygon's shape"), nullptr, &parent, MID_GNE_POLYGON_OPEN);
        } else {
            GUIDesigns::buildFXMenuCommand(ret, TL("Close shape"), TL("Close polygon's shape"), nullptr, &parent, MID_GNE_POLYGON_CLOSE);
        }
    }
    GUIDesigns::buildFXMenuCommand(ret, TL("Select elements within polygon"), TL("Select elements within polygon boundary"), nullptr, &parent, MID_GNE_POLYGON_SELECT);
    // add separator
    new FXMenuSeparator(ret);
    // create a extra FXMenuCommand if mouse is over a vertex
    const int index = getVertexIndex(myNet->getViewNet()->getPositionInformation(), false);
    if (index != -1) {
        // check if we're in network mode
        if (myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE) {
            GUIDesigns::buildFXMenuCommand(ret, "Set custom Geometry Point", nullptr, &parent, MID_GNE_CUSTOM_GEOMETRYPOINT);
        }
        FXMenuCommand* removeGeometryPoint = GUIDesigns::buildFXMenuCommand(ret, TL("Remove geometry point"), TL("Remove geometry point under mouse"), nullptr, &parent, MID_GNE_POLYGON_DELETE_GEOMETRY_POINT);
        FXMenuCommand* setFirstPoint = GUIDesigns::buildFXMenuCommand(ret, TL("Set first geometry point"), TL("Set first geometry point"), nullptr, &parent, MID_GNE_POLYGON_SET_FIRST_POINT);
        // disable setFirstPoint if shape only have three points
        if ((myShape.isClosed() && (myShape.size() <= 4)) || (!myShape.isClosed() && (myShape.size() <= 2))) {
            removeGeometryPoint->disable();
        }
        // disable setFirstPoint if mouse is over first point
        if (index == 0) {
            setFirstPoint->disable();
        }
    }
    return ret;
}


void
GNEPoly::drawGL(const GUIVisualizationSettings& s) const {
    // first check if poly can be drawn
    if (myNet->getViewNet()->getDemandViewOptions().showShapes() &&
            myNet->getViewNet()->getDataViewOptions().showShapes() &&
            GUIPolygon::checkDraw(s, this, this)) {
        // draw boundary
        const auto boundary = getCenteringBoundary();
        GLHelper::drawBoundary(s, getCenteringBoundary());
        // get exaggeration
        const double polyExaggeration = getExaggeration(s);
        // get detail level
        const auto d = s.getDetailLevel(polyExaggeration);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawPoly(boundary, isAttributeCarrierSelected())) {
            // get colors
            const RGBColor color = isAttributeCarrierSelected() ? s.colorSettings.selectionColor : getShapeColor();
            // push layer matrix
            GLHelper::pushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getShapeLayer());
            // draw polygon
            drawPolygon(s, d, color, polyExaggeration);
            // draw contour if don't move whole polygon
            if (!myNet->getViewNet()->getViewParent()->getMoveFrame()->getNetworkModeOptions()->getMoveWholePolygons()) {
                // get darker color
                const RGBColor darkerColor = color.changedBrightness(-32);
                // draw contour
                drawPolygonContour(s, d, darkerColor, polyExaggeration);
                // draw geometry points
                drawGeometryPoints(s, d, darkerColor, polyExaggeration);
            }
            // pop layer matrix
            GLHelper::popMatrix();
            // draw name and type
            drawPolygonNameAndType(s);
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), getPositionInView(), polyExaggeration);
            // draw dotted contour
            myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // calculate contour
        calculateContourPolygons(s, d, polyExaggeration, getFill());
    }
}


int
GNEPoly::getVertexIndex(Position pos, bool snapToGrid) {
    // check if position has to be snapped to grid
    if (snapToGrid) {
        pos = myNet->getViewNet()->snapToActiveGrid(pos);
    }
    // first check if vertex already exists
    for (const auto& shapePosition : myShape) {
        if (shapePosition.distanceTo2D(pos) < myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.polygonGeometryPointRadius) {
            return myShape.indexOfClosest(shapePosition);
        }
    }
    return -1;
}


void
GNEPoly::deleteGeometryPoint(const Position& pos, bool allowUndo) {
    if (myShape.size() > 1) {
        // obtain index
        PositionVector modifiedShape = myShape;
        int index = modifiedShape.indexOfClosest(pos);
        // remove point dependending of
        if (myShape.isClosed() && (index == 0 || index == (int)modifiedShape.size() - 1) && (myShape.size() > 2)) {
            modifiedShape.erase(modifiedShape.begin());
            modifiedShape.erase(modifiedShape.end() - 1);
            modifiedShape.push_back(modifiedShape.front());
        } else {
            modifiedShape.erase(modifiedShape.begin() + index);
        }
        // set new shape depending of allowUndo
        if (allowUndo) {
            myNet->getViewNet()->getUndoList()->begin(this, "delete geometry point");
            setAttribute(SUMO_ATTR_SHAPE, toString(modifiedShape), myNet->getViewNet()->getUndoList());
            myNet->getViewNet()->getUndoList()->end();
        } else {
            // first remove object from grid due shape is used for boundary
            myNet->removeGLObjectFromGrid(this);
            // set new shape
            myShape = modifiedShape;
            // disable simplified shape flag
            mySimplifiedShape = false;
            // add object into grid again
            myNet->addGLObjectIntoGrid(this);
        }
        myTesselation.clear();
    } else {
        WRITE_WARNING(TL("Number of remaining points insufficient"))
    }
}


bool
GNEPoly::isPolygonClosed() const {
    return myShape.isClosed();
}


void
GNEPoly::openPolygon(bool allowUndo) {
    // only open if shape is closed
    if (myShape.isClosed()) {
        if (allowUndo) {
            myNet->getViewNet()->getUndoList()->begin(this, "open polygon");
            setAttribute(GNE_ATTR_CLOSE_SHAPE, "false", myNet->getViewNet()->getUndoList());
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myShape.pop_back();
            // disable simplified shape flag
            mySimplifiedShape = false;
            // update geometry to avoid grabbing Problems
            updateGeometry();
        }
    } else {
        WRITE_WARNING(TL("Polygon already opened"))
    }
}


void
GNEPoly::closePolygon(bool allowUndo) {
    // only close if shape is opened
    if (!myShape.isClosed()) {
        if (allowUndo) {
            myNet->getViewNet()->getUndoList()->begin(this, "close shape");
            setAttribute(GNE_ATTR_CLOSE_SHAPE, "true", myNet->getViewNet()->getUndoList());
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myShape.closePolygon();
            // disable simplified shape flag
            mySimplifiedShape = false;
            // update geometry to avoid grabbing Problems
            updateGeometry();
        }
    } else {
        WRITE_WARNING(TL("Polygon already closed"))
    }
}


void
GNEPoly::changeFirstGeometryPoint(int oldIndex, bool allowUndo) {
    // check that old index is correct
    if (oldIndex >= (int)myShape.size()) {
        throw InvalidArgument("Invalid old Index");
    } else if (oldIndex == 0) {
        WRITE_WARNING(TL("Selected point must be different of the first point"))
    } else {
        // Configure new shape
        PositionVector newShape;
        for (int i = oldIndex; i < (int)myShape.size(); i++) {
            newShape.push_back(myShape[i]);
        }
        if (myShape.isClosed()) {
            for (int i = 1; i < oldIndex; i++) {
                newShape.push_back(myShape[i]);
            }
            newShape.push_back(newShape.front());
        } else {
            for (int i = 0; i < oldIndex; i++) {
                newShape.push_back(myShape[i]);
            }
        }
        // set new rotated shape
        if (allowUndo) {
            myNet->getViewNet()->getUndoList()->begin(this, "change first geometry point");
            setAttribute(SUMO_ATTR_SHAPE, toString(newShape), myNet->getViewNet()->getUndoList());
            myNet->getViewNet()->getUndoList()->end();
        } else {
            // set new shape
            myShape = newShape;
            // disable simplified shape flag
            mySimplifiedShape = false;
            // update geometry to avoid grabbing Problems
            updateGeometry();
        }
    }
}


void
GNEPoly::simplifyShape(bool allowUndo) {
    if (!mySimplifiedShape && myShape.size() > 2) {
        const Boundary b =  myShape.getBoxBoundary();
        // create a square as simplified shape
        PositionVector simplifiedShape;
        simplifiedShape.push_back(Position(b.xmin(), b.ymin()));
        simplifiedShape.push_back(Position(b.xmin(), b.ymax()));
        simplifiedShape.push_back(Position(b.xmax(), b.ymax()));
        simplifiedShape.push_back(Position(b.xmax(), b.ymin()));
        if (myShape.isClosed()) {
            simplifiedShape.push_back(simplifiedShape[0]);
        }
        // set new shape depending of allowUndo
        if (allowUndo) {
            myNet->getViewNet()->getUndoList()->begin(this, "simplify shape");
            setAttribute(SUMO_ATTR_SHAPE, toString(simplifiedShape), myNet->getViewNet()->getUndoList());
            myNet->getViewNet()->getUndoList()->end();
        } else {
            // set new shape
            myShape = simplifiedShape;
            // update geometry to avoid grabbing Problems
            updateGeometry();
        }
        // change flag after setting simplified shape
        mySimplifiedShape = true;
    } else {
        WRITE_WARNING(TL("Polygon already simplified"))
    }
}


std::string
GNEPoly::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myID;
        case SUMO_ATTR_SHAPE:
            return toString(myShape);
        case SUMO_ATTR_GEOSHAPE:
            return toString(myGeoShape, gPrecisionGeo);
        case SUMO_ATTR_COLOR:
            return toString(getShapeColor());
        case SUMO_ATTR_FILL:
            return toString(myFill);
        case SUMO_ATTR_LINEWIDTH:
            return toString(myLineWidth);
        case SUMO_ATTR_LAYER:
            if (getShapeLayer() == Shape::DEFAULT_LAYER) {
                return "default";
            } else {
                return toString(getShapeLayer());
            }
        case SUMO_ATTR_TYPE:
            return getShapeType();
        case SUMO_ATTR_IMGFILE:
            return getShapeImgFile();
        case SUMO_ATTR_RELATIVEPATH:
            return toString(getShapeRelativePath());
        case SUMO_ATTR_ANGLE:
            return toString(getShapeNaviDegree());
        case SUMO_ATTR_GEO:
            return toString(myGEO);
        case SUMO_ATTR_NAME:
            return getShapeName();
        case GNE_ATTR_CLOSE_SHAPE:
            return toString(myShape.isClosed());
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return SUMOPolygon::getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEPoly::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " attribute '" + toString(key) + "' not allowed");
}


const Parameterised::Map&
GNEPoly::getACParametersMap() const {
    return SUMOPolygon::getParametersMap();
}


void
GNEPoly::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_GEOSHAPE:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_FILL:
        case SUMO_ATTR_LINEWIDTH:
        case SUMO_ATTR_LAYER:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_IMGFILE:
        case SUMO_ATTR_RELATIVEPATH:
        case SUMO_ATTR_ANGLE:
        case SUMO_ATTR_GEO:
        case SUMO_ATTR_NAME:
        case GNE_ATTR_CLOSE_SHAPE:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEPoly::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(NamespaceIDs::polygons, value);
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_GEOSHAPE:
            // empty shapes AREN'T allowed
            if (value.empty()) {
                return false;
            } else {
                return canParse<PositionVector>(value);
            }
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_FILL:
            return canParse<bool>(value);
        case SUMO_ATTR_LINEWIDTH:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_LAYER:
            if (value == "default") {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_TYPE:
            return true;
        case SUMO_ATTR_IMGFILE:
            if (value == "") {
                return true;
            } else {
                // check that image can be loaded
                return GUITexturesHelper::getTextureID(value) != -1;
            }
        case SUMO_ATTR_RELATIVEPATH:
            return canParse<bool>(value);
        case SUMO_ATTR_ANGLE:
            return canParse<double>(value);
        case SUMO_ATTR_GEO:
            return canParse<bool>(value);
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case GNE_ATTR_CLOSE_SHAPE:
            if (canParse<bool>(value)) {
                bool closePolygon = parse<bool>(value);
                if (closePolygon && (myShape.begin() == myShape.end())) {
                    // Polygon already closed, then invalid value
                    return false;
                } else if (!closePolygon && (myShape.begin() != myShape.end())) {
                    // Polygon already open, then invalid value
                    return false;
                } else {
                    return true;
                }
            } else {
                return false;
            }
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEPoly::isAttributeEnabled(SumoXMLAttr /* key */) const {
    // check if we're in supermode Network
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        return true;
    } else {
        return false;
    }
}


std::string
GNEPoly::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNEPoly::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEPoly::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID: {
            // update microsimID
            setAdditionalID(value);
            // set named ID
            myID = value;
            break;
        }
        case SUMO_ATTR_SHAPE: {
            // set new shape
            myShape = parse<PositionVector>(value);
            // set GEO shape
            myGeoShape = myShape;
            for (int i = 0; i < (int) myGeoShape.size(); i++) {
                GeoConvHelper::getFinal().cartesian2geo(myGeoShape[i]);
            }
            // disable simplified shape flag
            mySimplifiedShape = false;
            // update geometry
            updateGeometry();
            // update centering boundary
            updateCenteringBoundary(true);
            break;
        }
        case SUMO_ATTR_GEOSHAPE: {
            // set new GEO shape
            myGeoShape = parse<PositionVector>(value);
            // set shape
            myShape = myGeoShape ;
            for (int i = 0; i < (int) myShape.size(); i++) {
                GeoConvHelper::getFinal().x2cartesian_const(myShape[i]);
            }
            // disable simplified shape flag
            mySimplifiedShape = false;
            // update geometry
            updateGeometry();
            // update centering boundary
            updateCenteringBoundary(true);
            break;
        }
        case SUMO_ATTR_COLOR:
            setShapeColor(parse<RGBColor>(value));
            break;
        case SUMO_ATTR_FILL:
            myFill = parse<bool>(value);
            myAdditionalContour.clearContour();
            break;
        case SUMO_ATTR_LINEWIDTH:
            myLineWidth = parse<double>(value);
            break;
        case SUMO_ATTR_LAYER:
            if (value == "default") {
                setShapeLayer(Shape::DEFAULT_LAYER);
            } else {
                setShapeLayer(parse<double>(value));
            }
            break;
        case SUMO_ATTR_TYPE:
            setShapeType(value);
            break;
        case SUMO_ATTR_IMGFILE:
            setShapeImgFile(value);
            // all textures must be refresh
            GUITexturesHelper::clearTextures();
            break;
        case SUMO_ATTR_RELATIVEPATH:
            setShapeRelativePath(parse<bool>(value));
            break;
        case SUMO_ATTR_ANGLE:
            setShapeNaviDegree(parse<double>(value));
            break;
        case SUMO_ATTR_GEO:
            myGEO = parse<bool>(value);
            // update centering boundary
            updateCenteringBoundary(true);
            break;
        case SUMO_ATTR_NAME:
            setShapeName(value);
            break;
        case GNE_ATTR_CLOSE_SHAPE:
            if (parse<bool>(value)) {
                myShape.closePolygon();
                myGeoShape.closePolygon();
            } else {
                myShape.pop_back();
                myGeoShape.pop_back();
            }
            // disable simplified shape flag
            mySimplifiedShape = false;
            // update geometry
            updateGeometry();
            // update centering boundary
            updateCenteringBoundary(true);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            SUMOPolygon::setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEPoly::setMoveShape(const GNEMoveResult& moveResult) {
    // update new shape
    myShape = moveResult.shapeToUpdate;
    // update geometry
    myAdditionalGeometry.updateGeometry(myShape);
}


void
GNEPoly::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // commit new shape
    undoList->begin(this, "moving " + toString(SUMO_ATTR_SHAPE) + " of " + getTagStr());
    GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_SHAPE, toString(moveResult.shapeToUpdate), undoList);
    undoList->end();
}


void
GNEPoly::drawPolygon(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                     const RGBColor& color, const double exaggeration) const {
    // check if we're drawing a polygon or a polyline
    if (getFill()) {
        // draw inner polygon
        GUIPolygon::drawInnerPolygon(s, this, this, myAdditionalGeometry.getShape(), 0, getFill(), myTagProperty.isJuPedSimElement() ? false : drawUsingSelectColor());
    } else {
        // push matrix
        GLHelper::pushMatrix();
        // set color
        GLHelper::setColor(color);
        // draw geometry (polyline)
        GUIGeometry::drawGeometry(d, myAdditionalGeometry, s.neteditSizeSettings.polylineWidth * exaggeration);
        // pop matrix
        GLHelper::popMatrix();
    }
}


void
GNEPoly::drawPolygonContour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                            const RGBColor& color, const double exaggeration) const {
    // push contour matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, 0.1);
    // set color
    GLHelper::setColor(color);
    // draw polygon contour
    GUIGeometry::drawGeometry(d, myAdditionalGeometry, s.neteditSizeSettings.polygonContourWidth * exaggeration);
    // pop contour matrix
    GLHelper::popMatrix();
}


void
GNEPoly::drawGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                            const RGBColor& color, const double exaggeration) const {
    // draw shape points only in supermode network
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        // check if we're in move mode
        const bool moveMode = (myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE);
        // get geometry point sizes
        const double geometryPointSize = s.neteditSizeSettings.polygonGeometryPointRadius * (moveMode ? 1 : 0.5);
        // draw geometry points
        GUIGeometry::drawGeometryPoints(d, myAdditionalGeometry.getShape(), color, geometryPointSize, exaggeration,
                                        myNet->getViewNet()->getNetworkViewOptions().editingElevation());
        // draw dotted contours for geometry points if we're in move mode
        if (moveMode) {
            myAdditionalContour.drawDottedContourGeometryPoints(s, d, this, myAdditionalGeometry.getShape(), geometryPointSize,
                    exaggeration, s.dottedContourSettings.segmentWidthSmall);
        }
    }
}


void
GNEPoly::drawPolygonNameAndType(const GUIVisualizationSettings& s) const {
    // get name position
    const Position& namePos = myAdditionalGeometry.getShape().getPolygonCenter();
    // draw name
    drawName(namePos, s.scale, s.polyName, s.angle);
    // check if draw poly type
    if (s.polyType.show(this)) {
        const Position p = namePos + Position(0, -0.6 * s.polyType.size / s.scale);
        GLHelper::drawTextSettings(s.polyType, getShapeType(), p, s.scale, s.angle);
    }
}

/****************************************************************************/
