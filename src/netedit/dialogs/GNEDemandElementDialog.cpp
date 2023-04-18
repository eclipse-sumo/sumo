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
/// @file    GNEDemandElementDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2018
///
// A abstract class for editing additional elements
/****************************************************************************/

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEDemandElementDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDemandElementDialog) GNEDemandElementDialogMap[] = {
    FXMAPFUNC(SEL_KEYPRESS,     0,                      GNEDemandElementDialog::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,   0,                      GNEDemandElementDialog::onKeyRelease),
    FXMAPFUNC(SEL_CLOSE,        0,                      GNEDemandElementDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_ACCEPT,  GNEDemandElementDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_CANCEL,  GNEDemandElementDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_RESET,   GNEDemandElementDialog::onCmdReset),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_FOCUS,   GNEDemandElementDialog::onCmdFocusOnFrame),
};

// Object abstract implementation
FXIMPLEMENT_ABSTRACT(GNEDemandElementDialog, FXTopWindow, GNEDemandElementDialogMap, ARRAYNUMBER(GNEDemandElementDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDemandElementDialog::GNEDemandElementDialog(GNEDemandElement* editedDemandElement, bool updatingElement, int width, int height) :
    FXTopWindow(editedDemandElement->getNet()->getViewNet(), ("Edit '" + editedDemandElement->getID() + "' data").c_str(), editedDemandElement->getACIcon(), editedDemandElement->getACIcon(), GUIDesignDialogBoxExplicit(width, height)),
    myEditedDemandElement(editedDemandElement),
    myUpdatingElement(updatingElement),
    myChangesDescription("Change " + editedDemandElement->getTagStr() + " values"),
    myNumberOfChanges(0) {
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // Create frame for contents
    myContentFrame = new FXVerticalFrame(mainFrame, GUIDesignContentsFrame);
    // create buttons centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, (TL("&Accept") + std::string("\t\t") + TL("close accepting changes")).c_str(),  GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, (TL("&Cancel") + std::string("\t\t") + TL("close discarding changes")).c_str(), GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    myResetButton = new FXButton(buttonsFrame, (TL("&Reset") + std::string("\t\t") + TL("reset to previous values")).c_str(),  GUIIconSubSys::getIcon(GUIIcon::RESET),  this, MID_GNE_BUTTON_RESET,  GUIDesignButtonReset);
    myFocusButton = new FXButton(buttonsFrame,  "&F", nullptr, this, MID_GNE_BUTTON_FOCUS, GUIDesignButtonFocus);

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


long
GNEDemandElementDialog::onCmdFocusOnFrame(FXObject*, FXSelector, void*) {
    setFocus();
    return 1;
}


void
GNEDemandElementDialog::changeDemandElementDialogHeader(const std::string& newHeader) {
    // change FXDialogBox title
    setTitle(newHeader.c_str());
}


void
GNEDemandElementDialog::initChanges() {
    // init commandGroup
    myEditedDemandElement->getNet()->getViewNet()->getUndoList()->begin(myEditedDemandElement->getTagProperty().getGUIIcon(), myChangesDescription);
    // save number of command group changes
    myNumberOfChanges = myEditedDemandElement->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize();
}


void
GNEDemandElementDialog::acceptChanges() {
    // commit changes or abort last command group depending of number of changes did
    if (myNumberOfChanges < myEditedDemandElement->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize()) {
        myEditedDemandElement->getNet()->getViewNet()->getUndoList()->end();
    } else {
        myEditedDemandElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
    }
}


void
GNEDemandElementDialog::cancelChanges() {
    myEditedDemandElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
}


void
GNEDemandElementDialog::resetChanges() {
    // abort last command group an start editing again
    myEditedDemandElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
    myEditedDemandElement->getNet()->getViewNet()->getUndoList()->begin(myEditedDemandElement->getTagProperty().getGUIIcon(), myChangesDescription);
}


/****************************************************************************/
