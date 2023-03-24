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
/// @file    MFXLabelTooltip.cpp
/// @author  Pablo Alvarez Lopez
/// @date    2022-08-10
///
// Label similar to FXLabel but with the possibility of showing tooltips
/****************************************************************************/
#include <config.h>

#include "MFXLabelTooltip.h"


FXDEFMAP(MFXLabelTooltip) MFXLabelTooltipMap[] = {
    FXMAPFUNC(SEL_PAINT,    0,  MFXLabelTooltip::onPaint),
    FXMAPFUNC(SEL_ENTER,    0,  MFXLabelTooltip::onEnter),
    FXMAPFUNC(SEL_LEAVE,    0,  MFXLabelTooltip::onLeave),
    FXMAPFUNC(SEL_MOTION,   0,  MFXLabelTooltip::onMotion),
};

// Object implementation
FXIMPLEMENT(MFXLabelTooltip, FXButton, MFXLabelTooltipMap, ARRAYNUMBER(MFXLabelTooltipMap))


MFXLabelTooltip::MFXLabelTooltip(FXComposite* p, MFXStaticToolTip* staticToolTip, const FXString& text, FXIcon* ic, FXuint opts,
                                 FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) :
    FXButton(p, text, ic, nullptr, 0, opts, x, y, w, h, pl, pr, pt, pb),
    myStaticToolTip(staticToolTip) {
    // to avoid select button, just disable it (we can improve this in the future)
    disable();
}


MFXLabelTooltip::~MFXLabelTooltip() {}


long
MFXLabelTooltip::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent* ev = (FXEvent*)ptr;
    FXDCWindow dc(this, ev);
    FXint tw = 0, th = 0, iw = 0, ih = 0, tx, ty, ix, iy;
    dc.setForeground(backColor);
    dc.fillRectangle(0, 0, width, height);
    if (!label.empty()) {
        tw = labelWidth(label);
        th = labelHeight(label);
    }
    if (icon) {
        iw = icon->getWidth();
        ih = icon->getHeight();
    }
    just_x(tx, ix, tw, iw);
    just_y(ty, iy, th, ih);
    if (icon) {
        dc.drawIcon(icon, ix, iy);
    }
    if (!label.empty()) {
        dc.setFont(font);
        dc.setForeground(textColor);
        drawLabel(dc, label, hotoff, tx, ty, tw, th);
    }
    drawFrame(dc, 0, 0, width, height);
    return 1;
}


long
MFXLabelTooltip::onEnter(FXObject* sender, FXSelector sel, void* ptr) {
    // show tip show
    myStaticToolTip->showStaticToolTip(getTipText());
    return FXButton::onEnter(sender, sel, ptr);
}


long
MFXLabelTooltip::onLeave(FXObject* sender, FXSelector sel, void* ptr) {
    // hide static toolTip
    myStaticToolTip->hideStaticToolTip();
    return FXButton::onLeave(sender, sel, ptr);
}


long
MFXLabelTooltip::onMotion(FXObject* sender, FXSelector sel, void* ptr) {
    // update static tooltip
    myStaticToolTip->onUpdate(sender, sel, ptr);
    return FXButton::onMotion(sender, sel, ptr);
}

/****************************************************************************/
