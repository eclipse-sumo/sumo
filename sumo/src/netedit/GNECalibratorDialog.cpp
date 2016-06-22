/****************************************************************************/
/// @file    GNECalibratorDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
/// Dialog for edit rerouters
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
#include "GNECalibratorDialog.h"
#include "GNECalibrator.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECalibratorDialog) GNECalibratorDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT, GNECalibratorDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_CANCEL, GNECalibratorDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_RESET,  GNECalibratorDialog::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNECalibratorDialog, FXDialogBox, GNECalibratorDialogMap, ARRAYNUMBER(GNECalibratorDialogMap))

// ===========================================================================
// static member definitions
// ===========================================================================


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorDialog::GNECalibratorDialog(GNECalibrator *calibratorParent) : 
    GNEAdditionalDialog(calibratorParent, 320, 240),
    myRerouterParent(calibratorParent) {
    // Execute additional dialog (To make it modal)
    execute(); 
}


GNECalibratorDialog::~GNECalibratorDialog() {
}


long 
GNECalibratorDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this,TRUE);
    return 1;
}


long 
GNECalibratorDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this,TRUE);
    return 1;
}


long
GNECalibratorDialog::onCmdReset(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
