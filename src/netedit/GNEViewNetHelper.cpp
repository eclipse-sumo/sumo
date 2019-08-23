/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEViewNetHelper.cpp
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2019
/// @version $Id$
///
// A file used to reduce the size of GNEViewNet.h grouping structs and classes
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include <netedit/additionals/GNEPOI.h>
#include <netedit/additionals/GNEPoly.h>
#include <netedit/additionals/GNETAZ.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/frames/GNETLSEditorFrame.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/options/OptionsCont.h>

#include "GNEViewNetHelper.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEUndoList.h"
#include "GNEViewParent.h"
#include "GNEApplicationWindow.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEViewNetHelper::ObjectsUnderCursor - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::ObjectsUnderCursor::ObjectsUnderCursor() {}


void
GNEViewNetHelper::ObjectsUnderCursor::updateObjectUnderCursor(const std::vector<GUIGlObject*>& GUIGlObjects, GNEPoly* editedPolyShape) {
    // first clear all containers
    myAttributeCarriers.clear();
    myNetElements.clear();
    myAdditionals.clear();
    myShapes.clear();
    myDemandElements.clear();
    myJunctions.clear();
    myEdges.clear();
    myLanes.clear();
    myCrossings.clear();
    myConnections.clear();
    myTAZs.clear();
    myPOIs.clear();
    myPolys.clear();
    // set GUIGlObject
    sortGUIGlObjectsByAltitude(GUIGlObjects);
    // iterate over GUIGlObjects
    for (const auto& i : myGUIGlObjects) {
        // only continue if isn't GLO_NETELEMENT (0)
        if (i->getType() != GLO_NETELEMENT) {
            // cast attribute carrier from glObject
            myAttributeCarriers.push_back(dynamic_cast<GNEAttributeCarrier*>(i));
            // only continue if attributeCarrier isn't nullptr;
            if (myAttributeCarriers.back()) {
                // If we're editing a shape, ignore rest of elements (including other polygons)
                if (editedPolyShape != nullptr) {
                    if (myAttributeCarriers.back() == editedPolyShape) {
                        // cast Poly from attribute carrier
                        myPolys.push_back(dynamic_cast<GNEPoly*>(myAttributeCarriers.back()));
                    }
                } else {
                    // obtain tag property (only for improve code legibility)
                    const auto& tagValue = myAttributeCarriers.back()->getTagProperty();
                    // check if attributeCarrier can be casted into netElement, additional or shape
                    if (tagValue.isNetElement()) {
                        // cast netElement from attribute carrier
                        myNetElements.push_back(dynamic_cast<GNENetElement*>(myAttributeCarriers.back()));
                    } else if (tagValue.isDemandElement()) {
                        // cast demand element from attribute carrier
                        myDemandElements.push_back(dynamic_cast<GNEDemandElement*>(myAttributeCarriers.back()));
                    } else if (tagValue.isAdditional()) {
                        // cast additional element from attribute carrier
                        myAdditionals.push_back(dynamic_cast<GNEAdditional*>(myAttributeCarriers.back()));
                    } else if (tagValue.isShape()) {
                        // cast shape element from attribute carrier
                        myShapes.push_back(dynamic_cast<GNEShape*>(myAttributeCarriers.back()));
                    } else if (tagValue.isTAZ()) {
                        // cast TAZ element from attribute carrier
                        myTAZs.push_back(dynamic_cast<GNETAZ*>(myAttributeCarriers.back()));
                    }
                    // now set specify AC type
                    switch (i->getType()) {
                        case GLO_JUNCTION:
                            myJunctions.push_back(dynamic_cast<GNEJunction*>(myAttributeCarriers.back()));
                            break;
                        case GLO_EDGE: {
                            // fisrt obtain Edge
                            GNEEdge* edge = dynamic_cast<GNEEdge*>(myAttributeCarriers.back());
                            // check if edge parent is already inserted in myEdges (for example, due clicking over Geometry Points)
                            if (std::find(myEdges.begin(), myEdges.end(), edge) == myEdges.end()) {
                                myEdges.push_back(edge);
                            }
                            break;
                        }
                        case GLO_LANE: {
                            myLanes.push_back(dynamic_cast<GNELane*>(myAttributeCarriers.back()));
                            // check if edge's lane parent is already inserted in myEdges (for example, due clicking over Geometry Points)
                            if (std::find(myEdges.begin(), myEdges.end(), &myLanes.back()->getParentEdge()) == myEdges.end()) {
                                myEdges.push_back(&myLanes.back()->getParentEdge());
                            }
                            break;
                        }
                        case GLO_CROSSING:
                            myCrossings.push_back(dynamic_cast<GNECrossing*>(myAttributeCarriers.back()));
                            break;
                        case GLO_CONNECTION:
                            myConnections.push_back(dynamic_cast<GNEConnection*>(myAttributeCarriers.back()));
                            break;
                        case GLO_POI:
                            myPOIs.push_back(dynamic_cast<GNEPOI*>(myAttributeCarriers.back()));
                            break;
                        case GLO_POLYGON:
                            myPolys.push_back(dynamic_cast<GNEPoly*>(myAttributeCarriers.back()));
                            break;
                        default:
                            break;
                    }
                }
            } else {
                myAttributeCarriers.pop_back();
            }
        }
    }
    // write information in debug mode
    WRITE_DEBUG("ObjectsUnderCursor: GUIGlObjects: " + toString(GUIGlObjects.size()) +
                ", AttributeCarriers: " + toString(myAttributeCarriers.size()) +
                ", NetElements: " + toString(myNetElements.size()) +
                ", Additionals: " + toString(myAdditionals.size()) +
                ", DemandElements: " + toString(myDemandElements.size()) +
                ", Shapes: " + toString(myShapes.size()) +
                ", Junctions: " + toString(myJunctions.size()) +
                ", Edges: " + toString(myEdges.size()) +
                ", Lanes: " + toString(myLanes.size()) +
                ", Crossings: " + toString(myCrossings.size()) +
                ", Connections: " + toString(myConnections.size()) +
                ", TAZs: " + toString(myTAZs.size()) +
                ", POIs: " + toString(myPOIs.size()) +
                ", Polys: " + toString(myPolys.size()));
}


void
GNEViewNetHelper::ObjectsUnderCursor::swapLane2Edge() {
    // clear some containers
    myGUIGlObjects.clear();
    myAttributeCarriers.clear();
    myNetElements.clear();
    // fill containers using edges
    for (const auto& i : myEdges) {
        myGUIGlObjects.push_back(i);
        myAttributeCarriers.push_back(i);
        myNetElements.push_back(i);
    }
    // write information for debug
    WRITE_DEBUG("ObjectsUnderCursor: swapped Lanes to edges")
}


void
GNEViewNetHelper::ObjectsUnderCursor::setCreatedJunction(GNEJunction* junction) {
    if (myJunctions.size() > 0) {
        myJunctions.front() = junction;
    } else {
        myJunctions.push_back(junction);
    }
}


GUIGlID
GNEViewNetHelper::ObjectsUnderCursor::getGlIDFront() const {
    if (myGUIGlObjects.size() > 0) {
        return myGUIGlObjects.front()->getGlID();
    } else {
        return 0;
    }
}


GUIGlObjectType
GNEViewNetHelper::ObjectsUnderCursor::getGlTypeFront() const {
    if (myGUIGlObjects.size() > 0) {
        return myGUIGlObjects.front()->getType();
    } else {
        return GLO_NETWORK;
    }
}


GNEAttributeCarrier*
GNEViewNetHelper::ObjectsUnderCursor::getAttributeCarrierFront() const {
    if (myAttributeCarriers.size() > 0) {
        return myAttributeCarriers.front();
    } else {
        return nullptr;
    }
}


