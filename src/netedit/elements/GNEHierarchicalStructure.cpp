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
/// @file    GNEHierarchicalStructure.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// Structure for GNEHierarchicalElements
/****************************************************************************/

#include <netedit/elements/network/GNEJunction.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNETAZSourceSink.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <utils/common/UtilExceptions.h>

#include "GNEHierarchicalStructure.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEHierarchicalStructure::GNEHierarchicalStructure() {}


GNEHierarchicalStructure::GNEHierarchicalStructure(
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
GNEHierarchicalStructure::getContainerSize() const {
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
GNEHierarchicalStructure::addParentElement(GNEJunction* junction) {
    myParentJunctions.push_back(junction);
}


template <> void
GNEHierarchicalStructure::addParentElement(GNEEdge* edge) {
    myParentEdges.push_back(edge);
}


template <> void
GNEHierarchicalStructure::addParentElement(GNELane* lane) {
    myParentLanes.push_back(lane);
}


template <> void
GNEHierarchicalStructure::addParentElement(GNEAdditional* additional) {
    myParentAdditionals.push_back(additional);
}


template <> void
GNEHierarchicalStructure::addParentElement(GNETAZSourceSink* TAZSourceSink) {
    myParentTAZSourceSinks.push_back(TAZSourceSink);
}


template <> void
GNEHierarchicalStructure::addParentElement(GNEDemandElement* demandElement) {
    myParentDemandElements.push_back(demandElement);
}


template <> void
GNEHierarchicalStructure::addParentElement(GNEGenericData* genericData) {
    myParentGenericDatas.push_back(genericData);
}


template <> void
GNEHierarchicalStructure::removeParentElement(GNEJunction* junction) {
    auto it = std::find(myParentJunctions.begin(), myParentJunctions.end(), junction);
    if (it != myParentJunctions.end()) {
        myParentJunctions.erase(it);
    } else {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructure::removeParentElement(GNEEdge* edge) {
    auto it = std::find(myParentEdges.begin(), myParentEdges.end(), edge);
    if (it != myParentEdges.end()) {
        myParentEdges.erase(it);
    } else {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructure::removeParentElement(GNELane* lane) {
    auto it = std::find(myParentLanes.begin(), myParentLanes.end(), lane);
    if (it != myParentLanes.end()) {
        myParentLanes.erase(it);
    } else {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructure::removeParentElement(GNEAdditional* additional) {
    auto it = std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional);
    if (it != myParentAdditionals.end()) {
        myParentAdditionals.erase(it);
    } else {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructure::removeParentElement(GNETAZSourceSink* TAZSourceSink) {
    auto it = std::find(myParentTAZSourceSinks.begin(), myParentTAZSourceSinks.end(), TAZSourceSink);
    if (it != myParentTAZSourceSinks.end()) {
        myParentTAZSourceSinks.erase(it);
    } else {
        throw ProcessError(TAZSourceSink->getTagStr() + " with ID='" + TAZSourceSink->getID() + "' is not a parent element");
    }
}


template <> void
GNEHierarchicalStructure::removeParentElement(GNEDemandElement* demandElement) {
    auto it = std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement);
    if (it != myParentDemandElements.end()) {
        myParentDemandElements.erase(it);
    } else {
        throw ProcessError(demandElement->getTagStr() + " is not a parent element");
    }
}


template <> void
GNEHierarchicalStructure::removeParentElement(GNEGenericData* genericData) {
    auto it = std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData);
    if (it != myParentGenericDatas.end()) {
        myParentGenericDatas.erase(it);
    } else {
        throw ProcessError(genericData->getTagStr() + " is not a parent element");
    }
}


template <> void
GNEHierarchicalStructure::addChildElement(GNEJunction* junction) {
    myChildJunctions.push_back(junction);
}


template <> void
GNEHierarchicalStructure::addChildElement(GNEEdge* edge) {
    myChildEdges.push_back(edge);
}


template <> void
GNEHierarchicalStructure::addChildElement(GNELane* lane) {
    myChildLanes.push_back(lane);
}


template <> void
GNEHierarchicalStructure::addChildElement(GNEAdditional* additional) {
    myChildAdditionals.push_back(additional);
}


template <> void
GNEHierarchicalStructure::addChildElement(GNETAZSourceSink* TAZSourceSink) {
    myChildSourceSinks.insert(TAZSourceSink);
}


template <> void
GNEHierarchicalStructure::addChildElement(GNEDemandElement* demandElement) {
    myChildDemandElements.push_back(demandElement);
}


template <> void
GNEHierarchicalStructure::addChildElement(GNEGenericData* genericData) {
    myChildGenericDatas.push_back(genericData);
}


template <> void
GNEHierarchicalStructure::removeChildElement(GNEJunction* junction) {
    auto it = std::find(myChildJunctions.begin(), myChildJunctions.end(), junction);
    if (it != myChildJunctions.end()) {
        myChildJunctions.erase(it);
    } else {
        throw ProcessError(junction->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructure::removeChildElement(GNEEdge* edge) {
    auto it = std::find(myChildEdges.begin(), myChildEdges.end(), edge);
    if (it != myChildEdges.end()) {
        myChildEdges.erase(it);
    } else {
        throw ProcessError(edge->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructure::removeChildElement(GNELane* lane) {
    auto it = std::find(myChildLanes.begin(), myChildLanes.end(), lane);
    if (it != myChildLanes.end()) {
        myChildLanes.erase(it);
    } else {
        throw ProcessError(lane->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructure::removeChildElement(GNEAdditional* additional) {
    auto it = std::find(myChildAdditionals.begin(), myChildAdditionals.end(), additional);
    if (it != myChildAdditionals.end()) {
        myChildAdditionals.erase(it);
    } else {
        throw ProcessError(additional->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructure::removeChildElement(GNETAZSourceSink* TAZSourceSink) {
    auto it = myChildSourceSinks.find(TAZSourceSink);
    if (it != myChildSourceSinks.end()) {
        myChildSourceSinks.erase(it);
    } else {
        throw ProcessError(TAZSourceSink->getTagStr() + " is not a child element");
    }
}

template <> void
GNEHierarchicalStructure::removeChildElement(GNEDemandElement* demandElement) {
    auto it = std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement);
    if (it != myChildDemandElements.end()) {
        myChildDemandElements.erase(it);
    } else {
        throw ProcessError(demandElement->getTagStr() + " is not a child element");
    }
}


template <> void
GNEHierarchicalStructure::removeChildElement(GNEGenericData* genericData) {
    auto it = std::find(myChildGenericDatas.begin(), myChildGenericDatas.end(), genericData);
    if (it != myChildGenericDatas.end()) {
        myChildGenericDatas.erase(it);
    } else {
        throw ProcessError(genericData->getTagStr() + " is not a child element");
    }
}


template<> const GNEHierarchicalContainerParents<GNEJunction*>&
GNEHierarchicalStructure::getParents() const {
    return myParentJunctions;
}


template<> const GNEHierarchicalContainerParents<GNEEdge*>&
GNEHierarchicalStructure::getParents() const {
    return myParentEdges;
}


template<> const GNEHierarchicalContainerParents<GNELane*>&
GNEHierarchicalStructure::getParents() const {
    return myParentLanes;
}


template<> const GNEHierarchicalContainerParents<GNEAdditional*>&
GNEHierarchicalStructure::getParents() const {
    return myParentAdditionals;
}


template<> const GNEHierarchicalContainerParents<GNETAZSourceSink*>&
GNEHierarchicalStructure::getParents() const {
    return myParentTAZSourceSinks;
}


template<> const GNEHierarchicalContainerParents<GNEDemandElement*>&
GNEHierarchicalStructure::getParents() const {
    return myParentDemandElements;
}


template<> const GNEHierarchicalContainerParents<GNEGenericData*>&
GNEHierarchicalStructure::getParents() const {
    return myParentGenericDatas;
}


template<> void
GNEHierarchicalStructure::setParents(const GNEHierarchicalContainerParents<GNEJunction*>& newParents) {
    myParentJunctions = newParents;
}


template<> void
GNEHierarchicalStructure::setParents(const GNEHierarchicalContainerParents<GNEEdge*>& newParents) {
    myParentEdges = newParents;
}


template<> void
GNEHierarchicalStructure::setParents(const GNEHierarchicalContainerParents<GNELane*>& newParents) {
    myParentLanes = newParents;
}


template<> void
GNEHierarchicalStructure::setParents(const GNEHierarchicalContainerParents<GNEAdditional*>& newParents) {
    myParentAdditionals = newParents;
}


template<> void
GNEHierarchicalStructure::setParents(const GNEHierarchicalContainerParents<GNETAZSourceSink*>& newParents) {
    myParentTAZSourceSinks = newParents;
}


template<> void
GNEHierarchicalStructure::setParents(const GNEHierarchicalContainerParents<GNEDemandElement*>& newParents) {
    myParentDemandElements = newParents;
}


template<> void
GNEHierarchicalStructure::setParents(const GNEHierarchicalContainerParents<GNEGenericData*>& newParents) {
    myParentGenericDatas = newParents;
}


template<> const GNEHierarchicalContainerChildren<GNEJunction*>&
GNEHierarchicalStructure::getChildren() const {
    return myChildJunctions;
}


template<> const GNEHierarchicalContainerChildren<GNEEdge*>&
GNEHierarchicalStructure::getChildren() const {
    return myChildEdges;
}


template<> const GNEHierarchicalContainerChildren<GNELane*>&
GNEHierarchicalStructure::getChildren() const {
    return myChildLanes;
}


template<> const GNEHierarchicalContainerChildren<GNEAdditional*>&
GNEHierarchicalStructure::getChildren() const {
    return myChildAdditionals;
}


template<> const GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*>&
GNEHierarchicalStructure::getChildrenSet() const {
    return myChildSourceSinks;
}


template<> const GNEHierarchicalContainerChildren<GNEDemandElement*>&
GNEHierarchicalStructure::getChildren() const {
    return myChildDemandElements;
}


template<> const GNEHierarchicalContainerChildren<GNEGenericData*>&
GNEHierarchicalStructure::getChildren() const {
    return myChildGenericDatas;
}


template<> void
GNEHierarchicalStructure::setChildren(const GNEHierarchicalContainerChildren<GNEJunction*>& newChildren) {
    myChildJunctions = newChildren;
}


template<> void
GNEHierarchicalStructure::setChildren(const GNEHierarchicalContainerChildren<GNEEdge*>& newChildren) {
    myChildEdges = newChildren;
}


template<> void
GNEHierarchicalStructure::setChildren(const GNEHierarchicalContainerChildren<GNELane*>& newChildren) {
    myChildLanes = newChildren;
}


template<> void
GNEHierarchicalStructure::setChildren(const GNEHierarchicalContainerChildren<GNEAdditional*>& newChildren) {
    myChildAdditionals = newChildren;
}


template<> void
GNEHierarchicalStructure::setChildrenSet(const GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*>& newChildren) {
    myChildSourceSinks = newChildren;
}


template<> void
GNEHierarchicalStructure::setChildren(const GNEHierarchicalContainerChildren<GNEDemandElement*>& newChildren) {
    myChildDemandElements = newChildren;
}


template<> void
GNEHierarchicalStructure::setChildren(const GNEHierarchicalContainerChildren<GNEGenericData*>& newChildren) {
    myChildGenericDatas = newChildren;
}

/****************************************************************************/
