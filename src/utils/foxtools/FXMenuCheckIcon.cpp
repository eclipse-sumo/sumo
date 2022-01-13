/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2022 German Aerospace Center (DLR) and others.
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
/// @file    FXMenuCheckIcon.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2021
///
//
/****************************************************************************/

#include <fxkeys.h>

#include "FXMenuCheckIcon.h"


#define LEADSPACE   22
#define TRAILSPACE  16

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(FXMenuCheckIcon) FXMenuCheckIconMap[] = {
    FXMAPFUNC(SEL_PAINT,                0,                          FXMenuCheckIcon::onPaint),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,      0,                          FXMenuCheckIcon::onButtonPress),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE,    0,                          FXMenuCheckIcon::onButtonRelease),
    FXMAPFUNC(SEL_MIDDLEBUTTONPRESS,    0,                          FXMenuCheckIcon::onButtonPress),
    FXMAPFUNC(SEL_MIDDLEBUTTONRELEASE,  0,                          FXMenuCheckIcon::onButtonRelease),
    FXMAPFUNC(SEL_RIGHTBUTTONPRESS,     0,                          FXMenuCheckIcon::onButtonPress),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   0,                          FXMenuCheckIcon::onButtonRelease),
    FXMAPFUNC(SEL_KEYPRESS,             0,                          FXMenuCheckIcon::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,           0,                          FXMenuCheckIcon::onKeyRelease),
    FXMAPFUNC(SEL_KEYPRESS,             FXWindow::ID_HOTKEY,        FXMenuCheckIcon::onHotKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,           FXWindow::ID_HOTKEY,        FXMenuCheckIcon::onHotKeyRelease),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_CHECK,         FXMenuCheckIcon::onCheck),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_UNCHECK,       FXMenuCheckIcon::onUncheck),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_UNKNOWN,       FXMenuCheckIcon::onUnknown),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_SETVALUE,      FXMenuCheckIcon::onCmdSetValue),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_SETINTVALUE,   FXMenuCheckIcon::onCmdSetIntValue),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_GETINTVALUE,   FXMenuCheckIcon::onCmdGetIntValue),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_ACCEL,         FXMenuCheckIcon::onCmdAccel),
};

// Object implementation
FXIMPLEMENT(FXMenuCheckIcon, FXMenuCommand, FXMenuCheckIconMap, ARRAYNUMBER(FXMenuCheckIconMap))

// ===========================================================================
// member method definitions
// ===========================================================================

FXMenuCheckIcon::FXMenuCheckIcon(FXComposite* p, const FXString& text, const FXIcon* icon, FXObject* tgt, FXSelector sel, FXuint opts) :
    FXMenuCommand(p, text, NULL, tgt, sel, opts),
    myIcon(icon),
    myCheck(FALSE),
    myBoxColor(getApp()->getBackColor()) {
}


FXint
FXMenuCheckIcon::getDefaultWidth() {
    FXint tw, aw;
    tw = aw = 0;
    if (!label.empty()) {
        tw = font->getTextWidth(label.text(), label.length());
    }
    if (!accel.empty()) {
        aw = font->getTextWidth(accel.text(), accel.length());
    }
    if (aw && tw) {
        aw += 5;
    }
    // return width depending of icon
    if (myIcon) {
        return LEADSPACE + (myIcon->getWidth() + 5) + tw + aw + TRAILSPACE;
    } else {
        return LEADSPACE + tw + aw + TRAILSPACE;
    }
}


FXint
FXMenuCheckIcon::getDefaultHeight() {
    FXint th = 0;
    if (!label.empty() || !accel.empty()) {
        th = font->getFontHeight() + 5;
    }
    return FXMAX(th, 20);
}


void
FXMenuCheckIcon::setCheck(FXbool s) {
    if (myCheck != s) {
        myCheck = s;
        update();
    }
}


FXbool
FXMenuCheckIcon::getCheck() const {
    return myCheck;
}


FXColor
FXMenuCheckIcon::getBoxColor() const {
    return myBoxColor;
}


long
FXMenuCheckIcon::onCheck(FXObject*, FXSelector, void*) {
    setCheck(TRUE);
    return 1;
}


long
FXMenuCheckIcon::onUncheck(FXObject*, FXSelector, void*) {
    setCheck(FALSE);
    return 1;
}


long
FXMenuCheckIcon::onUnknown(FXObject*, FXSelector, void*) {
    setCheck(MAYBE);
    return 1;
}


long
FXMenuCheckIcon::onCmdSetValue(FXObject*, FXSelector, void* ptr) {
    setCheck((FXbool)(FXuval)ptr);
    return 1;
}


long
FXMenuCheckIcon::onCmdSetIntValue(FXObject*, FXSelector, void* ptr) {
    setCheck((FXbool) * ((FXint*)ptr));
    return 1;
}