GNENetElement*
GNEViewNetHelper::ObjectsUnderCursor::getNetElementFront() const {
    if (myNetElements.size() > 0) {
        return myNetElements.front();
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNEViewNetHelper::ObjectsUnderCursor::getAdditionalFront() const {
    if (myAdditionals.size() > 0) {
        return myAdditionals.front();
    } else {
        return nullptr;
    }
}


GNEShape*
GNEViewNetHelper::ObjectsUnderCursor::getShapeFront() const {
    if (myShapes.size() > 0) {
        return myShapes.front();
    } else {
        return nullptr;
    }
}


GNEDemandElement*
GNEViewNetHelper::ObjectsUnderCursor::getDemandElementFront() const {
    if (myDemandElements.size() > 0) {
        return myDemandElements.front();
    } else {
        return nullptr;
    }
}


GNEJunction*
GNEViewNetHelper::ObjectsUnderCursor::getJunctionFront() const {
    if (myJunctions.size() > 0) {
        return myJunctions.front();
    } else {
        return nullptr;
    }
}


GNEEdge*
GNEViewNetHelper::ObjectsUnderCursor::getEdgeFront() const {
    if (myEdges.size() > 0) {
        return myEdges.front();
    } else {
        return nullptr;
    }
}


GNELane*
GNEViewNetHelper::ObjectsUnderCursor::getLaneFront() const {
    if (myLanes.size() > 0) {
        return myLanes.front();
    } else {
        return nullptr;
    }
}


GNECrossing*
GNEViewNetHelper::ObjectsUnderCursor::getCrossingFront() const {
    if (myCrossings.size() > 0) {
        return myCrossings.front();
    } else {
        return nullptr;
    }
}


GNEConnection*
GNEViewNetHelper::ObjectsUnderCursor::getConnectionFront() const {
    if (myConnections.size() > 0) {
        return myConnections.front();
    } else {
        return nullptr;
    }
}


GNETAZ*
GNEViewNetHelper::ObjectsUnderCursor::getTAZFront() const {
    if (myTAZs.size() > 0) {
        return myTAZs.front();
    } else {
        return nullptr;
    }
}


GNEPOI*
GNEViewNetHelper::ObjectsUnderCursor::getPOIFront() const {
    if (myPOIs.size() > 0) {
        return myPOIs.front();
    } else {
        return nullptr;
    }
}


GNEPoly*
GNEViewNetHelper::ObjectsUnderCursor::getPolyFront() const {
    if (myPolys.size() > 0) {
        return myPolys.front();
    } else {
        return nullptr;
    }
}


const std::vector<GNEAttributeCarrier*>&
GNEViewNetHelper::ObjectsUnderCursor::getClickedAttributeCarriers() const {
    return myAttributeCarriers;
}


void
GNEViewNetHelper::ObjectsUnderCursor::sortGUIGlObjectsByAltitude(const std::vector<GUIGlObject*>& GUIGlObjects) {
    // first clear myGUIGlObjects
    myGUIGlObjects.clear();
    // declare a map to save sorted GUIGlObjects
    std::map<GUIGlObjectType, std::vector<GUIGlObject*> > mySortedGUIGlObjects;
    for (const auto& i : GUIGlObjects) {
        mySortedGUIGlObjects[i->getType()].push_back(i);
    }
    // move sorted GUIGlObjects into myGUIGlObjects using a reverse iterator
    for (std::map<GUIGlObjectType, std::vector<GUIGlObject*> >::reverse_iterator i = mySortedGUIGlObjects.rbegin(); i != mySortedGUIGlObjects.rend(); i++) {
        for (const auto& j : i->second) {
            myGUIGlObjects.push_back(j);
        }
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::keyPressed - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::KeyPressed::KeyPressed() :
    myEventInfo(nullptr) {
}


void
GNEViewNetHelper::KeyPressed::update(void* eventData) {
    myEventInfo = (FXEvent*) eventData;
}


bool
GNEViewNetHelper::KeyPressed::shiftKeyPressed() const {
    if (myEventInfo) {
        return (myEventInfo->state & SHIFTMASK) != 0;
    } else {
        return false;
    }
}


bool
GNEViewNetHelper::KeyPressed::controlKeyPressed() const {
    if (myEventInfo) {
        return (myEventInfo->state & CONTROLMASK) != 0;
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::MoveSingleElementValues - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::MoveSingleElementValues::MoveSingleElementValues(GNEViewNet* viewNet) :
    movingIndexShape(-1),
    myViewNet(viewNet),
    myMovingStartPos(false),
    myMovingEndPos(false),
    myJunctionToMove(nullptr),
    myEdgeToMove(nullptr),
    myPolyToMove(nullptr),
    myPOIToMove(nullptr),
    myAdditionalToMove(nullptr),
    myDemandElementToMove(nullptr),
    myTAZToMove(nullptr) {
}


bool
GNEViewNetHelper::MoveSingleElementValues::beginMoveSingleElementNetworkMode() {
    // first obtain moving reference (common for all)
    myRelativeClickedPosition = myViewNet->getPositionInformation();
    // check what type of AC will be moved
    if (myViewNet->myObjectsUnderCursor.getPolyFront()) {
        // calculate poly movement values (can be entire shape, single geometry points, altitude, etc.)
        return calculatePolyValues();
    } else if (myViewNet->myObjectsUnderCursor.getPOIFront()) {
        // set POI moved object
        myPOIToMove = myViewNet->myObjectsUnderCursor.getPOIFront();
        // Save original Position of POI in view
        originalPositionInView = myPOIToMove->getPositionInView();
        // there is moved items, then return true
        return true;
    } else if (myViewNet->myObjectsUnderCursor.getAdditionalFront()) {
        // set additionals moved object
        myAdditionalToMove = myViewNet->myObjectsUnderCursor.getAdditionalFront();
        // save current position of additional
        originalPositionInView = myAdditionalToMove->getPositionInView();
        // start additional geometry moving
        myAdditionalToMove->startGeometryMoving();
        // there is moved items, then return true
        return true;
    } else if (myViewNet->myObjectsUnderCursor.getTAZFront()) {
        // calculate TAZ movement values (can be entire shape or single geometry points)
        return calculateTAZValues();
    } else if (myViewNet->myObjectsUnderCursor.getJunctionFront()) {
        // set junction moved object
        myJunctionToMove = myViewNet->myObjectsUnderCursor.getJunctionFront();
        // Save original Position of Element in view
        originalPositionInView = myJunctionToMove->getPositionInView();
        // start junction geometry moving
        myJunctionToMove->startGeometryMoving();
        // there is moved items, then return true
        return true;
    } else if (myViewNet->myObjectsUnderCursor.getEdgeFront() || myViewNet->myObjectsUnderCursor.getLaneFront()) {
        // calculate Edge movement values (can be entire shape, single geometry points, altitude, etc.)
        return calculateEdgeValues();
    } else {
        // there isn't moved items, then return false
        return false;
    }
}


bool
GNEViewNetHelper::MoveSingleElementValues::beginMoveSingleElementDemandMode() {
    // first obtain moving reference (common for all)
    myRelativeClickedPosition = myViewNet->getPositionInformation();
    // check what type of AC will be moved
    if (myViewNet->myObjectsUnderCursor.getDemandElementFront()) {
        // set additionals moved object
        myDemandElementToMove = myViewNet->myObjectsUnderCursor.getDemandElementFront();
        // save current position of demand element
        originalPositionInView = myDemandElementToMove->getPositionInView();
        // start demand element geometry moving
        myDemandElementToMove->startGeometryMoving();
        // there is moved items, then return true
        return true;
    } else {
        // there isn't moved items, then return false
        return false;
    }
}


void
GNEViewNetHelper::MoveSingleElementValues::moveSingleElement() {
    // calculate offsetMovement depending of current mouse position and relative clicked position
    // @note  #3521: Add checkBox to allow moving elements... has to be implemented and used here
    Position offsetMovement = myViewNet->getPositionInformation() - myViewNet->myMoveSingleElementValues.myRelativeClickedPosition;
    // calculate Z depending of moveElevation
    if (myViewNet->myNetworkViewOptions.menuCheckMoveElevation->shown() && myViewNet->myNetworkViewOptions.menuCheckMoveElevation->getCheck() == TRUE) {
        // reset offset X and Y and use Y for Z
        offsetMovement = Position(0, 0, offsetMovement.y());
    } else {
        // leave z empty (because in this case offset only actuates over X-Y)
        offsetMovement.setz(0);
    }
    // check what element will be moved
    if (myPolyToMove) {
        // move shape's geometry without commiting changes depending if polygon is blocked
        if (myPolyToMove->isPolygonBlocked()) {
            // move entire shape
            myPolyToMove->moveEntireShape(originalShapeBeforeMoving, offsetMovement);
        } else {
            // move only a certain Geometry Point
            movingIndexShape = myPolyToMove->moveVertexShape(movingIndexShape, originalPositionInView, offsetMovement);
        }
    } else if (myPOIToMove) {
        // Move POI's geometry without commiting changes
        myPOIToMove->moveGeometry(originalPositionInView, offsetMovement);
    } else if (myJunctionToMove) {
        // Move Junction's geometry without commiting changes
        myJunctionToMove->moveGeometry(originalPositionInView, offsetMovement);
    } else if (myEdgeToMove) {
        // check if we're moving the start or end position, or a geometry point
        if (myMovingStartPos) {
            myEdgeToMove->moveShapeStart(originalPositionInView, offsetMovement);
        } else if (myMovingEndPos) {
            myEdgeToMove->moveShapeEnd(originalPositionInView, offsetMovement);
        } else {
            // move edge's geometry without commiting changes
            movingIndexShape = myEdgeToMove->moveVertexShape(movingIndexShape, originalPositionInView, offsetMovement);
        }
    } else if (myAdditionalToMove && (myAdditionalToMove->isAdditionalBlocked() == false)) {
        // Move Additional geometry without commiting changes
        myAdditionalToMove->moveGeometry(offsetMovement);
    } else if (myDemandElementToMove/* && (myDemandElementToMove->isDemandElementBlocked() == false)*/) {
        // Move DemandElement geometry without commiting changes
        myDemandElementToMove->moveGeometry(offsetMovement);
    } else if (myTAZToMove) {
        /// move TAZ's geometry without commiting changes depending if polygon is blocked
        if (myTAZToMove->isShapeBlocked()) {
            // move entire shape
            myTAZToMove->moveEntireShape(originalShapeBeforeMoving, offsetMovement);
        } else {
            // move only a certain Geometry Point
            movingIndexShape = myTAZToMove->moveVertexShape(movingIndexShape, originalPositionInView, offsetMovement);
        }
    }
    // update view (needed to see the movement)
    myViewNet->update();
}


void
GNEViewNetHelper::MoveSingleElementValues::finishMoveSingleElement() {
    if (myPolyToMove) {
        myPolyToMove->commitShapeChange(originalShapeBeforeMoving, myViewNet->getUndoList());
        myPolyToMove = nullptr;
    } else if (myPOIToMove) {
        myPOIToMove->commitGeometryMoving(originalPositionInView, myViewNet->getUndoList());
        myPOIToMove = nullptr;
    } else if (myJunctionToMove) {
        // check if in the moved position there is another Junction and it will be merged
        if (!myViewNet->mergeJunctions(myJunctionToMove, originalPositionInView)) {
            myJunctionToMove->commitGeometryMoving(originalPositionInView, myViewNet->getUndoList());
        }
        myJunctionToMove = nullptr;
    } else if (myEdgeToMove) {
        // commit change depending of what was moved
        if (myMovingStartPos) {
            myEdgeToMove->commitShapeStartChange(originalPositionInView, myViewNet->getUndoList());
            myMovingStartPos = false;
        } else if (myMovingEndPos) {
            myEdgeToMove->commitShapeEndChange(originalPositionInView, myViewNet->getUndoList());
            myMovingEndPos = false;
        } else {
            myEdgeToMove->commitShapeChange(originalShapeBeforeMoving, myViewNet->getUndoList());
        }
        myEdgeToMove = nullptr;
    } else if (myAdditionalToMove) {
        myAdditionalToMove->commitGeometryMoving(myViewNet->getUndoList());
        myAdditionalToMove->endGeometryMoving();
        myAdditionalToMove = nullptr;
    } else if (myDemandElementToMove) {
        myDemandElementToMove->commitGeometryMoving(myViewNet->getUndoList());
        myDemandElementToMove->endGeometryMoving();
        myDemandElementToMove = nullptr;
    } else if (myTAZToMove) {
        myTAZToMove->commitShapeChange(originalShapeBeforeMoving, myViewNet->getUndoList());
        myTAZToMove = nullptr;
    }
}


bool
GNEViewNetHelper::MoveSingleElementValues::calculatePolyValues() {
    // set Poly to move
    myPolyToMove = myViewNet->myObjectsUnderCursor.getPolyFront();
    // now we have two cases: if we're editing the X-Y coordenade or the altitude (z)
    if (myViewNet->myNetworkViewOptions.menuCheckMoveElevation->shown() && myViewNet->myNetworkViewOptions.menuCheckMoveElevation->getCheck() == TRUE) {
        // check if in the clicked position a geometry point exist
        int existentIndex = myPolyToMove->getVertexIndex(myViewNet->getPositionInformation(), false, false);
        if (existentIndex != -1) {
            // save original shape (needed for commit change)
            myViewNet->myMoveSingleElementValues.originalShapeBeforeMoving = myPolyToMove->getShape();
            // obtain existent index
            myViewNet->myMoveSingleElementValues.movingIndexShape = existentIndex;
            myViewNet->myMoveSingleElementValues.originalPositionInView = myPolyToMove->getShape()[existentIndex];
            // poly values sucesfully calculated, then return true
            return true;
        } else {
            // stop poly moving
            myPolyToMove = nullptr;
            // poly values wasn't calculated, then return false
            return false;
        }
    } else {
        // save original shape (needed for commit change)
        myViewNet->myMoveSingleElementValues.originalShapeBeforeMoving = myPolyToMove->getShape();
        // save clicked position as moving original position
        myViewNet->myMoveSingleElementValues.originalPositionInView = myViewNet->getPositionInformation();
        // obtain index of vertex to move if shape isn't blocked
        if ((myPolyToMove->isPolygonBlocked() == false) && (myPolyToMove->isMovementBlocked() == false)) {
            // check if we want to remove a Geometry Point
            if (myViewNet->myKeyPressed.shiftKeyPressed()) {
                // check if we're clicked over a Geometry Point
                myViewNet->myMoveSingleElementValues.movingIndexShape = myPolyToMove->getVertexIndex(myViewNet->myMoveSingleElementValues.originalPositionInView, false, false);
                if (myViewNet->myMoveSingleElementValues.movingIndexShape != -1) {
                    myPolyToMove->deleteGeometryPoint(myViewNet->myMoveSingleElementValues.originalPositionInView);
                    // after removing Geomtery Point, reset PolyToMove
                    myPolyToMove = nullptr;
                    // poly values wasn't calculated, then return false
                    return false;
                }
                // poly values sucesfully calculated, then return true
                return true;
            } else {
                // obtain index of vertex to move and moving reference
                myViewNet->myMoveSingleElementValues.movingIndexShape = myPolyToMove->getVertexIndex(myViewNet->myMoveSingleElementValues.originalPositionInView, false, false);
                if (myViewNet->myMoveSingleElementValues.movingIndexShape == -1) {
                    // create new geometry point
                    myViewNet->myMoveSingleElementValues.movingIndexShape = myPolyToMove->getVertexIndex(myViewNet->myMoveSingleElementValues.originalPositionInView, true, true);
                }
                // poly values sucesfully calculated, then return true
                return true;
            }
        } else {
            myViewNet->myMoveSingleElementValues.movingIndexShape = -1;
            // poly values wasn't calculated, then return false
            return false;
        }
    }
}


bool
GNEViewNetHelper::MoveSingleElementValues::calculateEdgeValues() {
    if (myViewNet->myKeyPressed.shiftKeyPressed()) {
        // edit end point
        myViewNet->myObjectsUnderCursor.getEdgeFront()->editEndpoint(myViewNet->getPositionInformation(), myViewNet->myUndoList);
        // edge values wasn't calculated, then return false
        return false;
    } else {
        // assign clicked edge to edgeToMove
        myEdgeToMove = myViewNet->myObjectsUnderCursor.getEdgeFront();
        // check if we clicked over a start or end position
        if (myEdgeToMove->clickedOverShapeStart(myViewNet->getPositionInformation())) {
            // save start pos
            myViewNet->myMoveSingleElementValues.originalPositionInView = myEdgeToMove->getNBEdge()->getGeometry().front();
            myViewNet->myMoveSingleElementValues.myMovingStartPos = true;
            // start geometry moving
            myEdgeToMove->startGeometryMoving();
            // edge values sucesfully calculated, then return true
            return true;
        } else if (myEdgeToMove->clickedOverShapeEnd(myViewNet->getPositionInformation())) {
            // save end pos
            myViewNet->myMoveSingleElementValues.originalPositionInView = myEdgeToMove->getNBEdge()->getGeometry().back();
            myViewNet->myMoveSingleElementValues.myMovingEndPos = true;
            // start geometry moving
            myEdgeToMove->startGeometryMoving();
            // edge values sucesfully calculated, then return true
            return true;
        } else {
            // now we have two cases: if we're editing the X-Y coordenade or the altitude (z)
            if (myViewNet->myNetworkViewOptions.menuCheckMoveElevation->shown() && myViewNet->myNetworkViewOptions.menuCheckMoveElevation->getCheck() == TRUE) {
                // check if in the clicked position a geometry point exist
                int existentIndex = myEdgeToMove->getVertexIndex(myViewNet->getPositionInformation(), false, false);
                if (existentIndex != -1) {
                    myViewNet->myMoveSingleElementValues.movingIndexShape = existentIndex;
                    myViewNet->myMoveSingleElementValues.originalPositionInView = myEdgeToMove->getNBEdge()->getInnerGeometry()[existentIndex];
                    // start geometry moving
                    myEdgeToMove->startGeometryMoving();
                    // edge values sucesfully calculated, then return true
                    return true;
                } else {
                    // stop edge moving
                    myEdgeToMove = nullptr;
                    // edge values wasn't calculated, then return false
                    return false;
                }
            } else {
                // save original shape (needed for commit change)
                myViewNet->myMoveSingleElementValues.originalShapeBeforeMoving = myEdgeToMove->getNBEdge()->getInnerGeometry();
                // obtain index of vertex to move and moving reference
                myViewNet->myMoveSingleElementValues.movingIndexShape = myEdgeToMove->getVertexIndex(myViewNet->getPositionInformation(), false, false);
                // if index doesn't exist, create it snapping new edge to grid
                if (myViewNet->myMoveSingleElementValues.movingIndexShape == -1) {
                    myViewNet->myMoveSingleElementValues.movingIndexShape = myEdgeToMove->getVertexIndex(myViewNet->getPositionInformation(), true, true);
                }
                // make sure that myViewNet->myMoveSingleElementValues.movingIndexShape isn't -1
                if (myViewNet->myMoveSingleElementValues.movingIndexShape != -1) {
                    myViewNet->myMoveSingleElementValues.originalPositionInView = myEdgeToMove->getNBEdge()->getInnerGeometry()[myViewNet->myMoveSingleElementValues.movingIndexShape];
                    // start geometry moving
                    myEdgeToMove->startGeometryMoving();
                    // edge values sucesfully calculated, then return true
                    return true;
                } else {
                    // edge values wasn't calculated, then return false
                    return false;
                }
            }
        }
    }
}


bool
GNEViewNetHelper::MoveSingleElementValues::calculateTAZValues() {
    // set TAZ to move
    myTAZToMove = myViewNet->myObjectsUnderCursor.getTAZFront();
    // save original shape (needed for commit change)
    myViewNet->myMoveSingleElementValues.originalShapeBeforeMoving = myTAZToMove->getShape();
    // save clicked position as moving original position
    myViewNet->myMoveSingleElementValues.originalPositionInView = myViewNet->getPositionInformation();
    // obtain index of vertex to move if shape isn't blocked
    if ((myTAZToMove->isShapeBlocked() == false) && (myTAZToMove->isAdditionalBlocked() == false)) {
        // check if we want to remove a Geometry Point
        if (myViewNet->myKeyPressed.shiftKeyPressed()) {
            // check if we're clicked over a Geometry Point
            myViewNet->myMoveSingleElementValues.movingIndexShape = myTAZToMove->getVertexIndex(myViewNet->myMoveSingleElementValues.originalPositionInView, false, false);
            if (myViewNet->myMoveSingleElementValues.movingIndexShape != -1) {
                myTAZToMove->deleteGeometryPoint(myViewNet->myMoveSingleElementValues.originalPositionInView);
                // after removing Geomtery Point, reset TAZToMove
                myTAZToMove = nullptr;
                // TAZ values wasn't calculated, then return false
                return false;
            }
            // TAZ values sucesfully calculated, then return true
            return true;
        } else {
            // obtain index of vertex to move and moving reference
            myViewNet->myMoveSingleElementValues.movingIndexShape = myTAZToMove->getVertexIndex(myViewNet->myMoveSingleElementValues.originalPositionInView, false, false);
            if (myViewNet->myMoveSingleElementValues.movingIndexShape == -1) {
                // create new geometry point
                myViewNet->myMoveSingleElementValues.movingIndexShape = myTAZToMove->getVertexIndex(myViewNet->myMoveSingleElementValues.originalPositionInView, true, true);
            }
            // TAZ values sucesfully calculated, then return true
            return true;
        }
    } else {
        // abort moving index shape
        myViewNet->myMoveSingleElementValues.movingIndexShape = -1;
        // TAZ values wasn't calculated, then return false
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::MoveMultipleElementValues - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::MoveMultipleElementValues::MoveMultipleElementValues(GNEViewNet* viewNet) :
    myViewNet(viewNet),
    myMovingSelection(false) {
}


void
GNEViewNetHelper::MoveMultipleElementValues::beginMoveSelection(GNEAttributeCarrier* originAC) {
    // enable moving selection
    myMovingSelection = true;
    // save clicked position (to calculate offset)
    myClickedPosition = myViewNet->getPositionInformation();
    // obtain Junctions and edges selected
    std::vector<GNEJunction*> selectedJunctions = myViewNet->getNet()->retrieveJunctions(true);
    std::vector<GNEEdge*> selectedEdges = myViewNet->getNet()->retrieveEdges(true);
    // Junctions are always moved, then save position of current selected junctions (Needed when mouse is released)
    for (auto i : selectedJunctions) {
        // save junction position
        myMovedJunctionOriginPositions[i] = i->getPositionInView();
        // start geometry moving
        i->startGeometryMoving();
    }
    // make special movement depending of clicked AC
    if (originAC->getTagProperty().getTag() == SUMO_TAG_JUNCTION) {
        // if clicked element is a junction, move shapes of all selected edges
        for (auto i : selectedEdges) {
            // save entire edge geometry
            myMovedEdgesOriginShape[i] = i->getNBEdge()->getInnerGeometry();
            // start geometry moving
            i->startGeometryMoving();
        }
    } else if (originAC->getTagProperty().getTag() == SUMO_TAG_EDGE) {
        // obtain clicked edge
        GNEEdge* clickedEdge = dynamic_cast<GNEEdge*>(originAC);
        // if clicked edge has origin and destiny junction selected, move shapes of all selected edges
        if (myMovedJunctionOriginPositions.count(clickedEdge->getGNEJunctionSource()) > 0 &&
                myMovedJunctionOriginPositions.count(clickedEdge->getGNEJunctionDestiny()) > 0) {
            for (auto i : selectedEdges) {
                // save entire edge geometry
                myMovedEdgesOriginShape[i] = i->getNBEdge()->getInnerGeometry();
                // start geometry moving
                i->startGeometryMoving();
            }
        } else {
            // declare three groups for dividing edges
            std::vector<GNEEdge*> noJunctionsSelected;
            std::vector<GNEEdge*> originJunctionSelected;
            std::vector<GNEEdge*> destinyJunctionSelected;
            // divide selected edges into four groups, depending of the selection of their junctions
            for (auto i : selectedEdges) {
                bool originSelected = myMovedJunctionOriginPositions.count(i->getGNEJunctionSource()) > 0;
                bool destinySelected = myMovedJunctionOriginPositions.count(i->getGNEJunctionDestiny()) > 0;
                // bot junctions selected
                if (!originSelected && !destinySelected) {
                    noJunctionsSelected.push_back(i);
                } else if (originSelected && !destinySelected) {
                    originJunctionSelected.push_back(i);
                } else if (!originSelected && destinySelected) {
                    destinyJunctionSelected.push_back(i);
                } else if (!originSelected && !destinySelected) {
                    // save edge geometry
                    myMovedEdgesOriginShape[i] = i->getNBEdge()->getInnerGeometry();
                    // start geometry moving
                    i->startGeometryMoving();
                }
            }
            // save original shape of all noJunctionsSelected edges (needed for commit change)
            for (auto i : noJunctionsSelected) {
                myMovedEgdesGeometryPoints[i] = new MoveSingleElementValues(myViewNet);
                // save edge geometry
                myMovedEgdesGeometryPoints[i]->originalShapeBeforeMoving = i->getNBEdge()->getInnerGeometry();
                // start geometry moving
                i->startGeometryMoving();
            }
            // obtain index shape of clicked edge
            int index = clickedEdge->getVertexIndex(myViewNet->getPositionInformation(), true, true);
            // check that index is valid
            if (index < 0) {
                // end geometry moving without changes in moved junctions
                for (auto i : myMovedJunctionOriginPositions) {
                    i.first->endGeometryMoving();
                }
                // end geometry moving without changes in moved edges
                for (auto i : myMovedEdgesOriginShape) {
                    i.first->endGeometryMoving();
                }
                // end geometry moving without changes in moved shapes
                for (auto i : myMovedEgdesGeometryPoints) {
                    i.first->endGeometryMoving();
                }
                // stop moving selection
                myMovingSelection = false;
                // clear containers
                myMovedJunctionOriginPositions.clear();
                myMovedEdgesOriginShape.clear();
                // delete all movedEgdesGeometryPoints before clear container
                for (const auto& i : myMovedEgdesGeometryPoints) {
                    delete i.second;
                }
                myMovedEgdesGeometryPoints.clear();
            } else {
                // save index and original position
                myMovedEgdesGeometryPoints[clickedEdge] = new MoveSingleElementValues(myViewNet);
                myMovedEgdesGeometryPoints[clickedEdge]->movingIndexShape = index;
                myMovedEgdesGeometryPoints[clickedEdge]->originalPositionInView = myViewNet->getPositionInformation();
                // start moving of clicked edge AFTER getting vertex Index
                clickedEdge->startGeometryMoving();
                // do the same for  the rest of noJunctionsSelected edges
                for (auto i : noJunctionsSelected) {
                    if (i != clickedEdge) {
                        myMovedEgdesGeometryPoints[i] = new MoveSingleElementValues(myViewNet);
                        // save index and original position
                        myMovedEgdesGeometryPoints[i]->movingIndexShape = i->getVertexIndex(myViewNet->getPositionInformation(), true, true);
                        // set originalPosition depending if edge is opposite to clicked edge
                        if (i->getOppositeEdge() == clickedEdge) {
                            myMovedEgdesGeometryPoints[i]->originalPositionInView = myViewNet->getPositionInformation();
                        } else {
                            myMovedEgdesGeometryPoints[i]->originalPositionInView = i->getNBEdge()->getInnerGeometry()[myMovedEgdesGeometryPoints[i]->movingIndexShape];
                        }
                        // start moving of clicked edge AFTER getting vertex Index
                        i->startGeometryMoving();
                    }
                }
            }
        }
    }
}


void
GNEViewNetHelper::MoveMultipleElementValues::moveSelection() {
    // calculate offset between current position and original position
    Position offsetMovement = myViewNet->getPositionInformation() - myClickedPosition;
    // calculate Z depending of Grid
    if (myViewNet->myNetworkViewOptions.menuCheckMoveElevation->shown() && myViewNet->myNetworkViewOptions.menuCheckMoveElevation->getCheck() == TRUE) {
        // reset offset X and Y and use Y for Z
        offsetMovement = Position(0, 0, offsetMovement.y());
    } else {
        // leave z empty (because in this case offset only actuates over X-Y)
        offsetMovement.setz(0);
    }
    // move selected junctions
    for (auto i : myMovedJunctionOriginPositions) {
        i.first->moveGeometry(i.second, offsetMovement);
    }
    // move entire edge shapes
    for (auto i : myMovedEdgesOriginShape) {
        i.first->moveEntireShape(i.second, offsetMovement);
    }
    // move partial shapes
    for (auto i : myMovedEgdesGeometryPoints) {
        i.first->moveVertexShape(i.second->movingIndexShape, i.second->originalPositionInView, offsetMovement);
    }
    // update view (needed to see the movement)
    myViewNet->update();
}


void
GNEViewNetHelper::MoveMultipleElementValues::finishMoveSelection() {
    // begin undo list
    myViewNet->getUndoList()->p_begin("position of selected elements");
    // commit positions of moved junctions
    for (auto i : myMovedJunctionOriginPositions) {
        i.first->commitGeometryMoving(i.second, myViewNet->getUndoList());
    }
    // commit shapes of entired moved edges
    for (auto i : myMovedEdgesOriginShape) {
        i.first->commitShapeChange(i.second, myViewNet->getUndoList());
    }
    //commit shapes of partial moved shapes
    for (auto i : myMovedEgdesGeometryPoints) {
        i.first->commitShapeChange(i.second->originalShapeBeforeMoving, myViewNet->getUndoList());
    }
    // end undo list
    myViewNet->getUndoList()->p_end();
    // stop moving selection
    myMovingSelection = false;
    // clear containers
    myMovedJunctionOriginPositions.clear();
    myMovedEdgesOriginShape.clear();
    // delete all movedEgdesGeometryPoints before clear container
    for (const auto& i : myMovedEgdesGeometryPoints) {
        delete i.second;
    }
    myMovedEgdesGeometryPoints.clear();
}


bool
GNEViewNetHelper::MoveMultipleElementValues::isMovingSelection() const {
    return myMovingSelection;
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::VehicleOptions - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::VehicleOptions::VehicleOptions(GNEViewNet* viewNet) :
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::VehicleOptions::buildVehicleOptionsMenuChecks() {
    UNUSED_PARAMETER(myViewNet);
    // currently unused
}


void
GNEViewNetHelper::VehicleOptions::hideVehicleOptionsMenuChecks() {
    // currently unused
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::VehicleTypeOptions - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::VehicleTypeOptions::VehicleTypeOptions(GNEViewNet* viewNet) :
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::VehicleTypeOptions::buildVehicleTypeOptionsMenuChecks() {
    UNUSED_PARAMETER(myViewNet);
    // currently unused
}


void
GNEViewNetHelper::VehicleTypeOptions::hideVehicleTypeOptionsMenuChecks() {
    // currently unused
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::SelectingArea - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::SelectingArea::SelectingArea(GNEViewNet* viewNet) :
    selectingUsingRectangle(false),
    startDrawing(false),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::SelectingArea::beginRectangleSelection() {
    selectingUsingRectangle = true;
    selectionCorner1 = myViewNet->getPositionInformation();
    selectionCorner2 = selectionCorner1;
}


void
GNEViewNetHelper::SelectingArea::moveRectangleSelection() {
    // start drawing
    startDrawing = true;
    // only update selection corner 2
    selectionCorner2 = myViewNet->getPositionInformation();
    // update status bar
    myViewNet->setStatusBarText("Selection width:" + toString(fabs(selectionCorner1.x() - selectionCorner2.x()))
                                + " height:" + toString(fabs(selectionCorner1.y() - selectionCorner2.y()))
                                + " diagonal:" + toString(selectionCorner1.distanceTo2D(selectionCorner2)));
    // update view (needed to update rectangle)
    myViewNet->update();
}


void
GNEViewNetHelper::SelectingArea::finishRectangleSelection() {
    // finish rectangle selection
    selectingUsingRectangle = false;
    startDrawing = false;
}


void
GNEViewNetHelper::SelectingArea::processRectangleSelection() {
    // shift held down on mouse-down and mouse-up and check that rectangle exist
    if ((abs(selectionCorner1.x() - selectionCorner2.x()) > 0.01) &&
            (abs(selectionCorner1.y() - selectionCorner2.y()) > 0.01) &&
            myViewNet->myKeyPressed.shiftKeyPressed()) {
        // create boundary between two corners
        Boundary rectangleBoundary;
        rectangleBoundary.add(selectionCorner1);
        rectangleBoundary.add(selectionCorner2);
        // process selection within boundary
        processBoundarySelection(rectangleBoundary);
    }
}


std::vector<GNEEdge*>
GNEViewNetHelper::SelectingArea::processEdgeRectangleSelection() {
    // declare vector for selection
    std::vector<GNEEdge*> result;
    // shift held down on mouse-down and mouse-up and check that rectangle exist
    if ((abs(selectionCorner1.x() - selectionCorner2.x()) > 0.01) &&
            (abs(selectionCorner1.y() - selectionCorner2.y()) > 0.01) &&
            myViewNet->myKeyPressed.shiftKeyPressed()) {
        // create boundary between two corners
        Boundary rectangleBoundary;
        rectangleBoundary.add(selectionCorner1);
        rectangleBoundary.add(selectionCorner2);
        if (myViewNet->makeCurrent()) {
            // obtain all ACs in Rectangle BOundary
            std::set<std::pair<std::string, GNEAttributeCarrier*> > ACsInBoundary = myViewNet->getAttributeCarriersInBoundary(rectangleBoundary);
            // Filter ACs in Boundary and get only edges
            for (auto i : ACsInBoundary) {
                if (i.second->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                    result.push_back(dynamic_cast<GNEEdge*>(i.second));
                }
            }
            myViewNet->makeNonCurrent();
        }
    }
    return result;
}


void
GNEViewNetHelper::SelectingArea::processShapeSelection(const PositionVector& shape) {
    processBoundarySelection(shape.getBoxBoundary());
}


void
GNEViewNetHelper::SelectingArea::drawRectangleSelection(const RGBColor& color) const {
    if (selectingUsingRectangle) {
        glPushMatrix();
        glTranslated(0, 0, GLO_MAX - 1);
        GLHelper::setColor(color);
        glLineWidth(2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_QUADS);
        glVertex2d(selectionCorner1.x(), selectionCorner1.y());
        glVertex2d(selectionCorner1.x(), selectionCorner2.y());
        glVertex2d(selectionCorner2.x(), selectionCorner2.y());
        glVertex2d(selectionCorner2.x(), selectionCorner1.y());
        glEnd();
        glPopMatrix();
    }
}


void
GNEViewNetHelper::SelectingArea::processBoundarySelection(const Boundary& boundary) {
    if (myViewNet->makeCurrent()) {
        std::set<std::pair<std::string, GNEAttributeCarrier*> > ACsInBoundary = myViewNet->getAttributeCarriersInBoundary(boundary);
        // filter ACsInBoundary depending of current supermode
        std::set<std::pair<std::string, GNEAttributeCarrier*> > ACsInBoundaryFiltered;
        for (const auto& i : ACsInBoundary) {
            if (((myViewNet->myEditModes.currentSupermode == GNE_SUPERMODE_NETWORK) && !i.second->getTagProperty().isDemandElement()) ||
                    ((myViewNet->myEditModes.currentSupermode == GNE_SUPERMODE_DEMAND) && i.second->getTagProperty().isDemandElement())) {
                ACsInBoundaryFiltered.insert(i);
            }
        }
        // declare two sets of attribute carriers, one for select and another for unselect
        std::vector<GNEAttributeCarrier*> ACToSelect;
        std::vector<GNEAttributeCarrier*> ACToUnselect;
        // reserve memory (we assume that in the worst case we're going to insert all elements of ACsInBoundaryFiltered
        ACToSelect.reserve(ACsInBoundaryFiltered.size());
        ACToUnselect.reserve(ACsInBoundaryFiltered.size());
        // in restrict AND replace mode all current selected attribute carriers will be unselected
        if ((myViewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode() == GNESelectorFrame::ModificationMode::SET_RESTRICT) ||
                (myViewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode() == GNESelectorFrame::ModificationMode::SET_REPLACE)) {
            // obtain selected ACs depending of current supermode
            std::vector<GNEAttributeCarrier*> selectedAC = myViewNet->getNet()->getSelectedAttributeCarriers(false);
            // add id into ACs to unselect
            for (auto i : selectedAC) {
                ACToUnselect.push_back(i);
            }
        }
        // iterate over AtributeCarriers obtained of boundary an place it in ACToSelect or ACToUnselect
        for (auto i : ACsInBoundaryFiltered) {
            switch (myViewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode()) {
                case GNESelectorFrame::ModificationMode::SET_SUB:
                    ACToUnselect.push_back(i.second);
                    break;
                case GNESelectorFrame::ModificationMode::SET_RESTRICT:
                    if (std::find(ACToUnselect.begin(), ACToUnselect.end(), i.second) != ACToUnselect.end()) {
                        ACToSelect.push_back(i.second);
                    }
                    break;
                default:
                    ACToSelect.push_back(i.second);
                    break;
            }
        }
        // select junctions and their connections and crossings if Auto select junctions is enabled (note: only for "add mode")
        if (myViewNet->autoSelectNodes() && GNESelectorFrame::ModificationMode::SET_ADD) {
            std::vector<GNEEdge*> edgesToSelect;
            // iterate over ACToSelect and extract edges
            for (auto i : ACToSelect) {
                if (i->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                    edgesToSelect.push_back(dynamic_cast<GNEEdge*>(i));
                }
            }
            // iterate over extracted edges
            for (auto i : edgesToSelect) {
                // select junction source and all their connections and crossings
                ACToSelect.push_back(i->getGNEJunctionSource());
                for (auto j : i->getGNEJunctionSource()->getGNEConnections()) {
                    ACToSelect.push_back(j);
                }
                for (auto j : i->getGNEJunctionSource()->getGNECrossings()) {
                    ACToSelect.push_back(j);
                }
                // select junction destiny and all their connections crossings
                ACToSelect.push_back(i->getGNEJunctionDestiny());
                for (auto j : i->getGNEJunctionDestiny()->getGNEConnections()) {
                    ACToSelect.push_back(j);
                }
                for (auto j : i->getGNEJunctionDestiny()->getGNECrossings()) {
                    ACToSelect.push_back(j);
                }
            }
        }
        // only continue if there is ACs to select or unselect
        if ((ACToSelect.size() + ACToUnselect.size()) > 0) {
            // first unselect AC of ACToUnselect and then selects AC of ACToSelect
            myViewNet->myUndoList->p_begin("selection using rectangle");
            for (auto i : ACToUnselect) {
                i->setAttribute(GNE_ATTR_SELECTED, "0", myViewNet->myUndoList);
            }
            for (auto i : ACToSelect) {
                if (i->getTagProperty().isSelectable()) {
                    i->setAttribute(GNE_ATTR_SELECTED, "1", myViewNet->myUndoList);
                }
            }
            myViewNet->myUndoList->p_end();
        }
        myViewNet->makeNonCurrent();
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::TestingMode - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::TestingMode::TestingMode(GNEViewNet* viewNet) :
    myViewNet(viewNet),
    myTestingEnabled(OptionsCont::getOptions().getBool("gui-testing")),
    myTestingWidth(0),
    myTestingHeight(0) {
}


void
GNEViewNetHelper::TestingMode::initTestingMode() {
    // first check if testing mode is enabled and window size is correct
    if (myTestingEnabled && OptionsCont::getOptions().isSet("window-size")) {
        std::vector<std::string> windowSize = OptionsCont::getOptions().getStringVector("window-size");
        // make sure that given windows size has exactly two valid int values
        if ((windowSize.size() == 2) && GNEAttributeCarrier::canParse<int>(windowSize[0]) && GNEAttributeCarrier::canParse<int>(windowSize[1])) {
            myTestingWidth = GNEAttributeCarrier::parse<int>(windowSize[0]);
            myTestingHeight = GNEAttributeCarrier::parse<int>(windowSize[1]);
        } else {
            WRITE_ERROR("Invalid windows size-format: " + toString(windowSize) + "for option 'window-size'");
        }
    }
}


void
GNEViewNetHelper::TestingMode::drawTestingElements(GUIMainWindow* mainWindow) {
    // first check if testing mode is neabled
    if (myTestingEnabled) {
        // check if main windows has to be resized
        if (myTestingWidth > 0 && ((myViewNet->getWidth() != myTestingWidth) || (myViewNet->getHeight() != myTestingHeight))) {
            // only resize once to avoid flickering
            //std::cout << " before resize: view=" << getWidth() << ", " << getHeight() << " app=" << mainWindow->getWidth() << ", " << mainWindow->getHeight() << "\n";
            mainWindow->resize(myTestingWidth + myTestingWidth - myViewNet->getWidth(), myTestingHeight + myTestingHeight - myViewNet->getHeight());
            //std::cout << " directly after resize: view=" << getWidth() << ", " << getHeight() << " app=" << mainWindow->getWidth() << ", " << mainWindow->getHeight() << "\n";
            myTestingWidth = 0;
        }
        //std::cout << " fixed: view=" << getWidth() << ", " << getHeight() << " app=" << mainWindow->getWidth() << ", " << mainWindow->getHeight() << "\n";
        // draw pink square in the upper left corner on top of everything
        glPushMatrix();
        const double size = myViewNet->p2m(32);
        Position center = myViewNet->screenPos2NetPos(8, 8);
        GLHelper::setColor(RGBColor::MAGENTA);
        glTranslated(center.x(), center.y(), GLO_MAX - 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_QUADS);
        glVertex2d(0, 0);
        glVertex2d(0, -size);
        glVertex2d(size, -size);
        glVertex2d(size, 0);
        glEnd();
        glPopMatrix();
        glPushMatrix();
        // show box with the current position relative to pink square
        Position posRelative = myViewNet->screenPos2NetPos(myViewNet->getWidth() - 40, myViewNet->getHeight() - 20);
        // adjust cursor position (24,25) to show exactly the same position as in function netedit.leftClick(match, X, Y)
        GLHelper::drawTextBox(toString(myViewNet->getWindowCursorPosition().x() - 24) + " " + toString(myViewNet->getWindowCursorPosition().y() - 25), posRelative, GLO_MAX - 1, myViewNet->p2m(20), RGBColor::BLACK, RGBColor::WHITE);
        glPopMatrix();
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::EditModes - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::EditModes::EditModes(GNEViewNet* viewNet) :
    currentSupermode(GNE_SUPERMODE_NONE),
    networkEditMode(GNE_NMODE_INSPECT),
    demandEditMode(GNE_DMODE_INSPECT),
    networkButton(nullptr),
    demandButton(nullptr),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::EditModes::buildSuperModeButtons() {
    // create buttons
    networkButton = new MFXCheckableButton(false, myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().superModes, "Network\t\tSet mode for edit network elements.",
                                           GUIIconSubSys::getIcon(ICON_SUPERMODENETWORK), myViewNet, MID_HOTKEY_F3_SUPERMODE_NETWORK, GUIDesignButtonToolbarSupermode);
    networkButton->create();

    demandButton = new MFXCheckableButton(false, myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().superModes, "Demand\t\tSet mode for edit traffic demand.",
                                          GUIIconSubSys::getIcon(ICON_SUPERMODEDEMAND), myViewNet, MID_HOTKEY_F4_SUPERMODE_DEMAND, GUIDesignButtonToolbarSupermode);
    demandButton->create();

    // recalc menu bar because there is new elements
    myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
    // show menu bar modes
    myViewNet->getViewParent()->getGNEAppWindows()->getToolbarsGrip().modes->show();
}


void
GNEViewNetHelper::EditModes::setSupermode(Supermode supermode) {
    if (supermode == currentSupermode) {
        myViewNet->setStatusBarText("Mode already selected");
        if (myViewNet->myCurrentFrame != nullptr) {
            myViewNet->myCurrentFrame->focusUpperElement();
        }
    } else {
        myViewNet->setStatusBarText("");
        // abort current operation
        myViewNet->abortOperation(false);
        // set super mode
        currentSupermode = supermode;
        // set supermodes
        if (supermode == GNE_SUPERMODE_NETWORK) {
            // change buttons
            networkButton->setChecked(true);
            demandButton->setChecked(false);
            // show network buttons
            myViewNet->myNetworkCheckableButtons.showNetworkCheckableButtons();
            // hide demand buttons
            myViewNet->myDemandCheckableButtons.hideDemandCheckableButtons();
            // force update network mode
            setNetworkEditMode(networkEditMode, true);
        } else if (supermode == GNE_SUPERMODE_DEMAND) {
            // change buttons
            networkButton->setChecked(false);
            demandButton->setChecked(true);
            // hide network buttons
            myViewNet->myNetworkCheckableButtons.hideNetworkCheckableButtons();
            // show demand buttons
            myViewNet->myDemandCheckableButtons.showDemandCheckableButtons();
            // force update demand mode
            setDemandEditMode(demandEditMode, true);
        }
        // update buttons
        networkButton->update();
        demandButton->update();
        // update Supermode CommandButtons in GNEAppWindows
        myViewNet->myViewParent->getGNEAppWindows()->updateSuperModeMenuCommands(currentSupermode);
    }
}


void
GNEViewNetHelper::EditModes::setNetworkEditMode(NetworkEditMode mode, bool force) {
    if ((mode == networkEditMode) && !force) {
        myViewNet->setStatusBarText("Network mode already selected");
        if (myViewNet->myCurrentFrame != nullptr) {
            myViewNet->myCurrentFrame->focusUpperElement();
        }
    } else if (networkEditMode == GNE_NMODE_TLS && !myViewNet->myViewParent->getTLSEditorFrame()->isTLSSaved()) {
        myViewNet->setStatusBarText("save modifications in TLS before change mode");
        myViewNet->myCurrentFrame->focusUpperElement();
    } else {
        myViewNet->setStatusBarText("");
        myViewNet->abortOperation(false);
        // stop editing of custom shapes
        myViewNet->myEditShapes.stopEditCustomShape();
        // set new Network mode
        networkEditMode = mode;
        // for common modes (Inspect/Delete/Select/move) change also the other supermode
        if (networkEditMode == GNE_NMODE_INSPECT) {
            demandEditMode = GNE_DMODE_INSPECT;
        } else if (networkEditMode == GNE_NMODE_DELETE) {
            demandEditMode = GNE_DMODE_DELETE;
        } else if (networkEditMode == GNE_NMODE_SELECT) {
            demandEditMode = GNE_DMODE_SELECT;
        } else if (networkEditMode == GNE_NMODE_MOVE) {
            demandEditMode = GNE_DMODE_MOVE;
        }
        // certain modes requiere a recomputing
        switch (mode) {
            case GNE_NMODE_CONNECT:
            case GNE_NMODE_PROHIBITION:
            case GNE_NMODE_TLS:
                // modes which depend on computed data
                myViewNet->myNet->computeNetwork(myViewNet->myViewParent->getGNEAppWindows());
                break;
            default:
                break;
        }
        // update network mode specific controls
        myViewNet->updateNetworkModeSpecificControls();
    }
}


void
GNEViewNetHelper::EditModes::setDemandEditMode(DemandEditMode mode, bool force) {
    if ((mode == demandEditMode) && !force) {
        myViewNet->setStatusBarText("Demand mode already selected");
        if (myViewNet->myCurrentFrame != nullptr) {
            myViewNet->myCurrentFrame->focusUpperElement();
        }
    } else {
        myViewNet->setStatusBarText("");
        myViewNet->abortOperation(false);
        // stop editing of custom shapes
        myViewNet->myEditShapes.stopEditCustomShape();
        // set new Demand mode
        demandEditMode = mode;
        // for common modes (Inspect/Delete/Select/Move) change also the other supermode
        if (demandEditMode == GNE_DMODE_INSPECT) {
            networkEditMode = GNE_NMODE_INSPECT;
        } else if (demandEditMode == GNE_DMODE_DELETE) {
            networkEditMode = GNE_NMODE_DELETE;
        } else if (demandEditMode == GNE_DMODE_SELECT) {
            networkEditMode = GNE_NMODE_SELECT;
        } else if (demandEditMode == GNE_DMODE_MOVE) {
            networkEditMode = GNE_NMODE_MOVE;
        }
        // demand modes requiere ALWAYS a recomputing
        myViewNet->myNet->computeNetwork(myViewNet->myViewParent->getGNEAppWindows());
        // update DijkstraRouter of RouteCalculatorInstance
        GNEDemandElement::getRouteCalculatorInstance()->updateDijkstraRouter();
        // compute demand elements (currently disabled)
        // myViewNet->getNet()->computeDemandElements(myViewNet->myViewParent->getGNEAppWindows());
        // update network mode specific controls
        myViewNet->updateDemandModeSpecificControls();
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::CommonViewOptions - methods
// ---------------------------------------------------------------------------


GNEViewNetHelper::CommonViewOptions::CommonViewOptions(GNEViewNet* viewNet) :
    menuCheckShowGrid(nullptr),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::CommonViewOptions::buildCommonViewOptionsMenuChecks() {

    menuCheckShowGrid = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
                                        ("Grid\t\tshow grid and restrict movement to the grid (size defined in visualization options)"),
                                        myViewNet, MID_GNE_COMMONVIEWOPTIONS_SHOWGRID, LAYOUT_FIX_HEIGHT);
    menuCheckShowGrid->setHeight(23);
    menuCheckShowGrid->setCheck(false);
    menuCheckShowGrid->create();

}


void
GNEViewNetHelper::CommonViewOptions::hideCommonViewOptionsMenuChecks() {
    menuCheckShowGrid->hide();
}


void
GNEViewNetHelper::CommonViewOptions::getVisibleCommonMenuCommands(std::vector<FXMenuCheck*>& commands) const {
    // save visible menu commands in commands vector
    if (menuCheckShowGrid->shown()) {
        commands.push_back(menuCheckShowGrid);
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::NetworkViewOptions - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::NetworkViewOptions::NetworkViewOptions(GNEViewNet* viewNet) :
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::NetworkViewOptions::buildNetworkViewOptionsMenuChecks() {
    menuCheckShowDemandElements = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
            ("Show demand elements\t\tToggle show demand elements"),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS, LAYOUT_FIX_HEIGHT);
    menuCheckShowDemandElements->setHeight(23);
    menuCheckShowDemandElements->setCheck(false);
    menuCheckShowDemandElements->create();

    menuCheckSelectEdges = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
                                           ("Select edges\t\tToggle whether clicking should select " + toString(SUMO_TAG_EDGE) + "s or " + toString(SUMO_TAG_LANE) + "s").c_str(),
                                           myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES, LAYOUT_FIX_HEIGHT);
    menuCheckSelectEdges->setHeight(23);
    menuCheckSelectEdges->setCheck(true);
    menuCheckSelectEdges->create();

    menuCheckShowConnections = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
            ("Show " + toString(SUMO_TAG_CONNECTION) + "s\t\tToggle show " + toString(SUMO_TAG_CONNECTION) + "s over " + toString(SUMO_TAG_JUNCTION) + "s").c_str(),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS, LAYOUT_FIX_HEIGHT);
    menuCheckShowConnections->setHeight(23);
    menuCheckShowConnections->setCheck(myViewNet->getVisualisationSettings()->showLane2Lane);
    menuCheckShowConnections->create();

    menuCheckHideConnections = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
            ("hide " + toString(SUMO_TAG_CONNECTION) + "s\t\tHide connections").c_str(),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS, LAYOUT_FIX_HEIGHT);
    menuCheckHideConnections->setHeight(23);
    menuCheckHideConnections->setCheck(false);
    menuCheckHideConnections->create();

    menuCheckExtendSelection = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
            ("Auto-select " + toString(SUMO_TAG_JUNCTION) + "s\t\tToggle whether selecting multiple " + toString(SUMO_TAG_EDGE) + "s should automatically select their " + toString(SUMO_TAG_JUNCTION) + "s").c_str(),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION, LAYOUT_FIX_HEIGHT);
    menuCheckExtendSelection->setHeight(23);
    menuCheckExtendSelection->setCheck(false);
    menuCheckExtendSelection->create();

    menuCheckChangeAllPhases = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
            ("Apply change to all phases\t\tToggle whether clicking should apply state changes to all phases of the current " + toString(SUMO_TAG_TRAFFIC_LIGHT) + " plan").c_str(),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES, LAYOUT_FIX_HEIGHT);
    menuCheckChangeAllPhases->setHeight(23);
    menuCheckChangeAllPhases->setCheck(false);
    menuCheckChangeAllPhases->create();

    menuCheckWarnAboutMerge = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
            ("Ask for merge\t\tAsk for confirmation before merging " + toString(SUMO_TAG_JUNCTION) + ".").c_str(),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE, LAYOUT_FIX_HEIGHT);
    menuCheckWarnAboutMerge->setHeight(23);
    menuCheckWarnAboutMerge->setCheck(true);
    menuCheckWarnAboutMerge->create();

    menuCheckShowJunctionBubble = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
            ("Bubbles\t\tShow bubbles over " + toString(SUMO_TAG_JUNCTION) + "'s shapes.").c_str(),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES, LAYOUT_FIX_HEIGHT);
    menuCheckShowJunctionBubble->setHeight(23);
    menuCheckShowJunctionBubble->setCheck(false);
    menuCheckShowJunctionBubble->create();

    menuCheckMoveElevation = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
            ("Elevation\t\tApply mouse movement to elevation instead of x,y position"),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION, LAYOUT_FIX_HEIGHT);
    menuCheckMoveElevation->setHeight(23);
    menuCheckMoveElevation->setCheck(false);
    menuCheckMoveElevation->create();

    menuCheckChainEdges = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
                                          ("Chain\t\tCreate consecutive " + toString(SUMO_TAG_EDGE) + "s with a single click (hit ESC to cancel chain).").c_str(),
                                          myViewNet, MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES, LAYOUT_FIX_HEIGHT);
    menuCheckChainEdges->setHeight(23);
    menuCheckChainEdges->setCheck(false);
    menuCheckChainEdges->create();

    menuCheckAutoOppositeEdge = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
            ("Two-way\t\tAutomatically create an " + toString(SUMO_TAG_EDGE) + " in the opposite direction").c_str(),
            myViewNet, MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES, LAYOUT_FIX_HEIGHT);
    menuCheckAutoOppositeEdge->setHeight(23);
    menuCheckAutoOppositeEdge->setCheck(false);
    menuCheckAutoOppositeEdge->create();

    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->recalc();
}


void
GNEViewNetHelper::NetworkViewOptions::hideNetworkViewOptionsMenuChecks() {
    menuCheckShowDemandElements->hide();
    menuCheckSelectEdges->hide();
    menuCheckShowConnections->hide();
    menuCheckHideConnections->hide();
    menuCheckExtendSelection->hide();
    menuCheckChangeAllPhases->hide();
    menuCheckWarnAboutMerge->hide();
    menuCheckShowJunctionBubble->hide();
    menuCheckMoveElevation->hide();
    menuCheckChainEdges->hide();
    menuCheckAutoOppositeEdge->hide();
    // Also hide toolbar grip
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
}


void
GNEViewNetHelper::NetworkViewOptions::getVisibleNetworkMenuCommands(std::vector<FXMenuCheck*>& commands) const {
    // save visible menu commands in commands vector
    if (menuCheckShowDemandElements->shown()) {
        commands.push_back(menuCheckShowDemandElements);
    }
    if (menuCheckSelectEdges->shown()) {
        commands.push_back(menuCheckSelectEdges);
    }
    if (menuCheckShowConnections->shown()) {
        commands.push_back(menuCheckShowConnections);
    }
    if (menuCheckHideConnections->shown()) {
        commands.push_back(menuCheckHideConnections);
    }
    if (menuCheckExtendSelection->shown()) {
        commands.push_back(menuCheckExtendSelection);
    }
    if (menuCheckChangeAllPhases->shown()) {
        commands.push_back(menuCheckChangeAllPhases);
    }
    if (menuCheckWarnAboutMerge->shown()) {
        commands.push_back(menuCheckWarnAboutMerge);
    }
    if (menuCheckShowJunctionBubble->shown()) {
        commands.push_back(menuCheckShowJunctionBubble);
    }
    if (menuCheckMoveElevation->shown()) {
        commands.push_back(menuCheckMoveElevation);
    }
    if (menuCheckChainEdges->shown()) {
        commands.push_back(menuCheckChainEdges);
    }
    if (menuCheckAutoOppositeEdge->shown()) {
        commands.push_back(menuCheckAutoOppositeEdge);
    }
}


bool
GNEViewNetHelper::NetworkViewOptions::showDemandElements() const {
    if (menuCheckShowDemandElements->shown()) {
        return (menuCheckShowDemandElements->getCheck() == TRUE);
    } else {
        // by default, if menuCheckShowDemandElements isn't shown, always show demand elements
        return true;
    }
}


bool
GNEViewNetHelper::NetworkViewOptions::selectEdges() const {
    if (menuCheckSelectEdges->shown()) {
        return (menuCheckSelectEdges->getCheck() == TRUE);
    } else {
        // by default, if menuCheckSelectEdges isn't shown, always select edges
        return true;
    }
}


bool
GNEViewNetHelper::NetworkViewOptions::showConnections() const {
    if (myViewNet->myEditModes.networkEditMode == GNE_NMODE_CONNECT) {
        // check if menu hceck hide connections ins shown
        return (menuCheckHideConnections->getCheck() == FALSE);
    } else if (myViewNet->myEditModes.networkEditMode == GNE_NMODE_PROHIBITION) {
        return true;
    } else if (menuCheckShowConnections->shown() == false) {
        return false;
    } else {
        return (myViewNet->getVisualisationSettings()->showLane2Lane);
    }
}


bool
GNEViewNetHelper::NetworkViewOptions::editingElevation() const {
    if (menuCheckMoveElevation->shown()) {
        return (menuCheckMoveElevation->getCheck() == TRUE);
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::DemandViewOptions - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::DemandViewOptions::DemandViewOptions(GNEViewNet* viewNet) :
    menuCheckHideShapes(nullptr),
    menuCheckHideNonInspectedDemandElements(nullptr),
    menuCheckShowAllPersonPlans(nullptr),
    menuCheckLockPerson(nullptr),
    myViewNet(viewNet),
    myLockedPerson(nullptr) {
}


void
GNEViewNetHelper::DemandViewOptions::buildDemandViewOptionsMenuChecks() {

    menuCheckHideShapes = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
                                          ("Hide shapes\t\tToggle show shapes (Polygons and POIs)"),
                                          myViewNet, MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES, LAYOUT_FIX_HEIGHT);
    menuCheckHideShapes->setHeight(23);
    menuCheckHideShapes->setCheck(false);
    menuCheckHideShapes->create();

    menuCheckHideNonInspectedDemandElements = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
            ("Hide non-inspected elements\t\tToggle show non-inspected demand elements"),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED, LAYOUT_FIX_HEIGHT);
    menuCheckHideNonInspectedDemandElements->setHeight(23);
    menuCheckHideNonInspectedDemandElements->setCheck(false);
    menuCheckHideNonInspectedDemandElements->create();

    menuCheckShowAllPersonPlans = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
            ("Show all person plans\t\tshow all person plans"),
            myViewNet, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS, LAYOUT_FIX_HEIGHT);
    menuCheckShowAllPersonPlans->setHeight(23);
    menuCheckShowAllPersonPlans->setCheck(false);
    menuCheckShowAllPersonPlans->create();

    menuCheckLockPerson = new FXMenuCheck(myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions,
                                          ("Lock person\t\tLock selected person"),
                                          myViewNet, MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON, LAYOUT_FIX_HEIGHT);
    menuCheckLockPerson->setHeight(23);
    menuCheckLockPerson->setCheck(false);
    menuCheckLockPerson->create();

    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->recalc();
}


void
GNEViewNetHelper::DemandViewOptions::hideDemandViewOptionsMenuChecks() {
    menuCheckHideShapes->hide();
    menuCheckHideNonInspectedDemandElements->hide();
    menuCheckShowAllPersonPlans->hide();
    menuCheckLockPerson->hide();
    // Also hide toolbar grip
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
}


void
GNEViewNetHelper::DemandViewOptions::getVisibleDemandMenuCommands(std::vector<FXMenuCheck*>& commands) const {
    // save visible menu commands in commands vector
    if (menuCheckHideShapes->shown()) {
        commands.push_back(menuCheckHideShapes);
    }
    if (menuCheckHideNonInspectedDemandElements->shown()) {
        commands.push_back(menuCheckHideNonInspectedDemandElements);
    }
    if (menuCheckShowAllPersonPlans->shown() && menuCheckShowAllPersonPlans->isEnabled()) {
        commands.push_back(menuCheckShowAllPersonPlans);
    }
    if (menuCheckLockPerson->shown() && menuCheckLockPerson->isEnabled()) {
        commands.push_back(menuCheckLockPerson);
    }
}


bool
GNEViewNetHelper::DemandViewOptions::showNonInspectedDemandElements(const GNEDemandElement* demandElement) const {
    if (menuCheckHideNonInspectedDemandElements->shown()) {
        // check conditions
        if ((menuCheckHideNonInspectedDemandElements->getCheck() == FALSE) || (myViewNet->getDottedAC() == nullptr)) {
            // if checkbox is disabled or there isn't insepected element, then return true
            return true;
        } else if (myViewNet->getDottedAC()->getTagProperty().isDemandElement()) {
            if (myViewNet->getDottedAC() == demandElement) {
                // if inspected element correspond to demandElement, return true
                return true;
            } else {
                // if demandElement is a route, check if dottedAC is one of their children (Vehicle or Stop)
                for (const auto& i : demandElement->getDemandElementChildren()) {
                    if (i == myViewNet->getDottedAC()) {
                        return true;
                    }
                }
                // if demandElement is a vehicle, check if dottedAC is one of his route Parent
                for (const auto& i : demandElement->getDemandElementParents()) {
                    if (i == myViewNet->getDottedAC()) {
                        return true;
                    }
                }
                // dottedAC isn't one of their parent, then return false
                return false;
            }
        } else {
            // we're inspecting a demand element, then return true
            return true;
        }
    } else {
        // we're inspecting a demand element, then return true
        return true;
    }
}


bool
GNEViewNetHelper::DemandViewOptions::showShapes() const {
    if (menuCheckHideShapes->shown()) {
        return (menuCheckHideShapes->getCheck() == FALSE);
    } else {
        return true;
    }
}


bool
GNEViewNetHelper::DemandViewOptions::showAllPersonPlans() const {
    if (menuCheckShowAllPersonPlans->shown() && menuCheckShowAllPersonPlans->isEnabled()) {
        return (menuCheckShowAllPersonPlans->getCheck() == TRUE);
    } else {
        return false;
    }
}


void
GNEViewNetHelper::DemandViewOptions::lockPerson(const GNEDemandElement* person) {
    myLockedPerson = person;
}


void
GNEViewNetHelper::DemandViewOptions::unlockPerson() {
    myLockedPerson = nullptr;
}


const GNEDemandElement*
GNEViewNetHelper::DemandViewOptions::getLockedPerson() const {
    return myLockedPerson;
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::CommonCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::CommonCheckableButtons::CommonCheckableButtons(GNEViewNet* viewNet) :
    inspectButton(nullptr),
    deleteButton(nullptr),
    selectButton(nullptr),
    moveButton(nullptr),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::CommonCheckableButtons::buildCommonCheckableButtons() {
    // inspect button
    inspectButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tset inspect mode\tMode for inspect elements and change their attributes.",
                                           GUIIconSubSys::getIcon(ICON_MODEINSPECT), myViewNet, MID_HOTKEY_I_INSPECTMODE, GUIDesignButtonToolbarCheckable);
    inspectButton->create();
    // delete button
    deleteButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tset delete mode\tMode for delete elements.",
                                          GUIIconSubSys::getIcon(ICON_MODEDELETE), myViewNet, MID_HOTKEY_D_DELETEMODE, GUIDesignButtonToolbarCheckable);
    deleteButton->create();
    // select button
    selectButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tset select mode\tMode for select elements.",
                                          GUIIconSubSys::getIcon(ICON_MODESELECT), myViewNet, MID_HOTKEY_S_SELECTMODE, GUIDesignButtonToolbarCheckable);
    selectButton->create();
    // move button
    moveButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tset move mode\tMode for move elements.",
                                        GUIIconSubSys::getIcon(ICON_MODEMOVE), myViewNet, MID_HOTKEY_M_MOVEMODE, GUIDesignButtonToolbarCheckable);
    moveButton->create();
    // always recalc menu bar after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::CommonCheckableButtons::showCommonCheckableButtons() {
    inspectButton->show();
    deleteButton->show();
    selectButton->show();
    moveButton->show();
}


void
GNEViewNetHelper::CommonCheckableButtons::hideCommonCheckableButtons() {
    inspectButton->hide();
    deleteButton->hide();
    selectButton->hide();
    moveButton->hide();
}


void
GNEViewNetHelper::CommonCheckableButtons::disableCommonCheckableButtons() {
    inspectButton->setChecked(false);
    deleteButton->setChecked(false);
    selectButton->setChecked(false);
    moveButton->setChecked(false);
}


void
GNEViewNetHelper::CommonCheckableButtons::updateCommonCheckableButtons() {
    inspectButton->update();
    deleteButton->update();
    selectButton->update();
    moveButton->update();
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::NetworkCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::NetworkCheckableButtons::NetworkCheckableButtons(GNEViewNet* viewNet) :
    createEdgeButton(nullptr),
    connectionButton(nullptr),
    trafficLightButton(nullptr),
    additionalButton(nullptr),
    crossingButton(nullptr),
    TAZButton(nullptr),
    shapeButton(nullptr),
    prohibitionButton(nullptr),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::NetworkCheckableButtons::buildNetworkCheckableButtons() {
    // create edge
    createEdgeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tset create edge mode\tMode for creating junction and edges.",
            GUIIconSubSys::getIcon(ICON_MODECREATEEDGE), myViewNet, MID_HOTKEY_E_EDGEMODE, GUIDesignButtonToolbarCheckable);
    createEdgeButton->create();
    // connection mode
    connectionButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tset connection mode\tMode for edit connections between lanes.",
            GUIIconSubSys::getIcon(ICON_MODECONNECTION), myViewNet, MID_HOTKEY_C_CONNECTMODE_PERSONPLANMODE, GUIDesignButtonToolbarCheckable);
    connectionButton->create();
    // prohibition mode
    prohibitionButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tset prohibition mode\tMode for editing connection prohibitions.",
            GUIIconSubSys::getIcon(ICON_MODEPROHIBITION), myViewNet, MID_HOTKEY_W_PROHIBITIONMODE_PERSONTYPEMODE, GUIDesignButtonToolbarCheckable);
    prohibitionButton->create();
    // traffic light mode
    trafficLightButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tset traffic light mode\tMode for edit traffic lights over junctions.",
            GUIIconSubSys::getIcon(ICON_MODETLS), myViewNet, MID_HOTKEY_T_TLSMODE_VTYPEMODE, GUIDesignButtonToolbarCheckable);
    trafficLightButton->create();
    // additional mode
    additionalButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tset additional mode\tMode for adding additional elements.",
            GUIIconSubSys::getIcon(ICON_MODEADDITIONAL), myViewNet, MID_HOTKEY_A_ADDITIONALMODE_STOPMODE, GUIDesignButtonToolbarCheckable);
    additionalButton->create();
    // crossing mode
    crossingButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tset crossing mode\tMode for creating crossings between edges.",
                                            GUIIconSubSys::getIcon(ICON_MODECROSSING), myViewNet, MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE, GUIDesignButtonToolbarCheckable);
    crossingButton->create();
    // TAZ Mode
    TAZButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tset TAZ mode\tMode for creating Traffic Assignment Zones.",
                                       GUIIconSubSys::getIcon(ICON_MODETAZ), myViewNet, MID_HOTKEY_Z_TAZMODE, GUIDesignButtonToolbarCheckable);
    TAZButton->create();
    // shape mode
    shapeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tset polygon mode\tMode for creating polygons and POIs.",
                                         GUIIconSubSys::getIcon(ICON_MODEPOLYGON), myViewNet, MID_HOTKEY_P_POLYGONMODE_PERSONMODE, GUIDesignButtonToolbarCheckable);
    shapeButton->create();
    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::NetworkCheckableButtons::showNetworkCheckableButtons() {
    createEdgeButton->show();
    connectionButton->show();
    trafficLightButton->show();
    additionalButton->show();
    crossingButton->show();
    TAZButton->show();
    shapeButton->show();
    prohibitionButton->show();
}


