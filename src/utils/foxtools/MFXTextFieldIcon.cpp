/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2024 German Aerospace Center (DLR) and others.
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
/// @date    Aug 2023
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================


#include <utils/common/StdDefs.h>
#include "MFXTextFieldIcon.h"

#include <fxkeys.h>
#include <FX88591Codec.h>
#include <FX88591Codec.h>
#include <FXCP1252Codec.h>
#include <FXUTF16Codec.h>


// ===========================================================================
// Macros
// ===========================================================================

#define JUSTIFY_MASK    (JUSTIFY_HZ_APART|JUSTIFY_VT_APART)
#define TEXTFIELD_MASK  (TEXTFIELD_PASSWD|TEXTFIELD_INTEGER|TEXTFIELD_REAL|TEXTFIELD_READONLY|TEXTFIELD_ENTER_ONLY|TEXTFIELD_LIMITED|TEXTFIELD_OVERSTRIKE|TEXTFIELD_AUTOHIDE|TEXTFIELD_AUTOGRAY)
#define ICON_SPACING    4   // Spacing between icon and label (2 + 2)
#define ICON_SIZE       16

// ===========================================================================
// FOX callback mapping
// ===========================================================================

// Map
FXDEFMAP(MFXTextFieldIcon) MFXTextFieldIconMap[] = {
    FXMAPFUNC(SEL_PAINT,                0,                                  MFXTextFieldIcon::onPaint),
    FXMAPFUNC(SEL_UPDATE,               0,                                  MFXTextFieldIcon::onUpdate),
    FXMAPFUNC(SEL_MOTION,               0,                                  MFXTextFieldIcon::onMotion),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,      0,                                  MFXTextFieldIcon::onLeftBtnPress),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE,    0,                                  MFXTextFieldIcon::onLeftBtnRelease),
    FXMAPFUNC(SEL_MIDDLEBUTTONPRESS,    0,                                  MFXTextFieldIcon::onMiddleBtnPress),
    FXMAPFUNC(SEL_MIDDLEBUTTONRELEASE,  0,                                  MFXTextFieldIcon::onMiddleBtnRelease),
    FXMAPFUNC(SEL_KEYPRESS,             0,                                  MFXTextFieldIcon::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,           0,                                  MFXTextFieldIcon::onKeyRelease),
    FXMAPFUNC(SEL_VERIFY,               0,                                  MFXTextFieldIcon::onVerify),
    FXMAPFUNC(SEL_SELECTION_LOST,       0,                                  MFXTextFieldIcon::onSelectionLost),
    FXMAPFUNC(SEL_SELECTION_GAINED,     0,                                  MFXTextFieldIcon::onSelectionGained),
    FXMAPFUNC(SEL_SELECTION_REQUEST,    0,                                  MFXTextFieldIcon::onSelectionRequest),
    FXMAPFUNC(SEL_CLIPBOARD_LOST,       0,                                  MFXTextFieldIcon::onClipboardLost),
    FXMAPFUNC(SEL_CLIPBOARD_GAINED,     0,                                  MFXTextFieldIcon::onClipboardGained),
    FXMAPFUNC(SEL_CLIPBOARD_REQUEST,    0,                                  MFXTextFieldIcon::onClipboardRequest),
    FXMAPFUNC(SEL_FOCUSIN,              0,                                  MFXTextFieldIcon::onFocusIn),
    FXMAPFUNC(SEL_FOCUSOUT,             0,                                  MFXTextFieldIcon::onFocusOut),
    FXMAPFUNC(SEL_FOCUS_SELF,           0,                                  MFXTextFieldIcon::onFocusSelf),
    FXMAPFUNC(SEL_QUERY_TIP,            0,                                  MFXTextFieldIcon::onQueryTip),
    FXMAPFUNC(SEL_QUERY_HELP,           0,                                  MFXTextFieldIcon::onQueryHelp),
    FXMAPFUNC(SEL_TIMEOUT,              FXTextField::ID_BLINK,              MFXTextFieldIcon::onBlink),
    FXMAPFUNC(SEL_TIMEOUT,              FXTextField::ID_AUTOSCROLL,         MFXTextFieldIcon::onAutoScroll),
    FXMAPFUNC(SEL_UPDATE,               FXTextField::ID_TOGGLE_EDITABLE,    MFXTextFieldIcon::onUpdToggleEditable),
    FXMAPFUNC(SEL_UPDATE,               FXTextField::ID_TOGGLE_OVERSTRIKE,  MFXTextFieldIcon::onUpdToggleOverstrike),
    FXMAPFUNC(SEL_UPDATE,               FXTextField::ID_CUT_SEL,            MFXTextFieldIcon::onUpdHaveSelection),
    FXMAPFUNC(SEL_UPDATE,               FXTextField::ID_COPY_SEL,           MFXTextFieldIcon::onUpdHaveSelection),
    FXMAPFUNC(SEL_UPDATE,               FXTextField::ID_PASTE_SEL,          MFXTextFieldIcon::onUpdYes),
    FXMAPFUNC(SEL_UPDATE,               FXTextField::ID_DELETE_SEL,         MFXTextFieldIcon::onUpdHaveSelection),
    FXMAPFUNC(SEL_UPDATE,               FXTextField::ID_SELECT_ALL,         MFXTextFieldIcon::onUpdSelectAll),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_SETVALUE,           MFXTextFieldIcon::onCmdSetValue),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_SETINTVALUE,        MFXTextFieldIcon::onCmdSetIntValue),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_SETREALVALUE,       MFXTextFieldIcon::onCmdSetRealValue),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_SETSTRINGVALUE,     MFXTextFieldIcon::onCmdSetStringValue),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_GETINTVALUE,        MFXTextFieldIcon::onCmdGetIntValue),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_GETREALVALUE,       MFXTextFieldIcon::onCmdGetRealValue),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_GETSTRINGVALUE,     MFXTextFieldIcon::onCmdGetStringValue),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_CURSOR_HOME,        MFXTextFieldIcon::onCmdCursorHome),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_CURSOR_END,         MFXTextFieldIcon::onCmdCursorEnd),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_CURSOR_RIGHT,       MFXTextFieldIcon::onCmdCursorRight),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_CURSOR_LEFT,        MFXTextFieldIcon::onCmdCursorLeft),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_CURSOR_WORD_LEFT,   MFXTextFieldIcon::onCmdCursorWordLeft),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_CURSOR_WORD_RIGHT,  MFXTextFieldIcon::onCmdCursorWordRight),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_CURSOR_WORD_START,  MFXTextFieldIcon::onCmdCursorWordStart),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_CURSOR_WORD_END,    MFXTextFieldIcon::onCmdCursorWordEnd),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_MARK,               MFXTextFieldIcon::onCmdMark),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_EXTEND,             MFXTextFieldIcon::onCmdExtend),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_SELECT_ALL,         MFXTextFieldIcon::onCmdSelectAll),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_DESELECT_ALL,       MFXTextFieldIcon::onCmdDeselectAll),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_CUT_SEL,            MFXTextFieldIcon::onCmdCutSel),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_COPY_SEL,           MFXTextFieldIcon::onCmdCopySel),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_PASTE_SEL,          MFXTextFieldIcon::onCmdPasteSel),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_PASTE_MIDDLE,       MFXTextFieldIcon::onCmdPasteMiddle),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_DELETE_SEL,         MFXTextFieldIcon::onCmdDeleteSel),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_DELETE_ALL,         MFXTextFieldIcon::onCmdDeleteAll),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_OVERST_STRING,      MFXTextFieldIcon::onCmdOverstString),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_INSERT_STRING,      MFXTextFieldIcon::onCmdInsertString),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_BACKSPACE,          MFXTextFieldIcon::onCmdBackspace),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_DELETE,             MFXTextFieldIcon::onCmdDelete),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_TOGGLE_EDITABLE,    MFXTextFieldIcon::onCmdToggleEditable),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_TOGGLE_OVERSTRIKE,  MFXTextFieldIcon::onCmdToggleOverstrike),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_SETHELPSTRING,      MFXTextFieldIcon::onCmdSetHelp),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_GETHELPSTRING,      MFXTextFieldIcon::onCmdGetHelp),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_SETTIPSTRING,       MFXTextFieldIcon::onCmdSetTip),
    FXMAPFUNC(SEL_COMMAND,              FXTextField::ID_GETTIPSTRING,       MFXTextFieldIcon::onCmdGetTip),
};

