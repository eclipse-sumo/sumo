/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
/// @file    MFXListItemIcon.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2023
///
//
/****************************************************************************/

/* =========================================================================
 * included modules
 * ======================================================================= */

#include "MFXListItemIcon.h"


#define SIDE_SPACING        6   // Left or right spacing between items
#define ICON_SPACING        4   // Spacing between icon and label (2 + 2)
#define ICON_SIZE           16

// ===========================================================================
// FOX callback mapping
// ===========================================================================

// Object implementation
FXIMPLEMENT(MFXListItemIcon, FXListItem, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

MFXListItemIcon::MFXListItemIcon(const FXString& text, FXIcon* ic, FXColor backGroundColor, void* ptr):
    FXListItem(text, ic, ptr),
    myBackGroundColor(backGroundColor) {
}


void
MFXListItemIcon::draw(const FXList* myList, FXDC& dc, FXint xx, FXint yy, FXint ww, FXint hh) {
    FXFont* font = myList->getFont();
    FXint ih = 0, th = 0;
    ih = ICON_SIZE;
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
    xx += ICON_SPACING + ICON_SIZE;
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
MFXListItemIcon::getBackGroundColor() const {
    return myBackGroundColor;
}


MFXListItemIcon::MFXListItemIcon() :
    FXListItem("", nullptr),
    myBackGroundColor(FXRGB(0, 0, 0)) {
}
