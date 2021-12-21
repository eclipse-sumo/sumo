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
/// @file    FXGroupBoxModule.cpp
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

FXDEFMAP(FXGroupBoxModule) FXGroupBoxModuleMap[] = {
    FXMAPFUNC(SEL_PAINT,  0,  FXGroupBoxModule::onPaint),
    FXMAPFUNC(SEL_COMMAND,  MID_SWITCH,  FXGroupBoxModule::onCollapseButton),
};

// Object implementation
FXIMPLEMENT(FXGroupBoxModule, FXVerticalFrame, FXGroupBoxModuleMap, ARRAYNUMBER(FXGroupBoxModuleMap))

// ===========================================================================
// method definitions
// ===========================================================================

FXGroupBoxModule::FXGroupBoxModule(FXVerticalFrame* contentFrame, const std::string &text, const bool collapsible) :
    FXVerticalFrame(contentFrame, GUIDesignHorizontalFrame),
    myCollapsed(false) {
    // build button and labels
    FXHorizontalFrame *headerFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    if (collapsible) {
        myCollapseButton = new FXButton(headerFrame, "", GUIIconSubSys::getIcon(GUIIcon::COLLAPSE), this, MID_SWITCH, GUIDesignButtonFXGroupBoxModule);
    }
    myLabel = new FXLabel(headerFrame, text.c_str(), nullptr, GUIDesignLabelFXGroupBoxModule);
    // build collapsable frame
    myCollapsableFrame = new FXVerticalFrame(this, GUIDesignCollapsableFrame);
}


FXGroupBoxModule::~FXGroupBoxModule() {}


void 
FXGroupBoxModule::setText(const std::string& text) {
    myLabel->setText(text.c_str());
}


FXVerticalFrame* 
FXGroupBoxModule::getCollapsableFrame() {
    return myCollapsableFrame;
}


long
FXGroupBoxModule::onPaint(FXObject*, FXSelector, void* ptr) {
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
FXGroupBoxModule::onCollapseButton(FXObject*,FXSelector,void*) {
    if (myCollapsed) {
        myCollapsed = false;
        myCollapseButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::COLLAPSE));
        myCollapsableFrame->show();
    } else {
        myCollapsed = true;
        myCollapseButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::UNCOLLAPSE));
        myCollapsableFrame->hide();
    }
    recalc();
    return 1;
}


FXGroupBoxModule::FXGroupBoxModule() :
    myCollapsed(false) {}