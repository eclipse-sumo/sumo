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


#define HSPACE  4
#define VSPACE  2

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(FXStaticToolTip) FXStaticToolTipMap[] = {
    FXMAPFUNC(SEL_PAINT,    0,                      FXStaticToolTip::onPaint),
    FXMAPFUNC(SEL_TIMEOUT,  FXToolTip::ID_TIP_SHOW, FXStaticToolTip::onTipShow),
};

// Object implementation
FXIMPLEMENT(FXStaticToolTip, FXToolTip, FXStaticToolTipMap, ARRAYNUMBER(FXStaticToolTipMap))

// ===========================================================================
// method definitions
// ===========================================================================

FXStaticToolTip::FXStaticToolTip(FXApp* app) :
    FXToolTip(app) {
}


FXStaticToolTip::~FXStaticToolTip() {}


long 
FXStaticToolTip::onPaint(FXObject* obj, FXSelector sel, void* ptr) {
    FXEvent *ev = (FXEvent*)ptr;
    FXDCWindow dc(this,ev);
    const FXchar *beg, *end;
    FXint tx,ty;
    dc.setForeground(backColor);
    dc.fillRectangle(ev->rect.x, ev->rect.y, ev->rect.w, ev->rect.h);
    dc.setForeground(textColor);
    dc.setFont(font);
    dc.drawRectangle(0, 0, width - 1, height - 1);
    beg = label.text();
    if (beg) {
        tx = 1 + HSPACE;
        ty = 1 + VSPACE + font->getFontAscent();
        do {
            end = beg;
            while ((*end != '\0') && (*end != '\n')) {
                end++;
            }
            dc.drawText(tx, ty, beg, end - beg);
            ty += font->getFontHeight();
            beg = end + 1;
        } while(*end != '\0');
    }
    return 1;
}


long
FXStaticToolTip::onTipShow(FXObject*, FXSelector, void*) {
    if (!label.empty()) {
        autoplace();
        show();
        if (!(options & TOOLTIP_PERMANENT)) {
            FXint timeoutms = getApp()->getTooltipTime();
            if (options & TOOLTIP_VARIABLE){
                timeoutms = timeoutms/4 + (timeoutms * label.length())/64;
            }
            getApp()->addTimeout(this, ID_TIP_HIDE, timeoutms);
        }
    }
    return 1;
}


FXStaticToolTip::FXStaticToolTip() :
    FXToolTip() {
}
