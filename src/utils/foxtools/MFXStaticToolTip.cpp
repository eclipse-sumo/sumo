/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXStaticToolTip.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2022
///
//
/****************************************************************************/

/* =========================================================================
* included modules
* ======================================================================= */
#include <config.h>

#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "MFXStaticToolTip.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXStaticToolTip) MFXStaticToolTipMap[] = {
    FXMAPFUNC(SEL_PAINT,    0,  MFXStaticToolTip::onPaint),
    FXMAPFUNC(SEL_UPDATE,   0,  MFXStaticToolTip::onUpdate),
};

// Object implementation
FXIMPLEMENT(MFXStaticToolTip, FXToolTip, MFXStaticToolTipMap, ARRAYNUMBER(MFXStaticToolTipMap))

// ===========================================================================
// method definitions
// ===========================================================================

MFXStaticToolTip::MFXStaticToolTip(FXApp* app) :
    FXToolTip(app) {
    // set empty test
    setText("");
    // start hide
    hide();
}


MFXStaticToolTip::~MFXStaticToolTip() {}


void
MFXStaticToolTip::enableStaticToolTip(const bool value) {
    if (value) {
        myEnableStaticTooltip = true;
    } else {
        myEnableStaticTooltip = false;
        hideStaticToolTip();
    }
}


bool
MFXStaticToolTip::isStaticToolTipEnabled() const {
    return myEnableStaticTooltip;
}


void
MFXStaticToolTip::showStaticToolTip(const FXString& toolTipText) {
    if (!myEnableStaticTooltip || toolTipText.empty()) {
        hideStaticToolTip();
    } else {
        // set tip text
        setText(toolTipText);
        // update before show (for position)
        onUpdate(nullptr, 0, nullptr);
        // show StaticToolTip
        show();
    }
}


void
MFXStaticToolTip::hideStaticToolTip() {
    // clear text
    setText("");
    // hide staticTooltip
    hide();
}


long
MFXStaticToolTip::onPaint(FXObject* sender, FXSelector sel, void* obj) {
    // draw tooltip using myToolTippedObject
    if (!label.empty() && myEnableStaticTooltip) {
        return FXToolTip::onPaint(sender, sel, obj);
    } else {
        return 0;
    }
}


long
MFXStaticToolTip::onUpdate(FXObject* sender, FXSelector sel, void* ptr) {
    // Regular GUI update
    FXWindow::onUpdate(sender, sel, ptr);
    // Ask the help source for a new status text first
    if (label.empty()) {
        popped = FALSE;
        hide();
    } else {
        popped = TRUE;
        FXint x, y;
        FXuint state;
        getRoot()->getCursorPosition(x, y, state);
        place(x, y);
    }
    return 1;
}


MFXStaticToolTip::MFXStaticToolTip() :
    FXToolTip() {
}
