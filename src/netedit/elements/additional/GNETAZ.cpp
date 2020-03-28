/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNETAZ.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2018
///
//
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/network/GNETAZFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNETAZ.h"


// ===========================================================================
// callbacks definitions
// ===========================================================================

void APIENTRY beginTAZCallback(GLenum which) {
    glBegin(which);
}


void APIENTRY endTAZCallback(void) {
    glEnd();
}


void APIENTRY combineTAZCallback(GLdouble coords[3],
                                 GLdouble* vertex_data[4],
                                 GLfloat weight[4], GLdouble** dataOut) {
    UNUSED_PARAMETER(weight);
    UNUSED_PARAMETER(*vertex_data);
    GLdouble* vertex;

    vertex = (GLdouble*) malloc(7 * sizeof(GLdouble));

    vertex[0] = coords[0];
    vertex[1] = coords[1];
    vertex[2] = coords[2];
    *dataOut = vertex;
}

// ===========================================================================
// static members
// ===========================================================================
const double GNETAZ::myHintSize = 0.8;
const double GNETAZ::myHintSizeSquared = 0.64;


// ===========================================================================
// member method definitions
// ===========================================================================
GNETAZ::GNETAZ(const std::string& id, GNEViewNet* viewNet, PositionVector shape, RGBColor color, bool blockMovement) :
    GNEAdditional(id, viewNet, GLO_TAZ, SUMO_TAG_TAZ, "", blockMovement, 
        {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}),
    myColor(color),
    myTAZShape(shape),
    myBlockShape(false),
    myDrawFill(false),
    myCurrentMovingVertexIndex(-1),
    myMaxWeightSource(0),
    myMinWeightSource(0),
    myAverageWeightSource(0),
    myMaxWeightSink(0),
    myMinWeightSink(0),
    myAverageWeightSink(0) {
}


GNETAZ::~GNETAZ() {}


const PositionVector&
GNETAZ::getTAZShape() const {
    return myTAZShape;
}


void
GNETAZ::updateGeometry() {
    // Nothing to do
}


void
GNETAZ::updateDottedContour() {
    myDottedGeometry.updateDottedGeometry(myViewNet->getVisualisationSettings(), myTAZShape);
}


Position
GNETAZ::getPositionInView() const {
    return myTAZShape.getCentroid();
}


