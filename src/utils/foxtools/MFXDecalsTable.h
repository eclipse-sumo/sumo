/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXDecalsTable.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2023
///
// Table used for show and edit decal values
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>

#include <utils/common/UtilExceptions.h>
#include <utils/foxtools/MFXLabelTooltip.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GUIDialog_ViewSettings;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MFXDecalsTable
 */
class MFXDecalsTable : public FXVerticalFrame {
    /// @brief fox declaration
    FXDECLARE(MFXDecalsTable)

public:
    /// @brief constructor (Exactly like the FXButton constructor)
    MFXDecalsTable(GUIDialog_ViewSettings* dialogViewSettingsParent, FXComposite* parent);

    /// @brief destructor (Called automatically)
    ~MFXDecalsTable();

    /// @brief clear table
    void clearTable();

    /// @brief Modify cell text
    void setItemText(FXint row, FXint column, const std::string& text);

    /// @brief Return cell text
    std::string getItemText(const int row, const int column) const;

    /// @brief Get number of rows
    int getNumRows() const;

    /// @brief Get current selected row
    int getCurrentSelectedRow() const;

    /// @brief Select a row
    void selectRow(const int rowIndex);

    /// @brief Change column header text
    void setColumnLabel(const int column, const std::string& text, const std::string& tooltip = "");

    /// @brief fill table
    void fillTable();

    /// @name FOX callbacks
    /// @{

    /// @brief called when a row is focused
    long onFocusRow(FXObject*, FXSelector, void*);

    /// @brief called when a key is pressed
    long onCmdKeyPress(FXObject*, FXSelector, void*);

    /// @brief called when a string is updated
    long onCmdEditRowString(FXObject*, FXSelector, void*);

    /// @brief called when a spinner is updated
    long onCmdEditRowSpinner(FXObject*, FXSelector, void*);

    /// @brief called when a checkBox is updated
    long onCmdEditRowCheckBox(FXObject*, FXSelector, void*);

    /// @brief called when open decal button is pressed
    long onCmdOpenDecal(FXObject*, FXSelector, void*);

    /// @brief called when add row button is pressed
    long onCmdAddRow(FXObject*, FXSelector, void*);

    /// @brief update add row button
    long onUpdAddRow(FXObject*, FXSelector, void*);

    /// @brief called when remove row button is pressed
    long onCmdRemoveRow(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(MFXDecalsTable)

    /// @brief table cell
    class Cell {

    public:
        /// @brief constructor for textField
        Cell(MFXDecalsTable* decalsTable, FXTextField* textField, int col, int row);

        /// @brief constructor for index label
        Cell(MFXDecalsTable* decalsTable, FXLabel* indexLabel, FXLabel* indexLabelBold, int col, int row);

        /// @brief constructor for buttons
        Cell(MFXDecalsTable* decalsTable, FXButton* button, int col, int row);

        /// @brief constructor for check buttons
        Cell(MFXDecalsTable* decalsTable, FXCheckButton* checkButton, int col, int row);

        /// @brief constructor for spinners
        Cell(MFXDecalsTable* decalsTable, FXRealSpinner* spinner, int col, int row);

        /// @brief destructor
        ~Cell();

        /// @brief check if current cell has focus
        bool hasFocus() const;

        /// @brief set focus in the current cell
        void setFocus();

        /// @brief get textField
        FXTextField* getTextField() const;

        /// @brief get index label
        FXLabel* getIndexLabel() const;

        /// @brief get open button
        FXButton* getButton();

        /// @brief get check button
        FXCheckButton* getCheckButton();

        /// @brief get spinner
        FXRealSpinner* getSpinner();

        /// @brief show label index normal
        void showIndexLabelNormal();

        /// @brief show label index bold
        void showIndexLabelBold();

        /// @brief column index
        int getCol() const;

        /// @brief row index
        int getRow() const;

        /// @brief get column type
        char getType() const;

    private:
        /// @brief pointer to decals table parent
        MFXDecalsTable* myDecalsTable = nullptr;

        /// @brief FXTextField
        FXTextField* myTextField = nullptr;

        /// @brief index label
        FXLabel* myIndexLabel = nullptr;

        /// @brief index label bold
        FXLabel* myIndexLabelBold = nullptr;

        /// @brief button
        FXButton* myButton = nullptr;

        /// @brief spinner
        FXRealSpinner* mySpinner = nullptr;

        /// @brief check button
        FXCheckButton* myCheckButton = nullptr;

        /// @brief column index
        const int myCol;

        /// @brief row index
        const int myRow;

        /// @brief default constructor
        Cell();
    };

    /// @brief table column
    class Column {

    public:
        /// @brief constructor
        Column(MFXDecalsTable* table, const int index, const char type);

        /// @brief destructor
        ~Column();

        /// @brief get vertical cell frame
        FXVerticalFrame* getVerticalCellFrame() const;

        /// @brief get column type
        char getType() const;

        /// @brief get column label
        FXString getColumnLabel() const;

        /// @brief set column label
        void setColumnLabel(const std::string& text, const std::string& tooltip);

    private:
        /// @brief pointer to table
        MFXDecalsTable* myTable = nullptr;

        /// @brief vertical frame
        FXVerticalFrame* myVerticalFrame = nullptr;

        /// @brief column top tooltip label
        MFXLabelTooltip* myTopLabel = nullptr;

        /// @brief vertical frame
        FXVerticalFrame* myVerticalCellFrame = nullptr;

        /// @brief column index
        const int myIndex;

        /// @brief column type
        const char myType;

        /// @brief adjust column width
        void adjustColumnWidth();

        /// @brief default constructor
        Column();
    };

    /// @brief table row
    class Row {

    public:
        /// @brief constructor
        Row(MFXDecalsTable* table);

        /// @brief destructor
        ~Row();

        /// @brief get text
        std::string getText(int index) const;

        /// @brief set text
        void setText(int index, const std::string& text) const;

        /// @brief get cells
        const std::vector<Cell*>& getCells() const;

        /// @brief disable row buttons
        void disableButtons();

    protected:
        /// @brief poiner to table parent
        MFXDecalsTable* myTable = nullptr;

        /// @brief list wtih cells
        std::vector<Cell*> myCells;

    private:
        /// @brief default constructor
        Row();
    };

    /// @brief update index labels
    void updateIndexLabel();

    /// @brief move focus to current row
    bool moveFocus();

    /// @brief horizontal columns frame
    FXHorizontalFrame* myColumnsFrame = nullptr;

    /// @brief font for index
    FXFont* myIndexFont = nullptr;

    /// @brief font for index selected
    FXFont* myIndexSelectedFont = nullptr;

    /// @frame pointer to GUIDialog_ViewSettings parent
    GUIDialog_ViewSettings* myDialogViewSettings = nullptr;

    /// @brief columns
    std::vector<Column*> myColumns;

    /// @brief rows
    std::vector<Row*> myRows;

    /// @brief add button
    FXButton* myAddButton = nullptr;

    /// @brief current selected row
    int myCurrentSelectedRow = -1;

private:
    /// @brief Invalidated duplicate constructor.
    MFXDecalsTable(const MFXDecalsTable&) = delete;

    /// @brief Invalidated assignment operator.
    MFXDecalsTable& operator=(const MFXDecalsTable&) = delete;
};
