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
/// @file    GNEHierarchicalElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// A abstract class for representation of hierarchical elements
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEHierarchicalStructureParents.h"
#include "GNEHierarchicalStructureChildren.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEHierarchicalElement {

public:
    /// @brief declare GNEChange_Children as friend class
    friend class GNEChange_Children;
    friend class GNEDemandElement;

    /// @brief default Constructor
    GNEHierarchicalElement();

    /// @brief Destructor
    ~GNEHierarchicalElement();

    /// @brief clear hierarchical structure parents (used in GNE_Change)
    void clearParents();

    /// @name get parent functions
    /// @{

    /// @brief get parents container
    const GNEHierarchicalStructureParents& getParents() const;

    /// @brief get parent junctions
    const GNEHierarchicalContainerParents<GNEJunction*>& getParentJunctions() const;

    /// @brief get parent edges
    const GNEHierarchicalContainerParents<GNEEdge*>& getParentEdges() const;

    /// @brief get parent lanes
    const GNEHierarchicalContainerParents<GNELane*>& getParentLanes() const;

    /// @brief get parent additionals
    const GNEHierarchicalContainerParents<GNEAdditional*>& getParentAdditionals() const;

    /// @brief get parent stoppingPlaces (used by plans)
    const GNEHierarchicalContainerParents<GNEAdditional*> getParentStoppingPlaces() const;

    /// @brief get parent TAZs (used by plans)
    const GNEHierarchicalContainerParents<GNEAdditional*> getParentTAZs() const;

    /// @brief get parent demand elements
    const GNEHierarchicalContainerParents<GNEDemandElement*>& getParentDemandElements() const;

    /// @brief get parent demand elements
    const GNEHierarchicalContainerParents<GNEGenericData*>& getParentGenericDatas() const;

    /// @}

    /// @name get children functions
    /// @{

    /// @brief get child container
    const GNEHierarchicalStructureChildren& getChildren() const;

    /// @brief get child junctions
    const GNEHierarchicalContainerChildren<GNEJunction*>& getChildJunctions() const;

    /// @brief get child edges
    const GNEHierarchicalContainerChildren<GNEEdge*>& getChildEdges() const;

    /// @brief get child lanes
    const GNEHierarchicalContainerChildren<GNELane*>& getChildLanes() const;

    /// @brief return child additionals
    const GNEHierarchicalContainerChildren<GNEAdditional*>& getChildAdditionals() const;

    /// @brief return child demand elements
    const GNEHierarchicalContainerChildren<GNEDemandElement*>& getChildDemandElements() const;

    /// @brief return child generic data elements
    const GNEHierarchicalContainerChildren<GNEGenericData*>& getChildGenericDatas() const;

    /// @brief return child TAZSourceSinks (Set)
    const GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*>& getChildTAZSourceSinks() const;

    /// @}

    /// @brief edit parent and childrens without maintain integrity (use carefully)
    /// @{

    /// @brief set single parent element (ONLY use in constructors)
    template<typename ParentType>
    void setParent(ParentType parent) {
        GNEHierarchicalContainerParents<ParentType> parents;
        parents.push_back(parent);
        myHierarchicalStructureParents.replaceAll(parents);
    }

    /// @brief set multiple parent element (ONLY use in constructors)
    template<typename ParentType>
    void setParents(const GNEHierarchicalContainerParents<ParentType>& parents) {
        myHierarchicalStructureParents.replaceAll(parents);
    }

    /// @brief add child without updating parent (ONLY used if we're creating elements without undo-redo)
    template<typename ChildType>
    void addChildElement(ChildType* element) {
        myHierarchicalStructureChildren.add(element);
    }

    /// @brief Sort childrens
    template<typename ChildType>
    void sortChildren(std::vector<ChildType*> sortedChildrens) {
        // remove elements and add again
        for (auto child : sortedChildrens) {
            myHierarchicalStructureChildren.remove(child);
        }
        for (auto child : sortedChildrens) {
            myHierarchicalStructureChildren.add(child);
        }
    }

    /// @}

    /// @name edit function maintain integrity
    /// @{

    /// @brief insert parent element
    template<typename ElementType, typename ParentType>
    static void insertParent(ElementType* element, ParentType* parent, const int index = -1) {
        element->myHierarchicalStructureParents.add(parent, index);
        parent->myHierarchicalStructureChildren.add(element);
    }

    /// @brief remove parent element
    template<typename ElementType, typename ParentType>
    static void removeParent(ElementType* element, ParentType* parent) {
        element->myHierarchicalStructureParents.remove(parent);
        parent->myHierarchicalStructureChildren.remove(element);
    }

    /// @brief update single parent element
    template<typename ElementType, typename ParentType>
    static void updateParent(ElementType element, const int index, ParentType newParent) {
        // remove element from old parent
        auto oldParent = element->myHierarchicalStructureParents.template at<ParentType>(index);
        oldParent->myHierarchicalStructureChildren.remove(element);
        // update parent
        element->myHierarchicalStructureParents.replaceSingle(index, newParent);
        // insert child in new parent
        newParent->myHierarchicalStructureChildren.add(element);
    }

    /// @brief update all parent elements
    template<typename ElementType, typename ParentType>
    static void updateParents(ElementType element, GNEHierarchicalContainerParents<ParentType> newParents) {
        // remove children
        for (const auto parent : element->myHierarchicalStructureParents.template get<ParentType>()) {
            parent->myHierarchicalStructureChildren.remove(element);
        }
        // update parents
        element->myHierarchicalStructureParents.replaceAll(newParents);
        // restore children
        for (const auto parent : element->myHierarchicalStructureParents.template get<ParentType>()) {
            parent->myHierarchicalStructureChildren.add(element);
        }
    }

    /// @brief insert child element
    template<typename ElementType, typename ChildType>
    static void insertChild(ElementType element, ChildType child) {
        element->myHierarchicalStructureChildren.add(child);
        child->myHierarchicalStructureParents.add(element);
    }

    /// @brief remove child element
    template<typename ElementType, typename ChildType>
    static void removeChild(ElementType element, ChildType child) {
        element->myHierarchicalStructureChildren.remove(child);
        child->myHierarchicalStructureParents.remove(element);
    }

    /// @brief update all children elements
    template<typename ElementType, typename ChildType>
    static void updateChildren(ElementType element, GNEHierarchicalContainerChildren<ChildType> newChildren) {
        // remove children
        for (const auto children : element->myHierarchicalStructureChildren.template get<ChildType>()) {
            children->myHierarchicalStructureParents.remove(element);
        }
        // update children
        element->myHierarchicalStructureChildren.replaceAll(newChildren);
        // restore children
        for (const auto children : element->myHierarchicalStructureChildren.template get<ChildType>()) {
            children->myHierarchicalStructureParents.add(element);
        }
    }

    /// @}

    /// @name specific get functions
    /// @{

    /// @brief if use edge/parent lanes as a list of consecutive elements, obtain a list of IDs of elements after insert a new element
    std::string getNewListOfParents(const GNENetworkElement* currentElement, const GNENetworkElement* newNextElement) const;

    /// @}

private:
    /// @brief hierarchical structure with parents
    GNEHierarchicalStructureParents myHierarchicalStructureParents;

    /// @brief hierarchical structure with  children
    GNEHierarchicalStructureChildren myHierarchicalStructureChildren;

    /// @brief Invalidated copy constructor.
    GNEHierarchicalElement(const GNEHierarchicalElement&) = delete;
};
