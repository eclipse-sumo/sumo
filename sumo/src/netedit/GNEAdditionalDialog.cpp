/****************************************************************************/
/// @file    GNEAdditionalDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
/// A abstract class for editing additional elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
#include "GNEAdditionalDialog.h"
#include "GNEAdditional.h"
#include "GNEViewNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAdditionalDialog) GNEAdditionalDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT, GNEAdditionalDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_CANCEL, GNEAdditionalDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_RESET,  GNEAdditionalDialog::onCmdReset),
};

// Object abstract implementation
FXIMPLEMENT_ABSTRACT(GNEAdditionalDialog, FXDialogBox, GNEAdditionalDialogMap, ARRAYNUMBER(GNEAdditionalDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalDialog::GNEAdditionalDialog(GNEAdditional *parent, int width, int height) : 
    FXDialogBox(parent->getViewNet(), ("Edit '" + parent->getID() + "' data").c_str(), LAYOUT_EXPLICIT | DECOR_TITLE | DECOR_BORDER, 0, 0, width, height) {
    // Create frames
    myContentFrame = new FXVerticalFrame(this, LAYOUT_EXPLICIT, 0, 0, width, height - 30);
    myButtonFrame = new FXHorizontalFrame(this, LAYOUT_EXPLICIT, 0, height - 30, width, 30);
    // create buttons
    new FXHorizontalFrame(myButtonFrame, LAYOUT_FILL_X);
    myAcceptButton = new FXButton(myButtonFrame, "accept\t\tclose", 0, this, MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT, ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED, 0, 0, 75, 23, 2, 2, 2, 2);
    myCancelButton = new FXButton(myButtonFrame, "cancel\t\tclose", 0, this, MID_GNE_MODE_ADDITIONALDIALOG_CANCEL, ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED, 0, 0, 75, 23, 2, 2, 2, 2);
    myResetButton = new FXButton(myButtonFrame,  "reset\t\tclose",  0, this, MID_GNE_MODE_ADDITIONALDIALOG_RESET,  ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED, 0, 0, 75, 23, 2, 2, 2, 2);
    new FXHorizontalFrame(myButtonFrame, LAYOUT_FILL_X);
}


GNEAdditionalDialog::~GNEAdditionalDialog() {
}

void 
GNEAdditionalDialog::show() {
    FXDialogBox::show();
}


void 
GNEAdditionalDialog::hide() {
    FXDialogBox::hide();
}


void 
GNEAdditionalDialog::execute() {
    FXDialogBox::execute();
}

/****************************************************************************/