// Object implementation
FXIMPLEMENT(MFXTextFieldIcon, FXFrame, MFXTextFieldIconMap, ARRAYNUMBER(MFXTextFieldIconMap))

// ===========================================================================
// member method definitions
// ===========================================================================

MFXTextFieldIcon::MFXTextFieldIcon(FXComposite* p, FXint ncols, FXIcon* ic, FXObject* tgt, FXSelector sel,
                                   FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) :
    FXFrame(p, opts, x, y, w, h, pl, pr, pt, pb),
    icon(ic) {
    if (ncols < 0) {
        ncols = 0;
    }
    flags |= FLAG_ENABLED;
    target = tgt;
    message = sel;
    if (!(options & JUSTIFY_RIGHT)) {
        options |= JUSTIFY_LEFT;
    }
    defaultCursor = getApp()->getDefaultCursor(DEF_TEXT_CURSOR);
    dragCursor = getApp()->getDefaultCursor(DEF_TEXT_CURSOR);
    font = getApp()->getNormalFont();
    backColor = getApp()->getBackColor();
    textColor = getApp()->getForeColor();
    selbackColor = getApp()->getSelbackColor();
    seltextColor = getApp()->getSelforeColor();
    cursorColor = getApp()->getForeColor();
    columns = ncols;
}


MFXTextFieldIcon::~MFXTextFieldIcon() {
    getApp()->removeTimeout(this, FXTextField::ID_BLINK);
    getApp()->removeTimeout(this, ID_AUTOSCROLL);
    font = (FXFont*) - 1L;
}


void
MFXTextFieldIcon::create() {
    FXFrame::create();
    if (!textType) {
        textType = getApp()->registerDragType(textTypeName);
    }
    if (!utf8Type) {
        utf8Type = getApp()->registerDragType(utf8TypeName);
    }
    if (!utf16Type) {
        utf16Type = getApp()->registerDragType(utf16TypeName);
    }
    font->create();
}


void
MFXTextFieldIcon::setFont(FXFont* fnt) {
    if (!fnt) {
        fxerror("%s::setFont: NULL font specified.\n", getClassName());
    }
    if (font != fnt) {
        font = fnt;
        recalc();
        update();
    }
}


void
MFXTextFieldIcon::enable() {
    if (!(flags & FLAG_ENABLED)) {
        FXFrame::enable();
        update();
    }
}


void
MFXTextFieldIcon::disable() {
    if (flags & FLAG_ENABLED) {
        FXFrame::disable();
        update();
    }
}


FXint
MFXTextFieldIcon::getDefaultWidth() {
    return padleft + padright + (border << 1) + columns * font->getTextWidth("8", 1);
}



FXint
MFXTextFieldIcon::getDefaultHeight() {
    return padtop + padbottom + (border << 1) + font->getFontHeight();
}


long
MFXTextFieldIcon::onUpdate(FXObject* sender, FXSelector sel, void* ptr) {
    if (!FXFrame::onUpdate(sender, sel, ptr)) {
        if (options & TEXTFIELD_AUTOHIDE) {
            if (shown()) {
                hide();
                recalc();
            }
        }
        if (options & TEXTFIELD_AUTOGRAY) {
            disable();
        }
    }
    return 1;
}


long
MFXTextFieldIcon::onSelectionGained(FXObject* sender, FXSelector sel, void* ptr) {
    FXFrame::onSelectionGained(sender, sel, ptr);
    update();
    return 1;
}


long
MFXTextFieldIcon::onSelectionLost(FXObject* sender, FXSelector sel, void* ptr) {
    FXFrame::onSelectionLost(sender, sel, ptr);
    update();
    return 1;
}


long
MFXTextFieldIcon::onSelectionRequest(FXObject* sender, FXSelector sel, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXString string;
    FXuint   start;
    FXuint   len;

    // Make sure
    FXASSERT(0 <= anchor && anchor <= contents.length());
    FXASSERT(0 <= cursor && cursor <= contents.length());

    // Perhaps the target wants to supply its own data for the selection
    if (FXFrame::onSelectionRequest(sender, sel, ptr)) {
        return 1;
    }

    // Recognize the request?
    if (event->target == stringType || event->target == textType || event->target == utf8Type || event->target == utf16Type) {

        // Figure selected bytes
        if (anchor < cursor) {
            start = anchor;
            len = cursor - anchor;
        } else {
            start = cursor;
            len = anchor - cursor;
        }

        // Get selected fragment
        string = contents.mid(start, len);

        // If password mode,  replace by stars
        if (options & TEXTFIELD_PASSWD) {
            string.assign('*', string.count());
        }

        // Return text of the selection as UTF-8
        if (event->target == utf8Type) {
            FXTRACE((100, "Request UTF8\n"));
            setDNDData(FROM_SELECTION, event->target, string);
            return 1;
        }

        // Return text of the selection translated to 8859-1
        if (event->target == stringType || event->target == textType) {
            FX88591Codec ascii;
            FXTRACE((100, "Request ASCII\n"));
            setDNDData(FROM_SELECTION, event->target, ascii.utf2mb(string));
            return 1;
        }

        // Return text of the selection translated to UTF-16
        if (event->target == utf16Type) {
            FXUTF16LECodec unicode;           // FIXME maybe other endianness for unix
            FXTRACE((100, "Request UTF16\n"));
            setDNDData(FROM_SELECTION, event->target, unicode.utf2mb(string));
            return 1;
        }
    }
    return 0;
}


long
MFXTextFieldIcon::onClipboardGained(FXObject* sender, FXSelector sel, void* ptr) {
    FXFrame::onClipboardGained(sender, sel, ptr);
    return 1;
}


long
MFXTextFieldIcon::onClipboardLost(FXObject* sender, FXSelector sel, void* ptr) {
    FXFrame::onClipboardLost(sender, sel, ptr);
    clipped.clear();
    return 1;
}



long
MFXTextFieldIcon::onClipboardRequest(FXObject* sender, FXSelector sel, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXString string;

    // Perhaps the target wants to supply its own data for the clipboard
    if (FXFrame::onClipboardRequest(sender, sel, ptr)) {
        return 1;
    }

    // Recognize the request?
    if (event->target == stringType || event->target == textType || event->target == utf8Type || event->target == utf16Type) {

        // Get clipped string
        string = clipped;

        // If password mode,  replace by stars
        if (options & TEXTFIELD_PASSWD) {
            string.assign('*', string.count());
        }

        // Return clipped text as as UTF-8
        if (event->target == utf8Type) {
            FXTRACE((100, "Request UTF8\n"));
            setDNDData(FROM_CLIPBOARD, event->target, string);
            return 1;
        }

        // Return clipped text translated to 8859-1
        if (event->target == stringType || event->target == textType) {
            FX88591Codec ascii;
            FXTRACE((100, "Request ASCII\n"));
            setDNDData(FROM_CLIPBOARD, event->target, ascii.utf2mb(string));
            return 1;
        }

        // Return text of the selection translated to UTF-16
        if (event->target == utf16Type) {
            FXUTF16LECodec unicode;             // FIXME maybe other endianness for unix
            FXTRACE((100, "Request UTF16\n"));
            setDNDData(FROM_CLIPBOARD, event->target, unicode.utf2mb(string));
            return 1;
        }
    }
    return 0;
}


long
MFXTextFieldIcon::onBlink(FXObject*, FXSelector, void*) {
    drawCursor(flags ^ FLAG_CARET);
    getApp()->addTimeout(this, FXTextField::ID_BLINK, getApp()->getBlinkSpeed());
    return 0;
}


long
MFXTextFieldIcon::onFocusIn(FXObject* sender, FXSelector sel, void* ptr) {
    FXFrame::onFocusIn(sender, sel, ptr);
    if (isEditable()) {
        getApp()->addTimeout(this, FXTextField::ID_BLINK, getApp()->getBlinkSpeed());
        drawCursor(FLAG_CARET);
    }
    if (hasSelection()) {
        update(border, border, width - (border << 1), height - (border << 1));
    }
    return 1;
}


