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


void
GNEHierarchicalStructureParents::clear() {
    myParentJunctions.clear();
    myParentEdges.clear();
    myParentLanes.clear();
    myParentAdditionals.clear();
    myParentTAZSourceSinks.clear();
    myParentDemandElements.clear();
    myParentGenericDatas.clear();
}

// get functions

template<> const GNEHierarchicalContainerParents<GNEJunction*>&
GNEHierarchicalStructureParents::get() const {
    return myParentJunctions;
}


template<> const GNEHierarchicalContainerParents<GNEEdge*>&
GNEHierarchicalStructureParents::get() const {
    return myParentEdges;
}


template<> const GNEHierarchicalContainerParents<GNELane*>&
GNEHierarchicalStructureParents::get() const {
    return myParentLanes;
}


template<> const GNEHierarchicalContainerParents<GNEAdditional*>&
GNEHierarchicalStructureParents::get() const {
    return myParentAdditionals;
}


template<> const GNEHierarchicalContainerParents<GNETAZSourceSink*>&
GNEHierarchicalStructureParents::get() const {
    return myParentTAZSourceSinks;
}


template<> const GNEHierarchicalContainerParents<GNEDemandElement*>&
GNEHierarchicalStructureParents::get() const {
    return myParentDemandElements;
}


template<> const GNEHierarchicalContainerParents<GNEGenericData*>&
GNEHierarchicalStructureParents::get() const {
    return myParentGenericDatas;
}

// at functions

template<> GNEJunction*
GNEHierarchicalStructureParents::at(const int index) const {
    return myParentJunctions.at(index);
}


template<> GNEEdge*
GNEHierarchicalStructureParents::at(const int index) const {
    return myParentEdges.at(index);
}


template<> GNELane*
GNEHierarchicalStructureParents::at(const int index) const {
    return myParentLanes.at(index);
}


template<> GNEAdditional*
GNEHierarchicalStructureParents::at(const int index) const {
    return myParentAdditionals.at(index);
}


template<> GNETAZSourceSink*
GNEHierarchicalStructureParents::at(const int index) const {
    return myParentTAZSourceSinks.at(index);
}


template<> GNEDemandElement*
GNEHierarchicalStructureParents::at(const int index) const {
    return myParentDemandElements.at(index);
}


template<> GNEGenericData*
GNEHierarchicalStructureParents::at(const int index) const {
    return myParentGenericDatas.at(index);
}

// add functions

template <> void
GNEHierarchicalStructureParents::add(GNEJunction* parent, const int index) {
    if (index == -1) {
        myParentJunctions.push_back(parent);
    } else if ((index < 0) || (index > (int)myParentJunctions.size())) {
        myParentJunctions.insert(myParentJunctions.begin() + index, parent);
    } else {
        throw ProcessError("invalid index in GNEHierarchicalStructureParents::add");
    }
}


template <> void
GNEHierarchicalStructureParents::add(GNEEdge* parent, const int index) {
    if (index == -1) {
        myParentEdges.push_back(parent);
    } else if ((index < 0) || (index > (int)myParentEdges.size())) {
        myParentEdges.insert(myParentEdges.begin() + index, parent);
    } else {
        throw ProcessError("invalid index in GNEHierarchicalStructureParents::add");
    }
}


template <> void
GNEHierarchicalStructureParents::add(GNELane* parent, const int index) {
    if (index == -1) {
        myParentLanes.push_back(parent);
    } else if ((index < 0) || (index > (int)myParentLanes.size())) {
        myParentLanes.insert(myParentLanes.begin() + index, parent);
    } else {
        throw ProcessError("invalid index in GNEHierarchicalStructureParents::add");
    }
}


template <> void
GNEHierarchicalStructureParents::add(GNEAdditional* parent, const int index) {
    if (index == -1) {
        myParentAdditionals.push_back(parent);
    } else if ((index < 0) || (index > (int)myParentAdditionals.size())) {
        myParentAdditionals.insert(myParentAdditionals.begin() + index, parent);
    } else {
        throw ProcessError("invalid index in GNEHierarchicalStructureParents::add");
    }
}


