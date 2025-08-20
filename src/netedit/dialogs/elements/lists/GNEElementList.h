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
// Table used in GNEElementList
/****************************************************************************/
#pragma once
#include <config.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEDialog;
class GNEElementTable;
class GNETagProperties;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEElementList : public FXVerticalFrame {
    /// @brief FOX-declaration
    FXDECLARE_ABSTRACT(GNEElementList)

public:
    /// @brief constructor
    GNEElementList(FXVerticalFrame* contentFrame, const GNETagProperties* tagProperty,
                   const bool fixHeight);

    /// @brief destructor
    ~GNEElementList();

    /// @brief enable list
    void enableList();

    /// @brief disable list
    void disableList(const std::string& reason);

    /// @brief check if the current list is valid
    bool isListValid() const;

    /// @brief update table
    virtual long updateTable() = 0;

    /// @brief remove row
    virtual long addRow() = 0;

    /// @brief open dialog
    virtual long sortRows() = 0;

    /// @brief remove row
    virtual long removeRow(const size_t rowIndex) = 0;

    /// @brief open dialog
    virtual long openDialog(const size_t rowIndex) = 0;

    /// @name FOX callbacks
    /// @{

    /// @brief called when user press add button
    long onCmdAddRow(FXObject* sender, FXSelector, void*);

    /// @brief called when user press sort button
    long onCmdSort(FXObject* sender, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEElementList)

    /// @brief pointer to tag property
    const GNETagProperties* myTagProperty = nullptr;

    /// @brief element table
    GNEElementTable* myElementTable = nullptr;

private:
    /// @brief add button
    FXButton* myAddButton = nullptr;

    /// @brief sort button
    FXButton* mySortButton = nullptr;

    /// @brief label
    FXLabel* myLabel = nullptr;

    /// @brief Invalidated copy constructor
    GNEElementList(const GNEElementList&) = delete;

    /// @brief Invalidated assignment operator
    GNEElementList& operator=(const GNEElementList&) = delete;
};
