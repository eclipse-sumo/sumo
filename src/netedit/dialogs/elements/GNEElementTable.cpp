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


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEElementTable) GNEElementTableMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ELEMENTTABLE_EDIT,  GNEElementTable::onCmdEditRow)
};

// Object implementation
FXIMPLEMENT(GNEElementTable, FXVerticalFrame, GNEElementTableMap, ARRAYNUMBER(GNEElementTableMap))

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
// GNEElementTable::Row - methods
// ---------------------------------------------------------------------------

GNEElementTable::Row::Row(GNEElementTable* table, const size_t index, GNEAttributeCarrier* AC, const bool allowOpenDialog) :
    FXHorizontalFrame(table->myRowsFrame, GUIDesignAuxiliarHorizontalFrame),
    myIndex(index),
    myAC(AC) {
    // create and disable index label
    myIndexLabel = new FXLabel(this, std::to_string(index + 1).c_str(), nullptr, GUIDesignLabelIconThick);
    myIndexLabel->disable();
    // create horizontal frame for text fields packed uniformly
    FXHorizontalFrame* textFieldsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrameUniform);
    // create text fields
    const auto toolTip = AC->getNet()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    for (const auto& attrProperty : AC->getTagProperty()->getAttributeProperties()) {
        // check if this attribute can be edited in dialog
        if (attrProperty->isDialogEditor()) {
            // create text field targeting the GNEElementTable
            auto textField = new MFXTextFieldTooltip(textFieldsFrame, toolTip, GUIDesignTextFieldNCol, table,
                    MID_GNE_ELEMENTTABLE_EDIT, GUIDesignTextField);
            myTextFields.push_back(std::make_pair(textField, attrProperty->getAttr()));
        }
    }
    // create remove button targeting the GNEDialog
    myRemoveButton = new FXButton(this, "", GUIIconSubSys::getIcon(GUIIcon::REMOVE), table->myTargetDialog,
                                  MID_GNE_ELEMENTTABLE_REMOVE, GUIDesignButtonIcon);
    // only create open dialog button if allowed
    if (allowOpenDialog) {
        // create open dialog button targeting the GNEDialog
        myOpenDialogButton = new FXButton(this, "", GUIIconSubSys::getIcon(GUIIcon::MODEINSPECT), table->myTargetDialog,
                                          MID_GNE_ELEMENTTABLE_DIALOG, GUIDesignButtonIcon);
    }
}


GNEElementTable::Row::~Row() {}


void
GNEElementTable::Row::enableRow() {
    // enable index label
    myIndexLabel->enable();
    // enable all text fields
    for (const auto& textField : myTextFields) {
        textField.first->enable();
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
    for (const auto& textField : myTextFields) {
        textField.first->disable();
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
    for (const auto& textField : myTextFields) {
        // get value from attribute carrier
        const std::string value = myAC->getAttribute(textField.second);
        // set text in text field
        textField.first->setText(value.c_str());
        // set valid color
        textField.first->setTextColor(TEXTCOLOR_BLACK);
    }
}


std::string
GNEElementTable::Row::getValue(const int index) const {
    // check index
    if ((index >= 0) && (index < (int)myTextFields.size())) {
        // return text from text field
        return myTextFields.at(index).first->getText().text();
    } else {
        throw ProcessError("Index out of bounds in GNEElementTable::Row::getValue");
    }
}


void
GNEElementTable::Row::updateValue(const FXObject* sender) {
    // iterate over all text fields
    for (const auto& textField : myTextFields) {
        // check if sender is the text field
        if (textField.first == sender) {
            // check if the value is valid
            if (!myAC->isValid(textField.second, textField.first->getText().text())) {
                // set red color
                textField.first->setTextColor(TEXTCOLOR_RED);
            } else {
                // set value in GNEAttributeCarrier using undo-redo
                myAC->setAttribute(textField.second, textField.first->getText().text(), myAC->getNet()->getViewNet()->getUndoList());
                // restore black color and kill focus
                textField.first->setTextColor(TEXTCOLOR_BLACK);
                textField.first->killFocus();
            }
            // break after found text field
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// GNEElementTable - methods
// ---------------------------------------------------------------------------

GNEElementTable::GNEElementTable(FXVerticalFrame* contentFrame, GNEDialog* targetDialog, const bool fixHeight) :
    FXVerticalFrame(contentFrame, LAYOUT_FIX_WIDTH | (fixHeight ? LAYOUT_FIX_HEIGHT : LAYOUT_FILL_Y),
                    0, 0, 400, 300, 0, 0, 0, 0, 0, 0),
    myTargetDialog(targetDialog) {
    // create scroll windows for rows
    myScrollWindow = new FXScrollWindow(this, GUIDesignScrollWindowFixed);
    myScrollWindow->setWidth(400);
    // create vertical frame for rows
    myRowsFrame = new FXVerticalFrame(myScrollWindow, GUIDesignAuxiliarFrame);
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


long
GNEElementTable::onCmdEditRow(FXObject* sender, FXSelector, void*) {
    // set value in the row
    for (const auto& row : myRows) {
        row->updateValue(sender);
    }
    return 1;
}

/****************************************************************************/
