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


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEMoveFrame::ChangeJunctionsZ - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::ChangeJunctionsZ::ChangeJunctionsZ(GNEMoveFrame* moveFrameParent) :
    FXGroupBox(moveFrameParent->myContentFrame, "Change junctions Z", GUIDesignGroupBoxFrame),
    myMoveFrameParent(moveFrameParent) {

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
    GNEFrame::show();
}


void
GNEMoveFrame::hide() {
    GNEFrame::hide();
}

/****************************************************************************/