long
MFXTextFieldIcon::onFocusOut(FXObject* sender, FXSelector sel, void* ptr) {
    FXFrame::onFocusOut(sender, sel, ptr);
    getApp()->removeTimeout(this, FXTextField::ID_BLINK);
    drawCursor(0);
    if (hasSelection()) {
        update(border, border, width - (border << 1), height - (border << 1));
    }
    return 1;
}


long
MFXTextFieldIcon::onFocusSelf(FXObject* sender, FXSelector sel, void* ptr) {
    if (FXFrame::onFocusSelf(sender, sel, ptr)) {
        FXEvent* event = (FXEvent*)ptr;
        if (event->type == SEL_KEYPRESS || event->type == SEL_KEYRELEASE) {
            handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_SELECT_ALL), NULL);
        }
        return 1;
    }
    return 0;
}


bool
MFXTextFieldIcon::canFocus() const {
    return true;
}


void
MFXTextFieldIcon::setFocus() {
    FXFrame::setFocus();
    setDefault(TRUE);
    flags &= ~FLAG_UPDATE;
    if (getApp()->hasInputMethod()) {
        createComposeContext();
    }
}


void
MFXTextFieldIcon::killFocus() {
    FXFrame::killFocus();
    setDefault(MAYBE);
    flags |= FLAG_UPDATE;
    if (flags & FLAG_CHANGED) {
        flags &= ~FLAG_CHANGED;
        if (!(options & TEXTFIELD_ENTER_ONLY)) {
            if (target) {
                target->tryHandle(this, FXSEL(SEL_COMMAND, message), (void*)contents.text());
            }
        }
    }
    if (getApp()->hasInputMethod()) {
        destroyComposeContext();
    }
}


long
MFXTextFieldIcon::onCmdSetHelp(FXObject*, FXSelector, void* ptr) {
    setHelpText(*((FXString*)ptr));
    return 1;
}


long
MFXTextFieldIcon::onCmdGetHelp(FXObject*, FXSelector, void* ptr) {
    *((FXString*)ptr) = getHelpText();
    return 1;
}


long
MFXTextFieldIcon::onCmdSetTip(FXObject*, FXSelector, void* ptr) {
    setTipText(*((FXString*)ptr));
    return 1;
}


long
MFXTextFieldIcon::onCmdGetTip(FXObject*, FXSelector, void* ptr) {
    *((FXString*)ptr) = getTipText();
    return 1;
}


long
MFXTextFieldIcon::onQueryTip(FXObject* sender, FXSelector sel, void* ptr) {
    if (FXWindow::onQueryTip(sender, sel, ptr)) {
        return 1;
    }
    if ((flags & FLAG_TIP) && !tip.empty()) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), (void*)&tip);
        return 1;
    }
    return 0;
}


long
MFXTextFieldIcon::onQueryHelp(FXObject* sender, FXSelector sel, void* ptr) {
    if (FXWindow::onQueryHelp(sender, sel, ptr)) {
        return 1;
    }
    if ((flags & FLAG_HELP) && !help.empty()) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), (void*)&help);
        return 1;
    }
    return 0;
}


long
MFXTextFieldIcon::onCmdSetValue(FXObject*, FXSelector, void* ptr) {
    setText((const FXchar*)ptr);
    return 1;
}


long
MFXTextFieldIcon::onCmdSetIntValue(FXObject*, FXSelector, void* ptr) {
    setText(FXStringVal(*((FXint*)ptr)));
    return 1;
}


long
MFXTextFieldIcon::onCmdSetRealValue(FXObject*, FXSelector, void* ptr) {
    setText(FXStringVal(*((FXdouble*)ptr)));
    return 1;
}


long
MFXTextFieldIcon::onCmdSetStringValue(FXObject*, FXSelector, void* ptr) {
    setText(*((FXString*)ptr));
    return 1;
}


long
MFXTextFieldIcon::onCmdGetIntValue(FXObject*, FXSelector, void* ptr) {
    *((FXint*)ptr) = FXIntVal(contents);
    return 1;
}


long
MFXTextFieldIcon::onCmdGetRealValue(FXObject*, FXSelector, void* ptr) {
    *((FXdouble*)ptr) = FXDoubleVal(contents);
    return 1;
}


long
MFXTextFieldIcon::onCmdGetStringValue(FXObject*, FXSelector, void* ptr) {
    *((FXString*)ptr) = getText();
    return 1;
}


long
MFXTextFieldIcon::onLeftBtnPress(FXObject*, FXSelector, void* ptr) {
    FXEvent* ev = (FXEvent*)ptr;
    flags &= ~FLAG_TIP;
    handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);
    if (isEnabled()) {
        grab();
        if (target && target->tryHandle(this, FXSEL(SEL_LEFTBUTTONPRESS, message), ptr)) {
            return 1;
        }
        flags &= ~FLAG_UPDATE;
        if (ev->click_count == 1) {
            setCursorPos(index(ev->win_x));
            if (ev->state & SHIFTMASK) {
                extendSelection(cursor);
            } else {
                killSelection();
                setAnchorPos(cursor);
            }
            makePositionVisible(cursor);
            flags |= FLAG_PRESSED;
        } else {
            setAnchorPos(0);
            setCursorPos(contents.length());
            extendSelection(contents.length());
            makePositionVisible(cursor);
        }
        return 1;
    }
    return 0;
}


long
MFXTextFieldIcon::onLeftBtnRelease(FXObject*, FXSelector, void* ptr) {
    if (isEnabled()) {
        ungrab();
        flags &= ~FLAG_PRESSED;
        if (target) {
            target->tryHandle(this, FXSEL(SEL_LEFTBUTTONRELEASE, message), ptr);
        }
        return 1;
    }
    return 0;
}


long
MFXTextFieldIcon::onMiddleBtnPress(FXObject*, FXSelector, void* ptr) {
    FXEvent* ev = (FXEvent*)ptr;
    flags &= ~FLAG_TIP;
    handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);
    if (isEnabled()) {
        grab();
        if (target && target->tryHandle(this, FXSEL(SEL_MIDDLEBUTTONPRESS, message), ptr)) {
            return 1;
        }
        setCursorPos(index(ev->win_x));
        setAnchorPos(cursor);
        makePositionVisible(cursor);
        update(border, border, width - (border << 1), height - (border << 1));
        flags &= ~FLAG_UPDATE;
        return 1;
    }
    return 0;
}


long
MFXTextFieldIcon::onMiddleBtnRelease(FXObject*, FXSelector, void* ptr) {
    if (isEnabled()) {
        ungrab();
        if (target && target->tryHandle(this, FXSEL(SEL_MIDDLEBUTTONRELEASE, message), ptr)) {
            return 1;
        }
        handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_PASTE_MIDDLE), NULL);
    }
    return 0;
}


long
MFXTextFieldIcon::onMotion(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXint t;
    if (flags & FLAG_PRESSED) {
        if (event->win_x < (border + padleft) || (width - border - padright) < event->win_x) {
            if (!getApp()->hasTimeout(this, ID_AUTOSCROLL)) {
                getApp()->addTimeout(this, ID_AUTOSCROLL, getApp()->getScrollSpeed(), event);
            }
        } else {
            getApp()->removeTimeout(this, ID_AUTOSCROLL);
            t = index(event->win_x);
            if (t != cursor) {
                drawCursor(0);
                cursor = t;
                extendSelection(cursor);
            }
        }
        return 1;
    }
    return 0;
}


