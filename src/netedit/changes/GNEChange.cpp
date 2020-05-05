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
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNEShape.h>
#include <netedit/elements/additional/GNETAZElement.h>
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
const std::vector<GNEAdditional*> GNEChange::myEmptyAdditionals = {};
const std::vector<GNEShape*> GNEChange::myEmptyShapes = {};
const std::vector<GNETAZElement*> GNEChange::myEmptyTAZElements = {};
const std::vector<GNEDemandElement*> GNEChange::myEmptyDemandElements = {};
const std::vector<GNEGenericData*> GNEChange::myEmptyGenericDatas = {};


// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange::GNEChange(bool forward) :
    myForward(forward),
    myParentEdges(myEmptyEdges),
    myParentLanes(myEmptyLanes),
    myParentAdditionals(myEmptyAdditionals),
    myParentShapes(myEmptyShapes),
    myParentTAZElements(myEmptyTAZElements),
    myParentDemandElements(myEmptyDemandElements),
    myParentGenericDatas(myEmptyGenericDatas),
    myChildEdges(myEmptyEdges),
    myChildLanes(myEmptyLanes),
    myChildAdditionals(myEmptyAdditionals),
    myChildShapes(myEmptyShapes),
    myChildTAZElements(myEmptyTAZElements),
    myChildDemandElements(myEmptyDemandElements),
    myChildGenericDatas(myEmptyGenericDatas) {}


GNEChange::GNEChange(GNEHierarchicalParentElements* parents, GNEHierarchicalChildElements* childs, bool forward) :
    myForward(forward),
    myParentEdges(parents->getParentEdges()),
    myParentLanes(parents->getParentLanes()),
    myParentAdditionals(parents->getParentAdditionals()),
    myParentShapes(parents->getParentShapes()),
    myParentTAZElements(parents->getParentTAZElements()),
    myParentDemandElements(parents->getParentDemandElements()),
    myParentGenericDatas(parents->getParentGenericDatas()),
    myChildEdges(childs->getChildEdges()),
    myChildLanes(childs->getChildLanes()),
    myChildAdditionals(childs->getChildAdditionals()),
    myChildShapes(childs->getChildShapes()),
    myChildTAZElements(childs->getChildTAZElements()),
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


template <> void
GNEChange::addElementInParentsAndChildren(GNEEdge* edge) {
    // add edge in parent elements
    for (const auto& parent : myParentEdges) {
        parent->addChildElement(edge);
    }
    for (const auto& parent : myParentLanes) {
        parent->addChildElement(edge);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->addChildElement(edge);
    }
    for (const auto& parent : myParentShapes) {
        parent->addChildElement(edge);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->addChildElement(edge);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->addChildElement(edge);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->addChildElement(edge);
    }
    // add edge in child elements
    for (const auto& child : myChildEdges) {
        child->addParentElement(edge);
    }
    for (const auto& child : myChildLanes) {
        child->addParentElement(edge);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentElement(edge);
    }
    for (const auto& child : myChildShapes) {
        child->addParentElement(edge);
    }
    for (const auto& child : myChildTAZElements) {
        child->addParentElement(edge);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentElement(edge);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addParentElement(edge);
    }
}


template <> void
GNEChange::removeElementFromParentsAndChildren(GNEEdge* edge) {
    // Remove edge from parent elements
    for (const auto& parent : myParentEdges) {
        parent->removeChildElement(edge);
    }
    for (const auto& parent : myParentLanes) {
        parent->removeChildElement(edge);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->removeChildElement(edge);
    }
    for (const auto& parent : myParentShapes) {
        parent->removeChildElement(edge);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->removeChildElement(edge);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->removeChildElement(edge);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->removeChildElement(edge);
    }
    // Remove edge from child elements
    for (const auto& child : myChildEdges) {
        child->removeParentElement(edge);
    }
    for (const auto& child : myChildLanes) {
        child->removeParentElement(edge);
    }
    for (const auto& child : myChildAdditionals) {
        child->removeParentElement(edge);
    }
    for (const auto& child : myChildShapes) {
        child->removeParentElement(edge);
    }
    for (const auto& child : myChildTAZElements) {
        child->removeParentElement(edge);
    }
    for (const auto& child : myChildDemandElements) {
        child->removeParentElement(edge);
    }
    for (const auto& child : myChildGenericDatas) {
        child->removeParentElement(edge);
    }
}


