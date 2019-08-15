/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPoly.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
/// @version $Id$
///
// A class for visualizing and editing POIS in netedit (adapted from
// GUIPolygon and NLHandler)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <string>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <netedit/GNENet.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEPoly.h"


// ===========================================================================
// static members
// ===========================================================================

const double GNEPoly::myHintSize = 0.8;

// ===========================================================================
// method definitions
// ===========================================================================
GNEPoly::GNEPoly(GNENet* net, const std::string& id, const std::string& type, const PositionVector& shape, bool geo, bool fill, double lineWidth,
                 const RGBColor& color, double layer, double angle, const std::string& imgFile, bool relativePath, bool movementBlocked, bool shapeBlocked) :
    GUIPolygon(id, type, color, shape, geo, fill, lineWidth, layer, angle, imgFile, relativePath),
    GNEShape(net, SUMO_TAG_POLY, movementBlocked, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}),
         myNetElementShapeEdited(nullptr),
         myBlockShape(shapeBlocked),
         myClosedShape(shape.front() == shape.back()),
         mySimplifiedShape(false),
myCurrentMovingVertexIndex(-1) {
    // check if imgFile is valid
    if (!imgFile.empty() && GUITexturesHelper::getTextureID(imgFile) == -1) {
        setShapeImgFile("");
    }
    // set GEO shape
    myGeoShape = myShape;
    for (int i = 0; i < (int) myGeoShape.size(); i++) {
        GeoConvHelper::getFinal().cartesian2geo(myGeoShape[i]);
    }
}


GNEPoly::~GNEPoly() {}


std::string
GNEPoly::generateChildID(SumoXMLTag childTag) {
    int counter = myNet->getPolygons().size();
    while (myNet->retrievePolygon(getID() + toString(childTag) + toString(counter), false) != nullptr) {
        counter++;
    }
    return (getID() + toString(childTag) + toString(counter));
}


void
GNEPoly::startGeometryMoving() {
    // save current centering boundary
    myMovingGeometryBoundary = getCenteringBoundary();
}


void
GNEPoly::endGeometryMoving() {
    // check that endGeometryMoving was called only once
    if (myMovingGeometryBoundary.isInitialised()) {
        // Remove object from net
        myNet->removeGLObjectFromGrid(this);
        // reset myMovingGeometryBoundary
        myMovingGeometryBoundary.reset();
        // add object into grid again (using the new centering boundary)
        myNet->addGLObjectIntoGrid(this);
    }
}


int
GNEPoly::moveVertexShape(const int index, const Position& oldPos, const Position& offset) {
    // only move shape if block movement block shape are disabled
    if (!myBlockMovement && !myBlockShape && (index != -1)) {
        // check that index is correct before change position
        if (index < (int)myShape.size()) {
            // save current moving Geometry Point
            myCurrentMovingVertexIndex = index;
            // if closed shape and cliked is first or last, move both giving more priority to first always
            if (myClosedShape && (index == 0 || index == (int)myShape.size() - 1)) {
                // Change position of first shape Geometry Point and filtern position using snap to active grid
                myShape.front() = oldPos;
                myShape.front().add(offset);
                myShape.front() = myNet->getViewNet()->snapToActiveGrid(myShape[index]);
                // Change position of last shape Geometry Point and filtern position using snap to active grid
                myShape.back() = oldPos;
                myShape.back().add(offset);
                myShape.back() = myNet->getViewNet()->snapToActiveGrid(myShape[index]);
            } else {
                // change position of Geometry Point and filtern position using snap to active grid
                myShape[index] = oldPos;
                myShape[index].add(offset);
                myShape[index] = myNet->getViewNet()->snapToActiveGrid(myShape[index]);
            }
            // return index of moved Geometry Point
            return index;
        } else {
            throw InvalidArgument("Index greater than shape size");
        }
    } else {
        return index;
    }
}


