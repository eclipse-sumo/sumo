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
/// @file    GNEHierarchicalContainer.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2025
///
// Template container for GNEHierarchical elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>


#define MAX_HIERARCHICAL_PARENTS 10

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

/// @brief container type
enum class HierarchicalContainerType {
    BASIC
};

template <typename T> 
class GNEHierarchicalContainer : public std::vector<T> {

public:
    /// @brief default constructor
    GNEHierarchicalContainer() {}

    /// @brief parameter constructor
    GNEHierarchicalContainer(const std::vector<T>& elements, HierarchicalContainerType type):
        myType(type) {
        if (myType == HierarchicalContainerType::BASIC) {
            myVectorElements.reserve(MAX_HIERARCHICAL_PARENTS);
        }
    }

    /// @brief get container size
    size_t size() const {
        return myVectorElements.size();
    }

    /// @brief add parent element
    void insert(T element) {
        myVectorElements.push_back(element);
    }

    /// @brief remove parent element
    bool erase(T element) {
        auto it = std::find(myVectorElements.begin(), myVectorElements.end(), element);
        if (it != myVectorElements.end()) {
            myVectorElements.erase(it);
            return true;
        } else {
            return false;
        }
    }

    /// @brief update all elements
    void set(const std::vector<T>& elements) {
        myVectorElements.clear();
        myVectorElements.reserve(elements.size());
        for (const auto &element : elements) {
            myVectorElements.push_back(element);
        }
    }

private:
    /// @brief container type
    HierarchicalContainerType myType;

    /// @brief elements vector
    std::vector<T> myVectorElements;
};
