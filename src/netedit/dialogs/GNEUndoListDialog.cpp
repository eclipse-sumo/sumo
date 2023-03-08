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
/// @file    GNEUndoListDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2021
///
// Dialog for show undo-list
/****************************************************************************/

#include <utils/gui/div/GUIDesigns.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEUndoList.h>

#include "GNEUndoListDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEUndoListDialog) GNEUndoListDialogMap[] = {
    FXMAPFUNC(SEL_CLOSE,    0,                      GNEUndoListDialog::onCmdClose),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNEUndoListDialog::onCmdClose),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEUndoListDialog::onCmdSelectRow),
};

// Object implementation
FXIMPLEMENT(GNEUndoListDialog, FXTopWindow, GNEUndoListDialogMap, ARRAYNUMBER(GNEUndoListDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEUndoListDialog::GNEUndoListDialog(GNEApplicationWindow* GNEApp) :
    FXTopWindow(GNEApp->getApp(), "Undo/Redo history", GUIIconSubSys::getIcon(GUIIcon::UNDOLIST), GUIIconSubSys::getIcon(GUIIcon::UNDOLIST), GUIDesignDialogBoxExplicit(560, 400)),
    myGNEApp(GNEApp) {
    // create main frame
    auto mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create scroll windows for rows
    auto* scrollWindowsContents = new FXScrollWindow(mainFrame, GUIDesignContentsScrollUndoList);
    myRowFrame = new FXVerticalFrame(scrollWindowsContents, GUIDesignAuxiliarFrame);
    // add separator
    new FXSeparator(mainFrame);
    // create buttons centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(buttonsFrame, TL("OK\tclose dialog"), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
}


GNEUndoListDialog::~GNEUndoListDialog() {}


void
GNEUndoListDialog::show() {
    // recalc list
    recalcList();
    // show
    FXTopWindow::show(PLACEMENT_SCREEN);
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    myGNEApp->getApp()->runModalFor(this);

}


void
GNEUndoListDialog::hide() {
    // stop modal
    myGNEApp->getApp()->stopModal(this);
    FXTopWindow::hide();
}


bool
GNEUndoListDialog::shown() const {
    return FXWindow::shown();
}


void
GNEUndoListDialog::setFocus() {
    FXWindow::setFocus();
}


long
GNEUndoListDialog::onCmdClose(FXObject*, FXSelector, void*) {
    // close dialog
    hide();
    return 1;
}


long
GNEUndoListDialog::onCmdSelectRow(FXObject* obj, FXSelector, void*) {
    int index = 0;
    // search button
    for (const auto& row : myGUIRows) {
        if (row->getRadioButton() == obj) {
            index = row->getIndex();
        }
    }
    // now apply undo-redos
    if (index < 0) {
        for (int i = 0; i < (index * -1); i++) {
            myGNEApp->getUndoList()->undo();
        }
    } else {
        for (int i = 0; i < index; i++) {
            myGNEApp->getUndoList()->redo();
        }
    }
    // update list again
    updateList();
    return 1;
}


void
GNEUndoListDialog::updateList() {
    // declare vector of undoListRows
    std::vector<UndoListRow> undoListRows;
    // declare redo iterator over UndoList
    GNEUndoList::RedoIterator itRedo(myGNEApp->getUndoList());
    // declare index
    int index = 1;
    // fill undoListRows rows with elements to redo (in inverse)
    while (!itRedo.end()) {
        undoListRows.push_back(UndoListRow(index, itRedo.getIcon(), itRedo.getDescription(), itRedo.getTimeStamp()));
        // update counters
        itRedo++;
        index++;
    }
    // reverse undoListRows rows (because redo are inserted inverted)
    std::reverse(undoListRows.begin(), undoListRows.end());
    // declare undo iterator over UndoList
    GNEUndoList::UndoIterator itUndo(myGNEApp->getUndoList());
    // reset index
    index = 0;
    // fill undoListRows with elements to undo
    while (!itUndo.end()) {
        undoListRows.push_back(UndoListRow(index, itUndo.getIcon(), itUndo.getDescription(), itUndo.getTimeStamp()));
        // update counters
        itUndo++;
        index--;
    }
    // fill GUIRows with undoListRows
    for (int i = 0; i < (int)undoListRows.size(); i++) {
        myGUIRows.at(i)->update(undoListRows.at(i));
        if (undoListRows.at(i).index < 0) {
            myGUIRows.at(i)->setBlueBackground();
        } else if (undoListRows.at(i).index > 0) {
            myGUIRows.at(i)->setRedBackground();
        } else {
            myGUIRows.at(i)->checkRow();
        }
    }
}


void
GNEUndoListDialog::recalcList() {
    // first clear rows
    for (auto& GUIRow : myGUIRows) {
        delete GUIRow;
    }
    myGUIRows.clear();
    // declare redo iterator over undoList and fill rows
    GNEUndoList::RedoIterator itRedo(myGNEApp->getUndoList());
    while (!itRedo.end()) {
        myGUIRows.push_back(new GUIRow(this, myRowFrame, myGNEApp->getStaticTooltipView()));
        itRedo++;
    }
    // declare undo iterator over undoList and fill rows
    GNEUndoList::UndoIterator itUndo(myGNEApp->getUndoList());
    while (!itUndo.end()) {
        myGUIRows.push_back(new GUIRow(this, myRowFrame, myGNEApp->getStaticTooltipView()));
        itUndo++;
    }
    // recalc frame and update list
    myRowFrame->recalc();
    updateList();
}


GNEUndoListDialog::UndoListRow::UndoListRow(const int index_, FXIcon* icon_, const std::string description_, const std::string timestamp_) :
    index(index_),
    icon(icon_),
    description(description_),
    timestamp(timestamp_) {}


GNEUndoListDialog::GUIRow::GUIRow(GNEUndoListDialog* undoListDialog, FXVerticalFrame* mainFrame, MFXStaticToolTip* staticToolTip) {
    FXHorizontalFrame* horizontalFrame = new FXHorizontalFrame(mainFrame, GUIDesignAuxiliarHorizontalFrame);
    // build radio button
    myRadioButton = new FXRadioButton(horizontalFrame, "", undoListDialog, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonSquared);
    // build icon label
    myIcon = new FXLabel(horizontalFrame, "", nullptr, GUIDesignLabelIconThick);
    // build description label
    myTextFieldDescription = new MFXTextFieldTooltip(horizontalFrame, staticToolTip, GUIDesignTextFieldNCol, undoListDialog, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTextFieldDescription->setEditable(false);
    // build text label
    myTextFieldTimeStamp = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, undoListDialog, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldFixed(70));
    myTextFieldTimeStamp->setEditable(false);
    // create elements
    horizontalFrame->create();
    myIcon->create();
    myTextFieldDescription->create();
    myTextFieldTimeStamp->create();
}


GNEUndoListDialog::GUIRow::~GUIRow() {
    delete myRadioButton;
    delete myIcon;
    delete myTextFieldDescription;
    delete myTextFieldTimeStamp;
}


void
GNEUndoListDialog::GUIRow::update(const UndoListRow& row) {
    myIndex = row.index;
    myIcon->setIcon(row.icon);
    // check if text must be trimmed
    if (row.description.size() > 57) {
        std::string textFieldTrimmed;
        for (int i = 0; i < 57; i++) {
            textFieldTrimmed.push_back(row.description.at(i));
        }
        textFieldTrimmed.append("...");
        myTextFieldDescription->setText(textFieldTrimmed.c_str());
        myTextFieldDescription->setToolTipText(row.description.c_str());
    } else {
        myTextFieldDescription->setText(row.description.c_str());
        myTextFieldDescription->setToolTipText("");
    }
    myTextFieldTimeStamp->setText(row.timestamp.c_str());
}


int
GNEUndoListDialog::GUIRow::getIndex() const {
    return myIndex;
}


const FXRadioButton*
GNEUndoListDialog::GUIRow::getRadioButton() const {
    return myRadioButton;
}


void
GNEUndoListDialog::GUIRow::setRedBackground() {
    myRadioButton->setCheck(FALSE);
    myRadioButton->setBackColor(FXRGBA(255, 213, 213, 255));
}


void
GNEUndoListDialog::GUIRow::setBlueBackground() {
    myRadioButton->setCheck(FALSE);
    myRadioButton->setBackColor(FXRGBA(210, 233, 255, 255));
}


void
GNEUndoListDialog::GUIRow::checkRow() {
    myRadioButton->setCheck(TRUE);
    myRadioButton->setBackColor(FXRGBA(240, 255, 205, 255));
}
