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
/// @file    GNETLSTable.cpp
/// @author  Pablo Alvarez Lopez
/// @date    2022-07-21
///
// Table used in GNETLSFrame for editing TLS programs
/****************************************************************************/
#include <config.h>

#include "GNETLSTable.h"

#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>

#define EXTRAMARGING 1
#define DEFAULTWIDTH 190

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNETLSTable) GNETLSTableMap[] = {
    FXMAPFUNC(SEL_FOCUSIN,  MID_GNE_TLSTABLE_TEXTFIELD,     GNETLSTable::onFocusRow),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TLSTABLE_TEXTFIELD,     GNETLSTable::onEditRow),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TLSTABLE_RADIOBUTTON,   GNETLSTable::onRowSelected),
    FXMAPFUNC(SEL_ENTER,    0,                              GNETLSTable::onEnter),
    FXMAPFUNC(SEL_LEAVE,    0,                              GNETLSTable::onLeave),
};


// Object implementation
FXIMPLEMENT(GNETLSTable, FXHorizontalFrame, GNETLSTableMap, ARRAYNUMBER(GNETLSTableMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNETLSTable::TableCell - methods
// ---------------------------------------------------------------------------

GNETLSTable::TableCell::TableCell(FXTextField* textField, int col, int row) :
    myTextField(textField),
    myCol(col),
    myRow(row) {
    // create
    textField->create();
}


GNETLSTable::TableCell::TableCell(FXRadioButton* radioButton, int col, int row) :
    myRadioButton(radioButton),
    myCol(col),
    myRow(row) {
    // create
    radioButton->create();
}


GNETLSTable::TableCell::TableCell(FXButton* button, int col, int row) :
    myButton(button),
    myCol(col),
    myRow(row) {
    // create
    button->create();
}


FXTextField* 
GNETLSTable::TableCell::getTextField() {
    return myTextField;
}


FXRadioButton* 
GNETLSTable::TableCell::getRadioButton() {
    return myRadioButton;
}


FXButton* 
GNETLSTable::TableCell::getButton() {
    return myButton;
}


const int 
GNETLSTable::TableCell::getCol() {
    return myCol;
}


const int
GNETLSTable::TableCell::getRow() {
    return myRow;
}


GNETLSTable::TableCell::TableCell() :
    myCol(-1),
    myRow(-1) {
}

// ---------------------------------------------------------------------------
// GNETLSTable - methods
// ---------------------------------------------------------------------------

GNETLSTable::GNETLSTable(GNETLSEditorFrame::TLSPhases* TLSPhasesParent) :
    FXHorizontalFrame(TLSPhasesParent->getCollapsableFrame(), GUIDesignAuxiliarTLSTable),
    myProgramFont(new FXFont(getApp(), "Courier New", 10)),
    myTLSPhasesParent(TLSPhasesParent) {
    // set default width
    recalcTableWidth();
}


GNETLSTable::~GNETLSTable() {
    // delete font
    delete myProgramFont;
}


void
GNETLSTable::recalcTableWidth() {
    // get width of all elements
    int tableWidth = 0;
    for (const auto& column : myColumns) {
        tableWidth += column->adjustColumnWidth();
    }
    // set new width
    setWidth((tableWidth == 0)? DEFAULTWIDTH : tableWidth);
}


void
GNETLSTable::setTableSize(const std::string columnsType, const int numberRow) {
    // first clear table
    clearTable();
    // create columns
    for (int i = 0; i < (FXint)columnsType.size(); i++) {
        myColumns.push_back(new Column(this, i, columnsType.at(i)));
    }
    // create rows
    for (int i = 0; i < numberRow; i++) {
        myRows.push_back(new Row(this));
    }
}


long
GNETLSTable::onFocusRow(FXObject* sender, FXSelector, void*) {
    // search selected text field
    for (int rowIndex = 0; rowIndex < (int)myRows.size(); rowIndex++) {
        // iterate over every cell
        for (const auto &cellTextField : myRows.at(rowIndex)->getCells()) {
            if ((cellTextField->getTextField() == sender) && (myCurrentSelectedRow != rowIndex)) {
                myCurrentSelectedRow = rowIndex;
                myTLSPhasesParent->getTLSEditorParent()->switchPhase();
                // set radio buttons checks
                for (int rowIndex2 = 0; rowIndex2 < (int)myRows.size(); rowIndex2++) {
                    // iterate over every cell
                    for (const auto &cellRadioButton : myRows.at(rowIndex2)->getCells()) {
                        if (cellRadioButton->getRadioButton()) {
                            if (myCurrentSelectedRow == rowIndex2) {
                                cellRadioButton->getRadioButton()->setCheck(TRUE, FALSE);
                            } else {
                                cellRadioButton->getRadioButton()->setCheck(FALSE, FALSE);
                            }
                        }
                    }
                }
                // row focused, then stop
                return 1;
            }
        }
    }
    return 0;
}


long 
GNETLSTable::onEditRow(FXObject* sender, FXSelector, void*) {
    // search selected text field
    for (int columnIndex = 0; columnIndex < (int)myColumns.size(); columnIndex++) {
        for (int rowIndex = 0; rowIndex < (int)myRows.size(); rowIndex++) {
            if (myRows.at(rowIndex)->getCells().at(columnIndex)->getTextField() == sender) {
                // switch phase
                myTLSPhasesParent->getTLSEditorParent()->switchPhase();
                return 1;
            }
        }
    }
    // nothing to do
    return 0;
}


long
GNETLSTable::onRowSelected(FXObject* sender, FXSelector, void*) {
    // search selected text field
    for (int indexRow = 0; indexRow < (int)myRows.size(); indexRow++) {
        // iterate over every cell
        for (const auto &cellTextField : myRows.at(indexRow)->getCells()) {
            if ((cellTextField->getRadioButton() == sender) && (myCurrentSelectedRow != indexRow)) {
                // update current selected row
                myCurrentSelectedRow = indexRow;
                // set radio buttons checks
                for (int indexRow2 = 0; indexRow2 < (int)myRows.size(); indexRow2++) {
                    // iterate over every cell
                    for (const auto &cellRadioButton : myRows.at(indexRow2)->getCells()) {
                        if (cellRadioButton->getRadioButton()) {
                            if (myCurrentSelectedRow == indexRow2) {
                                cellRadioButton->getRadioButton()->setCheck(TRUE, FALSE);
                            } else {
                                cellRadioButton->getRadioButton()->setCheck(FALSE, FALSE);
                            }
                        }
                    }
                }
                // switch phase
                myTLSPhasesParent->getTLSEditorParent()->switchPhase();
                // row focused, then stop
                return 1;
            }
        }
    }
    return 0;
}


long
GNETLSTable::onEnter(FXObject* sender, FXSelector sel, void* ptr) {
    return FXHorizontalFrame::onEnter(sender, sel, ptr);
}


long
GNETLSTable::onLeave(FXObject* sender, FXSelector sel, void* ptr) {
    return FXHorizontalFrame::onLeave(sender, sel, ptr);
}


void
GNETLSTable::setItemText(FXint row, FXint column, const FXString& text, FXbool notify) {
    if (row < (FXint)myRows.size() && column < (FXint)myColumns.size()) {
        myRows.at(row)->setText(column, text, notify);
    } else {
        throw ProcessError("Invalid row or column");
    }
}


FXString
GNETLSTable::getItemText(FXint row, FXint column) const {
    if (row < (FXint)myRows.size() && column < (FXint)myColumns.size()) {
        return myRows.at(row)->getText(column);
    }
    throw ProcessError("Invalid row or column");
}


FXint
GNETLSTable::getNumRows() const {
    return (int)myRows.size();
}


FXint
GNETLSTable::getCurrentSelectedRow() const {
    return myCurrentSelectedRow;
}


void
GNETLSTable::selectRow(const int row) {
    if ((row >= 0) && (row < (FXint)myRows.size())) {
        // select row
        myRows.at(row)->select();
    } else {
        throw ProcessError("Invalid row");
    }
}


void
GNETLSTable::setCurrentItem(FXint /* row */, FXint /* column */, FXbool /* notify */) {
    // CHECK
}


void
GNETLSTable::setColumnText(FXint column, const FXString& text) {
    if (column < (FXint)myColumns.size()) {
        myColumns.at(column)->setColumnLabel(text);
    } else {
        throw ProcessError("Invalid column");
    }
}


FXTextField*
GNETLSTable::getItem(FXint row, FXint col) const {
    if (row < (FXint)myRows.size() && col < (FXint)myColumns.size()) {
        return myRows.at(row)->getCells().at(col)->getTextField();
    }
    throw ProcessError("Invalid row or column");
}


void
GNETLSTable::clearTable() {
    // clear rows (always before columns)
    for (const auto& row : myRows) {
        delete row;
    }
    // clear columns
    for (const auto& column : myColumns) {
        delete column;
    }
    // drop rows and columns
    myRows.clear();
    myColumns.clear();
}


GNETLSTable::Column::Column(GNETLSTable* table, const int index, const char type) :
    myTable(table),
    myIndex(index),
    myType(type) {
    // create vertical frame
    myVerticalFrame = new FXVerticalFrame(table, GUIDesignAuxiliarTLSTable);
    // create label
    myLabel = new FXLabel(myVerticalFrame, "", nullptr, GUIDesignLabelTLSTable);
    // create both
    myVerticalFrame->create();
    myLabel->create();
}


GNETLSTable::Column::~Column() {
    // destroy frame and label
    myVerticalFrame->destroy();
    myLabel->destroy();
    // delete vertical frame (this also delete Label and Row textFields)
    delete myVerticalFrame;
}


FXVerticalFrame*
GNETLSTable::Column::getVerticalFrame() const {
    return myVerticalFrame;
}


char
GNETLSTable::Column::getType() const {
    return myType;
}


void
GNETLSTable::Column::setColumnLabel(const FXString& text) {
    myLabel->setText(text);
}


int
GNETLSTable::Column::adjustColumnWidth() {
    // declare columnWidth (by default is a square) 
    int columnWidth = GUIDesignHeight;
    // only adjust for textFields
    if ((myType == 'p') || (myType == '-')) {
        // calculate columnWidth using label
        columnWidth = myLabel->getFont()->getTextWidth(myLabel->getText().text(), myLabel->getText().length() + EXTRAMARGING);
        // iterate over all textFields and check widths
        for (const auto& row : myTable->myRows) {
            // get text field
            const auto textField = row->getCells().at(myIndex)->getTextField();
            // get textField width
            const auto textFieldWidth = textField->getFont()->getTextWidth(textField->getText().text(), textField->getText().length() + EXTRAMARGING);
            // compare widths
            if (textFieldWidth > columnWidth) {
                columnWidth = textFieldWidth;
            }
        }
        // adjust textFields width
        for (const auto& row : myTable->myRows) {
            row->getCells().at(myIndex)->getTextField()->setWidth(columnWidth);
        }
    }
    // adjust label and vertical frame
    myLabel->setWidth(columnWidth);
    myVerticalFrame->setWidth(columnWidth);
    // use columnWidth to set table size
    return columnWidth;
}


GNETLSTable::Column::Column() :
    myIndex(0),
    myType('-') {}


GNETLSTable::Row::Row(GNETLSTable* table) :
    myTable(table) {
    // build textFields
    for (int columnIndex = 0; columnIndex < (FXint)table->myColumns.size(); columnIndex++) {
        // get number of cells
        const int numCells = (int)myCells.size();
        // continue depending of type
        switch (table->myColumns.at(columnIndex)->getType()) {
            case ('s'): {
                // create radio button for selecting row
                auto radioButton = new FXRadioButton(table->myColumns.at(columnIndex)->getVerticalFrame(), "", table, MID_GNE_TLSTABLE_RADIOBUTTON, GUIDesignRadioButtonTLSTable);
                myCells.push_back(new TableCell(radioButton, columnIndex, numCells));
                break;
            }
            case ('p'): {
                // create text field for program (state)
                auto textField = new FXTextField(table->myColumns.at(columnIndex)->getVerticalFrame(), GUIDesignTextFieldNCol, table, MID_GNE_TLSTABLE_TEXTFIELD, GUIDesignTextFieldTLSTable);
                // set special font
                textField->setFont(myTable->myProgramFont);
                myCells.push_back(new TableCell(textField, columnIndex, numCells));
                break;
            }
            case ('i'): {
                // create button for insert phase
                auto button = new FXButton(table->myColumns.at(columnIndex)->getVerticalFrame(), "", GUIIconSubSys::getIcon(GUIIcon::ADD), table, MID_GNE_TLSTABLE_ADDPHASE, GUIDesignButtonIcon);
                myCells.push_back(new TableCell(button, columnIndex, numCells));
                break;
            }
            case ('d'): {
                // create button for delete phase
                auto button = new FXButton(table->myColumns.at(columnIndex)->getVerticalFrame(), "", GUIIconSubSys::getIcon(GUIIcon::REMOVE), table, MID_GNE_TLSTABLE_REMOVEPHASE, GUIDesignButtonIcon);
                myCells.push_back(new TableCell(button, columnIndex, numCells));
                break;
            }
            case ('-'): {
                // create text field
                auto textField = new FXTextField(table->myColumns.at(columnIndex)->getVerticalFrame(), GUIDesignTextFieldNCol, table, MID_GNE_TLSTABLE_TEXTFIELD, GUIDesignTextFieldTLSTable);
                myCells.push_back(new TableCell(textField, columnIndex, numCells));
                break;
            }
            default:
                throw ProcessError("Invalid Cell type");
        }
    }
}


GNETLSTable::Row::~Row() {
    // destroy all textFields
    for (const auto& cell : myCells) {
        if (cell->getTextField()) {
            cell->getTextField()->destroy();
        } else if (cell->getRadioButton()) {
            cell->getRadioButton()->destroy();
        }
        delete cell;
    }
}


FXString
GNETLSTable::Row::getText(int index) const {
    if (myCells.at(index)->getTextField()) {
        return myCells.at(index)->getTextField()->getText();
    } else {
        throw ProcessError("Cell doesn't have a textField");
    }
}


void
GNETLSTable::Row::setText(int index, const FXString& text, FXbool notify) const {
    // set text
    myCells.at(index)->getTextField()->setText(text, notify);
}


const std::vector<GNETLSTable::TableCell*> &
GNETLSTable::Row::getCells() const {
    return myCells;
}


void
GNETLSTable::Row::select() {
    // iterate over row and enable radio buttons
    for (const auto &cell : myCells) {
        if (cell->getRadioButton()) {
            cell->getRadioButton()->setCheck(TRUE, TRUE);
        }
    }
}


GNETLSTable::Row::Row() {}

/****************************************************************************/
