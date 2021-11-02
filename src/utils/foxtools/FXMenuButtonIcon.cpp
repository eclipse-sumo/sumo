/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2021 German Aerospace Center (DLR) and others.
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
/// @file    FXMenuButtonIcon.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2021
///
//
/****************************************************************************/

#include <fxkeys.h>

#include "FXMenuButtonIcon.h"

#define MENUBUTTONARROW_WIDTH   11
#define MENUBUTTONARROW_HEIGHT  5

#define MENUBUTTON_MASK     (MENUBUTTON_AUTOGRAY | MENUBUTTON_AUTOHIDE | MENUBUTTON_TOOLBAR | MENUBUTTON_NOARROWS)
#define POPUP_MASK          (MENUBUTTON_UP | MENUBUTTON_LEFT)
#define ATTACH_MASK         (MENUBUTTON_ATTACH_RIGHT | MENUBUTTON_ATTACH_CENTER)


FXDEFMAP(FXMenuButtonIcon) FXMenuButtonIconMap[] = {
    FXMAPFUNC(SEL_PAINT,                0,                      FXMenuButtonIcon::onPaint),
    FXMAPFUNC(SEL_UPDATE,               0,                      FXMenuButtonIcon::onUpdate),
    FXMAPFUNC(SEL_ENTER,                0,                      FXMenuButtonIcon::onEnter),
    FXMAPFUNC(SEL_LEAVE,                0,                      FXMenuButtonIcon::onLeave),
    FXMAPFUNC(SEL_MOTION,               0,                      FXMenuButtonIcon::onMotion),
    FXMAPFUNC(SEL_FOCUSIN,              0,                      FXMenuButtonIcon::onFocusIn),
    FXMAPFUNC(SEL_FOCUSOUT,             0,                      FXMenuButtonIcon::onFocusOut),
    FXMAPFUNC(SEL_UNGRABBED,            0,                      FXMenuButtonIcon::onUngrabbed),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,      0,                      FXMenuButtonIcon::onLeftBtnPress),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE,    0,                      FXMenuButtonIcon::onLeftBtnRelease),
    FXMAPFUNC(SEL_KEYPRESS,             0,                      FXMenuButtonIcon::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,           0,                      FXMenuButtonIcon::onKeyRelease),
    FXMAPFUNC(SEL_KEYPRESS,             FXWindow::ID_HOTKEY,    FXMenuButtonIcon::onHotKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,           FXWindow::ID_HOTKEY,    FXMenuButtonIcon::onHotKeyRelease),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_POST,      FXMenuButtonIcon::onCmdPost),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_UNPOST,    FXMenuButtonIcon::onCmdUnpost),
};


// Object implementation
FXIMPLEMENT(FXMenuButtonIcon,   FXLabel,    FXMenuButtonIconMap,    ARRAYNUMBER(FXMenuButtonIconMap))


FXMenuButtonIcon::FXMenuButtonIcon(FXComposite* p, const FXString& text, FXIcon* ic, FXPopup* pup, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) :
    FXLabel(p, text, nullptr, opts, x, y, w, h, pl, pr, pt, pb),
    myPane(pup),
    myIcon(ic),
    myoffsetX(0),
    myOffsetY(0),
    myState(FALSE) {
}


FXMenuButtonIcon::~FXMenuButtonIcon() {
    myPane = (FXPopup*) - 1L;
}


void 
FXMenuButtonIcon::create() {
    FXLabel::create();
    if (myPane) {
        myPane->create();
    }
}


void 
FXMenuButtonIcon::detach() {
    FXLabel::detach();
    if (myPane) {
        myPane->detach();
    }
}


bool 
FXMenuButtonIcon::canFocus() const { 
    return true;
}