void
GNEPoly::moveEntireShape(const PositionVector& oldShape, const Position& offset) {
    // only move shape if block movement is disabled and block shape is enabled
    if (!myBlockMovement && myBlockShape) {
        // restore original shape
        myShape = oldShape;
        // change all points of the shape shape using offset
        for (auto& i : myShape) {
            i.add(offset);
        }
        // update Geometry after moving
        updateGeometry();
    }
}


void
GNEPoly::commitShapeChange(const PositionVector& oldShape, GNEUndoList* undoList) {
    if (!myBlockMovement) {
        // disable current moving vertex
        myCurrentMovingVertexIndex = -1;
        // restore original shape into shapeToCommit
        PositionVector shapeToCommit = myShape;
        // restore old shape in polygon (to avoid problems with RTree)
        myShape = oldShape;
        // first check if double points has to be removed
        shapeToCommit.removeDoublePoints(myHintSize);
        if (shapeToCommit.size() != myShape.size() && !myNet->getViewNet()->getEditShapes().editingNetElementShapes) {
            WRITE_WARNING("Merged shape's point")
        }

        // check if polygon has to be closed
        if (shapeToCommit.size() > 1 && shapeToCommit.front().distanceTo2D(shapeToCommit.back()) < (2 * myHintSize)) {
            shapeToCommit.pop_back();
            shapeToCommit.push_back(shapeToCommit.front());
        }
        // only use GNEChange_Attribute if we aren't editing a junction's shape
        if (myNetElementShapeEdited == nullptr) {
            // commit new shape
            undoList->p_begin("moving " + toString(SUMO_ATTR_SHAPE) + " of " + getTagStr());
            undoList->p_add(new GNEChange_Attribute(this, myNet, SUMO_ATTR_SHAPE, toString(shapeToCommit)));
            undoList->p_end();
        } else {
            // set new shape calling private setAttribute function
            setAttribute(SUMO_ATTR_SHAPE, toString(shapeToCommit));
        }
    }
}


void
GNEPoly::updateGeometry() {
    // nothing to do
}

void
GNEPoly::writeShape(OutputDevice& device) {
    writeXML(device, myGEO);
}


Position
GNEPoly::getPositionInView() const {
    return myShape.getPolygonCenter();
}


Boundary
GNEPoly::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (myMovingGeometryBoundary.isInitialised()) {
        return myMovingGeometryBoundary;
    }  else {
        return GUIPolygon::getCenteringBoundary();
    }
}


GUIGlID
GNEPoly::getGlID() const {
    return GUIPolygon::getGlID();
}


std::string
GNEPoly::getParentName() const {
    if (myNetElementShapeEdited != nullptr) {
        return myNetElementShapeEdited->getMicrosimID();
    } else {
        return myNet->getMicrosimID();
    }
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
    FXMenuCommand* simplifyShape = new FXMenuCommand(ret, "Simplify Shape\t\tReplace current shape with a rectangle", nullptr, &parent, MID_GNE_POLYGON_SIMPLIFY_SHAPE);
    // disable simplify shape if polygon was already simplified
    if (mySimplifiedShape || myShape.size() <= 2) {
        simplifyShape->disable();
    }
    // create open or close polygon's shape only if myNetElementShapeEdited is nullptr
    if (myNetElementShapeEdited == nullptr) {
        if (myClosedShape) {
            new FXMenuCommand(ret, "Open shape\t\tOpen polygon's shape", nullptr, &parent, MID_GNE_POLYGON_OPEN);
        } else {
            new FXMenuCommand(ret, "Close shape\t\tClose polygon's shape", nullptr, &parent, MID_GNE_POLYGON_CLOSE);
        }
    }
    // create a extra FXMenuCommand if mouse is over a vertex
    int index = getVertexIndex(myNet->getViewNet()->getPositionInformation(), false, false);
    if (index != -1) {
        FXMenuCommand* removeGeometryPoint = new FXMenuCommand(ret, "Remove geometry point\t\tRemove geometry point under mouse", nullptr, &parent, MID_GNE_POLYGON_DELETE_GEOMETRY_POINT);
        FXMenuCommand* setFirstPoint = new FXMenuCommand(ret, "Set first geometry point\t\tSet", nullptr, &parent, MID_GNE_POLYGON_SET_FIRST_POINT);
        // disable setFirstPoint if shape only have three points
        if ((myClosedShape && (myShape.size() <= 4)) || (!myClosedShape && (myShape.size() <= 2))) {
            removeGeometryPoint->disable();
        }
        // disable setFirstPoint if mouse is over first point
        if (index == 0) {
            setFirstPoint->disable();
        }
    }
    return ret;
}


