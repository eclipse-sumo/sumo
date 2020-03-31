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
/// @file    GNEEdge.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// A road/street connecting two junctions (netedit-version, adapted from GUIEdge)
// Basically a container for an NBEdge with drawing and editing capabilities
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/additional/GNEDetectorE2.h>
#include <netedit/elements/additional/GNERouteProbe.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Lane.h>
#include <netedit/elements/demand/GNERoute.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/options/OptionsCont.h>

#include "GNEConnection.h"
#include "GNECrossing.h"
#include "GNEJunction.h"
#include "GNELane.h"
#include "GNEEdge.h"

//#define DEBUG_SMOOTH_GEOM
//#define DEBUGCOND(obj) (true)
#define VEHICLE_GAP 1

// ===========================================================================
// static
// ===========================================================================
const double GNEEdge::SNAP_RADIUS = SUMO_const_halfLaneWidth;

// ===========================================================================
// members methods
// ===========================================================================

GNEEdge::GNEEdge(GNENet* net, NBEdge* nbe, bool wasSplit, bool loaded):
    GNENetworkElement(net, nbe->getID(), GLO_EDGE, SUMO_TAG_EDGE,
        {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}),
    myNBEdge(nbe),
    myGNEJunctionSource(myNet->retrieveJunction(nbe->getFromNode()->getID())),
    myGNEJunctionDestiny(myNet->retrieveJunction(nbe->getToNode()->getID())),
    myLanes(0),
    myAmResponsible(false),
    myWasSplit(wasSplit),
    myConnectionStatus(loaded ? FEATURE_LOADED : FEATURE_GUESSED),
    myUpdateGeometry(true) {
    // Create lanes
    int numLanes = myNBEdge->getNumLanes();
    myLanes.reserve(numLanes);
    for (int i = 0; i < numLanes; i++) {
        myLanes.push_back(new GNELane(this, i));
        myLanes.back()->incRef("GNEEdge::GNEEdge");
    }
    // update Lane geometries
    for (const auto& i : myLanes) {
        i->updateGeometry();
    }
}


GNEEdge::~GNEEdge() {
    // Delete references to this eddge in lanes
    for (const auto& lane : myLanes) {
        lane->decRef("GNEEdge::~GNEEdge");
        if (lane->unreferenced()) {
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + lane->getTagStr() + " '" + lane->getID() + "' in GNEEdge destructor");
            delete lane;
        }
    }
    // delete references to this eddge in connections
    for (const auto& connection : myGNEConnections) {
        connection->decRef("GNEEdge::~GNEEdge");
        if (connection->unreferenced()) {
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + connection->getTagStr() + " '" + connection->getID() + "' in GNEEdge destructor");
            delete connection;
        }
    }
    if (myAmResponsible) {
        delete myNBEdge;
    }
}


std::string
GNEEdge::generateChildID(SumoXMLTag /*childTag*/) {
    // currently unused
    return "";
}


void
GNEEdge::updateGeometry() {
    // first check if myUpdateGeometry flag is enabled
    if (myUpdateGeometry) {
        // Update geometry of lanes
        for (const auto& lane : myLanes) {
            lane->updateGeometry();
        }
        // Update geometry of connections (Only if updateGrid is enabled, because in move mode connections are hidden
        // (note: only the previous marked as deprecated will be updated)
        if (!myMovingGeometryBoundary.isInitialised()) {
            for (const auto& connection : myGNEConnections) {
                connection->updateGeometry();
            }
        }
        // Update geometry of additionals children vinculated to this edge
        for (const auto& childAdditionals : getChildAdditionals()) {
            childAdditionals->updateGeometry();
        }
        // Update geometry of parent additionals that have this edge as parent
        for (const auto& additionalParent : getParentAdditionals()) {
            additionalParent->updateGeometry();
        }
        // Update partial geometry of demand elements parents that have this edge as parent
        for (const auto& demandElementParent : getParentDemandElements()) {
            demandElementParent->updatePartialGeometry(this);
        }
        // Update partial geometry of demand elements children vinculated to this edge
        for (const auto& childDemandElements : getChildDemandElements()) {
            childDemandElements->updatePartialGeometry(this);
        }
        // Update partial geometry of routes vinculated to this edge
        for (const auto& pathElementChild : myPathDemandElementsElementChilds) {
            pathElementChild->updatePartialGeometry(this);
        }
        // mark dotted geometry deprecated
        myDottedGeometry.markDottedGeometryDeprecated();
    }
    // update vehicle geometry
    updateVehicleSpreadGeometries();
    // update vehicle stack labels
    updateVehicleStackLabels();
}


Position
GNEEdge::getPositionInView() const {
    // currently unused
    return Position(0, 0);
}


bool
GNEEdge::clickedOverShapeStart(const Position& pos) {
    if (myNBEdge->getGeometry().front() != myGNEJunctionSource->getPositionInView()) {
        return (myNBEdge->getGeometry().front().distanceTo2D(pos) < SNAP_RADIUS);
    } else {
        return false;
    }
}


bool
GNEEdge::clickedOverShapeEnd(const Position& pos) {
    if (myNBEdge->getGeometry().back() != myGNEJunctionDestiny->getPositionInView()) {
        return (myNBEdge->getGeometry().back().distanceTo2D(pos) < SNAP_RADIUS);
    } else {
        return false;
    }
}


void 
GNEEdge::startShapeBegin() {
    myPositionBeforeMoving = myNBEdge->getGeometry().front();
    // save current centering boundary
    myMovingGeometryBoundary = getCenteringBoundary();
}


void 
GNEEdge::startShapeEnd() {
    myPositionBeforeMoving = myNBEdge->getGeometry().back();
    // save current centering boundary
    myMovingGeometryBoundary = getCenteringBoundary();
}


void
GNEEdge::moveShapeBegin(const Position& offset) {
    // change shape startPosition using oldPosition and offset
    Position shapeStartEdited = myPositionBeforeMoving;
    shapeStartEdited.add(offset);
    // snap to active grid
    shapeStartEdited = myNet->getViewNet()->snapToActiveGrid(shapeStartEdited, offset.z() == 0);
    // make sure that start and end position are different
    if (shapeStartEdited != myNBEdge->getGeometry().back()) {
        // set shape start position without updating grid
        setShapeStartPos(shapeStartEdited);
        updateGeometry();
    }
}


void
GNEEdge::moveShapeEnd(const Position& offset) {
    // change shape endPosition using oldPosition and offset
    Position shapeEndEdited = myPositionBeforeMoving;
    shapeEndEdited.add(offset);
    // snap to active grid
    shapeEndEdited = myNet->getViewNet()->snapToActiveGrid(shapeEndEdited, offset.z() == 0);
    // make sure that start and end position are different
    if (shapeEndEdited != myNBEdge->getGeometry().front()) {
        // set shape end position without updating grid
        setShapeEndPos(shapeEndEdited);
        updateGeometry();
    }
}