template <> void
GNEChange::addElementInParentsAndChildren(GNELane* lane) {
    // add lane in parent elements
    for (const auto& parent : myParentEdges) {
        parent->addChildElement(lane);
    }
    for (const auto& parent : myParentLanes) {
        parent->addChildElement(lane);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->addChildElement(lane);
    }
    for (const auto& parent : myParentShapes) {
        parent->addChildElement(lane);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->addChildElement(lane);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->addChildElement(lane);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->addChildElement(lane);
    }
    // add lane in child elements
    for (const auto& child : myChildEdges) {
        child->addParentElement(lane);
    }
    for (const auto& child : myChildLanes) {
        child->addParentElement(lane);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentElement(lane);
    }
    for (const auto& child : myChildShapes) {
        child->addParentElement(lane);
    }
    for (const auto& child : myChildTAZElements) {
        child->addParentElement(lane);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentElement(lane);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addParentElement(lane);
    }
}


template <> void
GNEChange::removeElementFromParentsAndChildren(GNELane* lane) {
    // Remove lane from parent elements
    for (const auto& parent : myParentEdges) {
        parent->removeChildElement(lane);
    }
    for (const auto& parent : myParentLanes) {
        parent->removeChildElement(lane);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->removeChildElement(lane);
    }
    for (const auto& parent : myParentShapes) {
        parent->removeChildElement(lane);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->removeChildElement(lane);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->removeChildElement(lane);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->removeChildElement(lane);
    }
    // Remove lane from child elements
    for (const auto& child : myChildEdges) {
        child->removeParentElement(lane);
    }
    for (const auto& child : myChildLanes) {
        child->removeParentElement(lane);
    }
    for (const auto& child : myChildAdditionals) {
        child->removeParentElement(lane);
    }
    for (const auto& child : myChildShapes) {
        child->removeParentElement(lane);
    }
    for (const auto& child : myChildTAZElements) {
        child->removeParentElement(lane);
    }
    for (const auto& child : myChildDemandElements) {
        child->removeParentElement(lane);
    }
    for (const auto& child : myChildGenericDatas) {
        child->removeParentElement(lane);
    }
}


template <> void
GNEChange::addElementInParentsAndChildren(GNEShape* shape) {
    // add shape in parent elements
    for (const auto& parent : myParentEdges) {
        parent->addChildElement(shape);
    }
    for (const auto& parent : myParentLanes) {
        parent->addChildElement(shape);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->addChildElement(shape);
    }
    for (const auto& parent : myParentShapes) {
        parent->addChildElement(shape);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->addChildElement(shape);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->addChildElement(shape);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->addChildElement(shape);
    }
    // add shape in child elements
    for (const auto& child : myChildEdges) {
        child->addParentElement(shape);
    }
    for (const auto& child : myChildLanes) {
        child->addParentElement(shape);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentElement(shape);
    }
    for (const auto& child : myChildShapes) {
        child->addParentElement(shape);
    }
    for (const auto& child : myChildTAZElements) {
        child->addParentElement(shape);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentElement(shape);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addParentElement(shape);
    }
}


template <> void
GNEChange::removeElementFromParentsAndChildren(GNEShape* shape) {
    // Remove shape from parent elements
    for (const auto& parent : myParentEdges) {
        parent->removeChildElement(shape);
    }
    for (const auto& parent : myParentLanes) {
        parent->removeChildElement(shape);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->removeChildElement(shape);
    }
    for (const auto& parent : myParentShapes) {
        parent->removeChildElement(shape);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->removeChildElement(shape);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->removeChildElement(shape);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->removeChildElement(shape);
    }
    // Remove shape from child elements
    for (const auto& child : myChildEdges) {
        child->removeParentElement(shape);
    }
    for (const auto& child : myChildLanes) {
        child->removeParentElement(shape);
    }
    for (const auto& child : myChildAdditionals) {
        child->removeParentElement(shape);
    }
    for (const auto& child : myChildShapes) {
        child->removeParentElement(shape);
    }
    for (const auto& child : myChildTAZElements) {
        child->removeParentElement(shape);
    }
    for (const auto& child : myChildDemandElements) {
        child->removeParentElement(shape);
    }
    for (const auto& child : myChildGenericDatas) {
        child->removeParentElement(shape);
    }
}


