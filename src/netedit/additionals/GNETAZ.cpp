/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETAZ.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2018
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/div/GLHelper.h>
#include <netedit/frames/GNETAZFrame.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/globjects/GLIncludes.h>
#include "GNETAZ.h"


// ===========================================================================
// static members
// ===========================================================================
const double GNETAZ::myHintSize = 0.8;
const double GNETAZ::myHintSizeSquared = 0.64;


// ===========================================================================
// member method definitions
// ===========================================================================
GNETAZ::GNETAZ(const std::string& id, GNEViewNet* viewNet, PositionVector shape, RGBColor color, bool blockMovement) :
    GNEAdditional(id, viewNet, GLO_TAZ, SUMO_TAG_TAZ, "", blockMovement, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}),
              myColor(color),
              myBlockShape(false),
              myCurrentMovingVertexIndex(-1),
              myMaxWeightSource(0),
              myMinWeightSource(0),
              myAverageWeightSource(0),
              myMaxWeightSink(0),
              myMinWeightSink(0),
myAverageWeightSink(0) {
    // set TAZ shape
    myGeometry.shape = shape;
}


GNETAZ::~GNETAZ() {}


void
GNETAZ::updateGeometry() {
    // Nothing to do
}


Position
GNETAZ::getPositionInView() const {
    return myGeometry.shape.getCentroid();
}