GUIParameterTableWindow*
GNEPoly::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    return GUIPolygon::getParameterWindow(app, parent);
}


void
GNEPoly::drawGL(const GUIVisualizationSettings& s) const {
    // first check if poly can be drawn
    if (myNet->getViewNet()->getDemandViewOptions().showShapes()) {
        // check if boundary has to be drawn
        if (s.drawBoundaries) {
            GLHelper::drawBoundary(getCenteringBoundary());
        }
        // push name (needed for getGUIGlObjectsUnderCursor(...)
        glPushName(getGlID());
        // first check if inner polygon can be drawn
        if (checkDraw(s)) {
            // draw inner polygon
            drawInnerPolygon(s, drawUsingSelectColor());
        }
        // draw details of Netedit
        double circleWidth = myHintSize * MIN2((double)1, s.polySize.getExaggeration(s, this));
        double circleWidthSquared = circleWidth * circleWidth;
        // Obtain exaggeration of the draw
        const double exaggeration = s.addSize.getExaggeration(s, this);
        // draw geometry details hints if is not too small and isn't in selecting mode
        if (s.scale * circleWidth > 1.) {
            // set values relative to mouse position regarding to shape
            bool mouseOverVertex = false;
            bool modeMove = myNet->getViewNet()->getEditModes().networkEditMode == GNE_NMODE_MOVE;
            Position mousePosition = myNet->getViewNet()->getPositionInformation();
            double distanceToShape = myShape.distance2D(mousePosition);
            // set colors
            RGBColor invertedColor, darkerColor;
            if (drawUsingSelectColor()) {
                invertedColor = s.colorSettings.selectionColor.invertedColor();
                darkerColor = s.colorSettings.selectionColor.changedBrightness(-32);
            } else {
                invertedColor = GLHelper::getColor().invertedColor();
                darkerColor = GLHelper::getColor().changedBrightness(-32);
            }
            // Draw geometry hints if polygon's shape isn't blocked
            if (myBlockShape == false) {
                // draw a boundary for moving using darkerColor
                glPushMatrix();
                glTranslated(0, 0, GLO_POLYGON + 0.01);
                GLHelper::setColor(darkerColor);
                GLHelper::drawBoxLines(myShape, (myHintSize / 4) * s.polySize.getExaggeration(s, this));
                glPopMatrix();
                // draw shape points only in Network supemode
                if (myNet->getViewNet()->getEditModes().currentSupermode != GNE_SUPERMODE_DEMAND) {
                    for (auto i : myShape) {
                        if (!s.drawForSelecting || (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(i) <= (circleWidthSquared + 2))) {
                            glPushMatrix();
                            glTranslated(i.x(), i.y(), GLO_POLYGON + 0.02);
                            // Change color of vertex and flag mouseOverVertex if mouse is over vertex
                            if (modeMove && (i.distanceTo(mousePosition) < circleWidth)) {
                                mouseOverVertex = true;
                                GLHelper::setColor(invertedColor);
                            } else {
                                GLHelper::setColor(darkerColor);
                            }
                            GLHelper::drawFilledCircle(circleWidth, s.getCircleResolution());
                            glPopMatrix();
                            // draw elevation or special symbols (Start, End and Block)
                            if (!s.drawForSelecting && myNet->getViewNet()->getNetworkViewOptions().editingElevation()) {
                                // Push matrix
                                glPushMatrix();
                                // Traslate to center of detector
                                glTranslated(i.x(), i.y(), getType() + 1);
                                // draw Z
                                GLHelper::drawText(toString(i.z()), Position(), .1, 0.7, RGBColor::BLUE);
                                // pop matrix
                                glPopMatrix();
                            } else if ((i == myShape.front()) && !s.drawForSelecting && s.drawDetail(s.detailSettings.geometryPointsText, exaggeration)) {
                                // draw a "s" over first point
                                glPushMatrix();
                                glTranslated(i.x(), i.y(), GLO_POLYGON + 0.03);
                                GLHelper::drawText("S", Position(), .1, 2 * circleWidth, invertedColor);
                                glPopMatrix();
                            } else if ((i == myShape.back()) && (myClosedShape == false) && !s.drawForSelecting && s.drawDetail(s.detailSettings.geometryPointsText, exaggeration)) {
                                // draw a "e" over last point if polygon isn't closed
                                glPushMatrix();
                                glTranslated(i.x(), i.y(), GLO_POLYGON + 0.03);
                                GLHelper::drawText("E", Position(), .1, 2 * circleWidth, invertedColor);
                                glPopMatrix();
                            }
                        }
                    }
                    // check if draw moving hint has to be drawed
                    if (modeMove && (mouseOverVertex == false) && (myBlockMovement == false) && (distanceToShape < circleWidth)) {
                        // push matrix
                        glPushMatrix();
                        Position hintPos = myShape.size() > 1 ? myShape.positionAtOffset2D(myShape.nearest_offset_to_point2D(mousePosition)) : myShape[0];
                        glTranslated(hintPos.x(), hintPos.y(), GLO_POLYGON + 0.04);
                        GLHelper::setColor(invertedColor);
                        GLHelper:: drawFilledCircle(circleWidth, s.getCircleResolution());
                        glPopMatrix();
                    }
                }
            }
        }
        // check if dotted contour has to be drawn
        if (myNet->getViewNet()->getDottedAC() == this) {
            GLHelper::drawShapeDottedContourAroundClosedShape(s, getType(), getShape());
        }
        // pop name
        glPopName();
    }
}


