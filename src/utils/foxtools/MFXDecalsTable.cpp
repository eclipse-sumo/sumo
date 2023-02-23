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
/// @file    MFXDecalsTable.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2023
///
// Table used for show and edit decal values
/****************************************************************************/
#include <config.h>

#include "MFXDecalsTable.h"

#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIDialog_ViewSettings.h>


#define EXTRAMARGING 1
#define DEFAULTWIDTH 190

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXDecalsTable) MFXDecalsTableMap[] = {
    FXMAPFUNC(MID_MBTTIP_FOCUS,     0,                                  MFXDecalsTable::onFocusRow),
    FXMAPFUNC(MID_MBTTIP_SELECTED,  0,                                  MFXDecalsTable::onCmdAddPhasePressed),
    // text fields
    FXMAPFUNC(SEL_FOCUSIN,  MID_GNE_TLSTABLE_TEXTFIELD,                 MFXDecalsTable::onFocusRow),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TLSTABLE_TEXTFIELD,                 MFXDecalsTable::onCmdEditRow),
    FXMAPFUNC(SEL_KEYPRESS, MID_GNE_TLSTABLE_TEXTFIELD,                 MFXDecalsTable::onCmdKeyPress),
    // add phase buttons
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TLSTABLE_ADDPHASE,                  MFXDecalsTable::onCmdAddPhase),
};

