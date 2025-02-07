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
    const GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*>& getChildTAZSourceSinks() const;

    /// @brief return child demand elements
    const GNEHierarchicalContainerChildren<GNEDemandElement*>& getChildDemandElements() const;

    /// @brief return child generic data elements
    const GNEHierarchicalContainerChildren<GNEGenericData*>& getChildGenericDatas() const;

    /// @}

    /// @brief add child (only used if we're creating elements without undo-redo)
    template<typename T>
    void addChildElement(T* element) {
        myHierarchicalStructureChildren.addChildElement(element);
    }

    /// @brief insert parent element and update their child
    template<typename T, typename U>
    static void insertParent(T* element, U* parent) {
        element->myHierarchicalStructureParents.addParentElement(parent);
        parent->myHierarchicalStructureChildren.addChildElement(element);
    }

    /// @brief remove parent element and update their child
    template<typename T, typename U>
    static void removeParent(T* element, U* parent) {
        element->myHierarchicalStructureParents.removeParentElement(parent);
        parent->myHierarchicalStructureChildren.removeChildElement(element);
    }

    /// @brief insert child element and update their parent
    template<typename T, typename U>
    static void insertChild(T* element, U* child) {
        element->myHierarchicalStructureChildren.addChildElement(child);
        child->myHierarchicalStructureParents.addParentElement(element);
    }

    /// @brief remove child element (and update their parent)
    template<typename T, typename U>
    static void removeChild(T* element, U* child) {
        element->myHierarchicalStructureChildren.removeChildElement(child);
        child->myHierarchicalStructureParents.removeParentElement(element);
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
