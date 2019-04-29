/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEEdge.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A road/street connecting two junctions (netedit-version, adapted from GUIEdge)
// Basically a container for an NBEdge with drawing and editing capabilities
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/StringTokenizer.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GLHelper.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Lane.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/additionals/GNERouteProbe.h>
#include <netedit/additionals/GNEDetectorE2.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/demandelements/GNERoute.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEConnection.h"
#include "GNECrossing.h"
#include "GNEJunction.h"
#include "GNELane.h"
#include "GNEEdge.h"

//#define DEBUG_SMOOTH_GEOM
//#define DEBUGCOND(obj) (true)

// ===========================================================================
// static
// ===========================================================================
const double GNEEdge::SNAP_RADIUS = SUMO_const_halfLaneWidth;

GNEEdge& GNEEdge::getDummyEdge() {
    // @note: using local static idiom to avoid static initialization order problem
    static GNEEdge* dummy = new GNEEdge(); // 'static local variable', this line is called only once
    return *dummy; // this line gets called with the same 'dummy' every time the function is called
}

// ===========================================================================
// members methods
// ===========================================================================

GNEEdge::GNEEdge(NBEdge& nbe, GNENet* net, bool wasSplit, bool loaded):
    GNENetElement(net, nbe.getID(), GLO_EDGE, SUMO_TAG_EDGE),
    myNBEdge(nbe),
    myGNEJunctionSource(myNet->retrieveJunction(myNBEdge.getFromNode()->getID())),
    myGNEJunctionDestiny(myNet->retrieveJunction(myNBEdge.getToNode()->getID())),
    myLanes(0),
    myAmResponsible(false),
    myWasSplit(wasSplit),
    myConnectionStatus(loaded ? FEATURE_LOADED : FEATURE_GUESSED) {
    // Create lanes
    int numLanes = myNBEdge.getNumLanes();
    myLanes.reserve(numLanes);
    for (int i = 0; i < numLanes; i++) {
        myLanes.push_back(new GNELane(*this, i));
        myLanes.back()->incRef("GNEEdge::GNEEdge");
    }
    // update Lane geometries
    for (auto i : myLanes) {
        i->updateGeometry();
    }
}

GNEEdge::GNEEdge() :
    GNENetElement(nullptr, "DUMMY", GLO_EDGE, SUMO_TAG_NOTHING),
    myNBEdge(NBEdge::DummyEdge) {
}

GNEEdge::~GNEEdge() {
    // Delete references to this eddge in lanes
    for (auto i : myLanes) {
        i->decRef("GNEEdge::~GNEEdge");
        if (i->unreferenced()) {
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + i->getTagStr() + " '" + i->getID() + "' in GNEEdge destructor");
            delete i;
        }
    }
    // delete references to this eddge in connections
    for (auto i : myGNEConnections) {
        i->decRef("GNEEdge::~GNEEdge");
        if (i->unreferenced()) {
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + i->getTagStr() + " '" + i->getID() + "' in GNEEdge destructor");
            delete i;
        }
    }
    if (myAmResponsible) {
        delete &myNBEdge;
    }
}


std::string
GNEEdge::generateChildID(SumoXMLTag /*childTag*/) {
    // currently unused
    return "";
}


void
GNEEdge::updateGeometry() {
    // first check if object has to be removed from grid (SUMOTree)
    if (!myMovingGeometryBoundary.isInitialised()) {
        myNet->removeGLObjectFromGrid(this);
    }
    // Update geometry of lanes
    for (auto i : myLanes) {
        i->updateGeometry();
    }
    // Update geometry of connections (Only if updateGrid is enabled, because in move mode connections are hidden
    // (note: only the previous marked as deprecated will be updated)
    if (!myMovingGeometryBoundary.isInitialised()) {
        for (auto i : myGNEConnections) {
            i->updateGeometry();
        }
    }
    // Update geometry of additionals childs vinculated to this edge
    for (auto i : getAdditionalChilds()) {
        i->updateGeometry();
    }
    // Update geometry of additional parents that have this edge as parent
    for (auto i : getAdditionalParents()) {
        i->updateGeometry();
    }
    // Update geometry of demand elements childs vinculated to this edge
    for (auto i : getDemandElementChilds()) {
        i->updateGeometry();
    }
    // Update geometry of demand elements parents that have this edge as parent
    for (auto i : getDemandElementParents()) {
        i->updateGeometry();
    }
    // last step is to check if object has to be added into grid (SUMOTree) again
    if (!myMovingGeometryBoundary.isInitialised()) {
        myNet->addGLObjectIntoGrid(this);
    }
}


Position
GNEEdge::getPositionInView() const {
    // currently unused
    return Position(0, 0);
}


bool
GNEEdge::clickedOverShapeStart(const Position& pos) {
    if (myNBEdge.getGeometry().front() != myGNEJunctionSource->getPositionInView()) {
        return (myNBEdge.getGeometry().front().distanceTo2D(pos) < SNAP_RADIUS);
    } else {
        return false;
    }
}


bool
GNEEdge::clickedOverShapeEnd(const Position& pos) {
    if (myNBEdge.getGeometry().back() != myGNEJunctionDestiny->getPositionInView()) {
        return (myNBEdge.getGeometry().back().distanceTo2D(pos) < SNAP_RADIUS);
    } else {
        return false;
    }
}


void
GNEEdge::moveShapeStart(const Position& oldPos, const Position& offset) {
    // change shape startPosition using oldPosition and offset
    Position shapeStartEdited = oldPos;
    shapeStartEdited.add(offset);
    // snap to active grid
    shapeStartEdited = myNet->getViewNet()->snapToActiveGrid(shapeStartEdited);
    // make sure that start and end position are different
    if (shapeStartEdited != myNBEdge.getGeometry().back()) {
        // set shape start position without updating grid
        setShapeStartPos(shapeStartEdited, false);
        updateGeometry();
    }
}


void
GNEEdge::moveShapeEnd(const Position& oldPos, const Position& offset) {
    // change shape endPosition using oldPosition and offset
    Position shapeEndEdited = oldPos;
    shapeEndEdited.add(offset);
    // snap to active grid
    shapeEndEdited = myNet->getViewNet()->snapToActiveGrid(shapeEndEdited);
    // make sure that start and end position are different
    if (shapeEndEdited != myNBEdge.getGeometry().front()) {
        // set shape end position without updating grid
        setShapeEndPos(shapeEndEdited, false);
        updateGeometry();
    }
}


