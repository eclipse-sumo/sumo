/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEHierarchicalStructureChildren.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2025
///
// Structure for // Structure for GNEHierarchicalElements centered in children
/****************************************************************************/

#include <netedit/elements/network/GNEJunction.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNETAZSourceSink.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <utils/common/UtilExceptions.h>

#include "GNEHierarchicalStructureChildren.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEHierarchicalStructureChildren::GNEHierarchicalStructureChildren() {}

// get functions

template<> const GNEHierarchicalContainerChildren<GNEJunction*>&
GNEHierarchicalStructureChildren::get() const {
    return myChildJunctions;
}


template<> const GNEHierarchicalContainerChildren<GNEEdge*>&
GNEHierarchicalStructureChildren::get() const {
    return myChildEdges;
}


template<> const GNEHierarchicalContainerChildren<GNELane*>&
GNEHierarchicalStructureChildren::get() const {
    return myChildLanes;
}


template<> const GNEHierarchicalContainerChildren<GNEAdditional*>&
GNEHierarchicalStructureChildren::get() const {
    return myChildAdditionals;
}


template<> const GNEHierarchicalContainerChildren<GNEDemandElement*>&
GNEHierarchicalStructureChildren::get() const {
    return myChildDemandElements;
}


template<> const GNEHierarchicalContainerChildren<GNEGenericData*>&
GNEHierarchicalStructureChildren::get() const {
    return myChildGenericDatas;
}


template<> const GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*>&
GNEHierarchicalStructureChildren::getSet() const {
    return myChildSourceSinks;
}

// add functions

template <> void
GNEHierarchicalStructureChildren::add(GNEJunction* child) {
    myChildJunctions.push_back(child);
}


template <> void
GNEHierarchicalStructureChildren::add(GNEEdge* child) {
    myChildEdges.push_back(child);
}


template <> void
GNEHierarchicalStructureChildren::add(GNELane* child) {
    myChildLanes.push_back(child);
}


template <> void
GNEHierarchicalStructureChildren::add(GNEAdditional* child) {
    myChildAdditionals.push_back(child);
}


template <> void
GNEHierarchicalStructureChildren::add(GNETAZSourceSink* child) {
    myChildSourceSinks.insert(child);
}


template <> void
GNEHierarchicalStructureChildren::add(GNEDemandElement* child) {
    myChildDemandElements.push_back(child);
}


template <> void
GNEHierarchicalStructureChildren::add(GNEGenericData* child) {
    myChildGenericDatas.push_back(child);
}

// remove functions

template <> void
GNEHierarchicalStructureChildren::remove(GNEJunction* child) {
    auto it = std::find(myChildJunctions.begin(), myChildJunctions.end(), child);
    if (it != myChildJunctions.end()) {
        myChildJunctions.erase(it);
    } else {
        throw ProcessError(child->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureChildren::remove(GNEEdge* child) {
    auto it = std::find(myChildEdges.begin(), myChildEdges.end(), child);
    if (it != myChildEdges.end()) {
        myChildEdges.erase(it);
    } else {
        throw ProcessError(child->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureChildren::remove(GNELane* child) {
    auto it = std::find(myChildLanes.begin(), myChildLanes.end(), child);
    if (it != myChildLanes.end()) {
        myChildLanes.erase(it);
    } else {
        throw ProcessError(child->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureChildren::remove(GNEAdditional* child) {
    auto it = std::find(myChildAdditionals.begin(), myChildAdditionals.end(), child);
    if (it != myChildAdditionals.end()) {
        myChildAdditionals.erase(it);
    } else {
        throw ProcessError(child->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureChildren::remove(GNETAZSourceSink* child) {
    auto it = myChildSourceSinks.find(child);
    if (it != myChildSourceSinks.end()) {
        myChildSourceSinks.erase(it);
    } else {
        throw ProcessError(child->getTagStr() + " is not a child element");
    }
}

template <> void
GNEHierarchicalStructureChildren::remove(GNEDemandElement* child) {
    auto it = std::find(myChildDemandElements.begin(), myChildDemandElements.end(), child);
    if (it != myChildDemandElements.end()) {
        myChildDemandElements.erase(it);
    } else {
        throw ProcessError(child->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureChildren::remove(GNEGenericData* child) {
    auto it = std::find(myChildGenericDatas.begin(), myChildGenericDatas.end(), child);
    if (it != myChildGenericDatas.end()) {
        myChildGenericDatas.erase(it);
    } else {
        throw ProcessError(child->getTagStr() + " is not a child element");
    }
}

// replaceAll functions

template <> void
GNEHierarchicalStructureChildren::replaceAll(const GNEHierarchicalContainerChildren<GNEJunction*>& children) {
    myChildJunctions = children;
}


template <> void
GNEHierarchicalStructureChildren::replaceAll(const GNEHierarchicalContainerChildren<GNEEdge*>& children) {
    myChildEdges = children;
}


template <> void
GNEHierarchicalStructureChildren::replaceAll(const GNEHierarchicalContainerChildren<GNELane*>& children) {
    myChildLanes = children;
}


template <> void
GNEHierarchicalStructureChildren::replaceAll(const GNEHierarchicalContainerChildren<GNEAdditional*>& children) {
    myChildAdditionals = children;
}


template <> void
GNEHierarchicalStructureChildren::replaceAll(const GNEHierarchicalContainerChildren<GNETAZSourceSink*>& children) {
    myChildSourceSinks.clear();
    for (const auto child : children) {
        myChildSourceSinks.insert(child);
    }
}


template <> void
GNEHierarchicalStructureChildren::replaceAll(const GNEHierarchicalContainerChildren<GNEDemandElement*>& children) {
    myChildDemandElements = children;
}


template <> void
GNEHierarchicalStructureChildren::replaceAll(const GNEHierarchicalContainerChildren<GNEGenericData*>& children) {
    myChildGenericDatas = children;
}

/****************************************************************************/