template <> void
GNEHierarchicalStructureParents::add(GNETAZSourceSink* parent, const int index) {
    if (index == -1) {
        myParentTAZSourceSinks.push_back(parent);
    } else if ((index < 0) || (index > (int)myParentTAZSourceSinks.size())) {
        myParentTAZSourceSinks.insert(myParentTAZSourceSinks.begin() + index, parent);
    } else {
        throw ProcessError("invalid index in GNEHierarchicalStructureParents::add");
    }
}


template <> void
GNEHierarchicalStructureParents::add(GNEDemandElement* parent, const int index) {
    if (index == -1) {
        myParentDemandElements.push_back(parent);
    } else if ((index < 0) || (index > (int)myParentDemandElements.size())) {
        myParentDemandElements.insert(myParentDemandElements.begin() + index, parent);
    } else {
        throw ProcessError("invalid index in GNEHierarchicalStructureParents::add");
    }
}


template <> void
GNEHierarchicalStructureParents::add(GNEGenericData* parent, const int index) {
    if (index == -1) {
        myParentGenericDatas.push_back(parent);
    } else if ((index < 0) || (index > (int)myParentGenericDatas.size())) {
        myParentGenericDatas.insert(myParentGenericDatas.begin() + index, parent);
    } else {
        throw ProcessError("invalid index in GNEHierarchicalStructureParents::add");
    }
}

// remove functions

template <> void
GNEHierarchicalStructureParents::remove(GNEJunction* junction) {
    auto it = std::find(myParentJunctions.begin(), myParentJunctions.end(), junction);
    if (it != myParentJunctions.end()) {
        myParentJunctions.erase(it);
    } else {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' is not a parent parent");
    }
}


template <> void
GNEHierarchicalStructureParents::remove(GNEEdge* edge) {
    auto it = std::find(myParentEdges.begin(), myParentEdges.end(), edge);
    if (it != myParentEdges.end()) {
        myParentEdges.erase(it);
    } else {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' is not a parent parent");
    }
}


template <> void
GNEHierarchicalStructureParents::remove(GNELane* lane) {
    auto it = std::find(myParentLanes.begin(), myParentLanes.end(), lane);
    if (it != myParentLanes.end()) {
        myParentLanes.erase(it);
    } else {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' is not a parent parent");
    }
}


template <> void
GNEHierarchicalStructureParents::remove(GNEAdditional* additional) {
    auto it = std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional);
    if (it != myParentAdditionals.end()) {
        myParentAdditionals.erase(it);
    } else {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' is not a parent parent");
    }
}


template <> void
GNEHierarchicalStructureParents::remove(GNETAZSourceSink* TAZSourceSink) {
    auto it = std::find(myParentTAZSourceSinks.begin(), myParentTAZSourceSinks.end(), TAZSourceSink);
    if (it != myParentTAZSourceSinks.end()) {
        myParentTAZSourceSinks.erase(it);
    } else {
        throw ProcessError(TAZSourceSink->getTagStr() + " with ID='" + TAZSourceSink->getID() + "' is not a parent parent");
    }
}


template <> void
GNEHierarchicalStructureParents::remove(GNEDemandElement* demandElement) {
    auto it = std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement);
    if (it != myParentDemandElements.end()) {
        myParentDemandElements.erase(it);
    } else {
        throw ProcessError(demandElement->getTagStr() + " is not a parent parent");
    }
}


template <> void
GNEHierarchicalStructureParents::remove(GNEGenericData* genericData) {
    auto it = std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData);
    if (it != myParentGenericDatas.end()) {
        myParentGenericDatas.erase(it);
    } else {
        throw ProcessError(genericData->getTagStr() + " is not a parent parent");
    }
}