void
GNEEdge::commitShapeStartChange(const Position& oldPos, GNEUndoList* undoList) {
    // first save current shape start position
    Position modifiedShapeStartPos = myNBEdge.getGeometry().front();
    // restore old shape start position
    setShapeStartPos(oldPos, true);
    // set attribute using undolist
    undoList->p_begin("shape start of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, myNet, GNE_ATTR_SHAPE_START, toString(modifiedShapeStartPos), true, toString(oldPos)));
    undoList->p_end();
}


void
GNEEdge::commitShapeEndChange(const Position& oldPos, GNEUndoList* undoList) {
    // first save current shape end position
    Position modifiedShapeEndPos = myNBEdge.getGeometry().back();
    // restore old shape end position
    setShapeEndPos(oldPos, true);
    // set attribute using undolist
    undoList->p_begin("shape end of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, myNet, GNE_ATTR_SHAPE_END, toString(modifiedShapeEndPos), true, toString(oldPos)));
    undoList->p_end();
}


void
GNEEdge::startGeometryMoving() {
    // save current centering boundary
    myMovingGeometryBoundary = getCenteringBoundary();
    // Save current centering boundary of lanes (and their childs)
    for (auto i : myLanes) {
        i->startGeometryMoving();
    }
    // Save current centering boundary of additionals childs vinculated to this edge
    for (auto i : getAdditionalChilds()) {
        i->startGeometryMoving();
    }
    // Save current centering boundary of additional parents that have this edge as parent
    for (auto i : getAdditionalParents()) {
        i->startGeometryMoving();
    }
    // Save current centering boundary of demand elements childs vinculated to this edge
    for (auto i : getDemandElementChilds()) {
        i->startGeometryMoving();
    }
    // Save current centering boundary of demand elements parents that have this edge as parent
    for (auto i : getDemandElementParents()) {
        i->startGeometryMoving();
    }
}


void
GNEEdge::endGeometryMoving() {
    // check that endGeometryMoving was called only once
    if (myMovingGeometryBoundary.isInitialised()) {
        // Remove object from net
        myNet->removeGLObjectFromGrid(this);
        // reset myMovingGeometryBoundary
        myMovingGeometryBoundary.reset();
        // update geometry without updating grid
        updateGeometry();
        // Restore centering boundary of lanes (and their childs)
        for (auto i : myLanes) {
            i->endGeometryMoving();
        }
        // Restore centering boundary of additionals childs vinculated to this edge
        for (auto i : getAdditionalChilds()) {
            i->endGeometryMoving();
        }
        // Restore centering boundary of additional parents that have this edge as parent
        for (auto i : getAdditionalParents()) {
            i->endGeometryMoving();
        }
        // Restore centering boundary of demand elements childs vinculated to this edge
        for (auto i : getDemandElementChilds()) {
            i->endGeometryMoving();
        }
        // Restore centering boundary of demand elements parents that have this edge as parent
        for (auto i : getDemandElementParents()) {
            i->endGeometryMoving();
        }
        // add object into grid again (using the new centering boundary)
        myNet->addGLObjectIntoGrid(this);
    }
}


int
GNEEdge::getVertexIndex(Position pos, bool createIfNoExist, bool snapToGrid) {
    PositionVector entireGeometry = myNBEdge.getGeometry();
    // check if position has to be snapped to grid
    if (snapToGrid) {
        pos = myNet->getViewNet()->snapToActiveGrid(pos);
    }
    double offset = entireGeometry.nearest_offset_to_point2D(pos, true);
    if (offset == GeomHelper::INVALID_OFFSET) {
        return -1;
    }
    Position newPos = entireGeometry.positionAtOffset2D(offset);
    // first check if vertex already exists in the inner geometry
    for (int i = 0; i < (int)entireGeometry.size(); i++) {
        if (entireGeometry[i].distanceTo2D(newPos) < SNAP_RADIUS) {
            if (i == 0 || i == (int)(entireGeometry.size() - 1)) {
                return -1;
            }
            // index refers to inner geometry
            return i - 1;
        }
    }
    // if vertex doesn't exist, insert it
    if (createIfNoExist) {
        // check if position has to be snapped to grid
        if (snapToGrid) {
            newPos = myNet->getViewNet()->snapToActiveGrid(newPos);
        }
        int index = entireGeometry.insertAtClosest(myNet->getViewNet()->snapToActiveGrid(newPos));
        setGeometry(entireGeometry, false, true);
        // index refers to inner geometry
        return (index - 1);
    } else {
        return -1;
    }
}


int
GNEEdge::getVertexIndex(const double offset, bool createIfNoExist, bool snapToGrid) {
    return getVertexIndex(myNBEdge.getGeometry().positionAtOffset2D(offset), createIfNoExist, snapToGrid);
}


int
GNEEdge::moveVertexShape(const int index, const Position& oldPos, const Position& offset) {
    // obtain inner geometry of edge
    PositionVector edgeGeometry = myNBEdge.getInnerGeometry();
    // Make sure that index is valid AND ins't the first and last index
    if (index != -1) {
        // check that index is correct before change position
        if (index < (int)edgeGeometry.size()) {
            // change position of vertex
            edgeGeometry[index] = oldPos;
            edgeGeometry[index].add(offset);
            // filtern position using snap to active grid
            edgeGeometry[index] = myNet->getViewNet()->snapToActiveGrid(edgeGeometry[index]);
            // update edge's geometry without updating RTree (To avoid unnecesary changes in RTree)
            setGeometry(edgeGeometry, true, false);
            return index;
        } else {
            throw InvalidArgument("Index greater than shape size");
        }
    } else {
        return index;
    }
}


void
GNEEdge::moveEntireShape(const PositionVector& oldShape, const Position& offset) {
    // make a copy of the old shape to change it
    PositionVector modifiedShape = oldShape;
    // change all points of the inner geometry using offset
    for (auto& i : modifiedShape) {
        i.add(offset);
    }
    // restore modified shape
    setGeometry(modifiedShape, true, false);
}


void
GNEEdge::commitShapeChange(const PositionVector& oldShape, GNEUndoList* undoList) {
    // restore original shape into shapeToCommit
    PositionVector innerShapeToCommit = myNBEdge.getInnerGeometry();
    // first check if second and penultimate isn't in Junction's buubles
    double buubleRadius = GNEJunction::BUBBLE_RADIUS * myNet->getViewNet()->getVisualisationSettings()->junctionSize.exaggeration;
    if (myNBEdge.getGeometry().size() > 2 && myNBEdge.getGeometry()[0].distanceTo(myNBEdge.getGeometry()[1]) < buubleRadius) {
        innerShapeToCommit.removeClosest(innerShapeToCommit[0]);
    }
    if (myNBEdge.getGeometry().size() > 2 && myNBEdge.getGeometry()[(int)myNBEdge.getGeometry().size() - 2].distanceTo(myNBEdge.getGeometry()[(int)myNBEdge.getGeometry().size() - 1]) < buubleRadius) {
        innerShapeToCommit.removeClosest(innerShapeToCommit[(int)innerShapeToCommit.size() - 1]);
    }
    // second check if double points has to be removed
    innerShapeToCommit.removeDoublePoints(SNAP_RADIUS);
    // show warning if some of edge's shape was merged
    if (innerShapeToCommit.size() != myNBEdge.getInnerGeometry().size()) {
        WRITE_WARNING("Merged shape's point")
    }
    // finish geometry moving
    endGeometryMoving();
    updateGeometry();
    // restore old geometry to allow change attribute (And restore shape if during movement a new point was created
    setGeometry(oldShape, true, true);
    // commit new shape
    undoList->p_begin("moving " + toString(SUMO_ATTR_SHAPE) + " of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, myNet, SUMO_ATTR_SHAPE, toString(innerShapeToCommit)));
    undoList->p_end();
}


void
GNEEdge::deleteGeometryPoint(const Position& pos, bool allowUndo) {
    // obtain index and remove point
    PositionVector modifiedShape = myNBEdge.getInnerGeometry();
    int index = modifiedShape.indexOfClosest(pos);
    modifiedShape.erase(modifiedShape.begin() + index);
    // set new shape depending of allowUndo
    if (allowUndo) {
        myNet->getViewNet()->getUndoList()->p_begin("delete geometry point");
        setAttribute(SUMO_ATTR_SHAPE, toString(modifiedShape), myNet->getViewNet()->getUndoList());
        myNet->getViewNet()->getUndoList()->p_end();
    } else {
        // set new shape
        setGeometry(modifiedShape, true, true);
    }
}


void
GNEEdge::updateJunctionPosition(GNEJunction* junction, const Position& origPos, bool updateGrid) {
    Position delta = junction->getNBNode()->getPosition() - origPos;
    PositionVector geom = myNBEdge.getGeometry();
    // geometry endpoint need not equal junction position hence we modify it with delta
    if (junction == myGNEJunctionSource) {
        geom[0].add(delta);
    } else {
        geom[-1].add(delta);
    }
    setGeometry(geom, false, updateGrid);
}


Boundary
GNEEdge::getBoundary() const {
    Boundary ret;
    for (auto i : myLanes) {
        ret.add(i->getBoundary());
    }
    // ensure that geometry points are selectable even if the lane geometry is strange
    for (const Position& pos : myNBEdge.getGeometry()) {
        ret.add(pos);
    }
    ret.grow(10); // !!! magic value
    return ret;
}


Boundary
GNEEdge::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (myMovingGeometryBoundary.isInitialised()) {
        return myMovingGeometryBoundary;
    }  else {
        Boundary b = getBoundary();
        b.grow(20);
        return b;
    }
}

const std::string
GNEEdge::getOptionalName() const {
    return myNBEdge.getStreetName();
}

GUIGLObjectPopupMenu*
GNEEdge::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // build position copy entry
    buildPositionCopyEntry(ret, false);
    return ret;
}


GNEJunction*
GNEEdge::getGNEJunctionSource() const {
    return myGNEJunctionSource;
}


GNEJunction*
GNEEdge::getGNEJunctionDestiny() const {
    return myGNEJunctionDestiny;
}


GNEEdge*
GNEEdge::getOppositeEdge() const {
    return myNet->retrieveEdge(myGNEJunctionDestiny, myGNEJunctionSource, false);
}


void
GNEEdge::drawGL(const GUIVisualizationSettings& s) const {
    // check if boundary has to be drawn
    if(s.drawBoundaries) {
        GLHelper::drawBoundary(getBoundary());
    }
    // obtain circle width
    double circleWidth = SNAP_RADIUS * MIN2((double)1, s.laneWidthExaggeration);
    double circleWidthSquared = circleWidth * circleWidth;
    int circleResolution = GNEAttributeCarrier::getCircleResolution(s);
    // draw lanes
    for (auto i : myLanes) {
        i->drawGL(s);
    }
    // draw parents
    for (const auto &i : getAdditionalParents()) {
        if (i->getTagProperty().getTag() == SUMO_TAG_REROUTER) {
            // Draw symbols in every lane
            const double rerouterExaggeration = s.addSize.getExaggeration(s, i);
            if (s.scale * rerouterExaggeration >= 3) {
                // Start drawing adding an gl identificator
                glPushName(i->getGlID());
                // draw rerouter symbol over all lanes
                for (const auto &j : myLanes) {
                    const Position &lanePos = i->getChildPosition(j);
                    const double laneRot = i->getChildRotation(j);
                    // draw rerouter symbol
                    glPushMatrix();
                    glTranslated(lanePos.x(), lanePos.y(), i->getType());
                    glRotated(-1 * laneRot, 0, 0, 1);
                    glScaled(rerouterExaggeration, rerouterExaggeration, 1);
                    // mode
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glBegin(GL_TRIANGLES);
                    glColor3d(1, .8f, 0);
                    // base
                    glVertex2d(0 - 1.4, 0);
                    glVertex2d(0 - 1.4, 6);
                    glVertex2d(0 + 1.4, 6);
                    glVertex2d(0 + 1.4, 0);
                    glVertex2d(0 - 1.4, 0);
                    glVertex2d(0 + 1.4, 6);
                    glEnd();
                    // draw "U"
                    GLHelper::drawText("U", Position(0, 2), .1, 3, RGBColor::BLACK, 180);
                    double probability = parse<double>(i->getAttribute(SUMO_ATTR_PROB))*100;
                    // draw Probability
                    GLHelper::drawText((toString(probability) + "%").c_str(), Position(0, 4), .1, 0.7, RGBColor::BLACK, 180);
                    // finish draw
                    glPopMatrix();
                    // draw contour if is selected
                    if (!s.drawForSelecting && (myNet->getViewNet()->getDottedAC() == i)) {
                        GLHelper::drawShapeDottedContour(getType(), lanePos, 2.8, 6, -1 * laneRot, 0, 3);
                    }
                }
            }
            // Pop name
            glPopName();
            // Draw connections
            i->drawChildConnections(getType());
        }
    }
    // draw childs
    for (const auto &i : getAdditionalChilds()) {
        i->drawGL(s);
    }
    if (myNet->getViewNet()->getViewOptions().showDemandElements()) {
        for (const auto &i : getDemandElementChilds()) {
            if (i->getTagProperty().getTag() == SUMO_TAG_ROUTE) {
                // calculate route width
                double routeWidth = s.addSize.getExaggeration(s, this) * 0.66;

                // Start drawing adding an gl identificator
                glPushName(i->getGlID());

                // Add a draw matrix
                glPushMatrix();

                // Start with the drawing of the area traslating matrix to origin
                glTranslated(0, 0, i->getType());

                // Set color of the base
                if (drawUsingSelectColor()) {
                    GLHelper::setColor(s.selectedAdditionalColor);
                } else {
                    GLHelper::setColor(i->getColor());
                }

                // draw route
                GLHelper::drawBoxLines(myLanes.front()->getGeometry().shape, myLanes.front()->getGeometry().shapeRotations, myLanes.front()->getGeometry().shapeLengths, routeWidth);

                // check if route has a connectio between this and the next edge
                GNEConnection *nextConnection = i->getNextConnection(this);

                if (nextConnection && (nextConnection->getEdgeFrom()->getGNEJunctionDestiny()->getNBNode()->getShape().size() > 0)) {
                    GLHelper::drawBoxLines(nextConnection->getGeometry().shape, nextConnection->getGeometry().shapeRotations, nextConnection->getGeometry().shapeLengths, routeWidth);
                } else {
                    // calculate line between this and the next edge
                    GNEHierarchicalElementParents::LineGeometry lineGeometry = i->getLinetoNextEdge(this);
                    GLHelper::drawBoxLine(lineGeometry.firstPoint, lineGeometry.rotation, lineGeometry.lenght, routeWidth);
                }

                // Pop last matrix
                glPopMatrix();

                // Draw name if isn't being drawn for selecting
                if (!s.drawForSelecting) {
                    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
                }
            
                // check if dotted contour has to be drawn
                if (!s.drawForSelecting && (myNet->getViewNet()->getDottedAC() == i)) {
                    GLHelper::drawShapeDottedContour(getType(), i->myGeometry.shape, routeWidth);
                }
            
                // Pop name
                glPopName();

                // draw route childs
                for (const auto &j : i->getDemandElementChilds()) {
                    j->drawGL(s);
                }
            }
        }
    }
    // draw geometry points if isnt's too small
    if (s.scale > 8.0) {
        RGBColor color = s.junctionColorer.getSchemes()[0].getColor(2);
        if (drawUsingSelectColor() && s.laneColorer.getActive() != 1) {
            // override with special colors (unless the color scheme is based on selection)
            color = s.selectedEdgeColor.changedBrightness(-20);
        }
        GLHelper::setColor(color);
        // recognize full transparency and simply don't draw
        if (color.alpha() > 0) {
            // push name
            glPushName(getGlID());
            // draw geometry points expect initial and final
            for (int i = 1; i < (int)myNBEdge.getGeometry().size() - 1; i++) {
                Position pos = myNBEdge.getGeometry()[i];
                if (!s.drawForSelecting || (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(pos) <= (circleWidthSquared + 2))) {
                    glPushMatrix();
                    glTranslated(pos.x(), pos.y(), GLO_JUNCTION - 0.01);
                    // resolution of drawn circle depending of the zoom (To improve smothness)
                    GLHelper::drawFilledCircle(circleWidth, circleResolution);
                    glPopMatrix();
                    // draw elevation or special symbols (Start, End and Block)
                    if (!s.drawForSelecting && myNet->getViewNet()->getMoveOptions().editingElevation()) {
                        glPushMatrix();
                        // Translate to geometry point
                        glTranslated(pos.x(), pos.y(), GLO_JUNCTION);
                        // draw Z value
                        GLHelper::drawText(toString(pos.z()), Position(), GLO_MAX - 5, s.edgeValue.scaledSize(s.scale) / 2, s.edgeValue.color);
                        glPopMatrix();
                    }
                }
            }
            // draw line geometry, start and end points if shapeStart or shape end is edited, and depending of drawForSelecting
            if (myNet->getViewNet()->getEditModes().networkEditMode == GNE_NMODE_MOVE) {
                if ((myNBEdge.getGeometry().front() != myGNEJunctionSource->getPositionInView()) &&
                        (!s.drawForSelecting || (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(myNBEdge.getGeometry().front()) <= (circleWidthSquared + 2)))) {
                    glPushMatrix();
                    glTranslated(myNBEdge.getGeometry().front().x(), myNBEdge.getGeometry().front().y(), GLO_JUNCTION + 0.01);
                    // resolution of drawn circle depending of the zoom (To improve smothness)
                    GLHelper::drawFilledCircle(circleWidth, circleResolution);
                    glPopMatrix();
                    // draw a "s" over last point depending of drawForSelecting
                    if (!s.drawForSelecting) {
                        glPushMatrix();
                        glTranslated(myNBEdge.getGeometry().front().x(), myNBEdge.getGeometry().front().y(), GLO_JUNCTION + 0.02);
                        GLHelper::drawText("S", Position(), 0, circleWidth, RGBColor::WHITE);
                        glPopMatrix();
                        // draw line between Junction and point
                        glPushMatrix();
                        glTranslated(0, 0, GLO_JUNCTION - 0.01);
                        glLineWidth(4);
                        GLHelper::drawLine(myNBEdge.getGeometry().front(), myGNEJunctionSource->getPositionInView());
                        // draw line between begin point of last lane shape and the first edge shape point
                        GLHelper::drawLine(myNBEdge.getGeometry().front(), myNBEdge.getLanes().back().shape.front());
                        glPopMatrix();
                    }
                }
                if ((myNBEdge.getGeometry().back() != myGNEJunctionDestiny->getPositionInView()) &&
                        (!s.drawForSelecting || (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(myNBEdge.getGeometry().back()) <= (circleWidthSquared + 2)))) {
                    glPushMatrix();
                    glTranslated(myNBEdge.getGeometry().back().x(), myNBEdge.getGeometry().back().y(), GLO_JUNCTION + 0.01);
                    // resolution of drawn circle depending of the zoom (To improve smothness)
                    GLHelper::drawFilledCircle(circleWidth, circleResolution);
                    glPopMatrix();
                    // draw a "e" over last point depending of drawForSelecting
                    if (!s.drawForSelecting) {
                        glPushMatrix();
                        glTranslated(myNBEdge.getGeometry().back().x(), myNBEdge.getGeometry().back().y(), GLO_JUNCTION + 0.02);
                        GLHelper::drawText("E", Position(), 0, circleWidth, RGBColor::WHITE);
                        glPopMatrix();
                        // draw line between Junction and point
                        glPushMatrix();
                        glTranslated(0, 0, GLO_JUNCTION - 0.01);
                        glLineWidth(4);
                        GLHelper::drawLine(myNBEdge.getGeometry().back(), myGNEJunctionDestiny->getPositionInView());
                        // draw line between last point of first lane shape and the last edge shape point
                        GLHelper::drawLine(myNBEdge.getGeometry().back(), myNBEdge.getLanes().back().shape.back());
                        glPopMatrix();
                    }
                }
            }
            // pop name
            glPopName();
        }
    }

    // (optionally) draw the name and/or the street name if isn't being drawn for selecting
    const bool drawStreetName = s.streetName.show && (myNBEdge.getStreetName() != "");
    const bool spreadSuperposed = s.spreadSuperposed && myLanes.back()->drawAsRailway(s) && myNBEdge.isBidiRail();
    if (!s.drawForSelecting && (s.edgeName.show || drawStreetName || s.edgeValue.show)) {
        glPushName(getGlID());
        GNELane* lane1 = myLanes[0];
        GNELane* lane2 = myLanes[myLanes.size() - 1];
        Position p = lane1->getGeometry().shape.positionAtOffset(lane1->getGeometry().shape.length() / (double) 2.);
        p.add(lane2->getGeometry().shape.positionAtOffset(lane2->getGeometry().shape.length() / (double) 2.));
        p.mul(.5);
        if (spreadSuperposed) {
            // move name to the right of the edge and towards its beginning
            const double dist = 0.6 * s.edgeName.scaledSize(s.scale);
            const double shiftA = lane1->getGeometry().shape.rotationAtOffset(lane1->getGeometry().shape.length() / (double) 2.) - DEG2RAD(135);
            Position shift(dist * cos(shiftA), dist * sin(shiftA));
            p.add(shift);
        }
        double angle = lane1->getGeometry().shape.rotationDegreeAtOffset(lane1->getGeometry().shape.length() / (double) 2.);
        angle += 90;
        if (angle > 90 && angle < 270) {
            angle -= 180;
        }
        if (s.edgeName.show) {
            drawName(p, s.scale, s.edgeName, angle);
        }
        if (drawStreetName) {
            GLHelper::drawTextSettings(s.streetName, myNBEdge.getStreetName(), p, s.scale, angle);
        }
        if (s.edgeValue.show) {
            double value = lane2->getColorValue(s, s.laneColorer.getActive());
            GLHelper::drawTextSettings(s.edgeValue, toString(value), p, s.scale, angle);
        }
        glPopName();
    }
    if (!s.drawForSelecting && (myNet->getViewNet()->getDottedAC() == this)) {
        // draw dotted contor around the first and last lane
        const double myHalfLaneWidthFront = myNBEdge.getLaneWidth(myLanes.front()->getIndex()) / 2;
        const double myHalfLaneWidthBack = spreadSuperposed ? 0 : myNBEdge.getLaneWidth(myLanes.back()->getIndex()) / 2;
        GLHelper::drawShapeDottedContour(GLO_JUNCTION, myLanes.front()->getGeometry().shape, myHalfLaneWidthFront, myLanes.back()->getGeometry().shape, -1 * myHalfLaneWidthBack);
    }
    glPopMatrix();
}


NBEdge*
GNEEdge::getNBEdge() {
    return &myNBEdge;
}


Position
GNEEdge::getSplitPos(const Position& clickPos) {
    const PositionVector& geom = myNBEdge.getGeometry();
    int index = geom.indexOfClosest(clickPos);
    if (geom[index].distanceTo(clickPos) < SNAP_RADIUS) {
        // split at existing geometry point
        return geom[index];
    } else {
        // split straight between the next two points
        return geom.positionAtOffset(geom.nearest_offset_to_point2D(clickPos));
    }
}


void
GNEEdge::editEndpoint(Position pos, GNEUndoList* undoList) {
    if ((myNBEdge.getGeometry().front() != myGNEJunctionSource->getPositionInView()) && (myNBEdge.getGeometry().front().distanceTo(pos) < SNAP_RADIUS)) {
        undoList->p_begin("remove endpoint");
        setAttribute(GNE_ATTR_SHAPE_START, "", undoList);
        undoList->p_end();
    } else if ((myNBEdge.getGeometry().back() != myGNEJunctionDestiny->getPositionInView()) && (myNBEdge.getGeometry().back().distanceTo(pos) < SNAP_RADIUS)) {
        undoList->p_begin("remove endpoint");
        setAttribute(GNE_ATTR_SHAPE_END, "", undoList);
        undoList->p_end();
    } else {
        // we need to create new Start/End position over Edge shape, not over clicked position
        double offset = myNBEdge.getGeometry().nearest_offset_to_point2D(myNet->getViewNet()->snapToActiveGrid(pos), true);
        if (offset != GeomHelper::INVALID_OFFSET) {
            PositionVector geom = myNBEdge.getGeometry();
            // calculate position over edge shape relative to clicked positino
            Position newPos = geom.positionAtOffset2D(offset);
            // snap new position to grid
            newPos = myNet->getViewNet()->snapToActiveGrid(newPos);
            undoList->p_begin("set endpoint");
            int index = geom.indexOfClosest(pos);
            // check if snap to existing geometry
            if (geom[index].distanceTo(pos) < SNAP_RADIUS) {
                pos = geom[index];
            }
            Position destPos = myGNEJunctionDestiny->getNBNode()->getPosition();
            Position sourcePos = myGNEJunctionSource->getNBNode()->getPosition();
            if (pos.distanceTo2D(destPos) < pos.distanceTo2D(sourcePos)) {
                setAttribute(GNE_ATTR_SHAPE_END, toString(newPos), undoList);
                myGNEJunctionDestiny->invalidateShape();
            } else {
                setAttribute(GNE_ATTR_SHAPE_START, toString(newPos), undoList);
                myGNEJunctionSource->invalidateShape();
            }
            // possibly existing inner point is no longer needed
            if (myNBEdge.getInnerGeometry().size() > 0 && getVertexIndex(pos, false, false) != -1) {
                deleteGeometryPoint(pos, false);
            }
            undoList->p_end();
        }
    }
}


void
GNEEdge::resetEndpoint(const Position& pos, GNEUndoList* undoList) {
    Position destPos = myGNEJunctionDestiny->getNBNode()->getPosition();
    Position sourcePos = myGNEJunctionSource->getNBNode()->getPosition();
    if (pos.distanceTo2D(destPos) < pos.distanceTo2D(sourcePos)) {
        setAttribute(GNE_ATTR_SHAPE_END, toString(destPos), undoList);
        myGNEJunctionDestiny->invalidateShape();
    } else {
        setAttribute(GNE_ATTR_SHAPE_START, toString(sourcePos), undoList);
        myGNEJunctionSource->invalidateShape();
    }
}


void
GNEEdge::setGeometry(PositionVector geom, bool inner, bool updateGrid) {
    if (updateGrid) {
        // first remove object from net grid
        myNet->removeGLObjectFromGrid(this);
    }
    // set new geometry
    myNBEdge.setGeometry(geom, inner);
    if (updateGrid) {
        // add object into net again
        myNet->addGLObjectIntoGrid(this);
    }
    updateGeometry();
    myGNEJunctionSource->invalidateShape();
    myGNEJunctionDestiny->invalidateShape();
}


void
GNEEdge::remakeGNEConnections() {
    // create new and removed unused GNEConnectinos
    const std::vector<NBEdge::Connection>& connections = myNBEdge.getConnections();
    // create a vector to keep retrieved and created connections
    std::vector<GNEConnection*> retrievedConnections;
    // iterate over NBEdge::Connections of GNEEdge
    for (auto it : connections) {
        // retrieve existent GNEConnection, or create it
        GNEConnection* retrievedGNEConnection = retrieveGNEConnection(it.fromLane, it.toEdge, it.toLane);
        retrievedGNEConnection->updateLinkState();
        retrievedConnections.push_back(retrievedGNEConnection);
        // check if previously this GNEConnections exists, and if true, remove it from myGNEConnections
        std::vector<GNEConnection*>::iterator retrievedExists = std::find(myGNEConnections.begin(), myGNEConnections.end(), retrievedGNEConnection);
        if (retrievedExists != myGNEConnections.end()) {
            myGNEConnections.erase(retrievedExists);
        } else {
            // include reference to created GNEConnection
            retrievedGNEConnection->incRef("GNEEdge::remakeGNEConnections");
        }
        // mark it as deprecated
        retrievedGNEConnection->markConnectionGeometryDeprecated();
    }
    // delete non retrieved GNEConnections
    for (auto it : myGNEConnections) {
        // decrease reference
        it->decRef();
        // delete GNEConnection if is unreferenced
        if (it->unreferenced()) {
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + it->getTagStr() + " '" + it->getID() + "' in rebuildGNEConnections()");
            delete it;
        }
    }
    // copy retrieved (existent and created) GNECrossigns to myGNEConnections
    myGNEConnections = retrievedConnections;
}