long
MFXTextFieldIcon::onAutoScroll(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    if (flags & FLAG_PRESSED) {
        FXint newcursor = cursor;
        FXint ll = border + padleft;
        FXint rr = width - border - padright;
        FXint ww = rr - ll;
        FXint tw;

        if (options & TEXTFIELD_PASSWD) {
            tw = font->getTextWidth("*", 1) * contents.count();
        } else {
            tw = font->getTextWidth(contents.text(), contents.length());
        }

        // Text right-aligned
        if (options & JUSTIFY_RIGHT) {

            // Scroll left
            if (event->win_x < ll) {
                if (tw > ww) {
                    shift += ll - event->win_x;
                    if (ww > tw - shift) {
                        shift = tw - ww;
                    } else {
                        getApp()->addTimeout(this, ID_AUTOSCROLL, getApp()->getScrollSpeed(), event);
                    }
                }
                newcursor = index(ll);
            }

            // Scroll right
            if (rr < event->win_x) {
                if (tw > ww) {
                    shift += rr - event->win_x;
                    if (shift <= 0) {
                        shift = 0;
                    } else {
                        getApp()->addTimeout(this, ID_AUTOSCROLL, getApp()->getScrollSpeed(), event);
                    }
                }
                newcursor = index(rr);
            }
        }

        // Text left-aligned
        else if (options & JUSTIFY_LEFT) {

            // Scroll left
            if (event->win_x < ll) {
                if (tw > ww) {
                    shift += ll - event->win_x;
                    if (shift >= 0) {
                        shift = 0;
                    } else {
                        getApp()->addTimeout(this, ID_AUTOSCROLL, getApp()->getScrollSpeed(), event);
                    }
                }
                newcursor = index(ll);
            }

            // Scroll right
            if (rr < event->win_x) {
                if (tw > ww) {
                    shift += rr - event->win_x;
                    if (shift + tw < ww) {
                        shift = ww - tw;
                    } else {
                        getApp()->addTimeout(this, ID_AUTOSCROLL, getApp()->getScrollSpeed(), event);
                    }
                }
                newcursor = index(rr);
            }
        } else {
            // Text centered, Scroll left
            if (event->win_x < ll) {
                if (tw > ww) {
                    shift += ll - event->win_x;
                    if (shift > tw / 2 - ww / 2) {
                        shift = tw / 2 - ww / 2;
                    } else {
                        getApp()->addTimeout(this, ID_AUTOSCROLL, getApp()->getScrollSpeed(), event);
                    }
                }
                newcursor = index(ll);
            }
            // Scroll right
            if (rr < event->win_x) {
                if (tw > ww) {
                    shift += rr - event->win_x;
                    if (shift < (ww - ww / 2) - tw / 2) {
                        shift = (ww - ww / 2) - tw / 2;
                    } else {
                        getApp()->addTimeout(this, ID_AUTOSCROLL, getApp()->getScrollSpeed(), event);
                    }
                }
                newcursor = index(rr);
            }
        }
        // Extend the selection
        if (newcursor != cursor) {
            drawCursor(0);
            cursor = newcursor;
            extendSelection(cursor);
        }
    }
    return 1;
}


long
MFXTextFieldIcon::onCmdToggleOverstrike(FXObject*, FXSelector, void*) {
    setOverstrike(!isOverstrike());
    return 1;
}


long
MFXTextFieldIcon::onUpdToggleOverstrike(FXObject* sender, FXSelector, void*) {
    sender->handle(this, isOverstrike() ? FXSEL(SEL_COMMAND, ID_CHECK) : FXSEL(SEL_COMMAND, ID_UNCHECK), NULL);
    sender->handle(this, FXSEL(SEL_COMMAND, ID_SHOW), NULL);
    sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
    return 1;
}


long
MFXTextFieldIcon::onCmdToggleEditable(FXObject*, FXSelector, void*) {
    setEditable(!isEditable());
    return 1;
}


long
MFXTextFieldIcon::onUpdToggleEditable(FXObject* sender, FXSelector, void*) {
    sender->handle(this, isEditable() ? FXSEL(SEL_COMMAND, ID_CHECK) : FXSEL(SEL_COMMAND, ID_UNCHECK), NULL);
    sender->handle(this, FXSEL(SEL_COMMAND, ID_SHOW), NULL);
    sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
    return 1;
}


long
MFXTextFieldIcon::onUpdHaveSelection(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this, hasSelection() ? FXSEL(SEL_COMMAND, ID_ENABLE) : FXSEL(SEL_COMMAND, ID_DISABLE), ptr);
    return 1;
}


long
MFXTextFieldIcon::onUpdSelectAll(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this, contents.empty() ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), ptr);
    return 1;
}


void
MFXTextFieldIcon::setCursorPos(FXint pos) {
    pos = contents.validate(FXCLAMP(0, pos, contents.length()));
    if (cursor != pos) {
        drawCursor(0);
        cursor = pos;
        if (isEditable() && hasFocus()) {
            drawCursor(FLAG_CARET);
        }
    }
}


void
MFXTextFieldIcon::setAnchorPos(FXint pos) {
    anchor = contents.validate(FXCLAMP(0, pos, contents.length()));
}


void
MFXTextFieldIcon::drawCursor(FXuint state) {
    FXint cl, ch, xx, xlo, xhi;
    if ((state ^ flags) & FLAG_CARET) {
        if (xid) {
            FXDCWindow dc(this);
            FXASSERT(0 <= cursor && cursor <= contents.length());
            FXASSERT(0 <= anchor && anchor <= contents.length());
            xx = coord(cursor) - 1;
            // add icon spacing
            if (icon) {
                xx += ICON_SPACING + ICON_SIZE;
            }
            // Clip rectangle around cursor
            xlo = FXMAX(xx - 2, border);
            xhi = FXMIN(xx + 3, width - border);
            // Cursor can overhang padding but not borders
            dc.setClipRectangle(xlo, border, xhi - xlo, height - (border << 1));
            // Draw I beam
            if (state & FLAG_CARET) {
                // Draw I-beam
                dc.setForeground(cursorColor);
                dc.fillRectangle(xx, padtop + border, 1, height - padbottom - padtop - (border << 1));
                dc.fillRectangle(xx - 2, padtop + border, 5, 1);
                dc.fillRectangle(xx - 2, height - border - padbottom - 1, 5, 1);
            } else {
                // Erase I-beam,  plus the text immediately surrounding it
                dc.setForeground(backColor);
                dc.fillRectangle(xx - 2, border, 5, height - (border << 1));
                // Draw two characters before and after cursor
                cl = ch = cursor;
                if (0 < cl) {
                    cl = contents.dec(cl);
                    if (0 < cl) {
                        cl = contents.dec(cl);
                    }
                }
                if (ch < contents.length()) {
                    ch = contents.inc(ch);
                    if (ch < contents.length()) {
                        ch = contents.inc(ch);
                    }
                }
                drawTextRange(dc, cl, ch);
            }
        }
        flags ^= FLAG_CARET;
    }
}


void
MFXTextFieldIcon::layout() {
    FXint rr = width - border - padright;
    FXint ll = border + padleft;
    FXint ww = rr - ll;
    FXint tw;
    if (!xid) {
        return;
    }

    // Figure text width
    if (options & TEXTFIELD_PASSWD) {
        tw = font->getTextWidth("*", 1) * contents.count();
    } else {
        tw = font->getTextWidth(contents.text(), contents.length());
    }

    // Constrain shift
    if (options & JUSTIFY_RIGHT) {
        if (ww >= tw) {
            shift = 0;
        } else if (shift < 0) {
            shift = 0;
        } else if (shift > tw - ww) {
            shift = tw - ww;
        }
    } else if (options & JUSTIFY_LEFT) {
        if (ww >= tw) {
            shift = 0;
        } else if (shift > 0) {
            shift = 0;
        } else if (shift < ww - tw) {
            shift = ww - tw;
        }
    } else {
        if (ww >= tw) {
            shift = 0;
        } else if (shift > tw / 2 - ww / 2) {
            shift = tw / 2 - ww / 2;
        } else if (shift < (ww - ww / 2) - tw / 2) {
            shift = (ww - ww / 2) - tw / 2;
        }
    }

    // Keep cursor in the picture if resizing field
    makePositionVisible(cursor);

    // Always redraw
    update();

    flags &= ~(FXuint)FLAG_DIRTY;
}