// updateParentElement functions

template <> void
GNEHierarchicalStructureParents::replaceSingle(const int index, GNEJunction* newParent) {
    if ((index >= 0) && (index < (int)myParentJunctions.size())) {
        myParentJunctions[index] = newParent;
    } else {
        throw ProcessError("Invalid index " + toString(newParent->getID()) + " updating junction parents");
    }
}


template <> void
GNEHierarchicalStructureParents::replaceSingle(const int index, GNEEdge* newParent) {
    if ((index >= 0) && (index < (int)myParentEdges.size())) {
        myParentEdges[index] = newParent;
    } else {
        throw ProcessError("Invalid index " + toString(newParent->getID()) + " updating edge parents");
    }
}


template <> void
GNEHierarchicalStructureParents::replaceSingle(const int index, GNELane* newParent) {
    if ((index >= 0) && (index < (int)myParentLanes.size())) {
        myParentLanes[index] = newParent;
    } else {
        throw ProcessError("Invalid index " + toString(newParent->getID()) + " updating lane parents");
    }
}


template <> void
GNEHierarchicalStructureParents::replaceSingle(const int index, GNEAdditional* newParent) {
    if ((index >= 0) && (index < (int)myParentAdditionals.size())) {
        myParentAdditionals[index] = newParent;
    } else {
        throw ProcessError("Invalid index " + toString(newParent->getID()) + " updating additional parents");
    }
}


template <> void
GNEHierarchicalStructureParents::replaceSingle(const int index, GNETAZSourceSink* newParent) {
    if ((index >= 0) && (index < (int)myParentTAZSourceSinks.size())) {
        myParentTAZSourceSinks[index] = newParent;
    } else {
        throw ProcessError("Invalid index " + toString(newParent->getID()) + " updating sourceSink parents");
    }
}


template <> void
GNEHierarchicalStructureParents::replaceSingle(const int index, GNEDemandElement* newParent) {
    if ((index >= 0) && (index < (int)myParentDemandElements.size())) {
        myParentDemandElements[index] = newParent;
    } else {
        throw ProcessError("Invalid index " + toString(newParent->getID()) + " updating demand parent parents");
    }
}


template <> void
GNEHierarchicalStructureParents::replaceSingle(const int index, GNEGenericData* newParent) {
    if ((index >= 0) && (index < (int)myParentGenericDatas.size())) {
        myParentGenericDatas[index] = newParent;
    } else {
        throw ProcessError("Invalid index " + toString(newParent->getID()) + " updating generic data parents");
    }
}

// updateParents functions

template <> void
GNEHierarchicalStructureParents::replaceAll(const GNEHierarchicalContainerParents<GNEJunction*>& newParents) {
    myParentJunctions = newParents;
}


template <> void
GNEHierarchicalStructureParents::replaceAll(const GNEHierarchicalContainerParents<GNEEdge*>& newParents) {
    myParentEdges = newParents;
}


template <> void
GNEHierarchicalStructureParents::replaceAll(const GNEHierarchicalContainerParents<GNELane*>& newParents) {
    myParentLanes = newParents;
}


template <> void
GNEHierarchicalStructureParents::replaceAll(const GNEHierarchicalContainerParents<GNEAdditional*>& newParents) {
    myParentAdditionals = newParents;
}


template <> void
GNEHierarchicalStructureParents::replaceAll(const GNEHierarchicalContainerParents<GNETAZSourceSink*>& newParents) {
    myParentTAZSourceSinks = newParents;
}


template <> void
GNEHierarchicalStructureParents::replaceAll(const GNEHierarchicalContainerParents<GNEDemandElement*>& newParents) {
    myParentDemandElements = newParents;
}


template <> void
GNEHierarchicalStructureParents::replaceAll(const GNEHierarchicalContainerParents<GNEGenericData*>& newParents) {
    myParentGenericDatas = newParents;
}

/****************************************************************************/
