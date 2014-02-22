/****************************************************************************/
/// @file    MFXCheckableButton.cpp
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MFXCheckableButton.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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
                                       FXint pl, FXint pr, FXint pt, FXint pb)
    : FXButton(p, text, ic, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb),
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