void
MFXTextFieldIcon::makePositionVisible(FXint pos) {
    FXint rr = width - border - padright;
    FXint ll = border + padleft;
    FXint ww = rr - ll;
    FXint oldshift = shift;
    FXint xx;
    if (!xid) {
        return;
    }
    pos = contents.validate(FXCLAMP(0, pos, contents.length()));
    if (options & JUSTIFY_RIGHT) {
        if (options & TEXTFIELD_PASSWD) {
            xx = font->getTextWidth("*", 1) * contents.count(pos, contents.length());
        } else {
            xx = font->getTextWidth(&contents[pos], contents.length() - pos);
        }
        if (shift - xx > 0) {
            shift = xx;
        } else if (shift - xx < -ww) {
            shift = xx - ww;
        }
    } else if (options & JUSTIFY_LEFT) {
        if (options & TEXTFIELD_PASSWD) {
            xx = font->getTextWidth("*", 1) * contents.index(pos);
        } else {
            xx = font->getTextWidth(contents.text(), pos);
        }
        if (shift + xx < 0) {
            shift = -xx;
        } else if (shift + xx >= ww) {
            shift = ww - xx;
        }
    } else {
        if (options & TEXTFIELD_PASSWD) {
            xx = font->getTextWidth("*", 1) * contents.index(pos) - (font->getTextWidth("*", 1) * contents.count()) / 2;
        } else {
            xx = font->getTextWidth(contents.text(), pos) - font->getTextWidth(contents.text(), contents.length()) / 2;
        }
        if (shift + ww / 2 + xx < 0) {
            shift = -ww / 2 - xx;
        } else if (shift + ww / 2 + xx >= ww) {
            shift = ww - ww / 2 - xx;
        }
    }
    if (shift != oldshift) {
        update(border, border, width - (border << 1), height - (border << 1));
    }
}


FXint
MFXTextFieldIcon::index(FXint x) const {
    FXint rr = width - border - padright;
    FXint ll = border + padleft;
    FXint mm = (ll + rr) / 2;
    FXint pos, xx, cw;
    if (options & TEXTFIELD_PASSWD) {
        cw = font->getTextWidth("*", 1);
        if (options & JUSTIFY_RIGHT) {
            xx = rr - cw * contents.count();
        } else if (options & JUSTIFY_LEFT) {
            xx = ll;
        } else {
            xx = mm - (cw * contents.count()) / 2;
        }
        xx += shift;
        pos = contents.offset((x - xx + (cw >> 1)) / cw);
    } else {
        if (options & JUSTIFY_RIGHT) {
            xx = rr - font->getTextWidth(contents.text(), contents.length());
        } else if (options & JUSTIFY_LEFT) {
            xx = ll;
        } else {
            xx = mm - font->getTextWidth(contents.text(), contents.length()) / 2;
        }
        xx += shift;
        for (pos = 0; pos < contents.length(); pos = contents.inc(pos)) {
            cw = font->getTextWidth(&contents[pos], contents.extent(pos));
            if (x < (xx + (cw >> 1))) {
                break;
            }
            xx += cw;
        }
    }
    if (pos < 0) {
        pos = 0;
    }
    if (pos > contents.length()) {
        pos = contents.length();
    }
    return pos;
}


FXint
MFXTextFieldIcon::coord(FXint i) const {
    FXint rr = width - border - padright;
    FXint ll = border + padleft;
    FXint mm = (ll + rr) / 2;
    FXint pos;
    FXASSERT(0 <= i && i <= contents.length());
    if (options & JUSTIFY_RIGHT) {
        if (options & TEXTFIELD_PASSWD) {
            pos = rr - font->getTextWidth("*", 1) * (contents.count() - contents.index(i));
        } else {
            pos = rr - font->getTextWidth(&contents[i], contents.length() - i);
        }
    } else if (options & JUSTIFY_LEFT) {
        if (options & TEXTFIELD_PASSWD) {
            pos = ll + font->getTextWidth("*", 1) * contents.index(i);
        } else {
            pos = ll + font->getTextWidth(contents.text(), i);
        }
    } else {
        if (options & TEXTFIELD_PASSWD) {
            pos = mm + font->getTextWidth("*", 1) * contents.index(i) - (font->getTextWidth("*", 1) * contents.count()) / 2;
        } else {
            pos = mm + font->getTextWidth(contents.text(), i) - font->getTextWidth(contents.text(), contents.length()) / 2;
        }
    }
    return pos + shift;
}


FXbool
MFXTextFieldIcon::isPosVisible(FXint pos) const {
    if (0 <= pos && pos <= contents.length()) {
        FXint x = coord(contents.validate(pos));
        return border + padleft <= x && x <= width - border - padright;
    }
    return FALSE;
}


FXbool
MFXTextFieldIcon::isPosSelected(FXint pos) const {
    return hasSelection() && FXMIN(anchor, cursor) <= pos && pos <= FXMAX(anchor, cursor);
}


void
MFXTextFieldIcon::drawTextFragment(FXDCWindow& dc, FXint x, FXint y, FXint fm, FXint to) {
    x += font->getTextWidth(contents.text(), fm);
    y += font->getFontAscent();
    dc.drawText(x, y, &contents[fm], to - fm);
}


void
MFXTextFieldIcon::drawPWDTextFragment(FXDCWindow& dc, FXint x, FXint y, FXint fm, FXint to) {
    FXint cw = font->getTextWidth("*", 1);
    FXint i;
    y += font->getFontAscent();
    x += cw * contents.index(fm);
    for (i = fm; i < to; i = contents.inc(i), x += cw) {
        dc.drawText(x, y, "*", 1);
    }
}


