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

#include "MFXStaticToolTip.h"

/**
 * @class MFXTable
 */
class MFXTable : public FXHorizontalFrame {
    /// @brief fox declaration
    FXDECLARE(MFXTable)

public:
    /// @brief constructor (Exactly like the FXButton constructor)
    MFXTable(FXComposite *p, FXObject* tgt, FXSelector sel);

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
    void setItemText(FXint row, FXint column, const FXString& text, FXbool notify = FALSE);

    /// Return cell text
    FXString getItemText(FXint row, FXint column) const;

    /// Get number of rows
    FXint getNumRows() const;

    /// Get row number of current item
    FXint getCurrentRow() const;

    /// Select a row
    FXbool selectRow(FXint row, FXbool notify = FALSE);

    /// Get selection start row; returns -1 if no selection
/*
    FXint getSelStartRow() const { 
        return selection.fm.row; 
    }
*/

    /// Change current item
    void setCurrentItem(FXint row, FXint column, FXbool notify = FALSE);

    /// Change column header text
    void setColumnText(FXint column, const FXString& text);

    /// Set the table size to nr rows and nc columns; all existing items will be removed
    void setTableSize(FXint numberRow, FXint numberColumn, FXbool notify = FALSE);

    /// Change column width
    void setColumnWidth(FXint column, FXint columnWidth);

    /// Return the item at the given index
    FXTextField* getItem(FXint row, FXint col) const;

    /// Get column width
    FXint getColumnWidth(FXint column) const;

    /// Change default column width
    void setDefColumnWidth(FXint columnWidth);

    /// Fit column widths to contents
    void fitColumnsToContents(FXint column, FXint nc = 1);

    /**
     * Change column header height mode to fixed or variable.
     * In variable height mode, the column header will size to
     * fit the contents in it.  In fixed mode, the size is
     * explicitly set using setColumnHeaderHeight().
     */
    void setColumnHeaderMode(FXuint hint = LAYOUT_FIX_HEIGHT);

    /**
     * Change row header width mode to fixed or variable.
     * In variable width mode, the row header will size to
     * fit the contents in it.  In fixed mode, the size is
     * explicitly set using setRowHeaderWidth().
     */
    void setRowHeaderMode(FXuint hint = LAYOUT_FIX_WIDTH);

    /// Change row header width
    void setRowHeaderWidth(FXint w);

    int getSelStartRow();

    /* */

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(MFXTable)

    /// @brief column 
    class Column {

    public:
        /// @brief constructor
        Column(MFXTable *table) {
            // create vertical frame
            verticalFrame = new FXVerticalFrame(table, GUIDesignAuxiliarFrame);
            verticalFrame->create();
            // create label for column
            label = new FXLabel(verticalFrame, "", nullptr, GUIDesignLabelAttribute);
            label->create();
        }

        /// @brief destructor
        ~Column() {
            // destroy frame and label
            verticalFrame->destroy();
            label->destroy();
            // delete vertical frame (this also delete Label and Row textFields)
            delete verticalFrame;
        }

        /// @brief vertical frame
        FXVerticalFrame* verticalFrame = nullptr;

        /// @brief column label 
        FXLabel* label = nullptr;

    private:
        /// @brief default constructor
        Column() {}
    };


    /// @brief Row
    class Row {

    public:
        /// @brief constructor
        Row(MFXTable *table) {
            // build textFields
            for (int i = 0; i < table->myColumns.size(); i++) {
                auto textField = new FXTextField(table->myColumns.at(i)->verticalFrame, GUIDesignTextFieldNCol, table->myTarget, table->mySelector, GUIDesignTextFielWidth50);
                textField->create();
                textFields.push_back(textField);
            }
        }

        /// @brief destructor
        ~Row() {
            // destroy all textFields
            for (const auto &textField : textFields) {
                textField->destroy();
            }
        }

        /// @brief select column
        void select() { 
            // finish
        }

        /// @brief list of text fields
        std::vector<FXTextField*> textFields;

    private:
        /// @brief default constructor
        Row() {}
    };

    /// @brief target used in Rows
    FXObject* myTarget = nullptr;

    /// @brief selector used in rows
    FXSelector mySelector = 0;

    /// @brief columns
    std::vector<Column*> myColumns;

    /// @brief rows
    std::vector<Row*> myRows;

    int currentRow = 0;

private:
    /// @brief clear table
    void clearTable() {
        // clear rows (always before columns)
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

    /// @brief Invalidated copy constructor.
    MFXTable(const MFXTable&) = delete;

    /// @brief Invalidated assignment operator.
    MFXTable& operator=(const MFXTable&) = delete;
};


#endif
