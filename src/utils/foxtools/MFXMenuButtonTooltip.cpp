/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXMenuButtonTooltip.cpp
/// @author  Pablo Alvarez Lopez
/// @date    2022-07-27
///
// MenuButton similar to FXMenuButton but with the possibility of showing tooltips
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include "MFXMenuButtonTooltip.h"


FXDEFMAP(MFXMenuButtonTooltip) MFXMenuButtonTooltipMap[] = {
    FXMAPFUNC(SEL_ENTER,            0,                  MFXMenuButtonTooltip::onEnter),
    FXMAPFUNC(SEL_LEAVE,            0,                  MFXMenuButtonTooltip::onLeave),
    FXMAPFUNC(SEL_MOTION,           0,                  MFXMenuButtonTooltip::onMotion),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,  0,                  MFXMenuButtonTooltip::onLeftBtnPress),
    FXMAPFUNC(SEL_KEYPRESS,         0,                  MFXMenuButtonTooltip::onKeyPress),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_POST,  MFXMenuButtonTooltip::onCmdPost),
};

// Object implementation
FXIMPLEMENT(MFXMenuButtonTooltip, FXMenuButton, MFXMenuButtonTooltipMap, ARRAYNUMBER(MFXMenuButtonTooltipMap))


MFXMenuButtonTooltip::MFXMenuButtonTooltip(FXComposite* p, MFXStaticToolTip* staticToolTip, const std::string& text, FXIcon* ic,
        FXPopup* pup, FXObject* optionalTarget, FXuint opts,
        FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) :
    FXMenuButton(p, text.c_str(), ic, pup, opts, x, y, w, h, pl, pr, pt, pb),
    myStaticToolTip(staticToolTip),
    myOptionalTarget(optionalTarget) {
}


MFXMenuButtonTooltip::~MFXMenuButtonTooltip() {}


long
MFXMenuButtonTooltip::onEnter(FXObject* sender, FXSelector sel, void* ptr) {
    // show tip show
    myStaticToolTip->showStaticToolTip(getTipText());
    return FXMenuButton::onEnter(sender, sel, ptr);
}


long
MFXMenuButtonTooltip::onLeave(FXObject* sender, FXSelector sel, void* ptr) {
    // hide static toolTip
    myStaticToolTip->hideStaticToolTip();
    return FXMenuButton::onLeave(sender, sel, ptr);
}


long
MFXMenuButtonTooltip::onMotion(FXObject* sender, FXSelector sel, void* ptr) {
    // update static tooltip
    myStaticToolTip->onUpdate(sender, sel, ptr);
    return FXMenuButton::onMotion(sender, sel, ptr);
}


long
MFXMenuButtonTooltip::onLeftBtnPress(FXObject* sender, FXSelector sel, void* ptr) {
    // inform optional target
    if (myOptionalTarget) {
        myOptionalTarget->tryHandle(this, FXSEL(MID_MBTTIP_FOCUS, message), nullptr);
    }
    // continue handling onLeftBtnPress
    return FXMenuButton::onLeftBtnPress(sender, sel, ptr);
}

long
MFXMenuButtonTooltip::onKeyPress(FXObject* sender, FXSelector sel, void* ptr) {
    // inform optional target
    if (myOptionalTarget) {
        myOptionalTarget->tryHandle(this, FXSEL(MID_MBTTIP_FOCUS, message), nullptr);
    }
    // continue handling onKeyPress
    return FXMenuButton::onKeyPress(sender, sel, ptr);
}


long
MFXMenuButtonTooltip::onCmdPost(FXObject* sender, FXSelector sel, void* ptr) {
    // inform optional target
    if (myOptionalTarget) {
        myOptionalTarget->tryHandle(this, FXSEL(MID_MBTTIP_SELECTED, message), nullptr);
    }
    // continue handling onCheck
    return FXMenuButton::onCmdPost(sender, sel, ptr);
}

/****************************************************************************/