void
GNEEdge::clearGNEConnections() {
    // Drop all existents connections that aren't referenced anymore
    for (auto i : myGNEConnections) {
        // check if connection is selected
        if (i->isAttributeCarrierSelected()) {
            i->unselectAttributeCarrier();
        }
        // remove it from Tree
        myNet->removeGLObjectFromGrid(i);
        // Dec reference of connection
        i->decRef("GNEEdge::clearGNEConnections");
        // Delete GNEConnectionToErase if is unreferenced
        if (i->unreferenced()) {
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + i->getTagStr() + " '" + i->getID() + "' in clearGNEConnections()");
            delete i;
        }
    }
    myGNEConnections.clear();
}


int
GNEEdge::getRouteProbeRelativePosition(GNERouteProbe* routeProbe) const {
    std::vector<GNEAdditional*> routeProbes;
    for (auto i : getAdditionalChilds()) {
        if (i->getTagProperty().getTag() == routeProbe->getTagProperty().getTag()) {
            routeProbes.push_back(i);
        }
    }
    // return index of routeProbe in routeProbes vector
    auto it = std::find(routeProbes.begin(), routeProbes.end(), routeProbe);
    if (it == routeProbes.end()) {
        return -1;
    } else {
        return (int)(it - routeProbes.begin());
    }
}


