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
/// @file    GNEFrictionCoefficientDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @author  Thomas Weber
/// @date    Jan 2018
///
// A class for edit phases of Variable Friction Coefficient
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/elements/additional/GNEFrictionCoefficient.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEFrictionCoefficientDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrictionCoefficientDialog) GNERerouterDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_FRICTIONCOEFFICIENT_ADDROW,   GNEFrictionCoefficientDialog::onCmdAddStep),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_FRICTIONCOEFFICIENT_SORT,     GNEFrictionCoefficientDialog::onCmdSortSteps),
    FXMAPFUNC(SEL_CLICKED,          MID_GNE_FRICTIONCOEFFICIENT_TABLE,    GNEFrictionCoefficientDialog::onCmdClickedStep),
    FXMAPFUNC(SEL_DOUBLECLICKED,    MID_GNE_FRICTIONCOEFFICIENT_TABLE,    GNEFrictionCoefficientDialog::onCmdClickedStep),
    FXMAPFUNC(SEL_TRIPLECLICKED,    MID_GNE_FRICTIONCOEFFICIENT_TABLE,    GNEFrictionCoefficientDialog::onCmdClickedStep),
    FXMAPFUNC(SEL_UPDATE,           MID_GNE_FRICTIONCOEFFICIENT_TABLE,    GNEFrictionCoefficientDialog::onCmdEditStep),
};

// Object implementation
FXIMPLEMENT(GNEFrictionCoefficientDialog, GNEAdditionalDialog, GNERerouterDialogMap, ARRAYNUMBER(GNERerouterDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFrictionCoefficientDialog::GNEFrictionCoefficientDialog(GNEFrictionCoefficient* editedFrictionCoefficient) :
    GNEAdditionalDialog(editedFrictionCoefficient, false, 300, 400),
    myStepsValids(false) {
    // create Horizontal frame for row elements
    FXHorizontalFrame* myAddStepFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    // create Button and Label for adding new Wors
    myAddStepButton = new FXButton(myAddStepFrame, "", GUIIconSubSys::getIcon(GUIIcon::ADD), this, MID_GNE_FRICTIONCOEFFICIENT_ADDROW, GUIDesignButtonIcon);
    new FXLabel(myAddStepFrame, ("Add new " + toString(SUMO_TAG_STEP)).c_str(), nullptr, GUIDesignLabelThick);
    // create Button and Label for sort intervals
    mySortStepButton = new FXButton(myAddStepFrame, "", GUIIconSubSys::getIcon(GUIIcon::RELOAD), this, MID_GNE_FRICTIONCOEFFICIENT_SORT, GUIDesignButtonIcon);
    new FXLabel(myAddStepFrame, ("Sort " + toString(SUMO_TAG_STEP) + "s").c_str(), nullptr, GUIDesignLabelThick);
    // create List with the data
    myStepsTable = new FXTable(myContentFrame, this, MID_GNE_FRICTIONCOEFFICIENT_TABLE, GUIDesignTableAdditionals);
    myStepsTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myStepsTable->setSelTextColor(FXRGBA(0, 0, 0, 255));
    // update table
    updateTableSteps();
    // start a undo list for editing local to this additional
    initChanges();
    // Open dialog as modal
    openAsModalDialog();
}


GNEFrictionCoefficientDialog::~GNEFrictionCoefficientDialog() {}


long
GNEFrictionCoefficientDialog::onCmdAddStep(FXObject*, FXSelector, void*) {
    // create step
    GNEFrictionCoefficientStep* step = new GNEFrictionCoefficientStep(myEditedAdditional, 0, "100");
    // add it using GNEChange_additional
    myEditedAdditional->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(step, true), true);
    // Update table
    updateTableSteps();
    return 1;
}


long
GNEFrictionCoefficientDialog::onCmdEditStep(FXObject*, FXSelector, void*) {
    // get COF children
    std::vector<GNEAdditional*> COFChildren;
    for (const auto& COFChild : myEditedAdditional->getChildAdditionals()) {
        if (!COFChild->getTagProperty().isSymbol()) {
            COFChildren.push_back(COFChild);
        }
    }
    myStepsValids = true;
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myStepsTable->getNumRows(); i++) {
        GNEAdditional* step = COFChildren.at(i);
        if (step->isValid(SUMO_ATTR_TIME, myStepsTable->getItem(i, 0)->getText().text()) == false) {
            myStepsValids = false;
            myStepsTable->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else if (step->isValid(SUMO_ATTR_FRICTION, myStepsTable->getItem(i, 1)->getText().text()) == false) {
            myStepsValids = false;
            myStepsTable->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else {
            // we need filter attribute (to avoid problemes as 1 != 1.00)
            const double time = GNEAttributeCarrier::parse<double>(myStepsTable->getItem(i, 0)->getText().text());
            const std::string friction = myStepsTable->getItem(i, 1)->getText().text();
            // set new values in Closing  reroute
            step->setAttribute(SUMO_ATTR_TIME, toString(time), myEditedAdditional->getNet()->getViewNet()->getUndoList());
            step->setAttribute(SUMO_ATTR_FRICTION, friction, myEditedAdditional->getNet()->getViewNet()->getUndoList());
            // set Correct label
            myStepsTable->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(GUIIcon::CORRECT));
        }
    }
    // update list
    myStepsTable->update();
    return 1;
}


long
GNEFrictionCoefficientDialog::onCmdClickedStep(FXObject*, FXSelector, void*) {
    // get COF children
    std::vector<GNEAdditional*> COFChildren;
    for (const auto& COFChild : myEditedAdditional->getChildAdditionals()) {
        if (!COFChild->getTagProperty().isSymbol()) {
            COFChildren.push_back(COFChild);
        }
    }
    // check if some delete button was pressed
    for (int i = 0; i < (int)COFChildren.size(); i++) {
        if (myStepsTable->getItem(i, 3)->hasFocus()) {
            myStepsTable->removeRows(i);
            myEditedAdditional->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(COFChildren.at(i), false), true);
            // Update table
            updateTableSteps();
            return 1;
        }
    }
    return 0;
}


