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
/// @file    MFXTextFieldIcon.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "fxheader.h"


// ===========================================================================
// class definitions
// ===========================================================================

class MFXTextFieldIcon : public FXFrame {
    FXDECLARE(MFXTextFieldIcon)

public:
    long onPaint(FXObject*,FXSelector,void*);
    long onUpdate(FXObject*,FXSelector,void*);
    long onKeyPress(FXObject*,FXSelector,void*);
    long onKeyRelease(FXObject*,FXSelector,void*);
    long onLeftBtnPress(FXObject*,FXSelector,void*);
    long onLeftBtnRelease(FXObject*,FXSelector,void*);
    long onMiddleBtnPress(FXObject*,FXSelector,void*);
    long onMiddleBtnRelease(FXObject*,FXSelector,void*);
    long onVerify(FXObject*,FXSelector,void*);
    long onMotion(FXObject*,FXSelector,void*);
    long onSelectionLost(FXObject*,FXSelector,void*);
    long onSelectionGained(FXObject*,FXSelector,void*);
    long onSelectionRequest(FXObject*,FXSelector,void* ptr);
    long onClipboardLost(FXObject*,FXSelector,void*);
    long onClipboardGained(FXObject*,FXSelector,void*);
    long onClipboardRequest(FXObject*,FXSelector,void*);
    long onFocusSelf(FXObject*,FXSelector,void*);
    long onFocusIn(FXObject*,FXSelector,void*);
    long onFocusOut(FXObject*,FXSelector,void*);
    long onBlink(FXObject*,FXSelector,void*);
    long onAutoScroll(FXObject*,FXSelector,void*);
    long onCmdSetValue(FXObject*,FXSelector,void*);
    long onCmdSetIntValue(FXObject*,FXSelector,void*);
    long onCmdSetRealValue(FXObject*,FXSelector,void*);
    long onCmdSetStringValue(FXObject*,FXSelector,void*);
    long onCmdGetIntValue(FXObject*,FXSelector,void*);
    long onCmdGetRealValue(FXObject*,FXSelector,void*);
    long onCmdGetStringValue(FXObject*,FXSelector,void*);
    long onCmdCursorHome(FXObject*,FXSelector,void*);
    long onCmdCursorEnd(FXObject*,FXSelector,void*);
    long onCmdCursorRight(FXObject*,FXSelector,void*);
    long onCmdCursorLeft(FXObject*,FXSelector,void*);
    long onCmdCursorWordLeft(FXObject*,FXSelector,void*);
    long onCmdCursorWordRight(FXObject*,FXSelector,void*);
    long onCmdCursorWordStart(FXObject*,FXSelector,void*);
    long onCmdCursorWordEnd(FXObject*,FXSelector,void*);
    long onCmdMark(FXObject*,FXSelector,void*);
    long onCmdExtend(FXObject*,FXSelector,void*);
    long onCmdSelectAll(FXObject*,FXSelector,void*);
    long onCmdDeselectAll(FXObject*,FXSelector,void*);
    long onCmdCutSel(FXObject*,FXSelector,void*);
    long onCmdCopySel(FXObject*,FXSelector,void*);
    long onCmdPasteSel(FXObject*,FXSelector,void*);
    long onCmdPasteMiddle(FXObject*,FXSelector,void*);
    long onCmdDeleteSel(FXObject*,FXSelector,void*);
    long onCmdDeleteAll(FXObject*,FXSelector,void*);
    long onCmdOverstString(FXObject*,FXSelector,void*);
    long onCmdInsertString(FXObject*,FXSelector,void*);
    long onCmdBackspace(FXObject*,FXSelector,void*);
    long onCmdDelete(FXObject*,FXSelector,void*);
    long onCmdToggleEditable(FXObject*,FXSelector,void*);
    long onUpdToggleEditable(FXObject*,FXSelector,void*);
    long onCmdToggleOverstrike(FXObject*,FXSelector,void*);
    long onUpdToggleOverstrike(FXObject*,FXSelector,void*);
    long onUpdHaveSelection(FXObject*,FXSelector,void*);
    long onUpdSelectAll(FXObject*,FXSelector,void*);
    long onCmdSetHelp(FXObject*,FXSelector,void*);
    long onCmdGetHelp(FXObject*,FXSelector,void*);
    long onCmdSetTip(FXObject*,FXSelector,void*);
    long onCmdGetTip(FXObject*,FXSelector,void*);
    long onQueryHelp(FXObject*,FXSelector,void*);
    long onQueryTip(FXObject*,FXSelector,void*);

public:

    /// Construct text field wide enough to display ncols columns
    MFXTextFieldIcon(FXComposite* p,FXint ncols,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=TEXTFIELD_NORMAL,FXint x=0,FXint y=0,FXint w=0,FXint h=0,FXint pl=DEFAULT_PAD,FXint pr=DEFAULT_PAD,FXint pt=DEFAULT_PAD,FXint pb=DEFAULT_PAD);

    /// Create server-side resources
    virtual void create();

    /// Perform layout
    virtual void layout();

    /// Enable text field
    virtual void enable();

    /// Disable text field
    virtual void disable();

    /// Return default width
    virtual FXint getDefaultWidth();

    /// Return default height
    virtual FXint getDefaultHeight();

    /// Yes, text field may receive focus
    virtual bool canFocus() const;

