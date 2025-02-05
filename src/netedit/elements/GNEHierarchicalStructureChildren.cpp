/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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


size_t
GNEHierarchicalStructureChildren::getContainerSize() const {
    return (
               myChildJunctions.size() +
               myChildEdges.size() +
               myChildLanes.size() +
               myChildAdditionals.size() +
               myChildSourceSinks.size() +
               myChildDemandElements.size() +
               myChildGenericDatas.size()
           );
}


template <> void
GNEHierarchicalStructureChildren::addChildElement(GNEJunction* junction) {
    myChildJunctions.push_back(junction);
}


template <> void
GNEHierarchicalStructureChildren::addChildElement(GNEEdge* edge) {
    myChildEdges.push_back(edge);
}


template <> void
GNEHierarchicalStructureChildren::addChildElement(GNELane* lane) {
    myChildLanes.push_back(lane);
}


template <> void
GNEHierarchicalStructureChildren::addChildElement(GNEAdditional* additional) {
    myChildAdditionals.push_back(additional);
}


template <> void
GNEHierarchicalStructureChildren::addChildElement(GNETAZSourceSink* TAZSourceSink) {
    myChildSourceSinks.insert(TAZSourceSink);
}


template <> void
GNEHierarchicalStructureChildren::addChildElement(GNEDemandElement* demandElement) {
    myChildDemandElements.push_back(demandElement);
}


template <> void
GNEHierarchicalStructureChildren::addChildElement(GNEGenericData* genericData) {
    myChildGenericDatas.push_back(genericData);
}


template <> void
GNEHierarchicalStructureChildren::removeChildElement(GNEJunction* junction) {
    auto it = std::find(myChildJunctions.begin(), myChildJunctions.end(), junction);
    if (it != myChildJunctions.end()) {
        myChildJunctions.erase(it);
    } else {
        throw ProcessError(junction->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureChildren::removeChildElement(GNEEdge* edge) {
    auto it = std::find(myChildEdges.begin(), myChildEdges.end(), edge);
    if (it != myChildEdges.end()) {
        myChildEdges.erase(it);
    } else {
        throw ProcessError(edge->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureChildren::removeChildElement(GNELane* lane) {
    auto it = std::find(myChildLanes.begin(), myChildLanes.end(), lane);
    if (it != myChildLanes.end()) {
        myChildLanes.erase(it);
    } else {
        throw ProcessError(lane->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureChildren::removeChildElement(GNEAdditional* additional) {
    auto it = std::find(myChildAdditionals.begin(), myChildAdditionals.end(), additional);
    if (it != myChildAdditionals.end()) {
        myChildAdditionals.erase(it);
    } else {
        throw ProcessError(additional->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureChildren::removeChildElement(GNETAZSourceSink* TAZSourceSink) {
    auto it = myChildSourceSinks.find(TAZSourceSink);
    if (it != myChildSourceSinks.end()) {
        myChildSourceSinks.erase(it);
    } else {
        throw ProcessError(TAZSourceSink->getTagStr() + " is not a child element");
    }
}

template <> void
GNEHierarchicalStructureChildren::removeChildElement(GNEDemandElement* demandElement) {
    auto it = std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement);
    if (it != myChildDemandElements.end()) {
        myChildDemandElements.erase(it);
    } else {
        throw ProcessError(demandElement->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureChildren::removeChildElement(GNEGenericData* genericData) {
    auto it = std::find(myChildGenericDatas.begin(), myChildGenericDatas.end(), genericData);
    if (it != myChildGenericDatas.end()) {
        myChildGenericDatas.erase(it);
    } else {
        throw ProcessError(genericData->getTagStr() + " is not a child element");
    }
}


template<> const GNEHierarchicalContainerChildren<GNEJunction*>&
GNEHierarchicalStructureChildren::getChildren() const {
    return myChildJunctions;
}


template<> const GNEHierarchicalContainerChildren<GNEEdge*>&
GNEHierarchicalStructureChildren::getChildren() const {
    return myChildEdges;
}


template<> const GNEHierarchicalContainerChildren<GNELane*>&
GNEHierarchicalStructureChildren::getChildren() const {
    return myChildLanes;
}


template<> const GNEHierarchicalContainerChildren<GNEAdditional*>&
GNEHierarchicalStructureChildren::getChildren() const {
    return myChildAdditionals;
}


template<> const GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*>&
GNEHierarchicalStructureChildren::getChildrenSet() const {
    return myChildSourceSinks;
}


template<> const GNEHierarchicalContainerChildren<GNEDemandElement*>&
GNEHierarchicalStructureChildren::getChildren() const {
    return myChildDemandElements;
}


template<> const GNEHierarchicalContainerChildren<GNEGenericData*>&
GNEHierarchicalStructureChildren::getChildren() const {
    return myChildGenericDatas;
}


template<> void
GNEHierarchicalStructureChildren::setChildren(const GNEHierarchicalContainerChildren<GNEJunction*>& newChildren) {
    myChildJunctions = newChildren;
}


template<> void
GNEHierarchicalStructureChildren::setChildren(const GNEHierarchicalContainerChildren<GNEEdge*>& newChildren) {
    myChildEdges = newChildren;
}


template<> void
GNEHierarchicalStructureChildren::setChildren(const GNEHierarchicalContainerChildren<GNELane*>& newChildren) {
    myChildLanes = newChildren;
}


template<> void
GNEHierarchicalStructureChildren::setChildren(const GNEHierarchicalContainerChildren<GNEAdditional*>& newChildren) {
    myChildAdditionals = newChildren;
}


template<> void
GNEHierarchicalStructureChildren::setChildrenSet(const GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*>& newChildren) {
    myChildSourceSinks = newChildren;
}


template<> void
GNEHierarchicalStructureChildren::setChildren(const GNEHierarchicalContainerChildren<GNEDemandElement*>& newChildren) {
    myChildDemandElements = newChildren;
}


template<> void
GNEHierarchicalStructureChildren::setChildren(const GNEHierarchicalContainerChildren<GNEGenericData*>& newChildren) {
    myChildGenericDatas = newChildren;
}

/****************************************************************************/
