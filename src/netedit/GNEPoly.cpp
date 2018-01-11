/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utility>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <netimport/NIImporter_SUMO.h>
#include <netwrite/NWWriter_SUMO.h>

#include "GNENet.h"
#include "GNEEdge.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"
#include "GNEChange_Attribute.h"
#include "GNEPoly.h"
#include "GNEJunction.h"

// ===========================================================================
// static members
// ===========================================================================
const double GNEPoly::myHintSize = 0.8;

// ===========================================================================
// method definitions
// ===========================================================================
GNEPoly::GNEPoly(GNENet* net, const std::string& id, const std::string& type, const PositionVector& shape, bool geo, bool fill,
                 const RGBColor& color, double layer, double angle, const std::string& imgFile, bool movementBlocked, bool shapeBlocked) :
    GUIPolygon(id, type, color, shape, geo, fill, layer, angle, imgFile),
    GNEShape(net, SUMO_TAG_POLY, ICON_LOCATEPOLY, movementBlocked, shapeBlocked),
    myNetElementShapeEdited(NULL),
    myClosedShape(shape.front() == shape.back()),
    mySimplifiedShape(false),
    myCurrentMovingVertexIndex(-1) {
    // check if imgFile is valid
    if (!imgFile.empty() && GUITexturesHelper::getTextureID(imgFile) == -1) {
        setImgFile("");
    }
    // set GEO shape
    myGeoShape = myShape;
    for (int i = 0; i < (int) myGeoShape.size(); i++) {
        GeoConvHelper::getFinal().cartesian2geo(myGeoShape[i]);
    }
}


GNEPoly::~GNEPoly() {}


int
GNEPoly::moveVertexShape(const int index, const Position& oldPos, const Position& offset) {
    // only move shape if block movement block shape are disabled
    if (!myBlockMovement && !myBlockShape && (index != -1)) {
        // check that index is correct before change position
        if (index < (int)myShape.size()) {
            // save current moving vertex
            myCurrentMovingVertexIndex = index;
            // if closed shape and cliked is first or last, move both giving more priority to first always
            if (myClosedShape && (index == 0 || index == (int)myShape.size() - 1)) {
                myShape.front() = oldPos;
                myShape.front().add(offset);
                myShape.back() = oldPos;
                myShape.back().add(offset);
                return 0;
            } else {
                // change position of vertex
                myShape[index] = oldPos;
                myShape[index].add(offset);
                return index;
            }
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
        //  update Geometry after moving
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
        // first check if double points has to be removed
        shapeToCommit.removeDoublePoints(myHintSize);
        if (shapeToCommit.size() != myShape.size()) {
            WRITE_WARNING("Merged shape's point")
        }
        // check if polygon has to be closed
        if (shapeToCommit.size() > 1 && shapeToCommit.front().distanceTo2D(shapeToCommit.back()) < (2 * myHintSize)) {
            shapeToCommit.pop_back();
            shapeToCommit.push_back(shapeToCommit.front());
        }
        // only use GNEChange_Attribute if we aren't editing a junction's shape
        if (myNetElementShapeEdited == NULL) {
            myShape = oldShape;
            // commit new shape
            undoList->p_begin("moving " + toString(SUMO_ATTR_SHAPE) + " of " + toString(getTag()));
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_SHAPE, toString(shapeToCommit)));
            undoList->p_end();
        } else {
            setAttribute(SUMO_ATTR_SHAPE, toString(shapeToCommit));
        }
    }
}


void
GNEPoly::updateGeometry() {
    // simply refresh element in net
    myNet->refreshElement(this);
}

void
GNEPoly::writeShape(OutputDevice& device) {
    writeXML(device, myGEO);
}


Position
GNEPoly::getPositionInView() const {
    return myShape.getPolygonCenter();
}


GUIGlID
GNEPoly::getGlID() const {
    return GUIPolygon::getGlID();
}


