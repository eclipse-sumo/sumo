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
/// @file    MFXTextFieldSearch.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// TextField for search elements
/****************************************************************************/

#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>

#include "MFXTextFieldSearch.h"

// =========================================================================
// defines
// =========================================================================

#define ICON_SPACING    4   // Spacing between icon and label (2 + 2)
#define ICON_SIZE       16

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXTextFieldSearch) MFXTextFieldSearchMap[] = {
    FXMAPFUNC(SEL_PAINT,        0,  MFXTextFieldSearch::onPaint),
    FXMAPFUNC(SEL_FOCUSIN,      0,  MFXTextFieldSearch::onFocusIn),
    FXMAPFUNC(SEL_FOCUSOUT,     0,  MFXTextFieldSearch::onFocusOut),
    FXMAPFUNC(SEL_FOCUS_SELF,   0,  MFXTextFieldSearch::onFocusSelf),
    FXMAPFUNC(SEL_KEYPRESS,     0,  MFXTextFieldSearch::onKeyPress),
};

// Object implementation
FXIMPLEMENT(MFXTextFieldSearch, MFXTextFieldIcon, MFXTextFieldSearchMap, ARRAYNUMBER(MFXTextFieldSearchMap))

// ===========================================================================
// member method definitions
// ===========================================================================

MFXTextFieldSearch::MFXTextFieldSearch(FXComposite* p, FXint ncols, FXObject* tgt, FXSelector sel, FXuint opt, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) :
    MFXTextFieldIcon(p, ncols, GUIIconSubSys::getIcon(GUIIcon::SEARCH), tgt, sel, opt, x, y, w, h, pl, pr, pt, pb),
    myTarget(tgt) {
}


long
MFXTextFieldSearch::onKeyPress(FXObject* obj, FXSelector sel, void* ptr) {
    MFXTextFieldIcon::onKeyPress(obj, sel, ptr);
    return myTarget->handle(this, FXSEL(SEL_COMMAND, MID_MTEXTFIELDSEARCH_UPDATED), ptr);
}


long
MFXTextFieldSearch::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent* ev = (FXEvent*)ptr;
    FXDCWindow dc(this, ev);
    // Draw frame
    drawFrame(dc, 0, 0, width, height);
    // Gray background if disabled
    if (isEnabled()) {
        dc.setForeground(backColor);
    } else {
        dc.setForeground(baseColor);
    }
    // Draw background
    dc.fillRectangle(border, border, width - (border << 1), height - (border << 1));
    // Draw text,  clipped against frame interior
    dc.setClipRectangle(border, border, width - (border << 1), height - (border << 1));
    // continue depending of search string
    if (hasFocus() || (contents.count() > 0)) {
        drawTextRange(dc, 0, contents.length());
    } else {
        drawSearchTextRange(dc, 0, TL("Type to search..."));
    }
    // Draw caret
    if (flags & FLAG_CARET) {
        int xx = coord(cursor) - 1;
        xx += ICON_SPACING + ICON_SIZE;
        dc.setForeground(cursorColor);
        dc.fillRectangle(xx, padtop + border, 1, height - padbottom - padtop - (border << 1));
        dc.fillRectangle(xx - 2, padtop + border, 5, 1);
        dc.fillRectangle(xx - 2, height - border - padbottom - 1, 5, 1);
    }
    // draw icon
    dc.drawIcon(icon, 3, border + padtop + (height - padbottom - padtop - (border << 1) - ICON_SIZE) / 2);
    return 1;
}


long
MFXTextFieldSearch::onFocusIn(FXObject* sender, FXSelector sel, void* ptr) {
    update();
    return MFXTextFieldIcon::onFocusIn(sender, sel, ptr);
}



long
MFXTextFieldSearch::onFocusOut(FXObject* sender, FXSelector sel, void* ptr) {
    update();
    return MFXTextFieldIcon::onFocusOut(sender, sel, ptr);
}



long
MFXTextFieldSearch::onFocusSelf(FXObject* sender, FXSelector sel, void* ptr) {
    //onPaint(sender, sel, ptr);
    return MFXTextFieldIcon::onFocusSelf(sender, sel, ptr);
}


MFXTextFieldSearch::MFXTextFieldSearch() :
    MFXTextFieldIcon() {
}


void
MFXTextFieldSearch::drawSearchTextRange(FXDCWindow& dc, FXint fm, const FXString& searchString) {
    FXint xx, yy, cw, hh, ww, si, ei, lx, rx, t;
    FXint rr = width - border - padright;
    FXint ll = border + padleft;
    FXint mm = (ll + rr) / 2;
    FXint to = (int)searchString.length();
    if (to <= fm) {
        return;
    }
    dc.setFont(font);
    // Text color
    dc.setForeground(FXRGBA(128, 128, 128, 255));
    // Height
    hh = font->getFontHeight();
    // Text sticks to top of field
    if (options & JUSTIFY_TOP) {
        yy = padtop + border;
    } else if (options & JUSTIFY_BOTTOM) {
        // Text sticks to bottom of field
        yy = height - padbottom - border - hh;
    } else {
        // Text centered in y
        yy = border + padtop + (height - padbottom - padtop - (border << 1) - hh) / 2;
    }
    if (anchor < cursor) {
        si = anchor;
        ei = cursor;
    } else {
        si = cursor;
        ei = anchor;
    }
    // Normal mode
    ww = font->getTextWidth(searchString.text(), searchString.length());
    // Text sticks to right of field
    if (options & JUSTIFY_RIGHT) {
        xx = shift + rr - ww;
    } else if (options & JUSTIFY_LEFT) {
        // Text sticks on left of field
        xx = shift + ll;
    } else {
        // Text centered in field
        xx = shift + mm - ww / 2;
    }
    // add icon spacing
    xx += ICON_SPACING + ICON_SIZE;
    // Reduce to avoid drawing excessive amounts of text
    lx = xx + font->getTextWidth(&searchString[0], fm);
    rx = lx + font->getTextWidth(&searchString[fm], to - fm);
    while (fm < to) {
        t = searchString.inc(fm);
        cw = font->getTextWidth(&searchString[fm], t - fm);
        if (lx + cw >= 0) {
            break;
        }
        lx += cw;
        fm = t;
    }
    while (fm < to) {
        t = searchString.dec(to);
        cw = font->getTextWidth(&searchString[t], to - t);
        if (rx - cw < width) {
            break;
        }
        rx -= cw;
        to = t;
    }
    // Adjust selected range
    if (si < fm) {
        si = fm;
    }
    if (ei > to) {
        ei = to;
    }
    // draw text
    xx += font->getTextWidth(searchString.text(), fm);
    yy += font->getFontAscent();
    dc.drawText(xx, yy, &searchString[fm], to - fm);
}