std::vector<GNECrossing*>
GNEEdge::getGNECrossings() {
    std::vector<GNECrossing*> crossings;
    for (auto i : myGNEJunctionSource->getGNECrossings()) {
        if (i->checkEdgeBelong(this)) {
            crossings.push_back(i);
        }
    }
    for (auto i : myGNEJunctionDestiny->getGNECrossings()) {
        if (i->checkEdgeBelong(this)) {
            crossings.push_back(i);
        }
    }
    return crossings;
}


void
GNEEdge::copyTemplate(GNEEdge* tpl, GNEUndoList* undoList) {
    undoList->p_begin("copy template");
    setAttribute(SUMO_ATTR_NUMLANES,   tpl->getAttribute(SUMO_ATTR_NUMLANES),  undoList);
    setAttribute(SUMO_ATTR_TYPE,       tpl->getAttribute(SUMO_ATTR_TYPE),     undoList);
    setAttribute(SUMO_ATTR_PRIORITY,   tpl->getAttribute(SUMO_ATTR_PRIORITY), undoList);
    setAttribute(SUMO_ATTR_SPREADTYPE, tpl->getAttribute(SUMO_ATTR_SPREADTYPE), undoList);
    // copy raw values for lane-specific attributes
    setAttribute(SUMO_ATTR_SPEED,      toString(myNBEdge.getSpeed()), undoList);
    setAttribute(SUMO_ATTR_WIDTH,      toString(myNBEdge.getLaneWidth()), undoList);
    setAttribute(SUMO_ATTR_ENDOFFSET,  toString(myNBEdge.getEndOffset()), undoList);
    // copy lane attributes as well
    for (int i = 0; i < (int)myLanes.size(); i++) {
        myLanes[i]->setAttribute(SUMO_ATTR_ALLOW, tpl->myLanes[i]->getAttribute(SUMO_ATTR_ALLOW), undoList);
        myLanes[i]->setAttribute(SUMO_ATTR_SPEED, tpl->myLanes[i]->getAttribute(SUMO_ATTR_SPEED), undoList);
        myLanes[i]->setAttribute(SUMO_ATTR_WIDTH, tpl->myLanes[i]->getAttribute(SUMO_ATTR_WIDTH), undoList);
        myLanes[i]->setAttribute(SUMO_ATTR_ENDOFFSET, tpl->myLanes[i]->getAttribute(SUMO_ATTR_ENDOFFSET), undoList);
    }
    undoList->p_end();
}


