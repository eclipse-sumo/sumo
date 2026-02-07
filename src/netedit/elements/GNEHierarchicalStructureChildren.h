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
/// @file    GNEHierarchicalStructureChildren.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2025
///
// Structure for GNEHierarchicalElements centered in children
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <set>

// ===========================================================================
// template declarations
// ===========================================================================

template <typename ChildType>
using GNEHierarchicalContainerChildren = std::vector<ChildType>;

template <typename ChildType>
using GNEHierarchicalContainerChildrenSet = std::set<ChildType>;

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

/// @brief Hierarchical structure used for keep children
class GNEHierarchicalStructureChildren {

public:
    /// @brief default constructor
    GNEHierarchicalStructureChildren();

    /// @brief get children
    template<typename ChildType>
    const GNEHierarchicalContainerChildren<ChildType>& get() const;

    /// @brief get children (set)
    template<typename ChildType>
    const GNEHierarchicalContainerChildrenSet<ChildType>& getSet() const;

    /// @brief add child element
    template<typename ChildType>
    void add(ChildType child);

    /// @brief remove child element
    template<typename ChildType>
    void remove(ChildType child);

    /// @brief update all children
    template<typename ChildType>
    void replaceAll(const GNEHierarchicalContainerChildren<ChildType>& children);

private:
    /// @brief children junctions
    GNEHierarchicalContainerChildren<GNEJunction*> myChildJunctions;

    /// @brief children edges
    GNEHierarchicalContainerChildren<GNEEdge*> myChildEdges;

    /// @brief children lanes
    GNEHierarchicalContainerChildren<GNELane*> myChildLanes;

    /// @brief children additional elements
    GNEHierarchicalContainerChildren<GNEAdditional*> myChildAdditionals;

    /// @brief children demand elements
    GNEHierarchicalContainerChildren<GNEDemandElement*> myChildDemandElements;

    /// @brief children genericdata elements
    GNEHierarchicalContainerChildren<GNEGenericData*> myChildGenericDatas;

    /// @brief children TAZSourceSinks elements (set)
    GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*> myChildSourceSinks;
};
