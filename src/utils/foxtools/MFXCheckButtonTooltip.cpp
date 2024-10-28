/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    MFXCheckButtonTooltip.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 23
///
// CheckButton similar to FXCheckButton but with the possibility of showing tooltips
/****************************************************************************/
#include <config.h>

#include "MFXCheckButtonTooltip.h"


FXDEFMAP(MFXCheckButtonTooltip) MFXCheckButtonTooltipMap[] = {
    FXMAPFUNC(SEL_ENTER,    0,  MFXCheckButtonTooltip::onEnter),
    FXMAPFUNC(SEL_LEAVE,    0,  MFXCheckButtonTooltip::onLeave),
    FXMAPFUNC(SEL_MOTION,   0,  MFXCheckButtonTooltip::onMotion),
};

// Object implementation
FXIMPLEMENT(MFXCheckButtonTooltip, FXCheckButton, MFXCheckButtonTooltipMap, ARRAYNUMBER(MFXCheckButtonTooltipMap))


MFXCheckButtonTooltip::MFXCheckButtonTooltip(FXComposite* p, MFXStaticToolTip* staticToolTip, const FXString& text, FXObject* tgt,
        FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) :
    FXCheckButton(p, text, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb),
    myStaticToolTip(staticToolTip) {
}


MFXCheckButtonTooltip::~MFXCheckButtonTooltip() {}


void
MFXCheckButtonTooltip::setToolTipText(const FXString& toolTip) {
    myToolTipText = toolTip;
}


long
MFXCheckButtonTooltip::onEnter(FXObject* sender, FXSelector sel, void* ptr) {
    // check if show toolTip text
    if (!myToolTipText.empty()) {
        // show toolTip text
        setTipText(myToolTipText);
        // show tip show
        myStaticToolTip->showStaticToolTip(getTipText());
    }
    // continue with FXCheckButton function
    return FXCheckButton::onEnter(sender, sel, ptr);
}


long
MFXCheckButtonTooltip::onLeave(FXObject* sender, FXSelector sel, void* ptr) {
    // hide static toolTip
    myStaticToolTip->hideStaticToolTip();
    // continue with FXCheckButton function
    return FXCheckButton::onLeave(sender, sel, ptr);
}


long
MFXCheckButtonTooltip::onMotion(FXObject* sender, FXSelector sel, void* ptr) {
    // update static tooltip
    myStaticToolTip->onUpdate(sender, sel, ptr);
    return FXCheckButton::onMotion(sender, sel, ptr);
}

/****************************************************************************/