void
GNEViewNetHelper::NetworkCheckableButtons::hideNetworkCheckableButtons() {
    createEdgeButton->hide();
    connectionButton->hide();
    trafficLightButton->hide();
    additionalButton->hide();
    crossingButton->hide();
    TAZButton->hide();
    shapeButton->hide();
    prohibitionButton->hide();
}


void
GNEViewNetHelper::NetworkCheckableButtons::disableNetworkCheckableButtons() {
    createEdgeButton->setChecked(false);
    connectionButton->setChecked(false);
    trafficLightButton->setChecked(false);
    additionalButton->setChecked(false);
    crossingButton->setChecked(false);
    TAZButton->setChecked(false);
    shapeButton->setChecked(false);
    prohibitionButton->setChecked(false);
}


void
GNEViewNetHelper::NetworkCheckableButtons::updateNetworkCheckableButtons() {
    createEdgeButton->update();
    connectionButton->update();
    trafficLightButton->update();
    additionalButton->update();
    crossingButton->update();
    TAZButton->update();
    shapeButton->update();
    prohibitionButton->update();
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::DemandCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::DemandCheckableButtons::DemandCheckableButtons(GNEViewNet* viewNet) :
    routeButton(nullptr),
    vehicleButton(nullptr),
    vehicleTypeButton(nullptr),
    stopButton(nullptr),
    personTypeButton(nullptr),
    personButton(nullptr),
    personPlanButton(nullptr),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::DemandCheckableButtons::buildDemandCheckableButtons() {
    // route mode
    routeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tcreate route mode\tMode for creating routes.",
                                         GUIIconSubSys::getIcon(ICON_MODEROUTE), myViewNet, MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE, GUIDesignButtonToolbarCheckable);
    routeButton->create();
    // vehicle mode
    vehicleButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tcreate vehicle mode\tMode for creating vehicles.",
                                           GUIIconSubSys::getIcon(ICON_MODEVEHICLE), myViewNet, MID_HOTKEY_V_VEHICLEMODE, GUIDesignButtonToolbarCheckable);
    vehicleButton->create();
    // vehicle type mode
    vehicleTypeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tcreate vehicle type mode\tMode for creating vehicle types.",
            GUIIconSubSys::getIcon(ICON_MODEVEHICLETYPE), myViewNet, MID_HOTKEY_T_TLSMODE_VTYPEMODE, GUIDesignButtonToolbarCheckable);
    vehicleTypeButton->create();
    // stop mode
    stopButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tcreate stop mode\tMode for creating stops.",
                                        GUIIconSubSys::getIcon(ICON_MODESTOP), myViewNet, MID_HOTKEY_A_ADDITIONALMODE_STOPMODE, GUIDesignButtonToolbarCheckable);
    stopButton->create();
    // person type mode
    personTypeButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tcreate person type mode\tMode for creating person types.",
            GUIIconSubSys::getIcon(ICON_MODEPERSONTYPE), myViewNet, MID_HOTKEY_W_PROHIBITIONMODE_PERSONTYPEMODE, GUIDesignButtonToolbarCheckable);
    personTypeButton->create();
    // person mode
    personButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tcreate person mode\tMode for creating persons.",
                                          GUIIconSubSys::getIcon(ICON_MODEPERSON), myViewNet, MID_HOTKEY_P_POLYGONMODE_PERSONMODE, GUIDesignButtonToolbarCheckable);
    personButton->create();
    // person plan mode
    personPlanButton = new MFXCheckableButton(false, myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, "\tcreate person plan mode\tMode for creating person plans.",
            GUIIconSubSys::getIcon(ICON_MODEPERSONPLAN), myViewNet, MID_HOTKEY_C_CONNECTMODE_PERSONPLANMODE, GUIDesignButtonToolbarCheckable);
    personPlanButton->create();
    // always recalc after creating new elements
    myViewNet->myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
}


