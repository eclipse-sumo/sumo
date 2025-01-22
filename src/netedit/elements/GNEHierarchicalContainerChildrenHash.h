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
/// @file    GNEHierarchicalContainerChildrenHash.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2025
///
// Template container for GNEHierarchical elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <set>

// ===========================================================================
// class definitions
// ===========================================================================

template <typename T> 
class GNEHierarchicalContainerChildrenHash : public std::set<T> {

public:
    /// @brief default constructor
    GNEHierarchicalContainerChildrenHash() {
    }

    /// @brief parameter constructor
    GNEHierarchicalContainerChildrenHash(const std::set<T>& elements) :
        std::set<T>(elements) {
    }
};
