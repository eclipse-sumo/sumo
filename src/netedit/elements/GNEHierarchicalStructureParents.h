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
/// @file    GNEHierarchicalStructureParents.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2025
///
// Structure for GNEHierarchicalElements centered in parents
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>

// ===========================================================================
// template declarations
// ===========================================================================

template <typename ParentType>
using GNEHierarchicalContainerParents = std::vector<ParentType>;

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

/// @brief Hierarchical structure used for keep parents
class GNEHierarchicalStructureParents {

public:
    /// @brief default constructor
    GNEHierarchicalStructureParents();

    /// @brief clear container
    void clear();

    /// @brief get parents
    template<typename ParentType>
    const GNEHierarchicalContainerParents<ParentType>& get() const;

    /// @brief get parent at the given position
    template<typename ParentType>
    ParentType at(const int index) const;

    /**@brief add parent element
     * @param parent new parent element to be inserted
     * @param index position (-1 means push back)
     */
    template<typename ParentType>
    void add(ParentType parent, const int index = -1);

    /// @brief remove parent element
    template<typename ParentType>
    void remove(ParentType parent);

    /**@brief update single parent element
     * @param index position
     * @param parent new parent element to be updated
     */
    template<typename ParentType>
    void replaceSingle(const int index, ParentType newParent);

    /// @brief update all parent element
    template<typename ParentType>
    void replaceAll(const GNEHierarchicalContainerParents<ParentType>& newParents);

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
};