// Draw range of text
void MFXTextFieldIcon::drawTextRange(FXDCWindow& dc, FXint fm, FXint to) {
    FXint sx, ex, xx, yy, cw, hh, ww, si, ei, lx, rx, t;
    FXint rr = width - border - padright;
    FXint ll = border + padleft;
    FXint mm = (ll + rr) / 2;
    if (to <= fm) {
        return;
    }
    // set font
    dc.setFont(font);
    // Text color
    dc.setForeground(textColor);
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
    // Password mode
    if (options & TEXTFIELD_PASSWD) {
        cw = font->getTextWidth("*", 1);
        ww = cw * contents.count();
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
        // check if add icon spacing
        if (icon) {
            xx += ICON_SPACING + ICON_SIZE;
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
            xx = shift + rr - ww;
        } else if (options & JUSTIFY_LEFT) {
            // Text sticks on left of field
            xx = shift + ll;
        } else {
            // Text centered in field
            xx = shift + mm - ww / 2;
        }
        // check if add icon spacing
        if (icon) {
            xx += ICON_SPACING + ICON_SIZE;
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
}


long
MFXTextFieldIcon::onPaint(FXObject*, FXSelector, void* ptr) {
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
    // Draw text, clipped against frame interior
    dc.setClipRectangle(border, border, width - (border << 1), height - (border << 1));
    drawTextRange(dc, 0, contents.length());
    // Draw caret
    if (flags & FLAG_CARET) {
        int xx = coord(cursor) - 1;
        // check if add icon spacing
        if (icon) {
            xx += ICON_SPACING + ICON_SIZE;
        }
        dc.setForeground(cursorColor);
        dc.fillRectangle(xx, padtop + border, 1, height - padbottom - padtop - (border << 1));
        dc.fillRectangle(xx - 2, padtop + border, 5, 1);
        dc.fillRectangle(xx - 2, height - border - padbottom - 1, 5, 1);
    }
    // draw icon
    if (icon) {
        dc.drawIcon(icon, ICON_SPACING / 2, border + padtop + (height - padbottom - padtop - (border << 1) - ICON_SIZE) / 2);
    }
    return 1;
}


long
MFXTextFieldIcon::onCmdCursorHome(FXObject*, FXSelector, void*) {
    setCursorPos(0);
    makePositionVisible(0);
    return 1;
}


long
MFXTextFieldIcon::onCmdCursorEnd(FXObject*, FXSelector, void*) {
    setCursorPos(contents.length());
    makePositionVisible(cursor);
    return 1;
}


long
MFXTextFieldIcon::onCmdCursorRight(FXObject*, FXSelector, void*) {
    setCursorPos(contents.inc(cursor));
    makePositionVisible(cursor);
    return 1;
}



long
MFXTextFieldIcon::onCmdCursorLeft(FXObject*, FXSelector, void*) {
    setCursorPos(contents.dec(cursor));
    makePositionVisible(cursor);
    return 1;
}


static FXbool
isdelimiter(const FXchar* delimiters, FXwchar w) {
    return w < 128 && strchr(delimiters, w); // FIXME for w >= 128
}


FXint
MFXTextFieldIcon::leftWord(FXint pos) const {
    FXint pp = pos, p;

    // Ensure input is valid
    FXASSERT(0 <= pos && pos <= contents.length());

    // Back up until space or delimiter
    while (0 <= (p = contents.dec(pp)) && !Unicode::isSpace(contents.wc(p)) && !isdelimiter(delimiters, contents.wc(p))) {
        pp = p;
    }

    // Back up over run of spaces
    while (0 <= (p = contents.dec(pp)) && Unicode::isSpace(contents.wc(p))) {
        pp = p;
    }

    // One more in case we didn't move
    if ((pos == pp) && 0 <= (p = contents.dec(pp))) {
        pp = p;
    }

    return pp;
}


FXint
MFXTextFieldIcon::rightWord(FXint pos) const {
    FXint pp = pos;

    // Ensure input is valid
    FXASSERT(0 <= pos && pos <= contents.length());

    // Advance until space or delimiter
    while (pp < contents.length() && !Unicode::isSpace(contents.wc(pp)) && !isdelimiter(delimiters, contents.wc(pp))) {
        pp = contents.inc(pp);
    }

    // Advance over run of spaces
    while (pp < contents.length() && Unicode::isSpace(contents.wc(pp))) {
        pp = contents.inc(pp);
    }

    // One more in case we didn't move
    if ((pos == pp) && pp < contents.length()) {
        pp = contents.inc(pp);
    }

    return pp;
}


FXint
MFXTextFieldIcon::wordStart(FXint pos) const {
    FXint p;
    FXASSERT(0 <= pos && pos <= contents.length());
    if (pos == contents.length() || Unicode::isSpace(contents.wc(pos))) {
        while (0 <= (p = contents.dec(pos)) && Unicode::isSpace(contents.wc(p))) {
            pos = p;
        }
    } else if (isdelimiter(delimiters, contents.wc(pos))) {
        while (0 <= (p = contents.dec(pos)) && isdelimiter(delimiters, contents.wc(p))) {
            pos = p;
        }
    } else {
        while (0 <= (p = contents.dec(pos)) && !isdelimiter(delimiters, contents.wc(p)) && !Unicode::isSpace(contents.wc(p))) {
            pos = p;
        }
    }
    return pos;
}


FXint
MFXTextFieldIcon::wordEnd(FXint pos) const {
    FXASSERT(0 <= pos && pos <= contents.length());
    if (pos == contents.length() || Unicode::isSpace(contents.wc(pos))) {
        while (pos < contents.length() && Unicode::isSpace(contents.wc(pos))) {
            pos = contents.inc(pos);
        }
    } else if (isdelimiter(delimiters, contents.wc(pos))) {
        while (pos < contents.length() && isdelimiter(delimiters, contents.wc(pos))) {
            pos = contents.inc(pos);
        }
    } else {
        while (pos < contents.length() && !isdelimiter(delimiters, contents.wc(pos)) && !Unicode::isSpace(contents.wc(pos))) {
            pos = contents.inc(pos);
        }
    }
    return pos;
}


long
MFXTextFieldIcon::onCmdCursorWordRight(FXObject*, FXSelector, void*) {
    setCursorPos(rightWord(cursor));
    makePositionVisible(cursor);
    return 1;
}


long
MFXTextFieldIcon::onCmdCursorWordLeft(FXObject*, FXSelector, void*) {
    setCursorPos(leftWord(cursor));
    makePositionVisible(cursor);
    return 1;
}


long
MFXTextFieldIcon::onCmdCursorWordStart(FXObject*, FXSelector, void*) {
    setCursorPos(wordStart(cursor));
    makePositionVisible(cursor);
    return 1;
}


long
MFXTextFieldIcon::onCmdCursorWordEnd(FXObject*, FXSelector, void*) {
    setCursorPos(wordEnd(cursor));
    makePositionVisible(cursor);
    return 1;
}


long
MFXTextFieldIcon::onCmdMark(FXObject*, FXSelector, void*) {
    setAnchorPos(cursor);
    return 1;
}


long
MFXTextFieldIcon::onCmdExtend(FXObject*, FXSelector, void*) {
    extendSelection(cursor);
    return 1;
}


long
MFXTextFieldIcon::onCmdSelectAll(FXObject*, FXSelector, void*) {
    selectAll();
    makePositionVisible(cursor);
    return 1;
}


long
MFXTextFieldIcon::onCmdDeselectAll(FXObject*, FXSelector, void*) {
    killSelection();
    return 1;
}


long
MFXTextFieldIcon::onCmdCutSel(FXObject*, FXSelector, void*) {
    if (isEditable()) {
        if (hasSelection()) {
            FXDragType types[4];
            types[0] = stringType;
            types[1] = textType;
            types[2] = utf8Type;
            types[3] = utf16Type;
            if (acquireClipboard(types, 4)) {
                if (anchor < cursor) {
                    clipped = contents.mid(anchor, cursor - anchor);
                } else {
                    clipped = contents.mid(cursor, anchor - cursor);
                }
                handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_DELETE_SEL), NULL);
            }
        }
    } else {
        getApp()->beep();
    }
    return 1;
}


long
MFXTextFieldIcon::onCmdCopySel(FXObject*, FXSelector, void*) {
    if (hasSelection()) {
        FXDragType types[4];
        types[0] = stringType;
        types[1] = textType;
        types[2] = utf8Type;
        types[3] = utf16Type;
        if (acquireClipboard(types, 4)) {
            if (anchor < cursor) {
                clipped = contents.mid(anchor, cursor - anchor);
            } else {
                clipped = contents.mid(cursor, anchor - cursor);
            }
        }
    }
    return 1;
}


long
MFXTextFieldIcon::onCmdPasteSel(FXObject*, FXSelector, void*) {
    if (isEditable()) {
        FXString string;

        // Delete existing selection
        if (hasSelection()) {
            handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_DELETE_SEL), NULL);
        }

        // First,  try UTF-8
        if (getDNDData(FROM_CLIPBOARD, utf8Type, string)) {
            FXTRACE((100, "Paste UTF8\n"));
            handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_INSERT_STRING), (void*)string.text());
            return 1;
        }

        // Next,  try UTF-16
        if (getDNDData(FROM_CLIPBOARD, utf16Type, string)) {
            FXUTF16LECodec unicode;           // FIXME maybe other endianness for unix
            FXTRACE((100, "Paste UTF16\n"));
            handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_INSERT_STRING), (void*)unicode.mb2utf(string).text());
            return 1;
        }

        // Next,  try good old Latin-1
        if (getDNDData(FROM_CLIPBOARD, stringType, string)) {
            FX88591Codec ascii;
            FXTRACE((100, "Paste ASCII\n"));
            handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_INSERT_STRING), (void*)ascii.mb2utf(string).text());
            return 1;
        }
    } else {
        getApp()->beep();
    }
    return 1;
}


long
MFXTextFieldIcon::onCmdPasteMiddle(FXObject*, FXSelector, void*) {
    if (isEditable()) {
        FXString string;

        // First,  try UTF-8
        if (getDNDData(FROM_SELECTION, utf8Type, string)) {
            FXTRACE((100, "Paste UTF8\n"));
            handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_INSERT_STRING), (void*)string.text());
            return 1;
        }

        // Next,  try UTF-16
        if (getDNDData(FROM_SELECTION, utf16Type, string)) {
            FXUTF16LECodec unicode;                 // FIXME maybe other endianness for unix
            FXTRACE((100, "Paste UTF16\n"));
            handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_INSERT_STRING), (void*)unicode.mb2utf(string).text());
            return 1;
        }

        // Finally,  try good old 8859-1
        if (getDNDData(FROM_SELECTION, stringType, string)) {
            FX88591Codec ascii;
            FXTRACE((100, "Paste ASCII\n"));
            handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_INSERT_STRING), (void*)ascii.mb2utf(string).text());
            return 1;
        }
    } else {
        getApp()->beep();
    }
    return 1;
}


long
MFXTextFieldIcon::onCmdDeleteSel(FXObject*, FXSelector, void*) {
    if (isEditable()) {
        if (!hasSelection()) {
            return 1;
        }
        FXint st = FXMIN(anchor, cursor);
        FXint en = FXMAX(anchor, cursor);
        setCursorPos(st);
        setAnchorPos(st);
        contents.erase(st, en - st);
        layout();
        makePositionVisible(st);
        killSelection();
        flags |= FLAG_CHANGED;
        if (target) {
            target->tryHandle(this, FXSEL(SEL_CHANGED, message), (void*)contents.text());
        }
    } else {
        getApp()->beep();
    }
    return 1;
}


