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
/// @file    GNEHierarchicalElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// A abstract class for representation of hierarchical elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/div/GUIGeometry.h>
#include <netedit/elements/GNEAttributeCarrier.h>

#include "GNEHierarchicalStructureParents.h"
#include "GNEHierarchicalStructureChildren.h"

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEHierarchicalElements
 * @brief An special type of Attribute carrier that owns hierarchical elements
 */
class GNEHierarchicalElement : public GNEAttributeCarrier {

public:
    /// @brief declare GNEChange_Children as friend class
    friend class GNEChange_Children;
    friend class GNEDemandElement;

    /**@brief Constructor
     * @param[in] tag SUMO Tag assigned to this type of object
     * @param[in] net GNENet in which this AttributeCarrier is stored
     * @param[in] parentJunctions vector of parent junctions
     * @param[in] parentEdges vector of parent edges
     * @param[in] parentLanes vector of parent lanes
     * @param[in] parentAdditionals vector of parent additionals
     * @param[in] parentDemandElements vector of parent demand elements
     * @param[in] parentGenericData vector of parent generic data elements
     */
    GNEHierarchicalElement(GNENet* net, SumoXMLTag tag,
                           const std::vector<GNEJunction*>& parentJunctions,
                           const std::vector<GNEEdge*>& parentEdges,
                           const std::vector<GNELane*>& parentLanes,
                           const std::vector<GNEAdditional*>& parentAdditionals,
                           const std::vector<GNEDemandElement*>& parentDemandElements,
                           const std::vector<GNEGenericData*>& parentGenericDatas);

    /// @brief Destructor
    ~GNEHierarchicalElement();

    /// @brief get GNEHierarchicalElement associated with this AttributeCarrier
    GNEHierarchicalElement* getHierarchicalElement();

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief Returns position of hierarchical element in view
    virtual Position getPositionInView() const = 0;
    /// @}

    /// @brief get parents(used in GNE_Change)
    const GNEHierarchicalStructureParents getParents() const;

    /// @brief clear hierarchical structure parents (used in GNE_Change)
    void clearParents();

    /// @name common get functions
    /// @{

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

    /// @brief get child junctions
    const GNEHierarchicalContainerChildren<GNEJunction*>& getChildJunctions() const;

    /// @brief get child edges
    const GNEHierarchicalContainerChildren<GNEEdge*>& getChildEdges() const;

    /// @brief get child lanes
    const GNEHierarchicalContainerChildren<GNELane*>& getChildLanes() const;

    /// @brief return child additionals
    const GNEHierarchicalContainerChildren<GNEAdditional*>& getChildAdditionals() const;

    /// @brief return child TAZSourceSinks (Hash)
    const GNEHierarchicalContainerChildren<GNETAZSourceSink*>& getChildTAZSourceSinks() const;

    /// @brief return child demand elements
    const GNEHierarchicalContainerChildren<GNEDemandElement*>& getChildDemandElements() const;

    /// @brief return child generic data elements
    const GNEHierarchicalContainerChildren<GNEGenericData*>& getChildGenericDatas() const;

    /// @}

    /// @name edit function maintain integrity
    /// @{

    /// @brief insert parent element
    template<typename T, typename U>
    static void insertParent(T* element, U* parent, const int index = -1) {
        element->myHierarchicalStructureParents.add(parent, index);
        parent->myHierarchicalStructureChildren.add(element);
    }

    /// @brief remove parent element
    template<typename T, typename U>
    static void removeParent(T* element, U* parent) {
        element->myHierarchicalStructureParents.remove(parent);
        parent->myHierarchicalStructureChildren.remove(element);
    }

    /// @brief update single parent element
    template<typename T, typename U>
    static void updateParent(T element, const int index, U newParent) {
        // remove element from old parent
        auto oldParent = element->myHierarchicalStructureParents.at<U>(index);
        oldParent->myHierarchicalStructureChildren.remove(element);
        // update parent
        element->myHierarchicalStructureParents.replaceSingle(index, newParent);
        // insert child in new parent
        newParent->myHierarchicalStructureChildren.add(element);
    }

    /// @brief update all parent elements
    template<typename T, typename U>
    static void updateParents(T element, GNEHierarchicalContainerParents<U> newParents) {
        // remove children
        for (const auto parent : element->myHierarchicalStructureParents.get<U>()) {
            parent->myHierarchicalStructureChildren.remove(element);
        }
        // update parents
        element->myHierarchicalStructureParents.replaceAll(newParents);
        // restore children
        for (const auto parent : element->myHierarchicalStructureParents.get<U>()) {
            parent->myHierarchicalStructureChildren.add(element);
        }
    }

    /// @brief insert child element
    template<typename T, typename U>
    static void insertChild(T element, U child) {
        element->myHierarchicalStructureChildren.add(child);
        child->myHierarchicalStructureParents.add(element);
    }

    /// @brief remove child element
    template<typename T, typename U>
    static void removeChild(T element, U child) {
        element->myHierarchicalStructureChildren.remove(child);
        child->myHierarchicalStructureParents.remove(element);
    }

    /// @brief update all children elements
    template<typename T, typename U>
    static void updateChildren(T element, GNEHierarchicalContainerChildren<U> newChildren) {
        // remove children
        for (const auto children : element->myHierarchicalStructureChildren.get<U>()) {
            children->myHierarchicalStructureParents.remove(element);
        }
        // update children
        element->myHierarchicalStructureChildren.replaceAll(newChildren);
        // restore children
        for (const auto children : element->myHierarchicalStructureChildren.get<U>()) {
            children->myHierarchicalStructureParents.add(element);
        }
    }

    /// @}

    /// @brief add child without updating parent (ONLY used if we're creating elements without undo-redo)
    template<typename T>
    void addChildElement(T* element) {
        myHierarchicalStructureChildren.add(element);
    }

    /// @name specific get functions
    /// @{

    /// @brief if use edge/parent lanes as a list of consecutive elements, obtain a list of IDs of elements after insert a new element
    std::string getNewListOfParents(const GNENetworkElement* currentElement, const GNENetworkElement* newNextElement) const;

    /// @}

    /// @brief check if children are overlapped (Used by Rerouters)
    bool checkChildAdditionalsOverlapping() const;

    /// @brief check if child demand elements are overlapped
    bool checkChildDemandElementsOverlapping() const;

private:
    /// @brief hierarchical structure with parents
    GNEHierarchicalStructureParents myHierarchicalStructureParents;

    /// @brief hierarchical structure with  children
    GNEHierarchicalStructureChildren myHierarchicalStructureChildren;

    /// @brief Invalidated copy constructor.
    GNEHierarchicalElement(const GNEHierarchicalElement&) = delete;
};