std::set<GUIGlID>
GNEEdge::getLaneGlIDs() const {
    std::set<GUIGlID> result;
    for (auto i : myLanes) {
        result.insert(i->getGlID());
    }
    return result;
}


const std::vector<GNELane*>&
GNEEdge::getLanes() const {
    return myLanes;
}


const std::vector<GNEConnection*>&
GNEEdge::getGNEConnections() const {
    return myGNEConnections;
}


bool
GNEEdge::wasSplit() {
    return myWasSplit;
}


std::string
GNEEdge::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_FROM:
            return myGNEJunctionSource->getMicrosimID();
        case SUMO_ATTR_TO:
            return myGNEJunctionDestiny->getMicrosimID();
        case SUMO_ATTR_NUMLANES:
            return toString(myNBEdge.getNumLanes());
        case SUMO_ATTR_PRIORITY:
            return toString(myNBEdge.getPriority());
        case SUMO_ATTR_LENGTH:
            return toString(myNBEdge.getFinalLength());
        case SUMO_ATTR_TYPE:
            return myNBEdge.getTypeID();
        case SUMO_ATTR_SHAPE:
            return toString(myNBEdge.getInnerGeometry());
        case SUMO_ATTR_SPREADTYPE:
            return toString(myNBEdge.getLaneSpreadFunction());
        case SUMO_ATTR_NAME:
            return myNBEdge.getStreetName();
        case SUMO_ATTR_ALLOW:
            return (getVehicleClassNames(myNBEdge.getPermissions()) + (myNBEdge.hasLaneSpecificPermissions() ? " (combined!)" : ""));
        case SUMO_ATTR_DISALLOW: {
            return (getVehicleClassNames(invertPermissions(myNBEdge.getPermissions())) + (myNBEdge.hasLaneSpecificPermissions() ? " (combined!)" : ""));
        }
        case SUMO_ATTR_SPEED:
            if (myNBEdge.hasLaneSpecificSpeed()) {
                return "lane specific";
            } else {
                return toString(myNBEdge.getSpeed());
            }
        case SUMO_ATTR_WIDTH:
            if (myNBEdge.hasLaneSpecificWidth()) {
                return "lane specific";
            } else {
                return toString(myNBEdge.getLaneWidth());
            }
        case SUMO_ATTR_ENDOFFSET:
            if (myNBEdge.hasLaneSpecificEndOffset()) {
                return "lane specific";
            } else {
                return toString(myNBEdge.getEndOffset());
            }
        case SUMO_ATTR_DISTANCE:
            return toString(myNBEdge.getDistance());
        case GNE_ATTR_MODIFICATION_STATUS:
            return myConnectionStatus;
        case GNE_ATTR_SHAPE_START:
            if (myNBEdge.getGeometry().front() == myGNEJunctionSource->getPositionInView()) {
                return "";
            } else {
                return toString(myNBEdge.getGeometry().front());
            }
        case GNE_ATTR_SHAPE_END:
            if (myNBEdge.getGeometry().back() == myGNEJunctionDestiny->getPositionInView()) {
                return "";
            } else {
                return toString(myNBEdge.getGeometry().back());
            }
        case GNE_ATTR_BIDIR:
            return toString(myNBEdge.isBidiRail());
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

std::string
GNEEdge::getAttributeForSelection(SumoXMLAttr key) const {
    std::string result = getAttribute(key);
    if ((key == SUMO_ATTR_ALLOW || key == SUMO_ATTR_DISALLOW) && result.find("all") != std::string::npos) {
        result += " " + getVehicleClassNames(SVCAll, true);
    }
    return result;
}

