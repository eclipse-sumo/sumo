/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXListIcon.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2023
///
//
/****************************************************************************/

/* =========================================================================
 * included modules
 * ======================================================================= */

#include <utils/common/UtilExceptions.h>

#include "MFXListIcon.h"


#define SIDE_SPACING    6   // Left or right spacing between items
#define ICON_SPACING    4   // Spacing between icon and label (2 + 2)
#define LINE_SPACING    4   // Line spacing between items
#define ICON_SIZE       16


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXListIcon) MFXListIconMap[] = {
    FXMAPFUNC(SEL_PAINT,    0,                          MFXListIcon::onPaint),
    FXMAPFUNC(SEL_TIMEOUT,  FXWindow::ID_AUTOSCROLL,    MFXListIcon::onAutoScroll),
};

// Object implementation
FXIMPLEMENT(MFXListIconItem,    FXListItem, nullptr, 0)
FXIMPLEMENT(MFXListIcon,        FXList,     MFXListIconMap, ARRAYNUMBER(MFXListIconMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// MFXListIconItem - methods
// ---------------------------------------------------------------------------

MFXListIconItem::MFXListIconItem(const FXString& text, FXIcon* ic, FXColor backGroundColor, void* ptr):
    FXListItem(text, ic, ptr),
    myBackGroundColor(backGroundColor) {
}


void
MFXListIconItem::draw(const FXList* myList, FXDC& dc, FXint xx, FXint yy, FXint ww, FXint hh) {
    FXFont* font = myList->getFont();
    FXint ih = icon? ICON_SIZE : 0;
    FXint th = 0;
    if (!label.empty()) {
        th = font->getFontHeight();
    }
    if (isSelected()) {
        dc.setForeground(myList->getSelBackColor());
    } else {
        dc.setForeground(myBackGroundColor);     // FIXME maybe paint background in onPaint?
    }
    dc.fillRectangle(xx, yy, ww, hh);
    if (hasFocus()) {
        dc.drawFocusRectangle(xx + 1, yy + 1, ww - 2, hh - 2);
    }
    xx += SIDE_SPACING / 2;
    if (icon) {
        dc.drawIcon(icon, xx, yy + (hh - ih) / 2);
    }
    if (icon) {
        xx += ICON_SPACING + ICON_SIZE;
    }
    if (!label.empty()) {
        dc.setFont(font);
        if (!isEnabled()) {
            dc.setForeground(makeShadowColor(myList->getBackColor()));
        } else if (isSelected()) {
            dc.setForeground(myList->getSelTextColor());
        } else {
            dc.setForeground(myList->getTextColor());
        }
        dc.drawText(xx, yy + (hh - th) / 2 + font->getFontAscent(), label);
    }
}

const FXColor&
MFXListIconItem::getBackGroundColor() const {
    return myBackGroundColor;
}


MFXListIconItem::MFXListIconItem() :
    FXListItem("", nullptr),
    myBackGroundColor(FXRGB(0, 0, 0)) {
}

// ---------------------------------------------------------------------------
// MFXListIcon - methods
// ---------------------------------------------------------------------------

MFXListIcon::MFXListIcon(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXList(p, tgt, sel, opts, x, y, w, h) {
}


FXint
MFXListIcon::getDefaultHeight() {
    if (visible) {
        return visible * (LINE_SPACING + FXMAX(font->getFontHeight(), ICON_SIZE));
    } else {
        return FXScrollArea::getDefaultHeight();
    }
}


long
MFXListIcon::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXDCWindow dc(this, event);
    FXint i, y, h;
    // Paint items
    y = pos_y;
    for (i = 0; i < items.no(); i++) {
        const auto listIcon = dynamic_cast<MFXListIconItem*>(items[i]);
        if (listIcon) {
            h = listIcon->getHeight(this);
            if (event->rect.y <= (y + h) && y < (event->rect.y + event->rect.h)) {
                listIcon->draw(this, dc, pos_x, y, FXMAX(listWidth, viewport_w), h);
            }
            y += h;
        }
    }
    // Paint blank area below items
    if (y < (event->rect.y + event->rect.h)) {
        dc.setForeground(backColor);
        dc.fillRectangle(event->rect.x, y, event->rect.w, event->rect.y + event->rect.h - y);
    }
    return 1;
}


long
MFXListIcon::onAutoScroll(FXObject*, FXSelector, void*) {
    // nothing to do
    return 1;
}


MFXListIcon::MFXListIcon() :
    FXList() {
}