Boundary
GNETAZ::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (myMove.movingGeometryBoundary.isInitialised()) {
        return myMove.movingGeometryBoundary;
    } else if (myGeometry.shape.size() > 0) {
        Boundary b = myGeometry.shape.getBoxBoundary();
        b.grow(20);
        return b;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNETAZ::moveGeometry(const Position& offset) {
    // restore old position, apply offset and update Geometry
    myGeometry.shape[0] = myMove.originalViewPosition;
    myGeometry.shape[0].add(offset);
    // filtern position using snap to active grid
    myGeometry.shape[0] = myViewNet->snapToActiveGrid(myGeometry.shape[0]);
    updateGeometry();
}


void
GNETAZ::commitGeometryMoving(GNEUndoList* undoList) {
    // commit new position allowing undo/redo
    undoList->p_begin("position of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_SHAPE, toString(myGeometry.shape[0]), true, toString(myMove.originalViewPosition)));
    undoList->p_end();
}


int
GNETAZ::moveVertexShape(const int index, const Position& oldPos, const Position& offset) {
    // only move shape if block movement block shape are disabled
    if (!myBlockMovement && !myBlockShape && (index != -1)) {
        // check that index is correct before change position
        if (index < (int)myGeometry.shape.size()) {
            // save current moving Geometry Point
            myCurrentMovingVertexIndex = index;
            // if closed shape and cliked is first or last, move both giving more priority to first always
            if ((index == 0 || index == (int)myGeometry.shape.size() - 1)) {
                // Change position of first shape Geometry Point and filtern position using snap to active grid
                myGeometry.shape.front() = oldPos;
                myGeometry.shape.front().add(offset);
                myGeometry.shape.front() = myViewNet->snapToActiveGrid(myGeometry.shape.front());
                // Change position of last shape Geometry Point and filtern position using snap to active grid
                myGeometry.shape.back() = oldPos;
                myGeometry.shape.back().add(offset);
                myGeometry.shape.back() = myViewNet->snapToActiveGrid(myGeometry.shape.back());
            } else {
                // change position of Geometry Point and filtern position using snap to active grid
                myGeometry.shape[index] = oldPos;
                myGeometry.shape[index].add(offset);
                myGeometry.shape[index] = myViewNet->snapToActiveGrid(myGeometry.shape[index]);
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
GNETAZ::moveEntireShape(const PositionVector& oldShape, const Position& offset) {
    // only move shape if block movement is disabled and block shape is enabled
    if (!myBlockMovement && myBlockShape) {
        // restore original shape
        myGeometry.shape = oldShape;
        // change all points of the shape shape using offset
        for (auto& i : myGeometry.shape) {
            i.add(offset);
        }
        // update Geometry after moving
        updateGeometry();
    }
}


void
GNETAZ::commitShapeChange(const PositionVector& oldShape, GNEUndoList* undoList) {
    if (!myBlockMovement) {
        // disable current moving vertex
        myCurrentMovingVertexIndex = -1;
        // restore original shape into shapeToCommit
        PositionVector shapeToCommit = myGeometry.shape;
        // restore old shape in polygon (to avoid problems with RTree)
        myGeometry.shape = oldShape;
        // first check if double points has to be removed
        shapeToCommit.removeDoublePoints(myHintSize);
        if (shapeToCommit.size() != myGeometry.shape.size()) {
            WRITE_WARNING("Merged shape's point")
        }
        // check if polygon has to be closed
        if (shapeToCommit.size() > 1 && shapeToCommit.front().distanceTo2D(shapeToCommit.back()) < (2 * myHintSize)) {
            shapeToCommit.pop_back();
            shapeToCommit.push_back(shapeToCommit.front());
        }
        // commit new shape
        undoList->p_begin("moving " + toString(SUMO_ATTR_SHAPE) + " of " + getTagStr());
        undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_SHAPE, toString(shapeToCommit)));
        undoList->p_end();
    }
}


int
GNETAZ::getVertexIndex(Position pos, bool createIfNoExist, bool snapToGrid) {
    // check if position has to be snapped to grid
    if (snapToGrid) {
        pos = myViewNet->snapToActiveGrid(pos);
    }
    // first check if vertex already exists
    for (auto i : myGeometry.shape) {
        if (i.distanceTo2D(pos) < myHintSize) {
            return myGeometry.shape.indexOfClosest(i);
        }
    }
    // if vertex doesn't exist, insert it
    if (createIfNoExist) {
        return myGeometry.shape.insertAtClosest(pos);
    } else {
        return -1;
    }
}


void
GNETAZ::deleteGeometryPoint(const Position& pos, bool allowUndo) {
    if (myGeometry.shape.size() > 2) {
        // obtain index
        PositionVector modifiedShape = myGeometry.shape;
        int index = modifiedShape.indexOfClosest(pos);
        // remove point dependending of
        if ((index == 0 || index == (int)modifiedShape.size() - 1)) {
            modifiedShape.erase(modifiedShape.begin());
            modifiedShape.erase(modifiedShape.end() - 1);
            modifiedShape.push_back(modifiedShape.front());
        } else {
            modifiedShape.erase(modifiedShape.begin() + index);
        }
        // set new shape depending of allowUndo
        if (allowUndo) {
            myViewNet->getUndoList()->p_begin("delete geometry point");
            setAttribute(SUMO_ATTR_SHAPE, toString(modifiedShape), myViewNet->getUndoList());
            myViewNet->getUndoList()->p_end();
        } else {
            // first remove object from grid due shape is used for boundary
            myViewNet->getNet()->removeGLObjectFromGrid(this);
            // set new shape
            myGeometry.shape = modifiedShape;
            // add object into grid again
            myViewNet->getNet()->addGLObjectIntoGrid(this);
        }
    } else {
        WRITE_WARNING("Number of remaining points insufficient")
    }
}


bool
GNETAZ::isShapeBlocked() const {
    return myBlockShape;
}


std::string
GNETAZ::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


void
GNETAZ::drawGL(const GUIVisualizationSettings& s) const {
    // check if boundary has to be drawn
    if (s.drawBoundaries) {
        GLHelper::drawBoundary(getCenteringBoundary());
    }
    if (s.polySize.getExaggeration(s, this) == 0) {
        return;
    }
    Boundary boundary = myGeometry.shape.getBoxBoundary();
    if (s.scale * MAX2(boundary.getWidth(), boundary.getHeight()) < s.polySize.minSize) {
        return;
    }
    glPushName(getGlID());
    if (myGeometry.shape.size() > 1) {
        glPushMatrix();
        glTranslated(0, 0, 128);
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectionColor);
        } else {
            GLHelper::setColor(myColor);
        }
        GLHelper::drawLine(myGeometry.shape);
        GLHelper::drawBoxLines(myGeometry.shape, s.polySize.getExaggeration(s, this));
        glPopMatrix();
        const Position namePos = myGeometry.shape.getPolygonCenter();
        drawName(namePos, s.scale, s.polyName, s.angle);
    }
    // draw geometry details hints if is not too small and isn't in selecting mode
    if (s.scale * myHintSize > 1.) {
        // set values relative to mouse position regarding to shape
        bool mouseOverVertex = false;
        bool modeMove = myViewNet->getEditModes().networkEditMode == GNE_NMODE_MOVE;
        Position mousePosition = myViewNet->getPositionInformation();
        double distanceToShape = myGeometry.shape.distance2D(mousePosition);
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
            GLHelper::drawBoxLines(myGeometry.shape, (myHintSize / 4) * s.polySize.getExaggeration(s, this));
            glPopMatrix();
            // draw shape points only in Network supemode
            if (myViewNet->getEditModes().currentSupermode != GNE_SUPERMODE_DEMAND) {
                for (auto i : myGeometry.shape) {
                    if (!s.drawForSelecting || (myViewNet->getPositionInformation().distanceSquaredTo2D(i) <= (myHintSizeSquared + 2))) {
                        glPushMatrix();
                        glTranslated(i.x(), i.y(), GLO_POLYGON + 0.02);
                        // Change color of vertex and flag mouseOverVertex if mouse is over vertex
                        if (modeMove && (i.distanceTo(mousePosition) < myHintSize)) {
                            mouseOverVertex = true;
                            GLHelper::setColor(invertedColor);
                        } else {
                            GLHelper::setColor(darkerColor);
                        }
                        GLHelper::drawFilledCircle(myHintSize, s.getCircleResolution());
                        glPopMatrix();
                    }
                }
                // check if draw moving hint has to be drawed
                if (modeMove && (mouseOverVertex == false) && (myBlockMovement == false) && (distanceToShape < myHintSize)) {
                    // push matrix
                    glPushMatrix();
                    Position hintPos = myGeometry.shape.size() > 1 ? myGeometry.shape.positionAtOffset2D(myGeometry.shape.nearest_offset_to_point2D(mousePosition)) : myGeometry.shape[0];
                    glTranslated(hintPos.x(), hintPos.y(), GLO_POLYGON + 0.04);
                    GLHelper::setColor(invertedColor);
                    GLHelper:: drawFilledCircle(myHintSize, s.getCircleResolution());
                    glPopMatrix();
                }
            }
        }
    }
    // check if dotted contour has to be drawn
    if ((myViewNet->getDottedAC() == this) || (myViewNet->getViewParent()->getTAZFrame()->getTAZCurrentModul()->getTAZ() == this)) {
        GLHelper::drawShapeDottedContourAroundClosedShape(s, GLO_POLYGON + 1, getShape());
    }
    // pop name
    glPopName();
}


std::string
GNETAZ::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_SHAPE:
            return toString(myGeometry.shape);
        case SUMO_ATTR_COLOR:
            return toString(myColor);
        case SUMO_ATTR_EDGES: {
            std::vector<std::string> edgeIDs;
            for (auto i : getAdditionalChildren()) {
                edgeIDs.push_back(i->getAttribute(SUMO_ATTR_EDGE));
            }
            return toString(edgeIDs);
        }
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_BLOCK_SHAPE:
            return toString(myBlockShape);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        case GNE_ATTR_MIN_SOURCE:
            return toString(myMinWeightSource);
        case GNE_ATTR_MIN_SINK:
            return toString(myMinWeightSink);
        case GNE_ATTR_MAX_SOURCE:
            return toString(myMaxWeightSource);
        case GNE_ATTR_MAX_SINK:
            return toString(myMaxWeightSink);
        case GNE_ATTR_AVERAGE_SOURCE:
            return toString(myAverageWeightSource);
        case GNE_ATTR_AVERAGE_SINK:
            return toString(myAverageWeightSink);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNETAZ::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_EDGES:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_BLOCK_SHAPE:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNETAZ::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_SHAPE:
            return canParse<PositionVector>(value);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_EDGES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfTypeID(value);
            }
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_SHAPE:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNETAZ::getPopUpID() const {
    return getTagStr() + ":" + getID();
}


