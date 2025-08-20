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
/// @file    GNEElementList.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Table used in GNEElementList, specific for additional elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/elements/GNEElementDialog.h>

#include "GNEElementList.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAdditional;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAdditionalElementList : public GNEElementList {

public:
    /// @brief constructor
    GNEAdditionalElementList(GNEElementDialog<GNEAdditional>* elementDialogParent,
                             FXVerticalFrame* contentFrame, SumoXMLTag tag,
                             const bool allowSortElements, const bool allowOpenDialog,
                             const bool fixHeight);

    /// @brief get edited additional elements
    const std::vector<GNEAdditional*>& getEditedAdditionalElements() const;

    /// @brief add element
    long addAdditionalElement(GNEAdditional* additionalElement);

    /// @brief update table
    long updateTable();

    /// @brief check if the additional elements are sorted
    bool checkSort() const;

    /// @brief open dialog
    long sortRows();

    /// @brief remove element
    long removeElement(const size_t rowIndex);

    /// @brief add element
    virtual long addElement() = 0;

    /// @brief open dialog
    virtual long openDialog(const size_t rowIndex) = 0;

protected:
    /// @brief element dialog parent
    GNEElementDialog<GNEAdditional>* myElementDialogParent = nullptr;

private:
    /// @brief edited elements
    std::vector<GNEAdditional*> myEditedAdditionalElements;

    /// @brief Invalidated copy constructor
    GNEAdditionalElementList(const GNEAdditionalElementList&) = delete;

    /// @brief Invalidated assignment operator
    GNEAdditionalElementList& operator=(const GNEAdditionalElementList&) = delete;
};
