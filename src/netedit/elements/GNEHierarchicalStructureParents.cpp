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


template <> void
GNEHierarchicalStructureParents::addParentElement(GNEJunction* element, const int index) {
    if (index == -1) {
        myParentJunctions.push_back(element);
    } else {
        myParentJunctions.insert(myParentJunctions.begin() + index, element);
    }
}


template <> void
GNEHierarchicalStructureParents::addParentElement(GNEEdge* element, const int index) {
    if (index == -1) {
        myParentEdges.push_back(element);
    } else {
        myParentEdges.insert(myParentEdges.begin() + index, element);
    }
}


template <> void
GNEHierarchicalStructureParents::addParentElement(GNELane* element, const int index) {
    if (index == -1) {
        myParentLanes.push_back(element);
    } else {
        myParentLanes.insert(myParentLanes.begin() + index, element);
    }
}


template <> void
GNEHierarchicalStructureParents::addParentElement(GNEAdditional* element, const int index) {
    if (index == -1) {
        myParentAdditionals.push_back(element);
    } else {
        myParentAdditionals.insert(myParentAdditionals.begin() + index, element);
    }
}


template <> void
GNEHierarchicalStructureParents::addParentElement(GNETAZSourceSink* element, const int index) {
    if (index == -1) {
        myParentTAZSourceSinks.push_back(element);
    } else {
        myParentTAZSourceSinks.insert(myParentTAZSourceSinks.begin() + index, element);
    }
}


template <> void
GNEHierarchicalStructureParents::addParentElement(GNEDemandElement* element, const int index) {
    if (index == -1) {
        myParentDemandElements.push_back(element);
    } else {
        myParentDemandElements.insert(myParentDemandElements.begin() + index, element);
    }
}


template <> void
GNEHierarchicalStructureParents::addParentElement(GNEGenericData* element, const int index) {
    if (index == -1) {
        myParentGenericDatas.push_back(element);
    } else {
        myParentGenericDatas.insert(myParentGenericDatas.begin() + index, element);
    }
}


template <> void
GNEHierarchicalStructureParents::updateParentElement(const int index, GNEJunction* element) {
    if (index >= 0 && index < (int)myParentJunctions.size()) {
        myParentJunctions[index] = element;
    } else {
        throw ProcessError("Invalid index " + toString(element->getID()) + " updating junction parents");
    }
}


template <> void
GNEHierarchicalStructureParents::updateParentElement(const int index, GNEEdge* element) {
    if (index >= 0 && index < (int)myParentEdges.size()) {
        myParentEdges[index] = element;
    } else {
        throw ProcessError("Invalid index " + toString(element->getID()) + " updating edge parents");
    }
}


template <> void
GNEHierarchicalStructureParents::updateParentElement(const int index, GNELane* element) {
    if (index >= 0 && index < (int)myParentLanes.size()) {
        myParentLanes[index] = element;
    } else {
        throw ProcessError("Invalid index " + toString(element->getID()) + " updating lane parents");
    }
}


template <> void
GNEHierarchicalStructureParents::updateParentElement(const int index, GNEAdditional* element) {
    if (index >= 0 && index < (int)myParentAdditionals.size()) {
        myParentAdditionals[index] = element;
    } else {
        throw ProcessError("Invalid index " + toString(element->getID()) + " updating additional parents");
    }
}


template <> void
GNEHierarchicalStructureParents::updateParentElement(const int index, GNETAZSourceSink* element) {
    if (index >= 0 && index < (int)myParentTAZSourceSinks.size()) {
        myParentTAZSourceSinks[index] = element;
    } else {
        throw ProcessError("Invalid index " + toString(element->getID()) + " updating sourceSink parents");
    }
}


template <> void
GNEHierarchicalStructureParents::updateParentElement(const int index, GNEDemandElement* element) {
    if (index >= 0 && index < (int)myParentDemandElements.size()) {
        myParentDemandElements[index] = element;
    } else {
        throw ProcessError("Invalid index " + toString(element->getID()) + " updating demand element parents");
    }
}


template <> void
GNEHierarchicalStructureParents::updateParentElement(const int index, GNEGenericData* element) {
    if (index >= 0 && index < (int)myParentGenericDatas.size()) {
        myParentGenericDatas[index] = element;
    } else {
        throw ProcessError("Invalid index " + toString(element->getID()) + " updating generic data parents");
    }
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

/****************************************************************************/