std::string
GNETAZ::getHierarchyName() const {
    return getTagStr();
}


void
GNETAZ::updateAdditionalParent() {
    // reset all stadistic variables
    myMaxWeightSource = 0;
    myMinWeightSource = -1;
    myAverageWeightSource = 0;
    myMaxWeightSink = 0;
    myMinWeightSink = -1;
    myAverageWeightSink = 0;
    // declare an extra variables for saving number of children
    int numberOfSources = 0;
    int numberOfSinks = 0;
    // iterate over additional children
    for (auto i : getAdditionalChildren()) {
        if (i->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
            double weight = parse<double>(i->getAttribute(SUMO_ATTR_WEIGHT));
            // check max Weight
            if (myMaxWeightSource < weight) {
                myMaxWeightSource = weight;
            }
            // check min Weight
            if ((myMinWeightSource == -1) || (weight < myMinWeightSource)) {
                myMinWeightSource = weight;
            }
            // update Average
            myAverageWeightSource += weight;
            // update number of sources
            numberOfSources++;
        } else if (i->getTagProperty().getTag() == SUMO_TAG_TAZSINK) {
            double weight = parse<double>(i->getAttribute(SUMO_ATTR_WEIGHT));
            // check max Weight
            if (myMaxWeightSink < weight) {
                myMaxWeightSink = weight;
            }
            // check min Weight
            if ((myMinWeightSink == -1) || (weight < myMinWeightSink)) {
                myMinWeightSink = weight;
            }
            // update Average
            myAverageWeightSink += weight;
            // update number of sinks
            numberOfSinks++;
        }
    }
    // calculate average
    myAverageWeightSource /= numberOfSources;
    myAverageWeightSink /= numberOfSinks;
}

// ===========================================================================
// private
// ===========================================================================

void
GNETAZ::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_SHAPE:
            myViewNet->getNet()->removeGLObjectFromGrid(this);
            myGeometry.shape = parse<PositionVector>(value);
            myViewNet->getNet()->addGLObjectIntoGrid(this);
            break;
        case SUMO_ATTR_COLOR:
            myColor = parse<RGBColor>(value);
            break;
        case SUMO_ATTR_EDGES:
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
            break;
        case GNE_ATTR_BLOCK_SHAPE:
            myBlockShape = parse<bool>(value);
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
}


/****************************************************************************/
