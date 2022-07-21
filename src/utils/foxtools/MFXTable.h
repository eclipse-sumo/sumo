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
#include "MFXStaticToolTip.h"

/**
 * @class MFXTable
 */
class MFXTable : public FXHorizontalFrame {
    /// @brief fox declaration
    FXDECLARE(MFXTable)

public:
    /// @brief constructor (Exactly like the FXButton constructor)
    MFXTable(FXComposite *p, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = 0, 
             FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0, FXint pl = DEFAULT_MARGIN, 
             FXint pr = DEFAULT_MARGIN, FXint pt = DEFAULT_MARGIN, FXint pb = DEFAULT_MARGIN);

    /// @brief destructor (Called automatically)
    ~MFXTable();

    /// @name FOX callbacks
    /// @{
    /// @brief called when mouse enter in MFXTable
    long onEnter(FXObject*, FXSelector, void*);

    /// @brief called when mouse leaves in MFXTable
    long onLeave(FXObject*, FXSelector, void*);
    /// @}


    /* FUNCTIONS USED IN TLSEditorFrame */

    /// Modify cell text
    void setItemText(FXint row, FXint column, const FXString& text, FXbool notify = FALSE) {

    }

    /// Return cell text
    FXString getItemText(FXint row, FXint column) const {
        return "";
    }

    /// Get number of rows
    FXint getNumRows() const { 
        return myRows.size(); 
    }

    /// Get row number of current item
    FXint getCurrentRow() const { 
        return currentRow; 
    }

    /// Select a row
    FXbool selectRow(FXint row, FXbool notify = FALSE) {
        //
    }

    /// Get selection start row; returns -1 if no selection
/*
    FXint getSelStartRow() const { 
        return selection.fm.row; 
    }
*/

    /// Change current item
    void setCurrentItem(FXint row, FXint column, FXbool notify = FALSE) {
        
    }

    /// Change column header text
    void setColumnText(FXint index,const FXString& text) {
        
    }

    /// Set the table size to nr rows and nc columns; all existing items will be removed
    void setTableSize(FXint numberRow, FXint numberColumn, FXbool notify = FALSE) {
        // first clear table
        clear();
        // create columns
        for (int i = 0; i < numberColumn; i++) {
            myColumns.push_back(new Column(this));
        }
        // create rows
        for (int i = 0; i < numberRow; i++) {
            myRows.push_back(new Row(this));
        }

    }

    /// Change visible rows
    void setVisibleRows(FXint numVisibleRows) {

    }

    /// Change visible columns
    void setVisibleColumns(FXint numVisibleColumns) {

    }

    /// Change column width
    void setColumnWidth(FXint column, FXint columnWidth) {

    }

    /// Return the item at the given index
    FXTableItem *getItem(FXint row,FXint col) const {

    }

    /// Get column width
    FXint getColumnWidth(FXint col) const {

    }

    /// Change default column width
    void setDefColumnWidth(FXint columnWidth) {

    }

    /// Fit column widths to contents
    void fitColumnsToContents(FXint column, FXint nc = 1) {

    }

    /**
     * Change column header height mode to fixed or variable.
     * In variable height mode, the column header will size to
     * fit the contents in it.  In fixed mode, the size is
     * explicitly set using setColumnHeaderHeight().
     */
    void setColumnHeaderMode(FXuint hint = LAYOUT_FIX_HEIGHT) {

    }

    /// Change column header height
    void setColumnHeaderHeight(FXint h) {

    }

    /**
     * Change row header width mode to fixed or variable.
     * In variable width mode, the row header will size to
     * fit the contents in it.  In fixed mode, the size is
     * explicitly set using setRowHeaderWidth().
     */
    void setRowHeaderMode(FXuint hint = LAYOUT_FIX_WIDTH) {

    }

    /// Change row header width
    void setRowHeaderWidth(FXint w) {

    }

    /* */

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(MFXTable)

    struct Column {
        /// @brief constructor
        Column(MFXTable *table) {
            // create vertical frame
            verticalFrame = new FXVerticalFrame(table, GUIDesignAuxiliarHorizontalFrame);
            // create label for column
            label = new FXLabel(verticalFrame, "", nullptr, GUIDesignLabelLeft);
        }

        FXVerticalFrame* verticalFrame = nullptr;

        FXLabel* label = nullptr;

    private:
        /// @brief default constructor
        Column() {}
    };


    /// @brief Row
    struct Row {
        /// @brief constructor
        Row(MFXTable *table) {
            // build textFields
            for (int i = 0; i < table->myColumns.size(); i++) {
                auto textField = new FXTextField(table->myColumns.at(0)->verticalFrame, GUIDesignTextFieldNCol, table, 0, GUIDesignTextField);
                textField->create();
                textFields.push_back(textField);
                
            }
        }

        /// @brief destructor
        ~Row() {
            for (const auto &textField : textFields) {
                textField->destroy();
            }
        }

        std::vector<FXTextField*> textFields;

    private:
        /// @brief default constructor
        Row() {}
    };

    /// @brief columns
    std::vector<Column*> myColumns;

    /// @brief rows
    std::vector<Row*> myRows;

    int currentRow = 0;

    /// @brief clear table
    void clear() {
        // clear rows
        for (const auto &row : myRows) {
            delete row;
        }
        // clear columns
        for (const auto &column : myColumns) {
            delete column;
        }
        // drop rows and columns
        myRows.clear();
        myColumns.clear();
    }

private:
    /// @brief Invalidated copy constructor.
    MFXTable(const MFXTable&) = delete;

    /// @brief Invalidated assignment operator.
    MFXTable& operator=(const MFXTable&) = delete;
};


#endif
