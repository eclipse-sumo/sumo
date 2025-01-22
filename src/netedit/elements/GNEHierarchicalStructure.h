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
/// @file    GNEHierarchicalStructure.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// Structure for GNEHierarchicalElements
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <set>

// ===========================================================================
// template declarations
// ===========================================================================

template <typename T>
using GNEHierarchicalContainerParents = std::vector<T>;

template <typename T>
using GNEHierarchicalContainerChildren = std::vector<T>;

template <typename T>
using GNEHierarchicalContainerChildrenSet = std::set<T>;

// ===========================================================================
// class declarations
// ===========================================================================

class GNENetworkElement;
class GNEJunction;
class GNEEdge;
class GNELane;
class GNEAdditional;
class GNETAZSourceSink;
class GNEDemandElement;
class GNEGenericData;
class GNEHierarchicalElement;

// ===========================================================================
// class definitions
// ===========================================================================

/// @brief Hierarchical container (used for keep myParent and myChildren
class GNEHierarchicalStructure {

public:
    /// @brief default constructor
    GNEHierarchicalStructure();

    /// @brief parameter constructor (only for parents)
    GNEHierarchicalStructure(
        const std::vector<GNEJunction*>& parentJunctions,
        const std::vector<GNEEdge*>& parentEdges,
        const std::vector<GNELane*>& parentLanes,
        const std::vector<GNEAdditional*>& parentAdditionals,
        const std::vector<GNEDemandElement*>& parentDemandElements,
        const std::vector<GNEGenericData*>& parentGenericDatas);

    /// @brief get container size
    size_t getContainerSize() const;

    /// @brief add parent element
    template<typename T>
    void addParentElement(T* element);

    /// @brief remove parent element
    template<typename T>
    void removeParentElement(T* element);

    /// @brief add child element
    template<typename T>
    void addChildElement(T* element);

    /// @brief remove child element
    template<typename T>
    void removeChildElement(T* element);

    /// @brief get parents
    template<typename T>
    const GNEHierarchicalContainerParents<T>& getParents() const;

    /// @brief set parents
    template<typename T>
    void setParents(const GNEHierarchicalContainerParents<T>& newParents);

    /// @brief get children
    template<typename T>
    const GNEHierarchicalContainerChildren<T>& getChildren() const;

    /// @brief get children set
    template<typename T>
    const GNEHierarchicalContainerChildrenSet<T>& getChildrenSet() const;

    /// @brief set children
    template<typename T>
    void setChildren(const GNEHierarchicalContainerChildren<T>& newChildren);

    /// @brief set children set
    template<typename T>
    void setChildrenSet(const GNEHierarchicalContainerChildrenSet<T>& newChildren);

private:
    /// @brief parents junctions
    GNEHierarchicalContainerParents<GNEJunction*> myParentJunctions;

    /// @brief parents edges
    GNEHierarchicalContainerParents<GNEEdge*> myParentEdges;

    /// @brief parents lanes
    GNEHierarchicalContainerParents<GNELane*> myParentLanes;

    /// @brief parents additionals
    GNEHierarchicalContainerParents<GNEAdditional*> myParentAdditionals;

    /// @brief parents TAZSourceSinks (Unused, but needed for function addElementInParentsAndChildren)
    GNEHierarchicalContainerParents<GNETAZSourceSink*> myParentTAZSourceSinks;

    /// @brief parents demand elements
    GNEHierarchicalContainerParents<GNEDemandElement*> myParentDemandElements;

    /// @brief parents generic datas
    GNEHierarchicalContainerParents<GNEGenericData*> myParentGenericDatas;

    /// @brief children junctions
    GNEHierarchicalContainerChildren<GNEJunction*> myChildJunctions;

    /// @brief children edges
    GNEHierarchicalContainerChildren<GNEEdge*> myChildEdges;

    /// @brief children lanes
    GNEHierarchicalContainerChildren<GNELane*> myChildLanes;

    /// @brief children additional elements
    GNEHierarchicalContainerChildren<GNEAdditional*> myChildAdditionals;

    /// @brief children TAZSourceSinks elements (set)
    GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*> myChildSourceSinks;

    /// @brief children demand elements
    GNEHierarchicalContainerChildren<GNEDemandElement*> myChildDemandElements;

    /// @brief children genericdata elements
    GNEHierarchicalContainerChildren<GNEGenericData*> myChildGenericDatas;
};