int
GNEPoly::getVertexIndex(Position pos, bool createIfNoExist, bool snapToGrid) {
    // check if position has to be snapped to grid
    if (snapToGrid) {
        pos = myNet->getViewNet()->snapToActiveGrid(pos);
    }
    // first check if vertex already exists
    for (auto i : myShape) {
        if (i.distanceTo2D(pos) < myHintSize) {
            return myShape.indexOfClosest(i);
        }
    }
    // if vertex doesn't exist, insert it
    if (createIfNoExist) {
        return myShape.insertAtClosest(pos);
    } else {
        return -1;
    }
}


void
GNEPoly::deleteGeometryPoint(const Position& pos, bool allowUndo) {
    if (myShape.size() > 1) {
        // obtain index
        PositionVector modifiedShape = myShape;
        int index = modifiedShape.indexOfClosest(pos);
        // remove point dependending of
        if (myClosedShape && (index == 0 || index == (int)modifiedShape.size() - 1) && (myShape.size() > 2)) {
            modifiedShape.erase(modifiedShape.begin());
            modifiedShape.erase(modifiedShape.end() - 1);
            modifiedShape.push_back(modifiedShape.front());
        } else {
            modifiedShape.erase(modifiedShape.begin() + index);
        }
        // set new shape depending of allowUndo
        if (allowUndo) {
            myNet->getViewNet()->getUndoList()->p_begin("delete geometry point");
            setAttribute(SUMO_ATTR_SHAPE, toString(modifiedShape), myNet->getViewNet()->getUndoList());
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            // first remove object from grid due shape is used for boundary
            myNet->removeGLObjectFromGrid(this);
            // set new shape
            myShape = modifiedShape;
            // Check if new shape is closed
            myClosedShape = (myShape.front() == myShape.back());
            // disable simplified shape flag
            mySimplifiedShape = false;
            // add object into grid again
            myNet->addGLObjectIntoGrid(this);
        }
    } else {
        WRITE_WARNING("Number of remaining points insufficient")
    }
}