template <> void
GNEChange::addElementInParentsAndChildren(GNETAZElement* TAZElement) {
    // add shape in parent elements
    for (const auto& parent : myParentEdges) {
        parent->addChildElement(TAZElement);
    }
    for (const auto& parent : myParentLanes) {
        parent->addChildElement(TAZElement);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->addChildElement(TAZElement);
    }
    for (const auto& parent : myParentShapes) {
        parent->addChildElement(TAZElement);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->addChildElement(TAZElement);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->addChildElement(TAZElement);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->addChildElement(TAZElement);
    }
    // add shape in child elements
    for (const auto& child : myChildEdges) {
        child->addParentElement(TAZElement);
    }
    for (const auto& child : myChildLanes) {
        child->addParentElement(TAZElement);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentElement(TAZElement);
    }
    for (const auto& child : myChildShapes) {
        child->addParentElement(TAZElement);
    }
    for (const auto& child : myChildTAZElements) {
        child->addParentElement(TAZElement);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentElement(TAZElement);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addParentElement(TAZElement);
    }
}


template <> void
GNEChange::removeElementFromParentsAndChildren(GNETAZElement* TAZElement) {
    // Remove shape from parent elements
    for (const auto& parent : myParentEdges) {
        parent->removeChildElement(TAZElement);
    }
    for (const auto& parent : myParentLanes) {
        parent->removeChildElement(TAZElement);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->removeChildElement(TAZElement);
    }
    for (const auto& parent : myParentShapes) {
        parent->removeChildElement(TAZElement);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->removeChildElement(TAZElement);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->removeChildElement(TAZElement);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->removeChildElement(TAZElement);
    }
    // Remove shape from child elements
    for (const auto& child : myChildEdges) {
        child->removeParentElement(TAZElement);
    }
    for (const auto& child : myChildLanes) {
        child->removeParentElement(TAZElement);
    }
    for (const auto& child : myChildAdditionals) {
        child->removeParentElement(TAZElement);
    }
    for (const auto& child : myChildShapes) {
        child->removeParentElement(TAZElement);
    }
    for (const auto& child : myChildTAZElements) {
        child->removeParentElement(TAZElement);
    }
    for (const auto& child : myChildDemandElements) {
        child->removeParentElement(TAZElement);
    }
    for (const auto& child : myChildGenericDatas) {
        child->removeParentElement(TAZElement);
    }
}


template <> void
GNEChange::addElementInParentsAndChildren(GNEAdditional* additional) {
    // add additional in parent elements
    for (const auto& parent : myParentEdges) {
        parent->addChildElement(additional);
    }
    for (const auto& parent : myParentLanes) {
        parent->addChildElement(additional);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->addChildElement(additional);
    }
    for (const auto& parent : myParentShapes) {
        parent->addChildElement(additional);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->addChildElement(additional);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->addChildElement(additional);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->addChildElement(additional);
    }
    // add additional in child elements
    for (const auto& child : myChildEdges) {
        child->addParentElement(additional);
    }
    for (const auto& child : myChildLanes) {
        child->addParentElement(additional);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentElement(additional);
    }
    for (const auto& child : myChildShapes) {
        child->addParentElement(additional);
    }
    for (const auto& child : myChildTAZElements) {
        child->addParentElement(additional);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentElement(additional);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addChildElement(additional);
    }
}


template <> void
GNEChange::removeElementFromParentsAndChildren(GNEAdditional* additional) {
    // Remove additional from parent elements
    for (const auto& parent : myParentEdges) {
        parent->removeChildElement(additional);
    }
    for (const auto& parent : myParentLanes) {
        parent->removeChildElement(additional);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->removeChildElement(additional);
    }
    for (const auto& parent : myParentShapes) {
        parent->removeChildElement(additional);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->removeChildElement(additional);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->removeChildElement(additional);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->removeChildElement(additional);
    }
    // Remove additional from child elements
    for (const auto& child : myChildEdges) {
        child->removeParentElement(additional);
    }
    for (const auto& child : myChildLanes) {
        child->removeParentElement(additional);
    }
    for (const auto& child : myChildAdditionals) {
        child->removeParentElement(additional);
    }
    for (const auto& child : myChildShapes) {
        child->removeParentElement(additional);
    }
    for (const auto& child : myChildTAZElements) {
        child->removeParentElement(additional);
    }
    for (const auto& child : myChildDemandElements) {
        child->removeParentElement(additional);
    }
    for (const auto& child : myChildGenericDatas) {
        child->removeParentElement(additional);
    }
}