void
GNEViewNetHelper::DemandCheckableButtons::showDemandCheckableButtons() {
    routeButton->show();
    vehicleButton->show();
    vehicleTypeButton->show();
    stopButton->show();
    personTypeButton->show();
    personButton->show();
    personPlanButton->show();
}


void
GNEViewNetHelper::DemandCheckableButtons::hideDemandCheckableButtons() {
    routeButton->hide();
    vehicleButton->hide();
    vehicleTypeButton->hide();
    stopButton->hide();
    personTypeButton->hide();
    personButton->hide();
    personPlanButton->hide();
}


void
GNEViewNetHelper::DemandCheckableButtons::disableDemandCheckableButtons() {
    routeButton->setChecked(false);
    vehicleButton->setChecked(false);
    vehicleTypeButton->setChecked(false);
    stopButton->setChecked(false);
    personTypeButton->setChecked(false);
    personButton->setChecked(false);
    personPlanButton->setChecked(false);
}


void
GNEViewNetHelper::DemandCheckableButtons::updateDemandCheckableButtons() {
    routeButton->update();
    vehicleButton->update();
    vehicleTypeButton->update();
    stopButton->update();
    personTypeButton->update();
    personButton->update();
    personPlanButton->update();
}

// ---------------------------------------------------------------------------
// GNEViewNetHelper::EditShapes - methods
// ---------------------------------------------------------------------------