    /// Move the focus to this window
    virtual void setFocus();

    /// Remove the focus from this window
    virtual void killFocus();

    /// Set editable mode
    void setEditable(FXbool edit=TRUE);

    /// Return TRUE if text field may be edited
    FXbool isEditable() const;

    /// Set overstrike mode
    void setOverstrike(FXbool over=TRUE);

    /// Return TRUE if overstrike mode in effect
    FXbool isOverstrike() const;

    /// Set cursor position
    void setCursorPos(FXint pos);

    /// Return cursor position
    FXint getCursorPos() const { return cursor; }

    /// Change anchor position
    void setAnchorPos(FXint pos);

    /// Return anchor position
    FXint getAnchorPos() const { return anchor; }

    /// Change the text and move cursor to end
    void setText(const FXString& text,FXbool notify=FALSE);

    /// Get the text for this label
    FXString getText() const { return contents; }

    /// Set the text font
    void setFont(FXFont* fnt);

    /// Get the text font
    FXFont* getFont() const { return font; }

    /// Change text color
    void setTextColor(FXColor clr);

    /// Return text color
    FXColor getTextColor() const { return textColor; }

    /// Change selected background color
    void setSelBackColor(FXColor clr);

    /// Return selected background color
    FXColor getSelBackColor() const { return selbackColor; }

    /// Change selected text color
    void setSelTextColor(FXColor clr);

    /// Return selected text color
    FXColor getSelTextColor() const { return seltextColor; }

    /// Changes the cursor color
    void setCursorColor(FXColor clr);

    /// Return the cursor color
    FXColor getCursorColor() const { return cursorColor; }

    /**
    * Change the default width of the text field in terms of a number
    * of columns times the width of the numeral '8'.
    */
    void setNumColumns(FXint cols);

    /// Return number of columns
    FXint getNumColumns() const { return columns; }

    /**
    * Change text justification mode. The justify mode is a combination of
    * horizontal justification (JUSTIFY_LEFT, JUSTIFY_RIGHT, or JUSTIFY_CENTER_X),
    * and vertical justification (JUSTIFY_TOP, JUSTIFY_BOTTOM, JUSTIFY_CENTER_Y).
    * Note that JUSTIFY_CENTER_X can not be set from the constructor since by
    * default text fields are left-justified.
    */
    void setJustify(FXuint mode);

    /// Return text justification mode
    FXuint getJustify() const;

    /// Change word delimiters
    void setDelimiters(const FXchar* delims=FXTextField::textDelimiters){ delimiters=delims; }

    /// Return word delimiters
    const FXchar* getDelimiters() const { return delimiters; }

    /// Set the status line help text for this label
    void setHelpText(const FXString& text){ help=text; }

    /// Get the status line help text for this label
    const FXString& getHelpText() const { return help; }

    /// Set the tool tip message for this text field
    void setTipText(const FXString& text){ tip=text; }

    /// Get the tool tip message for this text field
    const FXString& getTipText() const { return tip; }

    /// Change text style
    void setTextStyle(FXuint style);

    /// Return text style
    FXuint getTextStyle() const;

    /// Select all text
    FXbool selectAll();

    /// Select len characters starting at given position pos
    FXbool setSelection(FXint pos,FXint len);

    /// Extend the selection from the anchor to the given position
    FXbool extendSelection(FXint pos);

    /// Unselect the text
    FXbool killSelection();

    /// Return TRUE if position pos is selected
    FXbool isPosSelected(FXint pos) const;

    /// Return TRUE if position is fully visible
    FXbool isPosVisible(FXint pos) const;

    /// Scroll text to make the given position visible
    void makePositionVisible(FXint pos);

    /// Save text field to a stream
    virtual void save(FXStream& store) const;

    /// Load text field from a stream
    virtual void load(FXStream& store);

    /// Destructor
    virtual ~MFXTextFieldIcon();

protected:
    FXString      contents;       // Edited text
    const FXchar *delimiters;     // Set of delimiters
    FXFont       *font;           // Text font
    FXColor       textColor;      // Text color
    FXColor       selbackColor;   // Selected background color
    FXColor       seltextColor;   // Selected text color
    FXColor       cursorColor;    // Color of the Cursor
    FXint         cursor;         // Cursor position
    FXint         anchor;         // Anchor position
    FXint         columns;        // Number of columns visible
    FXint         shift;          // Shift amount
    FXString      clipped;        // Clipped text
    FXString      help;           // Help string
    FXString      tip;            // Tooltip

    MFXTextFieldIcon();
    FXint index(FXint x) const;
    FXint coord(FXint i) const;
    void drawCursor(FXuint state);
    void drawTextRange(FXDCWindow& dc,FXint fm,FXint to);
    void drawTextFragment(FXDCWindow& dc,FXint x,FXint y,FXint fm,FXint to);
    void drawPWDTextFragment(FXDCWindow& dc,FXint x,FXint y,FXint fm,FXint to);
    FXint rightWord(FXint pos) const;
    FXint leftWord(FXint pos) const;
    FXint wordStart(FXint pos) const;
    FXint wordEnd(FXint pos) const;

private:
    MFXTextFieldIcon(const MFXTextFieldIcon&) = delete;
    MFXTextFieldIcon& operator=(const MFXTextFieldIcon&) = delete;
};