long
MFXTextFieldIcon::onCmdDeleteAll(FXObject*, FXSelector, void*) {
    if (isEditable()) {
        setCursorPos(0);
        setAnchorPos(0);
        contents.clear();
        layout();
        makePositionVisible(0);
        killSelection();
        flags |= FLAG_CHANGED;
        if (target) {
            target->tryHandle(this, FXSEL(SEL_CHANGED, message), (void*)contents.text());
        }
    } else {
        getApp()->beep();
    }
    return 1;
}


long
MFXTextFieldIcon::onCmdOverstString(FXObject*, FXSelector, void* ptr) {
    if (isEditable()) {
        FXString tentative = contents;
        FXint len = (int)strlen((FXchar*)ptr);
        FXint reppos = cursor;
        FXint replen = len;
        if (hasSelection()) {
            reppos = FXMIN(anchor, cursor);
            replen = FXMAX(anchor, cursor) - reppos;
        }
        tentative.replace(reppos, replen, (FXchar*)ptr, len);
        if (handle(this, FXSEL(SEL_VERIFY, 0), (void*)tentative.text())) {
            getApp()->beep();
            return 1;
        }
        setCursorPos(reppos);
        setAnchorPos(reppos);
        contents = tentative;
        layout();
        setCursorPos(reppos + len);
        setAnchorPos(reppos + len);
        makePositionVisible(reppos + len);
        killSelection();
        update(border, border, width - (border << 1), height - (border << 1));
        flags |= FLAG_CHANGED;
        if (target) {
            target->tryHandle(this, FXSEL(SEL_CHANGED, message), (void*)contents.text());
        }
    } else {
        getApp()->beep();
    }
    return 1;
}


long
MFXTextFieldIcon::onCmdInsertString(FXObject*, FXSelector, void* ptr) {
    if (isEditable()) {
        FXString tentative = contents;
        FXint len = (int)strlen((FXchar*)ptr);
        FXint reppos = cursor;
        FXint replen = 0;
        if (hasSelection()) {
            reppos = FXMIN(anchor, cursor);
            replen = FXMAX(anchor, cursor) - reppos;
        }
        tentative.replace(reppos, replen, (FXchar*)ptr, len);
        if (handle(this, FXSEL(SEL_VERIFY, 0), (void*)tentative.text())) {
            getApp()->beep();
            return 1;
        }
        setCursorPos(reppos);
        setAnchorPos(reppos);
        contents = tentative;
        layout();
        setCursorPos(reppos + len);
        setAnchorPos(reppos + len);
        makePositionVisible(reppos + len);
        killSelection();
        update(border, border, width - (border << 1), height - (border << 1));
        flags |= FLAG_CHANGED;
        if (target) {
            target->tryHandle(this, FXSEL(SEL_CHANGED, message), (void*)contents.text());
        }
    } else {
        getApp()->beep();
    }
    return 1;
}



long
MFXTextFieldIcon::onCmdBackspace(FXObject*, FXSelector, void*) {
    if (isEditable() && 0 < cursor) {
        setCursorPos(contents.dec(cursor));
        setAnchorPos(cursor);
        contents.erase(cursor, contents.extent(cursor));
        layout();
        makePositionVisible(cursor);
        update(border, border, width - (border << 1), height - (border << 1));
        flags |= FLAG_CHANGED;
        if (target) {
            target->tryHandle(this, FXSEL(SEL_CHANGED, message), (void*)contents.text());
        }
    } else {
        getApp()->beep();
    }
    return 1;
}


long
MFXTextFieldIcon::onCmdDelete(FXObject*, FXSelector, void*) {
    if (isEditable() && cursor < contents.length()) {
        contents.erase(cursor, contents.extent(cursor));
        layout();
        setCursorPos(cursor);
        setAnchorPos(cursor);
        makePositionVisible(cursor);
        update(border, border, width - (border << 1), height - (border << 1));
        flags |= FLAG_CHANGED;
        if (target) {
            target->tryHandle(this, FXSEL(SEL_CHANGED, message), (void*)contents.text());
        }
    } else {
        getApp()->beep();
    }
    return 1;
}


long
MFXTextFieldIcon::onVerify(FXObject*, FXSelector, void* ptr) {
    FXchar* p = (FXchar*)ptr;

    // Limit number of columns
    if (options & TEXTFIELD_LIMITED) {
        if ((FXint)wcslen(p) > columns) {
            return 1;
        }
    }

    // Integer input
    if (options & TEXTFIELD_INTEGER) {
        while (Ascii::isSpace(*p)) {
            p++;
        }
        if (*p == '-' || *p == '+') {
            p++;
        }
        while (Ascii::isDigit(*p)) {
            p++;
        }
        while (Ascii::isSpace(*p)) {
            p++;
        }
        if (*p != '\0') {
            return 1;    // Objection!
        }
    }

    // Real input
    if (options & TEXTFIELD_REAL) {
        while (Ascii::isSpace(*p)) {
            p++;
        }
        if (*p == '-' || *p == '+') {
            p++;
        }
        while (Ascii::isDigit(*p)) {
            p++;
        }
        if (*p == '.') {
            p++;
        }
        while (Ascii::isDigit(*p)) {
            p++;
        }
        if (*p == 'E' || *p == 'e') {
            p++;
            if (*p == '-' || *p == '+') {
                p++;
            }
            while (Ascii::isDigit(*p)) {
                p++;
            }
        }
        while (Ascii::isSpace(*p)) {
            p++;
        }
        if (*p != '\0') {
            return 1;    // Objection!
        }
    }

    // Target has chance to object to the proposed change
    if (target && target->tryHandle(this, FXSEL(SEL_VERIFY, message), ptr)) {
        return 1;
    }

    // No objections have been raised!
    return 0;
}


long
MFXTextFieldIcon::onKeyPress(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    flags &= ~FLAG_TIP;
    if (isEnabled()) {
        FXTRACE((200, "%s::onKeyPress keysym = 0x%04x state = %04x\n", getClassName(), event->code, event->state));
        if (target && target->tryHandle(this, FXSEL(SEL_KEYPRESS, message), ptr)) {
            return 1;
        }
        flags &= ~FLAG_UPDATE;
        switch (event->code) {
            case KEY_Right:
            case KEY_KP_Right:
                if (!(event->state & SHIFTMASK)) {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_DESELECT_ALL), NULL);
                }
                if (event->state & CONTROLMASK) {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_CURSOR_WORD_RIGHT), NULL);
                } else {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_CURSOR_RIGHT), NULL);
                }
                if (event->state & SHIFTMASK) {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_EXTEND), NULL);
                } else {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_MARK), NULL);
                }
                return 1;
            case KEY_Left:
            case KEY_KP_Left:
                if (!(event->state & SHIFTMASK)) {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_DESELECT_ALL), NULL);
                }
                if (event->state & CONTROLMASK) {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_CURSOR_WORD_LEFT), NULL);
                } else {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_CURSOR_LEFT), NULL);
                }
                if (event->state & SHIFTMASK) {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_EXTEND), NULL);
                } else {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_MARK), NULL);
                }
                return 1;
            case KEY_Home:
            case KEY_KP_Home:
                if (!(event->state & SHIFTMASK)) {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_DESELECT_ALL), NULL);
                }
                handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_CURSOR_HOME), NULL);
                if (event->state & SHIFTMASK) {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_EXTEND), NULL);
                } else {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_MARK), NULL);
                }
                return 1;
            case KEY_End:
            case KEY_KP_End:
                if (!(event->state & SHIFTMASK)) {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_DESELECT_ALL), NULL);
                }
                handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_CURSOR_END), NULL);
                if (event->state & SHIFTMASK) {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_EXTEND), NULL);
                } else {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_MARK), NULL);
                }
                return 1;
            case KEY_Insert:
            case KEY_KP_Insert:
                if (event->state & CONTROLMASK) {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_COPY_SEL), NULL);
                } else if (event->state & SHIFTMASK) {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_PASTE_SEL), NULL);
                } else {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_TOGGLE_OVERSTRIKE), NULL);
                }
                return 1;
            case KEY_Delete:
            case KEY_KP_Delete:
                if (hasSelection())     {
                    if (event->state & SHIFTMASK) {
                        handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_CUT_SEL), NULL);
                    } else {
                        handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_DELETE_SEL), NULL);
                    }
                } else {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_DELETE), NULL);
                }
                return 1;
            case KEY_BackSpace:
                if (hasSelection()) {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_DELETE_SEL), NULL);
                } else {
                    handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_BACKSPACE), NULL);
                }
                return 1;
            case KEY_Return:
            case KEY_KP_Enter:
                if (isEditable()) {
                    flags |= FLAG_UPDATE;
                    flags &= ~FLAG_CHANGED;
                    if (target) {
                        target->tryHandle(this, FXSEL(SEL_COMMAND, message), (void*)contents.text());
                    }
                } else {
                    getApp()->beep();
                }
                return 1;
            case KEY_a:
                if (!(event->state & CONTROLMASK)) {
                    break;
                }
                handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_SELECT_ALL), NULL);
                return 1;
            case KEY_x:
                if (!(event->state & CONTROLMASK)) {
                    break;
                }
                FALLTHROUGH;
            case KEY_F20:                             // Sun Cut key
                handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_CUT_SEL), NULL);
                return 1;
            case KEY_c:
                if (!(event->state & CONTROLMASK)) {
                    break;
                }
                FALLTHROUGH;
            case KEY_F16:                             // Sun Copy key
                handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_COPY_SEL), NULL);
                return 1;
            case KEY_v:
                if (!(event->state & CONTROLMASK)) {
                    break;
                }
                FALLTHROUGH;
            case KEY_F18:                             // Sun Paste key
                handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_PASTE_SEL), NULL);
                return 1;
            default:
                break;
        }
        if ((event->state & (CONTROLMASK | ALTMASK)) || ((FXuchar)event->text[0] < 32)) {
            return 0;
        }
        if (isOverstrike()) {
            handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_OVERST_STRING), (void*)event->text.text());
        } else {
            handle(this, FXSEL(SEL_COMMAND, FXTextField::ID_INSERT_STRING), (void*)event->text.text());
        }
        return 1;
    }
    return 0;
}


