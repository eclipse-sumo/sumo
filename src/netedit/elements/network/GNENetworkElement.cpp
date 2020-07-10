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
/// @file    GNENetworkElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
///
// A abstract class for networkElements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GUIParameterTableWindow.h>

#include "GNENetworkElement.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNENetworkElement::GNENetworkElement(GNENet* net, const std::string& id, GUIGlObjectType type, SumoXMLTag tag,
        const std::vector<GNEJunction*>& junctionParents,
        const std::vector<GNEEdge*>& edgeParents,
        const std::vector<GNELane*>& laneParents,
        const std::vector<GNEAdditional*>& additionalParents,
        const std::vector<GNEShape*>& shapeParents,
        const std::vector<GNETAZElement*>& TAZElementParents,
        const std::vector<GNEDemandElement*>& demandElementParents,
        const std::vector<GNEGenericData*>& genericDataParents,
        const std::vector<GNEJunction*>& junctionChildren,
        const std::vector<GNEEdge*>& edgeChildren,
        const std::vector<GNELane*>& laneChildren,
        const std::vector<GNEAdditional*>& additionalChildren,
        const std::vector<GNEShape*>& shapeChildren,
        const std::vector<GNETAZElement*>& TAZElementChildren,
        const std::vector<GNEDemandElement*>& demandElementChildren,
        const std::vector<GNEGenericData*>& genericDataChildren) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag, net),
    GNEHierarchicalParentElements(this, junctionParents, edgeParents, laneParents, additionalParents, shapeParents, TAZElementParents, demandElementParents, genericDataParents),
    GNEHierarchicalChildElements(this, junctionChildren, edgeChildren, laneChildren, additionalChildren, shapeChildren, TAZElementChildren, demandElementChildren, genericDataChildren),
    myMovingGeometryBoundary(),
    myShapeEdited(false) {
}


GNENetworkElement::~GNENetworkElement() {}


const std::string&
GNENetworkElement::getID() const {
    return getMicrosimID();
}


GUIGlObject*
GNENetworkElement::getGUIGlObject() {
    return this;
}


void 
GNENetworkElement::setShapeEdited(const bool value) {
    myShapeEdited = value;
}


bool 
GNENetworkElement::isShapeEdited() const {
    return myShapeEdited;
}


void
GNENetworkElement::startShapeGeometryMoving(const double shapeOffset) {
    PositionVector shape = parse<PositionVector>(getAttribute(SUMO_ATTR_SHAPE));
    // save current centering boundary
    myMovingGeometryBoundary = getCenteringBoundary();
    // start move shape depending of block shape
    startMoveShape(shape, shapeOffset, myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.movingGeometryPointRadius);
}


void
GNENetworkElement::endShapeGeometryMoving() {
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
GNENetworkElement::getShapeVertexIndex(Position pos, const bool snapToGrid) const {
    const PositionVector shape = parse<PositionVector>(getAttribute(SUMO_ATTR_SHAPE));
    // check if position has to be snapped to grid
    if (snapToGrid) {
        pos = myNet->getViewNet()->snapToActiveGrid(pos);
    }
    const double offset = shape.nearest_offset_to_point2D(pos, true);
    if (offset == GeomHelper::INVALID_OFFSET) {
        return -1;
    }
    Position newPos = shape.positionAtOffset2D(offset);
    // first check if vertex already exists in the inner geometry
    for (int i = 0; i < (int)shape.size(); i++) {
        if (shape[i].distanceTo2D(newPos) < myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.movingGeometryPointRadius) {
            // index refers to inner geometry
            if (i == 0 || i == (int)(shape.size() - 1)) {
                return -1;
            }
            return i;
        }
    }
    return -1;
}


void
GNENetworkElement::moveShape(const Position& offset) {
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
        if ((newShape.size() > 2) && (newShape.front() == newShape.back()) &&
            ((geometryPointIndex == 0) || (geometryPointIndex == lastIndex))) {
            // move first and last position in newShape
            newShape[0].add(offset);
            newShape[lastIndex] = newShape[0];
        } else {
            // move geometry point within newShape
            newShape[geometryPointIndex].add(offset);
        }
    }
    // set new shape
    setAttribute(SUMO_ATTR_SHAPE, toString(newShape));
    // update geometry
    updateGeometry();
}


void
GNENetworkElement::commitShapeChange(GNEUndoList* undoList) {
    // get visualisation settings
    auto &s = myNet->getViewNet()->getVisualisationSettings();
    // restore original shape into shapeToCommit
    PositionVector shapeToCommit = parse<PositionVector>(getAttribute(SUMO_ATTR_SHAPE));
    // get geometryPoint radius
    const double geometryPointRadius = s.neteditSizeSettings.movingGeometryPointRadius * s.polySize.getExaggeration(s, this);
    // remove double points
    shapeToCommit.removeDoublePoints(geometryPointRadius);
    // check if we have to merge start and end points
    if ((shapeToCommit.front() != shapeToCommit.back()) && (shapeToCommit.front().distanceTo2D(shapeToCommit.back()) < geometryPointRadius)) {
        shapeToCommit[0] = shapeToCommit.back();
    }
    // update geometry
    updateGeometry();
    // restore old geometry to allow change attribute (And restore shape if during movement a new point was created
    setAttribute(SUMO_ATTR_SHAPE, toString(getShapeBeforeMoving()));
    // finish geometry moving
    endShapeGeometryMoving();
    // commit new shape
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_SHAPE, toString(shapeToCommit)));
    undoList->p_end();
}


GUIParameterTableWindow*
GNENetworkElement::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // Iterate over attributes
    for (const auto& i : myTagProperty) {
        // Add attribute and set it dynamic if aren't unique
        if (i.isUnique()) {
            ret->mkItem(i.getAttrStr().c_str(), false, getAttribute(i.getAttr()));
        } else {
            ret->mkItem(i.getAttrStr().c_str(), true, getAttribute(i.getAttr()));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


void
GNENetworkElement::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


void
GNENetworkElement::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


std::string
GNENetworkElement::getPopUpID() const {
    if (myTagProperty.getTag() == SUMO_TAG_CONNECTION) {
        return getAttribute(SUMO_ATTR_FROM) + "_" + getAttribute(SUMO_ATTR_FROM_LANE) + " -> " + getAttribute(SUMO_ATTR_TO) + "_" + getAttribute(SUMO_ATTR_TO_LANE);
    } else {
        return getTagStr() + ": " + getID();
    }
}


std::string
GNENetworkElement::getHierarchyName() const {
    if (myTagProperty.getTag() == SUMO_TAG_LANE) {
        return toString(SUMO_TAG_LANE) + " " + getAttribute(SUMO_ATTR_INDEX);
    } else if (myTagProperty.getTag() == SUMO_TAG_CONNECTION) {
        return getAttribute(SUMO_ATTR_FROM_LANE) + " -> " + getAttribute(SUMO_ATTR_TO_LANE);
    } else if ((myTagProperty.getTag() == SUMO_TAG_EDGE) || (myTagProperty.getTag() == SUMO_TAG_CROSSING)) {
        return getPopUpID();
    } else {
        return getTagStr();
    }
}


void
GNENetworkElement::setEnabledAttribute(const int /*enabledAttributes*/) {
    //
}


/****************************************************************************/
