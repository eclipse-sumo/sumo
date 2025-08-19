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

#include <vector>
#include <utils/tests/InternalTestStep.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEDialog;
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEElementTable : public FXVerticalFrame {
    /// @brief fox declaration
    FXDECLARE(GNEElementTable)

public:
    /// @brief table row header
    class RowHeader : protected FXHorizontalFrame {

    public:
        /// @brief constructor
        RowHeader(GNEElementTable* table, const GNETagProperties* tagProperties);

        /// @brief destructor
        ~RowHeader();

        /// @brief enable row header
        void enableRowHeader();

        /// @brief disable row header
        void disableRowHeader();

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

    public:
        /// @brief constructor
        Row(GNEElementTable* table, const size_t index, GNEAttributeCarrier* AC,
            const bool allowOpenDialog);

        /// @brief destructor
        ~Row();

        /// @brief enable row
        void enableRow();

        /// @brief disable row
        void disableRow();

        /// @brief update row
        void updateRow(GNEAttributeCarrier* AC);

        /// @brief get text
        std::string getValue(const int index) const;

        /// @brief called when user update the value of a text field
        void updateValue(const FXObject* sender);

    protected:
        /// @brief poiner to table parent
        GNEElementTable* myTable = nullptr;

        /// @brief row index
        const size_t myIndex = 0;

        /// @brief attribute carrier
        GNEAttributeCarrier* myAC = nullptr;

        /// @brief index label
        FXLabel* myIndexLabel = nullptr;

        /// @brief list with textfields and their associated attribute
        std::vector<std::pair<FXTextField*, SumoXMLAttr> > myTextFields;

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
    GNEElementTable(FXVerticalFrame* contentFrame, GNEDialog* targetDialog,
                    const GNETagProperties* tagProperties, const bool fixHeight);

    /// @brief destructor
    ~GNEElementTable();

    /// @brief Enable table
    void enableTable();

    /// @brief Disable table
    void disableTable();

    /// @brief resize table
    void resizeTable(const size_t numRows);

    /// @brief update row
    void updateRow(const size_t index, GNEAttributeCarrier* AC);

    /// @name FOX callbacks
    /// @{

    /// @brief called when user edits a row
    long onCmdEditRow(FXObject* sender, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEElementTable)

    /// @brief target dialog
    GNEDialog* myTargetDialog = nullptr;

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
