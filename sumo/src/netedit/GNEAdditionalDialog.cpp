/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNEAdditionalDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
// A abstract class for editing additional elements
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
#include <utils/gui/images/GUIIconSubSys.h>

#include "GNEAdditionalDialog.h"
#include "GNEAdditional.h"
#include "GNEViewNet.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAdditionalDialog) GNEAdditionalDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT, GNEAdditionalDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL, GNEAdditionalDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ADDITIONALDIALOG_BUTTONRESET,  GNEAdditionalDialog::onCmdReset),
};

// Object abstract implementation
FXIMPLEMENT_ABSTRACT(GNEAdditionalDialog, FXDialogBox, GNEAdditionalDialogMap, ARRAYNUMBER(GNEAdditionalDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalDialog::GNEAdditionalDialog(GNEAdditional* parent, int width, int height) :
    FXDialogBox(parent->getViewNet(), ("Edit '" + parent->getID() + "' data").c_str(), GUIDesignDialogBoxExplicit, 0, 0, width, height, 0, 0, 0, 0) {
    // check that parent isn't NULL
    assert(parent != NULL);
    // set icon
    setIcon(parent->getIcon());
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // Create frame for contents
    myContentFrame = new FXVerticalFrame(mainFrame, GUIDesignContentsFrame);
    // create buttons centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, "accept\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), this, MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, "cancel\t\tclose", GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL, GUIDesignButtonCancel);
    myResetButton = new FXButton(buttonsFrame,  "reset\t\tclose",  GUIIconSubSys::getIcon(ICON_RESET), this, MID_GNE_ADDITIONALDIALOG_BUTTONRESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
}


GNEAdditionalDialog::~GNEAdditionalDialog() {}


void
GNEAdditionalDialog::changeAdditionalDialogHeader(const std::string& newHeader) {
    // change FXDialogBox title
    setTitle(newHeader.c_str());
}

/****************************************************************************/
