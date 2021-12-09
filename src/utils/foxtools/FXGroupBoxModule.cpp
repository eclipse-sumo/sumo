/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2021 German Aerospace Center (DLR) and others.
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
/// @file    FXGroupBoxModul.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2021
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

#include "FXGroupBoxModule.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(FXGroupBoxModul) FXGroupBoxModulMap[] = {
    FXMAPFUNC(SEL_PAINT,  0,  FXGroupBoxModul::onPaint),
    FXMAPFUNC(SEL_COMMAND,  MID_SWITCH,  FXGroupBoxModul::onCollapseButton),
};

// Object implementation
FXIMPLEMENT(FXGroupBoxModul, FXVerticalFrame, FXGroupBoxModulMap, ARRAYNUMBER(FXGroupBoxModulMap))

// ===========================================================================
// method definitions
// ===========================================================================

FXGroupBoxModul::FXGroupBoxModul(FXVerticalFrame* contentFrame, const std::string &text, const bool collapsible) :
    FXVerticalFrame(contentFrame, GUIDesignHorizontalFrame),
    myCollapsed(false) {
    // build collapse button or label
    if (collapsible) {
        myCollapseButton = new FXButton(this, text.c_str(), GUIIconSubSys::getIcon(GUIIcon::OK), this, MID_SWITCH, GUIDesignButtonFXGroupBoxModul);
    } else {
        myLabel = new FXLabel(this, text.c_str(), nullptr, GUIDesignLabelFXGroupBoxModul);
    }
    // build collapsable frame
    myCollapsableFrame = new FXVerticalFrame(this, GUIDesignCollapsableFrame);
}


FXGroupBoxModul::~FXGroupBoxModul() {}


void 
FXGroupBoxModul::setText(const std::string& text) {
    if (myCollapseButton) {
        myCollapseButton->setText(text.c_str());
    } else {
        myLabel->setText(text.c_str());
    }
}


FXVerticalFrame* 
FXGroupBoxModul::getCollapsableFrame() {
    return myCollapsableFrame;
}


long
FXGroupBoxModul::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent *event = (FXEvent*)ptr;
    FXDCWindow dc(this, event);
    // Paint background
    dc.setForeground(backColor);
    dc.fillRectangle(event->rect.x, event->rect.y, event->rect.w, event->rect.h);
    // draw groove rectangle
    drawGrooveRectangle(dc, 0, 15, width, height - 15);
    return 1;
}


long 
FXGroupBoxModul::onCollapseButton(FXObject*,FXSelector,void*) {
    if (myCollapsed) {
        myCollapsed = false;
        myCollapseButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::OK));
        myCollapsableFrame->show();
    } else {
        myCollapsed = true;
        myCollapseButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::CANCEL));
        myCollapsableFrame->hide();
    }
    recalc();
    return 1;
}


FXGroupBoxModul::FXGroupBoxModul() :
    myCollapsed(false) {}