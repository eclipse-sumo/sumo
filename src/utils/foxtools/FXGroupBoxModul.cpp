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
/// @file    FXGroupBoxModul.cpp
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

#include "FXGroupBoxModul.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(FXGroupBoxModul) FXGroupBoxModulMap[] = {
    FXMAPFUNC(SEL_PAINT,  0,  FXGroupBoxModul::onPaint),
};

// Object implementation
FXIMPLEMENT(FXGroupBoxModul, FXGroupBox, FXGroupBoxModulMap, ARRAYNUMBER(FXGroupBoxModulMap))

// ===========================================================================
// method definitions
// ===========================================================================

FXGroupBoxModul::FXGroupBoxModul(FXVerticalFrame* contentFrame, const std::string &text, const bool collapsible) :
    FXGroupBox(contentFrame, text.c_str(), GUIDesignGroupBoxFrame),
    myCollapsible(collapsible) {
}


FXGroupBoxModul::~FXGroupBoxModul() {}


FXComposite* 
FXGroupBoxModul::getComposite() {
    return this;
}


long
FXGroupBoxModul::onPaint(FXObject* obj ,FXSelector sel, void* ptr) {
    // first check if this FXGroupBoxModul is collapsible
    if (myCollapsible) {
        FXEvent *event = (FXEvent*)ptr;
        FXDCWindow dc(this, event);
        // declare int for positions
        FXint tw = 0;
        FXint th = 0;
        FXint xx = 0;
        FXint yy = 0;
        // Paint background
        dc.setForeground(backColor);
        dc.fillRectangle(event->rect.x, event->rect.y, event->rect.w, event->rect.h);
        // Draw label if there is one
        if (!label.empty()) {
            yy = 2 + font->getFontAscent() / 2;
        }
        // draw groove rectangle
        drawGrooveRectangle(dc, 0, yy, width, height - yy);
        // Draw label
        if(!label.empty()){
            tw = font->getTextWidth(label);
            th = font->getFontHeight() + 4;
            if (options&GROUPBOX_TITLE_RIGHT) {
                xx = width - tw - 12;
            } else if (options&GROUPBOX_TITLE_CENTER) {
                xx = (width - tw) / 2 - 4;
            } else {
                xx = 4;
            }
            if (xx < 4) {
                xx = 4;
            }
            if ((tw + 16) > width) {
                tw = width - 16;
            }
            if (0 < tw) {
                dc.setForeground(backColor);
                dc.setFont(font);
                dc.fillRectangle(xx, yy, tw + 8, 2);
                dc.setClipRectangle(xx + 4, 0, tw, th);
                if (isEnabled()) {
                    dc.setForeground(textColor);
                    dc.drawText(xx + 4, 2 + font->getFontAscent(), label);
                } else {
                    dc.setForeground(hiliteColor);
                    dc.drawText(xx + 5, 3 + font->getFontAscent(), label);
                    dc.setForeground(shadowColor);
                    dc.drawText(xx + 4, 2 + font->getFontAscent(), label);
                }
            }
        }
        return 1;
    } else {
        // just draw like a normal FXGroupBox
        return FXGroupBox::onPaint(obj, sel, ptr);
    }
}


FXGroupBoxModul::FXGroupBoxModul() :
    myCollapsible(false) {}