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


GNEHierarchicalStructureChildren::GNEHierarchicalStructureChildren(
    const std::vector<GNEJunction*>& parentJunctions,
    const std::vector<GNEEdge*>& parentEdges,
    const std::vector<GNELane*>& parentLanes,
    const std::vector<GNEAdditional*>& parentAdditionals,
    const std::vector<GNEDemandElement*>& parentDemandElements,
    const std::vector<GNEGenericData*>& parentGenericDatas):
    myParentJunctions(parentJunctions),
    myParentEdges(parentEdges),
    myParentLanes(parentLanes),
    myParentAdditionals(parentAdditionals),
    myParentDemandElements(parentDemandElements),
    myParentGenericDatas(parentGenericDatas) {
}


size_t
GNEHierarchicalStructureChildren::getContainerSize() const {
    return (
               myParentJunctions.size() +
               myParentEdges.size() +
               myParentLanes.size() +
               myParentAdditionals.size() +
               myParentTAZSourceSinks.size() +
               myParentDemandElements.size() +
               myParentGenericDatas.size() +
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
GNEHierarchicalStructureChildren::addParentElement(GNEJunction* junction) {
    myParentJunctions.push_back(junction);
}


template <> void
GNEHierarchicalStructureChildren::addParentElement(GNEEdge* edge) {
    myParentEdges.push_back(edge);
}


template <> void
GNEHierarchicalStructureChildren::addParentElement(GNELane* lane) {
    myParentLanes.push_back(lane);
}


template <> void
GNEHierarchicalStructureChildren::addParentElement(GNEAdditional* additional) {
    myParentAdditionals.push_back(additional);
}


template <> void
GNEHierarchicalStructureChildren::addParentElement(GNETAZSourceSink* TAZSourceSink) {
    myParentTAZSourceSinks.push_back(TAZSourceSink);
}


template <> void
GNEHierarchicalStructureChildren::addParentElement(GNEDemandElement* demandElement) {
    myParentDemandElements.push_back(demandElement);
}


template <> void
GNEHierarchicalStructureChildren::addParentElement(GNEGenericData* genericData) {
    myParentGenericDatas.push_back(genericData);
}


template <> void
GNEHierarchicalStructureChildren::removeParentElement(GNEJunction* junction) {
    auto it = std::find(myParentJunctions.begin(), myParentJunctions.end(), junction);
    if (it != myParentJunctions.end()) {
        myParentJunctions.erase(it);
    } else {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructureChildren::removeParentElement(GNEEdge* edge) {
    auto it = std::find(myParentEdges.begin(), myParentEdges.end(), edge);
    if (it != myParentEdges.end()) {
        myParentEdges.erase(it);
    } else {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructureChildren::removeParentElement(GNELane* lane) {
    auto it = std::find(myParentLanes.begin(), myParentLanes.end(), lane);
    if (it != myParentLanes.end()) {
        myParentLanes.erase(it);
    } else {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructureChildren::removeParentElement(GNEAdditional* additional) {
    auto it = std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional);
    if (it != myParentAdditionals.end()) {
        myParentAdditionals.erase(it);
    } else {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructureChildren::removeParentElement(GNETAZSourceSink* TAZSourceSink) {
    auto it = std::find(myParentTAZSourceSinks.begin(), myParentTAZSourceSinks.end(), TAZSourceSink);
    if (it != myParentTAZSourceSinks.end()) {
        myParentTAZSourceSinks.erase(it);
    } else {
        throw ProcessError(TAZSourceSink->getTagStr() + " with ID='" + TAZSourceSink->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructureChildren::removeParentElement(GNEDemandElement* demandElement) {
    auto it = std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement);
    if (it != myParentDemandElements.end()) {
        myParentDemandElements.erase(it);
    } else {
        throw ProcessError(demandElement->getTagStr() + " is not a parent element");
    }
}


template <> void
GNEHierarchicalStructureChildren::removeParentElement(GNEGenericData* genericData) {
    auto it = std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData);
    if (it != myParentGenericDatas.end()) {
        myParentGenericDatas.erase(it);
    } else {
        throw ProcessError(genericData->getTagStr() + " is not a parent element");
    }
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


template<> const GNEHierarchicalContainerParents<GNEJunction*>&
GNEHierarchicalStructureChildren::getParents() const {
    return myParentJunctions;
}


template<> const GNEHierarchicalContainerParents<GNEEdge*>&
GNEHierarchicalStructureChildren::getParents() const {
    return myParentEdges;
}


template<> const GNEHierarchicalContainerParents<GNELane*>&
GNEHierarchicalStructureChildren::getParents() const {
    return myParentLanes;
}


template<> const GNEHierarchicalContainerParents<GNEAdditional*>&
GNEHierarchicalStructureChildren::getParents() const {
    return myParentAdditionals;
}


template<> const GNEHierarchicalContainerParents<GNETAZSourceSink*>&
GNEHierarchicalStructureChildren::getParents() const {
    return myParentTAZSourceSinks;
}


template<> const GNEHierarchicalContainerParents<GNEDemandElement*>&
GNEHierarchicalStructureChildren::getParents() const {
    return myParentDemandElements;
}


template<> const GNEHierarchicalContainerParents<GNEGenericData*>&
GNEHierarchicalStructureChildren::getParents() const {
    return myParentGenericDatas;
}


template<> void
GNEHierarchicalStructureChildren::setParents(const GNEHierarchicalContainerParents<GNEJunction*>& newParents) {
    myParentJunctions = newParents;
}


template<> void
GNEHierarchicalStructureChildren::setParents(const GNEHierarchicalContainerParents<GNEEdge*>& newParents) {
    myParentEdges = newParents;
}


template<> void
GNEHierarchicalStructureChildren::setParents(const GNEHierarchicalContainerParents<GNELane*>& newParents) {
    myParentLanes = newParents;
}


template<> void
GNEHierarchicalStructureChildren::setParents(const GNEHierarchicalContainerParents<GNEAdditional*>& newParents) {
    myParentAdditionals = newParents;
}


template<> void
GNEHierarchicalStructureChildren::setParents(const GNEHierarchicalContainerParents<GNETAZSourceSink*>& newParents) {
    myParentTAZSourceSinks = newParents;
}


template<> void
GNEHierarchicalStructureChildren::setParents(const GNEHierarchicalContainerParents<GNEDemandElement*>& newParents) {
    myParentDemandElements = newParents;
}


template<> void
GNEHierarchicalStructureChildren::setParents(const GNEHierarchicalContainerParents<GNEGenericData*>& newParents) {
    myParentGenericDatas = newParents;
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