GNEViewNetHelper::EditShapes::EditShapes(GNEViewNet* viewNet) :
    editedShapePoly(nullptr),
    editingNetElementShapes(false),
    myViewNet(viewNet) {
}


void
GNEViewNetHelper::EditShapes::startEditCustomShape(GNENetElement* element, const PositionVector& shape, bool fill) {
    if ((editedShapePoly == nullptr) && (element != nullptr) && (shape.size() > 1)) {
        // save current edit mode before starting
        myPreviousNetworkEditMode = myViewNet->myEditModes.networkEditMode;
        if ((element->getTagProperty().getTag() == SUMO_TAG_CONNECTION) || (element->getTagProperty().getTag() == SUMO_TAG_CROSSING)) {
            editingNetElementShapes = true;
        } else {
            editingNetElementShapes = false;
        }
        // set move mode
        myViewNet->myEditModes.setNetworkEditMode(GNE_NMODE_MOVE);
        // add special GNEPoly fo edit shapes (color is taken from junction color settings)
        RGBColor col = myViewNet->getVisualisationSettings()->junctionColorer.getSchemes()[0].getColor(3);
        editedShapePoly = myViewNet->myNet->addPolygonForEditShapes(element, shape, fill, col);
        // update view net to show the new editedShapePoly
        myViewNet->update();
    }
}


