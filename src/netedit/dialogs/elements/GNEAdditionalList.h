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

#include "GNEElementDialog.h"
#include "GNEElementList.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAdditional;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAdditionalList : public GNEElementList {

public:
    /// @brief constructor
    GNEAdditionalList(GNEElementDialog<GNEAdditional>* elementDialogParent,
                      FXVerticalFrame* contentFrame, SumoXMLTag tag,
                      const bool fixHeight);

    /// @brief get elements
    const std::vector<GNEAdditional*>& getEditedAdditionals() const;

    /// @brief add element
    long addAdditionalElement(GNEAdditional* additionalElement);

    /// @brief update table
    long updateTable();

    /// @brief open dialog
    long sortRows();

    /// @brief remove row
    long removeRow(const size_t rowIndex);

    /// @brief remove row
    virtual long addRow() = 0;

    /// @brief open dialog
    virtual long openDialog(const size_t rowIndex) = 0;

protected:
    /// @brief edited elements
    std::vector<GNEAdditional*> myEditedAdditionalElements;

private:
    /// @brief Invalidated copy constructor
    GNEAdditionalList(const GNEAdditionalList&) = delete;

    /// @brief Invalidated assignment operator
    GNEAdditionalList& operator=(const GNEAdditionalList&) = delete;
};