// Object implementation
FXIMPLEMENT(MFXDecalsTable, FXHorizontalFrame, MFXDecalsTableMap, ARRAYNUMBER(MFXDecalsTableMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// MFXDecalsTable - public methods
// ---------------------------------------------------------------------------

MFXDecalsTable::MFXDecalsTable(GUIDialog_ViewSettings* dialogViewSettingsParent, FXComposite *parent) :
    FXHorizontalFrame(parent, GUIDesignAuxiliarTLSTable),
    myIndexFont(new FXFont(getApp(), "Segoe UI", 9)),
    myIndexSelectedFont(new FXFont(getApp(), "Segoe UI", 9, FXFont::Bold)),
    myDialogViewSettings(dialogViewSettingsParent) {
}


MFXDecalsTable::~MFXDecalsTable() {
    // delete fonts
    delete myIndexFont;
    delete myIndexSelectedFont;
}


void
MFXDecalsTable::clearTable() {
    // clear rows (always before columns, because delete row delete also all cells)
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


void
MFXDecalsTable::fillTable() {
    // first clear table
    clearTable();
    // create columns
    std::string columnsType = "ibffffffff";
    for (int i = 0; i < (FXint)columnsType.size(); i++) {
        myColumns.push_back(new Column(this, i, columnsType.at(i)));
    }
    // create rows
    for (int i = 0; i < (int)myDialogViewSettings->getSUMOAbstractView()->getDecals().size(); i++) {
        myRows.push_back(new Row(this));
    }
    // if we have only a row, disable remove and move buttons
    if (myRows.size() == 1) {
        myRows.front()->disableButtons();
    }
}


void
MFXDecalsTable::setItemText(FXint row, FXint column, const std::string& text) {
    if ((row >= 0) && (row < (FXint)myRows.size()) &&
            (column >= 0) && (column < (FXint)myColumns.size())) {
        myRows.at(row)->setText(column, text);
    } else {
        throw ProcessError(TL("Invalid row or column"));
    }
}


std::string
MFXDecalsTable::getItemText(const int row, const int column) const {
    if ((row >= 0) && (row < (FXint)myRows.size()) &&
            (column >= 0) && (column < (FXint)myColumns.size())) {
        return myRows.at(row)->getText(column);
    }
    throw ProcessError(TL("Invalid row or column"));
}


int
MFXDecalsTable::getNumRows() const {
    return (int)myRows.size();
}


int
MFXDecalsTable::getCurrentSelectedRow() const {
    return myCurrentSelectedRow;
}


void
MFXDecalsTable::selectRow(const int row) {
    if ((row >= 0) && (row < (FXint)myRows.size())) {
        // update current selected row
        myCurrentSelectedRow = row;
        // update index label
        updateIndexLabel();
    } else {
        throw ProcessError(TL("Invalid row"));
    }
}


void
MFXDecalsTable::setColumnLabelTop(const int column, const std::string& text, const std::string& tooltip) {
    if ((column >= 0) && (column < (int)myColumns.size())) {
        myColumns.at(column)->setColumnLabelTop(text, tooltip);
    } else {
        throw ProcessError(TL("Invalid column"));
    }
}


void
MFXDecalsTable::setColumnLabelBot(const int column, const std::string& text) {
    if ((column >= 0) && (column < (int)myColumns.size())) {
        myColumns.at(column)->setColumnLabelBot(text);
    } else {
        throw ProcessError(TL("Invalid column"));
    }
}


long
MFXDecalsTable::onFocusRow(FXObject* sender, FXSelector, void*) {
    int selectedRow = -1;
    // search selected text field
    for (int rowIndex = 0; rowIndex < (int)myRows.size(); rowIndex++) {
        // iterate over every cell
        for (const auto& cell : myRows.at(rowIndex)->getCells()) {
            if ((cell->getTextField() == sender) || (cell->getButton() == sender)) {
                selectedRow = rowIndex;
            }
        }
    }
    // update index label
    updateIndexLabel();
    // set new row
    if (myCurrentSelectedRow != selectedRow) {
        myCurrentSelectedRow = selectedRow;
        updateIndexLabel();
    }
    return 0;
}


long
MFXDecalsTable::onCmdAddPhasePressed(FXObject* sender, FXSelector, void*) {
    // search selected add button
    for (int columnIndex = 0; columnIndex < (int)myColumns.size(); columnIndex++) {
        for (int rowIndex = 0; rowIndex < (int)myRows.size(); rowIndex++) {
            if (myRows.at(rowIndex)->getCells().at(columnIndex)->getButton() == sender) {
                ////
                return 1;
            }
        }
    }
    // nothing to focus
    return 0;
}


long
MFXDecalsTable::onCmdEditRow(FXObject* sender, FXSelector, void*) {
    // search selected text field
    for (int columnIndex = 0; columnIndex < (int)myColumns.size(); columnIndex++) {
        for (int rowIndex = 0; rowIndex < (int)myRows.size(); rowIndex++) {
            // get text field
            const auto textField = myRows.at(rowIndex)->getCells().at(columnIndex)->getTextField();
            if (textField == sender) {
/*
                // edit value and change value depending of result
                if (myTLSPhasesParent->changePhaseValue(columnIndex, rowIndex, textField->getText().text())) {
                    WRITE_DEBUG(("Valid " + myColumns.at(columnIndex)->getColumnLabelTop()).text());
                    textField->setTextColor(FXRGB(0, 0, 0));
                    textField->killFocus();
                    myTLSPhasesParent->getTLSEditorParent()->update();
                } else {
                    WRITE_DEBUG(("Invalid " + myColumns.at(columnIndex)->getColumnLabelTop()).text());
                    textField->setTextColor(FXRGB(255, 0, 0));
                }
*/
                return 1;
            }
        }
    }
    // nothing to edit
    return 0;
}


long
MFXDecalsTable::onCmdKeyPress(FXObject* sender, FXSelector sel, void* ptr) {
    // get FXEvent
    FXEvent* eventInfo = (FXEvent*)ptr;
    // check code
    if (eventInfo->code == 65362) {
        // move up
        if (myCurrentSelectedRow > 0) {
            myCurrentSelectedRow -= 1;
        } else {
            // we're in the first, then select last
            myCurrentSelectedRow = ((int)myRows.size() - 1);
        }
        // update index label
        updateIndexLabel();
        // move focus
        moveFocus();
        return 1;
    } else if (eventInfo->code == 65364) {
        // move down
        if (myCurrentSelectedRow < ((int)myRows.size() - 1)) {
            myCurrentSelectedRow += 1;
        } else {
            // we're in the last, then select first
            myCurrentSelectedRow = 0;
        }
        // update index label
        updateIndexLabel();
        // move focus
        moveFocus();
        return 1;
    } else {
        // continue handling key pres
        return sender->handle(sender, sel, ptr);
    }
}


long
MFXDecalsTable::onCmdAddPhase(FXObject* sender, FXSelector, void*) {
    WRITE_DEBUG("Add default phase");
    // search selected text field
    for (int indexRow = 0; indexRow < (int)myRows.size(); indexRow++) {
        // iterate over every cell
        for (const auto& cell : myRows.at(indexRow)->getCells()) {
            if (cell->getButton() == sender) {
                // add row
                //myTLSPhasesParent->addPhase(indexRow);
                // stop
                return 0;
            }
        }
    }
    return 0;
}


void
MFXDecalsTable::updateIndexLabel() {
    // update radio buttons checks
    for (int rowIndex = 0; rowIndex < (int)myRows.size(); rowIndex++) {
        // iterate over every cell
        for (const auto& cell : myRows.at(rowIndex)->getCells()) {
            if (cell->getIndexLabel()) {
                if (myCurrentSelectedRow == rowIndex) {
                    cell->showIndexLabelBold();
                } else {
                    cell->showIndexLabelNormal();
                }
            }
        }
    }
}


bool
MFXDecalsTable::moveFocus() {
    // first find focus
    // update radio buttons checks
    for (int rowIndex = 0; rowIndex < (int)myRows.size(); rowIndex++) {
        for (int cellIndex = 0; cellIndex < (int)myRows.at(rowIndex)->getCells().size(); cellIndex++) {
            if (myRows.at(rowIndex)->getCells().at(cellIndex)->hasFocus()) {
                // set focus in current row
                myRows.at(myCurrentSelectedRow)->getCells().at(cellIndex)->setFocus();
                return true;
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// MFXDecalsTable::Cell - methods
// ---------------------------------------------------------------------------

MFXDecalsTable::Cell::Cell(MFXDecalsTable* decalsTable, FXTextField* textField, int col, int row) :
    myDecalsTable(decalsTable),
    myTextField(textField),
    myCol(col),
    myRow(row) {
    // create
    textField->create();
}


MFXDecalsTable::Cell::Cell(MFXDecalsTable* decalsTable, FXLabel* indexLabel, FXLabel* indexLabelBold, int col, int row) :
    myDecalsTable(decalsTable),
    myIndexLabel(indexLabel),
    myIndexLabelBold(indexLabelBold),
    myCol(col),
    myRow(row) {
    // create both
    indexLabel->create();
    indexLabelBold->create();
    // hide bold and set background
    indexLabelBold->hide();
    indexLabelBold->setBackColor(FXRGBA(210, 233, 255, 255));
}


MFXDecalsTable::Cell::Cell(MFXDecalsTable* decalsTable, FXButton* button, int col, int row) :
    myDecalsTable(decalsTable),
    myButton(button),
    myCol(col),
    myRow(row) {
    // create
    button->create();
}


MFXDecalsTable::Cell::~Cell() {
    // delete all elements
    if (myTextField) {
        delete myTextField;
    }
    if (myIndexLabel) {
        delete myIndexLabel;
    }
    if (myIndexLabelBold) {
        delete myIndexLabelBold;
    }
    if (myButton) {
        delete myButton;
    }
}

void
MFXDecalsTable::Cell::enable() {
    // enable all elements
    if (myTextField) {
        myTextField->enable();
    }
    if (myIndexLabel) {
        myIndexLabel->enable();
    }
    if (myIndexLabelBold) {
        myIndexLabelBold->enable();
    }
    if (myButton) {
        myButton->enable();
    }
}


void
MFXDecalsTable::Cell::disable() {
    // disable all elements
    if (myTextField) {
        myTextField->disable();
    }
    if (myIndexLabel) {
        myIndexLabel->disable();
    }
    if (myIndexLabelBold) {
        myIndexLabelBold->disable();
    }
    if (myButton) {
        myButton->disable();
    }
}


bool
MFXDecalsTable::Cell::hasFocus() const {
    // check if one of the cell elements has the focus
    if (myTextField && myTextField->hasFocus()) {
        return true;
    } else if (myButton && myButton->hasFocus()) {
        return true;
    } else {
        return false;
    }
}


void
MFXDecalsTable::Cell::setFocus() {
    // set focus
    if (myTextField) {
        myTextField->setFocus();
    } else if (myButton) {
        myButton->setFocus();
    }
}


FXTextField*
MFXDecalsTable::Cell::getTextField() const {
    return myTextField;
}


FXLabel*
MFXDecalsTable::Cell::getIndexLabel() const {
    return myIndexLabel;
}


FXButton*
MFXDecalsTable::Cell::getButton() {
    return myButton;
}


void
MFXDecalsTable::Cell::showIndexLabelNormal() {
    myIndexLabel->show();
    myIndexLabelBold->hide();
    // recalc both
    myIndexLabel->recalc();
    myIndexLabelBold->recalc();
}


void
MFXDecalsTable::Cell::showIndexLabelBold() {
    myIndexLabel->hide();
    myIndexLabelBold->show();
    // recalc both
    myIndexLabel->recalc();
    myIndexLabelBold->recalc();
}


int
MFXDecalsTable::Cell::getCol() const {
    return myCol;
}


int
MFXDecalsTable::Cell::getRow() const {
    return myRow;
}


char
MFXDecalsTable::Cell::getType() const {
    return myDecalsTable->myColumns.at(myCol)->getType();
}


void
MFXDecalsTable::Cell::disableButton() {
    if (myButton) {
        myButton->disable();
    }
}


MFXDecalsTable::Cell::Cell() :
    myCol(-1),
    myRow(-1) {
}

// ---------------------------------------------------------------------------
// MFXDecalsTable::Column - methods
// ---------------------------------------------------------------------------

MFXDecalsTable::Column::Column(MFXDecalsTable* table, const int index, const char type) :
    myTable(table),
    myIndex(index),
    myType(type) {
    // create vertical frame
    myVerticalFrame = new FXVerticalFrame(table, GUIDesignAuxiliarTLSTable);
    // create top label
    switch (myType) {
        case 's':
        case 'i':
        case 'd':
        case 't':
        case 'b':
            // empty label
            myTopLabel = new FXLabel(myVerticalFrame, "", nullptr, GUIDesignLabelTLSTableEmpty);
            break;
        default:
            // ticked label
            myTopLabel = new FXLabel(myVerticalFrame, "", nullptr, GUIDesignLabelTLSTableEmpty);
            break;
    }
    // create vertical frame for cells
    myVerticalCellFrame = new FXVerticalFrame(myVerticalFrame, GUIDesignAuxiliarTLSTable);
    // create bot label
    switch (myType) {
        case 's':
            // label with icon
            myBotLabel = new FXLabel(myVerticalFrame, "", GUIIconSubSys::getIcon(GUIIcon::SUM), GUIDesignLabelTLSTable);
            break;
        case 'u':
        case 'p':
            // ticked label
            myBotLabel = new FXLabel(myVerticalFrame, "", nullptr, GUIDesignLabelTLSTable);
            break;
        default:
            // empty label
            myBotLabel = new FXLabel(myVerticalFrame, "", nullptr, GUIDesignLabelTLSTableEmpty);
            break;
    }
    // create elements
    myVerticalFrame->create();
    myTopLabel->create();
    myVerticalCellFrame->create();
    myBotLabel->create();
}


MFXDecalsTable::Column::~Column() {
    // delete vertical frame (this also delete all childrens)
    delete myVerticalFrame;
}


FXVerticalFrame*
MFXDecalsTable::Column::getVerticalCellFrame() const {
    return myVerticalCellFrame;
}


char
MFXDecalsTable::Column::getType() const {
    return myType;
}


FXString
MFXDecalsTable::Column::getColumnLabelTop() const {
    return myTopLabel->getText();
}


void
MFXDecalsTable::Column::setColumnLabelTop(const std::string& text, const std::string& tooltip) {
    myTopLabel->setText(text.c_str());
    myTopLabel->setTipText(tooltip.c_str());
}


void
MFXDecalsTable::Column::setColumnLabelBot(const std::string& text) {
    myBotLabel->setText(text.c_str());
}


int
MFXDecalsTable::Column::getColumnMinimumWidth() {
    // declare columnWidth
    int columnWidth = 0;
    // check column type
    if (myType == 's') {
        // set index column width
        columnWidth = 30;
    } else if (isTextFieldColumn()) {
        // calculate top label width
        columnWidth = myTopLabel->getFont()->getTextWidth(myTopLabel->getText().text(), myTopLabel->getText().length() + EXTRAMARGING);
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
        // calculate bot label width
        const auto botLabelWidth = myBotLabel->getFont()->getTextWidth(myBotLabel->getText().text(), myBotLabel->getText().length() + EXTRAMARGING);
        if (botLabelWidth > columnWidth) {
            columnWidth = botLabelWidth;
        }
    } else {
        // is an index column, then return icon size
        columnWidth = GUIDesignHeight;
    }
    return columnWidth;
}


void
MFXDecalsTable::Column::setColumnWidth(const int colWidth) {
    // only adjust for textField columns
    if (isTextFieldColumn()) {
        for (const auto& row : myTable->myRows) {
            row->getCells().at(myIndex)->getTextField()->setWidth(colWidth);
        }
    }
    // adjust labels and vertical frames
    myVerticalFrame->setWidth(colWidth);
    myTopLabel->setWidth(colWidth);
    myVerticalCellFrame->setWidth(colWidth);
    myBotLabel->setWidth(colWidth);
}


bool
MFXDecalsTable::Column::isTextFieldColumn() const {
    return ((myType == 'u') || (myType == 'f') || (myType == 'p') || (myType == 'm') || (myType == '-'));
}


MFXDecalsTable::Column::Column() :
    myIndex(0),
    myType('-') {}

// ---------------------------------------------------------------------------
// MFXDecalsTable::Row - methods
// ---------------------------------------------------------------------------

MFXDecalsTable::Row::Row(MFXDecalsTable* table) :
    myTable(table) {
    // build textFields
    for (int columnIndex = 0; columnIndex < (FXint)table->myColumns.size(); columnIndex++) {
        // get number of cells
        const int numCells = (int)myCells.size();
        // continue depending of type
        switch (table->myColumns.at(columnIndex)->getType()) {
            case ('s'): {
                // create labels for index
                auto indexLabel = new FXLabel(table->myColumns.at(columnIndex)->getVerticalCellFrame(),
                                              toString(myTable->myRows.size()).c_str(), nullptr, GUIDesignLabelTLSTableIndex);
                auto indexLabelBold = new FXLabel(table->myColumns.at(columnIndex)->getVerticalCellFrame(),
                                                  toString(myTable->myRows.size()).c_str(), nullptr, GUIDesignLabelTLSTableIndex);
                // set fonts
                indexLabel->setFont(myTable->myIndexFont);
                indexLabelBold->setFont(myTable->myIndexSelectedFont);
                myCells.push_back(new Cell(table, indexLabel, indexLabelBold, columnIndex, numCells));
                break;
            }
            case ('u'):
            case ('f'):
            case ('m'):
            case ('-'): {
                // create textField for values
                auto textField = new FXTextField(table->myColumns.at(columnIndex)->getVerticalCellFrame(),
                        GUIDesignTextFieldNCol, table, MID_GNE_TLSTABLE_TEXTFIELD, GUIDesignTextFieldTLSTable);
                myCells.push_back(new Cell(table, textField, columnIndex, numCells));
                break;
            }
            case ('d'): {
                // create button for delete phase
                auto button = new FXButton(table->myColumns.at(columnIndex)->getVerticalCellFrame(),
                    (std::string("\t") + TL("Delete phase") + std::string("\t") + TL("Delete this phase.")).c_str(),
                    GUIIconSubSys::getIcon(GUIIcon::REMOVE), table, MID_GNE_TLSTABLE_REMOVEPHASE, GUIDesignButtonIcon);
                myCells.push_back(new Cell(table, button, columnIndex, numCells));
                break;
            }
            default:
                throw ProcessError("Invalid Cell type");
        }
    }
}


MFXDecalsTable::Row::~Row() {
    // delete all cells
    for (const auto& cell : myCells) {
        delete cell;
    }
}


std::string
MFXDecalsTable::Row::getText(int index) const {
    if (myCells.at(index)->getTextField()) {
        return myCells.at(index)->getTextField()->getText().text();
    } else {
        throw ProcessError("Cell doesn't have a textField");
    }
}


void
MFXDecalsTable::Row::setText(int index, const std::string& text) const {
    // set text
    myCells.at(index)->getTextField()->setText(text.c_str());
}


const std::vector<MFXDecalsTable::Cell*>&
MFXDecalsTable::Row::getCells() const {
    return myCells;
}


void
MFXDecalsTable::Row::disableButtons() {
    // search move up button and disable it
    for (const auto& cell : myCells) {
        if ((cell->getType() == 'd') || (cell->getType() == 'b') || (cell->getType() == 't')) {
            cell->disableButton();
        }
    }
}


MFXDecalsTable::Row::Row() {}

/****************************************************************************/