long
FXMenuCheckIcon::onCmdGetIntValue(FXObject*, FXSelector, void* ptr) {
    *((FXint*)ptr) = getCheck();
    return 1;
}


long
FXMenuCheckIcon::onButtonPress(FXObject*, FXSelector, void*) {
    if (!isEnabled()) {
        return 0;
    }
    return 1;
}


long
FXMenuCheckIcon::onButtonRelease(FXObject*, FXSelector, void*) {
    FXbool active = isActive();
    if (!isEnabled()) {
        return 0;
    }
    getParent()->handle(this, FXSEL(SEL_COMMAND, ID_UNPOST), NULL);
    if (active) {
        setCheck(!myCheck);
        if (target) {
            target->tryHandle(this, FXSEL(SEL_COMMAND, message), (void*)(FXuval)myCheck);
        }
    }
    return 1;
}


long
FXMenuCheckIcon::onKeyPress(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    if (isEnabled() && !(flags & FLAG_PRESSED)) {
        FXTRACE((200, "%s::onKeyPress %p keysym = 0x%04x state = %04x\n", getClassName(), (void*)this, event->code, event->state));
        if (event->code == FX::KEY_space || event->code == FX::KEY_KP_Space || event->code == FX::KEY_Return || event->code == FX::KEY_KP_Enter) {
            flags |= FLAG_PRESSED;
            return 1;
        }
    }
    return 0;
}


long
FXMenuCheckIcon::onKeyRelease(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    if (isEnabled() && (flags & FLAG_PRESSED)) {
        FXTRACE((200, "%s::onKeyRelease %p keysym = 0x%04x state = %04x\n", getClassName(), (void*)this, event->code, event->state));
        if (event->code == FX::KEY_space || event->code == FX::KEY_KP_Space || event->code == FX::KEY_Return || event->code == FX::KEY_KP_Enter) {
            flags &= ~FLAG_PRESSED;
            setCheck(!myCheck);
            getParent()->handle(this, FXSEL(SEL_COMMAND, ID_UNPOST), NULL);
            if (target) {
                target->tryHandle(this, FXSEL(SEL_COMMAND, message), (void*)(FXuval)myCheck);
            }
            return 1;
        }
    }
    return 0;
}


long
FXMenuCheckIcon::onHotKeyPress(FXObject*, FXSelector, void* ptr) {
    FXTRACE((200, "%s::onHotKeyPress %p\n", getClassName(), (void*)this));
    handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);
    if (isEnabled() && !(flags & FLAG_PRESSED)) {
        flags |= FLAG_PRESSED;
    }
    return 1;
}


long
FXMenuCheckIcon::onHotKeyRelease(FXObject*, FXSelector, void*) {
    FXTRACE((200, "%s::onHotKeyRelease %p\n", getClassName(), (void*)this));
    if (isEnabled() && (flags & FLAG_PRESSED)) {
        flags &= ~FLAG_PRESSED;
        setCheck(!myCheck);
        getParent()->handle(this, FXSEL(SEL_COMMAND, ID_UNPOST), NULL);
        if (target) {
            target->tryHandle(this, FXSEL(SEL_COMMAND, message), (void*)(FXuval)myCheck);
        }
    }
    return 1;
}


long
FXMenuCheckIcon::onCmdAccel(FXObject*, FXSelector, void*) {
    if (isEnabled()) {
        setCheck(!myCheck);
        if (target) {
            target->tryHandle(this, FXSEL(SEL_COMMAND, message), (void*)(FXuval)myCheck);
        }
        return 1;
    }
    return 0;
}


