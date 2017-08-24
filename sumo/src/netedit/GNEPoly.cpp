/****************************************************************************/
/// @file    GNEPoly.cpp
/// @author  Jakob Erdmann
/// @date    Sept 2012
/// @version $Id$
///
// A class for visualizing and editing POIS in netedit (adapted from
// GUIPolygon and NLHandler)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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
#include <foreign/polyfonts/polyfonts.h>
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
// method definitions
// ===========================================================================
GNEPoly::GNEPoly(GNENet* net, GNEJunction* junction, const std::string& id, const std::string& type, const PositionVector& shape, bool fill,
                 const RGBColor& color, double layer, double angle, const std::string& imgFile, bool movementBlocked, bool shapeBlocked) :
    GUIPolygon(id, type, color, shape, fill, layer, angle, imgFile),
    GNEShape(net, SUMO_TAG_POLY, ICON_LOCATEPOLY, movementBlocked, shapeBlocked),
    myJunction(junction),
    myClosedShape(shape.front() == shape.back()),
    myCurrentMovingVertexIndex(-1) {
    // check that number of points is correct and area isn't empty
    assert((shape.size() >= 2) && (shape.area() > 0));
    // update boundary blocked shape
    updateBoundaryBlockedShape(1);
}


GNEPoly::~GNEPoly() {}


int 
GNEPoly::moveVertexShape(int index, const Position& newPos) {
    // only move shape if block movement block shape are disabled
    if(!myBlockMovement && !myBlockShape && (index != -1)) {
        // check that index is correct before change position
        if(index < myShape.size()) {
            // save current moving vertex
            myCurrentMovingVertexIndex = index;
            // change position of vertex
            myShape[index] = newPos;
            return index;
        } else {
            throw InvalidArgument("Index greather than Shape size");
        }
    } else {
        return index;
    }
}


void 
GNEPoly::moveEntireShape(const PositionVector& oldShape, const Position& offset) {
    // only move shape if block movement is disabled and block shape is enabled
    if(!myBlockMovement && myBlockShape) {
        // clear myDrawingBlockedShape
        myDrawingBlockedShape.clear();
        // restore original shape
        myShape = oldShape;
        // change all points of the shape shape using noffset
        for (auto i = myShape.begin(); i != myShape.end(); i++) {
            i->setx(i->x() + offset.x());
            i->sety(i->y() + offset.y());
        }
        // refresh element
        myNet->refreshPolygon(this);
    }
}


void 
GNEPoly::commitShapeChange(const PositionVector& oldShape, GNEUndoList* undoList) {
    if(!myBlockMovement) {
        // disable current moving vertex
        myCurrentMovingVertexIndex = -1;
        // restore original shape (needed for commit change correctly)
        PositionVector shapeToCommit = myShape;
        myShape = oldShape;
        // commit new shape
        undoList->p_begin("moving " + toString(SUMO_ATTR_SHAPE) + " of " + toString(getTag()));
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_SHAPE, toString(shapeToCommit)));
        undoList->p_end();
    }
}


void GNEPoly::writeShape(OutputDevice &device) {
    writeXML(device);
}


Position 
GNEPoly::getPositionInView() const {
    return myShape.getPolygonCenter();
}


const std::string& 
GNEPoly::getParentName() const {
    if(myJunction != NULL) {
        return myJunction->getMicrosimID();
    } else {
        return myNet->getMicrosimID();
    }
}


