/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXMenuCheckIcon.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2021
///
//
/****************************************************************************/

#include <fxkeys.h>

#include "MFXMenuCheckIcon.h"


#define LEADSPACE   22
#define TRAILSPACE  16

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXMenuCheckIcon) MFXMenuCheckIconMap[] = {
    FXMAPFUNC(SEL_PAINT,                0,                          MFXMenuCheckIcon::onPaint),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,      0,                          MFXMenuCheckIcon::onButtonPress),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE,    0,                          MFXMenuCheckIcon::onButtonRelease),
    FXMAPFUNC(SEL_MIDDLEBUTTONPRESS,    0,                          MFXMenuCheckIcon::onButtonPress),
    FXMAPFUNC(SEL_MIDDLEBUTTONRELEASE,  0,                          MFXMenuCheckIcon::onButtonRelease),
    FXMAPFUNC(SEL_RIGHTBUTTONPRESS,     0,                          MFXMenuCheckIcon::onButtonPress),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   0,                          MFXMenuCheckIcon::onButtonRelease),
    FXMAPFUNC(SEL_KEYPRESS,             0,                          MFXMenuCheckIcon::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,           0,                          MFXMenuCheckIcon::onKeyRelease),
    FXMAPFUNC(SEL_KEYPRESS,             FXWindow::ID_HOTKEY,        MFXMenuCheckIcon::onHotKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,           FXWindow::ID_HOTKEY,        MFXMenuCheckIcon::onHotKeyRelease),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_CHECK,         MFXMenuCheckIcon::onCheck),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_UNCHECK,       MFXMenuCheckIcon::onUncheck),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_UNKNOWN,       MFXMenuCheckIcon::onUnknown),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_SETVALUE,      MFXMenuCheckIcon::onCmdSetValue),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_SETINTVALUE,   MFXMenuCheckIcon::onCmdSetIntValue),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_GETINTVALUE,   MFXMenuCheckIcon::onCmdGetIntValue),
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_ACCEL,         MFXMenuCheckIcon::onCmdAccel),
};

// Object implementation
FXIMPLEMENT(MFXMenuCheckIcon, FXMenuCommand, MFXMenuCheckIconMap, ARRAYNUMBER(MFXMenuCheckIconMap))

// ===========================================================================
// member method definitions
// ===========================================================================

MFXMenuCheckIcon::MFXMenuCheckIcon(FXComposite* p, const std::string& text, const std::string& shortcut, const std::string& info, const FXIcon* icon, FXObject* tgt, FXSelector sel, FXuint opts) :
    FXMenuCommand(p, (text + "\t" + shortcut + "\t" + info).c_str(), NULL, tgt, sel, opts),
    myIcon(icon),
    myCheck(FALSE),
    myBoxColor(getApp()->getBackColor()) {
}


FXint
MFXMenuCheckIcon::getDefaultWidth() {
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
MFXMenuCheckIcon::getDefaultHeight() {
    FXint th = 0;
    if (!label.empty() || !accel.empty()) {
        th = font->getFontHeight() + 5;
    }
    return FXMAX(th, 20);
}


void
MFXMenuCheckIcon::setCheck(FXbool s) {
    if (myCheck != s) {
        myCheck = s;
        update();
    }
}


FXbool
MFXMenuCheckIcon::getCheck() const {
    return myCheck;
}


FXColor
MFXMenuCheckIcon::getBoxColor() const {
    return myBoxColor;
}


long
MFXMenuCheckIcon::onCheck(FXObject*, FXSelector, void*) {
    setCheck(TRUE);
    return 1;
}


long
MFXMenuCheckIcon::onUncheck(FXObject*, FXSelector, void*) {
    setCheck(FALSE);
    return 1;
}


long
MFXMenuCheckIcon::onUnknown(FXObject*, FXSelector, void*) {
    setCheck(MAYBE);
    return 1;
}


long
MFXMenuCheckIcon::onCmdSetValue(FXObject*, FXSelector, void* ptr) {
    setCheck((FXbool)(FXuval)ptr);
    return 1;
}


long
MFXMenuCheckIcon::onCmdSetIntValue(FXObject*, FXSelector, void* ptr) {
    setCheck((FXbool) * ((FXint*)ptr));
    return 1;
}


long
MFXMenuCheckIcon::onCmdGetIntValue(FXObject*, FXSelector, void* ptr) {
    *((FXint*)ptr) = getCheck();
    return 1;
}


long
MFXMenuCheckIcon::onButtonPress(FXObject*, FXSelector, void*) {
    if (!isEnabled()) {
        return 0;
    }
    return 1;
}


long
MFXMenuCheckIcon::onButtonRelease(FXObject*, FXSelector, void*) {
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
MFXMenuCheckIcon::onKeyPress(FXObject*, FXSelector, void* ptr) {
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
MFXMenuCheckIcon::onKeyRelease(FXObject*, FXSelector, void* ptr) {
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
MFXMenuCheckIcon::onHotKeyPress(FXObject*, FXSelector, void* ptr) {
    FXTRACE((200, "%s::onHotKeyPress %p\n", getClassName(), (void*)this));
    handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);
    if (isEnabled() && !(flags & FLAG_PRESSED)) {
        flags |= FLAG_PRESSED;
    }
    return 1;
}


long
MFXMenuCheckIcon::onHotKeyRelease(FXObject*, FXSelector, void*) {
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
MFXMenuCheckIcon::onCmdAccel(FXObject*, FXSelector, void*) {
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
MFXMenuCheckIcon::onPaint(FXObject*, FXSelector, void* ptr) {
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
MFXMenuCheckIcon::setBoxColor(FXColor clr) {
    if (clr != myBoxColor) {
        myBoxColor = clr;
        update();
    }
}


void
MFXMenuCheckIcon::save(FXStream& store) const {
    FXMenuCommand::save(store);
    store << myCheck;
    store << myBoxColor;
}


void MFXMenuCheckIcon::load(FXStream& store) {
    FXMenuCommand::load(store);
    store >> myCheck;
    store >> myBoxColor;
}


MFXMenuCheckIcon::MFXMenuCheckIcon() :
    myIcon(nullptr),
    myCheck(FALSE),
    myBoxColor(0) {
}