long
GNEFrictionCoefficientDialog::onCmdSortSteps(FXObject*, FXSelector, void*) {
    // update table
    updateTableSteps();
    return 1;
}


long
GNEFrictionCoefficientDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myStepsValids == false) {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox of type 'warning'");
        // open warning Box
        FXMessageBox::warning(getApp(), MBOX_OK, ("Error updating " + toString(SUMO_TAG_COF) + " " + toString(SUMO_TAG_STEP)).c_str(), "%s",
                              (toString(SUMO_TAG_COF) + " " + toString(SUMO_TAG_STEP) + "cannot be updated because there are invalid values").c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox of type 'warning' with 'OK'");
        return 0;
    } else {
        // accept changes before closing dialog
        acceptChanges();
        // stop dialgo sucesfully
        getApp()->stopModal(this, TRUE);
        return 1;
    }
}


long
GNEFrictionCoefficientDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // cancel changes
    cancelChanges();
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNEFrictionCoefficientDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update steps tables
    updateTableSteps();
    return 1;
}


void
GNEFrictionCoefficientDialog::updateTableSteps() {
    // get COF children
    std::vector<GNEAdditional*> COFChildren;
    for (const auto& COFChild : myEditedAdditional->getChildAdditionals()) {
        if (!COFChild->getTagProperty().isSymbol()) {
            COFChildren.push_back(COFChild);
        }
    }
    // clear table
    myStepsTable->clearItems();
    // set number of rows
    myStepsTable->setTableSize(int(COFChildren.size()), 4);
    // Configure list
    myStepsTable->setVisibleColumns(4);
    myStepsTable->setColumnWidth(0, 115);
    myStepsTable->setColumnWidth(1, 114);
    myStepsTable->setColumnWidth(2, GUIDesignHeight);
    myStepsTable->setColumnWidth(3, GUIDesignHeight);
    myStepsTable->setColumnText(0, "timeStep");
    myStepsTable->setColumnText(1, "friction (%)");
    myStepsTable->setColumnText(2, "");
    myStepsTable->setColumnText(3, "");
    myStepsTable->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    FXTableItem* item = nullptr;
    // iterate over values
    for (int i = 0; i < (int)COFChildren.size(); i++) {
        // Set time
        item = new FXTableItem(COFChildren.at(i)->getAttribute(SUMO_ATTR_TIME).c_str());
        myStepsTable->setItem(i, 0, item);
        // Set friction
        item = new FXTableItem(COFChildren.at(i)->getAttribute(SUMO_ATTR_FRICTION).c_str());
        myStepsTable->setItem(i, 1, item);
        // set valid icon
        item = new FXTableItem("");
        item->setIcon(GUIIconSubSys::getIcon(GUIIcon::CORRECT));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myStepsTable->setItem(i, 2, item);
        // set remove Icon
        item = new FXTableItem("", GUIIconSubSys::getIcon(GUIIcon::REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myStepsTable->setItem(i, 3, item);
    }
}


/****************************************************************************/
