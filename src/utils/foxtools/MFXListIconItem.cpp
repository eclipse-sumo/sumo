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
/// @file    MFXListIconItem.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2023
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <utils/common/UtilExceptions.h>
#include <fxkeys.h>

#include "MFXListIconItem.h"
#include "MFXListIcon.h"

// ===========================================================================
// Macross
// ===========================================================================

#define SIDE_SPACING    6   // Left or right spacing between items
#define ICON_SPACING    4   // Spacing between icon and label (2 + 2)
#define LINE_SPACING    4   // Line spacing between items
#define ICON_SIZE       16

// ===========================================================================
// FOX callback mapping
// ===========================================================================

// Object implementation
FXIMPLEMENT(MFXListIconItem, FXObject, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

MFXListIconItem::MFXListIconItem(const FXString& text, FXIcon* ic, FXColor backGroundColor, void* ptr):
    label(text),
    icon(ic),
    data(ptr),
    myBackGroundColor(backGroundColor) {
}


MFXListIconItem::~MFXListIconItem() {
    if (state & ICONOWNED) {
        delete icon;
    }
    icon = (FXIcon*) - 1L;
}


void
MFXListIconItem::setFocus(FXbool focus) {
    if (focus) {
        state |= FOCUS;
    } else {
        state &= ~FOCUS;
    }
}


FXbool
MFXListIconItem::hasFocus() const {
    return (state & FOCUS) != 0;
}


void
MFXListIconItem::setSelected(FXbool selected) {
    if (selected) {
        state |= SELECTED;
    } else {
        state &= ~SELECTED;
    }
}


FXbool
MFXListIconItem::isSelected() const {
    return (state & SELECTED) != 0;
}


void
MFXListIconItem::setEnabled(FXbool enabled) {
    if (enabled) {
        state &= ~DISABLED;
    } else {
        state |= DISABLED;
    }
}


FXbool
MFXListIconItem::isEnabled() const {
    return (state & DISABLED) == 0;
}


void
MFXListIconItem::setDraggable(FXbool draggable) {
    if (draggable) {
        state |= DRAGGABLE;
    } else {
        state &= ~DRAGGABLE;
    }
}


FXbool
MFXListIconItem::isDraggable() const {
    return (state & DRAGGABLE) != 0;
}


void
MFXListIconItem::setText(const FXString& txt) {
    label = txt;
}


const FXString&
MFXListIconItem::getText() const {
    return label;
}


FXIcon*
MFXListIconItem::getIcon() const {
    return icon;
}


void
MFXListIconItem::create() {
    if (icon) {
        icon->create();
    }
}


void
MFXListIconItem::destroy() {
    if ((state & ICONOWNED) && icon) {
        icon->destroy();
    }
}


void
MFXListIconItem::detach() {
    if (icon) {
        icon->detach();
    }
}


FXint
MFXListIconItem::getWidth(const MFXListIcon* list) const {
    FXFont* font = list->getFont();
    FXint w = 0;
    if (icon) {
        w = icon->getWidth();
    }
    if (!label.empty()) {
        if (w) {
            w += ICON_SPACING;
        }
        w += font->getTextWidth(label.text(), label.length());
    }
    return SIDE_SPACING + w;
}



FXint
MFXListIconItem::getHeight(const MFXListIcon* list) const {
    FXFont* font = list->getFont();
    FXint th = 0, ih = 0;
    if (icon) {
        ih = icon->getHeight();
    }
    if (!label.empty()) {
        th = font->getFontHeight();
    }
    return LINE_SPACING + FXMAX(th, ih);
}



const FXColor&
MFXListIconItem::getBackGroundColor() const {
    return myBackGroundColor;
}


MFXListIconItem::MFXListIconItem() {}


void
MFXListIconItem::draw(const MFXListIcon* list, FXDC&   dc, FXint xx, FXint yy, FXint ww, FXint hh) {
    FXFont* font = list->getFont();
    FXint ih = icon ? ICON_SIZE : 0;
    FXint th = 0;
    if (!label.empty()) {
        th = font->getFontHeight();
    }
    if (isSelected()) {
        dc.setForeground(list->getSelBackColor());
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
            dc.setForeground(makeShadowColor(list->getBackColor()));
        } else if (isSelected()) {
            dc.setForeground(list->getSelTextColor());
        } else {
            dc.setForeground(list->getTextColor());
        }
        dc.drawText(xx, yy + (hh - th) / 2 + font->getFontAscent(), label);
    }
}


FXint
MFXListIconItem::hitItem(const MFXListIcon* list, FXint xx, FXint yy) const {
    FXint iw = 0, ih = 0, tw = 0, th = 0, ix, iy, tx, ty, h;
    FXFont* font = list->getFont();
    if (icon) {
        iw = icon->getWidth();
        ih = icon->getHeight();
    }
    if (!label.empty()) {
        tw = 4 + font->getTextWidth(label.text(), label.length());
        th = 4 + font->getFontHeight();
    }
    h = LINE_SPACING + FXMAX(th, ih);
    ix = SIDE_SPACING / 2;
    tx = SIDE_SPACING / 2;
    if (iw) {
        tx += iw + ICON_SPACING;
    }
    iy = (h - ih) / 2;
    ty = (h - th) / 2;

    // In icon?
    if (ix <= xx && iy <= yy && xx < ix + iw && yy < iy + ih) {
        return 1;
    }

    // In text?
    if (tx <= xx && ty <= yy && xx < tx + tw && yy < ty + th) {
        return 2;
    }

    // Outside
    return 0;
}
