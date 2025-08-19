/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEElementTable.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Table used in GNEElementList
/****************************************************************************/

#include <netedit/GNETagProperties.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/foxtools/MFXMenuButtonTooltip.h>
#include <utils/foxtools/MFXTextFieldTooltip.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEElementTable.h"
#include "GNEElementList.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEElementTable::Row) RowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ELEMENTTABLE_EDIT,          GNEElementTable::Row::onCmdEditRow),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ELEMENTTABLE_REMOVE,        GNEElementTable::Row::onCmdRemoveRow),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ELEMENTTABLE_OPENDIALOG,    GNEElementTable::Row::onCmdOpenDialog)
};

// Object implementation
FXIMPLEMENT(GNEElementTable::Row, FXHorizontalFrame, RowMap, ARRAYNUMBER(RowMap))

// ===========================================================================
// defines
// ===========================================================================

#define TEXTCOLOR_BLACK FXRGB(0, 0, 0)
#define TEXTCOLOR_BLUE FXRGB(0, 0, 255)
#define TEXTCOLOR_RED FXRGB(255, 0, 0)
#define TEXTCOLOR_BACKGROUND_RED FXRGBA(255, 213, 213, 255)
#define TEXTCOLOR_BACKGROUND_WHITE FXRGB(255, 255, 255)

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEElementTable::RowHeader - methods
// ---------------------------------------------------------------------------

GNEElementTable::RowHeader::RowHeader(GNEElementTable* table, const GNETagProperties* tagProperties) :
    FXHorizontalFrame(table, GUIDesignAuxiliarHorizontalFrame) {
    // create horizontal label with uniform width
    auto horizontalFrameLabels = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrameUniform);
    // create empty label
    new FXLabel(horizontalFrameLabels, "", nullptr, GUIDesignLabelFixed(GUIDesignHeight));
    // create a label for every attribute
    for (const auto& attrProperty : tagProperties->getAttributeProperties()) {
        // check if this attribute can be edited in dialog
        if (attrProperty->isDialogEditor()) {
            // create label
            myLabels.push_back(new FXLabel(horizontalFrameLabels, attrProperty->getAttrStr().c_str(),
                                           nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL)));
        }
    }
    // create empty label (icons and vertical scroller)
    new FXLabel(horizontalFrameLabels, "", nullptr, GUIDesignLabelFixed(GUIDesignHeight + GUIDesignHeight + 15));
}


GNEElementTable::RowHeader::~RowHeader() {}


void
GNEElementTable::RowHeader::enableRowHeader() {
    // enable all labels
    for (const auto& label : myLabels) {
        label->enable();
    }
}


void
GNEElementTable::RowHeader::disableRowHeader() {
    // disable all labels
    for (const auto& label : myLabels) {
        label->disable();
    }
}


size_t
GNEElementTable::RowHeader::getNumColumns() const {
    return myLabels.size();
}

// ---------------------------------------------------------------------------
// GNEElementTable::Row - methods
// ---------------------------------------------------------------------------

GNEElementTable::Row::Row(GNEElementTable* elementTable, const size_t rowIndex,
                          GNEAttributeCarrier* AC, const bool allowOpenDialog) :
    FXHorizontalFrame(elementTable->myRowsFrame, GUIDesignAuxiliarHorizontalFrame),
    myRowIndex(rowIndex),
    myAC(AC) {
    // create and disable index label
    myIndexLabel = new FXLabel(this, std::to_string(rowIndex + 1).c_str(), nullptr, GUIDesignLabelIconThick);
    // create horizontal frame for text fields packed uniformly
    FXHorizontalFrame* textFieldsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrameUniform);
    // create text fields
    const auto toolTip = AC->getNet()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    for (const auto& attrProperty : AC->getTagProperty()->getAttributeProperties()) {
        // check if this attribute can be edited in dialog
        if (attrProperty->isDialogEditor()) {
            // create text field targeting the GNEElementTable
            auto textField = new MFXTextFieldTooltip(textFieldsFrame, toolTip, GUIDesignTextFieldNCol, elementTable,
                    MID_GNE_ELEMENTTABLE_EDIT, GUIDesignTextField);
            // set value from attribute carrier
            textField->setText(AC->getAttribute(attrProperty->getAttr()).c_str());
            // add in AttributeTextFields vector
            myAttributeTextFields.push_back(std::make_pair(attrProperty->getAttr(), textField));
        }
    }
    // create remove button targeting the GNEDialog
    myRemoveButton = new FXButton(this, "", GUIIconSubSys::getIcon(GUIIcon::REMOVE), this,
                                  MID_GNE_ELEMENTTABLE_REMOVE, GUIDesignButtonIcon);
    // only create open dialog button if allowed
    if (allowOpenDialog) {
        // create open dialog button targeting the GNEDialog
        myOpenDialogButton = new FXButton(this, "", GUIIconSubSys::getIcon(GUIIcon::MODEINSPECT), this,
                                          MID_GNE_ELEMENTTABLE_OPENDIALOG, GUIDesignButtonIcon);
    }
    // create row if table was previously created
    if (elementTable->id() != 0) {
        create();
        elementTable->myRowsFrame->recalc();
    }
}


GNEElementTable::Row::~Row() {}


void
GNEElementTable::Row::enableRow() {
    // enable index label
    myIndexLabel->enable();
    // enable all text fields
    for (const auto& attributeTextField : myAttributeTextFields) {
        attributeTextField.second->enable();
    }
    // enable remove button
    myRemoveButton->enable();
    // enable open dialog button if it exists
    if (myOpenDialogButton) {
        myOpenDialogButton->enable();
    }
}


