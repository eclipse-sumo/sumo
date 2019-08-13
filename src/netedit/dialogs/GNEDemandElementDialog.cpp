/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDemandElementDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2018
/// @version $Id$
///
// A abstract class for editing additional elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEDemandElementDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDemandElementDialog) GNEDemandElementDialogMap[] = {
    FXMAPFUNC(SEL_KEYPRESS,     0,                                      GNEDemandElementDialog::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,   0,                                      GNEDemandElementDialog::onKeyRelease),
    FXMAPFUNC(SEL_CLOSE,        0,                                      GNEDemandElementDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT,  GNEDemandElementDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL,  GNEDemandElementDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_ADDITIONALDIALOG_BUTTONRESET,   GNEDemandElementDialog::onCmdReset),
};

// Object abstract implementation
FXIMPLEMENT_ABSTRACT(GNEDemandElementDialog, FXTopWindow, GNEDemandElementDialogMap, ARRAYNUMBER(GNEDemandElementDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDemandElementDialog::GNEDemandElementDialog(GNEDemandElement* editedDemandElement, bool updatingElement, int width, int height) :
    FXTopWindow(editedDemandElement->getViewNet(), ("Edit '" + editedDemandElement->getID() + "' data").c_str(), editedDemandElement->getIcon(), editedDemandElement->getIcon(), GUIDesignDialogBoxExplicit(width, height)),
    myEditedDemandElement(editedDemandElement),
    myUpdatingElement(updatingElement),
    myChangesDescription("change " + editedDemandElement->getTagStr() + " values"),
    myNumberOfChanges(0) {
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // Create frame for contents
    myContentFrame = new FXVerticalFrame(mainFrame, GUIDesignContentsFrame);
    // create buttons centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, "accept\t\tclose accepting changes",  GUIIconSubSys::getIcon(ICON_ACCEPT), this, MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, "cancel\t\tclose discarding changes", GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL, GUIDesignButtonCancel);
    myResetButton = new FXButton(buttonsFrame,  "reset\t\treset to previous values",  GUIIconSubSys::getIcon(ICON_RESET),  this, MID_GNE_ADDITIONALDIALOG_BUTTONRESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
}


GNEDemandElementDialog::~GNEDemandElementDialog() {
    // return focus to GNEViewNet to avoid minimization
    getParent()->setFocus();
}


FXint
GNEDemandElementDialog::openAsModalDialog(FXuint placement) {
    // create Dialog
    create();
    // show in the given position
    show(placement);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    return getApp()->runModalFor(this);
}


GNEDemandElement*
GNEDemandElementDialog::getEditedDemandElement() const {
    return myEditedDemandElement;
}


long
GNEDemandElementDialog::onKeyPress(FXObject* sender, FXSelector sel, void* ptr) {
    return FXTopWindow::onKeyPress(sender, sel, ptr);
}


long
GNEDemandElementDialog::onKeyRelease(FXObject* sender, FXSelector sel, void* ptr) {
    return FXTopWindow::onKeyRelease(sender, sel, ptr);
}


void
GNEDemandElementDialog::changeDemandElementDialogHeader(const std::string& newHeader) {
    // change FXDialogBox title
    setTitle(newHeader.c_str());
}


void
GNEDemandElementDialog::initChanges() {
    // init commandGroup
    myEditedDemandElement->getViewNet()->getUndoList()->p_begin(myChangesDescription);
    // save number of command group changes
    myNumberOfChanges = myEditedDemandElement->getViewNet()->getUndoList()->currentCommandGroupSize();
}


void
GNEDemandElementDialog::acceptChanges() {
    // commit changes or abort last command group depending of number of changes did
    if (myNumberOfChanges < myEditedDemandElement->getViewNet()->getUndoList()->currentCommandGroupSize()) {
        myEditedDemandElement->getViewNet()->getUndoList()->p_end();
    } else {
        myEditedDemandElement->getViewNet()->getUndoList()->p_abortLastCommandGroup();
    }
}


void
GNEDemandElementDialog::cancelChanges() {
    myEditedDemandElement->getViewNet()->getUndoList()->p_abortLastCommandGroup();
}


void
GNEDemandElementDialog::resetChanges() {
    // abort last command group an start editing again
    myEditedDemandElement->getViewNet()->getUndoList()->p_abortLastCommandGroup();
    myEditedDemandElement->getViewNet()->getUndoList()->p_begin(myChangesDescription);
}

/****************************************************************************/
