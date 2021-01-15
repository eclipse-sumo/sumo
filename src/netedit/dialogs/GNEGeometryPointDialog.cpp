/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEGeometryPointDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2021
///
// A dialog for set Geometry Points
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEGeometryPointDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEGeometryPointDialog) GNEGeometryPointDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_SET_ATTRIBUTE,  GNEGeometryPointDialog::onCmdChangeGeometryPoint),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_ACCEPT,  GNEGeometryPointDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_CANCEL,  GNEGeometryPointDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_RESET,   GNEGeometryPointDialog::onCmdReset),
    FXMAPFUNC(SEL_CLOSE,        0,                      GNEGeometryPointDialog::onCmdCancel),
};

// Object abstract implementation
FXIMPLEMENT_ABSTRACT(GNEGeometryPointDialog, FXTopWindow, GNEGeometryPointDialogMap, ARRAYNUMBER(GNEGeometryPointDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEGeometryPointDialog::GNEGeometryPointDialog(GNEViewNet* viewNet, Position* pos) :
    FXTopWindow(viewNet, "Geom. Point", GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), GUIDesignDialogBoxExplicit(100, 80)),
    myViewNet(viewNet),
    myPos(pos),
    myOriginalPos(*pos) {
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create frame for X,Y
    FXHorizontalFrame* XYFrame = new FXHorizontalFrame(mainFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(XYFrame, "X,Y", nullptr, GUIDesignLabelThick50);
    myTextFieldXY = new FXTextField(XYFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create frame for lon,lat
    FXHorizontalFrame* lonLatFrame = new FXHorizontalFrame(mainFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(lonLatFrame, "lon,lat", nullptr, GUIDesignLabelThick50);
    myTextFieldLonLat = new FXTextField(lonLatFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // Create frame for contents
    // myContentFrame = new FXVerticalFrame(mainFrame, GUIDesignContentsFrame);
    // create buttons centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, "\t\tclose accepting changes",  GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonCustomWidth(35));
    myCancelButton = new FXButton(buttonsFrame, "\t\tclose discarding changes", GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCustomWidth(35));
    myResetButton = new FXButton(buttonsFrame,  "\t\treset to previous values",  GUIIconSubSys::getIcon(GUIIcon::RESET),  this, MID_GNE_BUTTON_RESET, GUIDesignButtonCustomWidth(35));
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // create
    create();
    // show in the given position
    show(PLACEMENT_SCREEN);
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(this);
}


GNEGeometryPointDialog::~GNEGeometryPointDialog() {
    // return focus to GNEViewNet to avoid minimization
    getParent()->setFocus();
}


long 
GNEGeometryPointDialog::onCmdChangeGeometryPoint(FXObject* sender, FXSelector sel, void* ptr) {
    //
    return 1;
}


long 
GNEGeometryPointDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // stop modal
    getApp()->stopModal(this);
    return 1;
}


long 
GNEGeometryPointDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // set original positions
    myPos->setx(myOriginalPos.x());
    myPos->sety(myOriginalPos.y());
    // stop modal
    getApp()->stopModal(this);
    return 1;
}


long
GNEGeometryPointDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // set original positions
    myPos->setx(myOriginalPos.x());
    myPos->sety(myOriginalPos.y());

    return 1;
}

/****************************************************************************/
