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

#include <utils/gui/windows/GUIAppEnum.h>

#define EXTRAMARGING 1

FXDEFMAP(GNETLSTable) GNETLSTableMap[] = {
    FXMAPFUNC(SEL_FOCUSIN,  MID_CHOOSEN_SELECT, GNETLSTable::onFocusRow),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SELECT, GNETLSTable::onEditRow),
    FXMAPFUNC(SEL_ENTER,    0,                  GNETLSTable::onEnter),
    FXMAPFUNC(SEL_LEAVE,    0,                  GNETLSTable::onLeave),
};


// Object implementation
FXIMPLEMENT(GNETLSTable, FXHorizontalFrame, GNETLSTableMap, ARRAYNUMBER(GNETLSTableMap))


GNETLSTable::GNETLSTable(FXComposite* p, FXObject* tgt, FXSelector sel) :
    FXHorizontalFrame(p, GUIDesignAuxiliarTLSTable),
    myTarget(tgt),
    myTablePos(new GNETLSTablePos()),
    mySelector(sel) {
}


GNETLSTable::~GNETLSTable() {
    delete myTablePos;
}


long
GNETLSTable::onFocusRow(FXObject* sender, FXSelector, void*) {
    // search selected text field
    for (int rowIndex = 0; rowIndex < (int)myRows.size(); rowIndex++) {
        // iterate over every cell
        for (const auto &cellTextField : myRows.at(rowIndex)->getCells()) {
            if ((cellTextField.textField == sender) && (myCurrentSelectedRow != rowIndex)) {
                myCurrentSelectedRow = rowIndex;
                myTarget->handle(this, FXSEL(SEL_SELECTED, mySelector), nullptr);
                // set radio buttons checks
                for (int rowIndex2 = 0; rowIndex2 < (int)myRows.size(); rowIndex2++) {
                    // iterate over every cell
                    for (const auto &cellRadioButton : myRows.at(rowIndex2)->getCells()) {
                        if (cellRadioButton.radioButton) {
                            if (myCurrentSelectedRow == rowIndex2) {
                                cellRadioButton.radioButton->setCheck(TRUE, FALSE);
                            } else {
                                cellRadioButton.radioButton->setCheck(FALSE, FALSE);
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
            if (myRows.at(rowIndex)->getCells().at(columnIndex).textField == sender) {
                // update myTablePos
                myTablePos->col = columnIndex;
                myTablePos->row = rowIndex;
                // inform target
                return myTarget->handle(this, FXSEL(SEL_REPLACED, mySelector), myTablePos);
                // stop
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
    return myRows.size();
}


FXint
GNETLSTable::getCurrentSelectedRow() const {
    return myCurrentSelectedRow;
}


FXbool
GNETLSTable::selectRow(FXint row, FXbool /* notify */) {
    if (row < (FXint)myRows.size()) {
        myRows.at(row)->select();
        return TRUE;
    }
    throw ProcessError("Invalid row");
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


void
GNETLSTable::setTableSize(const std::string columns, FXint numberRow, FXbool /* notify */) {
    // first clear table
    clearTable();
    // create columns
    for (int i = 0; i < (FXint)columns.size(); i++) {
        myColumns.push_back(new Column(this, i, columns.at(i)));
    }
    // create rows
    for (int i = 0; i < numberRow; i++) {
        myRows.push_back(new Row(this));
    }
    // adjust table size
    for (const auto& column : myColumns) {
        column->adjustColumnWidth();
    }
}


FXTextField*
GNETLSTable::getItem(FXint row, FXint col) const {
    if (row < (FXint)myRows.size() && col < (FXint)myColumns.size()) {
        return myRows.at(row)->getCells().at(col).textField;
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
    // continue depending of type
    if (myType == '-') {
        // create vertical frame extended over frame
        myVerticalFrame = new FXVerticalFrame(table, GUIDesignAuxiliarTLSTable);
        // create label extended over frame
        myLabel = new FXLabel(myVerticalFrame, "", nullptr, GUIDesignLabelTLSTable);
    } else {
        // create vertical frame with fixed height
        myVerticalFrame = new FXVerticalFrame(table, GUIDesignAuxiliarTLSTableSquare);
        // create label extended with fixed height
        myLabel = new FXLabel(myVerticalFrame, "", nullptr, GUIDesignLabelTLSTableSquare);
    }
    // create botwh
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
    // adjust column width
    adjustColumnWidth();
}


void
GNETLSTable::Column::adjustColumnWidth() {
    // only adjust for textfields
    if (myType == '-') {
        // declare width using label
        int width = myLabel->getFont()->getTextWidth(myLabel->getText().text(), myLabel->getText().length() + EXTRAMARGING);
        // iterate over all textFields and check widths
        for (const auto& row : myTable->myRows) {
            // get text field
            const auto textField = row->getCells().at(myIndex).textField;
            // get textField width
            const auto textFieldWidth = textField->getFont()->getTextWidth(textField->getText().text(), textField->getText().length() + EXTRAMARGING);
            // compare widths
            if (textFieldWidth > width) {
                width = textFieldWidth;
            }
        }
        // set width in all elements
        myLabel->setWidth(width);
        for (const auto& row : myTable->myRows) {
            row->getCells().at(myIndex).textField->setWidth(width);
        }
    }
}


GNETLSTable::Column::Column() :
    myIndex(0),
    myType('-') {}


GNETLSTable::Row::Row(GNETLSTable* table) :
    myTable(table) {
    // build textFields
    for (int i = 0; i < (FXint)table->myColumns.size(); i++) {
        if (table->myColumns.at(i)->getType() == 's') {
            auto radioButton = new FXRadioButton(table->myColumns.at(i)->getVerticalFrame(), "", table, MID_CHOOSEN_SELECT, GUIDesignRadioButtonTLSTable);
            radioButton->create();
            myCells.push_back(Cell(radioButton));
        } else if (table->myColumns.at(i)->getType() == '-') {
            auto textField = new FXTextField(table->myColumns.at(i)->getVerticalFrame(), GUIDesignTextFieldNCol, table, MID_CHOOSEN_SELECT, GUIDesignTextFieldTLSTable);
            textField->create();
            myCells.push_back(Cell(textField));
        } else {
            throw ProcessError("Invalid Cell type");
        }
    }
}


GNETLSTable::Row::~Row() {
    // destroy all textFields
    for (const auto& cell : myCells) {
        if (cell.textField) {
            cell.textField->destroy();
        } else if (cell.radioButton) {
            cell.radioButton->destroy();
        }
    }
}


FXString
GNETLSTable::Row::getText(int index) const {
    if (myCells.at(index).textField) {
        return myCells.at(index).textField->getText();
    } else {
        throw ProcessError("Cell doesn't have a textField");
    }
}


void
GNETLSTable::Row::setText(int index, const FXString& text, FXbool notify) const {
    myCells.at(index).textField->setText(text, notify);
    // adjust column width
    myTable->myColumns.at(index)->adjustColumnWidth();
}


const std::vector<GNETLSTable::Row::Cell> &
GNETLSTable::Row::getCells() const {
    return myCells;
}


void GNETLSTable::Row::select() {
    // finish
}


GNETLSTable::Row::Row() {}

/****************************************************************************/