const std::string&
GNEPoly::getParentName() const {
    if (myNetElementShapeEdited != NULL) {
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
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret);
    FXMenuCommand* simplifyShape = new FXMenuCommand(ret, "Simplify Shape\t\tReplace current shape with a rectangle", 0, &parent, MID_GNE_POLYGON_SIMPLIFY_SHAPE);
    // disable simplify shape if polygon was already simplified
    if (mySimplifiedShape) {
        simplifyShape->disable();
    }
    // create open or close polygon's shape only if myNetElementShapeEdited is NULL
    if (myNetElementShapeEdited == NULL) {
        if (myClosedShape) {
            new FXMenuCommand(ret, "Open shape\t\tOpen polygon's shape", 0, &parent, MID_GNE_POLYGON_OPEN);
        } else {
            new FXMenuCommand(ret, "Close shape\t\tClose polygon's shape", 0, &parent, MID_GNE_POLYGON_CLOSE);
        }
    }
    // create a extra FXMenuCommand if mouse is over a vertex
    int index = getVertexIndex(myNet->getViewNet()->getPositionInformation(), false);
    if (index != -1) {
        FXMenuCommand* removeGeometryPoint = new FXMenuCommand(ret, "Remove geometry point\t\tRemove geometry point under mouse", 0, &parent, MID_GNE_POLYGON_DELETE_GEOMETRY_POINT);
        FXMenuCommand* setFirstPoint = new FXMenuCommand(ret, "Set first geometry point\t\tSet", 0, &parent, MID_GNE_POLYGON_SET_FIRST_POINT);
        // disable setFirstPoint if shape only have three points
        if ((myClosedShape && (myShape.size() <= 4)) || (!myClosedShape && (myShape.size() <= 3))) {
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


Boundary
GNEPoly::getCenteringBoundary() const {
    return GUIPolygon::getCenteringBoundary();
}


void
GNEPoly::drawGL(const GUIVisualizationSettings& s) const {
    // change temporally attribute color if polygon is selected
    bool selected = gSelected.isSelected(GLO_POLYGON, getGlID());
    // simply use GUIPolygon::drawGL
    GUIPolygon::drawGL(s);
    // draw geometry details hints if is not too small
    if (s.scale * myHintSize > 1.) {
        // set values relative to mouse position regarding to shape
        bool mouseOverVertex = false;
        bool modeMove = myNet->getViewNet()->getCurrentEditMode() == GNE_MODE_MOVE;
        Position mousePosition = myNet->getViewNet()->getPositionInformation();
        double distanceToShape = myShape.distance2D(mousePosition);
        Position PostionOverShapeLine = myShape.size() > 1 ? myShape.positionAtOffset2D(myShape.nearest_offset_to_point2D(mousePosition)) : myShape[0];
        // set colors
        RGBColor invertedColor, darkerColor;
        if (selected) {
            invertedColor = myNet->selectionColor.invertedColor();
            darkerColor = myNet->selectedLaneColor;
        } else {
            invertedColor = GLHelper::getColor().invertedColor();
            darkerColor = GLHelper::getColor().changedBrightness(-32);
        }
        // push matrix
        glPushName(getGlID());
        // Draw geometry hints if polygon's shape isn't blocked
        if (myBlockShape == false) {
            // draw a boundary for moving using darkerColor
            glPushMatrix();
            glTranslated(0, 0, GLO_POLYGON + 0.01);
            GLHelper::setColor(darkerColor);
            GLHelper::drawBoxLines(myShape, (myHintSize / 4) * s.polySize.getExaggeration(s));
            glPopMatrix();
            // draw points of shape
            for (auto i : myShape) {
                glPushMatrix();
                glTranslated(i.x(), i.y(), GLO_POLYGON + 0.02);
                // Change color of vertex and flag mouseOverVertex if mouse is over vertex
                if (modeMove && (i.distanceTo(mousePosition) < myHintSize)) {
                    mouseOverVertex = true;
                    GLHelper::setColor(invertedColor);
                } else {
                    GLHelper::setColor(darkerColor);
                }
                GLHelper:: drawFilledCircle(myHintSize, 32);
                glPopMatrix();
                // draw special symbols (Start, End and Block)
                if (i == myShape.front()) {
                    // draw a "s" over first point
                    glPushMatrix();
                    glTranslated(i.x(), i.y(), GLO_POLYGON + 0.03);
                    GLHelper::drawText("S", Position(), .1, 2 * myHintSize, invertedColor);
                    glPopMatrix();
                } else if ((i == myShape.back()) && (myClosedShape == false)) {
                    // draw a "e" over last point if polygon isn't closed
                    glPushMatrix();
                    glTranslated(i.x(), i.y(), GLO_POLYGON + 0.03);
                    GLHelper::drawText("E", Position(), .1, 2 * myHintSize, invertedColor);
                    glPopMatrix();
                }
            }
            // check if draw moving hint has to be drawed
            if (modeMove && (mouseOverVertex == false) && (myBlockMovement == false) && (distanceToShape < myHintSize)) {
                // push matrix
                glPushMatrix();
                glTranslated(PostionOverShapeLine.x(), PostionOverShapeLine.y(), GLO_POLYGON + 0.04);
                GLHelper::setColor(invertedColor);
                GLHelper:: drawFilledCircle(myHintSize, 32);
                glPopMatrix();
            }
        }
        // pop name
        glPopName();
    }
}


int
GNEPoly::getVertexIndex(const Position& pos, bool createIfNoExist) {
    // first check if vertex already exists
    for (auto i : myShape) {
        if (i.distanceTo2D(pos) < myHintSize) {
            return myShape.indexOfClosest(i);
        }
    }
    // if vertex doesn't exist, insert it
    if (createIfNoExist && (myShape.distance2D(pos) < myHintSize)) {
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
        if (myClosedShape && (index == 0 || index == (int)modifiedShape.size() - 1)) {
            modifiedShape.erase(modifiedShape.begin());
            modifiedShape.erase(modifiedShape.end() - 1);
            myShape.push_back(modifiedShape.front());
        } else {
            modifiedShape.erase(modifiedShape.begin() + index);
        }
        // set new shape depending of allowUndo
        if (allowUndo) {
            myNet->getViewNet()->getUndoList()->p_begin("delete geometry point");
            setAttribute(SUMO_ATTR_SHAPE, toString(modifiedShape), myNet->getViewNet()->getUndoList());
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            // set new shape
            myShape = modifiedShape;
            // Check if new shape is closed
            myClosedShape = (myShape.front() == myShape.back());
            // disable simplified shape flag
            mySimplifiedShape = false;
            // update geometry to avoid grabbing Problems
            updateGeometry();
        }
    } else {
        WRITE_WARNING("Number of remaining points insufficient")
    }
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
        throw InvalidArgument("Junction cannot be NULL");
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
    if (!mySimplifiedShape) {
        const Boundary b =  myShape.getBoxBoundary();
        PositionVector simplifiedShape;
        // create a square as simplified shape
        simplifiedShape.push_back(Position(b.xmin(), b.ymin()));
        simplifiedShape.push_back(Position(b.xmin(), b.ymax()));
        simplifiedShape.push_back(Position(b.xmax(), b.ymax()));
        simplifiedShape.push_back(Position(b.xmax(), b.ymin()));
        simplifiedShape.push_back(simplifiedShape[0]);
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
            return toString(myGeoShape);
        case SUMO_ATTR_COLOR:
            return toString(myColor);
        case SUMO_ATTR_FILL:
            return toString(myFill);
        case SUMO_ATTR_LAYER:
            return toString(myLayer);
        case SUMO_ATTR_TYPE:
            return myType;
        case SUMO_ATTR_IMGFILE:
            return myImgFile;
        case SUMO_ATTR_ANGLE:
            return toString(getNaviDegree());
        case SUMO_ATTR_GEO:
            return toString(myGEO);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_BLOCK_SHAPE:
            return toString(myBlockShape);
        case GNE_ATTR_CLOSE_SHAPE:
            return toString(myClosedShape);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
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
        case SUMO_ATTR_LAYER:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_IMGFILE:
        case SUMO_ATTR_ANGLE:
        case SUMO_ATTR_GEO:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_BLOCK_SHAPE:
        case GNE_ATTR_CLOSE_SHAPE:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEPoly::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidID(value) && (myNet->retrievePolygon(value, false) == 0);
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_GEOSHAPE: {
            bool ok = true;
            // check if shape can be parsed
            PositionVector shape = GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, true);
            return (shape.size() > 0);
        }
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_FILL:
            return canParse<bool>(value);
        case SUMO_ATTR_LAYER:
            return canParse<double>(value);
        case SUMO_ATTR_TYPE:
            return true;
        case SUMO_ATTR_IMGFILE:
            if (value == "") {
                return true;
            } else if (isValidFilename(value)) {
                // check that image can be loaded
                return GUITexturesHelper::getTextureID(value) != -1;
            } else {
                return false;
            }
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
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEPoly::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID: {
            std::string oldID = myID;
            myID = value;
            myNet->changeShapeID(this, oldID);
            break;
        }
        case SUMO_ATTR_SHAPE: {
            bool ok = true;
            // set new shape
            myShape = GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, true);
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
            bool ok = true;
            // set new GEO shape
            myGeoShape = GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, true);
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
            myColor = parse<RGBColor>(value);
            break;
        case SUMO_ATTR_FILL:
            myFill = parse<bool>(value);
            break;
        case SUMO_ATTR_LAYER:
            myLayer = parse<double>(value);
            break;
        case SUMO_ATTR_TYPE:
            myType = value;
            break;
        case SUMO_ATTR_IMGFILE:
            myType = value;
            break;
        case SUMO_ATTR_ANGLE:
            setNaviDegree(parse<double>(value));
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
            } else {
                myShape.pop_back();
            }
            // disable simplified shape flag
            mySimplifiedShape = false;
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // update geometry after every change
    updateGeometry();
}


/****************************************************************************/
