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

#include "GNEHierarchicalContainer.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNENetworkElement;
class GNEJunction;
class GNEEdge;
class GNELane;
class GNEAdditional;
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
    const GNEHierarchicalContainer<T>& getParents() const;

    /// @brief set parents
    template<typename T>
    void setParents(const T& newParents);

    /// @brief get children
    template<typename T>
    const GNEHierarchicalContainer<T>& getChildren() const;

    /// @brief set children
    template<typename T>
    void setChildren(const T& newChildren);

private:
    /// @brief parents junctions
    GNEHierarchicalContainer<GNEJunction*> myParentJunctions;

    /// @brief parents edges
    GNEHierarchicalContainer<GNEEdge*> myParentEdges;

    /// @brief parents lanes
    GNEHierarchicalContainer<GNELane*> myParentLanes;

    /// @brief parents additionals
    GNEHierarchicalContainer<GNEAdditional*> myParentAdditionals;

    /// @brief parents demand elements
    GNEHierarchicalContainer<GNEDemandElement*> myParentDemandElements;

    /// @brief parents generic datas
    GNEHierarchicalContainer<GNEGenericData*> myParentGenericDatas;

    /// @brief children junctions
    GNEHierarchicalContainer<GNEJunction*> myChildJunctions;

    /// @brief children edges
    GNEHierarchicalContainer<GNEEdge*> myChildEdges;

    /// @brief children lanes
    GNEHierarchicalContainer<GNELane*> myChildLanes;

    /// @brief children additional elements
    GNEHierarchicalContainer<GNEAdditional*> myChildAdditionals;

    /// @brief children demand elements
    GNEHierarchicalContainer<GNEDemandElement*> myChildDemandElements;

    /// @brief children genericdata elements
    GNEHierarchicalContainer<GNEGenericData*> myChildGenericDatas;
};
