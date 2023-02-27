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

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXDecalsTable) MFXDecalsTableMap[] = {
    FXMAPFUNC(MID_MBTTIP_FOCUS,     0,                  MFXDecalsTable::onFocusRow),
    // text fields
    FXMAPFUNC(SEL_FOCUSIN,  MID_DECALSTABLE_TEXTFIELD,  MFXDecalsTable::onFocusRow),
    FXMAPFUNC(SEL_COMMAND,  MID_DECALSTABLE_TEXTFIELD,  MFXDecalsTable::onCmdEditRow),
    FXMAPFUNC(SEL_KEYPRESS, MID_DECALSTABLE_TEXTFIELD,  MFXDecalsTable::onCmdKeyPress),
    // row funcitons
    FXMAPFUNC(SEL_COMMAND,  MID_DECALSTABLE_OPEN,       MFXDecalsTable::onCmdOpenDecal),
    FXMAPFUNC(SEL_COMMAND,  MID_DECALSTABLE_ADD,        MFXDecalsTable::onCmdAddRow),
    FXMAPFUNC(SEL_COMMAND,  MID_DECALSTABLE_REMOVE,     MFXDecalsTable::onCmdRemoveRow),
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
    FXHorizontalFrame(parent, GUIDesignMFXDecalTable),
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
    std::string columnsType = "ibfssssssc";
    for (int i = 0; i < (FXint)columnsType.size(); i++) {
        myColumns.push_back(new Column(this, i, columnsType.at(i)));
    }
    // create rows
    for (const auto &decal : myDialogViewSettings->getSUMOAbstractView()->getDecals()) {
        // create row
        auto row = new Row(this);
        // fill cells
        row->getCells().at(2)->getTextField()->setText(decal.filename.c_str());
        row->getCells().at(3)->getTextField()->setText(toString(decal.centerX).c_str());
        row->getCells().at(4)->getTextField()->setText(toString(decal.centerY).c_str());
        row->getCells().at(5)->getTextField()->setText(toString(decal.width).c_str());
        row->getCells().at(6)->getTextField()->setText(toString(decal.height).c_str());
        row->getCells().at(7)->getTextField()->setText(toString(decal.rot).c_str());
        row->getCells().at(8)->getTextField()->setText(toString(decal.layer).c_str());
        row->getCells().at(9)->getCheckButton()->setCheck(decal.screenRelative);
        myRows.push_back(row);
    }
    // set headers
    myColumns.at(2)->setColumnLabel("filename", "");
    myColumns.at(3)->setColumnLabel("centerX", "");
    myColumns.at(4)->setColumnLabel("centerY", "");
    myColumns.at(5)->setColumnLabel("width", "");
    myColumns.at(6)->setColumnLabel("height", "");
    myColumns.at(7)->setColumnLabel("rotation", "");
    myColumns.at(8)->setColumnLabel("layer", "");
    myColumns.at(9)->setColumnLabel("r", "relative");
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
MFXDecalsTable::setColumnLabel(const int column, const std::string& text, const std::string& tooltip) {
    if ((column >= 0) && (column < (int)myColumns.size())) {
        myColumns.at(column)->setColumnLabel(text, tooltip);
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
MFXDecalsTable::onCmdOpenDecal(FXObject*, FXSelector, void*) {
    return 1;
}


long
MFXDecalsTable::onCmdAddRow(FXObject* sender, FXSelector, void*) {
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
MFXDecalsTable::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    return 1;
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


MFXDecalsTable::Cell::Cell(MFXDecalsTable* decalsTable, FXCheckButton* checkButton, int col, int row) :
    myDecalsTable(decalsTable),
    myCheckButton(checkButton),
    myCol(col),
    myRow(row) {
    // create
    checkButton->create();
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
    if (myCheckButton) {
        delete myCheckButton;
    }
}


bool
MFXDecalsTable::Cell::hasFocus() const {
    // check if one of the cell elements has the focus
    if (myTextField && myTextField->hasFocus()) {
        return true;
    } else if (myButton && myButton->hasFocus()) {
        return true;
    } else if (myCheckButton && myCheckButton->hasFocus()) {
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
    } else if (myCheckButton) {
        myCheckButton->setFocus();
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


FXCheckButton*
MFXDecalsTable::Cell::getCheckButton() {
    return myCheckButton;
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
    if (myType == 'f') {
        myVerticalFrame = new FXVerticalFrame(table, GUIDesignAuxiliarFrame);
    } else {
        myVerticalFrame = new FXVerticalFrame(table, GUIDesignAuxiliarFrameFixWidth);
    }
    // create top label
    if (myType == 'f') {
        // ticked label extended
        myTopLabel = new MFXLabelTooltip(myVerticalFrame, 
            table->myDialogViewSettings->getSUMOAbstractView()->getGUIGlChildWindow()->getGUIMainWindowParent()->getStaticTooltipMenu(), 
            "", nullptr, GUIDesignLabelCenterThick);
    } else if (myType == 's') {
        // ticked label fixed
        myTopLabel = new MFXLabelTooltip(myVerticalFrame, 
            table->myDialogViewSettings->getSUMOAbstractView()->getGUIGlChildWindow()->getGUIMainWindowParent()->getStaticTooltipMenu(), 
            "", nullptr, GUIDesignLabelFixedWidthTicked);
    } else if (myType == 'c') {
        // ticked label for checkbox
        myTopLabel = new MFXLabelTooltip(myVerticalFrame, 
            table->myDialogViewSettings->getSUMOAbstractView()->getGUIGlChildWindow()->getGUIMainWindowParent()->getStaticTooltipMenu(), 
            "", nullptr, GUIDesignLabelThick30);
    } else if (myType == 'i') {
        // ticked label for index
        myTopLabel = new MFXLabelTooltip(myVerticalFrame, 
            table->myDialogViewSettings->getSUMOAbstractView()->getGUIGlChildWindow()->getGUIMainWindowParent()->getStaticTooltipMenu(), 
            "", nullptr, GUIDesignLabel30);
    } else {
        // empty label (for buttons)
        myTopLabel = new MFXLabelTooltip(myVerticalFrame, 
            table->myDialogViewSettings->getSUMOAbstractView()->getGUIGlChildWindow()->getGUIMainWindowParent()->getStaticTooltipMenu(),
            "", nullptr, GUIDesignLabelFixedWidth);
    }
    // create vertical frame for cells
    if (myType == 'f') {
        myVerticalCellFrame = new FXVerticalFrame(myVerticalFrame, GUIDesignAuxiliarFrame);
    } else {
        myVerticalCellFrame = new FXVerticalFrame(myVerticalFrame, GUIDesignAuxiliarFrameFixWidth);
    }
    // create elements
    myVerticalFrame->create();
    myTopLabel->create();
    myVerticalCellFrame->create();
    // adjust column width
    adjustColumnWidth();
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
MFXDecalsTable::Column::getColumnLabel() const {
    return myTopLabel->getText();
}


void
MFXDecalsTable::Column::setColumnLabel(const std::string& text, const std::string& tooltip) {
    myTopLabel->setText(text.c_str());
    myTopLabel->setTipText(tooltip.c_str());
    // adjust column width
    adjustColumnWidth();
}


void
MFXDecalsTable::Column::adjustColumnWidth() {
    // filename always extended
    if (myType != 'f') {
        // declare columnWidth
        int columnWidth = GUIDesignHeight;
        // adjust depending of label
        if (myType == 's') {
            // calculate top label width
            columnWidth = myTopLabel->getFont()->getTextWidth(myTopLabel->getText().text(), myTopLabel->getText().length() + EXTRAMARGING);
        }
        // adjust width in all rows
        for (const auto& row : myTable->myRows) {
            if (row->getCells().at(myIndex)->getTextField()) {
                row->getCells().at(myIndex)->getTextField()->setWidth(columnWidth);
            } else if (row->getCells().at(myIndex)->getButton()) {
                row->getCells().at(myIndex)->getButton()->setWidth(columnWidth);
            }
        }
        // adjust labels and vertical frames
        myVerticalFrame->setWidth(columnWidth);
        myTopLabel->setWidth(columnWidth);
        myVerticalCellFrame->setWidth(columnWidth);
    }
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
            case ('f'): {
                // create textField for values
                auto textField = new FXTextField(table->myColumns.at(columnIndex)->getVerticalCellFrame(),
                        GUIDesignTextFieldNCol, table, MID_DECALSTABLE_TEXTFIELD, GUIDesignTextField);
                myCells.push_back(new Cell(table, textField, columnIndex, numCells));
                break;
            }
            case ('s'): {
                // create textField for values
                auto textField = new FXTextField(table->myColumns.at(columnIndex)->getVerticalCellFrame(),
                    GUIDesignTextFieldNCol, table, MID_DECALSTABLE_TEXTFIELD, GUIDesignTextFieldTickedMinWidth);
                myCells.push_back(new Cell(table, textField, columnIndex, numCells));
                break;
            }
            case ('i'): {
                // create labels for index
                auto indexLabel = new FXLabel(table->myColumns.at(columnIndex)->getVerticalCellFrame(),
                    toString(myTable->myRows.size()).c_str(), nullptr, GUIDesignLabelThick30);
                auto indexLabelBold = new FXLabel(table->myColumns.at(columnIndex)->getVerticalCellFrame(),
                    toString(myTable->myRows.size()).c_str(), nullptr, GUIDesignLabelThick30);
                // set fonts
                indexLabel->setFont(myTable->myIndexFont);
                indexLabelBold->setFont(myTable->myIndexSelectedFont);
                myCells.push_back(new Cell(table, indexLabel, indexLabelBold, columnIndex, numCells));
                break;
            }
            case ('b'): {
                // create button for open decal
                auto button = new FXButton(table->myColumns.at(columnIndex)->getVerticalCellFrame(),
                    (std::string("\t") + TL("Open decal") + std::string("\t") + TL("Open decal.")).c_str(),
                    GUIIconSubSys::getIcon(GUIIcon::OPEN), table, MID_GNE_TLSTABLE_REMOVEPHASE, GUIDesignButtonIcon);
                myCells.push_back(new Cell(table, button, columnIndex, numCells));
                break;
            }
            case ('c'): {
                // create checkbox for 
                auto checkableButton = new FXCheckButton(table->myColumns.at(columnIndex)->getVerticalCellFrame(),
                        "", table, MID_HOTKEY_C_MODE_CONNECT_PERSONPLAN, GUIDesignMFXCheckableButtonSquare);
                myCells.push_back(new Cell(table, checkableButton, columnIndex, numCells));
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


MFXDecalsTable::Row::Row() {}

/****************************************************************************/
