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
/// @file    GNEHierarchicalStructureParents.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2025
///
// Structure for GNEHierarchicalElements centered in parents
/****************************************************************************/

#include <netedit/elements/network/GNEJunction.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNETAZSourceSink.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <utils/common/UtilExceptions.h>

#include "GNEHierarchicalStructureParents.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEHierarchicalStructureParents::GNEHierarchicalStructureParents() {}


GNEHierarchicalStructureParents::GNEHierarchicalStructureParents(
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
GNEHierarchicalStructureParents::getContainerSize() const {
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
GNEHierarchicalStructureParents::addParentElement(GNEJunction* junction) {
    myParentJunctions.push_back(junction);
}


template <> void
GNEHierarchicalStructureParents::addParentElement(GNEEdge* edge) {
    myParentEdges.push_back(edge);
}


template <> void
GNEHierarchicalStructureParents::addParentElement(GNELane* lane) {
    myParentLanes.push_back(lane);
}


template <> void
GNEHierarchicalStructureParents::addParentElement(GNEAdditional* additional) {
    myParentAdditionals.push_back(additional);
}


template <> void
GNEHierarchicalStructureParents::addParentElement(GNETAZSourceSink* TAZSourceSink) {
    myParentTAZSourceSinks.push_back(TAZSourceSink);
}


template <> void
GNEHierarchicalStructureParents::addParentElement(GNEDemandElement* demandElement) {
    myParentDemandElements.push_back(demandElement);
}


template <> void
GNEHierarchicalStructureParents::addParentElement(GNEGenericData* genericData) {
    myParentGenericDatas.push_back(genericData);
}


template <> void
GNEHierarchicalStructureParents::removeParentElement(GNEJunction* junction) {
    auto it = std::find(myParentJunctions.begin(), myParentJunctions.end(), junction);
    if (it != myParentJunctions.end()) {
        myParentJunctions.erase(it);
    } else {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructureParents::removeParentElement(GNEEdge* edge) {
    auto it = std::find(myParentEdges.begin(), myParentEdges.end(), edge);
    if (it != myParentEdges.end()) {
        myParentEdges.erase(it);
    } else {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructureParents::removeParentElement(GNELane* lane) {
    auto it = std::find(myParentLanes.begin(), myParentLanes.end(), lane);
    if (it != myParentLanes.end()) {
        myParentLanes.erase(it);
    } else {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructureParents::removeParentElement(GNEAdditional* additional) {
    auto it = std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional);
    if (it != myParentAdditionals.end()) {
        myParentAdditionals.erase(it);
    } else {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructureParents::removeParentElement(GNETAZSourceSink* TAZSourceSink) {
    auto it = std::find(myParentTAZSourceSinks.begin(), myParentTAZSourceSinks.end(), TAZSourceSink);
    if (it != myParentTAZSourceSinks.end()) {
        myParentTAZSourceSinks.erase(it);
    } else {
        throw ProcessError(TAZSourceSink->getTagStr() + " with ID='" + TAZSourceSink->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructureParents::removeParentElement(GNEDemandElement* demandElement) {
    auto it = std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement);
    if (it != myParentDemandElements.end()) {
        myParentDemandElements.erase(it);
    } else {
        throw ProcessError(demandElement->getTagStr() + " is not a parent element");
    }
}


template <> void
GNEHierarchicalStructureParents::removeParentElement(GNEGenericData* genericData) {
    auto it = std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData);
    if (it != myParentGenericDatas.end()) {
        myParentGenericDatas.erase(it);
    } else {
        throw ProcessError(genericData->getTagStr() + " is not a parent element");
    }
}


template <> void
GNEHierarchicalStructureParents::addChildElement(GNEJunction* junction) {
    myChildJunctions.push_back(junction);
}


template <> void
GNEHierarchicalStructureParents::addChildElement(GNEEdge* edge) {
    myChildEdges.push_back(edge);
}


template <> void
GNEHierarchicalStructureParents::addChildElement(GNELane* lane) {
    myChildLanes.push_back(lane);
}


template <> void
GNEHierarchicalStructureParents::addChildElement(GNEAdditional* additional) {
    myChildAdditionals.push_back(additional);
}


template <> void
GNEHierarchicalStructureParents::addChildElement(GNETAZSourceSink* TAZSourceSink) {
    myChildSourceSinks.insert(TAZSourceSink);
}


template <> void
GNEHierarchicalStructureParents::addChildElement(GNEDemandElement* demandElement) {
    myChildDemandElements.push_back(demandElement);
}


template <> void
GNEHierarchicalStructureParents::addChildElement(GNEGenericData* genericData) {
    myChildGenericDatas.push_back(genericData);
}


template <> void
GNEHierarchicalStructureParents::removeChildElement(GNEJunction* junction) {
    auto it = std::find(myChildJunctions.begin(), myChildJunctions.end(), junction);
    if (it != myChildJunctions.end()) {
        myChildJunctions.erase(it);
    } else {
        throw ProcessError(junction->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureParents::removeChildElement(GNEEdge* edge) {
    auto it = std::find(myChildEdges.begin(), myChildEdges.end(), edge);
    if (it != myChildEdges.end()) {
        myChildEdges.erase(it);
    } else {
        throw ProcessError(edge->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureParents::removeChildElement(GNELane* lane) {
    auto it = std::find(myChildLanes.begin(), myChildLanes.end(), lane);
    if (it != myChildLanes.end()) {
        myChildLanes.erase(it);
    } else {
        throw ProcessError(lane->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureParents::removeChildElement(GNEAdditional* additional) {
    auto it = std::find(myChildAdditionals.begin(), myChildAdditionals.end(), additional);
    if (it != myChildAdditionals.end()) {
        myChildAdditionals.erase(it);
    } else {
        throw ProcessError(additional->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureParents::removeChildElement(GNETAZSourceSink* TAZSourceSink) {
    auto it = myChildSourceSinks.find(TAZSourceSink);
    if (it != myChildSourceSinks.end()) {
        myChildSourceSinks.erase(it);
    } else {
        throw ProcessError(TAZSourceSink->getTagStr() + " is not a child element");
    }
}

template <> void
GNEHierarchicalStructureParents::removeChildElement(GNEDemandElement* demandElement) {
    auto it = std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement);
    if (it != myChildDemandElements.end()) {
        myChildDemandElements.erase(it);
    } else {
        throw ProcessError(demandElement->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructureParents::removeChildElement(GNEGenericData* genericData) {
    auto it = std::find(myChildGenericDatas.begin(), myChildGenericDatas.end(), genericData);
    if (it != myChildGenericDatas.end()) {
        myChildGenericDatas.erase(it);
    } else {
        throw ProcessError(genericData->getTagStr() + " is not a child element");
    }
}


template<> const GNEHierarchicalContainerParents<GNEJunction*>&
GNEHierarchicalStructureParents::getParents() const {
    return myParentJunctions;
}


template<> const GNEHierarchicalContainerParents<GNEEdge*>&
GNEHierarchicalStructureParents::getParents() const {
    return myParentEdges;
}


template<> const GNEHierarchicalContainerParents<GNELane*>&
GNEHierarchicalStructureParents::getParents() const {
    return myParentLanes;
}


template<> const GNEHierarchicalContainerParents<GNEAdditional*>&
GNEHierarchicalStructureParents::getParents() const {
    return myParentAdditionals;
}


template<> const GNEHierarchicalContainerParents<GNETAZSourceSink*>&
GNEHierarchicalStructureParents::getParents() const {
    return myParentTAZSourceSinks;
}


template<> const GNEHierarchicalContainerParents<GNEDemandElement*>&
GNEHierarchicalStructureParents::getParents() const {
    return myParentDemandElements;
}


template<> const GNEHierarchicalContainerParents<GNEGenericData*>&
GNEHierarchicalStructureParents::getParents() const {
    return myParentGenericDatas;
}


template<> void
GNEHierarchicalStructureParents::setParents(const GNEHierarchicalContainerParents<GNEJunction*>& newParents) {
    myParentJunctions = newParents;
}


template<> void
GNEHierarchicalStructureParents::setParents(const GNEHierarchicalContainerParents<GNEEdge*>& newParents) {
    myParentEdges = newParents;
}


template<> void
GNEHierarchicalStructureParents::setParents(const GNEHierarchicalContainerParents<GNELane*>& newParents) {
    myParentLanes = newParents;
}


template<> void
GNEHierarchicalStructureParents::setParents(const GNEHierarchicalContainerParents<GNEAdditional*>& newParents) {
    myParentAdditionals = newParents;
}


template<> void
GNEHierarchicalStructureParents::setParents(const GNEHierarchicalContainerParents<GNETAZSourceSink*>& newParents) {
    myParentTAZSourceSinks = newParents;
}


template<> void
GNEHierarchicalStructureParents::setParents(const GNEHierarchicalContainerParents<GNEDemandElement*>& newParents) {
    myParentDemandElements = newParents;
}


template<> void
GNEHierarchicalStructureParents::setParents(const GNEHierarchicalContainerParents<GNEGenericData*>& newParents) {
    myParentGenericDatas = newParents;
}


template<> const GNEHierarchicalContainerChildren<GNEJunction*>&
GNEHierarchicalStructureParents::getChildren() const {
    return myChildJunctions;
}


template<> const GNEHierarchicalContainerChildren<GNEEdge*>&
GNEHierarchicalStructureParents::getChildren() const {
    return myChildEdges;
}


template<> const GNEHierarchicalContainerChildren<GNELane*>&
GNEHierarchicalStructureParents::getChildren() const {
    return myChildLanes;
}


template<> const GNEHierarchicalContainerChildren<GNEAdditional*>&
GNEHierarchicalStructureParents::getChildren() const {
    return myChildAdditionals;
}


template<> const GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*>&
GNEHierarchicalStructureParents::getChildrenSet() const {
    return myChildSourceSinks;
}


template<> const GNEHierarchicalContainerChildren<GNEDemandElement*>&
GNEHierarchicalStructureParents::getChildren() const {
    return myChildDemandElements;
}


template<> const GNEHierarchicalContainerChildren<GNEGenericData*>&
GNEHierarchicalStructureParents::getChildren() const {
    return myChildGenericDatas;
}


template<> void
GNEHierarchicalStructureParents::setChildren(const GNEHierarchicalContainerChildren<GNEJunction*>& newChildren) {
    myChildJunctions = newChildren;
}


template<> void
GNEHierarchicalStructureParents::setChildren(const GNEHierarchicalContainerChildren<GNEEdge*>& newChildren) {
    myChildEdges = newChildren;
}


template<> void
GNEHierarchicalStructureParents::setChildren(const GNEHierarchicalContainerChildren<GNELane*>& newChildren) {
    myChildLanes = newChildren;
}


template<> void
GNEHierarchicalStructureParents::setChildren(const GNEHierarchicalContainerChildren<GNEAdditional*>& newChildren) {
    myChildAdditionals = newChildren;
}


template<> void
GNEHierarchicalStructureParents::setChildrenSet(const GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*>& newChildren) {
    myChildSourceSinks = newChildren;
}


template<> void
GNEHierarchicalStructureParents::setChildren(const GNEHierarchicalContainerChildren<GNEDemandElement*>& newChildren) {
    myChildDemandElements = newChildren;
}


template<> void
GNEHierarchicalStructureParents::setChildren(const GNEHierarchicalContainerChildren<GNEGenericData*>& newChildren) {
    myChildGenericDatas = newChildren;
}

/****************************************************************************/
