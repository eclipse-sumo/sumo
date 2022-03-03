/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEWireFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2021
///
// The Widget for editing wires
/****************************************************************************/
#include <config.h>

#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNEJunction.h>
#include <netedit/GNEViewNet.h>

#include "GNEWireFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEWireFrame) GNEWireFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,     GNEWireFrame::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_OK,         GNEWireFrame::onCmdOK)
};

// Object implementation
FXIMPLEMENT(GNEWireFrame, FXVerticalFrame, GNEWireFrameMap, ARRAYNUMBER(GNEWireFrameMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEWireFrame - methods
// ---------------------------------------------------------------------------

GNEWireFrame::GNEWireFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Prohibits") {
    // set frame header label
    getFrameHeaderLabel()->setText("Wires");
}


GNEWireFrame::~GNEWireFrame() {}


void
GNEWireFrame::handleWireClick(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {

}


void
GNEWireFrame::show() {
    GNEFrame::show();
}


void
GNEWireFrame::hide() {
    GNEFrame::hide();
}


long
GNEWireFrame::onCmdCancel(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEWireFrame::onCmdOK(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNEWireFrame - private methods
// ---------------------------------------------------------------------------

/****************************************************************************/
