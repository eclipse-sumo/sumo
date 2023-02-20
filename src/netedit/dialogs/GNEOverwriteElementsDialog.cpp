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
/// @file    GNEOverwriteElementsDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
///
// Dialog used to ask user if overwrite elements during loading
/****************************************************************************/

#include <utils/common/MsgHandler.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/GNEApplicationWindow.h>

#include "GNEOverwriteElementsDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEOverwriteElementsDialog) GNEOverwriteElementsDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,  GNEOverwriteElementsDialog::onCmdSelectOption),
    FXMAPFUNC(SEL_CLOSE,    0,               GNEOverwriteElementsDialog::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GNEOverwriteElementsDialog, FXDialogBox, GNEOverwriteElementsDialogMap, ARRAYNUMBER(GNEOverwriteElementsDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEOverwriteElementsDialog - methods
// ---------------------------------------------------------------------------

GNEOverwriteElementsDialog::GNEOverwriteElementsDialog(GNEApplicationWindow* applicationWindow, const std::string elementType) :
    FXDialogBox(applicationWindow->getApp(), ("Overwrite " + elementType + " elements").c_str(), GUIDesignDialogBoxExplicit(310, 90)) {
    // set busStop icon for this dialog
    setIcon(GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create label
    new FXLabel(mainFrame, ("Selected " + elementType + " file was already loaded.\n Continue or overwrite elements?").c_str(), nullptr, GUIDesignLabelOverwrite);
    // create buttons centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, (TL("accept") + std::string("\t\t") + TL("load elements")).c_str(),  GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_SELECT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, (TL("cancel") + std::string("\t\t") + TL("cancel loading of elements")).c_str(), GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_SELECT, GUIDesignButtonCancel);
    myOverwriteButton = new FXButton(buttonsFrame, (TL("overwrite") + std::string("\t\t") + TL("overwrite elements")).c_str(),  GUIIconSubSys::getIcon(GUIIcon::RESET),  this, MID_GNE_SELECT, GUIDesignButtonOverwrite);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // create Dialog
    create();
    // show in the given position
    show(PLACEMENT_SCREEN);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    applicationWindow->getApp()->runModalFor(this);
}


GNEOverwriteElementsDialog::~GNEOverwriteElementsDialog() {
}


GNEOverwriteElementsDialog::Result
GNEOverwriteElementsDialog::getResult() const {
    return myResult;
}


long
GNEOverwriteElementsDialog::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if (obj == myAcceptButton) {
        myResult = Result::ACCEPT;
    } else if (obj == myOverwriteButton) {
        myResult = Result::OVERWRITE;
    }
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}

long
GNEOverwriteElementsDialog::onCmdClose(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}

/****************************************************************************/