GUIGLObjectPopupMenu*
GNEPoly::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = GUIPolygon::getPopUpMenu(app, parent);
    new FXMenuSeparator(ret);
    new FXMenuCommand(ret, "Set custom shape (ENTER)", 0, &app, MID_GNE_HOTKEY_ENTER);
    new FXMenuCommand(ret, "Discard custom shape (ESC)", 0, &app, MID_GNE_ABORT);
    new FXMenuCommand(ret, "Simplify Shape\t\tReplace shape with a rectangle", 0, &parent, MID_GNE_SIMPLIFY_SHAPE);
    new FXMenuCommand(ret, "Remove geometry point\t\tRemove the closest geometry point", 0, &parent, MID_GNE_DELETE_GEOMETRY);
    // let the GNEViewNet store the popup position
    (dynamic_cast<GNEViewNet&>(parent)).markPopupPosition();
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
    const double hintSize = 0.8;
    // first draw polygon
    GUIPolygon::drawGL(s);
    // draw geometry details hints if is not too small
    if (s.scale * hintSize > 1.) {
        // set values relative to mouse position regarding to shape
        bool mouseOverVertex = false;
        Position mousePosition = myNet->getViewNet()->getPositionInformation();
        double distanceToShape = myShape.distance2D(mousePosition);
        Position PostionOverShapeLine = myShape.positionAtOffset2D(myShape.nearest_offset_to_point2D(mousePosition));        
        // set colors
        RGBColor invertedColor = GLHelper::getColor().invertedColor();
        RGBColor darkerColor = GLHelper::getColor().changedBrightness(-32);
        // push matrix
        glPushName(getGlID());
        // Draw geometry hints if polygon's shape isn't blocked
        if(myBlockShape == false) {
            // draw a boundary for moving using darkerColor
            glPushMatrix();
            glTranslated(0, 0, GLO_POLYGON + 0.01);
            GLHelper::setColor(darkerColor);
            GLHelper::drawBoxLines(myShape, (hintSize/4) * s.polySize.getExaggeration(s));
            glPopMatrix();
            // draw points of shape
            for (auto i : myShape) {
                glPushMatrix();
                glTranslated(i.x(), i.y(), GLO_POLYGON + 0.02);
                // Change color of vertex and flag mouseOverVertex if mouse is over vertex
                if(i.distanceTo(mousePosition) < hintSize) {
                    mouseOverVertex = true;
                    GLHelper::setColor(invertedColor);
                } else {
                    GLHelper::setColor(darkerColor);
                }
                GLHelper:: drawFilledCircle(hintSize, 32);
                glPopMatrix();
                // draw special symbols (Start, End and Block)
                if(i == myShape.front()) {
                    // draw a "s" over first point
                    glPushMatrix();
                    glTranslated(i.x(), i.y(), GLO_POLYGON + 0.03);
                    GLHelper::drawText("S", Position(), .1, 1.6, invertedColor);
                    glPopMatrix();
                } else if ((i == myShape.back()) && (myClosedShape == false)) {
                    // draw a "e" over last point if polygon isn't closed
                    glPushMatrix();
                    glTranslated(i.x(), i.y(), GLO_POLYGON + 0.03);
                    GLHelper::drawText("E", Position(), .1, 1.6, invertedColor);
                    glPopMatrix();
                } else {
                    drawLockIcon(i, GLO_POLYGON, 0.25);
                }
            }
            // check if draw moving hint has to be drawed
            if((mouseOverVertex == false) && (myBlockMovement == false) && 
                (myNet->getViewNet()->getCurrentEditMode() == GNE_MODE_MOVE) && (distanceToShape < hintSize)) {
                // push matrix
                glPushMatrix();
                glTranslated(PostionOverShapeLine.x(), PostionOverShapeLine.y(), GLO_POLYGON + 2);
                GLHelper::setColor(invertedColor);
                GLHelper:: drawFilledCircle(hintSize, 32);
                glPopMatrix();
            }
        } else if (myFill) {
            // draw only a block icon in center if polygon is filled
            drawLockIcon(myShape.getPolygonCenter(), GLO_POLYGON, 0.25);
        } else {
            // draw a lock in every vertex if polygon isn't filled
            for (auto i : myShape) {
                drawLockIcon(i, GLO_POLYGON, 0.25);
            }
        }
        // pop name
        glPopName();
    }
}


int GNEPoly::getVertexIndex(const Position &pos) {
    // first check if vertex already exists
    for(auto i : myShape) {
        if (i.distanceTo2D(pos) < 0.8) {
            return myShape.indexOfClosest(i);
        }
    }
    // if vertex doesn't exist, insert it
    if(myShape.distance2D(pos) < 0.8) {
        return myShape.insertAtClosest(pos);
    } else {
        return -1;
    }
}


bool 
GNEPoly::isPolygonClosed() const {
    return myClosedShape;
}