Boundary
GNETAZ::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (myMove.movingGeometryBoundary.isInitialised()) {
        return myMove.movingGeometryBoundary;
    } else if (myTAZShape.size() > 0) {
        Boundary b = myTAZShape.getBoxBoundary();
        b.grow(20);
        return b;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNETAZ::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNETAZ::moveGeometry(const Position& offset) {
    // restore old position, apply offset and update Geometry
    myTAZShape[0] = myMove.originalViewPosition;
    myTAZShape[0].add(offset);
    // filtern position using snap to active grid
    myTAZShape[0] = myViewNet->snapToActiveGrid(myTAZShape[0]);
}


void
GNETAZ::commitGeometryMoving(GNEUndoList* undoList) {
    // commit new position allowing undo/redo
    undoList->p_begin("position of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_SHAPE, toString(myTAZShape[0]), true, toString(myMove.originalViewPosition)));
    undoList->p_end();
}


void
GNETAZ::startTAZGeometryMoving(const double shapeOffset) {
    // save current centering boundary
    myMove.movingGeometryBoundary = getCenteringBoundary();
    // start move shape depending of block shape
    if (myBlockShape) {
        startMoveShape(myTAZShape, -1, myHintSize);
    } else {
        startMoveShape(myTAZShape, shapeOffset, myHintSize);
    }
}


void
GNETAZ::endTAZGeometryMoving() {
    // check that endGeometryMoving was called only once
    if (myMove.movingGeometryBoundary.isInitialised()) {
        // Remove object from net
        myViewNet->getNet()->removeGLObjectFromGrid(this);
        // reset myMovingGeometryBoundary
        myMove.movingGeometryBoundary.reset();
        // add object into grid again (using the new centering boundary)
        myViewNet->getNet()->addGLObjectIntoGrid(this);
    }
}


int
GNETAZ::getTAZVertexIndex(Position pos, const bool snapToGrid) const {
    // check if position has to be snapped to grid
    if (snapToGrid) {
        pos = myViewNet->snapToActiveGrid(pos);
    }
    const double offset = myTAZShape.nearest_offset_to_point2D(pos, true);
    if (offset == GeomHelper::INVALID_OFFSET) {
        return -1;
    }
    Position newPos = myTAZShape.positionAtOffset2D(offset);
    // first check if vertex already exists in the inner geometry
    for (int i = 0; i < (int)myTAZShape.size(); i++) {
        if (myTAZShape[i].distanceTo2D(newPos) < myHintSize) {
            // index refers to inner geometry
            if (i == 0 || i == (int)(myTAZShape.size() - 1)) {
                return -1;
            }
            return i;
        }
    }
    return -1;
}


void
GNETAZ::moveTAZShape(const Position& offset) {
    // first obtain a copy of shapeBeforeMoving
    PositionVector newShape = getShapeBeforeMoving();
    if (moveEntireShape()) {
        // move entire shape
        newShape.add(offset);
    } else {
        int geometryPointIndex = getGeometryPointIndex();
        // if geometryPoint is -1, then we have to create a new geometry point
        if (geometryPointIndex == -1) {
            geometryPointIndex = newShape.insertAtClosest(getPosOverShapeBeforeMoving(), true);
        }
        // get last index
        const int lastIndex = (int)newShape.size() - 1;
        // check if we have to move first and last postion
        if ((newShape.size() > 2) &&
            ((geometryPointIndex == 0) || (geometryPointIndex == lastIndex))) {
            // move first and last position in newShape
            newShape[0].add(offset);
            newShape[lastIndex].add(offset);
        } else {
            // move geometry point within newShape
            newShape[geometryPointIndex].add(offset);
        }
    }
    // set new TAZ shape
    myTAZShape = newShape;
}


void
GNETAZ::commitTAZShapeChange(GNEUndoList* undoList) {
    // restore original shape into shapeToCommit
    PositionVector shapeToCommit = myTAZShape;
    // get geometryPoint radius
    const double geometryPointRadius = myHintSize * myViewNet->getVisualisationSettings().junctionSize.exaggeration;
    // remove double points
    shapeToCommit.removeDoublePoints(geometryPointRadius);
    // check if we have to merge start and end points
    if ((shapeToCommit.front() != shapeToCommit.back()) && (shapeToCommit.front().distanceTo2D(shapeToCommit.back()) < geometryPointRadius)) {
        shapeToCommit[0] = shapeToCommit.back();
    }
    // update geometry
    updateGeometry();
    // restore old geometry to allow change attribute (And restore shape if during movement a new point was created
    myTAZShape = getShapeBeforeMoving();
    // finish geometry moving
    endTAZGeometryMoving();
    // commit new shape
    undoList->p_begin("moving " + toString(SUMO_ATTR_SHAPE) + " of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_SHAPE, toString(shapeToCommit)));
    undoList->p_end();
}


int
GNETAZ::getVertexIndex(Position pos, bool snapToGrid) {
    // check if position has to be snapped to grid
    if (snapToGrid) {
        pos = myViewNet->snapToActiveGrid(pos);
    }
    // first check if vertex already exists
    for (auto i : myTAZShape) {
        if (i.distanceTo2D(pos) < myHintSize) {
            return myTAZShape.indexOfClosest(i);
        }
    }
    return -1;
}


void
GNETAZ::deleteGeometryPoint(const Position& pos, bool allowUndo) {
    if (myTAZShape.size() > 2) {
        // obtain index
        PositionVector modifiedShape = myTAZShape;
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
            myTAZShape = modifiedShape;
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
    // obtain Exaggeration
    const double TAZExaggeration = s.polySize.getExaggeration(s, this);
    const Boundary TAZBoundary = myTAZShape.getBoxBoundary();
    // check if TAZ can be drawn
    if ((TAZExaggeration > 0) && myViewNet->getDataViewOptions().showAdditionals() && 
        (s.scale * MAX2(TAZBoundary.getWidth(), TAZBoundary.getHeight())) >= s.polySize.minSize) {
        // push name
        glPushName(getGlID());
        // check TAZ ssize
        if (myTAZShape.size() > 1) {
            // push line matrix
            glPushMatrix();
            // translate to 0 (GLO_NETWORK)
            glTranslated(0, 0, GLO_NETWORK);
            // set color
            if (drawUsingSelectColor()) {
                GLHelper::setColor(s.colorSettings.selectionColor);
            } else {
                GLHelper::setColor(myColor);
            }
            // recall tesselation
            performTesselation(s, 1);
            // pop tesselation matrix
            glPopMatrix();
            // draw name
            drawName(myTAZShape.getPolygonCenter(), s.scale, s.polyName, s.angle);
        }
        // draw geometry details hints if is not too small and isn't in selecting mode
        if (s.scale * myHintSize > 1.) {
            // set values relative to mouse position regarding to shape
            bool mouseOverVertex = false;
            bool modeMove = myViewNet->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE;
            Position mousePosition = myViewNet->getPositionInformation();
            double distanceToShape = myTAZShape.distance2D(mousePosition);
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
                glTranslated(0, 0, GLO_TAZ);
                GLHelper::setColor(darkerColor);
                GLHelper::drawBoxLines(myTAZShape, (myHintSize / 4) * s.polySize.getExaggeration(s, this));
                glPopMatrix();
                // draw shape points only in Network supemode
                if (!myViewNet->getEditModes().isCurrentSupermodeDemand()) {
                    for (const auto& TAZVertex : myTAZShape) {
                        if (!s.drawForRectangleSelection || (myViewNet->getPositionInformation().distanceSquaredTo2D(TAZVertex) <= (myHintSizeSquared + 2))) {
                            glPushMatrix();
                            glTranslated(TAZVertex.x(), TAZVertex.y(), GLO_TAZ + 0.01);
                            // Change color of vertex and flag mouseOverVertex if mouse is over vertex
                            if (modeMove && (TAZVertex.distanceTo2D(mousePosition) < myHintSize)) {
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
                        Position hintPos = myTAZShape.size() > 1 ? myTAZShape.positionAtOffset2D(myTAZShape.nearest_offset_to_point2D(mousePosition)) : myTAZShape[0];
                        glTranslated(hintPos.x(), hintPos.y(), GLO_TAZ + 0.02);
                        GLHelper::setColor(invertedColor);
                        GLHelper:: drawFilledCircle(myHintSize, s.getCircleResolution());
                        glPopMatrix();
                    }
                }
            }
        }
        // check if dotted contour has to be drawn
        if ((myViewNet->getDottedAC() == this) || (myViewNet->getViewParent()->getTAZFrame()->getTAZCurrentModul()->getTAZ() == this)) {
            GNEGeometry::drawShapeDottedContour(s, GLO_TAZ + 1, TAZExaggeration, myDottedGeometry);
        }
        // pop name
        glPopName();
    }
}


std::string
GNETAZ::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_SHAPE:
            return toString(myTAZShape);
        case SUMO_ATTR_COLOR:
            return toString(myColor);
        case SUMO_ATTR_FILL:
            return toString(myDrawFill);
        case SUMO_ATTR_EDGES: {
            std::vector<std::string> edgeIDs;
            for (auto i : getChildAdditionals()) {
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
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
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


double
GNETAZ::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case GNE_ATTR_MIN_SOURCE:
            return myMinWeightSource;
        case GNE_ATTR_MIN_SINK:
            return myMinWeightSink;
        case GNE_ATTR_MAX_SOURCE:
            return myMaxWeightSource;
        case GNE_ATTR_MAX_SINK:
            return myMaxWeightSink;
        case GNE_ATTR_AVERAGE_SOURCE:
            return myAverageWeightSource;
        case GNE_ATTR_AVERAGE_SINK:
            return myAverageWeightSink;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
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
        case SUMO_ATTR_FILL:
        case SUMO_ATTR_EDGES:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_BLOCK_SHAPE:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
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
        case SUMO_ATTR_FILL:
            return canParse<bool>(value);
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
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNETAZ::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
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
GNETAZ::updateParentAdditional() {
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
    // iterate over child additional
    for (const auto &additional : getChildAdditionals()) {
        if (additional->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
            double weight = additional->getAttributeDouble(SUMO_ATTR_WEIGHT);
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
        } else if (additional->getTagProperty().getTag() == SUMO_TAG_TAZSINK) {
            double weight = additional->getAttributeDouble(SUMO_ATTR_WEIGHT);
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
GNETAZ::performTesselation(const GUIVisualizationSettings& s, double /*lineWidth*/) const {
    const bool moveMode = (myViewNet->getEditModes().networkEditMode != NetworkEditMode::NETWORK_MOVE);
    if (myDrawFill && (!s.drawForPositionSelection || moveMode || myBlockShape)) {
        // draw the tesselated shape
        double* points = new double[myTAZShape.size() * 3];
        GLUtesselator* tobj = gluNewTess();
        gluTessCallback(tobj, GLU_TESS_VERTEX, (GLvoid(APIENTRY*)()) &glVertex3dv);
        gluTessCallback(tobj, GLU_TESS_BEGIN, (GLvoid(APIENTRY*)()) &beginTAZCallback);
        gluTessCallback(tobj, GLU_TESS_END, (GLvoid(APIENTRY*)()) &endTAZCallback);
        //gluTessCallback(tobj, GLU_TESS_ERROR, (GLvoid (APIENTRY*) ()) &errorCallback);
        gluTessCallback(tobj, GLU_TESS_COMBINE, (GLvoid(APIENTRY*)()) &combineTAZCallback);
        gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
        gluTessBeginPolygon(tobj, nullptr);
        gluTessBeginContour(tobj);
        for (int i = 0; i != (int)myTAZShape.size(); ++i) {
            points[3 * i]  = myTAZShape[(int) i].x();
            points[3 * i + 1]  = myTAZShape[(int) i].y();
            points[3 * i + 2]  = 0;
            gluTessVertex(tobj, points + 3 * i, points + 3 * i);
        }
        gluTessEndContour(tobj);

        gluTessEndPolygon(tobj);
        gluDeleteTess(tobj);
        delete[] points;
    } else {
        // draw line around TAZ
        GLHelper::drawBoxLines(myTAZShape, 1);
    }

}


void
GNETAZ::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myViewNet->getNet()->getAttributeCarriers()->updateID(this, value);
            break;
        case SUMO_ATTR_SHAPE:
            myViewNet->getNet()->removeGLObjectFromGrid(this);
            myTAZShape = parse<PositionVector>(value);
            myViewNet->getNet()->addGLObjectIntoGrid(this);
            break;
        case SUMO_ATTR_COLOR:
            myColor = parse<RGBColor>(value);
            break;
        case SUMO_ATTR_FILL:
            myDrawFill = parse<bool>(value);
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
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/
