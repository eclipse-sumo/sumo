/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2022 German Aerospace Center (DLR) and others.
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


#define HSPACE  4
#define VSPACE  2

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

MFXStaticToolTip::MFXStaticToolTip(FXMainWindow* mainWindow) :
    FXToolTip(mainWindow->getApp()),
    myMainWindow(mainWindow) {
    // start hide
    hide();
}


MFXStaticToolTip::~MFXStaticToolTip() {}


void 
MFXStaticToolTip::showStaticToolTip(FXWindow* toolTipObject) {
    if (!label.empty()) {
        // update toolTip object
        myToolTipObject = toolTipObject;
        // show StaticToolTip
        show();
    }
}


void 
MFXStaticToolTip::hideStaticToolTip() {
    // clear toolTip object
    myToolTipObject = nullptr;
    // hide staticTooltip
    hide();
}


void 
MFXStaticToolTip::setText(const FXString& text) {
    FXToolTip::setText(text);
}


long
MFXStaticToolTip::onPaint(FXObject* sender, FXSelector sel, void* obj) {
    // draw tooltip using myToolTippedObject
    if (myToolTipObject) {
        return FXToolTip::onPaint(sender, sel, obj);
    } else {
        return 0;
    }
}


long 
MFXStaticToolTip::onUpdate(FXObject* sender, FXSelector sel, void* ptr) {
    FXWindow *helpsource=getApp()->getCursorWindow();
    // Regular GUI update
    FXWindow::onUpdate(sender, sel, ptr);
    // Ask the help source for a new status text first
    if (myToolTipObject && helpsource && helpsource->handle(this, FXSEL(SEL_QUERY_TIP, 0), NULL)) {
        if(!popped){
            popped = TRUE;
            FXint x, y; 
            FXuint state;
            myMainWindow->getCursorPosition(x,y,state);
            place(x, y);
        }
    } else {
        popped = FALSE;
        hide();
    }
    return 1;
}


MFXStaticToolTip::MFXStaticToolTip() :
    FXToolTip(),
    myMainWindow(nullptr) {
}
