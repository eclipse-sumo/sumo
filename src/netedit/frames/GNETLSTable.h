/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    MFXTable.h
/// @author  Pablo Alvarez Lopez
/// @date    2022-07-21
///
// Button similar to FXButton but with the possibility of showing tooltips
/****************************************************************************/
#ifndef MFXTable_h
#define MFXTable_h
#include <config.h>

#include <vector>

#include "fxheader.h"

#include <utils/gui/div/GUIDesigns.h>
#include <utils/common/UtilExceptions.h>

#include "GNETLSTable.h"

/**
 * @class MFXTable
 */
class MFXTable : public FXHorizontalFrame {
    /// @brief fox declaration
    FXDECLARE(MFXTable)

public:
    /// @brief table position
    struct MFXTablePos {
        
        /// @brief constructor
        MFXTablePos() {}

        /// @brief column index
        int col = 0;

        /// @brief row index
        int row = 0;

    private:
        /// @brief Invalidated copy constructor.
        MFXTablePos(const MFXTablePos&) = delete;

        /// @brief Invalidated assignment operator.
        MFXTablePos& operator=(const MFXTablePos&) = delete;
    };

    /// @brief constructor (Exactly like the FXButton constructor)
    MFXTable(FXComposite* p, FXObject* tgt, FXSelector sel);

    /// @brief destructor (Called automatically)
    ~MFXTable();

    /// @name FOX callbacks
    /// @{
    /// @brief called when a row is focused
    long onFocusRow(FXObject*, FXSelector, void*);

    /// @brief called when a row is modified
    long onEditRow(FXObject*, FXSelector, void*);

    /// @brief called when mouse enter in MFXTable
    long onEnter(FXObject*, FXSelector, void*);

    /// @brief called when mouse leaves in MFXTable
    long onLeave(FXObject*, FXSelector, void*);
    /// @}


    /* FUNCTIONS USED IN TLSEditorFrame */

    /// Modify cell text
    void setItemText(FXint row, FXint column, const FXString& text, FXbool notify = FALSE);

    /// Return cell text
    FXString getItemText(FXint row, FXint column) const;

    /// Get number of rows
    FXint getNumRows() const;

    /// Get current selected row
    FXint getCurrentSelectedRow() const;

    /// Select a row
    FXbool selectRow(FXint row, FXbool notify = FALSE);

    /// Change current item
    void setCurrentItem(FXint row, FXint column, FXbool notify = FALSE);

    /// Change column header text
    void setColumnText(FXint column, const FXString& text);

    /// Set the table size to nr rows and nc columns; all existing items will be removed
    void setTableSize(const std::string columns, FXint numberRow, FXbool notify = FALSE);

    /// Return the item at the given index
    FXTextField* getItem(FXint row, FXint col) const;

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(MFXTable)

    /// @brief clear table
    void clearTable();

    /// @brief column
    class Column {

    public:
        /// @brief constructor
        Column(MFXTable* table, const int index, const char type);

        /// @brief destructor
        ~Column();

        /// @brief get vertical frame
        FXVerticalFrame* getVerticalFrame() const;

        /// @brief get column type
        char getType() const;

        /// @brief set column label
        void setColumnLabel(const FXString& text);

        /// @brief adjust column width
        void adjustColumnWidth();

    protected:
        /// @brief pointer to table
        MFXTable* myTable = nullptr;

        /// @brief vertical frame
        FXVerticalFrame* myVerticalFrame = nullptr;

        /// @brief column label
        FXLabel* myLabel = nullptr;

        /// @brief column index
        const int myIndex;

        /// @brief column type
        const char myType;

    private:
        /// @brief default constructor
        Column();
    };

    /// @brief Row
    class Row {

    public:
        /// @brief struct for every cell
        struct Cell {

            /// @brief constructor for textField
            Cell(FXTextField* textField_) :
                textField(textField_) {}

            /// @brief constructor for radio button
            Cell(FXRadioButton* radioButton_) :
                radioButton(radioButton_) {}

            /// @brief textField
            FXTextField* textField = nullptr;

            /// @brief radio button
            FXRadioButton* radioButton = nullptr;
        };

        /// @brief constructor
        Row(MFXTable* table);

        /// @brief destructor
        ~Row();

        /// @brief get text
        FXString getText(int index) const;

        /// @brief set text
        void setText(int index, const FXString& text, FXbool notify) const;

        /// @brief get cells
        const std::vector<Cell> &getCells() const;

        /// @brief select column
        void select();

    protected:
        /// @brief poiner to table parent
        MFXTable* myTable = nullptr;

        /// @brief list wtih cells
        std::vector<Cell> myCells;

    private:
        /// @brief default constructor
        Row();
    };

    /// @brief target used in Rows
    FXObject* myTarget = nullptr;

    /// @brief selector used in rows
    FXSelector mySelector = 0;

    /// @brief columns
    std::vector<Column*> myColumns;

    /// @brief rows
    std::vector<Row*> myRows;

    /// @brief current selected row
    int myCurrentSelectedRow = -1;

    /// @brief table pos
    MFXTablePos *myTablePos = nullptr;

private:
    /// @brief Invalidated copy constructor.
    MFXTable(const MFXTable&) = delete;

    /// @brief Invalidated assignment operator.
    MFXTable& operator=(const MFXTable&) = delete;
};


#endif
