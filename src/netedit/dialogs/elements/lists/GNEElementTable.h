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
/// @file    GNEElementTable.h
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

class GNEElementList;
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEElementTable : public FXVerticalFrame {

public:
    /// @brief row header
    class RowHeader : protected FXHorizontalFrame {

    public:
        /// @brief constructor
        RowHeader(GNEElementTable* elementTable, const GNETagProperties* tagProperties);

        /// @brief destructor
        ~RowHeader();

        /// @brief enable row header
        void enableRowHeader();

        /// @brief disable row header
        void disableRowHeader();

        /// @brief get num columns
        size_t getNumColumns() const;

    private:
        /// @brief labels
        std::vector<FXLabel*> myLabels;

        /// @brief Invalidated duplicate constructor.
        RowHeader(const RowHeader&) = delete;

        /// @brief Invalidated assignment operator.
        RowHeader& operator=(const RowHeader&) = delete;
    };

    /// @brief table row
    class Row : protected FXHorizontalFrame {
        /// @brief fox declaration
        FXDECLARE(Row)

    public:
        /// @brief constructor
        Row(GNEElementTable* elementTable, const size_t rowIndex,
            GNEAttributeCarrier* AC);

        /// @brief destructor
        ~Row();

        /// @brief enable row
        void enableRow();

        /// @brief disable row
        void disableRow();

        /// @brief update row
        void updateRow(GNEAttributeCarrier* AC);

        /// @brief get value of the given column index
        std::string getValue(const size_t column) const;

        /// @brief check if current value is valid
        bool isValid() const;

        /// @name FOX callbacks
        /// @{

        /// @brief called when user edits a row
        long onCmdEditRow(FXObject* sender, FXSelector, void*);

        /// @brief called when user press remove button
        long onCmdRemoveRow(FXObject* sender, FXSelector, void*);

        /// @brief called when user press open dialog button
        long onCmdOpenDialog(FXObject* sender, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(Row)

        /// @brief poiner to table parent
        GNEElementTable* myElementTable = nullptr;

        /// @brief index of the row
        const size_t myRowIndex = 0;

        /// @brief attribute carrier
        GNEAttributeCarrier* myAC = nullptr;

        /// @brief index label
        FXLabel* myIndexLabel = nullptr;

        /// @brief list with textfields and their associated attribute
        std::vector<std::pair<SumoXMLAttr, FXTextField*> > myAttributeTextFields;

        /// @brief remove button
        FXButton* myRemoveButton = nullptr;

        /// @brief open dialog button
        FXButton* myOpenDialogButton = nullptr;

    private:
        /// @brief Invalidated duplicate constructor.
        Row(const Row&) = delete;

        /// @brief Invalidated assignment operator.
        Row& operator=(const Row&) = delete;
    };

    /// @brief constructor
    GNEElementTable(GNEElementList* elementList, const GNETagProperties* tagProperties,
                    const bool allowOpenDialog, const bool fixHeight);

    /// @brief destructor
    ~GNEElementTable();

    /// @brief Enable table
    void enableTable();

    /// @brief Disable table
    void disableTable();

    /// @brief check if the current values of the table are valid
    bool isValid() const;

    /// @brief resize table
    void resizeTable(const size_t numRows);

    /// @brief update row
    void updateRow(const size_t index, GNEAttributeCarrier* AC);

    /// @brief get value
    std::string getValue(const size_t rowIndex, const size_t columnIndex) const;

    /// @brief get number of values
    size_t getNumColumns() const;

protected:
    /// @brief pointer to the parent element list
    GNEElementList* myElementList = nullptr;

    /// @brief check if open dialog in every row is allowed
    bool myAllowOpenDialog = false;

    /// @brief row header
    RowHeader* myRowHeader = nullptr;

    /// @brief scrollWindow for rows
    FXScrollWindow* myScrollWindow = nullptr;

    /// @brief vertical frame for rows
    FXVerticalFrame* myRowsFrame = nullptr;

    /// @brief rows
    std::vector<Row*> myRows;

private:
    /// @brief Invalidated duplicate constructor.
    GNEElementTable(const GNEElementTable&) = delete;

    /// @brief Invalidated assignment operator.
    GNEElementTable& operator=(const GNEElementTable&) = delete;
};
