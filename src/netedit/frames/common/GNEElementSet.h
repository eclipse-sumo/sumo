/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEElementSet.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2020
///
// The Widget for modifying selections of network-elements
// (some elements adapted from GUIDialog_GLChosenEditor)
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNESelectorFrame.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEElementSet : protected FXGroupBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEElementSet)

public:
    /// @brief Element Set Type
    enum class Type {
        NETWORKELEMENT,
        ADDITIONALELEMENT,
        SHAPE,
        TAZELEMENT,
        DEMANDELEMENT,
        DATA,
        INVALID,
    };

    /// @brief constructor
    GNEElementSet(GNESelectorFrame* selectorFrameParent);

    /// @brief destructor
    ~GNEElementSet();

    /// @brief get current selected element set
    Type getElementSet() const;

    /// @brief refresh element set
    void refreshElementSet();

    /// @brief update current element set (called after

    /// @name FOX-callbacks
    /// @{

    /// @brief Called when the user change the set of element to search (networkElement, Additional or shape)
    long onCmdSelectElementSet(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEElementSet)

private:
    /// @brief pointer to Selector Frame Parent
    GNESelectorFrame* mySelectorFrameParent;

    /// @brief Combo Box with the element sets
    FXComboBox* mySetComboBox;

    /// @brief current element set selected
    Type myCurrentGNEElementSet;

    /// @brief Invalidated copy constructor.
    GNEElementSet(const GNEElementSet&) = delete;

    /// @brief Invalidated assignment operator.
    GNEElementSet& operator=(const GNEElementSet&) = delete;
};