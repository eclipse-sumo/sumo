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

/****************************************************************************/