long
FXMenuCheckIcon::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent* ev = (FXEvent*)ptr;
    FXDCWindow dc(this, ev);
    FXint xx, yy;
    // set xx depending of myIcon
    if (myIcon) {
        xx = LEADSPACE + myIcon->getWidth() + 5;
    } else {
        xx = LEADSPACE;
    }
    // begin draw
    if (!isEnabled()) {
        // Grayed out
        dc.setForeground(backColor);
        dc.fillRectangle(0, 0, width, height);
        if (!label.empty()) {
            yy = font->getFontAscent() + (height - font->getFontHeight()) / 2;
            dc.setFont(font);
            dc.setForeground(hiliteColor);
            dc.drawText(xx + 1, yy + 1, label);
            if (!accel.empty()) {
                dc.drawText(width - TRAILSPACE - font->getTextWidth(accel) + 1, yy + 1, accel);
            }
            if (0 <= hotoff) {
                dc.fillRectangle(xx + font->getTextWidth(&label[0], hotoff) + 1, yy + 2, font->getTextWidth(&label[hotoff], wclen(&label[hotoff])), 1);
            }
            dc.setForeground(shadowColor);
            dc.drawText(xx, yy, label);
            if (!accel.empty()) {
                dc.drawText(width - TRAILSPACE - font->getTextWidth(accel), yy, accel);
            }
            if (0 <= hotoff) {
                dc.fillRectangle(xx + font->getTextWidth(&label[0], hotoff), yy + 1, font->getTextWidth(&label[hotoff], wclen(&label[hotoff])), 1);
            }
        }
    } else if (isActive()) {
        // Active
        dc.setForeground(selbackColor);
        dc.fillRectangle(0, 0, width, height);
        if (!label.empty()) {
            yy = font->getFontAscent() + (height - font->getFontHeight()) / 2;
            dc.setFont(font);
            dc.setForeground(isEnabled() ? seltextColor : shadowColor);
            dc.drawText(xx, yy, label);
            if (!accel.empty()) {
                dc.drawText(width - TRAILSPACE - font->getTextWidth(accel), yy, accel);
            }
            if (0 <= hotoff) {
                dc.fillRectangle(xx + font->getTextWidth(&label[0], hotoff), yy + 1, font->getTextWidth(&label[hotoff], wclen(&label[hotoff])), 1);
            }
        }
    } else {
        // Normal
        dc.setForeground(backColor);
        dc.fillRectangle(0, 0, width, height);
        if (!label.empty()) {
            yy = font->getFontAscent() + (height - font->getFontHeight()) / 2;
            dc.setFont(font);
            dc.setForeground(textColor);
            dc.drawText(xx, yy, label);
            if (!accel.empty()) {
                dc.drawText(width - TRAILSPACE - font->getTextWidth(accel), yy, accel);
            }
            if (0 <= hotoff) {
                dc.fillRectangle(xx + font->getTextWidth(&label[0], hotoff), yy + 1, font->getTextWidth(&label[hotoff], wclen(&label[hotoff])), 1);
            }
        }
    }
    // Draw the box
    xx = 5;
    yy = (height - 9) / 2;
    if (!isEnabled()) {
        dc.setForeground(backColor);
    } else {
        dc.setForeground(myBoxColor);
        dc.fillRectangle(xx + 1, yy + 1, 8, 8);
        dc.setForeground(shadowColor);
        dc.drawRectangle(xx, yy, 9, 9);
    }
    // Draw the check (tick)
    if (myCheck != FALSE) {
        FXSegment seg[6];
        seg[0].x1 = 2 + (FXshort)xx;
        seg[0].y1 = 4 + (FXshort)yy;
        seg[0].x2 = 4 + (FXshort)xx;
        seg[0].y2 = 6 + (FXshort)yy;
        seg[1].x1 = 2 + (FXshort)xx;
        seg[1].y1 = 5 + (FXshort)yy;
        seg[1].x2 = 4 + (FXshort)xx;
        seg[1].y2 = 7 + (FXshort)yy;
        seg[2].x1 = 2 + (FXshort)xx;
        seg[2].y1 = 6 + (FXshort)yy;
        seg[2].x2 = 4 + (FXshort)xx;
        seg[2].y2 = 8 + (FXshort)yy;
        seg[3].x1 = 4 + (FXshort)xx;
        seg[3].y1 = 6 + (FXshort)yy;
        seg[3].x2 = 8 + (FXshort)xx;
        seg[3].y2 = 2 + (FXshort)yy;
        seg[4].x1 = 4 + (FXshort)xx;
        seg[4].y1 = 7 + (FXshort)yy;
        seg[4].x2 = 8 + (FXshort)xx;
        seg[4].y2 = 3 + (FXshort)yy;
        seg[5].x1 = 4 + (FXshort)xx;
        seg[5].y1 = 8 + (FXshort)yy;
        seg[5].x2 = 8 + (FXshort)xx;
        seg[5].y2 = 4 + (FXshort)yy;
        if (isEnabled()) {
            if (myCheck == MAYBE) {
                dc.setForeground(shadowColor);
            } else {
                dc.setForeground(textColor);
            }
        } else {
            dc.setForeground(shadowColor);
        }
        dc.drawLineSegments(seg, 6);
    }
    // draw icon
    if (myIcon) {
        if (isEnabled()) {
            dc.drawIcon(myIcon, LEADSPACE, (height - myIcon->getHeight()) / 2);
            xx += 5 + myIcon->getWidth();
        } else {
            dc.drawIconSunken(myIcon, LEADSPACE, (height - myIcon->getHeight()) / 2);
            xx += 5 + myIcon->getWidth();
        }
    }
    return 1;
}


void
FXMenuCheckIcon::setBoxColor(FXColor clr) {
    if (clr != myBoxColor) {
        myBoxColor = clr;
        update();
    }
}


void
FXMenuCheckIcon::save(FXStream& store) const {
    FXMenuCommand::save(store);
    store << myCheck;
    store << myBoxColor;
}


void FXMenuCheckIcon::load(FXStream& store) {
    FXMenuCommand::load(store);
    store >> myCheck;
    store >> myBoxColor;
}


FXMenuCheckIcon::FXMenuCheckIcon() :
    myIcon(nullptr),
    myCheck(FALSE),
    myBoxColor(0) {
}