bool
GNEPoly::isPolygonBlocked() const {
    return myBlockShape;
}


bool
GNEPoly::isPolygonClosed() const {
    return myClosedShape;
}


void
GNEPoly::setShapeEditedElement(GNENetElement* element) {
    if (element) {
        myNetElementShapeEdited = element;
    } else {
        throw InvalidArgument("Junction cannot be nullptr");
    }
}


GNENetElement*
GNEPoly::getShapeEditedElement() const {
    return myNetElementShapeEdited;
}


void
GNEPoly::openPolygon(bool allowUndo) {
    // only open if shape is closed
    if (myClosedShape) {
        if (allowUndo) {
            myNet->getViewNet()->getUndoList()->p_begin("open polygon");
            setAttribute(GNE_ATTR_CLOSE_SHAPE, "false", myNet->getViewNet()->getUndoList());
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myClosedShape = false;
            myShape.pop_back();
            // disable simplified shape flag
            mySimplifiedShape = false;
            // update geometry to avoid grabbing Problems
            updateGeometry();
        }
    } else {
        WRITE_WARNING("Polygon already opened")
    }
}


void
GNEPoly::closePolygon(bool allowUndo) {
    // only close if shape is opened
    if (myClosedShape == false) {
        if (allowUndo) {
            myNet->getViewNet()->getUndoList()->p_begin("close shape");
            setAttribute(GNE_ATTR_CLOSE_SHAPE, "true", myNet->getViewNet()->getUndoList());
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            myClosedShape = true;
            myShape.closePolygon();
            // disable simplified shape flag
            mySimplifiedShape = false;
            // update geometry to avoid grabbing Problems
            updateGeometry();
        }
    } else {
        WRITE_WARNING("Polygon already closed")
    }
}