FXint FXMenuButtonIcon::getDefaultWidth() {
    FXint tw = 0, iw = 0, s = 0, w, pw;
    if (!label.empty()) { 
        tw = labelWidth(label); 
        s = 4; 
    }
    if (!(options & MENUBUTTON_NOARROWS)) {
        if (options & MENUBUTTON_LEFT) {
            iw = MENUBUTTONARROW_HEIGHT; 
        } else {
            iw = MENUBUTTONARROW_WIDTH;
        }
    }
/*
    if (icon) {
        iw = icon->getWidth();
    }
*/
    if (!(options & (ICON_AFTER_TEXT | ICON_BEFORE_TEXT))) {
        w = FXMAX(tw, iw); 
    } else {
        w = tw + iw + s;
    }
    w = padleft + padright + (border<<1) + w;
    if (!(options & MENUBUTTON_LEFT) && (options & MENUBUTTON_ATTACH_RIGHT) && (options & MENUBUTTON_ATTACH_CENTER)) {
        if (myPane) { 
            pw = myPane->getDefaultWidth(); 
            if (pw>w) {
                w = pw;
            }
        }
    }
    return w;
}


FXint 
FXMenuButtonIcon::getDefaultHeight() {
    FXint th = 0, ih = 0, h, ph;
    if (!label.empty()) { 
        th = labelHeight(label); 
    }
    if (!(options & MENUBUTTON_NOARROWS)) {
        if (options & MENUBUTTON_LEFT) {
            ih = MENUBUTTONARROW_WIDTH; 
        } else {
            ih = MENUBUTTONARROW_HEIGHT;
        }
    }
/*
    if (icon) {
        ih = icon->getHeight();
    }
*/
    if (!(options & (ICON_ABOVE_TEXT|ICON_BELOW_TEXT)))  {
        h = FXMAX(th, ih); 
    } else {
        h = th + ih;
    }
    h = padtop + padbottom + (border<<1) + h;
    if ((options & MENUBUTTON_LEFT) && (options & MENUBUTTON_ATTACH_BOTTOM) && (options & MENUBUTTON_ATTACH_CENTER)) {
        if (myPane) { 
            ph = myPane->getDefaultHeight(); 
            if (ph>h) {
                h = ph; 
            }
        }
    }
    return h;
}


void 
FXMenuButtonIcon::killFocus() {
    FXLabel::killFocus();
    handle(this, FXSEL(SEL_COMMAND, ID_UNPOST), NULL);
}


bool 
FXMenuButtonIcon::contains(FXint parentx, FXint parenty) const {
    if (myPane && myPane->shown() && myPane->contains(parentx, parenty)) {
        return true;
    } else {
        return false;
    }
}


void 
FXMenuButtonIcon::setMenu(FXPopup *pup) {
    if (pup != myPane) {
        myPane = pup;
        recalc();
    }
}


FXPopup* 
FXMenuButtonIcon::getMenu() const { 
    return myPane; 
}


void 
FXMenuButtonIcon::setXOffset(FXint offx) {
    myoffsetX = offx; 
}


FXint 
FXMenuButtonIcon::getXOffset() const { 
    return myoffsetX; 
}


void
FXMenuButtonIcon::setYOffset(FXint offy) { 
    myOffsetY = offy; 
}


FXint 
FXMenuButtonIcon::getYOffset() const { 
    return myOffsetY; 
}


void 
FXMenuButtonIcon::setIcon(FXIcon* ic) {
    myIcon = ic;
}


void
FXMenuButtonIcon::setButtonStyle(FXuint style) {
    FXuint opts = (options & ~MENUBUTTON_MASK) | (style & MENUBUTTON_MASK);
    if (options != opts) {
        options = opts;
        update();
    }
}


FXuint
FXMenuButtonIcon::getButtonStyle() const {
    return (options & MENUBUTTON_MASK);
}


void 
FXMenuButtonIcon::setPopupStyle(FXuint style) {
    FXuint opts = (options & ~POPUP_MASK) | (style & POPUP_MASK);
    if (options != opts) {
        options = opts;
        update();
    }
}


FXuint
FXMenuButtonIcon::getPopupStyle() const {
    return (options & POPUP_MASK);
}