void
GNEEdge::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_ENDOFFSET:
        case SUMO_ATTR_SPEED:
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW: {
            undoList->p_begin("change " + getTagStr() + " attribute");
            const std::string origValue = myLanes.at(0)->getAttribute(key); // will have intermediate value of "lane specific"
            // lane specific attributes need to be changed via lanes to allow undo
            for (auto it : myLanes) {
                it->setAttribute(key, value, undoList);
            }
            // ensure that the edge value is also changed. Actually this sets the lane attributes again but it does not matter
            undoList->p_add(new GNEChange_Attribute(this, myNet, key, value, true, origValue));
            undoList->p_end();
            break;
        }
        case SUMO_ATTR_FROM: {
            undoList->p_begin("change  " + getTagStr() + "  attribute");
            // Remove edge from crossings of junction source
            removeEdgeFromCrossings(myGNEJunctionSource, undoList);
            // continue changing from junction
            GNEJunction* oldGNEJunctionSource = myGNEJunctionSource;
            myGNEJunctionSource->setLogicValid(false, undoList);
            undoList->p_add(new GNEChange_Attribute(this, myNet, key, value));
            myGNEJunctionSource->setLogicValid(false, undoList);
            myNet->retrieveJunction(value)->setLogicValid(false, undoList);
            setAttribute(GNE_ATTR_SHAPE_START, toString(myGNEJunctionSource->getNBNode()->getPosition()), undoList);
            myGNEJunctionSource->invalidateShape();
            undoList->p_end();
            // update geometries of all implicated junctions
            oldGNEJunctionSource->updateGeometry();
            myGNEJunctionSource->updateGeometry();
            myGNEJunctionDestiny->updateGeometry();
            break;
        }
        case SUMO_ATTR_TO: {
            undoList->p_begin("change  " + getTagStr() + "  attribute");
            // Remove edge from crossings of junction destiny
            removeEdgeFromCrossings(myGNEJunctionDestiny, undoList);
            // continue changing destiny junction
            GNEJunction* oldGNEJunctionDestiny = myGNEJunctionDestiny;
            myGNEJunctionDestiny->setLogicValid(false, undoList);
            undoList->p_add(new GNEChange_Attribute(this, myNet, key, value));
            myGNEJunctionDestiny->setLogicValid(false, undoList);
            myNet->retrieveJunction(value)->setLogicValid(false, undoList);
            setAttribute(GNE_ATTR_SHAPE_END, toString(myGNEJunctionDestiny->getNBNode()->getPosition()), undoList);
            myGNEJunctionDestiny->invalidateShape();
            undoList->p_end();
            // update geometries of all implicated junctions
            oldGNEJunctionDestiny->updateGeometry();
            myGNEJunctionDestiny->updateGeometry();
            myGNEJunctionSource->updateGeometry();
            break;
        }
        case SUMO_ATTR_ID:
        case SUMO_ATTR_PRIORITY:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_SPREADTYPE:
        case SUMO_ATTR_DISTANCE:
        case GNE_ATTR_MODIFICATION_STATUS:
        case GNE_ATTR_SHAPE_START:
        case GNE_ATTR_SHAPE_END:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myNet, key, value));
            break;
        case SUMO_ATTR_NAME:
            // user cares about street names. Make sure they appear in the output
            OptionsCont::getOptions().resetWritable();
            OptionsCont::getOptions().set("output.street-names", "true");
            undoList->p_add(new GNEChange_Attribute(this, myNet, key, value));
            break;
        case SUMO_ATTR_NUMLANES:
            if (value != getAttribute(key)) {
                // Remove edge from crossings of junction source
                removeEdgeFromCrossings(myGNEJunctionSource, undoList);
                // Remove edge from crossings of junction destiny
                removeEdgeFromCrossings(myGNEJunctionDestiny, undoList);
                // set num lanes
                setNumLanes(parse<int>(value), undoList);
            }
            break;
        case SUMO_ATTR_SHAPE:
            // @note: assumes value of inner geometry!
            // actually the geometry is already updated (incrementally
            // during mouse movement). We set the restore point to the end
            // of the last change-set
            undoList->p_add(new GNEChange_Attribute(this, myNet, key, value));
            break;
        case GNE_ATTR_BIDIR:
            throw InvalidArgument("Attribute of '" + toString(key) + "' cannot be modified");
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEEdge::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->retrieveEdge(value, false) == nullptr);
        case SUMO_ATTR_FROM: {
            // check that is a valid ID and is different of ID of junction destiny
            if (SUMOXMLDefinitions::isValidNetID(value) && (value != myGNEJunctionDestiny->getMicrosimID())) {
                GNEJunction* junctionFrom = myNet->retrieveJunction(value, false);
                // check that there isn't already another edge with the same From and To Edge
                if ((junctionFrom != nullptr) && (myNet->retrieveEdge(junctionFrom, myGNEJunctionDestiny, false) == nullptr)) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
        case SUMO_ATTR_TO: {
            // check that is a valid ID and is different of ID of junction Source
            if (SUMOXMLDefinitions::isValidNetID(value) && (value != myGNEJunctionSource->getMicrosimID())) {
                GNEJunction* junctionTo = myNet->retrieveJunction(value, false);
                // check that there isn't already another edge with the same From and To Edge
                if ((junctionTo != nullptr) && (myNet->retrieveEdge(myGNEJunctionSource, junctionTo, false) == nullptr)) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
        case SUMO_ATTR_SPEED:
            return canParse<double>(value) && (parse<double>(value) > 0);
        case SUMO_ATTR_NUMLANES:
            return canParse<int>(value) && (parse<double>(value) > 0);
        case SUMO_ATTR_PRIORITY:
            return canParse<int>(value);
        case SUMO_ATTR_LENGTH:
            return canParse<double>(value) && ((parse<double>(value) > 0) || (parse<double>(value) == NBEdge::UNSPECIFIED_LOADED_LENGTH));
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_TYPE:
            return true;
        case SUMO_ATTR_SHAPE:
            // empty shapes are allowed
            return canParse<PositionVector>(value);
        case SUMO_ATTR_SPREADTYPE:
            return SUMOXMLDefinitions::LaneSpreadFunctions.hasString(value);
        case SUMO_ATTR_NAME:
            return true;
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value) && ((parse<double>(value) > 0) || (parse<double>(value) == NBEdge::UNSPECIFIED_WIDTH));
        case SUMO_ATTR_ENDOFFSET:
            return canParse<double>(value) && parse<double>(value) >= 0 && parse<double>(value) < myNBEdge.getLoadedLength();
        case SUMO_ATTR_DISTANCE:
            return canParse<double>(value);
        case GNE_ATTR_SHAPE_START: {
            if (value.empty()) {
                return true;
            } else if (canParse<Position>(value)) {
                Position shapeStart = parse<Position>(value);
                return (shapeStart != myNBEdge.getGeometry()[-1]);
            } else {
                return false;
            }
        }
        case GNE_ATTR_SHAPE_END: {
            if (value.empty()) {
                return true;
            } else if (canParse<Position>(value)) {
                Position shapeEnd = parse<Position>(value);
                return (shapeEnd != myNBEdge.getGeometry()[0]);
            } else {
                return false;
            }
        }
        case GNE_ATTR_BIDIR:
            return false;
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNEEdge::getGenericParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i : myNBEdge.getParametersMap()) {
        result += i.first + "=" + i.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNEEdge::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : myNBEdge.getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNEEdge::setGenericParametersStr(const std::string& value) {
    // clear parameters
    myNBEdge.clearParameter();
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
            myNBEdge.setParameter(parsedParameters.front(), parsedParameters.back());
        }
    }
}


void
GNEEdge::setResponsible(bool newVal) {
    myAmResponsible = newVal;
}

// ===========================================================================
// private
// ===========================================================================

void
GNEEdge::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myNet->renameEdge(this, value);
            break;
        case SUMO_ATTR_FROM:
            myNet->changeEdgeEndpoints(this, value, myGNEJunctionDestiny->getMicrosimID());
            // update this edge of list of outgoings edges of the old GNEJunctionSource
            myGNEJunctionSource->removeOutgoingGNEEdge(this);
            // update GNEJunctionSource
            myGNEJunctionSource = myNet->retrieveJunction(myNBEdge.getFromNode()->getID());
            // update this edge of list of outgoings edges of the new GNEJunctionSource
            myGNEJunctionSource->addOutgoingGNEEdge(this);
            break;
        case SUMO_ATTR_TO:
            myNet->changeEdgeEndpoints(this, myGNEJunctionSource->getMicrosimID(), value);
            // update this edge of list of incomings edges of the old GNEJunctionDestiny
            myGNEJunctionDestiny->removeIncomingGNEEdge(this);
            // update GNEJunctionDestiny
            myGNEJunctionDestiny = myNet->retrieveJunction(myNBEdge.getToNode()->getID());
            // update this edge of list of incomings edges of the new GNEJunctionDestiny
            myGNEJunctionDestiny->addIncomingGNEEdge(this);
            break;
        case SUMO_ATTR_NUMLANES:
            throw InvalidArgument("GNEEdge::setAttribute (private) called for attr SUMO_ATTR_NUMLANES. This should never happen");
            break;
        case SUMO_ATTR_PRIORITY:
            myNBEdge.myPriority = parse<int>(value);
            break;
        case SUMO_ATTR_LENGTH:
            myNBEdge.setLoadedLength(parse<double>(value));
            break;
        case SUMO_ATTR_TYPE:
            myNBEdge.myType = value;
            break;
        case SUMO_ATTR_SHAPE:
            setGeometry(parse<PositionVector>(value), true, true);
            break;
        case SUMO_ATTR_SPREADTYPE:
            myNBEdge.setLaneSpreadFunction(SUMOXMLDefinitions::LaneSpreadFunctions.get(value));
            break;
        case SUMO_ATTR_NAME:
            myNBEdge.setStreetName(value);
            break;
        case SUMO_ATTR_SPEED:
            myNBEdge.setSpeed(-1, parse<double>(value));
            break;
        case SUMO_ATTR_WIDTH:
            myNBEdge.setLaneWidth(-1, parse<double>(value));
            break;
        case SUMO_ATTR_ENDOFFSET:
            myNBEdge.setEndOffset(-1, parse<double>(value));
            break;
        case SUMO_ATTR_ALLOW:
            break;  // no edge value
        case SUMO_ATTR_DISALLOW:
            break; // no edge value
        case SUMO_ATTR_DISTANCE:
            myNBEdge.setDistance(parse<double>(value));
            break;
        case GNE_ATTR_MODIFICATION_STATUS:
            myConnectionStatus = value;
            if (value == FEATURE_GUESSED) {
                WRITE_DEBUG("invalidating (removing) connections of edge '" + getID() + "' due it were guessed");
                myNBEdge.invalidateConnections(true);
                clearGNEConnections();
            } else if (value != FEATURE_GUESSED) {
                WRITE_DEBUG("declaring connections of edge '" + getID() + "' as loaded (It will not be removed)");
                myNBEdge.declareConnectionsAsLoaded();
            }
            break;
        case GNE_ATTR_SHAPE_START: {
            // get geometry of NBEdge, remove FIRST element with the new value (or with the Junction Source position) and set it back to edge
            Position newShapeStart;
            if (value == "") {
                newShapeStart = myGNEJunctionSource->getPositionInView();
            } else {
                newShapeStart = parse<Position>(value);
            }
            // set shape start position
            setShapeStartPos(newShapeStart, true);
            break;
        }
        case GNE_ATTR_SHAPE_END: {
            // get geometry of NBEdge, remove LAST element with the new value (or with the Junction Destiny position) and set it back to edge
            Position newShapeEnd;
            if (value == "") {
                newShapeEnd = myGNEJunctionDestiny->getPositionInView();
            } else {
                newShapeEnd = parse<Position>(value);
            }
            // set shape end position
            setShapeEndPos(newShapeEnd, true);
            break;
        }
        case GNE_ATTR_BIDIR:
            throw InvalidArgument("Attribute of '" + toString(key) + "' cannot be modified");
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
    // check if updated attribute requieres update geometry
    if (myTagProperty.hasAttribute(key) && myTagProperty.getAttributeProperties(key).requiereUpdateGeometry()) {
        updateGeometry();
    }
}


