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
/// @file    MFXTextFieldIcon.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2021
///
//
/****************************************************************************/

#include "MFXTextFieldIcon.h"


MFXTextFieldIcon::MFXTextFieldIcon(FXComposite* p, FXint ncols, FXObject* tgt, FXSelector sel, FXuint opt, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) :
    FXTextField(p, ncols, tgt, sel, opt, x, y, w, h, pl, pr, pt, pb),
    myIcon(nullptr) {
}


void
MFXTextFieldIcon::setIcon(FXIcon* icon) {
    myIcon = icon;
}


void
MFXTextFieldIcon::resetTextField() {
    setText("");
    setBackColor(FXRGB(255, 255, 255));
    setIcon(nullptr);
}


void
MFXTextFieldIcon::drawIconTextRange(FXDCWindow& dc, const FXint iconWidth, FXint fm, FXint to) {
    FXint sx, ex, xx, yy, cw, hh, ww, si, ei, lx, rx, t;
    const FXint rr = width - border - padright;
    const FXint ll = border + padleft;
    const FXint mm = (ll + rr) / 2;
    if (to <= fm) {
        return;
    }
    dc.setFont(font);
    // Text color
    dc.setForeground(textColor);
    // Height
    hh = font->getFontHeight();
    // Text sticks to top of field
    if (options & JUSTIFY_TOP) {
        yy = padtop + border;
    }
    // Text sticks to bottom of field
    else if (options & JUSTIFY_BOTTOM) {
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
    // Password mode
    if (options & TEXTFIELD_PASSWD) {
        cw = font->getTextWidth("*", 1);
        ww = cw * contents.count();
        // Text sticks to right of field
        if (options & JUSTIFY_RIGHT) {
            xx = shift + rr - ww + iconWidth;
        } else if (options & JUSTIFY_LEFT) {
            // Text sticks on left of field
            xx = shift + ll + iconWidth;
        } else {
            // Text centered in field
            xx = shift + mm - ww / 2 + iconWidth;
        }
        // Reduce to avoid drawing excessive amounts of text
        lx = xx + cw * contents.index(fm);
        rx = xx + cw * contents.index(to);
        while (fm < to) {
            if (lx + cw >= 0) {
                break;
            }
            lx += cw;
            fm = contents.inc(fm);
        }
        while (fm < to) {
            if (rx - cw < width) {
                break;
            }
            rx -= cw;
            to = contents.dec(to);
        }
        // Adjust selected range
        if (si < fm) {
            si = fm;
        }
        if (ei > to) {
            ei = to;
        }
        // Nothing selected
        if (!hasSelection() || to <= si || ei <= fm) {
            drawPWDTextFragment(dc, xx, yy, fm, to);
        } else {
            // Stuff selected
            if (fm < si) {
                drawPWDTextFragment(dc, xx, yy, fm, si);
            } else {
                si = fm;
            }
            if (ei < to) {
                drawPWDTextFragment(dc, xx, yy, ei, to);
            } else {
                ei = to;
            }
            if (si < ei) {
                sx = xx + cw * contents.index(si);
                ex = xx + cw * contents.index(ei);
                if (hasFocus()) {
                    dc.setForeground(selbackColor);
                    dc.fillRectangle(sx, padtop + border, ex - sx, height - padtop - padbottom - (border << 1));
                    dc.setForeground(seltextColor);
                    drawPWDTextFragment(dc, xx, yy, si, ei);
                } else {
                    dc.setForeground(baseColor);
                    dc.fillRectangle(sx, padtop + border, ex - sx, height - padtop - padbottom - (border << 1));
                    dc.setForeground(textColor);
                    drawPWDTextFragment(dc, xx, yy, si, ei);
                }
            }
        }
    } else {
        // Normal mode
        ww = font->getTextWidth(contents.text(), contents.length());
        // Text sticks to right of field
        if (options & JUSTIFY_RIGHT) {
            xx = shift + rr - ww + iconWidth;
        } else if (options & JUSTIFY_LEFT) {
            // Text sticks on left of field
            xx = shift + ll + iconWidth;
        } else {
            // Text centered in field
            xx = shift + mm - ww / 2 + iconWidth;
        }
        // Reduce to avoid drawing excessive amounts of text
        lx = xx + font->getTextWidth(&contents[0], fm);
        rx = lx + font->getTextWidth(&contents[fm], to - fm);
        while (fm < to) {
            t = contents.inc(fm);
            cw = font->getTextWidth(&contents[fm], t - fm);
            if (lx + cw >= 0) {
                break;
            }
            lx += cw;
            fm = t;
        }
        while (fm < to) {
            t = contents.dec(to);
            cw = font->getTextWidth(&contents[t], to - t);
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
        // Nothing selected
        if (!hasSelection() || to <= si || ei <= fm) {
            drawTextFragment(dc, xx, yy, fm, to);
        } else {
            // Stuff selected
            if (fm < si) {
                drawTextFragment(dc, xx, yy, fm, si);
            } else {
                si = fm;
            }
            if (ei < to) {
                drawTextFragment(dc, xx, yy, ei, to);
            } else {
                ei = to;
            }
            if (si < ei) {
                sx = xx + font->getTextWidth(contents.text(), si);
                ex = xx + font->getTextWidth(contents.text(), ei);
                if (hasFocus()) {
                    dc.setForeground(selbackColor);
                    dc.fillRectangle(sx, padtop + border, ex - sx, height - padtop - padbottom - (border << 1));
                    dc.setForeground(seltextColor);
                    drawTextFragment(dc, xx, yy, si, ei);
                } else {
                    dc.setForeground(baseColor);
                    dc.fillRectangle(sx, padtop + border, ex - sx, height - padtop - padbottom - (border << 1));
                    dc.setForeground(textColor);
                    drawTextFragment(dc, xx, yy, si, ei);
                }
            }
        }
    }
    // draw icon
    if (myIcon) {
        dc.drawIcon(myIcon, xx - myIcon->getWidth() - 3, yy + (hh - myIcon->getHeight()) / 2);
    }
}