void 
FXMenuButtonIcon::setAttachment(FXuint att) {
    FXuint opts = (options & ~ATTACH_MASK) | (att&ATTACH_MASK);
    if (options != opts) {
        options = opts;
        update();
    }
}


FXuint 
FXMenuButtonIcon::getAttachment() const {
    return (options & ATTACH_MASK);
}


long
FXMenuButtonIcon::onUpdate(FXObject* sender, FXSelector sel, void* ptr) {
    if (!FXLabel::onUpdate(sender, sel, ptr)) {
        if (options & MENUBUTTON_AUTOHIDE) {
            if (shown()) {
                hide();recalc();
            }
        }
        if (options & MENUBUTTON_AUTOGRAY) {
            disable();
        }
    }
    return 1;
}


long 
FXMenuButtonIcon::onFocusIn(FXObject* sender, FXSelector sel, void* ptr) {
    FXLabel::onFocusIn(sender, sel, ptr);
    update(border, border, width - (border<<1), height - (border<<1));
    return 1;
}


long 
FXMenuButtonIcon::onFocusOut(FXObject* sender, FXSelector sel, void* ptr) {
    FXLabel::onFocusOut(sender, sel, ptr);
    update(border, border, width - (border<<1), height - (border<<1));
    return 1;
}


long 
FXMenuButtonIcon::onEnter(FXObject* sender, FXSelector sel, void* ptr) {
    FXLabel::onEnter(sender, sel, ptr);
    if (isEnabled()) {
        if (options & MENUBUTTON_TOOLBAR) {
            update();
        }
    }
    return 1;
}


long 
FXMenuButtonIcon::onLeave(FXObject* sender, FXSelector sel, void* ptr) {
    FXLabel::onLeave(sender, sel, ptr);
    if (isEnabled()) {
        if (options & MENUBUTTON_TOOLBAR) {
            update();
        }
    }
    return 1;
}


long
FXMenuButtonIcon::onLeftBtnPress(FXObject*, FXSelector, void* ptr) {
    flags &= ~FLAG_TIP;
    handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);
    if (isEnabled()) {
        if (target && target->tryHandle(this, FXSEL(SEL_LEFTBUTTONPRESS, message), ptr)) {
            return 1;
        }
        if (myState) {
            handle(this, FXSEL(SEL_COMMAND, ID_UNPOST), NULL);
        } else {
            handle(this, FXSEL(SEL_COMMAND, ID_POST), NULL);
        }
        return 1;
    }
    return 0;
}


long 
FXMenuButtonIcon::onLeftBtnRelease(FXObject*, FXSelector, void* ptr) {
    FXEvent* ev = (FXEvent*)ptr;
    if (isEnabled()) {
        if (target && target->tryHandle(this, FXSEL(SEL_LEFTBUTTONRELEASE, message), ptr)) {
            return 1;
        }
        if (ev->moved) { 
            handle(this, FXSEL(SEL_COMMAND, ID_UNPOST), NULL); 
        }
        return 1;
    }
    return 0;
}


long 
FXMenuButtonIcon::onMotion(FXObject*, FXSelector, void* ptr) {
    FXEvent* ev = (FXEvent*)ptr;
    if (myState) {
        if (myPane) {
            if (myPane->contains(ev->root_x, ev->root_y)) {
                if (grabbed()) {
                    ungrab();
                }
            } else {
                if (!grabbed()) {
                    grab();
                }
            }
            return 1;
        }
    }
    return 0;
}


long
FXMenuButtonIcon::onUngrabbed(FXObject* sender, FXSelector sel, void* ptr) {
    FXLabel::onUngrabbed(sender, sel, ptr);
    handle(this, FXSEL(SEL_COMMAND, ID_UNPOST), NULL);
    return 1;
}