void
GNEEdge::setNumLanes(int numLanes, GNEUndoList* undoList) {
    undoList->p_begin("change number of " + toString(SUMO_TAG_LANE) +  "s");
    myGNEJunctionSource->setLogicValid(false, undoList);
    myGNEJunctionDestiny->setLogicValid(false, undoList);

    const int oldNumLanes = (int)myLanes.size();
    for (int i = oldNumLanes; i < numLanes; i++) {
        // since the GNELane does not exist yet, it cannot have yet been referenced so we only pass a zero-pointer
        undoList->add(new GNEChange_Lane(this, nullptr,
                                         myNBEdge.getLaneStruct(oldNumLanes - 1), true), true);
    }
    for (int i = oldNumLanes - 1; i > numLanes - 1; i--) {
        // delete leftmost lane
        undoList->add(new GNEChange_Lane(this, myLanes[i], myNBEdge.getLaneStruct(i), false), true);
    }
    undoList->p_end();
}


void
GNEEdge::addLane(GNELane* lane, const NBEdge::Lane& laneAttrs, bool recomputeConnections) {
    // boundary of edge depends of number of lanes. We need to extract if before add or remove lane
    myNet->removeGLObjectFromGrid(this);
    const int index = lane ? lane->getIndex() : myNBEdge.getNumLanes();
    // the laneStruct must be created first to ensure we have some geometry
    // unless the connections are fully recomputed, existing indices must be shifted
    myNBEdge.addLane(index, true, recomputeConnections, !recomputeConnections);
    if (lane) {
        // restore a previously deleted lane
        myLanes.insert(myLanes.begin() + index, lane);

    } else {
        // create a new lane by copying leftmost lane
        lane = new GNELane(*this, index);
        myLanes.push_back(lane);
    }
    lane->incRef("GNEEdge::addLane");
    // check if lane is selected
    if (lane->isAttributeCarrierSelected()) {
        lane->selectAttributeCarrier();
    }
    // we copy all attributes except shape since this is recomputed from edge shape
    myNBEdge.setSpeed(lane->getIndex(), laneAttrs.speed);
    myNBEdge.setPermissions(laneAttrs.permissions, lane->getIndex());
    myNBEdge.setPreferredVehicleClass(laneAttrs.preferred, lane->getIndex());
    myNBEdge.setEndOffset(lane->getIndex(), laneAttrs.endOffset);
    myNBEdge.setLaneWidth(lane->getIndex(), laneAttrs.width);
    // udate indices
    for (int i = 0; i < (int)myLanes.size(); ++i) {
        myLanes[i]->setIndex(i);
    }
    /* while technically correct, this looks ugly
    myGNEJunctionSource->invalidateShape();
    myGNEJunctionDestiny->invalidateShape();
    */
    // Remake connections for this edge and all edges that target this lane
    remakeGNEConnections();
    // remake connections of all edges of junction source and destiny
    for (auto i : myGNEJunctionSource->getGNEEdges()) {
        i->remakeGNEConnections();
    }
    // remake connections of all edges of junction source and destiny
    for (auto i : myGNEJunctionDestiny->getGNEEdges()) {
        i->remakeGNEConnections();
    }
    // add object again
    myNet->addGLObjectIntoGrid(this);
    // Update geometry with the new lane
    updateGeometry();
}


void
GNEEdge::removeLane(GNELane* lane, bool recomputeConnections) {
    // boundary of edge depends of number of lanes. We need to extract if before add or remove lane
    myNet->removeGLObjectFromGrid(this);
    if (myLanes.size() == 0) {
        throw ProcessError("Should not remove the last " + toString(SUMO_TAG_LANE) + " from an " + getTagStr());
    }
    if (lane == nullptr) {
        lane = myLanes.back();
    }
    // check if lane is selected
    if (lane->isAttributeCarrierSelected()) {
        lane->unselectAttributeCarrier();
    }
    // Delete lane of edge's container
    // unless the connections are fully recomputed, existing indices must be shifted
    myNBEdge.deleteLane(lane->getIndex(), recomputeConnections, !recomputeConnections);
    lane->decRef("GNEEdge::removeLane");
    myLanes.erase(myLanes.begin() + lane->getIndex());
    // Delete lane if is unreferenced
    if (lane->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + lane->getTagStr() + " '" + lane->getID() + "' in removeLane()");
        delete lane;
    }
    // udate indices
    for (int i = 0; i < (int)myLanes.size(); ++i) {
        myLanes[i]->setIndex(i);
    }
    /* while technically correct, this looks ugly
    myGNEJunctionSource->invalidateShape();
    myGNEJunctionDestiny->invalidateShape();
    */
    // Remake connections of this edge
    remakeGNEConnections();
    // remake connections of all edges of junction source and destiny
    for (auto i : myGNEJunctionSource->getGNEEdges()) {
        i->remakeGNEConnections();
    }
    // remake connections of all edges of junction source and destiny
    for (auto i : myGNEJunctionDestiny->getGNEEdges()) {
        i->remakeGNEConnections();
    }
    // add object again
    myNet->addGLObjectIntoGrid(this);
    // Update element
    updateGeometry();
}


void
GNEEdge::addConnection(NBEdge::Connection nbCon, bool selectAfterCreation) {
    // If a new connection was sucesfully created
    if (myNBEdge.setConnection(nbCon.fromLane, nbCon.toEdge, nbCon.toLane, NBEdge::L2L_USER, true, nbCon.mayDefinitelyPass,
                               nbCon.keepClear, nbCon.contPos, nbCon.visibility,
                               nbCon.speed, nbCon.customShape, nbCon.uncontrolled)) {
        // Create  or retrieve existent GNEConection
        GNEConnection* con = retrieveGNEConnection(nbCon.fromLane, nbCon.toEdge, nbCon.toLane);
        // add it to GNEConnection container
        myGNEConnections.push_back(con);
        // Add reference
        myGNEConnections.back()->incRef("GNEEdge::addConnection");
        // select GNEConnection if needed
        if (selectAfterCreation) {
            con->selectAttributeCarrier();
        }
        // update geometry
        con->updateGeometry();
        // iterate over all additionals from "from" lane and check E2 multilane integrity
        for (auto i : con->getLaneFrom()->getAdditionalChilds()) {
            if (i->getTagProperty().getTag() == SUMO_TAG_E2DETECTOR_MULTILANE) {
                dynamic_cast<GNEDetectorE2*>(i)->checkE2MultilaneIntegrity();
            }
        }
        // iterate over all additionals from "to" lane and check E2 multilane integrity
        for (auto i : con->getLaneTo()->getAdditionalChilds()) {
            if (i->getTagProperty().getTag() == SUMO_TAG_E2DETECTOR_MULTILANE) {
                dynamic_cast<GNEDetectorE2*>(i)->checkE2MultilaneIntegrity();
            }
        }
    }
    // actually we only do this to force a redraw
    updateGeometry();
}


void
GNEEdge::removeConnection(NBEdge::Connection nbCon) {
    // check if is a explicit turnaround
    if (nbCon.toEdge == myNBEdge.getTurnDestination()) {
        myNet->removeExplicitTurnaround(getMicrosimID());
    }
    // remove NBEdge::connection from NBEdge
    myNBEdge.removeFromConnections(nbCon);
    // remove their associated GNEConnection
    GNEConnection* con = retrieveGNEConnection(nbCon.fromLane, nbCon.toEdge, nbCon.toLane, false);
    if (con != nullptr) {
        con->decRef("GNEEdge::removeConnection");
        myGNEConnections.erase(std::find(myGNEConnections.begin(), myGNEConnections.end(), con));
        // iterate over all additionals from "from" lane and check E2 multilane integrity
        for (auto i : con->getLaneFrom()->getAdditionalChilds()) {
            if (i->getTagProperty().getTag() == SUMO_TAG_E2DETECTOR_MULTILANE) {
                dynamic_cast<GNEDetectorE2*>(i)->checkE2MultilaneIntegrity();
            }
        }
        // iterate over all additionals from "to" lane and check E2 multilane integrity
        for (auto i : con->getLaneTo()->getAdditionalChilds()) {
            if (i->getTagProperty().getTag() == SUMO_TAG_E2DETECTOR_MULTILANE) {
                dynamic_cast<GNEDetectorE2*>(i)->checkE2MultilaneIntegrity();
            }
        }
        // remove it from Tree
        myNet->removeGLObjectFromGrid(con);
        // check if connection is selected
        if (con->isAttributeCarrierSelected()) {
            con->unselectAttributeCarrier();
        }
        if (con->unreferenced()) {
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + con->getTagStr() + " '" + con->getID() + "' in removeConnection()");
            delete con;
            // actually we only do this to force a redraw
            updateGeometry();
        }
    }
}