void
GNEViewNetHelper::EditShapes::stopEditCustomShape() {
    // stop edit shape junction deleting editedShapePoly
    if (editedShapePoly != nullptr) {
        myViewNet->myNet->removePolygonForEditShapes(editedShapePoly);
        editedShapePoly = nullptr;
        // restore previous edit mode
        if (myViewNet->myEditModes.networkEditMode != myPreviousNetworkEditMode) {
            myViewNet->myEditModes.setNetworkEditMode(myPreviousNetworkEditMode);
        }
    }
}


void
GNEViewNetHelper::EditShapes::saveEditedShape() {
    // save edited junction's shape
    if (editedShapePoly != nullptr) {
        myViewNet->myUndoList->p_begin("custom " + editedShapePoly->getShapeEditedElement()->getTagStr() + " shape");
        SumoXMLAttr attr = SUMO_ATTR_SHAPE;
        if (editedShapePoly->getShapeEditedElement()->getTagProperty().hasAttribute(SUMO_ATTR_CUSTOMSHAPE)) {
            attr = SUMO_ATTR_CUSTOMSHAPE;
        }
        editedShapePoly->getShapeEditedElement()->setAttribute(attr, toString(editedShapePoly->getShape()), myViewNet->myUndoList);
        myViewNet->myUndoList->p_end();
        stopEditCustomShape();
        myViewNet->update();
    }
}

/****************************************************************************/