long 
FXMenuButtonIcon::onKeyPress(FXObject*, FXSelector sel, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    flags &= ~FLAG_TIP;
    if (myPane && myPane->shown() && myPane->handle(myPane, sel, ptr)) {
        return 1;
    }
    if (isEnabled()) {
        if (target && target->tryHandle(this, FXSEL(SEL_KEYPRESS, message), ptr)) {
            return 1;
        }
        if (event->code == KEY_space || event->code == KEY_KP_Space) {
            if (myState) {
                handle(this, FXSEL(SEL_COMMAND, ID_UNPOST), NULL);
            } else {
                handle(this, FXSEL(SEL_COMMAND, ID_POST), NULL);
            }
            return 1;
        }
    }
    return 0;
}


long 
FXMenuButtonIcon::onKeyRelease(FXObject*, FXSelector sel, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    if (myPane && myPane->shown() && myPane->handle(myPane, sel, ptr)) {
        return 1;
    }
    if (isEnabled()) {
        if (target && target->tryHandle(this, FXSEL(SEL_KEYRELEASE, message), ptr)) {
            return 1;
        }
        if (event->code == KEY_space || event->code == KEY_KP_Space) {
            return 1;
        }
    }
    return 0;
}


long
FXMenuButtonIcon::onHotKeyPress(FXObject*, FXSelector, void* ptr) {
    flags &= ~FLAG_TIP;
    handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);
    if (isEnabled()) {
        if (myState) {
            handle(this, FXSEL(SEL_COMMAND, ID_UNPOST), NULL);
        } else {
            handle(this, FXSEL(SEL_COMMAND, ID_POST), NULL);
        }
    }
    return 1;
}



long 
FXMenuButtonIcon::onHotKeyRelease(FXObject*, FXSelector, void*) {
    return 1;
}


long 
FXMenuButtonIcon::onCmdPost(FXObject*, FXSelector, void*) {
    if (!myState) {
        if (myPane) {
            FXint x, y, w, h;
            translateCoordinatesTo(x, y, getRoot(), 0, 0);
            w = myPane->getShrinkWrap() ? myPane->getDefaultWidth() : myPane->getWidth();
            h = myPane->getShrinkWrap() ? myPane->getDefaultHeight() : myPane->getHeight();
            if ((options & MENUBUTTON_LEFT) && (options & MENUBUTTON_UP)) {   
                // Right
                if ((options & MENUBUTTON_ATTACH_BOTTOM) && (options & MENUBUTTON_ATTACH_CENTER)) {
                    h = height;
                } else if (options & MENUBUTTON_ATTACH_CENTER) {
                    y = y + (height - h)/2;
                } else if (options & MENUBUTTON_ATTACH_BOTTOM) {
                    y = y + height - h;
                }
                x = x + myoffsetX + width;
                y = y + myOffsetY;
            } else if (options & MENUBUTTON_LEFT) {   
                // Left
                if ((options & MENUBUTTON_ATTACH_BOTTOM) && (options & MENUBUTTON_ATTACH_CENTER)) {
                    h = height;
                } else if (options & MENUBUTTON_ATTACH_CENTER) {
                    y = y + (height - h)/2;
                } else if (options & MENUBUTTON_ATTACH_BOTTOM) {
                    y = y + height - h;
                }
                x = x - myoffsetX - w;
                y = y + myOffsetY;
            } else if (options & MENUBUTTON_UP) { 
                // Up
                if ((options & MENUBUTTON_ATTACH_RIGHT) && (options & MENUBUTTON_ATTACH_CENTER)) {
                    w = width;
                } else if (options & MENUBUTTON_ATTACH_CENTER) {
                    x = x + (width - w)/2;
                } else if (options & MENUBUTTON_ATTACH_RIGHT) {
                    x = x + width - w;
                }
                x = x + myoffsetX;
                y = y - myOffsetY - h;
            } else {
                // Down
                if ((options & MENUBUTTON_ATTACH_RIGHT) && (options & MENUBUTTON_ATTACH_CENTER)) {
                    w = width;
                } else if (options & MENUBUTTON_ATTACH_CENTER) {
                    x = x + (width - w)/2;
                } else if (options & MENUBUTTON_ATTACH_RIGHT) {
                    x = x + width - w;
                }
                x = x + myoffsetX;
                y = y + myOffsetY + height;
            }
            myPane->popup(this, x, y, w, h);
            if (!grabbed()) grab();
        }
        flags &= ~FLAG_UPDATE;
        myState = TRUE;
        update();
    }
    return 1;
}


