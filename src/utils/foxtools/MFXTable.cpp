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
#include <config.h>

#include "MFXTable.h"


FXDEFMAP(MFXTable) MFXTableMap[] = {
    FXMAPFUNC(SEL_ENTER,    0,  MFXTable::onEnter),
    FXMAPFUNC(SEL_LEAVE,    0,  MFXTable::onLeave),
};


// Object implementation
FXIMPLEMENT(MFXTable, FXHorizontalFrame, MFXTableMap, ARRAYNUMBER(MFXTableMap))


MFXTable::MFXTable(FXComposite *p , FXObject* tgt, FXSelector sel) :
    FXHorizontalFrame(p, GUIDesignAuxiliarFrame),
    myTarget(tgt),
    mySelector(sel) {
}


MFXTable::~MFXTable() {}


long
MFXTable::onEnter(FXObject* sender, FXSelector sel, void* ptr) {
    return FXHorizontalFrame::onEnter(sender, sel, ptr);
}


long
MFXTable::onLeave(FXObject* sender, FXSelector sel, void* ptr) {
    return FXHorizontalFrame::onLeave(sender, sel, ptr);
}


void
MFXTable::setItemText(FXint row, FXint column, const FXString& text, FXbool notify) {
    if ((row < myRows.size()) && (column < myColumns.size())) {
        myRows.at(row)->textFields.at(column)->setText(text, notify);
    } else {
        throw ProcessError("Invalid row or column");
    }
}


FXString
MFXTable::getItemText(FXint row, FXint column) const {
    if ((row < myRows.size()) && (column < myColumns.size())) {
        myRows.at(row)->textFields.at(column)->getText();
    } else {
        throw ProcessError("Invalid row or column");
    }
}


FXint
MFXTable::getNumRows() const {
    return myRows.size();
}


FXint 
MFXTable::getCurrentRow() const {
    return currentRow;
}


FXbool
MFXTable::selectRow(FXint row, FXbool notify) {
    if (row < myRows.size()) {
        myRows.at(row)->select();
    } else {
        throw ProcessError("Invalid row");
    }
}


void
MFXTable::setCurrentItem(FXint row, FXint column, FXbool notify) {
    // CHECK
}


void 
MFXTable::setColumnText(FXint column, const FXString& text) {
    if (column < myColumns.size()) {
        myColumns.at(column)->label->setText(text);
    } else {
        throw ProcessError("Invalid column");
    }
}


void
MFXTable::setTableSize(FXint numberRow, FXint numberColumn, FXbool notify) {
    // first clear table
    clearTable();
    // create columns
    for (int i = 0; i < numberColumn; i++) {
        myColumns.push_back(new Column(this));
    }
    // create rows
    for (int i = 0; i < numberRow; i++) {
        myRows.push_back(new Row(this));
    }
}


void 
MFXTable::setColumnWidth(FXint column, FXint columnWidth) {
    if (column < myColumns.size()) {
        myColumns.at(column)->verticalFrame->setWidth(columnWidth);
    } else {
        throw ProcessError("Invalid column");
    }
}


FXTextField* 
MFXTable::getItem(FXint row, FXint col) const {
    if ((row < myRows.size()) && (col < myColumns.size())) {
        return myRows.at(row)->textFields.at(col);
    }
    else {
        throw ProcessError("Invalid row or column");
    }
}


FXint 
MFXTable::getColumnWidth(FXint column) const {
    if (column < myColumns.size()) {
        return myColumns.at(column)->verticalFrame->getWidth();
    }
    else {
        throw ProcessError("Invalid column");
    }

    // CHECK

}


void 
MFXTable::setDefColumnWidth(FXint columnWidth) {
    // CHECK
}


void
MFXTable::fitColumnsToContents(FXint column, FXint nc) {
    // CHECK
}


void 
MFXTable::setColumnHeaderMode(FXuint hint) {
    // CHECK
}


void
MFXTable::setRowHeaderMode(FXuint hint) {
    // CHECK
}


void
MFXTable::setRowHeaderWidth(FXint w) {
    // CHECK
}


int
MFXTable::getSelStartRow() {
    return -1;
    // CHECK
}


/****************************************************************************/
