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
/// @file    GNEHierarchicalElementHelper.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// Helper class for GNEHierarchicalElements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/elements/network/GNEJunction.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>


// ===========================================================================
// member method definitions
// ===========================================================================

GNEHierarchicalElementHelper::HierarchicalContainer::HierarchicalContainer() {}


GNEHierarchicalElementHelper::HierarchicalContainer::HierarchicalContainer(
	const std::vector<GNEJunction*>& _parentJunctions,
	const std::vector<GNEEdge*>& _parentEdges,
	const std::vector<GNELane*>& _parentLanes,
	const std::vector<GNEAdditional*>& _parentAdditionals,
	const std::vector<GNEShape*>& _parentShapes,
	const std::vector<GNETAZElement*>& _parentTAZElements,
	const std::vector<GNEDemandElement*>& _ParentDemandElements,
	const std::vector<GNEGenericData*>& _parentGenericDatas,
	const std::vector<GNEJunction*>& _childJunctions,
	const std::vector<GNEEdge*>& _childEdges,
	const std::vector<GNELane*>& _childLanes,
	const std::vector<GNEAdditional*>& _childAdditionals,
	const std::vector<GNEShape*>& _childShapes,
	const std::vector<GNETAZElement*>& _childTAZElements,
	const std::vector<GNEDemandElement*>& _childDemandElements,
	const std::vector<GNEGenericData*>& _childGenericDatas):
	parentJunctions(_parentJunctions),
	parentEdges(_parentEdges),
	parentLanes(_parentLanes),
	parentAdditionals(_parentAdditionals),
	parentShapes(_parentShapes),
	parentTAZElements(_parentTAZElements),
	parentDemandElements(_ParentDemandElements),
	parentGenericDatas(_parentGenericDatas),
	childJunctions(_childJunctions),
	childEdges(_childEdges),
	childLanes(_childLanes),
	childAdditionals(_childAdditionals),
	childShapes(_childShapes),
	childTAZElements(_childTAZElements),
	childDemandElements(_childDemandElements),
    childGenericDatas(_childGenericDatas) {
}