long 
FXMenuButtonIcon::onCmdUnpost(FXObject*, FXSelector, void*) {
    if (myState) {
        if (myPane) {
            myPane->popdown();
            if (grabbed()) ungrab();
        }
        flags |= FLAG_UPDATE;
        myState = FALSE;
        update();
    }
    return 1;
}


long
FXMenuButtonIcon::onPaint(FXObject*, FXSelector, void* ptr) {
    FXint tw = 0, th = 0, iw = 0, ih = 0, tx, ty, ix, iy;
    FXEvent *ev = (FXEvent*)ptr;
    FXPoint points[3];
    FXDCWindow dc(this, ev);
    // Got a border at all?
    if (options & (FRAME_RAISED|FRAME_SUNKEN)) {
        // Toolbar style
        if (options & MENUBUTTON_TOOLBAR) {
            // Enabled and cursor inside,  and not popped up
            if (isEnabled() && underCursor() && !myState) {
                dc.setForeground(backColor);
                dc.fillRectangle(border, border, width - border*2, height - border*2);
                if (options & FRAME_THICK) {
                    drawDoubleRaisedRectangle(dc, 0, 0, width, height);
                } else {
                    drawRaisedRectangle(dc, 0, 0, width, height);
                }
            } else if (isEnabled() && myState) {    
                // Enabled and popped up
                dc.setForeground(hiliteColor);
                dc.fillRectangle(border, border, width - border*2, height - border*2);
                if (options & FRAME_THICK) {
                    drawDoubleSunkenRectangle(dc, 0, 0, width, height);
                } else {
                    drawSunkenRectangle(dc, 0, 0, width, height);
                }
            } else {
                // Disabled or unchecked or not under cursor
                dc.setForeground(backColor);
                dc.fillRectangle(0, 0, width, height);
            }
        } else {
            // Draw in up myState if disabled or up
            if (!isEnabled() || !myState) {
                dc.setForeground(backColor);
                dc.fillRectangle(border, border, width - border*2, height - border*2);
                if (options & FRAME_THICK) {
                    drawDoubleRaisedRectangle(dc, 0, 0, width, height);
                } else {
                    drawRaisedRectangle(dc, 0, 0, width, height);
                }
            } else {
                dc.setForeground(hiliteColor);
                dc.fillRectangle(border, border, width - border*2, height - border*2);
                if (options & FRAME_THICK) {
                    drawDoubleSunkenRectangle(dc, 0, 0, width, height);
                } else {
                    drawSunkenRectangle(dc, 0, 0, width, height);
                }
            }
        }
    } else {
        // No borders
        if (isEnabled() && myState) {
            dc.setForeground(hiliteColor);
            dc.fillRectangle(0, 0, width, height);
        } else {
            dc.setForeground(backColor);
            dc.fillRectangle(0, 0, width, height);
        }
    }
    // Position text & icon
    if (!label.empty()) {

    }
    // Arrows?
    if (!(options & MENUBUTTON_NOARROWS)) {
        if (options & MENUBUTTON_LEFT) {
            ih = MENUBUTTONARROW_WIDTH;
            iw = MENUBUTTONARROW_HEIGHT;
        } else {
            iw = MENUBUTTONARROW_WIDTH;
            ih = MENUBUTTONARROW_HEIGHT;
        }
    }
    // Keep some room for the arrow!
    just_x(tx, ix, tw, iw);
    just_y(ty, iy, th, ih);
    // Move a bit when pressed
    if (myState) { 
         ++tx; 
         ++ty; 
         ++ix; 
         ++iy; 
    }
    // draw arrows
    if (!(options & MENUBUTTON_NOARROWS)) {
        // Right arrow
        if ((options & MENUBUTTON_RIGHT) == MENUBUTTON_RIGHT) {
            if (isEnabled()) {
                dc.setForeground(textColor);
            } else {
                dc.setForeground(shadowColor);
            }
            points[0].x = (FXshort)ix;
            points[0].y = (FXshort)iy;
            points[1].x = (FXshort)ix;
            points[1].y = (FXshort)(iy + MENUBUTTONARROW_WIDTH - 1);
            points[2].x = (FXshort)(ix + MENUBUTTONARROW_HEIGHT);
            points[2].y = (FXshort)(iy + (MENUBUTTONARROW_WIDTH >> 1));
            dc.fillPolygon(points, 3);
        } else if (options & MENUBUTTON_LEFT) {
            // Left arrow
            if (isEnabled()) {
                dc.setForeground(textColor);
            } else {
                dc.setForeground(shadowColor);
            }
            points[0].x = (FXshort)(ix + MENUBUTTONARROW_HEIGHT);
            points[0].y = (FXshort)iy;
            points[1].x = (FXshort)(ix + MENUBUTTONARROW_HEIGHT);
            points[1].y = (FXshort)(iy + MENUBUTTONARROW_WIDTH - 1);
            points[2].x = (FXshort)ix;
            points[2].y = (FXshort)(iy + (MENUBUTTONARROW_WIDTH >> 1));
            dc.fillPolygon(points, 3);
        } else if (options & MENUBUTTON_UP) {
            // Up arrow
            if (isEnabled()) {
                dc.setForeground(textColor);
            } else {
                dc.setForeground(shadowColor);
            }
            points[0].x = (FXshort)(ix + (MENUBUTTONARROW_WIDTH >> 1));
            points[0].y = (FXshort)(iy - 1);
            points[1].x = (FXshort)ix;
            points[1].y = (FXshort)(iy + MENUBUTTONARROW_HEIGHT);
            points[2].x = (FXshort)(ix + MENUBUTTONARROW_WIDTH);
            points[2].y = (FXshort)(iy + MENUBUTTONARROW_HEIGHT);
            dc.fillPolygon(points, 3);
        } else {
            // Down arrow
            if (isEnabled()) {
                dc.setForeground(textColor);
            } else {
                dc.setForeground(shadowColor);
            }
            points[0].x = (FXshort)(ix + 1);
            points[0].y = (FXshort)iy;
            points[2].x = (FXshort)(ix + MENUBUTTONARROW_WIDTH - 1);
            points[2].y = (FXshort)iy;
            points[1].x = (FXshort)(ix + (MENUBUTTONARROW_WIDTH >> 1));
            points[1].y = (FXshort)(iy + MENUBUTTONARROW_HEIGHT);
            dc.fillPolygon(points, 3);
        }
    }
    // draw icon
    if (myIcon) {
        if (isEnabled()) {
            dc.drawIcon(myIcon, tx, ty);
        } else {
            dc.drawIconSunken(myIcon, tx, ty);
        }
    }
    // Draw text
    if (!label.empty()) {
        dc.setFont(font);
        if (isEnabled()) {
            dc.setForeground(textColor);
            drawLabel(dc, label, hotoff, tx, ty, tw, th);
        } else {
            dc.setForeground(hiliteColor);
            drawLabel(dc, label, hotoff, tx + 1, ty + 1, tw, th);
            dc.setForeground(shadowColor);
            drawLabel(dc, label, hotoff, tx, ty, tw, th);
        }
    }
    // Draw focus
    if (hasFocus()) {
        if (isEnabled()) {
            dc.drawFocusRectangle(border + 1, border + 1, width - 2*border - 2, height - 2*border - 2);
        }
    }
    return 1;
}


FXMenuButtonIcon::FXMenuButtonIcon() {
    myPane = (FXPopup*) - 1L;
    myoffsetX = 0;
    myOffsetY = 0;
    myState = FALSE;
}