void
GNEEdge::commitShapeChangeBegin(GNEUndoList* undoList) {
    // first save current shape start position
    Position modifiedShapeStartPos = myNBEdge->getGeometry().front();
    // restore old shape start position
    setShapeStartPos(myPositionBeforeMoving);
    // end geometry moving
    endEdgeGeometryMoving();
    // set attribute using undolist
    undoList->p_begin("shape start of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, myNet, GNE_ATTR_SHAPE_START, toString(modifiedShapeStartPos), true, toString(myPositionBeforeMoving)));
    undoList->p_end();
}


void
GNEEdge::commitShapeChangeEnd(GNEUndoList* undoList) {
    // first save current shape end position
    Position modifiedShapeEndPos = myNBEdge->getGeometry().back();
    // restore old shape end position
    setShapeEndPos(myPositionBeforeMoving);
    // end geometry moving
    endEdgeGeometryMoving();
    // set attribute using undolist
    undoList->p_begin("shape end of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, myNet, GNE_ATTR_SHAPE_END, toString(modifiedShapeEndPos), true, toString(myPositionBeforeMoving)));
    undoList->p_end();
}


int
GNEEdge::getEdgeVertexIndex(Position pos, const bool snapToGrid) const {
    // check if position has to be snapped to grid
    if (snapToGrid) {
        pos = myNet->getViewNet()->snapToActiveGrid(pos);
    }
    const double offset = myNBEdge->getGeometry().nearest_offset_to_point2D(pos, true);
    if (offset == GeomHelper::INVALID_OFFSET) {
        return -1;
    }
    Position newPos = myNBEdge->getGeometry().positionAtOffset2D(offset);
    // first check if vertex already exists in the inner geometry
    for (int i = 0; i < (int)myNBEdge->getGeometry().size(); i++) {
        if (myNBEdge->getGeometry()[i].distanceTo2D(newPos) < SNAP_RADIUS) {
            // index refers to inner geometry
            if (i == 0 || i == (int)(myNBEdge->getGeometry().size() - 1)) {
                return -1;
            }
            return i;
        }
    }
    return -1;
}


void
GNEEdge::startEdgeGeometryMoving(const double shapeOffset, const bool invertOffset) {
    // save current centering boundary
    myMovingGeometryBoundary = getCenteringBoundary();
    // start move shape
    if (invertOffset) {
        startMoveShape(myNBEdge->getGeometry(), myNBEdge->getGeometry().length() - shapeOffset, SNAP_RADIUS);
    } else {
        startMoveShape(myNBEdge->getGeometry(), shapeOffset, SNAP_RADIUS);
    }
    // Save current centering boundary of lanes (and their children)
    for (const auto &lane : myLanes) {
        lane->startGeometryMoving();
    }
    // Save current centering boundary of additionals children vinculated to this edge
    for (const auto &additional : getChildAdditionals()) {
        additional->startGeometryMoving();
    }
    // Save current centering boundary of parent additionals that have this edge as parent
    for (const auto &additional : getParentAdditionals()) {
        additional->startGeometryMoving();
    }
    // Save current centering boundary of demand elements children vinculated to this edge
    for (const auto &demandElement : getChildDemandElements()) {
        demandElement->startGeometryMoving();
    }
    // Save current centering boundary of demand elements parents that have this edge as parent
    for (const auto& demandElement : getParentDemandElements()) {
        demandElement->startGeometryMoving();
    }
}


void
GNEEdge::moveEdgeShape(const Position& offset) {
    // first make a copy of myMovingShape
    PositionVector newShape = getShapeBeforeMoving();
    // move entire shap if this edge and their junctions is selected
    const bool allSelected = mySelected && myGNEJunctionSource->isAttributeCarrierSelected() && myGNEJunctionDestiny->isAttributeCarrierSelected();
    if (moveEntireShape() || allSelected) {
        // move entire shape
        newShape.add(offset);
    } else {
        int geometryPointIndex = getGeometryPointIndex();
        // if geometryPoint is -1, then we have to create a new geometry point
        if (geometryPointIndex == -1) {
            geometryPointIndex = newShape.insertAtClosest(getPosOverShapeBeforeMoving(), true);
        }
        // move geometry point within newShape
        newShape[geometryPointIndex].add(offset);
        // check if edge is selected
        if (isAttributeCarrierSelected()) {
            // move more geometry points, depending if junctions are selected
            if (myGNEJunctionSource->isAttributeCarrierSelected()) {
                for (int i = 1; i < geometryPointIndex; i++) {
                    newShape[i].add(offset);
                }
            }
            if (myGNEJunctionDestiny->isAttributeCarrierSelected()) {
                for (int i = (geometryPointIndex + 1); i < (int)newShape.size(); i++) {
                    newShape[i].add(offset);
                }
            }
        }
    }
    // pop front and back 
    newShape.pop_front();
    newShape.pop_back();
    // set new inner shape
    setGeometry(newShape, true);
}


void
GNEEdge::endEdgeGeometryMoving() {
    // check that endGeometryMoving was called only once
    if (myMovingGeometryBoundary.isInitialised()) {
        // Remove object from net
        myNet->removeGLObjectFromGrid(this);
        // reset myMovingGeometryBoundary
        myMovingGeometryBoundary.reset();
        // Restore centering boundary of lanes (and their children)
        for (const auto& lane : myLanes) {
            lane->endGeometryMoving();
        }
        // Restore centering boundary of additionals children vinculated to this edge
        for (const auto& additional : getChildAdditionals()) {
            additional->endGeometryMoving();
        }
        // Restore centering boundary of parent additionals that have this edge as parent
        for (const auto& additional : getParentAdditionals()) {
            additional->endGeometryMoving();
        }
        // Restore centering boundary of demand elements children vinculated to this edge
        for (const auto& demandElement : getChildDemandElements()) {
            demandElement->endGeometryMoving();
        }
        // Restore centering boundary of demand elements parents that have this edge as parent
        for (const auto& demandElement : getParentDemandElements()) {
            demandElement->endGeometryMoving();
        }
        // add object into grid again (using the new centering boundary)
        myNet->addGLObjectIntoGrid(this);
    }
}


void
GNEEdge::commitEdgeShapeChange(GNEUndoList* undoList) {
    // restore original shape into shapeToCommit
    PositionVector innerShapeToCommit = myNBEdge->getInnerGeometry();
    // first check if second and penultimate isn't in Junction's buubles
    double buubleRadius = GNEJunction::BUBBLE_RADIUS * myNet->getViewNet()->getVisualisationSettings().junctionSize.exaggeration;
    if (myNBEdge->getGeometry().size() > 2 && myNBEdge->getGeometry()[0].distanceTo2D(myNBEdge->getGeometry()[1]) < buubleRadius) {
        innerShapeToCommit.removeClosest(innerShapeToCommit[0]);
    }
    if (myNBEdge->getGeometry().size() > 2 && myNBEdge->getGeometry()[(int)myNBEdge->getGeometry().size() - 2].distanceTo2D(myNBEdge->getGeometry()[(int)myNBEdge->getGeometry().size() - 1]) < buubleRadius) {
        innerShapeToCommit.removeClosest(innerShapeToCommit[(int)innerShapeToCommit.size() - 1]);
    }
    // second check if double points has to be removed
    innerShapeToCommit.removeDoublePoints(SNAP_RADIUS);
    // show warning if some of edge's shape was merged
    if (innerShapeToCommit.size() != myNBEdge->getInnerGeometry().size()) {
        WRITE_WARNING("Merged shape's point")
    }

    updateGeometry();
    // restore old geometry to allow change attribute (And restore shape if during movement a new point was created
    setGeometry(getShapeBeforeMoving(), false);
    // finish geometry moving
    endEdgeGeometryMoving();
    // commit new shape
    undoList->p_begin("moving " + toString(SUMO_ATTR_SHAPE) + " of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, myNet, SUMO_ATTR_SHAPE, toString(innerShapeToCommit)));
    undoList->p_end();
}


void
GNEEdge::deleteEdgeGeometryPoint(const Position& pos, bool allowUndo) {
    // obtain index and remove point
    PositionVector modifiedShape = myNBEdge->getInnerGeometry();
    int index = modifiedShape.indexOfClosest(pos);
    modifiedShape.erase(modifiedShape.begin() + index);
    // set new shape depending of allowUndo
    if (allowUndo) {
        myNet->getViewNet()->getUndoList()->p_begin("delete geometry point");
        setAttribute(SUMO_ATTR_SHAPE, toString(modifiedShape), myNet->getViewNet()->getUndoList());
        myNet->getViewNet()->getUndoList()->p_end();
    } else {
        // set new shape
        setGeometry(modifiedShape, true);
    }
}


void
GNEEdge::updateJunctionPosition(GNEJunction* junction, const Position& origPos) {
    Position delta = junction->getNBNode()->getPosition() - origPos;
    PositionVector geom = myNBEdge->getGeometry();
    // geometry endpoint need not equal junction position hence we modify it with delta
    if (junction == myGNEJunctionSource) {
        geom[0].add(delta);
    } else {
        geom[-1].add(delta);
    }
    setGeometry(geom, false);
}


Boundary
GNEEdge::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (myMovingGeometryBoundary.isInitialised()) {
        return myMovingGeometryBoundary;
    }  else {
        Boundary b;
        for (const auto& i : myLanes) {
            b.add(i->getCenteringBoundary());
        }
        // ensure that geometry points are selectable even if the lane geometry is strange
        for (const Position& pos : myNBEdge->getGeometry()) {
            b.add(pos);
        }
        b.grow(10);
        return b;
    }
}

const std::string
GNEEdge::getOptionalName() const {
    return myNBEdge->getStreetName();
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
    if (s.drawBoundaries) {
        GLHelper::drawBoundary(getCenteringBoundary());
    }
    // draw lanes
    for (const auto& lane : myLanes) {
        lane->drawGL(s);
    }
    // draw parent additionals
    for (const auto& additional : getParentAdditionals()) {
        if (additional->getTagProperty().getTag() == SUMO_TAG_REROUTER) {
            // draw rerouter symbol
            drawRerouterSymbol(s, additional);
        }
    }
    // draw child additional
    for (const auto& additional : getChildAdditionals()) {
        additional->drawGL(s);
    }
    // draw child edge
    if (myNet->getViewNet()->getNetworkViewOptions().showDemandElements() && myNet->getViewNet()->getDataViewOptions().showDemandElements()) {
        drawDemandElements(s);
    }
    // draw geometry points if isnt's too small and
    if ((s.scale > 8.0) && !myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        drawGeometryPoints(s);
    }
    // draw vehicles
    const std::map<const GNELane*, std::vector<GNEDemandElement*> > vehiclesMap = getVehiclesOverEdgeMap();
    for (const auto& vehicleMap : vehiclesMap) {
        for (const auto& vehicle : vehicleMap.second) {
            vehicle->drawGL(s);
        }
    }
    // draw name if isn't being drawn for selecting
    if (!s.drawForRectangleSelection) {
        drawEdgeName(s);
    }
    // draw dotted contor around the first and last lane if isn't being drawn for selecting
    if (myNet->getViewNet()->getDottedAC() == this) {
        // use GLO_JUNCTION instead GLO_EDGE
        GNEGeometry::drawShapeDottedContour(s, GLO_JUNCTION, s.laneWidthExaggeration, myDottedGeometry);
    }
}


NBEdge*
GNEEdge::getNBEdge() const {
    return myNBEdge;
}


Position
GNEEdge::getSplitPos(const Position& clickPos) {
    const PositionVector& geom = myNBEdge->getGeometry();
    int index = geom.indexOfClosest(clickPos);
    if (geom[index].distanceTo2D(clickPos) < SNAP_RADIUS) {
        // split at existing geometry point
        return geom[index];
    } else {
        // split straight between the next two points
        return geom.positionAtOffset(geom.nearest_offset_to_point2D(clickPos));
    }
}


void
GNEEdge::editEndpoint(Position pos, GNEUndoList* undoList) {
    if ((myNBEdge->getGeometry().front() != myGNEJunctionSource->getPositionInView()) && (myNBEdge->getGeometry().front().distanceTo2D(pos) < SNAP_RADIUS)) {
        undoList->p_begin("remove endpoint");
        setAttribute(GNE_ATTR_SHAPE_START, "", undoList);
        undoList->p_end();
    } else if ((myNBEdge->getGeometry().back() != myGNEJunctionDestiny->getPositionInView()) && (myNBEdge->getGeometry().back().distanceTo2D(pos) < SNAP_RADIUS)) {
        undoList->p_begin("remove endpoint");
        setAttribute(GNE_ATTR_SHAPE_END, "", undoList);
        undoList->p_end();
    } else {
        // we need to create new Start/End position over Edge shape, not over clicked position
        double offset = myNBEdge->getGeometry().nearest_offset_to_point2D(myNet->getViewNet()->snapToActiveGrid(pos), true);
        if (offset != GeomHelper::INVALID_OFFSET) {
            PositionVector geom = myNBEdge->getGeometry();
            // calculate position over edge shape relative to clicked positino
            Position newPos = geom.positionAtOffset2D(offset);
            // snap new position to grid
            newPos = myNet->getViewNet()->snapToActiveGrid(newPos);
            undoList->p_begin("set endpoint");
            int index = geom.indexOfClosest(pos);
            // check if snap to existing geometry
            if (geom[index].distanceTo2D(pos) < SNAP_RADIUS) {
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
            if (myNBEdge->getInnerGeometry().size() > 0 && getEdgeVertexIndex(pos, false) != -1) {
                deleteEdgeGeometryPoint(pos, false);
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
GNEEdge::setGeometry(PositionVector geom, bool inner) {
    // set new geometry
    const bool lefthand = OptionsCont::getOptions().getBool("lefthand");
    if (lefthand) {
        geom.mirrorX();
        myNBEdge->mirrorX();
    }
    myNBEdge->setGeometry(geom, inner);
    if (lefthand) {
        myNBEdge->mirrorX();
    }
    // update geometry
    updateGeometry();
    // invalidate junction source shape
    myGNEJunctionSource->invalidateShape();
    // iterate over GNEJunctionSource edges and update geometry
    for (const auto& edge : myGNEJunctionSource->getGNEIncomingEdges()) {
        edge->updateGeometry();
    }
    for (const auto& edge : myGNEJunctionSource->getGNEOutgoingEdges()) {
        edge->updateGeometry();
    }
    // invalidate junction destiny shape
    myGNEJunctionDestiny->invalidateShape();
    // iterate over GNEJunctionDestiny edges and update geometry
    for (const auto& edge : myGNEJunctionDestiny->getGNEIncomingEdges()) {
        edge->updateGeometry();
    }
    for (const auto& edge : myGNEJunctionDestiny->getGNEOutgoingEdges()) {
        edge->updateGeometry();
    }
}


const Position 
GNEEdge::getFrontUpShapePosition() const {
    PositionVector laneShape = myLanes.front()->getLaneShape();
    laneShape.move2side(myLanes.front()->getParentEdge()->getNBEdge()->getLaneWidth(myLanes.front()->getIndex()) / 2);
    return laneShape.front();
}


const Position 
GNEEdge::getFrontDownShapePosition() const {
    PositionVector laneShape = myLanes.back()->getLaneShape();
    laneShape.move2side(-1 * myLanes.back()->getParentEdge()->getNBEdge()->getLaneWidth(myLanes.back()->getIndex()) / 2);
    return laneShape.front();
}


const Position 
GNEEdge::getBackUpShapePosition() const {
    PositionVector laneShape = myLanes.front()->getLaneShape();
    laneShape.move2side(myLanes.front()->getParentEdge()->getNBEdge()->getLaneWidth(myLanes.front()->getIndex()) / 2);
    return laneShape.back();
}


const Position 
GNEEdge::getBackDownShapePosition() const {
    PositionVector laneShape = myLanes.back()->getLaneShape();
    laneShape.move2side(-1 * myLanes.back()->getParentEdge()->getNBEdge()->getLaneWidth(myLanes.back()->getIndex()) / 2);
    return laneShape.back();
}

void
GNEEdge::remakeGNEConnections() {
    // create new and removed unused GNEConnectinos
    const std::vector<NBEdge::Connection>& connections = myNBEdge->getConnections();
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
    for (auto i : getChildAdditionals()) {
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
    // begin undo list
    undoList->p_begin("copy template");
    // copy edge-specific attributes
    setAttribute(SUMO_ATTR_NUMLANES,   tpl->getAttribute(SUMO_ATTR_NUMLANES), undoList);
    setAttribute(SUMO_ATTR_TYPE,       tpl->getAttribute(SUMO_ATTR_TYPE), undoList);
    setAttribute(SUMO_ATTR_PRIORITY,   tpl->getAttribute(SUMO_ATTR_PRIORITY), undoList);
    setAttribute(SUMO_ATTR_SPREADTYPE, tpl->getAttribute(SUMO_ATTR_SPREADTYPE), undoList);
    // copy raw values for lane-specific attributes
    setAttribute(SUMO_ATTR_SPEED,      toString(myNBEdge->getSpeed()), undoList);
    setAttribute(SUMO_ATTR_WIDTH,      toString(myNBEdge->getLaneWidth()), undoList);
    setAttribute(SUMO_ATTR_ENDOFFSET,  toString(myNBEdge->getEndOffset()), undoList);
    // copy lane attributes as well
    for (int i = 0; i < (int)myLanes.size(); i++) {
        myLanes[i]->setAttribute(SUMO_ATTR_ALLOW, tpl->myLanes[i]->getAttribute(SUMO_ATTR_ALLOW), undoList);
        myLanes[i]->setAttribute(SUMO_ATTR_SPEED, tpl->myLanes[i]->getAttribute(SUMO_ATTR_SPEED), undoList);
        myLanes[i]->setAttribute(SUMO_ATTR_WIDTH, tpl->myLanes[i]->getAttribute(SUMO_ATTR_WIDTH), undoList);
        myLanes[i]->setAttribute(SUMO_ATTR_ENDOFFSET, tpl->myLanes[i]->getAttribute(SUMO_ATTR_ENDOFFSET), undoList);
    }
    // end undo list
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
            return getID();
        case SUMO_ATTR_FROM:
            return myGNEJunctionSource->getID();
        case SUMO_ATTR_TO:
            return myGNEJunctionDestiny->getID();
        case SUMO_ATTR_NUMLANES:
            return toString(myNBEdge->getNumLanes());
        case SUMO_ATTR_PRIORITY:
            return toString(myNBEdge->getPriority());
        case SUMO_ATTR_LENGTH:
            return toString(myNBEdge->getFinalLength());
        case SUMO_ATTR_TYPE:
            return myNBEdge->getTypeID();
        case SUMO_ATTR_SHAPE:
            return toString(myNBEdge->getInnerGeometry());
        case SUMO_ATTR_SPREADTYPE:
            return toString(myNBEdge->getLaneSpreadFunction());
        case SUMO_ATTR_NAME:
            return myNBEdge->getStreetName();
        case SUMO_ATTR_ALLOW:
            return (getVehicleClassNames(myNBEdge->getPermissions()) + (myNBEdge->hasLaneSpecificPermissions() ? " (combined!)" : ""));
        case SUMO_ATTR_DISALLOW: {
            return (getVehicleClassNames(invertPermissions(myNBEdge->getPermissions())) + (myNBEdge->hasLaneSpecificPermissions() ? " (combined!)" : ""));
        }
        case SUMO_ATTR_SPEED:
            if (myNBEdge->hasLaneSpecificSpeed()) {
                return "lane specific";
            } else {
                return toString(myNBEdge->getSpeed());
            }
        case SUMO_ATTR_WIDTH:
            if (myNBEdge->hasLaneSpecificWidth()) {
                return "lane specific";
            } else {
                return toString(myNBEdge->getLaneWidth());
            }
        case SUMO_ATTR_ENDOFFSET:
            if (myNBEdge->hasLaneSpecificEndOffset()) {
                return "lane specific";
            } else {
                return toString(myNBEdge->getEndOffset());
            }
        case SUMO_ATTR_DISTANCE:
            return toString(myNBEdge->getDistance());
        case GNE_ATTR_MODIFICATION_STATUS:
            return myConnectionStatus;
        case GNE_ATTR_SHAPE_START:
            if (myNBEdge->getGeometry().front() == myGNEJunctionSource->getPositionInView()) {
                return "";
            } else {
                return toString(myNBEdge->getGeometry().front());
            }
        case GNE_ATTR_SHAPE_END:
            if (myNBEdge->getGeometry().back() == myGNEJunctionDestiny->getPositionInView()) {
                return "";
            } else {
                return toString(myNBEdge->getGeometry().back());
            }
        case GNE_ATTR_BIDIR:
            return toString(myNBEdge->isBidiRail());
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return myNBEdge->getParametersStr();
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
        case GNE_ATTR_PARAMETERS:
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
            if (SUMOXMLDefinitions::isValidNetID(value) && (value != myGNEJunctionDestiny->getID())) {
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
            if (SUMOXMLDefinitions::isValidNetID(value) && (value != myGNEJunctionSource->getID())) {
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
            return canParse<double>(value) && ((parse<double>(value) >= -1) || (parse<double>(value) == NBEdge::UNSPECIFIED_WIDTH));
        case SUMO_ATTR_ENDOFFSET:
            return canParse<double>(value) && parse<double>(value) >= 0 && parse<double>(value) < myNBEdge->getLoadedLength();
        case SUMO_ATTR_DISTANCE:
            return canParse<double>(value);
        case GNE_ATTR_SHAPE_START: {
            if (value.empty()) {
                return true;
            } else if (canParse<Position>(value)) {
                Position shapeStart = parse<Position>(value);
                return (shapeStart != myNBEdge->getGeometry()[-1]);
            } else {
                return false;
            }
        }
        case GNE_ATTR_SHAPE_END: {
            if (value.empty()) {
                return true;
            } else if (canParse<Position>(value)) {
                Position shapeEnd = parse<Position>(value);
                return (shapeEnd != myNBEdge->getGeometry()[0]);
            } else {
                return false;
            }
        }
        case GNE_ATTR_BIDIR:
            return false;
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEEdge::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case GNE_ATTR_BIDIR:
            return false;
        default:
            return true;
    }
}


void
GNEEdge::setResponsible(bool newVal) {
    myAmResponsible = newVal;
}


GNELane*
GNEEdge::getLaneByAllowedVClass(const SUMOVehicleClass vClass) const {
    // iterate over all NBEdge lanes
    for (int i = 0; i < (int)myNBEdge->getLanes().size(); i++) {
        // if given VClass is in permissions, return lane
        if (myNBEdge->getLanes().at(i).permissions & vClass) {
            // return GNELane
            return myLanes.at(i);
        }
    }
    // return first lane
    return myLanes.front();
}


GNELane*
GNEEdge::getLaneByDisallowedVClass(const SUMOVehicleClass vClass) const {
    // iterate over all NBEdge lanes
    for (int i = 0; i < (int)myNBEdge->getLanes().size(); i++) {
        // if given VClass isn't in permissions, return lane
        if (~(myNBEdge->getLanes().at(i).permissions) & vClass) {
            // return GNELane
            return myLanes.at(i);
        }
    }
    // return first lane
    return myLanes.front();
}


void
GNEEdge::drawPartialRoute(const GUIVisualizationSettings& s, const GNEDemandElement* route, const GNEJunction* junction) const {
    // calculate route width
    double routeWidth = s.addSize.getExaggeration(s, this) * s.widthSettings.route;
    // obtain color
    RGBColor routeColor;
    if (route->drawUsingSelectColor()) {
        routeColor = s.colorSettings.selectedRouteColor;
    } else {
        routeColor = route->getColor();
    }
    // Start drawing adding an gl identificator
    glPushName(route->getGlID());
    // Add a draw matrix
    glPushMatrix();
    // Start with the drawing of the area traslating matrix to origin
    glTranslated(0, 0, route->getType());
    // draw route
    if (junction) {
        // iterate over segments
        for (const auto& segment : route->getDemandElementSegmentGeometry()) {
            // draw partial segment
            if ((segment.junction == junction) && (segment.AC == route)) {
                // Set route color (needed due drawShapeDottedContour)
                GLHelper::setColor(routeColor);
                // draw box lines
                GNEGeometry::drawSegmentGeometry(myNet->getViewNet(), segment, routeWidth);
                // check if shape dotted contour has to be drawn
                if (myNet->getViewNet()->getDottedAC() == route) {
                    GLHelper::drawShapeDottedContourAroundShape(s, getType(), segment.getShape(), routeWidth);
                }
            }
        }
    } else {
        // iterate over segments
        for (const auto& segment : route->getDemandElementSegmentGeometry()) {
            // draw partial segment
            if ((segment.edge == this) && (segment.AC == route)) {
                // Set route color (needed due drawShapeDottedContour)
                GLHelper::setColor(routeColor);
                // draw box lines
                GNEGeometry::drawSegmentGeometry(myNet->getViewNet(), segment, routeWidth);
                // check if shape dotted contour has to be drawn
                if (myNet->getViewNet()->getDottedAC() == route) {
                    GLHelper::drawShapeDottedContourAroundShape(s, getType(), segment.getShape(), routeWidth);
                }
            }
        }
    }
    // Pop last matrix
    glPopMatrix();
    // Draw name if isn't being drawn for selecting
    if (!s.drawForRectangleSelection) {
        drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    }
    // Pop name
    glPopName();
    // draw route children
    for (const auto& i : route->getChildDemandElements()) {
        if (i->getTagProperty().getTag() == SUMO_TAG_WALK_ROUTE) {
            drawPartialPersonPlan(s, i, junction);
        }
    }
}


void
GNEEdge::drawPartialTripFromTo(const GUIVisualizationSettings& s, const GNEDemandElement* tripOrFromTo, const GNEJunction* junction) const {
    // declare flag to draw spread vehicles
    const bool drawSpreadVehicles = (myNet->getViewNet()->getNetworkViewOptions().drawSpreadVehicles() || myNet->getViewNet()->getDemandViewOptions().drawSpreadVehicles());
    // calculate tripOrFromTo width
    double tripOrFromToWidth = s.addSize.getExaggeration(s, this) * s.widthSettings.trip;
    // Add a draw matrix
    glPushMatrix();
    // Start with the drawing of the area traslating matrix to origin
    glTranslated(0, 0, tripOrFromTo->getType());
    // Set color of the base
    if (tripOrFromTo->drawUsingSelectColor()) {
        GLHelper::setColor(s.colorSettings.selectedVehicleColor);
    } else {
        GLHelper::setColor(s.colorSettings.vehicleTrips);
    }
    // draw trip from to
    if (junction) {
        // iterate over segments
        if (drawSpreadVehicles) {
            for (const auto& segment : tripOrFromTo->getDemandElementSegmentSpreadGeometry()) {
                // draw partial segment
                GNEGeometry::drawSegmentGeometry(myNet->getViewNet(), segment, tripOrFromToWidth);
            }
        } else {
            for (const auto& segment : tripOrFromTo->getDemandElementSegmentGeometry()) {
                // draw partial segment
                GNEGeometry::drawSegmentGeometry(myNet->getViewNet(), segment, tripOrFromToWidth);
            }
        }
    } else {
        // iterate over segments
        if (drawSpreadVehicles) {
            for (const auto& segment : tripOrFromTo->getDemandElementSegmentSpreadGeometry()) {
                // draw partial segment
                if ((segment.edge == this) && (segment.AC == tripOrFromTo)) {
                    GNEGeometry::drawSegmentGeometry(myNet->getViewNet(), segment, tripOrFromToWidth);
                }
            }
        } else {
            for (const auto& segment : tripOrFromTo->getDemandElementSegmentGeometry()) {
                // draw partial segment
                if ((segment.edge == this) && (segment.AC == tripOrFromTo)) {
                    GNEGeometry::drawSegmentGeometry(myNet->getViewNet(), segment, tripOrFromToWidth);
                }
            }
        }
    }
    // Pop last matrix
    glPopMatrix();
    // Draw name if isn't being drawn for selecting
    if (!s.drawForRectangleSelection) {
        drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    }
}


void
GNEEdge::drawPartialPersonPlan(const GUIVisualizationSettings& s, const GNEDemandElement* personPlan, const GNEJunction* junction) const {
    // declare flag to enable or disable draw person plan
    bool drawPersonPlan = false;
    if (myNet->getViewNet()->getDemandViewOptions().showAllPersonPlans()) {
        drawPersonPlan = true;
    } else if (myNet->getViewNet()->getDottedAC() == personPlan->getParentDemandElements().front()) {
        drawPersonPlan = true;
    } else if (myNet->getViewNet()->getDemandViewOptions().getLockedPerson() == personPlan->getParentDemandElements().front()) {
        drawPersonPlan = true;
    } else if (myNet->getViewNet()->getDottedAC() && myNet->getViewNet()->getDottedAC()->getTagProperty().isPersonPlan() &&
               (myNet->getViewNet()->getDottedAC()->getAttribute(GNE_ATTR_PARENT) == personPlan->getAttribute(GNE_ATTR_PARENT))) {
        drawPersonPlan = true;
    }
    // check if draw person plan elements can be drawn
    if (drawPersonPlan) {
        // calculate personPlan width
        double personPlanWidth = 0;
        // flag to check if width must be duplicated
        bool duplicateWidth = (myNet->getViewNet()->getDottedAC() == personPlan) || (myNet->getViewNet()->getDottedAC() == personPlan->getParentDemandElements().front()) ? true : false;
        // Set width depending of person plan type
        if (personPlan->getTagProperty().isPersonTrip()) {
            personPlanWidth = s.addSize.getExaggeration(s, this) * s.widthSettings.personTrip;
        } else if (personPlan->getTagProperty().isWalk()) {
            personPlanWidth = s.addSize.getExaggeration(s, this) * s.widthSettings.walk;
        } else if (personPlan->getTagProperty().isRide()) {
            personPlanWidth = s.addSize.getExaggeration(s, this) * s.widthSettings.ride;
        }
        // check if width has to be duplicated
        if (duplicateWidth) {
            personPlanWidth *= 2;
        }
        // set personPlan color
        RGBColor personPlanColor;
        // Set color depending of person plan type
        if (personPlan->drawUsingSelectColor()) {
            personPlanColor = s.colorSettings.selectedPersonPlanColor;
        } else if (personPlan->getTagProperty().isPersonTrip()) {
            personPlanColor = s.colorSettings.personTrip;
        } else if (personPlan->getTagProperty().isWalk()) {
            personPlanColor = s.colorSettings.walk;
        } else if (personPlan->getTagProperty().isRide()) {
            personPlanColor = s.colorSettings.ride;
        }
        // Start drawing adding an gl identificator
        glPushName(personPlan->getGlID());
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, personPlan->getType());
        // draw person plan
        if (junction) {
            // iterate over segments
            for (const auto& segment : personPlan->getDemandElementSegmentGeometry()) {
                // draw partial segment
                if ((segment.junction == junction) && (segment.AC == personPlan)) {
                    // Set person plan color (needed due drawShapeDottedContour)
                    GLHelper::setColor(personPlanColor);
                    // draw box line
                    GNEGeometry::drawSegmentGeometry(myNet->getViewNet(), segment, personPlanWidth);
                    // check if shape dotted contour has to be drawn
                    if (myNet->getViewNet()->getDottedAC() == personPlan) {
                        GNEGeometry::drawSegmentGeometry(myNet->getViewNet(), segment, personPlanWidth);
                    }
                }
            }
        } else {
            // iterate over segments
            for (const auto& segment : personPlan->getDemandElementSegmentGeometry()) {
                // draw partial segment
                if ((segment.edge == this) && (segment.AC == personPlan)) {
                    // Set person plan color (needed due drawShapeDottedContour)
                    GLHelper::setColor(personPlanColor);
                    // draw box line
                    GNEGeometry::drawSegmentGeometry(myNet->getViewNet(), segment, personPlanWidth);
                    // check if shape dotted contour has to be drawn
                    if (myNet->getViewNet()->getDottedAC() == personPlan) {
                        GNEGeometry::drawSegmentGeometry(myNet->getViewNet(), segment, personPlanWidth);
                    }
                }
            }
        }
        // Pop last matrix
        glPopMatrix();
        // Draw name if isn't being drawn for selecting
        if (!s.drawForRectangleSelection) {
            drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
        }
        // Pop name
        glPopName();
        // check if person plan ArrivalPos attribute
        if (personPlan->getTagProperty().hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
            // obtain arrival position using last segment
            const Position& arrivalPos = personPlan->getDemandElementSegmentGeometry().getLastPosition();
            // only draw arrival position point if isn't -1
            if (arrivalPos != Position::INVALID) {
                // obtain circle width
                const double circleWidth = (duplicateWidth ? SNAP_RADIUS : (SNAP_RADIUS / 2.0)) * MIN2((double)0.5, s.laneWidthExaggeration);
                const double circleWidthSquared = circleWidth * circleWidth;
                if (!s.drawForRectangleSelection || (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(arrivalPos) <= (circleWidthSquared + 2))) {
                    glPushMatrix();
                    // translate to pos and move to upper using GLO_PERSONTRIP (to avoid overlapping)
                    glTranslated(arrivalPos.x(), arrivalPos.y(), GLO_PERSONTRIP + 0.01);
                    // Set color depending of person plan type
                    if (personPlan->drawUsingSelectColor()) {
                        GLHelper::setColor(s.colorSettings.selectedPersonPlanColor);
                    } else if (personPlan->getTagProperty().isPersonTrip()) {
                        GLHelper::setColor(s.colorSettings.personTrip);
                    } else if (personPlan->getTagProperty().isWalk()) {
                        GLHelper::setColor(s.colorSettings.walk);
                    } else if (personPlan->getTagProperty().isRide()) {
                        GLHelper::setColor(s.colorSettings.ride);
                    }
                    // resolution of drawn circle depending of the zoom (To improve smothness)
                    GLHelper::drawFilledCircle(circleWidth, s.getCircleResolution());
                    glPopMatrix();
                }
            }
        }
    }
    // draw person if this edge correspond to the first edge of first Person's person plan
    GNEEdge* firstEdge = nullptr;
    const GNEDemandElement* firstPersonPlan = personPlan->getParentDemandElements().front()->getChildDemandElements().front();
    if (firstPersonPlan->getTagProperty().isPersonStop()) {
        if (firstPersonPlan->getTagProperty().getTag() == SUMO_TAG_PERSONSTOP_LANE) {
            // obtain edge of parent lane
            firstEdge = firstPersonPlan->getParentLanes().front()->getParentEdge();
        } else  {
            // obtain edge of busstop's parent lane
            firstEdge = firstPersonPlan->getParentAdditionals().front()->getParentLanes().front()->getParentEdge();
        }
    } else if (firstPersonPlan->getTagProperty().getTag() == SUMO_TAG_WALK_ROUTE) {
        // obtain first rute edge
        firstEdge = firstPersonPlan->getParentDemandElements().at(1)->getParentEdges().front();
    } else {
        // obtain first parent edge
        firstEdge = firstPersonPlan->getParentEdges().front();
    }
    // draw person parent if this is the edge first edge and this is the first plan
    if ((firstEdge == this) && (firstPersonPlan == personPlan)) {
        personPlan->getParentDemandElements().front()->drawGL(s);
    }
}


void
GNEEdge::addPathElement(GNEDemandElement* pathElementChild) {
    // avoid insert duplicatd path element childs
    if (std::find(myPathDemandElementsElementChilds.begin(), myPathDemandElementsElementChilds.end(), pathElementChild) == myPathDemandElementsElementChilds.end()) {
        myPathDemandElementsElementChilds.push_back(pathElementChild);
    }
}


void
GNEEdge::removePathElement(GNEDemandElement* pathElementChild) {
    // search and remove pathElementChild
    auto it = std::find(myPathDemandElementsElementChilds.begin(), myPathDemandElementsElementChilds.end(), pathElementChild);
    if (it != myPathDemandElementsElementChilds.end()) {
        myPathDemandElementsElementChilds.erase(it);
    }
}


void
GNEEdge::invalidatePathChildElements() {
    // make a copy of myPathDemandElementsElementChilds
    auto copyOfPathDemandElementsElementChilds = myPathDemandElementsElementChilds;
    for (const auto& pathElementChild : copyOfPathDemandElementsElementChilds) {
        pathElementChild->invalidatePath();
    }
}


void
GNEEdge::updateVehicleSpreadGeometries() {
    // get lane vehicles map
    const std::map<const GNELane*, std::vector<GNEDemandElement*> > laneVehiclesMap = getVehiclesOverEdgeMap();
    // iterate over every lane
    for (const auto& laneVehicle : laneVehiclesMap) {
        // obtain total lenght
        double totalLength = 0;
        for (const auto& vehicle : laneVehicle.second) {
            totalLength += vehicle->getAttributeDouble(SUMO_ATTR_LENGTH) + VEHICLE_GAP;
        }
        // calculate multiplier for vehicle positions
        double multiplier = 1;
        const double laneShapeLenght = laneVehicle.first->getLaneShape().length();
        if (laneShapeLenght == 0) {
            multiplier = 0;
        } else if (totalLength > laneShapeLenght) {
            multiplier = (laneShapeLenght / totalLength);
        }
        // declare current lenght
        double lenght = 0;
        // iterate over vehicles to calculate position and rotations
        for (const auto& vehicle : laneVehicle.second) {
            vehicle->updateDemandElementSpreadGeometry(laneVehicle.first, lenght * multiplier);
            // update lenght
            lenght += vehicle->getAttributeDouble(SUMO_ATTR_LENGTH) + VEHICLE_GAP;
        }
    }
}


void
GNEEdge::updateVehicleStackLabels() {
    // get lane vehicles map
    const std::map<const GNELane*, std::vector<GNEDemandElement*> > laneVehiclesMap = getVehiclesOverEdgeMap();
    // declare map for sprt vehicles using their departpos+lenght position (StackPosition)
    std::vector<std::pair<GNEEdge::StackPosition, GNEDemandElement*> > departPosVehicles;
    // declare vector of stack demand elements
    std::vector<GNEEdge::StackDemandElements> stackedVehicles;
    // iterate over laneVehiclesMap and obtain a vector with
    for (const auto& vehicleMap : laneVehiclesMap) {
        // iterate over vehicles
        for (const auto& vehicle : vehicleMap.second) {
            // get vehicle's depart pos and lenght
            const double departPos = vehicle->getAttributeDouble(SUMO_ATTR_DEPARTPOS);
            const double length = vehicle->getAttributeDouble(SUMO_ATTR_LENGTH);
            double posOverLane = vehicle->getAttributeDouble(SUMO_ATTR_DEPARTPOS);
            // check if we have to adapt posOverLane
            if (posOverLane < 0) {
                posOverLane += vehicleMap.first->getLaneShape().length();
            }
            // make a stack position using departPos and length
            departPosVehicles.push_back(std::make_pair(StackPosition(departPos, length), vehicle));
            // update depart element geometry
            vehicle->updateDemandElementGeometry(vehicleMap.first, posOverLane);
            // reset vehicle stack label
            vehicle->updateDemandElementStackLabel(0);
        }
    }
    // sort departPosVehicles
    std::sort(departPosVehicles.begin(), departPosVehicles.end());
    // iterate over departPosVehicles
    for (const auto& departPosVehicle : departPosVehicles) {
        // obtain stack position and vehicle
        const GNEEdge::StackPosition& vehicleStackPosition = departPosVehicle.first;
        GNEDemandElement* vehicle = departPosVehicle.second;
        // if stackedVehicles is empty, add a new StackDemandElements
        if (stackedVehicles.empty()) {
            stackedVehicles.push_back(GNEEdge::StackDemandElements(vehicleStackPosition, vehicle));
        } else if (areStackPositionOverlapped(vehicleStackPosition, stackedVehicles.back().getStackPosition())) {
            // add new vehicle to last inserted stackDemandElements
            stackedVehicles[stackedVehicles.size() - 1].addDemandElements(vehicle);
        } else {
            // No overlapping, then add a new StackDemandElements
            stackedVehicles.push_back(GNEEdge::StackDemandElements(vehicleStackPosition, vehicle));
        }
    }
    // iterate over stackedVehicles
    for (const auto& vehicle : stackedVehicles) {
        // only update vehicles with one or more stack
        if (vehicle.getDemandElements().size() > 1) {
            // set stack labels
            vehicle.getDemandElements().front()->updateDemandElementStackLabel((int)vehicle.getDemandElements().size());
        }
    }
}

// ===========================================================================
// private
// ===========================================================================

GNEEdge::StackPosition::StackPosition(const double departPos, const double length) :
    tuple(departPos, departPos + length) {
}


double
GNEEdge::StackPosition::beginPosition() const {
    return std::get<0>(*this);
}


double
GNEEdge::StackPosition::endPosition() const {
    return std::get<1>(*this);
}


GNEEdge::StackDemandElements::StackDemandElements(const StackPosition stackedPosition, GNEDemandElement* demandElement) :
    tuple(stackedPosition, {
    demandElement
}) {
}


void
GNEEdge::StackDemandElements::addDemandElements(GNEDemandElement* demandElement) {
    std::get<1>(*this).push_back(demandElement);
}


const GNEEdge::StackPosition&
GNEEdge::StackDemandElements::getStackPosition() const {
    return std::get<0>(*this);
}


const std::vector<GNEDemandElement*>&
GNEEdge::StackDemandElements::getDemandElements() const {
    return std::get<1>(*this);
}


void
GNEEdge::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myNet->getAttributeCarriers()->updateID(this, value);
            break;
        case SUMO_ATTR_FROM:
            myNet->changeEdgeEndpoints(this, value, myGNEJunctionDestiny->getID());
            // update this edge of list of outgoings edges of the old GNEJunctionSource
            myGNEJunctionSource->removeOutgoingGNEEdge(this);
            // update GNEJunctionSource
            myGNEJunctionSource = myNet->retrieveJunction(myNBEdge->getFromNode()->getID());
            // update this edge of list of outgoings edges of the new GNEJunctionSource
            myGNEJunctionSource->addOutgoingGNEEdge(this);
            break;
        case SUMO_ATTR_TO:
            myNet->changeEdgeEndpoints(this, myGNEJunctionSource->getID(), value);
            // update this edge of list of incomings edges of the old GNEJunctionDestiny
            myGNEJunctionDestiny->removeIncomingGNEEdge(this);
            // update GNEJunctionDestiny
            myGNEJunctionDestiny = myNet->retrieveJunction(myNBEdge->getToNode()->getID());
            // update this edge of list of incomings edges of the new GNEJunctionDestiny
            myGNEJunctionDestiny->addIncomingGNEEdge(this);
            break;
        case SUMO_ATTR_NUMLANES:
            throw InvalidArgument("GNEEdge::setAttribute (private) called for attr SUMO_ATTR_NUMLANES. This should never happen");
            break;
        case SUMO_ATTR_PRIORITY:
            myNBEdge->myPriority = parse<int>(value);
            break;
        case SUMO_ATTR_LENGTH:
            myNBEdge->setLoadedLength(parse<double>(value));
            break;
        case SUMO_ATTR_TYPE:
            myNBEdge->myType = value;
            break;
        case SUMO_ATTR_SHAPE:
            // start geometry moving (because a new shape affect all child edges)
            startEdgeGeometryMoving(-1, false);
            // set new geometry
            setGeometry(parse<PositionVector>(value), true);
            // start geometry moving (because a new shape affect all child edges)
            endEdgeGeometryMoving();
            break;
        case SUMO_ATTR_SPREADTYPE:
            myNBEdge->setLaneSpreadFunction(SUMOXMLDefinitions::LaneSpreadFunctions.get(value));
            break;
        case SUMO_ATTR_NAME:
            myNBEdge->setStreetName(value);
            break;
        case SUMO_ATTR_SPEED:
            myNBEdge->setSpeed(-1, parse<double>(value));
            break;
        case SUMO_ATTR_WIDTH:
            myNBEdge->setLaneWidth(-1, parse<double>(value));
            break;
        case SUMO_ATTR_ENDOFFSET:
            myNBEdge->setEndOffset(-1, parse<double>(value));
            break;
        case SUMO_ATTR_ALLOW:
            break;  // no edge value
        case SUMO_ATTR_DISALLOW:
            break; // no edge value
        case SUMO_ATTR_DISTANCE:
            myNBEdge->setDistance(parse<double>(value));
            break;
        case GNE_ATTR_MODIFICATION_STATUS:
            myConnectionStatus = value;
            if (value == FEATURE_GUESSED) {
                WRITE_DEBUG("invalidating (removing) connections of edge '" + getID() + "' due it were guessed");
                myNBEdge->invalidateConnections(true);
                clearGNEConnections();
            } else if (value != FEATURE_GUESSED) {
                WRITE_DEBUG("declaring connections of edge '" + getID() + "' as loaded (It will not be removed)");
                myNBEdge->declareConnectionsAsLoaded();
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
            // Remove object from net
            myNet->removeGLObjectFromGrid(this);
            // set shape start position
            setShapeStartPos(newShapeStart);
            // add object from net
            myNet->addGLObjectIntoGrid(this);
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
            // Remove object from net
            myNet->removeGLObjectFromGrid(this);
            // set shape end position
            setShapeEndPos(newShapeEnd);
            // add object from net
            myNet->addGLObjectIntoGrid(this);
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
        case GNE_ATTR_PARAMETERS:
            myNBEdge->setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEEdge::updateDottedContour() {
    // obtain lanes
    const GNELane* frontLane = myLanes.front();
    const GNELane* backLane = myLanes.back();
    // obtain visualization settings
    const GUIVisualizationSettings& visualizationSetting = myNet->getViewNet()->getVisualisationSettings();
    // obtain lane widdths
    const double myHalfLaneWidthFront = myNBEdge->getLaneWidth(frontLane->getIndex()) / 2;
    const double myHalfLaneWidthBack = (visualizationSetting.spreadSuperposed && backLane->drawAsRailway(visualizationSetting) &&
                                        myNBEdge->isBidiRail()) ? 0 : myNBEdge->getLaneWidth(backLane->getIndex()) / 2;
    // obtain shapes from NBEdge
    PositionVector mainShape = frontLane->getParentEdge()->getNBEdge()->getLaneShape(frontLane->getIndex());
    PositionVector backShape = backLane->getParentEdge()->getNBEdge()->getLaneShape(backLane->getIndex());
    // move to side depending of lefthand
    if (visualizationSetting.lefthand) {
        mainShape.move2side(myHalfLaneWidthFront * -1);
        backShape.move2side(myHalfLaneWidthBack);
    } else {
        mainShape.move2side(myHalfLaneWidthFront);
        backShape.move2side(myHalfLaneWidthBack * -1);
    }
    // reverse back shape
    backShape = backShape.reverse();
    // add back shape into mainShape
    for (const auto& position : backShape) {
        mainShape.push_back(position);
    }
    // close polygon
    mainShape.closePolygon();
    // update edge dotted geometry
    updateDottedGeometry(mainShape);
}


void
GNEEdge::setNumLanes(int numLanes, GNEUndoList* undoList) {
    // begin undo list
    undoList->p_begin("change number of " + toString(SUMO_TAG_LANE) +  "s");
    // invalidate logic of source/destiny edges
    myGNEJunctionSource->setLogicValid(false, undoList);
    myGNEJunctionDestiny->setLogicValid(false, undoList);
    // disable update geometry (see #6336)
    myUpdateGeometry = false;
    // remove edge of RTREE
    myNet->removeGLObjectFromGrid(this);
    const int oldNumLanes = (int)myLanes.size();
    for (int i = oldNumLanes; i < numLanes; i++) {
        // since the GNELane does not exist yet, it cannot have yet been referenced so we only pass a zero-pointer
        undoList->add(new GNEChange_Lane(this, myNBEdge->getLaneStruct(oldNumLanes - 1)), true);
    }
    for (int i = (oldNumLanes - 1); i > (numLanes - 1); i--) {
        // delete leftmost lane
        undoList->add(new GNEChange_Lane(this, myLanes[i], myNBEdge->getLaneStruct(i), false), true);
    }
    // enable updateGeometry again
    myUpdateGeometry = true;
    // insert edge in RTREE again
    myNet->addGLObjectIntoGrid(this);
    // update geometry of entire edge
    updateGeometry();
    // end undo list
    undoList->p_end();
}


void
GNEEdge::addLane(GNELane* lane, const NBEdge::Lane& laneAttrs, bool recomputeConnections) {
    // boundary of edge depends of number of lanes. We need to extract if before add or remove lane
    if (myUpdateGeometry) {
        myNet->removeGLObjectFromGrid(this);
    }
    const int index = lane ? lane->getIndex() : myNBEdge->getNumLanes();
    // the laneStruct must be created first to ensure we have some geometry
    // unless the connections are fully recomputed, existing indices must be shifted
    myNBEdge->addLane(index, true, recomputeConnections, !recomputeConnections);
    if (lane) {
        // restore a previously deleted lane
        myLanes.insert(myLanes.begin() + index, lane);

    } else {
        // create a new lane by copying leftmost lane
        lane = new GNELane(this, index);
        myLanes.push_back(lane);
    }
    lane->incRef("GNEEdge::addLane");
    // check if lane is selected
    if (lane->isAttributeCarrierSelected()) {
        lane->selectAttributeCarrier();
    }
    // we copy all attributes except shape since this is recomputed from edge shape
    myNBEdge->setSpeed(lane->getIndex(), laneAttrs.speed);
    myNBEdge->setPermissions(laneAttrs.permissions, lane->getIndex());
    myNBEdge->setPreferredVehicleClass(laneAttrs.preferred, lane->getIndex());
    myNBEdge->setEndOffset(lane->getIndex(), laneAttrs.endOffset);
    myNBEdge->setLaneWidth(lane->getIndex(), laneAttrs.width);
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
    if (myUpdateGeometry) {
        myNet->addGLObjectIntoGrid(this);
    }
    // Update geometry with the new lane
    updateGeometry();
}


void
GNEEdge::removeLane(GNELane* lane, bool recomputeConnections) {
    // boundary of edge depends of number of lanes. We need to extract if before add or remove lane
    if (myUpdateGeometry) {
        myNet->removeGLObjectFromGrid(this);
    }
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
    myNBEdge->deleteLane(lane->getIndex(), recomputeConnections, !recomputeConnections);
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
    if (myUpdateGeometry) {
        myNet->addGLObjectIntoGrid(this);
    }
    // Update element
    updateGeometry();
}


void
GNEEdge::addConnection(NBEdge::Connection nbCon, bool selectAfterCreation) {
    // If a new connection was sucesfully created
    if (myNBEdge->setConnection(nbCon.fromLane, nbCon.toEdge, nbCon.toLane, NBEdge::L2L_USER, true, nbCon.mayDefinitelyPass,
                                nbCon.keepClear, nbCon.contPos, nbCon.visibility,
                                nbCon.speed, nbCon.customLength, nbCon.customShape, nbCon.uncontrolled)) {
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
        for (auto i : con->getLaneFrom()->getChildAdditionals()) {
            if (i->getTagProperty().getTag() == SUMO_TAG_E2DETECTOR_MULTILANE) {
                dynamic_cast<GNEDetectorE2*>(i)->checkE2MultilaneIntegrity();
            }
        }
        // iterate over all additionals from "to" lane and check E2 multilane integrity
        for (auto i : con->getLaneTo()->getChildAdditionals()) {
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
    if (nbCon.toEdge == myNBEdge->getTurnDestination()) {
        myNet->removeExplicitTurnaround(getID());
    }
    // remove NBEdge::connection from NBEdge
    myNBEdge->removeFromConnections(nbCon);
    // remove their associated GNEConnection
    GNEConnection* con = retrieveGNEConnection(nbCon.fromLane, nbCon.toEdge, nbCon.toLane, false);
    if (con != nullptr) {
        con->decRef("GNEEdge::removeConnection");
        myGNEConnections.erase(std::find(myGNEConnections.begin(), myGNEConnections.end(), con));
        // iterate over all additionals from "from" lane and check E2 multilane integrity
        for (auto i : con->getLaneFrom()->getChildAdditionals()) {
            if (i->getTagProperty().getTag() == SUMO_TAG_E2DETECTOR_MULTILANE) {
                dynamic_cast<GNEDetectorE2*>(i)->checkE2MultilaneIntegrity();
            }
        }
        // iterate over all additionals from "to" lane and check E2 multilane integrity
        for (auto i : con->getLaneTo()->getChildAdditionals()) {
            if (i->getTagProperty().getTag() == SUMO_TAG_E2DETECTOR_MULTILANE) {
                dynamic_cast<GNEDetectorE2*>(i)->checkE2MultilaneIntegrity();
            }
        }
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
        for (auto i : createdConnection->getLaneFrom()->getChildAdditionals()) {
            if (i->getTagProperty().getTag() == SUMO_TAG_E2DETECTOR_MULTILANE) {
                dynamic_cast<GNEDetectorE2*>(i)->checkE2MultilaneIntegrity();
            }
        }
        // iterate over all additionals from "to" lane and check E2 multilane integrity
        for (auto i : createdConnection->getLaneTo()->getChildAdditionals()) {
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
    PositionVector modifiedShape = myNBEdge->getGeometry().interpolateZ(
                                       myNBEdge->getFromNode()->getPosition().z(),
                                       myNBEdge->getToNode()->getPosition().z());
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
                                       << " fromGeometryLike=" << myNBEdge->getFromNode()->geometryLike()
                                       << " toGeometryLike=" << myNBEdge->getToNode()->geometryLike()
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
    } else if (myNBEdge->getFromNode()->geometryLike() && myNBEdge->getToNode()->geometryLike()) {
        PositionVector begShape;
        PositionVector endShape;
        const EdgeVector& incoming = myNBEdge->getFromNode()->getIncomingEdges();
        const EdgeVector& outgoing = myNBEdge->getToNode()->getOutgoingEdges();
        if (incoming.size() == 1) {
            begShape = incoming[0]->getGeometry();
        } else {
            assert(incoming.size() == 2);
            begShape = myNBEdge->isTurningDirectionAt(incoming[0]) ? incoming[1]->getGeometry() : incoming[0]->getGeometry();
        }
        if (outgoing.size() == 1) {
            endShape = outgoing[0]->getGeometry();
        } else {
            assert(outgoing.size() == 2);
            endShape = myNBEdge->isTurningDirectionAt(outgoing[0]) ? outgoing[1]->getGeometry() : outgoing[0]->getGeometry();
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
    PositionVector modifiedShape = smoothShape(myNBEdge->getGeometry(), false);
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
    for (const Position& pos : myNBEdge->getGeometry()) {
        if (elevationBase.size() == 0 || elevationBase[-1].z() != pos.z()) {
            elevationBase.push_back(pos);
        }
    }
    PositionVector elevation = smoothShape(elevationBase, true);
    if (elevation.size() <= 2) {
        WRITE_WARNING("Could not compute smooth elevation for edge '" + getID() + "'");
    } else {
        PositionVector modifiedShape = myNBEdge->getGeometry();
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
GNEEdge::setShapeStartPos(const Position& pos) {
    // remove start position and add it the new position
    PositionVector geom = myNBEdge->getGeometry();
    geom.pop_front();
    geom.push_front(pos);
    // restore modified shape
    setGeometry(geom, false);
}


void
GNEEdge::setShapeEndPos(const Position& pos) {
    // remove end position and add it the new position
    PositionVector geom = myNBEdge->getGeometry();
    geom.pop_back();
    geom.push_back(pos);
    // restore modified shape
    setGeometry(geom, false);
}


const std::map<const GNELane*, std::vector<GNEDemandElement*> >
GNEEdge::getVehiclesOverEdgeMap() const {
    // declare vehicles over edge vector
    std::vector<GNEDemandElement*> vehiclesOverEdge;
    // declare solution map
    std::map<const GNELane*, std::vector<GNEDemandElement*> > vehiclesOverEdgeMap;
    // declare a set of vehicles (to avoid duplicates)
    std::set<std::pair<double, GNEDemandElement*> > vehicles;
    // first obtain all vehicles of this edge
    for (const auto& edgeChild : getChildDemandElements()) {
        if (((edgeChild->getTagProperty().getTag() == SUMO_TAG_TRIP) || (edgeChild->getTagProperty().getTag() == SUMO_TAG_FLOW)) &&
                (edgeChild->getParentEdges().front() == this)) {
            vehicles.insert(std::make_pair(edgeChild->getAttributeDouble(SUMO_ATTR_DEPART), edgeChild));
            vehicles.insert(std::make_pair(edgeChild->getAttributeDouble(SUMO_ATTR_DEPART), edgeChild));
        } else if ((edgeChild->getTagProperty().getTag() == SUMO_TAG_ROUTE) && (edgeChild->getParentEdges().front() == this)) {
            for (const auto& routeChild : edgeChild->getChildDemandElements()) {
                if ((routeChild->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (routeChild->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) {
                    vehicles.insert(std::make_pair(routeChild->getAttributeDouble(SUMO_ATTR_DEPART), routeChild));
                }
            }
        } else if ((edgeChild->getTagProperty().getTag() == SUMO_TAG_EMBEDDEDROUTE) && (edgeChild->getParentEdges().front() == this)) {
            vehicles.insert(std::make_pair(edgeChild->getParentDemandElements().front()->getAttributeDouble(SUMO_ATTR_DEPART), edgeChild->getParentDemandElements().front()));
        }
    }
    // reserve
    vehiclesOverEdge.reserve(vehicles.size());
    // iterate over vehicles
    for (const auto& vehicle : vehicles) {
        // add it over vehiclesOverEdge;
        vehiclesOverEdge.push_back(vehicle.second);
    }
    // now split vehicles by lanes
    for (const auto& vehicle : vehiclesOverEdge) {
        const GNELane* vehicleLane = vehicle->getFirstAllowedVehicleLane();
        if (vehicleLane) {
            vehiclesOverEdgeMap[vehicleLane].push_back(vehicle);
        }
    }
    return vehiclesOverEdgeMap;
}


void
GNEEdge::drawGeometryPoints(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double exaggeration = s.addSize.getExaggeration(s, this);
    // obtain circle width
    bool drawBig = (myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE ||
                    myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_DELETE);
    double circleWidth = drawBig ? SNAP_RADIUS * MIN2((double)1, s.laneWidthExaggeration) : 0.5;
    double circleWidthSquared = circleWidth * circleWidth;
    // obtain color
    RGBColor color = s.junctionColorer.getSchemes()[0].getColor(2);
    if (drawUsingSelectColor() && s.laneColorer.getActive() != 1) {
        // override with special colors (unless the color scheme is based on selection)
        color = s.colorSettings.selectedEdgeColor.changedBrightness(-20);
    }
    GLHelper::setColor(color);
    // recognize full transparency and simply don't draw
    if (color.alpha() > 0) {
        // push name
        glPushName(getGlID());
        // draw geometry points expect initial and final
        for (int i = 1; i < (int)myNBEdge->getGeometry().size() - 1; i++) {
            Position pos = myNBEdge->getGeometry()[i];
            if (!s.drawForRectangleSelection || (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(pos) <= (circleWidthSquared + 2))) {
                glPushMatrix();
                glTranslated(pos.x(), pos.y(), GLO_JUNCTION - 0.01);
                // resolution of drawn circle depending of the zoom (To improve smothness)
                GLHelper::drawFilledCircle(circleWidth, s.getCircleResolution());
                glPopMatrix();
                // draw elevation or special symbols (Start, End and Block)
                if (!s.drawForRectangleSelection && myNet->getViewNet()->getNetworkViewOptions().editingElevation()) {
                    glPushMatrix();
                    // Translate to geometry point
                    glTranslated(pos.x(), pos.y(), GLO_JUNCTION);
                    // draw Z value
                    GLHelper::drawText(toString(pos.z()), Position(), GLO_MAX - 5, s.edgeValue.scaledSize(s.scale) / 2, s.edgeValue.color);
                    glPopMatrix();
                }
            }
        }
        // draw line geometry, start and end points if shapeStart or shape end is edited, and depending of drawForRectangleSelection
        if (myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE) {
            if ((myNBEdge->getGeometry().front() != myGNEJunctionSource->getPositionInView()) &&
                    (!s.drawForRectangleSelection || (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(myNBEdge->getGeometry().front()) <= (circleWidthSquared + 2)))) {
                glPushMatrix();
                glTranslated(myNBEdge->getGeometry().front().x(), myNBEdge->getGeometry().front().y(), GLO_JUNCTION + 0.01);
                // resolution of drawn circle depending of the zoom (To improve smothness)
                GLHelper::drawFilledCircle(circleWidth, s.getCircleResolution());
                glPopMatrix();
                // draw a "s" over last point depending of drawForRectangleSelection
                if (!s.drawForRectangleSelection && s.drawDetail(s.detailSettings.geometryPointsText, exaggeration)) {
                    glPushMatrix();
                    glTranslated(myNBEdge->getGeometry().front().x(), myNBEdge->getGeometry().front().y(), GLO_JUNCTION + 0.02);
                    GLHelper::drawText("S", Position(), 0, circleWidth, RGBColor::WHITE);
                    glPopMatrix();
                    // draw line between Junction and point
                    glPushMatrix();
                    glTranslated(0, 0, GLO_JUNCTION - 0.01);
                    glLineWidth(4);
                    GLHelper::drawLine(myNBEdge->getGeometry().front(), myGNEJunctionSource->getPositionInView());
                    // draw line between begin point of last lane shape and the first edge shape point
                    GLHelper::drawLine(myNBEdge->getGeometry().front(), myNBEdge->getLanes().back().shape.front());
                    glPopMatrix();
                }
            }
            if ((myNBEdge->getGeometry().back() != myGNEJunctionDestiny->getPositionInView()) &&
                    (!s.drawForRectangleSelection || (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(myNBEdge->getGeometry().back()) <= (circleWidthSquared + 2)))) {
                glPushMatrix();
                glTranslated(myNBEdge->getGeometry().back().x(), myNBEdge->getGeometry().back().y(), GLO_JUNCTION + 0.01);
                // resolution of drawn circle depending of the zoom (To improve smothness)
                GLHelper::drawFilledCircle(circleWidth, s.getCircleResolution());
                glPopMatrix();
                // draw a "e" over last point depending of drawForRectangleSelection
                if (!s.drawForRectangleSelection && s.drawDetail(s.detailSettings.geometryPointsText, exaggeration)) {
                    glPushMatrix();
                    glTranslated(myNBEdge->getGeometry().back().x(), myNBEdge->getGeometry().back().y(), GLO_JUNCTION + 0.02);
                    GLHelper::drawText("E", Position(), 0, circleWidth, RGBColor::WHITE);
                    glPopMatrix();
                    // draw line between Junction and point
                    glPushMatrix();
                    glTranslated(0, 0, GLO_JUNCTION - 0.01);
                    glLineWidth(4);
                    GLHelper::drawLine(myNBEdge->getGeometry().back(), myGNEJunctionDestiny->getPositionInView());
                    // draw line between last point of first lane shape and the last edge shape point
                    GLHelper::drawLine(myNBEdge->getGeometry().back(), myNBEdge->getLanes().back().shape.back());
                    glPopMatrix();
                }
            }
        }
        // pop name
        glPopName();
    }
}


void
GNEEdge::drawEdgeName(const GUIVisualizationSettings& s) const {
    // draw the name and/or the street name
    const bool drawStreetName = s.streetName.show && (myNBEdge->getStreetName() != "");
    const bool spreadSuperposed = s.spreadSuperposed && myLanes.back()->drawAsRailway(s) && myNBEdge->isBidiRail();
    if (s.edgeName.show || drawStreetName || s.edgeValue.show) {
        glPushName(getGlID());
        GNELane* lane1 = myLanes[0];
        GNELane* lane2 = myLanes[myLanes.size() - 1];
        Position p = lane1->getLaneShape().positionAtOffset(lane1->getLaneShape().length() / (double) 2.);
        p.add(lane2->getLaneShape().positionAtOffset(lane2->getLaneShape().length() / (double) 2.));
        p.mul(.5);
        if (spreadSuperposed) {
            // move name to the right of the edge and towards its beginning
            const double dist = 0.6 * s.edgeName.scaledSize(s.scale);
            const double shiftA = lane1->getLaneShape().rotationAtOffset(lane1->getLaneShape().length() / (double) 2.) - DEG2RAD(135);
            Position shift(dist * cos(shiftA), dist * sin(shiftA));
            p.add(shift);
        }
        double angle = lane1->getLaneShape().rotationDegreeAtOffset(lane1->getLaneShape().length() / (double) 2.);
        angle += 90;
        if (angle > 90 && angle < 270) {
            angle -= 180;
        }
        if (s.edgeName.show) {
            drawName(p, s.scale, s.edgeName, angle);
        }
        if (drawStreetName) {
            GLHelper::drawTextSettings(s.streetName, myNBEdge->getStreetName(), p, s.scale, angle);
        }
        if (s.edgeValue.show) {
            const int activeScheme = s.laneColorer.getActive();
            std::string value;
            if (activeScheme == 12) {
                // edge param, could be non-numerical
                value = getNBEdge()->getParameter(s.edgeParam, "");
            } else if (activeScheme == 13) {
                // lane param, could be non-numerical
                value = getNBEdge()->getLaneStruct(lane2->getIndex()).getParameter(s.laneParam, "");
            } else {
                // use numerical value value of leftmost lane to hopefully avoid sidewalks, bikelanes etc
                const double doubleValue = lane2->getColorValue(s, activeScheme);
                const RGBColor color = s.laneColorer.getScheme().getColor(doubleValue);
                value = color.alpha() == 0 ? "" : toString(doubleValue);
            }
            if (value != "") {
                GLHelper::drawTextSettings(s.edgeValue, value, p, s.scale, angle);
            }
        }
        glPopName();
    }
}


void
GNEEdge::drawRerouterSymbol(const GUIVisualizationSettings& s, GNEAdditional* rerouter) const {
    // Obtain exaggeration of the draw
    const double exaggeration = s.addSize.getExaggeration(s, rerouter);
    // first check if additional has to be drawn
    if (s.drawAdditionals(exaggeration) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // Start drawing adding an gl identificator
        glPushName(rerouter->getGlID());
        // draw rerouter symbol over all lanes
        for (const auto& j : myLanes) {
            const Position& lanePos = rerouter->getChildPosition(j);
            const double laneRot = rerouter->getChildRotation(j);
            // draw rerouter symbol
            glPushMatrix();
            glTranslated(lanePos.x(), lanePos.y(), rerouter->getType());
            glRotated(-1 * laneRot, 0, 0, 1);
            glScaled(exaggeration, exaggeration, 1);
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
            if (!s.drawForRectangleSelection) {
                GLHelper::drawText("U", Position(0, 2), .1, 3, RGBColor::BLACK, 180);
                double probability = parse<double>(rerouter->getAttribute(SUMO_ATTR_PROB)) * 100;
                // draw Probability
                GLHelper::drawText((toString(probability) + "%").c_str(), Position(0, 4), .1, 0.7, RGBColor::BLACK, 180);
            }
            // finish draw
            glPopMatrix();
            // draw contour if is selected
            if (myNet->getViewNet()->getDottedAC() == rerouter) {
                GLHelper::drawShapeDottedContourRectangle(s, getType(), lanePos, 2.8, 6, -1 * laneRot, 0, 3);
            }
        }
        // Pop name
        glPopName();
        // Draw connections
        rerouter->drawChildConnections(s, getType());
    }
}


void
GNEEdge::drawDemandElements(const GUIVisualizationSettings& s) const {
    // certain demand elements children can contain loops (for example, routes) and it causes overlapping problems. It's needed to filter it before drawing
    if (s.drawForPositionSelection) {
        // draw routes
        for (const auto& route : getChildDemandElementsByType(SUMO_TAG_ROUTE)) {
            // first check if route can be drawn
            if (myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(route)) {
                // draw partial route
                drawPartialRoute(s, route, nullptr);
            }
        }
        // draw embedded routes
        for (const auto& embeddedRoute : getChildDemandElementsByType(SUMO_TAG_EMBEDDEDROUTE)) {
            // first check if embedded route can be drawn
            if (myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(embeddedRoute)) {
                // draw partial route
                drawPartialRoute(s, embeddedRoute, nullptr);
            }
        }
    } else {
        // if drawForPositionSelection is disabled, only draw the first element
        if (getChildDemandElementsByType(SUMO_TAG_ROUTE).size() > 0) {
            const auto& route = getChildDemandElementsByType(SUMO_TAG_ROUTE).front();
            if (myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(route)) {
                drawPartialRoute(s, route, nullptr);
            }
        }
        if (getChildDemandElementsByType(SUMO_TAG_EMBEDDEDROUTE).size() > 0) {
            const auto& embeddedRoute = getChildDemandElementsByType(SUMO_TAG_EMBEDDEDROUTE).front();
            if (myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(embeddedRoute)) {
                drawPartialRoute(s, embeddedRoute, nullptr);
            }
        }
    }
    // draw person plans
    if (s.drawForPositionSelection) {
        for (const auto& personTripFromTo : getChildDemandElementsByType(SUMO_TAG_PERSONTRIP_FROMTO)) {
            drawPartialPersonPlan(s, personTripFromTo, nullptr);
        }
        for (const auto& personTripBusStop : getChildDemandElementsByType(SUMO_TAG_PERSONTRIP_BUSSTOP)) {
            drawPartialPersonPlan(s, personTripBusStop, nullptr);
        }
        for (const auto& walkEdges : getChildDemandElementsByType(SUMO_TAG_WALK_EDGES)) {
            drawPartialPersonPlan(s, walkEdges, nullptr);
        }
        for (const auto& walkFromTo : getChildDemandElementsByType(SUMO_TAG_WALK_FROMTO)) {
            drawPartialPersonPlan(s, walkFromTo, nullptr);
        }
        for (const auto& walkBusStop : getChildDemandElementsByType(SUMO_TAG_WALK_BUSSTOP)) {
            drawPartialPersonPlan(s, walkBusStop, nullptr);
        }
        for (const auto& walkRoute : getChildDemandElementsByType(SUMO_TAG_WALK_ROUTE)) {
            drawPartialPersonPlan(s, walkRoute, nullptr);
        }
        for (const auto& rideFromTo : getChildDemandElementsByType(SUMO_TAG_RIDE_FROMTO)) {
            drawPartialPersonPlan(s, rideFromTo, nullptr);
        }
        for (const auto& rideBusStop : getChildDemandElementsByType(SUMO_TAG_RIDE_BUSSTOP)) {
            drawPartialPersonPlan(s, rideBusStop, nullptr);
        }
    } else {
        // if drawForPositionSelection is disabled, only draw the first element
        if (getChildDemandElementsByType(SUMO_TAG_PERSONTRIP_FROMTO).size() > 0) {
            drawPartialPersonPlan(s, getChildDemandElementsByType(SUMO_TAG_PERSONTRIP_FROMTO).front(), nullptr);
        }
        if (getChildDemandElementsByType(SUMO_TAG_PERSONTRIP_BUSSTOP).size() > 0) {
            drawPartialPersonPlan(s, getChildDemandElementsByType(SUMO_TAG_PERSONTRIP_BUSSTOP).front(), nullptr);
        }
        if (getChildDemandElementsByType(SUMO_TAG_WALK_EDGES).size() > 0) {
            drawPartialPersonPlan(s, getChildDemandElementsByType(SUMO_TAG_WALK_EDGES).front(), nullptr);
        }
        if (getChildDemandElementsByType(SUMO_TAG_WALK_FROMTO).size() > 0) {
            drawPartialPersonPlan(s, getChildDemandElementsByType(SUMO_TAG_WALK_FROMTO).front(), nullptr);
        }
        if (getChildDemandElementsByType(SUMO_TAG_WALK_BUSSTOP).size() > 0) {
            drawPartialPersonPlan(s, getChildDemandElementsByType(SUMO_TAG_WALK_BUSSTOP).front(), nullptr);
        }
        if (getChildDemandElementsByType(SUMO_TAG_WALK_ROUTE).size() > 0) {
            drawPartialPersonPlan(s, getChildDemandElementsByType(SUMO_TAG_WALK_ROUTE).front(), nullptr);
        }
        if (getChildDemandElementsByType(SUMO_TAG_RIDE_FROMTO).size() > 0) {
            drawPartialPersonPlan(s, getChildDemandElementsByType(SUMO_TAG_RIDE_FROMTO).front(), nullptr);
        }
        if (getChildDemandElementsByType(SUMO_TAG_RIDE_BUSSTOP).size() > 0) {
            drawPartialPersonPlan(s, getChildDemandElementsByType(SUMO_TAG_RIDE_BUSSTOP).front(), nullptr);
        }
    }
    // draw path element childs
    for (const auto& elementChild : myPathDemandElementsElementChilds) {
        if (elementChild->getTagProperty().isVehicle()) {
            // draw partial trip only if is being inspected or selected (and we aren't in draw for selecting mode)
            if (!s.drawForRectangleSelection && ((myNet->getViewNet()->getDottedAC() == elementChild) || elementChild->isAttributeCarrierSelected())) {
                drawPartialTripFromTo(s, elementChild, nullptr);
            }
        } else if (elementChild->getTagProperty().isPersonPlan()) {
            drawPartialPersonPlan(s, elementChild, nullptr);
        }
    }
}


bool
GNEEdge::areStackPositionOverlapped(const GNEEdge::StackPosition& vehicleA, const GNEEdge::StackPosition& vehicleB) const {
    if ((vehicleA.beginPosition() == vehicleB.beginPosition()) && (vehicleA.endPosition() == vehicleB.endPosition())) {
        return true;
    } else if ((vehicleA.beginPosition() < vehicleB.beginPosition()) && (vehicleA.endPosition() > vehicleB.endPosition())) {
        return true;
    } else if ((vehicleA.beginPosition() < vehicleB.beginPosition()) && (vehicleA.endPosition() > vehicleB.beginPosition())) {
        return true;
    } else if ((vehicleA.beginPosition() < vehicleB.endPosition()) && (vehicleA.endPosition() > vehicleB.endPosition())) {
        return true;
    } else {
        return false;
    }
}


/****************************************************************************/
