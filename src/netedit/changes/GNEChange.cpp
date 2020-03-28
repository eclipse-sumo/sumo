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
/// @file    GNEChange.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// The reification of a NETEDIT editing operation (see command pattern)
// inherits from FXCommand and is used to for undo/redo
/****************************************************************************/

#include "GNEChange.h"

#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/additional/GNEShape.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/data/GNEEdgeData.h>

// ===========================================================================
// FOX-declarations
// ===========================================================================

FXIMPLEMENT_ABSTRACT(GNEChange, FXCommand, nullptr, 0)


// ===========================================================================
// static
// ===========================================================================

const std::vector<GNEEdge*> GNEChange::myEmptyEdges = {};
const std::vector<GNELane*> GNEChange::myEmptyLanes = {};
const std::vector<GNEShape*> GNEChange::myEmptyShapes = {};
const std::vector<GNEAdditional*> GNEChange::myEmptyAdditionals = {};
const std::vector<GNEDemandElement*> GNEChange::myEmptyDemandElements = {};
const std::vector<GNEGenericData*> GNEChange::myEmptyGenericDatas = {};


// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange::GNEChange(bool forward) :
    myNet(nullptr),
    myForward(forward),
    myParentEdges(myEmptyEdges),
    myParentLanes(myEmptyLanes),
    myParentShapes(myEmptyShapes),
    myParentAdditionals(myEmptyAdditionals),
    myParentDemandElements(myEmptyDemandElements),
    myParentGenericDatas(myEmptyGenericDatas),
    myChildEdges(myEmptyEdges),
    myChildLanes(myEmptyLanes),
    myChildShapes(myEmptyShapes),
    myChildAdditionals(myEmptyAdditionals),
    myChildDemandElements(myEmptyDemandElements),
    myChildGenericDatas(myEmptyGenericDatas) {}


GNEChange::GNEChange(GNENet* net, bool forward) :
    myNet(net),
    myForward(forward),
    myParentEdges(myEmptyEdges),
    myParentLanes(myEmptyLanes),
    myParentShapes(myEmptyShapes),
    myParentAdditionals(myEmptyAdditionals),
    myParentDemandElements(myEmptyDemandElements),
    myParentGenericDatas(myEmptyGenericDatas),
    myChildEdges(myEmptyEdges),
    myChildLanes(myEmptyLanes),
    myChildShapes(myEmptyShapes),
    myChildAdditionals(myEmptyAdditionals),
    myChildDemandElements(myEmptyDemandElements),
    myChildGenericDatas(myEmptyGenericDatas) {}


GNEChange::GNEChange(GNENet* net, GNEHierarchicalParentElements* parents, GNEHierarchicalChildElements* childs, bool forward) :
    myNet(net),
    myForward(forward),
    myParentEdges(parents->getParentEdges()),
    myParentLanes(parents->getParentLanes()),
    myParentShapes(parents->getParentShapes()),
    myParentAdditionals(parents->getParentAdditionals()),
    myParentDemandElements(parents->getParentDemandElements()),
    myParentGenericDatas(parents->getParentGenericDatas()),
    myChildEdges(childs->getChildEdges()),
    myChildLanes(childs->getChildLanes()),
    myChildShapes(childs->getChildShapes()),
    myChildAdditionals(childs->getChildAdditionals()),
    myChildDemandElements(childs->getChildDemandElements()),
    myChildGenericDatas(childs->getChildGenericDataElements()) {}


GNEChange::~GNEChange() {}


FXuint
GNEChange::size() const {
    return 1;
}


FXString
GNEChange::undoName() const {
    return "Undo";
}


FXString
GNEChange::redoName() const {
    return "Redo";
}


void
GNEChange::undo() {}


void
GNEChange::redo() {}

void
GNEChange::addEdge(GNEEdge* edge) {
    // add edge in parent elements
    for (const auto& parent : myParentEdges) {
        parent->addChildEdge(edge);
    }
    for (const auto& parent : myParentLanes) {
        parent->addChildEdge(edge);
    }
    for (const auto& parent : myParentShapes) {
        parent->addChildEdge(edge);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->addChildEdge(edge);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->addChildEdge(edge);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->addChildEdge(edge);
    }
    // add edge in child elements
    for (const auto& child : myChildEdges) {
        child->addParentEdge(edge);
    }
    for (const auto& child : myChildLanes) {
        child->addParentEdge(edge);
    }
    for (const auto& child : myChildShapes) {
        child->addParentEdge(edge);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentEdge(edge);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentEdge(edge);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addParentEdge(edge);
    }
}


void
GNEChange::removeEdge(GNEEdge* edge) {
    // Remove edge from parent elements
    for (const auto& parent : myParentEdges) {
        parent->removeChildEdge(edge);
    }
    for (const auto& parent : myParentLanes) {
        parent->removeChildEdge(edge);
    }
    for (const auto& parent : myParentShapes) {
        parent->removeChildEdge(edge);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->removeChildEdge(edge);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->removeChildEdge(edge);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->removeChildEdge(edge);
    }
    // Remove edge from child elements
    for (const auto& child : myChildEdges) {
        child->addParentEdge(edge);
    }
    for (const auto& child : myChildLanes) {
        child->addParentEdge(edge);
    }
    for (const auto& child : myChildShapes) {
        child->addParentEdge(edge);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentEdge(edge);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentEdge(edge);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addParentEdge(edge);
    }
}


