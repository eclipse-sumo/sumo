/****************************************************************************/
/// @file    GNEDialog_FixStoppingPlaces.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
/// @version $Id$
///
/// Dialog used to fix invalid stopping places
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/common/ToString.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/gui/div/GUIGlobalSelection.h>

#include "GNEDialog_FixStoppingPlaces.h"
#include "GNEAdditional.h"
#include "GNEStoppingPlace.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNEChange_Selection.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDialog_FixStoppingPlaces) GNEDialog_FixStoppingPlacesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,                  GNEDialog_FixStoppingPlaces::onCmdSelectOption),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,   GNEDialog_FixStoppingPlaces::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,   GNEDialog_FixStoppingPlaces::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GNEDialog_FixStoppingPlaces, FXDialogBox, GNEDialog_FixStoppingPlacesMap, ARRAYNUMBER(GNEDialog_FixStoppingPlacesMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDialog_FixStoppingPlaces::GNEDialog_FixStoppingPlaces(GNEViewNet *viewNet, std::vector<GNEAdditional*> invalidStoppingPlacesAndE2) :
    FXDialogBox(viewNet->getApp(), ("Fix stoppingPlaces positions"), GUIDesignDialogBoxExplicit, 0, 0, 458, 300, 0, 0, 0, 0),
    myViewNet(viewNet),
    myInvalidStoppingPlacesAndE2(invalidStoppingPlacesAndE2) {
    // set busStop icon for this dialog
    setIcon(GUIIconSubSys::getIcon(ICON_BUSSTOP));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create label for table
    new FXLabel(mainFrame, "List of Stopping places and E2 detectors with conflicts", 0, GUIDesignLabelCenterThick);
    // Create table, copy intervals and update table
    myTable = new FXTable(mainFrame, this, MID_GNE_FIXSTOPPINGPLACES_CHANGE, GUIDesignTableAdditionals);
    myTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myTable->setSelTextColor(FXRGBA(0, 0, 0, 255));
    myTable->setEditable(false);
    // clear table
    myTable->clearItems();
    // set number of rows
    myTable->setTableSize(int(myInvalidStoppingPlacesAndE2.size()), 3);
    // Configure list
    myTable->setVisibleColumns(4);
    myTable->setColumnWidth(0, GUIDesignTableIconCellWidth);
    myTable->setColumnWidth(1, 160);
    myTable->setColumnWidth(2, 260);
    myTable->setColumnText(0, "");
    myTable->setColumnText(1, toString(SUMO_ATTR_ID).c_str());
    myTable->setColumnText(2, "Conflict");
    myTable->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (std::vector<GNEAdditional*>::iterator i = myInvalidStoppingPlacesAndE2.begin(); i != myInvalidStoppingPlacesAndE2.end(); i++) {
        // Set icon
        item = new FXTableItem("", (*i)->getIcon());
        item->setIconPosition(FXTableItem::CENTER_X);
        myTable->setItem(indexRow, 0, item);
        // Set ID
        item = new FXTableItem((*i)->getID().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 1, item);
        // Set conflict
        if(dynamic_cast<GNEStoppingPlace*>(*i) != NULL) {
            GNEStoppingPlace *stoppingPlace = dynamic_cast<GNEStoppingPlace*>(*i);
            std::string errorStartPosition, errorEndPosition, separator;
            // check start position
            if(stoppingPlace->getAbsoluteStartPosition() < 0) {
                errorStartPosition = (toString(SUMO_ATTR_STARTPOS) + " < 0");
            }
            // check end position
            if(stoppingPlace->getAbsoluteEndPosition() > stoppingPlace->getLane()->getLaneParametricLength()) {
                errorEndPosition = (toString(SUMO_ATTR_ENDPOS) + " > lanes's length");
            }
            // check separator
            if((errorStartPosition.size() > 0) && (errorEndPosition.size() > 0)) {
                separator = " and ";
            }
            item = new FXTableItem((errorStartPosition + separator + errorEndPosition).c_str());
            item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
            myTable->setItem(indexRow, 2, item);
        }
        // Update index
        indexRow++;
    }
    // create label for elements
    new FXLabel(mainFrame, "Select a solution:", 0, GUIDesignLabelCenterThick);
    // create horizontal frames for radio buttons
    FXHorizontalFrame* RadioButtons = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    // create Vertical Frame for left options
    FXVerticalFrame* RadioButtonsLeft = new FXVerticalFrame(RadioButtons, GUIDesignAuxiliarVerticalFrame);
    myOptionA = new FXRadioButton(RadioButtonsLeft, "Activate friendlyPos and save\t\tFriendly pos parameter will be activated in all stopping places and E2 detectors",
                                      this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myOptionC = new FXRadioButton(RadioButtonsLeft, "Save invalid positions\t\tSave stopping places and E2 detectors with invalid positions",
                                      this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // create Vertical Frame for right options
    FXVerticalFrame* RadioButtonsRight = new FXVerticalFrame(RadioButtons, GUIDesignAuxiliarVerticalFrame);
    myOptionB = new FXRadioButton(RadioButtonsRight, "Fix positions and save\t\tPosition of stopping places and E2 detectors will be fixed",
                                      this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myOptionD = new FXRadioButton(RadioButtonsRight, "Select invalid additionals\t\tCancel saving of additionals and select invalid stopping places and E2 detectors",
                                      this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // leave option a as default
    myOptionA->setCheck(true);
    // create dialog buttons bot centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, "accept\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), this, MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, "cancel\t\tclose", GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_GNE_MODE_ADDITIONALDIALOG_CANCEL, GUIDesignButtonCancel);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
}


GNEDialog_FixStoppingPlaces::~GNEDialog_FixStoppingPlaces() {
}


long 
GNEDialog_FixStoppingPlaces::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if(obj == myOptionA) {
        myOptionA->setCheck(true);
        myOptionB->setCheck(false);
        myOptionC->setCheck(false);
        myOptionD->setCheck(false);
        return 1;
    } else if(obj == myOptionB) {
        myOptionA->setCheck(false);
        myOptionB->setCheck(true);
        myOptionC->setCheck(false);
        myOptionD->setCheck(false);
        return 1;
    } else if(obj == myOptionC) {
        myOptionA->setCheck(false);
        myOptionB->setCheck(false);
        myOptionC->setCheck(true);
        myOptionD->setCheck(false);
        return 1;
    } else if(obj == myOptionD) {
        myOptionA->setCheck(false);
        myOptionB->setCheck(false);
        myOptionC->setCheck(false);
        myOptionD->setCheck(true);
        return 1;
    } else {
        return 0;
    }
}


long
GNEDialog_FixStoppingPlaces::onCmdAccept(FXObject*, FXSelector, void*) {

    if(myOptionA->getCheck() == true) {
        // stop modal with TRUE
        getApp()->stopModal(this, TRUE);
        return 1;
    } else if(myOptionB->getCheck() == true) {
        // stop modal with TRUE
        getApp()->stopModal(this, TRUE);
        return 1;
    } else if(myOptionC->getCheck() == true) {
        // stop modal with TRUE
        getApp()->stopModal(this, TRUE);
        return 1;
    } else if(myOptionD->getCheck() == true) {
        std::set<GUIGlID> GLIDsToSelect;
        myViewNet->getUndoList()->p_begin("select invalid additionals");
        // clear previous selection
        myViewNet->getUndoList()->add(new GNEChange_Selection(myViewNet->getNet(), std::set<GUIGlID>(), gSelected.getSelected(), true), true);
        // iterate over invalid stopping places and E2 to select it
        for(std::vector<GNEAdditional*>::iterator i = myInvalidStoppingPlacesAndE2.begin(); i != myInvalidStoppingPlacesAndE2.end(); i++) {
            GLIDsToSelect.insert((*i)->getGlID());
        }
        myViewNet->getUndoList()->add(new GNEChange_Selection(myViewNet->getNet(), GLIDsToSelect, std::set<GUIGlID>(), true), true);
        myViewNet->getUndoList()->p_end();
        myViewNet->update();
        // stop modal with FALSE
        getApp()->stopModal(this, FALSE);
        return 0;
    } else {
        // stop modal with FALSE
        getApp()->stopModal(this, FALSE);
        return 0;
    }

}


long
GNEDialog_FixStoppingPlaces::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}

/****************************************************************************/
