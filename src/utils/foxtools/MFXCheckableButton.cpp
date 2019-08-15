/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MFXCheckableButton.cpp
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
/// @version $Id$
///
// missing_desc
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MFXCheckableButton.h"


FXDEFMAP(MFXCheckableButton) MFXCheckableButtonMap[] = {
    FXMAPFUNC(SEL_PAINT, 0, MFXCheckableButton::onPaint),
    FXMAPFUNC(SEL_UPDATE, 0, MFXCheckableButton::onUpdate),
};


// Object implementation
FXIMPLEMENT(MFXCheckableButton, FXButton, MFXCheckableButtonMap, ARRAYNUMBER(MFXCheckableButtonMap))

MFXCheckableButton::MFXCheckableButton(bool amChecked, FXComposite* p,
                                       const FXString& text, FXIcon* ic,
                                       FXObject* tgt, FXSelector sel,
                                       FXuint opts,
                                       FXint x, FXint y, FXint w, FXint h,
                                       FXint pl, FXint pr, FXint pt, FXint pb) :
    FXButton(p, text, ic, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb),
    myAmChecked(amChecked), myAmInitialised(false) {
    border = 0;
}


MFXCheckableButton::~MFXCheckableButton() {}


bool
MFXCheckableButton::amChecked() const {
    return myAmChecked;
}


void
MFXCheckableButton::setChecked(bool val) {
    myAmChecked = val;
}


long
MFXCheckableButton::onPaint(FXObject* sender, FXSelector sel, void* data) {
    if (!myAmInitialised) {
        buildColors();
    }
    setColors();
    return FXButton::onPaint(sender, sel, data);
}


long
MFXCheckableButton::onUpdate(FXObject* sender, FXSelector sel, void* data) {
    if (!myAmInitialised) {
        buildColors();
    }
    setColors();
    long ret = FXButton::onUpdate(sender, sel, data);
    return ret;
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

