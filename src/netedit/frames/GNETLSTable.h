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
/// @file    GNETLSTable.h
/// @author  Pablo Alvarez Lopez
/// @date    2022-07-21
///
// Table used in GNETLSFrame for editing TLS programs
/****************************************************************************/
#ifndef GNETLSTable_h
#define GNETLSTable_h
#include <config.h>

#include <vector>

#include <utils/common/UtilExceptions.h>
#include <utils/foxtools/fxheader.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETLSTable
 */
class GNETLSTable : public FXHorizontalFrame {
    /// @brief fox declaration
    FXDECLARE(GNETLSTable)

public:
    /// @brief struct for every cell
    class TableCell {

    public:
        /// @brief constructor for textField
        TableCell(FXTextField* textField, int col, int row);

        /// @brief constructor for radio button
        TableCell(FXRadioButton* radioButton, int col, int row);

        /// @brief constructor for buttons
        TableCell(FXButton* button, int col, int row);

        /// @brief get textField
        FXTextField* getTextField();

        /// @brief get radio button
        FXRadioButton* getRadioButton();

        /// @brief get button
        FXButton* getButton();

        /// @brief column index
        const int getCol();

        /// @brief row index
        const int getRow();

    protected:
        /// @brief textField
        FXTextField* myTextField = nullptr;

        /// @brief radio button
        FXRadioButton* myRadioButton = nullptr;

        /// @brief button
        FXButton* myButton = nullptr;

        /// @brief column index
        const int myCol;

        /// @brief row index
        const int myRow;

    private:
        /// @brief default constructor
        TableCell();
    };

    /// @brief constructor (Exactly like the FXButton constructor)
    GNETLSTable(GNETLSEditorFrame::TLSPhases* TLSPhasesParent);

    /// @brief destructor (Called automatically)
    ~GNETLSTable();

    /// @brief recalc width (call when all labels and contents are fill)
    void recalcTableWidth();

    /// @brief clear table
    void clearTable();

    /// @name FOX callbacks
    /// @{
    /// @brief called when a row is focused
    long onFocusRow(FXObject*, FXSelector, void*);

    /// @brief called when a row is modified
    long onEditRow(FXObject*, FXSelector, void*);

    /// @brief called when a row is selected
    long onRowSelected(FXObject*, FXSelector, void*);

    /// @brief called when mouse enter in GNETLSTable
    long onEnter(FXObject*, FXSelector, void*);

    /// @brief called when mouse leaves in GNETLSTable
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
    void selectRow(const int rowIndex);

    /// Change current item
    void setCurrentItem(FXint row, FXint column, FXbool notify = FALSE);

    /// Change column header text
    void setColumnText(FXint column, const FXString& text);

    /// Set the table size to nr rows and nc columns; all existing items will be removed
    /// Format: s -> radio button, p -> program (rrGggy...), i -> insert, d -> delete- -> textField 
    void setTableSize(const std::string columnsType, const int numberRow);

    /// Return the item at the given index
    FXTextField* getItem(FXint row, FXint col) const;

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNETLSTable)

    /// @brief column
    class Column {

    public:
        /// @brief constructor
        Column(GNETLSTable* table, const int index, const char type);

        /// @brief destructor
        ~Column();

        /// @brief get vertical frame
        FXVerticalFrame* getVerticalFrame() const;

        /// @brief get column type
        char getType() const;

        /// @brief set column label
        void setColumnLabel(const FXString& text);

        /// @brief adjust column width
        int adjustColumnWidth();

    protected:
        /// @brief pointer to table
        GNETLSTable* myTable = nullptr;

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
        /// @brief constructor
        Row(GNETLSTable* table);

        /// @brief destructor
        ~Row();

        /// @brief get text
        FXString getText(int index) const;

        /// @brief set text
        void setText(int index, const FXString& text, FXbool notify) const;

        /// @brief get cells
        const std::vector<TableCell*> &getCells() const;

        /// @brief select column
        void select();

    protected:
        /// @brief poiner to table parent
        GNETLSTable* myTable = nullptr;

        /// @brief list wtih cells
        std::vector<TableCell*> myCells;

    private:
        /// @brief default constructor
        Row();
    };

    /// @brief font for the phase table
    FXFont* myProgramFont = nullptr;

    /// @frame pointer to TLSEditorFrame phases parent
    GNETLSEditorFrame::TLSPhases* myTLSPhasesParent = nullptr;

    /// @brief columns
    std::vector<Column*> myColumns;

    /// @brief rows
    std::vector<Row*> myRows;

    /// @brief current selected row
    int myCurrentSelectedRow = -1;

private:
    /// @brief Invalidated copy constructor.
    GNETLSTable(const GNETLSTable&) = delete;

    /// @brief Invalidated assignment operator.
    GNETLSTable& operator=(const GNETLSTable&) = delete;
};


#endif