void
GNEElementTable::Row::disableRow() {
    // disable index label
    myIndexLabel->disable();
    // disable all text fields
    for (const auto& attributeTextField : myAttributeTextFields) {
        attributeTextField.second->disable();
    }
    // disable remove button
    myRemoveButton->disable();
    // disable open dialog button if it exists
    if (myOpenDialogButton) {
        myOpenDialogButton->disable();
    }
}


void
GNEElementTable::Row::updateRow(GNEAttributeCarrier* AC) {
    // set new attribute carrier
    myAC = AC;
    // update text fields
    for (const auto& attributeTextField : myAttributeTextFields) {
        // get value from attribute carrier
        const std::string value = myAC->getAttribute(attributeTextField.first);
        // set text in text field
        attributeTextField.second->setText(value.c_str());
        // set valid color
        attributeTextField.second->setTextColor(TEXTCOLOR_BLACK);
    }
}


std::string
GNEElementTable::Row::getValue(const size_t column) const {
    // check index
    if ((column >= 0) && (column < myAttributeTextFields.size())) {
        // return text from text field
        return myAttributeTextFields.at(column).second->getText().text();
    } else {
        throw ProcessError("Column ndex out of bounds in GNEElementTable::Row::getValue");
    }
}


bool
GNEElementTable::Row::isValid() const {
    // iterate over all text fields
    for (const auto& attributeTextField : myAttributeTextFields) {
        // check if text fields colors are valid
        if ((attributeTextField.second->getTextColor() != TEXTCOLOR_RED) ||
                (attributeTextField.second->getBackColor() != TEXTCOLOR_BACKGROUND_RED)) {
            return false;
        }
    }
    return true;
}


long
GNEElementTable::Row::onCmdEditRow(FXObject* sender, FXSelector, void*) {
    // iterate over all text fields
    for (const auto& attributeTextField : myAttributeTextFields) {
        // check if sender is the text field
        if (attributeTextField.second == sender) {
            // get value
            const std::string value = attributeTextField.second->getText().text();
            // check if the value is valid
            if (!myAC->isValid(attributeTextField.first, value)) {
                // set red color
                attributeTextField.second->setTextColor(TEXTCOLOR_RED);
                // set background red
                if (value.empty()) {
                    attributeTextField.second->setBackColor(TEXTCOLOR_BACKGROUND_RED);
                }
            } else {
                // set value in GNEAttributeCarrier using undo-redo
                myAC->setAttribute(attributeTextField.first, value, myAC->getNet()->getViewNet()->getUndoList());
                // restore black color and kill focus
                attributeTextField.second->setTextColor(TEXTCOLOR_BLACK);
                attributeTextField.second->setBackColor(TEXTCOLOR_BACKGROUND_WHITE);
                attributeTextField.second->killFocus();
            }
            // stop after found text field
            return 1;
        }
    }
    return 0;
}


long
GNEElementTable::Row::onCmdRemoveRow(FXObject* sender, FXSelector, void*) {
    return myElementTable->myElementList->removeRow(myRowIndex);
}


long
GNEElementTable::Row::onCmdOpenDialog(FXObject* sender, FXSelector, void*) {
    return myElementTable->myElementList->openDialog(myRowIndex);
}


// ---------------------------------------------------------------------------
// GNEElementTable - methods
// ---------------------------------------------------------------------------

GNEElementTable::GNEElementTable(GNEElementList* elementList, const GNETagProperties* tagProperties,
                                 const bool fixHeight) :
    FXVerticalFrame(elementList, LAYOUT_FIX_WIDTH | (fixHeight ? LAYOUT_FIX_HEIGHT : LAYOUT_FILL_Y),
                    0, 0, 400, 300, 0, 0, 0, 0, 0, 0),
    myElementList(elementList) {
    // create row header
    myRowHeader = new RowHeader(this, tagProperties);
    // create scroll windows for rows
    myScrollWindow = new FXScrollWindow(this, GUIDesignScrollWindowFixed(400));
    // create vertical frame for rows and set back
    myRowsFrame = new FXVerticalFrame(myScrollWindow, GUIDesignAuxiliarFrame);
    myRowsFrame->setBackColor(TEXTCOLOR_BACKGROUND_WHITE);
}


GNEElementTable::~GNEElementTable() {
}


void
GNEElementTable::enableTable() {
    // enable all rows
    for (const auto& row : myRows) {
        row->enableRow();
    }
    // enable horizontal frame
    enable();
}


void
GNEElementTable::disableTable() {
    // disable all rows
    for (const auto& row : myRows) {
        row->disableRow();
    }
    // disable horizontal frame
    disable();
}


bool
GNEElementTable::isValid() const {
    // check if we have any row invalid
    for (const auto& row : myRows) {
        if (!row->isValid()) {
            return false;
        }
    }
    return true;
}


void
GNEElementTable::resizeTable(const size_t numRows) {
    // simply remove the rows if numRows is less than the current size
    while (myRows.size() > numRows) {
        delete myRows.back();
        myRows.pop_back();
    }
}


void
GNEElementTable::updateRow(const size_t index, GNEAttributeCarrier* AC) {
    // continue depending of the index
    if (index < myRows.size()) {
        // simply update the row
        myRows.at(index)->updateRow(AC);
    } else if (index == myRows.size()) {
        // create new row and add it to the list
        myRows.push_back(new Row(this, index, AC, true));
    } else {
        throw ProcessError("Index out of bounds in GNEElementTable::updateRow");
    }
}


std::string
GNEElementTable::getValue(const size_t rowIndex, const size_t columnIndex) const {
    if (rowIndex < myRows.size()) {
        return myRows.at(rowIndex)->getValue(columnIndex);
    } else {
        throw ProcessError("Row index out of bounds in GNEElementTable::getValue");
    }
}


size_t
GNEElementTable::getNumColumns() const {
    return myRowHeader->getNumColumns();
}

/****************************************************************************/
