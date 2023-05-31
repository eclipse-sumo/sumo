/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXCheckableButton.cpp
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
///
// missing_desc
/****************************************************************************/
#include <config.h>

#include "MFXCheckableButton.h"


FXDEFMAP(MFXCheckableButton) MFXCheckableButtonMap[] = {
    FXMAPFUNC(SEL_PAINT,    0,  MFXCheckableButton::onPaint),
    FXMAPFUNC(SEL_UPDATE,   0,  MFXCheckableButton::onUpdate),
    FXMAPFUNC(SEL_ENTER,    0,  MFXCheckableButton::onEnter),
    FXMAPFUNC(SEL_LEAVE,    0,  MFXCheckableButton::onLeave),
    FXMAPFUNC(SEL_MOTION,   0,  MFXCheckableButton::onMotion),
};


// Object implementation
FXIMPLEMENT(MFXCheckableButton, FXButton, MFXCheckableButtonMap, ARRAYNUMBER(MFXCheckableButtonMap))

MFXCheckableButton::MFXCheckableButton(bool amChecked, FXComposite* p, MFXStaticToolTip* staticToolTip,
                                       const std::string& text, FXIcon* ic, FXObject* tgt, FXSelector sel,
                                       FXuint opts, FXint x, FXint y, FXint w, FXint h,
                                       FXint pl, FXint pr, FXint pt, FXint pb) :
    FXButton(p, text.c_str(), ic, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb),
    myAmChecked(amChecked), myAmInitialised(false),
    myStaticToolTip(staticToolTip) {
    border = 0;
}


MFXCheckableButton::~MFXCheckableButton() {}


bool
MFXCheckableButton::amChecked() const {
    return myAmChecked;
}


void
MFXCheckableButton::setChecked(bool val, const bool inform) {
    myAmChecked = val;
    if (inform) {
        if (myAmChecked) {
            FXButton::onCheck(nullptr, 0, nullptr);
        } else {
            FXButton::onUncheck(nullptr, 0, nullptr);
        }
    }
}


long
MFXCheckableButton::onPaint(FXObject* sender, FXSelector sel, void* ptr) {
    if (!myAmInitialised) {
        buildColors();
    }
    setColors();
    return FXButton::onPaint(sender, sel, ptr);
}


long
MFXCheckableButton::onUpdate(FXObject* sender, FXSelector sel, void* ptr) {
    if (!myAmInitialised) {
        buildColors();
    }
    setColors();
    return FXButton::onUpdate(sender, sel, ptr);
}


long
MFXCheckableButton::onEnter(FXObject* sender, FXSelector sel, void* ptr) {
    // show tip show
    myStaticToolTip->showStaticToolTip(getTipText());
    return FXButton::onEnter(sender, sel, ptr);
}


long
MFXCheckableButton::onLeave(FXObject* sender, FXSelector sel, void* ptr) {
    // hide static toolTip
    myStaticToolTip->hideStaticToolTip();
    return FXButton::onLeave(sender, sel, ptr);
}


long
MFXCheckableButton::onMotion(FXObject* sender, FXSelector sel, void* ptr) {
    // update static toolTip
    myStaticToolTip->onUpdate(sender, sel, ptr);
    return FXButton::onMotion(sender, sel, ptr);
}


void
MFXCheckableButton::buildColors() {
    myBackColor = backColor;
    myDarkColor = makeShadowColor(myBackColor);
    myHiliteColor = hiliteColor;
    myShadowColor = shadowColor;
    myAmInitialised = true;
}


void
MFXCheckableButton::setColors() {
    options &= (0xffffffff - (FRAME_SUNKEN | FRAME_SUNKEN | FRAME_THICK));
    if (myAmChecked) {
        backColor = myShadowColor;
        hiliteColor = myDarkColor;
        shadowColor = myHiliteColor;
        if (state == STATE_ENGAGED) {
            options |= FRAME_SUNKEN | FRAME_THICK;
        } else {
            options |= FRAME_SUNKEN;
        }
    } else {
        backColor = myBackColor;
        hiliteColor = myHiliteColor;
        shadowColor = myShadowColor;
        if (state == STATE_ENGAGED) {
            options |= FRAME_RAISED | FRAME_THICK;
        } else {
            options |= FRAME_RAISED;
        }
    }
}


/****************************************************************************/
