/****************************************************************************/
/// @file    GNEAdditionalDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
/// A abstract class for editing additional elements
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
#include <utils/gui/div/GUIDesigns.h>

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

GNEAdditionalDialog::GNEAdditionalDialog(GNEAdditional* parent, int width, int height) :
    FXDialogBox(parent->getViewNet(), ("Edit '" + parent->getID() + "' data").c_str(), LAYOUT_EXPLICIT | DECOR_TITLE | DECOR_BORDER, 0, 0, width, height, 0, 0, 0, 0) {
    
    FXVerticalFrame *mainFrame = new FXVerticalFrame(this, LAYOUT_FILL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    // Create frames
    myContentFrame = new FXVerticalFrame(mainFrame, LAYOUT_FILL);
    myButtonFrame = new FXHorizontalFrame(mainFrame, LAYOUT_FILL_X);

    // create buttons centered
    new FXHorizontalFrame(myButtonFrame, LAYOUT_FILL_X);
    myAcceptButton = new FXButton(myButtonFrame, "accept\t\tclose", 0, this, MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(myButtonFrame, "cancel\t\tclose", 0, this, MID_GNE_MODE_ADDITIONALDIALOG_CANCEL, GUIDesignButtonCancel);
    myResetButton = new FXButton(myButtonFrame,  "reset\t\tclose",  0, this, MID_GNE_MODE_ADDITIONALDIALOG_RESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(myButtonFrame, LAYOUT_FILL_X);
}


GNEAdditionalDialog::~GNEAdditionalDialog() {
}


/****************************************************************************/
