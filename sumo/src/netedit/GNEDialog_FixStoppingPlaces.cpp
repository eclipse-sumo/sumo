/****************************************************************************/
/// @file    GNEDialog_FixStoppingPlaces.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
/// @version $Id: GNEDialog_FixStoppingPlaces.cpp 24108 2017-04-27 18:43:30Z behrisch $
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
#include "GNEDialog_FixStoppingPlaces.h"
#include "GNEAdditional.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDialog_FixStoppingPlaces) GNEDialog_FixStoppingPlacesMap[] = {
  /*FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_CHANGE,               GNEDialog_FixStoppingPlaces::onCmdValueChanged),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_SELECTALL,            GNEDialog_FixStoppingPlaces::onCmdSelectAll),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_UNSELECTALL,          GNEDialog_FixStoppingPlaces::onCmdUnselectAll),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_SELECTONLYNONROAD,    GNEDialog_FixStoppingPlaces::onCmdSelectOnlyNonRoad),
    */
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,       GNEDialog_FixStoppingPlaces::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,       GNEDialog_FixStoppingPlaces::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_RESET,        GNEDialog_FixStoppingPlaces::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNEDialog_FixStoppingPlaces, FXDialogBox, GNEDialog_FixStoppingPlacesMap, ARRAYNUMBER(GNEDialog_FixStoppingPlacesMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDialog_FixStoppingPlaces::GNEDialog_FixStoppingPlaces(FXApp* app, std::vector<GNEAdditional*> invalidStoppingPlacesAndE2) :
    FXDialogBox(app, ("Fix stoppingPlaces positions"), GUIDesignDialogBox),
    myInvalidStoppingPlacesAndE2(invalidStoppingPlacesAndE2) {
    // set vehicle icon for this dialog
    this->setIcon(GUIIconSubSys::getIcon(ICON_GREENVEHICLE));
    // create dialog buttons bot centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, "accept\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), this, MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, "cancel\t\tclose", GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_GNE_MODE_ADDITIONALDIALOG_CANCEL, GUIDesignButtonCancel);
    myResetButton = new FXButton(buttonsFrame,  "reset\t\tclose",  GUIIconSubSys::getIcon(ICON_RESET), this, MID_GNE_MODE_ADDITIONALDIALOG_RESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // reset dialog
    onCmdReset(0, 0, 0);
}


GNEDialog_FixStoppingPlaces::~GNEDialog_FixStoppingPlaces() {
}


long
GNEDialog_FixStoppingPlaces::onCmdAccept(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNEDialog_FixStoppingPlaces::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNEDialog_FixStoppingPlaces::onCmdReset(FXObject*, FXSelector, void*) {

    return 1;
}


/****************************************************************************/