void
GNEPoly::changeFirstGeometryPoint(int oldIndex, bool allowUndo) {
    // check that old index is correct
    if (oldIndex >= (int)myShape.size()) {
        throw InvalidArgument("Invalid old Index");
    } else if (oldIndex == 0) {
        WRITE_WARNING("Selected point must be different of the first point")
    } else {
        // Configure new shape
        PositionVector newShape;
        for (int i = oldIndex; i < (int)myShape.size(); i++) {
            newShape.push_back(myShape[i]);
        }
        if (myClosedShape) {
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
            myNet->getViewNet()->getUndoList()->p_begin("change first geometry point");
            setAttribute(SUMO_ATTR_SHAPE, toString(newShape), myNet->getViewNet()->getUndoList());
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            // set new shape
            myShape = newShape;
            // Check if new shape is closed
            myClosedShape = (myShape.front() == myShape.back());
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
        PositionVector simplifiedShape;
        if (myShape.isClosed()) {
            // create a square as simplified shape
            simplifiedShape.push_back(Position(b.xmin(), b.ymin()));
            simplifiedShape.push_back(Position(b.xmin(), b.ymax()));
            simplifiedShape.push_back(Position(b.xmax(), b.ymax()));
            simplifiedShape.push_back(Position(b.xmax(), b.ymin()));
            simplifiedShape.push_back(simplifiedShape[0]);
        } else {
            // create a line as simplified shape
            simplifiedShape.push_back(myShape.front());
            simplifiedShape.push_back(myShape.back());
        }
        // set new shape depending of allowUndo
        if (allowUndo) {
            myNet->getViewNet()->getUndoList()->p_begin("simplify shape");
            setAttribute(SUMO_ATTR_SHAPE, toString(simplifiedShape), myNet->getViewNet()->getUndoList());
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            // set new shape
            myShape = simplifiedShape;
            // Check if new shape is closed
            myClosedShape = (myShape.front() == myShape.back());
            // update geometry to avoid grabbing Problems
            updateGeometry();
        }
        // change flag after setting simplified shape
        mySimplifiedShape = true;
    } else {
        WRITE_WARNING("Polygon already simplified")
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
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_BLOCK_SHAPE:
            return toString(myBlockShape);
        case GNE_ATTR_CLOSE_SHAPE:
            return toString(myClosedShape);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
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
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_BLOCK_SHAPE:
        case GNE_ATTR_CLOSE_SHAPE:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myNet, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEPoly::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->retrievePolygon(value, false) == nullptr);
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
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_SHAPE:
            return canParse<bool>(value);
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
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNEPoly::getGenericParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i : getParametersMap()) {
        result += i.first + "=" + i.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNEPoly::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNEPoly::setGenericParametersStr(const std::string& value) {
    // clear parameters
    clearParameter();
    // separate value in a vector of string using | as separator
    std::vector<std::string> parsedValues;
    StringTokenizer stValues(value, "|", true);
    while (stValues.hasNext()) {
        parsedValues.push_back(stValues.next());
    }
    // check that parsed values (A=B)can be parsed in generic parameters
    for (auto i : parsedValues) {
        std::vector<std::string> parsedParameters;
        StringTokenizer stParam(i, "=", true);
        while (stParam.hasNext()) {
            parsedParameters.push_back(stParam.next());
        }
        // Check that parsed parameters are exactly two and contains valid chracters
        if (parsedParameters.size() == 2 && SUMOXMLDefinitions::isValidGenericParameterKey(parsedParameters.front()) && SUMOXMLDefinitions::isValidGenericParameterValue(parsedParameters.back())) {
            setParameter(parsedParameters.front(), parsedParameters.back());
        }
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEPoly::setAttribute(SumoXMLAttr key, const std::string& value) {
    // first remove object from grid due almost modificactions affects to boundary (but avoided for certain attributes)
    if ((key != SUMO_ATTR_ID) && (key != GNE_ATTR_GENERIC) && (key != GNE_ATTR_SELECTED)) {
        myNet->removeGLObjectFromGrid(this);
    }
    switch (key) {
        case SUMO_ATTR_ID: {
            std::string oldID = myID;
            myID = value;
            myNet->changeShapeID(this, oldID);
            setMicrosimID(value);
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
            // Check if new shape is closed
            myClosedShape = (myShape.front() == myShape.back());
            // disable simplified shape flag
            mySimplifiedShape = false;
            // update geometry of shape edited element
            if (myNetElementShapeEdited) {
                myNetElementShapeEdited->updateGeometry();
            }
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
            // Check if new shape is closed
            myClosedShape = (myShape.front() == myShape.back());
            // disable simplified shape flag
            mySimplifiedShape = false;
            // update geometry of shape edited element
            if (myNetElementShapeEdited) {
                myNetElementShapeEdited->updateGeometry();
            }
            break;
        }
        case SUMO_ATTR_COLOR:
            setShapeColor(parse<RGBColor>(value));
            break;
        case SUMO_ATTR_FILL:
            myFill = parse<bool>(value);
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
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
            break;
        case GNE_ATTR_BLOCK_SHAPE:
            myBlockShape = parse<bool>(value);
            break;
        case GNE_ATTR_CLOSE_SHAPE:
            myClosedShape = parse<bool>(value);
            if (myClosedShape) {
                myShape.closePolygon();
                myGeoShape.closePolygon();
            } else {
                myShape.pop_back();
                myGeoShape.pop_back();
            }
            // disable simplified shape flag
            mySimplifiedShape = false;
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // add object into grid again (but avoided for certain attributes)
    if ((key != SUMO_ATTR_ID) && (key != GNE_ATTR_GENERIC) && (key != GNE_ATTR_SELECTED)) {
        myNet->addGLObjectIntoGrid(this);
    }
}


const GUIGlObject*
GNEPoly::getGUIGlObject() const {
    return this;
}

/****************************************************************************/
