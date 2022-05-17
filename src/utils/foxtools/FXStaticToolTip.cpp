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
/// @file    FXStaticToolTip.cpp
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

#include "FXStaticToolTip.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(FXStaticToolTip) FXStaticToolTipMap[] = {
    FXMAPFUNC(SEL_PAINT,    0,                              FXStaticToolTip::onPaint),
    FXMAPFUNC(SEL_COMMAND,  MID_GROUPBOXMODULE_COLLAPSE,    FXStaticToolTip::onCmdCollapseButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GROUPBOXMODULE_SAVE,        FXStaticToolTip::onCmdSaveButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GROUPBOXMODULE_LOAD,        FXStaticToolTip::onCmdLoadButton),
};

// Object implementation
FXIMPLEMENT(FXStaticToolTip, FXVerticalFrame, FXStaticToolTipMap, ARRAYNUMBER(FXStaticToolTipMap))

// ===========================================================================
// method definitions
// ===========================================================================

FXStaticToolTip::FXStaticToolTip(FXVerticalFrame* contentFrame, const std::string& text, const int options) :
    FXVerticalFrame(contentFrame, GUIDesignHorizontalFrame),
    myOptions(options),
    myCollapsed(false) {
    // build button and labels
    FXHorizontalFrame* headerFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    if (myOptions & Options::COLLAPSIBLE) {
        myCollapseButton = new FXButton(headerFrame, "", GUIIconSubSys::getIcon(GUIIcon::COLLAPSE), this, MID_GROUPBOXMODULE_COLLAPSE, GUIDesignButtonFXStaticToolTip);
    }
    if (myOptions & Options::SAVE) {
        mySaveButton = new FXButton(headerFrame, "", GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_GROUPBOXMODULE_SAVE, GUIDesignButtonFXStaticToolTip);
    }
    if (myOptions & Options::LOAD) {
        myLoadButton = new FXButton(headerFrame, "", GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, MID_GROUPBOXMODULE_LOAD, GUIDesignButtonFXStaticToolTip);
    }
    myLabel = new FXLabel(headerFrame, text.c_str(), nullptr, GUIDesignLabelFXStaticToolTip);
    // build collapsable frame
    myCollapsableFrame = new FXVerticalFrame(this, GUIDesignCollapsableFrame);
}


FXStaticToolTip::~FXStaticToolTip() {}


void
FXStaticToolTip::setText(const std::string& text) {
    myLabel->setText(text.c_str());
}


FXVerticalFrame*
FXStaticToolTip::getCollapsableFrame() {
    return myCollapsableFrame;
}


long
FXStaticToolTip::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXDCWindow dc(this, event);
    // Paint background
    dc.setForeground(backColor);
    dc.fillRectangle(event->rect.x, event->rect.y, event->rect.w, event->rect.h);
    // draw groove rectangle
    drawGrooveRectangle(dc, 0, 15, width, height - 15);
    return 1;
}


long
FXStaticToolTip::onCmdCollapseButton(FXObject*, FXSelector, void*) {
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


long
FXStaticToolTip::onCmdSaveButton(FXObject*, FXSelector, void*) {
    return saveContents();
}


long
FXStaticToolTip::onCmdLoadButton(FXObject*, FXSelector, void*) {
    return loadContents();
}


FXStaticToolTip::FXStaticToolTip() :
    myOptions(Options::NOTHING),
    myCollapsed(false) {
}


bool
FXStaticToolTip::saveContents() const {
    // nothing to do
    return false;
}


bool
FXStaticToolTip::loadContents() const {
    // nothing to do
    return false;
}


void
FXStaticToolTip::toogleSaveButton(const bool value) {
    if (mySaveButton) {
        if (value) {
            mySaveButton->enable();
        } else {
            mySaveButton->disable();
        }
    }
}