size_t 
GNEHierarchicalElementHelper::HierarchicalContainer::getContainerSize() const {
    return (
        parentJunctions.size() + 
        parentEdges.size() + 
        parentLanes.size() + 
        parentAdditionals.size() + 
        parentShapes.size() + 
        parentTAZElements.size() + 
        parentDemandElements.size() + 
        parentGenericDatas.size() + 
        childJunctions.size() + 
        childEdges.size() + 
        childLanes.size() + 
        childAdditionals.size() + 
        childShapes.size() + 
        childTAZElements.size() + 
        childDemandElements.size() + 
        childGenericDatas.size()
    );
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNEJunction* junction) {
    // check junction
    if (std::find(parentJunctions.begin(), parentJunctions.end(), junction) != parentJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentJunctions.push_back(junction);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNEEdge* edge) {
    // check edge
    if (std::find(parentEdges.begin(), parentEdges.end(), edge) != parentEdges.end()) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentEdges.push_back(edge);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNELane* lane) {
    // check lane
    if (std::find(parentLanes.begin(), parentLanes.end(), lane) != parentLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentLanes.push_back(lane);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNEAdditional* additional) {
    // check additional
    if (std::find(parentAdditionals.begin(), parentAdditionals.end(), additional) != parentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentAdditionals.push_back(additional);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNEShape* shape) {
    // check shape
    if (std::find(parentShapes.begin(), parentShapes.end(), shape) != parentShapes.end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentShapes.push_back(shape);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNETAZElement* TAZElement) {
    // check TAZElement
    if (std::find(parentTAZElements.begin(), parentTAZElements.end(), TAZElement) != parentTAZElements.end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentTAZElements.push_back(TAZElement);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNEDemandElement* demandElement) {
    // check TAZElement
    if (std::find(parentDemandElements.begin(), parentDemandElements.end(), demandElement) != parentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentDemandElements.push_back(demandElement);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNEGenericData* genericData) {
    // check generic data
    if (std::find(parentGenericDatas.begin(), parentGenericDatas.end(), genericData) != parentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentGenericDatas.push_back(genericData);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNEJunction* junction) {
    // check junction
    auto it = std::find(parentJunctions.begin(), parentJunctions.end(), junction);
    if (it == parentJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentJunctions.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNEEdge* edge) {
    // check edge
    auto it = std::find(parentEdges.begin(), parentEdges.end(), edge);
    if (it == parentEdges.end()) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentEdges.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNELane* lane) {
    // check lane
    auto it = std::find(parentLanes.begin(), parentLanes.end(), lane);
    if (it == parentLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentLanes.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNEAdditional* additional) {
    // check additional
    auto it = std::find(parentAdditionals.begin(), parentAdditionals.end(), additional);
    if (it == parentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentAdditionals.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNEShape* shape) {
    // check shape
    auto it = std::find(parentShapes.begin(), parentShapes.end(), shape);
    if (it == parentShapes.end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentShapes.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNETAZElement* TAZElement) {
    // check TAZElement
    auto it = std::find(parentTAZElements.begin(), parentTAZElements.end(), TAZElement);
    if (it == parentTAZElements.end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentTAZElements.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNEDemandElement* demandElement) {
    // check TAZElement
    auto it = std::find(parentDemandElements.begin(), parentDemandElements.end(), demandElement);
    if (it == parentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentDemandElements.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNEGenericData* genericData) {
    // check generic data
    auto it = std::find(parentGenericDatas.begin(), parentGenericDatas.end(), genericData);
    if (it == parentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentGenericDatas.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNEJunction* junction) {
    // check junction
    if (std::find(childJunctions.begin(), childJunctions.end(), junction) != childJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childJunctions.push_back(junction);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNEEdge* edge) {
    // check edge
    if (std::find(childEdges.begin(), childEdges.end(), edge) != childEdges.end()) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childEdges.push_back(edge);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNELane* lane) {
    // check lane
    if (std::find(childLanes.begin(), childLanes.end(), lane) != childLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childLanes.push_back(lane);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNEAdditional* additional) {
    // check additional
    if (std::find(childAdditionals.begin(), childAdditionals.end(), additional) != childAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childAdditionals.push_back(additional);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNEShape* shape) {
    // check shape
    if (std::find(childShapes.begin(), childShapes.end(), shape) != childShapes.end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childShapes.push_back(shape);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNETAZElement* TAZElement) {
    // check TAZElement
    if (std::find(childTAZElements.begin(), childTAZElements.end(), TAZElement) != childTAZElements.end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childTAZElements.push_back(TAZElement);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNEDemandElement* demandElement) {
    // check demand element
    if (std::find(childDemandElements.begin(), childDemandElements.end(), demandElement) != childDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childDemandElements.push_back(demandElement);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNEGenericData* genericData) {
    // check generic data
    if (std::find(childGenericDatas.begin(), childGenericDatas.end(), genericData) != childGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childGenericDatas.push_back(genericData);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNEJunction* junction) {
    // check junction
    auto it = std::find(childJunctions.begin(), childJunctions.end(), junction);
    if (it == childJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childJunctions.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNEEdge* edge) {
    // check edge
    auto it = std::find(childEdges.begin(), childEdges.end(), edge);
    if (it == childEdges.end()) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childEdges.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNELane* lane) {
    // check lane
    auto it = std::find(childLanes.begin(), childLanes.end(), lane);
    if (it == childLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childLanes.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNEAdditional* additional) {
    // check additional
    auto it = std::find(childAdditionals.begin(), childAdditionals.end(), additional);
    if (it == childAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childAdditionals.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNEShape* shape) {
    // check shape
    auto it = std::find(childShapes.begin(), childShapes.end(), shape);
    if (it == childShapes.end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childShapes.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNETAZElement* TAZElement) {
    // check TAZElement
    auto it = std::find(childTAZElements.begin(), childTAZElements.end(), TAZElement);
    if (it == childTAZElements.end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childTAZElements.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNEDemandElement* demandElement) {
    // check demand element
    auto it = std::find(childDemandElements.begin(), childDemandElements.end(), demandElement);
    if (it == childDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childDemandElements.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::HierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNEGenericData* genericData) {
    // check generic data
    auto it = std::find(childGenericDatas.begin(), childGenericDatas.end(), genericData);
    if (it == childGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childGenericDatas.erase(it);
    }
}


template<> const std::vector<GNEJunction*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getParents() const {
    return parentJunctions;
}


template<> const std::vector<GNEEdge*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getParents() const {
    return parentEdges;
}


template<> const std::vector<GNELane*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getParents() const {
    return parentLanes;
}


template<> const std::vector<GNEAdditional*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getParents() const {
    return parentAdditionals;
}


template<> const std::vector<GNEShape*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getParents() const {
    return parentShapes;
}


template<> const std::vector<GNETAZElement*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getParents() const {
    return parentTAZElements;
}


template<> const std::vector<GNEDemandElement*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getParents() const {
    return parentDemandElements;
}


template<> const std::vector<GNEGenericData*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getParents() const {
    return parentGenericDatas;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setParents(const std::vector<GNEJunction*>& newParents) {
    parentJunctions = newParents;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setParents(const std::vector<GNEEdge*>& newParents) {
     parentEdges = newParents;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setParents(const std::vector<GNELane*>& newParents) {
     parentLanes = newParents;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setParents(const std::vector<GNEAdditional*>& newParents) {
     parentAdditionals = newParents;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setParents(const std::vector<GNEShape*>& newParents) {
     parentShapes = newParents;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setParents(const std::vector<GNETAZElement*>& newParents) {
     parentTAZElements = newParents;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setParents(const std::vector<GNEDemandElement*>& newParents) {
     parentDemandElements = newParents;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setParents(const std::vector<GNEGenericData*>& newParents) {
     parentGenericDatas = newParents;
}


template<> const std::vector<GNEJunction*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getChildren() const {
    return childJunctions;
}


template<> const std::vector<GNEEdge*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getChildren() const {
    return childEdges;
}


template<> const std::vector<GNELane*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getChildren() const {
    return childLanes;
}


template<> const std::vector<GNEAdditional*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getChildren() const {
    return childAdditionals;
}


template<> const std::vector<GNEShape*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getChildren() const {
    return childShapes;
}


template<> const std::vector<GNETAZElement*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getChildren() const {
    return childTAZElements;
}


template<> const std::vector<GNEDemandElement*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getChildren() const {
    return childDemandElements;
}


template<> const std::vector<GNEGenericData*>&
GNEHierarchicalElementHelper::HierarchicalContainer::getChildren() const {
    return childGenericDatas;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setChildren(const std::vector<GNEJunction*>& newChildren) {
    childJunctions = newChildren;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setChildren(const std::vector<GNEEdge*>& newChildren) {
    childEdges = newChildren;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setChildren(const std::vector<GNELane*>& newChildren) {
    childLanes = newChildren;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setChildren(const std::vector<GNEAdditional*>& newChildren) {
    childAdditionals = newChildren;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setChildren(const std::vector<GNEShape*>& newChildren) {
    childShapes = newChildren;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setChildren(const std::vector<GNETAZElement*>& newChildren) {
    childTAZElements = newChildren;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setChildren(const std::vector<GNEDemandElement*>& newChildren) {
    childDemandElements = newChildren;
}


template<> void
GNEHierarchicalElementHelper::HierarchicalContainer::setChildren(const std::vector<GNEGenericData*>& newChildren) {
    childGenericDatas = newChildren;
}

/****************************************************************************/
