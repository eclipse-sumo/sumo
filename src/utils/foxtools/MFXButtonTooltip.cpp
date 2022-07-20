/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    MFXButtonTooltip.cpp
/// @author  Angelo Banse
/// @date    2022-06-21
///
// Button similar to FXButton but with the possibility of showing tooltips
/****************************************************************************/
#include <config.h>

#include "MFXButtonTooltip.h"


FXDEFMAP(MFXButtonTooltip) MFXButtonTooltipMap[] = {
    FXMAPFUNC(SEL_ENTER,    0,  MFXButtonTooltip::onEnter),
    FXMAPFUNC(SEL_LEAVE,    0,  MFXButtonTooltip::onLeave),
};


// Object implementation
FXIMPLEMENT(MFXButtonTooltip, FXButton, MFXButtonTooltipMap, ARRAYNUMBER(MFXButtonTooltipMap))

MFXButtonTooltip::MFXButtonTooltip(FXComposite* p,
                                   const FXString& text, FXIcon* ic,
                                   FXObject* tgt, FXSelector sel,
                                   FXuint opts,
                                   FXint x, FXint y, FXint w, FXint h,
                                   FXint pl, FXint pr, FXint pt, FXint pb) :
    FXButton(p, text, ic, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb) {
}


MFXButtonTooltip::~MFXButtonTooltip() {}


long
MFXButtonTooltip::onEnter(FXObject* sender, FXSelector sel, void* ptr) {
    // create on first enter
    if (myStaticToolTip == nullptr) {
        myStaticToolTip = new MFXStaticToolTip(getApp());
        myStaticToolTip->create();
    }
    // show tip show
    myStaticToolTip->onTipShow(sender, sel, ptr);
    return FXButton::onEnter(sender, sel, ptr);
}


long
MFXButtonTooltip::onLeave(FXObject* sender, FXSelector sel, void* ptr) {
    // hide tip show
    myStaticToolTip->onTipHide(sender, sel, this);
    return FXButton::onLeave(sender, sel, ptr);
}


/****************************************************************************/
