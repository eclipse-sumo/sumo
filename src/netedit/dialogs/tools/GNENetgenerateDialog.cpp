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
/// @file    GNENetgenerateDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Dialog for netgenerate
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNENetgenerateDialog.h"

#define MARGING 4
#define MAXNUMCOLUMNS 4
#define NUMROWSBYCOLUMN 20

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNENetgenerateDialog) GNENetgenerateDialogMap[] = {
    FXMAPFUNC(SEL_CLOSE,    0,                      GNENetgenerateDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RUN,     GNENetgenerateDialog::onCmdRun),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,  GNENetgenerateDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RESET,   GNENetgenerateDialog::onCmdReset)
};

// Object implementation
FXIMPLEMENT(GNENetgenerateDialog, FXDialogBox, GNENetgenerateDialogMap, ARRAYNUMBER(GNENetgenerateDialogMap))

// ============================================-===============================
// member method definitions
// ===========================================================================

GNENetgenerateDialog::GNENetgenerateDialog(GNEApplicationWindow* GNEApp) :
    FXDialogBox(GNEApp->getApp(), "Netgenerate", GUIDesignDialogBoxExplicit(200, 200)),
    myGNEApp(GNEApp) {
    // set icon
    setIcon(GUIIconSubSys::getIcon(GUIIcon::NETGENERATE));
/*
    // build row frames
    auto horizontalFrameRows = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // add header
    auto horizontalFrameLabel = new FXHorizontalFrame(getRowFrame(), GUIDesignAuxiliarHorizontalFrame);
    myParameterLabel = new FXLabel(horizontalFrameLabel, TL("Parameter"), nullptr, GUIDesignLabelThickedFixed(0));
    new FXLabel(horizontalFrameLabel, TL("Value"), nullptr, GUIDesignLabelThickedFixed(250));
    // add separator
    mySeparator = new FXSeparator(this);
    // create buttons centered
    myButtonsFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(myButtonsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myButtonsFrame, (TL("Run") + std::string("\t\t") + TL("close accepting changes")).c_str(),
        GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_RUN, GUIDesignButtonAccept);
    new FXButton(myButtonsFrame, (TL("Cancel") + std::string("\t\t") + TL("close discarding changes")).c_str(),
        GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    new FXButton(myButtonsFrame, (TL("Reset") + std::string("\t\t") + TL("reset to previous values")).c_str(), 
        GUIIconSubSys::getIcon(GUIIcon::RESET),  this, MID_GNE_BUTTON_RESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(myButtonsFrame, GUIDesignAuxiliarHorizontalFrame);
*/
}


GNENetgenerateDialog::~GNENetgenerateDialog() {}


long
GNENetgenerateDialog::openDialog(const OptionsCont *netgenerateOptions) {
    // show dialog
    FXDialogBox::show(PLACEMENT_SCREEN);
    // refresh APP
    getApp()->refresh();
    // resize dialog (Marging + contentFrame + MARGING separator + MARGING + buttonsFrame + MARGING)
    int rowFramesWidth = 0;
    int rowFramesHeight = 0;
    // resize dialog (rowFramesWidth, Marging + rowFramesHeight + MARGING separator + MARGING + buttonsFrame + MARGING)
    //resize(rowFramesWidth, rowFramesHeight + mySeparator->getHeight() + myButtonsFrame->getHeight() + (4 * MARGING));
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    return myGNEApp->getApp()->runModalFor(this);
}


long
GNENetgenerateDialog::onCmdRun(FXObject*, FXSelector, void*) {
    // stop modal
    myGNEApp->getApp()->stopModal(this);
    // hide dialog
    hide();
    // run tool
    //return myGNEApp->tryHandle(myNetgenerate->getMenuCommand(), FXSEL(SEL_COMMAND, MID_GNE_RUNPYTHONTOOL), nullptr);
    return 1;
}


long
GNENetgenerateDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // stop modal
    myGNEApp->getApp()->stopModal(this);
    // hide dialog
    hide();
    return 1;
}


long
GNENetgenerateDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // iterate over all arguments and reset values
    return 1;
}


GNENetgenerateDialog::GNENetgenerateDialog() :
    myGNEApp(nullptr) {
}

/****************************************************************************/