void
GNEChange::addLane(GNELane* lane) {
    // add lane in parent elements
    for (const auto& parent : myParentEdges) {
        parent->addChildLane(lane);
    }
    for (const auto& parent : myParentLanes) {
        parent->addChildLane(lane);
    }
    for (const auto& parent : myParentShapes) {
        parent->addChildLane(lane);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->addChildLane(lane);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->addChildLane(lane);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->addChildLane(lane);
    }
    // add lane in child elements
    for (const auto& child : myChildEdges) {
        child->addParentLane(lane);
    }
    for (const auto& child : myChildLanes) {
        child->addParentLane(lane);
    }
    for (const auto& child : myChildShapes) {
        child->addParentLane(lane);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentLane(lane);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentLane(lane);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addParentLane(lane);
    }
}


void
GNEChange::removeLane(GNELane* lane) {
    // Remove lane from parent elements
    for (const auto& parent : myParentEdges) {
        parent->removeChildLane(lane);
    }
    for (const auto& parent : myParentLanes) {
        parent->removeChildLane(lane);
    }
    for (const auto& parent : myParentShapes) {
        parent->removeChildLane(lane);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->removeChildLane(lane);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->removeChildLane(lane);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->removeChildLane(lane);
    }
    // Remove lane from child elements
    for (const auto& child : myChildEdges) {
        child->removeParentLane(lane);
    }
    for (const auto& child : myChildLanes) {
        child->removeParentLane(lane);
    }
    for (const auto& child : myChildShapes) {
        child->removeParentLane(lane);
    }
    for (const auto& child : myChildAdditionals) {
        child->removeParentLane(lane);
    }
    for (const auto& child : myChildDemandElements) {
        child->removeParentLane(lane);
    }
    for (const auto& child : myChildGenericDatas) {
        child->removeParentLane(lane);
    }
}


void
GNEChange::addShape(GNEShape* shape) {
    // add shape in parent elements
    for (const auto& parent : myParentEdges) {
        parent->addChildShape(shape);
    }
    for (const auto& parent : myParentLanes) {
        parent->addChildShape(shape);
    }
    for (const auto& parent : myParentShapes) {
        parent->addChildShape(shape);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->addChildShape(shape);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->addChildShape(shape);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->addChildShape(shape);
    }
    // add shape in child elements
    for (const auto& child : myChildEdges) {
        child->addParentShape(shape);
    }
    for (const auto& child : myChildLanes) {
        child->addParentShape(shape);
    }
    for (const auto& child : myChildShapes) {
        child->addParentShape(shape);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentShape(shape);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentShape(shape);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addParentShape(shape);
    }
}


void
GNEChange::removeShape(GNEShape* shape) {
    // Remove shape from parent elements
    for (const auto& parent : myParentEdges) {
        parent->removeChildShape(shape);
    }
    for (const auto& parent : myParentLanes) {
        parent->removeChildShape(shape);
    }
    for (const auto& parent : myParentShapes) {
        parent->removeChildShape(shape);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->removeChildShape(shape);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->removeChildShape(shape);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->removeChildShape(shape);
    }
    // Remove shape from child elements
    for (const auto& child : myChildEdges) {
        child->removeParentShape(shape);
    }
    for (const auto& child : myChildLanes) {
        child->removeParentShape(shape);
    }
    for (const auto& child : myChildShapes) {
        child->removeParentShape(shape);
    }
    for (const auto& child : myChildAdditionals) {
        child->removeParentShape(shape);
    }
    for (const auto& child : myChildDemandElements) {
        child->removeParentShape(shape);
    }
    for (const auto& child : myChildGenericDatas) {
        child->removeParentShape(shape);
    }
}


void
GNEChange::addAdditional(GNEAdditional* additional) {
    // add additional in parent elements
    for (const auto& parent : myParentEdges) {
        parent->addChildAdditional(additional);
    }
    for (const auto& parent : myParentLanes) {
        parent->addChildAdditional(additional);
    }
    for (const auto& parent : myParentShapes) {
        parent->addChildAdditional(additional);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->addChildAdditional(additional);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->addChildAdditional(additional);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->addChildAdditional(additional);
    }
    // add additional in child elements
    for (const auto& child : myChildEdges) {
        child->addParentAdditional(additional);
    }
    for (const auto& child : myChildLanes) {
        child->addParentAdditional(additional);
    }
    for (const auto& child : myChildShapes) {
        child->addParentAdditional(additional);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentAdditional(additional);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentAdditional(additional);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addChildAdditional(additional);
    }
}