GNEConnection*
GNEEdge::retrieveGNEConnection(int fromLane, NBEdge* to, int toLane, bool createIfNoExist) {
    for (auto i : myGNEConnections) {
        if ((i->getFromLaneIndex() == fromLane) && (i->getEdgeTo()->getNBEdge() == to) && (i->getToLaneIndex() == toLane)) {
            return i;
        }
    }
    if (createIfNoExist) {
        // create new connection. Will be added to the rTree on first geometry computation
        GNEConnection* createdConnection = new GNEConnection(myLanes[fromLane], myNet->retrieveEdge(to->getID())->getLanes()[toLane]);
        // show extra information for tests
        WRITE_DEBUG("Created " + createdConnection->getTagStr() + " '" + createdConnection->getID() + "' in retrieveGNEConnection()");
        // iterate over all additionals from "from" lane and check E2 multilane integrity
        for (auto i : createdConnection->getLaneFrom()->getAdditionalChilds()) {
            if (i->getTagProperty().getTag() == SUMO_TAG_E2DETECTOR_MULTILANE) {
                dynamic_cast<GNEDetectorE2*>(i)->checkE2MultilaneIntegrity();
            }
        }
        // iterate over all additionals from "to" lane and check E2 multilane integrity
        for (auto i : createdConnection->getLaneTo()->getAdditionalChilds()) {
            if (i->getTagProperty().getTag() == SUMO_TAG_E2DETECTOR_MULTILANE) {
                dynamic_cast<GNEDetectorE2*>(i)->checkE2MultilaneIntegrity();
            }
        }
        return createdConnection;
    } else {
        return nullptr;
    }
}



void
GNEEdge::setMicrosimID(const std::string& newID) {
    GUIGlObject::setMicrosimID(newID);
    for (auto i : myLanes) {
        i->setMicrosimID(getNBEdge()->getLaneID(i->getIndex()));
    }
}


bool
GNEEdge::hasRestrictedLane(SUMOVehicleClass vclass) const {
    for (auto i : myLanes) {
        if (i->isRestricted(vclass)) {
            return true;
        }
    }
    return false;
}


void
GNEEdge::removeEdgeFromCrossings(GNEJunction* junction, GNEUndoList* undoList) {
    // Remove all crossings that contain this edge in parameter "edges"
    for (GNECrossing* const i : junction->getGNECrossings()) {
        if (i->checkEdgeBelong(this)) {
            myNet->deleteCrossing(i, undoList);
        }
    }
}


void
GNEEdge::straightenElevation(GNEUndoList* undoList) {
    PositionVector modifiedShape = myNBEdge.getGeometry().interpolateZ(
                                       myNBEdge.getFromNode()->getPosition().z(),
                                       myNBEdge.getToNode()->getPosition().z());
    PositionVector innerShape(modifiedShape.begin() + 1, modifiedShape.end() - 1);
    setAttribute(SUMO_ATTR_SHAPE, toString(innerShape), undoList);
}


PositionVector
GNEEdge::smoothShape(const PositionVector& old, bool forElevation) {
    const OptionsCont& oc = OptionsCont::getOptions();
    // distinguish 3 cases:
    // a) if the edge has exactly 3 or 4 points, use these as control points
    // b) if the edge has more than 4 points, use the first 2 and the last 2 as control points
    // c) if the edge is straight and both nodes are geometry-like nodes, use geometry of the continuation edges as control points
    PositionVector init;
#ifdef DEBUG_SMOOTH_GEOM
    if (DEBUGCOND(this)) std::cout << getID()
                                       << " forElevation=" << forElevation
                                       << " fromGeometryLike=" << myNBEdge.getFromNode()->geometryLike()
                                       << " toGeometryLike=" << myNBEdge.getToNode()->geometryLike()
                                       << " smoothShape old=" << old << "\n";
#endif
    if (old.size() == 3 || old.size() == 4) {
        init = old;
    } else if (old.size() > 4 && !forElevation) {
        // for elevation, the initial segments are not useful
        init.push_back(old[0]);
        init.push_back(old[1]);
        init.push_back(old[-2]);
        init.push_back(old[-1]);
    } else if (myNBEdge.getFromNode()->geometryLike() && myNBEdge.getToNode()->geometryLike()) {
        PositionVector begShape;
        PositionVector endShape;
        const EdgeVector& incoming = myNBEdge.getFromNode()->getIncomingEdges();
        const EdgeVector& outgoing = myNBEdge.getToNode()->getOutgoingEdges();
        if (incoming.size() == 1) {
            begShape = incoming[0]->getGeometry();
        } else {
            assert(incoming.size() == 2);
            begShape = myNBEdge.isTurningDirectionAt(incoming[0]) ? incoming[1]->getGeometry() : incoming[0]->getGeometry();
        }
        if (outgoing.size() == 1) {
            endShape = outgoing[0]->getGeometry();
        } else {
            assert(outgoing.size() == 2);
            endShape = myNBEdge.isTurningDirectionAt(outgoing[0]) ? outgoing[1]->getGeometry() : outgoing[0]->getGeometry();
        }
        const double dist = MIN2(old.length2D(), MAX2(old.length2D() / 8, fabs(old[0].z() - old[-1].z()) * OptionsCont::getOptions().getFloat("geometry.max-grade") / 3));
        if (forElevation) {
            // initialize control point elevation for smooth continuation
            init.push_back(old[0]);
            init.push_back(old.positionAtOffset2D(dist));
            init.push_back(old.positionAtOffset2D(old.length2D() - dist));
            init.push_back(old[-1]);
            double begZ = begShape.positionAtOffset2D(MAX2(0.0, begShape.length2D() - dist)).z();
            double endZ = endShape.positionAtOffset2D(MIN2(begShape.length2D(), dist)).z();
            // continue incline
            init[1].setz(2 * init[0].z() - begZ);
            init[2].setz(2 * init[-1].z() - endZ);
        } else {
            bool ok = true;
            const double straightThresh = DEG2RAD(oc.getFloat("opendrive-output.straight-threshold"));
            init = NBNode::bezierControlPoints(begShape, endShape, false, dist, dist, ok, nullptr, straightThresh);
        }
#ifdef DEBUG_SMOOTH_GEOM
        if (DEBUGCOND(this)) {
            std::cout << "   begShape=" << begShape << " endShape=" << endShape << " forElevation=" << forElevation << " dist=" << dist << " ok=" << ok << " init=" << init << "\n";
        }
#endif
    }
    if (init.size() == 0) {
        return PositionVector::EMPTY;
    } else {
        const int numPoints = MAX2(oc.getInt("junctions.internal-link-detail"),
                                   int(old.length2D() / oc.getFloat("opendrive.curve-resolution")));
        return init.bezier(numPoints);
    }
}


void
GNEEdge::smooth(GNEUndoList* undoList) {
    PositionVector modifiedShape = smoothShape(myNBEdge.getGeometry(), false);
    if (modifiedShape.size() < 2) {
        WRITE_WARNING("Could not compute smooth shape for edge '" + getID() + "'");
    } else {
        PositionVector innerShape(modifiedShape.begin() + 1, modifiedShape.end() - 1);
        setAttribute(SUMO_ATTR_SHAPE, toString(innerShape), undoList);
    }
}


void
GNEEdge::smoothElevation(GNEUndoList* undoList) {
    PositionVector elevationBase;
    for (const Position& pos : myNBEdge.getGeometry()) {
        if (elevationBase.size() == 0 || elevationBase[-1].z() != pos.z()) {
            elevationBase.push_back(pos);
        }
    }
    PositionVector elevation = smoothShape(elevationBase, true);
    if (elevation.size() <= 2) {
        WRITE_WARNING("Could not compute smooth elevation for edge '" + getID() + "'");
    } else {
        PositionVector modifiedShape = myNBEdge.getGeometry();
        if (modifiedShape.size() < 5) {
            modifiedShape = modifiedShape.resample(OptionsCont::getOptions().getFloat("opendrive.curve-resolution"));
        }
        const double scale = elevation.length2D() / modifiedShape.length2D();
        //std::cout << "   elevation=" << elevation << "\n mod1=" << modifiedShape << " scale=" << scale << "\n";
        double seen = 0;
        for (int i = 1; i < (int)modifiedShape.size(); ++i) {
            seen += modifiedShape[i - 1].distanceTo2D(modifiedShape[i]);
            modifiedShape[i].setz(elevation.positionAtOffset2D(seen * scale).z());
        }
        //std::cout << "   mod2=" << modifiedShape << "\n";
        PositionVector innerShape(modifiedShape.begin() + 1, modifiedShape.end() - 1);
        setAttribute(SUMO_ATTR_SHAPE, toString(innerShape), undoList);
    }
}


void
GNEEdge::setShapeStartPos(const Position& pos, bool updateGrid) {
    // remove start position and add it the new position
    PositionVector geom = myNBEdge.getGeometry();
    geom.erase(geom.begin());
    geom.push_front_noDoublePos(pos);
    // restore modified shape
    setGeometry(geom, false, updateGrid);
}


void
GNEEdge::setShapeEndPos(const Position& pos, bool updateGrid) {
    // remove end position and add it the new position
    PositionVector geom = myNBEdge.getGeometry();
    geom.pop_back();
    geom.push_back_noDoublePos(pos);
    // restore modified shape
    setGeometry(geom, false, updateGrid);
}

/****************************************************************************/