template <> void
GNEChange::addElementInParentsAndChildren(GNEDemandElement* demandElement) {
    // add demandElement in parent elements
    for (const auto& parent : myParentEdges) {
        parent->addChildElement(demandElement);
    }
    for (const auto& parent : myParentLanes) {
        parent->addChildElement(demandElement);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->addChildElement(demandElement);
    }
    for (const auto& parent : myParentShapes) {
        parent->addChildElement(demandElement);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->addChildElement(demandElement);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->addChildElement(demandElement);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->addChildElement(demandElement);
    }
    // add demandElement in child elements
    for (const auto& child : myChildEdges) {
        child->addParentElement(demandElement);
    }
    for (const auto& child : myChildLanes) {
        child->addParentElement(demandElement);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentElement(demandElement);
    }
    for (const auto& child : myChildShapes) {
        child->addParentElement(demandElement);
    }
    for (const auto& child : myChildTAZElements) {
        child->addParentElement(demandElement);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentElement(demandElement);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addParentElement(demandElement);
    }
}


template <> void
GNEChange::removeElementFromParentsAndChildren(GNEDemandElement* demandElement) {
    // remove demandElement in parent elements
    for (const auto& parent : myParentEdges) {
        parent->removeChildElement(demandElement);
    }
    for (const auto& parent : myParentLanes) {
        parent->removeChildElement(demandElement);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->removeChildElement(demandElement);
    }
    for (const auto& parent : myParentShapes) {
        parent->removeChildElement(demandElement);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->removeChildElement(demandElement);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->removeChildElement(demandElement);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->removeChildElement(demandElement);
    }
    // remove demandElement in child elements
    for (const auto& child : myChildEdges) {
        child->removeParentElement(demandElement);
    }
    for (const auto& child : myChildLanes) {
        child->removeParentElement(demandElement);
    }
    for (const auto& child : myChildAdditionals) {
        child->removeParentElement(demandElement);
    }
    for (const auto& child : myChildShapes) {
        child->removeParentElement(demandElement);
    }
    for (const auto& child : myChildTAZElements) {
        child->removeParentElement(demandElement);
    }
    for (const auto& child : myChildDemandElements) {
        child->removeParentElement(demandElement);
    }
    for (const auto& child : myChildGenericDatas) {
        child->removeParentElement(demandElement);
    }
}


template <> void
GNEChange::addElementInParentsAndChildren(GNEGenericData* genericData) {
    // add genericData in parent elements
    for (const auto& parent : myParentEdges) {
        parent->addChildElement(genericData);
    }
    for (const auto& parent : myParentLanes) {
        parent->addChildElement(genericData);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->addChildElement(genericData);
    }
    for (const auto& parent : myParentShapes) {
        parent->addChildElement(genericData);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->addChildElement(genericData);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->addChildElement(genericData);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->addChildElement(genericData);
    }
    // add genericData in child elements
    for (const auto& child : myChildEdges) {
        child->addParentElement(genericData);
    }
    for (const auto& child : myChildLanes) {
        child->addParentElement(genericData);
    }
    for (const auto& child : myChildAdditionals) {
        child->addParentElement(genericData);
    }
    for (const auto& child : myChildShapes) {
        child->addParentElement(genericData);
    }
    for (const auto& child : myChildTAZElements) {
        child->addParentElement(genericData);
    }
    for (const auto& child : myChildDemandElements) {
        child->addParentElement(genericData);
    }
    for (const auto& child : myChildGenericDatas) {
        child->addParentElement(genericData);
    }
}


template <> void
GNEChange::removeElementFromParentsAndChildren(GNEGenericData* genericData) {
    // remove genericData in parent elements
    for (const auto& parent : myParentEdges) {
        parent->removeChildElement(genericData);
    }
    for (const auto& parent : myParentLanes) {
        parent->removeChildElement(genericData);
    }
    for (const auto& parent : myParentAdditionals) {
        parent->removeChildElement(genericData);
    }
    for (const auto& parent : myParentShapes) {
        parent->removeChildElement(genericData);
    }
    for (const auto& parent : myParentTAZElements) {
        parent->removeChildElement(genericData);
    }
    for (const auto& parent : myParentDemandElements) {
        parent->removeChildElement(genericData);
    }
    for (const auto& parent : myParentGenericDatas) {
        parent->removeChildElement(genericData);
    }
    // remove genericData in child elements
    for (const auto& child : myChildEdges) {
        child->removeParentElement(genericData);
    }
    for (const auto& child : myChildLanes) {
        child->removeParentElement(genericData);
    }
    for (const auto& child : myChildAdditionals) {
        child->removeParentElement(genericData);
    }
    for (const auto& child : myChildShapes) {
        child->removeParentElement(genericData);
    }
    for (const auto& child : myChildTAZElements) {
        child->removeParentElement(genericData);
    }
    for (const auto& child : myChildDemandElements) {
        child->removeParentElement(genericData);
    }
    for (const auto& child : myChildGenericDatas) {
        child->removeParentElement(genericData);
    }
}


/****************************************************************************/