long
MFXTextFieldIcon::onKeyRelease(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    if (isEnabled()) {
        FXTRACE((200, "%s::onKeyRelease keysym = 0x%04x state = %04x\n", getClassName(), ((FXEvent*)ptr)->code, ((FXEvent*)ptr)->state));
        if (target && target->tryHandle(this, FXSEL(SEL_KEYRELEASE, message), ptr)) {
            return 1;
        }
        switch (event->code) {
            case KEY_Right:
            case KEY_KP_Right:
            case KEY_Left:
            case KEY_KP_Left:
            case KEY_Home:
            case KEY_KP_Home:
            case KEY_End:
            case KEY_KP_End:
            case KEY_Insert:
            case KEY_KP_Insert:
            case KEY_Delete:
            case KEY_KP_Delete:
            case KEY_BackSpace:
            case KEY_Return:
            case KEY_F20:   // Sun Cut key
            case KEY_F16:   // Sun Copy key
            case KEY_F18:   // Sun Paste key
                return 1;
            case KEY_a:
            case KEY_x:
            case KEY_c:
            case KEY_v:
                if (event->state & CONTROLMASK) {
                    return 1;
                }
                FALLTHROUGH;
            default:
                if ((event->state & (CONTROLMASK | ALTMASK)) || ((FXuchar)event->text[0] < 32)) {
                    return 0;
                }
                return 1;
        }
    }
    return 0;
}


FXbool
MFXTextFieldIcon::killSelection() {
    if (hasSelection()) {
        releaseSelection();
        update(border, border, width - (border << 1), height - (border << 1));
        return TRUE;
    }
    return FALSE;
}


FXbool
MFXTextFieldIcon::selectAll() {
    setAnchorPos(0);
    setCursorPos(contents.length());
    extendSelection(cursor);
    return TRUE;
}


FXbool
MFXTextFieldIcon::setSelection(FXint pos, FXint len) {
    setAnchorPos(pos);
    setCursorPos(pos + len);
    extendSelection(cursor);
    return TRUE;
}


FXbool
MFXTextFieldIcon::extendSelection(FXint pos) {
    FXDragType types[4];

    // Validate position to start of character
    pos = contents.validate(FXCLAMP(0, pos, contents.length()));

    // Got a selection at all?
    if (anchor != pos) {
        types[0] = stringType;
        types[1] = textType;
        types[2] = utf8Type;
        types[3] = utf16Type;
        if (!hasSelection()) {
            acquireSelection(types, 4);
        }
    } else {
        if (hasSelection()) {
            releaseSelection();
        }
    }

    update(border, border, width - (border << 1), height - (border << 1));
    return TRUE;
}



void
MFXTextFieldIcon::setText(const FXString& text, FXbool notify) {
    killSelection();
    if (contents != text) {
        contents = text;
        anchor = contents.length();
        cursor = contents.length();
        if (xid) {
            layout();
        }
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_COMMAND, message), (void*)contents.text());
        }
    }
}


void
MFXTextFieldIcon::setIcon(FXIcon* ic) {
    killSelection();
    icon = ic;
    anchor = contents.length();
    cursor = contents.length();
    if (xid) {
        layout();
    }
}


void
MFXTextFieldIcon::setTextColor(FXColor clr) {
    if (textColor != clr) {
        textColor = clr;
        update();
    }
}



void
MFXTextFieldIcon::setSelBackColor(FXColor clr) {
    if (selbackColor != clr) {
        selbackColor = clr;
        update();
    }
}


void
MFXTextFieldIcon::setSelTextColor(FXColor clr) {
    if (seltextColor != clr) {
        seltextColor = clr;
        update();
    }
}



void
MFXTextFieldIcon::setCursorColor(FXColor clr) {
    if (clr != cursorColor) {
        cursorColor = clr;
        update();
    }
}



void
MFXTextFieldIcon::setNumColumns(FXint ncols) {
    if (ncols < 0) {
        ncols = 0;
    }
    if (columns != ncols) {
        shift = 0;
        columns = ncols;
        layout();   // This may not be necessary!
        recalc();
        update();
    }
}



FXbool
MFXTextFieldIcon::isEditable() const {
    return (options & TEXTFIELD_READONLY) == 0;
}



void
MFXTextFieldIcon::setEditable(FXbool edit) {
    if (edit) {
        options &= ~TEXTFIELD_READONLY;
    } else {
        options |= TEXTFIELD_READONLY;
    }
}



FXbool
MFXTextFieldIcon::isOverstrike() const {
    return (options & TEXTFIELD_OVERSTRIKE) != 0;
}



void
MFXTextFieldIcon::setOverstrike(FXbool over) {
    if (over) {
        options |= TEXTFIELD_OVERSTRIKE;
    } else {
        options &= ~TEXTFIELD_OVERSTRIKE;
    }
}


void
MFXTextFieldIcon::setTextStyle(FXuint style) {
    FXuint opts = (options & ~TEXTFIELD_MASK) | (style & TEXTFIELD_MASK);
    if (options != opts) {
        shift = 0;
        options = opts;
        recalc();
        update();
    }
}



FXuint
MFXTextFieldIcon::getTextStyle() const {
    return (options & TEXTFIELD_MASK);
}



void
MFXTextFieldIcon::setJustify(FXuint style) {
    FXuint opts = (options & ~JUSTIFY_MASK) | (style & JUSTIFY_MASK);
    if (options != opts) {
        shift = 0;
        options = opts;
        recalc();
        update();
    }
}



FXuint
MFXTextFieldIcon::getJustify() const {
    return (options & JUSTIFY_MASK);
}



void
MFXTextFieldIcon::save(FXStream& store) const {
    FXFrame::save(store);
    store << contents;
    store << font;
    store << textColor;
    store << selbackColor;
    store << seltextColor;
    store << cursorColor;
    store << columns;
    store << help;
    store << tip;
}



void
MFXTextFieldIcon::load(FXStream& store) {
    FXFrame::load(store);
    store >> contents;
    store >> font;
    store >> textColor;
    store >> selbackColor;
    store >> seltextColor;
    store >> cursorColor;
    store >> columns;
    store >> help;
    store >> tip;
}


MFXTextFieldIcon::MFXTextFieldIcon() {
    flags |= FLAG_ENABLED;
    font = (FXFont*) - 1L;
}