void
GNEPoly::simplifyShape() {
    const Boundary b =  myShape.getBoxBoundary();
    myShape.clear();
    myShape.push_back(Position(b.xmin(), b.ymin()));
    myShape.push_back(Position(b.xmin(), b.ymax()));
    myShape.push_back(Position(b.xmax(), b.ymax()));
    myShape.push_back(Position(b.xmax(), b.ymin()));
    myShape.push_back(myShape[0]);
}


void
GNEPoly::deleteGeometryNear(const Position& pos) {
    if (myShape.size() <= 3) {
        return;
    }
    int index = myShape.indexOfClosest(pos);
    if ((index == 0 || index == (int)myShape.size() - 1) && myShape.front() == myShape.back()) {
        myShape.erase(myShape.begin());
        myShape.erase(myShape.end() - 1);
        myShape.push_back(myShape.front());
    } else {
        myShape.erase(myShape.begin() + index);
    }
}


GNEJunction* 
GNEPoly::getEditedJunction() const {
    return myJunction;
}


std::string
GNEPoly::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myID;
        case SUMO_ATTR_SHAPE:
            return toString(myShape);
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
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_FILL:
        case SUMO_ATTR_LAYER:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_IMGFILE:
        case SUMO_ATTR_ANGLE:
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
        case SUMO_ATTR_SHAPE: {
            bool ok = true;
            PositionVector shape = GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, true);
            // check if shape was sucesfully parsed
            if(ok) {
                // remove consecutive points
                shape.removeDoublePoints();
                // shape is valid if has more than three points and shape's are isn't empty
                return (shape.size() >= 2) && (shape.area() > 0);
            } else {
                return false;
            }
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
            return isValidFilename(value);
        case SUMO_ATTR_ANGLE:
            return canParse<double>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_SHAPE:
            return canParse<bool>(value);
        case GNE_ATTR_CLOSE_SHAPE:
            if(canParse<bool>(value)) {
                bool closePolygon = parse<bool>(value);
                if(closePolygon && (myShape.begin() == myShape.end())) {
                    // Polygon already closed, then invalid value
                    return false;
                } else if(!closePolygon && (myShape.begin() != myShape.end())) {
                    // Polygon already open, then invalid value
                    return false;
                } else{
                    return true;
                }
            }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void 
GNEPoly::updateBoundaryBlockedShape(double distanceBetweenPoints) {
    assert (distanceBetweenPoints > 0);
    // clear myDrawingBlockedShape
    myDrawingBlockedShape.clear();
    double currentPosition = distanceBetweenPoints;
    // add first position
    myDrawingBlockedShape.push_back(myShape.front());
    // calculate rest of positions
    while (currentPosition < myShape.length()) {
        if(currentPosition <= myShape.length()) {
            myDrawingBlockedShape.push_back(myShape.positionAtOffset(currentPosition));
        } else {
            myDrawingBlockedShape.push_back(myShape.back());
        }
        // update current position
        currentPosition += distanceBetweenPoints;
    }
}


void
GNEPoly::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID: {
            std::string oldID = myID;
            myID = value;
            myNet->changePolygonID(this, oldID);
            break;
        }
        case SUMO_ATTR_SHAPE: {
            bool ok = true;
            // set new shape
            myShape = GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, true);
            // Check if new shape is closed
            myClosedShape = myShape.begin() == myShape.end();
            // update boundary blocked shape
            updateBoundaryBlockedShape(1);
            // refresh polygon in net to avoid grabbing problems
            myNet->refreshPolygon(this);
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
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
            break;
        case GNE_ATTR_BLOCK_SHAPE:
            myBlockShape = parse<bool>(value);
            break;
        case GNE_ATTR_CLOSE_SHAPE:
            myClosedShape = parse<bool>(value);
            if(myClosedShape) {
                myShape.closePolygon();
            } else {
                myShape.pop_back();
            }
            // update boundary blocked shape
            updateBoundaryBlockedShape(1);
            // refresh polygon in net to avoid grabbing problems
            myNet->refreshPolygon(this);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // update view after every change
    myNet->getViewNet()->update();
}


/****************************************************************************/