void
GNEChange::removeAdditional(GNEAdditional* additional) {
    // Remove additional from parent elements
    for (const auto& parent : myParentEdges) {
        parent->removeChildAdditional(additional);
    }
    for (const auto& parent : myParentLanes) {
        parent->removeChildAdditional(additional);
    }
    for (const auto& parent : myParentShapes) {
        parent->removeChildAdditional(additional);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->removeChildAdditional(additional);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->removeChildAdditional(additional);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->removeChildAdditional(additional);
    }
    // Remove additional from child elements
    for (const auto& child : myChildEdges) {
        child->removeParentAdditional(additional);
    }
    for (const auto& child : myChildLanes) {
        child->removeParentAdditional(additional);
    }
    for (const auto& child : myChildShapes) {
        child->removeChildAdditional(additional);
    }
    for (const auto& child : myChildAdditionals) {
        child->removeParentAdditional(additional);
    }
    for (const auto& child : myChildDemandElements) {
        child->removeParentAdditional(additional);
    }
    for (const auto& child : myChildGenericDatas) {
        child->removeParentAdditional(additional);
    }
}


void
GNEChange::addDemandElement(GNEDemandElement* demandElement) {
    // add demandElement in parent elements
    for (const auto& parent : myParentEdges) {
        parent->addChildDemandElement(demandElement);
    }
    for (const auto& parent : myParentLanes) {
        parent->addChildDemandElement(demandElement);
    }
    for (const auto& parent : myParentShapes) {
        parent->addChildDemandElement(demandElement);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->addChildDemandElement(demandElement);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->addChildDemandElement(demandElement);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->addChildDemandElement(demandElement);
    }
    // add demandElement in child elements
    for (const auto& child : myChildEdges) {
        child->addParentDemandElement(demandElement);
    }
    for (const auto& child : myChildLanes) {
        child->addParentDemandElement(demandElement);
    }
    for (const auto& child : myChildShapes) {
        child->addParentDemandElement(demandElement);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentDemandElement(demandElement);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentDemandElement(demandElement);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addParentDemandElement(demandElement);
    }
}


void
GNEChange::removeDemandElement(GNEDemandElement* demandElement) {
    // remove demandElement in parent elements
    for (const auto& parent : myParentEdges) {
        parent->removeChildDemandElement(demandElement);
    }
    for (const auto& parent : myParentLanes) {
        parent->removeChildDemandElement(demandElement);
    }
    for (const auto& parent : myParentShapes) {
        parent->removeChildDemandElement(demandElement);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->removeChildDemandElement(demandElement);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->removeChildDemandElement(demandElement);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->removeChildDemandElement(demandElement);
    }
    // remove demandElement in child elements
    for (const auto& child : myChildEdges) {
        child->removeParentDemandElement(demandElement);
    }
    for (const auto& child : myChildLanes) {
        child->removeParentDemandElement(demandElement);
    }
    for (const auto& child : myChildShapes) {
        child->removeParentDemandElement(demandElement);
    }
    for (const auto& child : myChildAdditionals) {
        child->removeParentDemandElement(demandElement);
    }
    for (const auto& child : myChildDemandElements) {
        child->removeParentDemandElement(demandElement);
    }
    for (const auto& child : myChildGenericDatas) {
        child->removeParentDemandElement(demandElement);
    }
}


void
GNEChange::addGenericData(GNEGenericData* genericData) {
    // add genericData in parent elements
    for (const auto& parent : myParentEdges) {
        parent->addChildGenericDataElement(genericData);
    }
    for (const auto& parent : myParentLanes) {
        parent->addChildGenericDataElement(genericData);
    }
    for (const auto& parent : myParentShapes) {
        parent->addChildGenericDataElement(genericData);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->addChildGenericDataElement(genericData);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->addChildGenericDataElement(genericData);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->addChildGenericDataElement(genericData);
    }
    // add genericData in child elements
    for (const auto& child : myChildEdges) {
        child->addParentGenericData(genericData);
    }
    for (const auto& child : myChildLanes) {
        child->addParentGenericData(genericData);
    }
    for (const auto& child : myChildShapes) {
        child->addParentGenericData(genericData);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentGenericData(genericData);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentGenericData(genericData);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addParentGenericData(genericData);
    }
}


void
GNEChange::removeGenericData(GNEGenericData* genericData) {
    // remove genericData in parent elements
    for (const auto& parent : myParentEdges) {
        parent->removeChildGenericDataElement(genericData);
    }
    for (const auto& parent : myParentLanes) {
        parent->removeChildGenericDataElement(genericData);
    }
    for (const auto& parent : myParentShapes) {
        parent->removeChildGenericDataElement(genericData);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->removeChildGenericDataElement(genericData);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->removeChildGenericDataElement(genericData);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->removeChildGenericDataElement(genericData);
    }
    // remove genericData in child elements
    for (const auto& child : myChildEdges) {
        child->removeParentGenericData(genericData);
    }
    for (const auto& child : myChildLanes) {
        child->removeParentGenericData(genericData);
    }
    for (const auto& child : myChildShapes) {
        child->removeParentGenericData(genericData);
    }
    for (const auto& child : myChildAdditionals) {
        child->removeParentGenericData(genericData);
    }
    for (const auto& child : myChildDemandElements) {
        child->removeParentGenericData(genericData);
    }
    for (const auto& child : myChildGenericDatas) {
        child->removeParentGenericData(genericData);
    }
}


/****************************************************************************/
