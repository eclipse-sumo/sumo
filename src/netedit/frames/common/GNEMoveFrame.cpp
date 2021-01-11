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
/// @file    GNEMoveFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2020
///
// The Widget for move elements
/****************************************************************************/
#include <config.h>

#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEMoveFrame::ChangeJunctionsZ) ChangeJunctionsZMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEMoveFrame::ChangeJunctionsZ::onCmdChangeZValue),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_APPLY,          GNEMoveFrame::ChangeJunctionsZ::onCmdApplyZ),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RESET,          GNEMoveFrame::ChangeJunctionsZ::onCmdResetZ),
};

// Object implementation
FXIMPLEMENT(GNEMoveFrame::ChangeJunctionsZ, FXGroupBox, ChangeJunctionsZMap, ARRAYNUMBER(ChangeJunctionsZMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEMoveFrame::ChangeJunctionsZ - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::ChangeJunctionsZ::ChangeJunctionsZ(GNEMoveFrame* moveFrameParent) :
    FXGroupBox(moveFrameParent->myContentFrame, "Change junctions Z", GUIDesignGroupBoxFrame),
    myMoveFrameParent(moveFrameParent) {
    // create horizontal frame
    FXHorizontalFrame* myZValueFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create elements for Z value
    new FXLabel(myZValueFrame, "Z value", 0, GUIDesignLabelAttribute);
    myZValueTextField = new FXTextField(myZValueFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myZValueTextField->setText("0");
    // create apply button
    new FXButton(moveFrameParent->myContentFrame, 
        "Apply\t\tApply Z value to all selected junctions", 
        GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_APPLY, GUIDesignButtonOK);
    // create reset button
    new FXButton(moveFrameParent->myContentFrame, 
        "Reset\t\tReset Z value in all selected junctions", 
        GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_RESET, GUIDesignButtonOK);
}


GNEMoveFrame::ChangeJunctionsZ::~ChangeJunctionsZ() {}


void 
GNEMoveFrame::ChangeJunctionsZ::showChangeJunctionsZ() {
    // show modul
    show();
}


void
GNEMoveFrame::ChangeJunctionsZ::hideChangeJunctionsZ() {
    // hide modul
    hide();
}


long 
GNEMoveFrame::ChangeJunctionsZ::onCmdChangeZValue(FXObject*, FXSelector, void*) {
    // currently nothing to do
    return 1;
}


long
GNEMoveFrame::ChangeJunctionsZ::onCmdApplyZ(FXObject*, FXSelector, void*) {

    return 1;
}


long
GNEMoveFrame::ChangeJunctionsZ::onCmdResetZ(FXObject*, FXSelector, void*) {

    return 1;
}

// ---------------------------------------------------------------------------
// GNEMoveFrame - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::GNEMoveFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Move"),
    myChangeJunctionsZ(nullptr) {
    // create change junctions z
    myChangeJunctionsZ = new ChangeJunctionsZ(this);
}


GNEMoveFrame::~GNEMoveFrame() {}


void
GNEMoveFrame::processClick(const Position& /*clickedPosition*/,
                           const GNEViewNetHelper::ObjectsUnderCursor& /*objectsUnderCursor*/,
                           const GNEViewNetHelper::ObjectsUnderCursor& /*objectsUnderGrippedCursor*/) {
    // currently unused
}


void
GNEMoveFrame::show() {
    // check if there are junctions selected
    if (myViewNet->getNet()->retrieveJunctions(true).size() > 0) {
        myChangeJunctionsZ->showChangeJunctionsZ();
    } else {
        myChangeJunctionsZ->hideChangeJunctionsZ();
    }
    // update
    recalc();
    // show
    GNEFrame::show();
}


void
GNEMoveFrame::hide() {
    // hide frame
    GNEFrame::hide();
}

/****************************************************************************/
