/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDialog_FixAdditionalPositions.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
/// @version $Id$
///
// Dialog used to fix invalid stopping places
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

#include "GNEDialog_FixAdditionalPositions.h"
#include "GNEAdditional.h"
#include "GNEStoppingPlace.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNEChange_Selection.h"
#include "GNEChange_Attribute.h"
#include "GNEAdditionalHandler.h"
#include "GNEDetectorE2.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDialog_FixAdditionalPositions) GNEDialog_FixAdditionalPositionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,                  GNEDialog_FixAdditionalPositions::onCmdSelectOption),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT,  GNEDialog_FixAdditionalPositions::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL,  GNEDialog_FixAdditionalPositions::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GNEDialog_FixAdditionalPositions, FXDialogBox, GNEDialog_FixAdditionalPositionsMap, ARRAYNUMBER(GNEDialog_FixAdditionalPositionsMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDialog_FixAdditionalPositions::GNEDialog_FixAdditionalPositions(GNEViewNet* viewNet, const std::vector<GNEStoppingPlace*>& invalidStoppingPlaces, const std::vector<GNEDetector*>& invalidDetectors) :
    FXDialogBox(viewNet->getApp(), ("Fix stoppingPlaces positions"), GUIDesignDialogBoxExplicit, 0, 0, 458, 300, 0, 0, 0, 0),
    myViewNet(viewNet),
    myInvalidStoppingPlaces(invalidStoppingPlaces),
    myInvalidDetectors(invalidDetectors) {
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
    myTable->setTableSize(int(myInvalidStoppingPlaces.size() + myInvalidDetectors.size()), 3);
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
    // iterate over stopping places
    for (auto i : myInvalidStoppingPlaces) {
        // Set icon
        item = new FXTableItem("", i->getIcon());
        item->setIconPosition(FXTableItem::CENTER_X);
        myTable->setItem(indexRow, 0, item);
        // Set ID
        item = new FXTableItem(i->getID().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 1, item);
        // Set conflict
        std::string errorStartPosition, errorEndPosition, separator;
        // check start position
        if (i->getAbsoluteStartPosition() < 0) {
            errorStartPosition = (toString(SUMO_ATTR_STARTPOS) + " < 0");
        }
        // check end position
        if (i->getAbsoluteEndPosition() > i->getLane()->getLaneParametricLength()) {
            errorEndPosition = (toString(SUMO_ATTR_ENDPOS) + " > lanes's length");
        }
        // check separator
        if ((errorStartPosition.size() > 0) && (errorEndPosition.size() > 0)) {
            separator = " and ";
        }
        item = new FXTableItem((errorStartPosition + separator + errorEndPosition).c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 2, item);
        // Update index
        indexRow++;
    }
    // iterate over detectors
    for (auto i : myInvalidDetectors) {
        // Set icon
        item = new FXTableItem("", i->getIcon());
        item->setIconPosition(FXTableItem::CENTER_X);
        myTable->setItem(indexRow, 0, item);
        // Set ID
        item = new FXTableItem(i->getID().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 1, item);
        // Set conflict
        std::string errorPosition, errorLenght, separator;
        // check position over lane
        if (i->getAbsolutePositionOverLane() < 0) {
            errorPosition = (toString(SUMO_ATTR_POSITION) + " < 0");
        } else if (i->getAbsolutePositionOverLane() > i->getLane()->getLaneParametricLength()) {
            errorPosition = (toString(SUMO_ATTR_POSITION) + " > lanes's length");
        }
        GNEDetectorE2* E2Detector = dynamic_cast<GNEDetectorE2*>(i);
        if ((E2Detector != NULL) && ((E2Detector->getAbsolutePositionOverLane() + E2Detector->getAbsoluteLenght()) > i->getLane()->getLaneParametricLength())) {
            errorLenght = (toString(SUMO_ATTR_LENGTH) + " > lanes's length");
        }

        // check separator
        if ((errorPosition.size() > 0) && (errorLenght.size() > 0)) {
            separator = " and ";
        }
        item = new FXTableItem((errorPosition + separator + errorLenght).c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 2, item);
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
    myAcceptButton = new FXButton(buttonsFrame, FXWindow::tr("&Accept"), GUIIconSubSys::getIcon(ICON_ACCEPT), this, MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, FXWindow::tr("&Cancel"), GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL, GUIDesignButtonCancel);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // set focus in accept button
    myAcceptButton->setFocus();
}


GNEDialog_FixAdditionalPositions::~GNEDialog_FixAdditionalPositions() {
}


long
GNEDialog_FixAdditionalPositions::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if (obj == myOptionA) {
        myOptionA->setCheck(true);
        myOptionB->setCheck(false);
        myOptionC->setCheck(false);
        myOptionD->setCheck(false);
        return 1;
    } else if (obj == myOptionB) {
        myOptionA->setCheck(false);
        myOptionB->setCheck(true);
        myOptionC->setCheck(false);
        myOptionD->setCheck(false);
        return 1;
    } else if (obj == myOptionC) {
        myOptionA->setCheck(false);
        myOptionB->setCheck(false);
        myOptionC->setCheck(true);
        myOptionD->setCheck(false);
        return 1;
    } else if (obj == myOptionD) {
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
GNEDialog_FixAdditionalPositions::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myOptionA->getCheck() == TRUE) {
        myViewNet->getUndoList()->p_begin(toString(SUMO_ATTR_FRIENDLY_POS) + " of invalid additionals");
        // iterate over invalid stopping places to enable friendly position
        for (auto i = myInvalidStoppingPlaces.begin(); i != myInvalidStoppingPlaces.end(); i++) {
            (*i)->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", myViewNet->getUndoList());
        }
        // iterate over invalid detectors to enable friendly position
        for (auto i = myInvalidDetectors.begin(); i != myInvalidDetectors.end(); i++) {
            (*i)->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", myViewNet->getUndoList());
        }
        myViewNet->getUndoList()->p_end();
        // stop modal with TRUE (continue saving)
        getApp()->stopModal(this, TRUE);
        return 1;
    } else if (myOptionB->getCheck() == TRUE) {
        myViewNet->getUndoList()->p_begin("Fixed positions of invalid additionals");
        // iterate over invalid stopping places to fix positions
        for (auto i = myInvalidStoppingPlaces.begin(); i != myInvalidStoppingPlaces.end(); i++) {
            GNEStoppingPlace* stoppingPlace = dynamic_cast<GNEStoppingPlace*>(*i);
            if (stoppingPlace != NULL) {
                // obtain start and end position
                double startPos = GNEAttributeCarrier::parse<double>(stoppingPlace->getAttribute(SUMO_ATTR_STARTPOS));
                double endPos = GNEAttributeCarrier::parse<double>(stoppingPlace->getAttribute(SUMO_ATTR_ENDPOS));
                // fix start and end positions using fixStoppinPlacePosition
                GNEAdditionalHandler::fixStoppinPlacePosition(startPos, endPos, stoppingPlace->getLane()->getLaneParametricLength(), POSITION_EPS, true);
                // set new start and end positions
                stoppingPlace->setAttribute(SUMO_ATTR_STARTPOS, toString(startPos), myViewNet->getUndoList());
                stoppingPlace->setAttribute(SUMO_ATTR_ENDPOS, toString(endPos), myViewNet->getUndoList());
            }
        }
        // iterate over invalid detectors
        for (auto i = myInvalidDetectors.begin(); i != myInvalidDetectors.end(); i++) {
            GNEDetectorE2* E2Detector = dynamic_cast<GNEDetectorE2*>(*i);
            // Check if we're handling a E2 detector o a E1/Entry/Exit
            if (E2Detector != NULL) {
                // obtain position and lenght
                double pos = GNEAttributeCarrier::parse<double>(E2Detector->getAttribute(SUMO_ATTR_POSITION));
                double length = GNEAttributeCarrier::parse<double>(E2Detector->getAttribute(SUMO_ATTR_LENGTH));
                // fix pos and lenght using fixE2DetectorPositionPosition
                GNEAdditionalHandler::fixE2DetectorPositionPosition(pos, length, E2Detector->getLane()->getLaneParametricLength(), true);
                // set new position and length
                E2Detector->setAttribute(SUMO_ATTR_POSITION, toString(pos), myViewNet->getUndoList());
                E2Detector->setAttribute(SUMO_ATTR_LENGTH, toString(length), myViewNet->getUndoList());
            } else {
                // obtain position
                double pos = GNEAttributeCarrier::parse<double>(E2Detector->getAttribute(SUMO_ATTR_POSITION));
                // fix pos and lenght  checkAndFixDetectorPositionPosition
                GNEAdditionalHandler::checkAndFixDetectorPositionPosition(pos, E2Detector->getLane()->getLaneParametricLength(), true);
                // set new position
                E2Detector->setAttribute(SUMO_ATTR_POSITION, toString(pos), myViewNet->getUndoList());
            }
        }
        myViewNet->getUndoList()->p_end();
        // stop modal with TRUE (continue saving)
        getApp()->stopModal(this, TRUE);
        return 1;
    } else if (myOptionC->getCheck() == TRUE) {
        // simply stop modal with TRUE to save additionals with invalid positions
        getApp()->stopModal(this, TRUE);
        return 1;
    } else if (myOptionD->getCheck() == TRUE) {
        std::set<GUIGlID> GLIDsToSelect;
        myViewNet->getUndoList()->p_begin("select invalid additionals");
        // clear previous selection
        myViewNet->getUndoList()->add(new GNEChange_Selection(myViewNet->getNet(), std::set<GUIGlID>(), gSelected.getSelected(), true), true);
        // iterate over invalid stopping places to select it
        for (auto i = myInvalidStoppingPlaces.begin(); i != myInvalidStoppingPlaces.end(); i++) {
            GLIDsToSelect.insert((*i)->getGlID());
        }
        // iterate over invalid detectors to enable to select it
        for (auto i = myInvalidDetectors.begin(); i != myInvalidDetectors.end(); i++) {
            GLIDsToSelect.insert((*i)->getGlID());
        }
        myViewNet->getUndoList()->add(new GNEChange_Selection(myViewNet->getNet(), GLIDsToSelect, std::set<GUIGlID>(), true), true);
        myViewNet->getUndoList()->p_end();
        myViewNet->update();
        // stop modal with FALSE (abort saving)
        getApp()->stopModal(this, FALSE);
        return 0;
    } else {
        // stop modal with FALSE (abort saving)
        getApp()->stopModal(this, FALSE);
        return 0;
    }
}


long
GNEDialog_FixAdditionalPositions::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal (abort saving)
    getApp()->stopModal(this, FALSE);
    return 1;
}

/****************************************************************************/
