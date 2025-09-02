/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2025 German Aerospace Center (DLR) and others.
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

#include <utils/gui/images/GUIIconSubSys.h>

#include "MFXStaticToolTip.h"

// ===========================================================================
// class definitions
// ===========================================================================

class MFXTextFieldIcon : public FXFrame {
    /// @brief FOX declaration
    FXDECLARE(MFXTextFieldIcon)

public:
    /// @brief Construct text field wide enough to display ncols columns
    MFXTextFieldIcon(FXComposite* p, MFXStaticToolTip* staticToolTip, GUIIcon icon, FXObject* tgt,
                     FXSelector sel, FXuint opts, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0,
                     FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// @brief Destructor
    virtual ~MFXTextFieldIcon();

    /// @brief Create server-side resources
    virtual void create();

    /// @brief Perform layout
    virtual void layout();

    /// @brief Enable text field
    virtual void enable();

    /// @brief Disable text field
    virtual void disable();

    /// @brief Return default width
    virtual FXint getDefaultWidth();

    /// @brief Return default height
    virtual FXint getDefaultHeight();

    /// @brief Yes,  text field may receive focus
    virtual bool canFocus() const;

    /// @brief Move the focus to this window
    virtual void setFocus();

    /// @brief Remove the focus from this window
    virtual void killFocus();

    /// @brief Set editable mode
    void setEditable(FXbool edit = TRUE);

    /// @brief Return TRUE if text field may be edited
    FXbool isEditable() const;

    /// @brief Set overstrike mode
    void setOverstrike(FXbool over = TRUE);

    /// @brief Return TRUE if overstrike mode in effect
    FXbool isOverstrike() const;

    /// @brief Set cursor position
    void setCursorPos(FXint pos);

    /// @brief Return cursor position
    FXint getCursorPos() const;

    /// @brief Change anchor position
    void setAnchorPos(FXint pos);

    /// @brief Return anchor position
    FXint getAnchorPos() const;

    /// @brief Change the text and move cursor to end
    void setText(const FXString& text, FXbool notify = FALSE);

    /// @brief Change the icon
    void setIcon(FXIcon* ic);

    /// @brief Get the text for this label
    FXString getText() const;

    /// @brief Set the text font
    void setFont(FXFont* fnt);

    /// @brief Get the text font
    FXFont* getFont() const;

    /// @brief Change text color
    void setTextColor(FXColor clr);

    /// @brief Return text color
    FXColor getTextColor() const;

    /// @brief Change selected background color
    void setSelBackColor(FXColor clr);

    /// @brief Return selected background color
    FXColor getSelBackColor() const {
        return mySelectedBackgroundColor;
    }

    /// @brief Change selected text color
    void setSelTextColor(FXColor clr);

    /// @brief Return selected text color
    FXColor getSelTextColor() const;

    /// @brief Changes the cursor color
    void setCursorColor(FXColor clr);

    /// @brief Return the cursor color
    FXColor getCursorColor() const;

    /// @brief Change the default width of the text field
    void setNumColumns(FXint cols);

    /// @brief Return number of columns
    FXint getNumColumns() const;

    /// @brief Change text justification mode
    void setJustify(FXuint mode);

    /// @brief Return text justification mode
    FXuint getJustify() const;

    /// @brief Change word delimiters
    void setDelimiters(const FXchar* delims = FXTextField::textDelimiters);

    /// @brief Return word delimiters
    const FXchar* getDelimiters() const;

    /// @brief Set the status line help text for this label
    void setHelpText(const FXString& text);

    /// @brief Get the status line help text for this label
    const FXString& getHelpText() const;

    /// @brief Set the tool tip message for this text field
    void setTipText(const FXString& text);

    /// @brief Get the tool tip message for this text field
    const FXString& getTipText() const;

    /// @brief Change text style
    void setTextStyle(FXuint style);

    /// @brief Return text style
    FXuint getTextStyle() const;

    /// @brief Select all text
    FXbool selectAll();

    /// @brief Select len characters starting at given position pos
    FXbool setSelection(FXint pos, FXint len);

    /// @brief Extend the selection from the anchor to the given position
    FXbool extendSelection(FXint pos);

    /// @brief Unselect the text
    FXbool killSelection();

    /// @brief Return TRUE if position pos is selected
    FXbool isPosSelected(FXint pos) const;

    /// @brief Return TRUE if position is fully visible
    FXbool isPosVisible(FXint pos) const;

    /// @brief Scroll text to make the given position visible
    void makePositionVisible(FXint pos);

    /// @brief set toolTip
    void setToolTipText(const FXString& toolTip);

    /// @brief fox callbacks
    /// @{

    long onEnter(FXObject*, FXSelector, void*);
    long onLeave(FXObject*, FXSelector, void*);
    long onPaint(FXObject*, FXSelector, void*);
    long onUpdate(FXObject*, FXSelector, void*);
    long onKeyPress(FXObject*, FXSelector, void*);
    long onKeyRelease(FXObject*, FXSelector, void*);
    long onLeftBtnPress(FXObject*, FXSelector, void*);
    long onLeftBtnRelease(FXObject*, FXSelector, void*);
    long onMiddleBtnPress(FXObject*, FXSelector, void*);
    long onMiddleBtnRelease(FXObject*, FXSelector, void*);
    long onVerify(FXObject*, FXSelector, void*);
    long onMotion(FXObject*, FXSelector, void*);
    long onSelectionLost(FXObject*, FXSelector, void*);
    long onSelectionGained(FXObject*, FXSelector, void*);
    long onSelectionRequest(FXObject*, FXSelector, void* ptr);
    long onClipboardLost(FXObject*, FXSelector, void*);
    long onClipboardGained(FXObject*, FXSelector, void*);
    long onClipboardRequest(FXObject*, FXSelector, void*);
    long onFocusSelf(FXObject*, FXSelector, void*);
    long onFocusIn(FXObject*, FXSelector, void*);
    long onFocusOut(FXObject*, FXSelector, void*);
    long onBlink(FXObject*, FXSelector, void*);
    long onAutoScroll(FXObject*, FXSelector, void*);
    long onCmdSetValue(FXObject*, FXSelector, void*);
    long onCmdSetIntValue(FXObject*, FXSelector, void*);
    long onCmdSetRealValue(FXObject*, FXSelector, void*);
    long onCmdSetStringValue(FXObject*, FXSelector, void*);
    long onCmdGetIntValue(FXObject*, FXSelector, void*);
    long onCmdGetRealValue(FXObject*, FXSelector, void*);
    long onCmdGetStringValue(FXObject*, FXSelector, void*);
    long onCmdCursorHome(FXObject*, FXSelector, void*);
    long onCmdCursorEnd(FXObject*, FXSelector, void*);
    long onCmdCursorRight(FXObject*, FXSelector, void*);
    long onCmdCursorLeft(FXObject*, FXSelector, void*);
    long onCmdCursorWordLeft(FXObject*, FXSelector, void*);
    long onCmdCursorWordRight(FXObject*, FXSelector, void*);
    long onCmdCursorWordStart(FXObject*, FXSelector, void*);
    long onCmdCursorWordEnd(FXObject*, FXSelector, void*);
    long onCmdMark(FXObject*, FXSelector, void*);
    long onCmdExtend(FXObject*, FXSelector, void*);
    long onCmdSelectAll(FXObject*, FXSelector, void*);
    long onCmdDeselectAll(FXObject*, FXSelector, void*);
    long onCmdCutSel(FXObject*, FXSelector, void*);
    long onCmdCopySel(FXObject*, FXSelector, void*);
    long onCmdPasteSel(FXObject*, FXSelector, void*);
    long onCmdPasteMiddle(FXObject*, FXSelector, void*);
    long onCmdDeleteSel(FXObject*, FXSelector, void*);
    long onCmdDeleteAll(FXObject*, FXSelector, void*);
    long onCmdOverstString(FXObject*, FXSelector, void*);
    long onCmdInsertString(FXObject*, FXSelector, void*);
    long onCmdBackspace(FXObject*, FXSelector, void*);
    long onCmdDelete(FXObject*, FXSelector, void*);
    long onCmdToggleEditable(FXObject*, FXSelector, void*);
    long onUpdToggleEditable(FXObject*, FXSelector, void*);
    long onCmdToggleOverstrike(FXObject*, FXSelector, void*);
    long onUpdToggleOverstrike(FXObject*, FXSelector, void*);
    long onUpdHaveSelection(FXObject*, FXSelector, void*);
    long onUpdSelectAll(FXObject*, FXSelector, void*);
    long onCmdSetHelp(FXObject*, FXSelector, void*);
    long onCmdGetHelp(FXObject*, FXSelector, void*);
    long onCmdSetTip(FXObject*, FXSelector, void*);
    long onCmdGetTip(FXObject*, FXSelector, void*);
    long onQueryHelp(FXObject*, FXSelector, void*);
    long onQueryTip(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief Edited text
    FXString contents;

    /// @brief Set of text delimiter
    const FXchar* myTextDelimiter = FXTextField::textDelimiters;

    /// @brief Text font
    FXFont* myFont;

    /// @brief Text color
    FXColor myTextColor = 0;

    /// @brief Selected background color
    FXColor mySelectedBackgroundColor = 0;

    /// @brief Selected text color
    FXColor mySelectedTextColor = 0;

    /// @brief Color of the Cursor
    FXColor myCursorColor = 0;

    /// @brief Cursor position
    FXint myCursorPosition = 0;

    /// @brief Anchor position
    FXint myAnchorPosition = 0;

    /// @brief Number of myVisibleColumns visible
    FXint myVisibleColumns = 0;

    /// @brief Shift amount
    FXint myShiftAmount = 0;

    /// @brief Clipped text
    FXString myClippedText;

    /// @brief Help string
    FXString myHelpText;

    /// @brief Tooltip
    FXString myTooltipText;

    /// @brief myIcon
    FXIcon* myIcon = nullptr;

    /// @brief static tooltip
    MFXStaticToolTip* myStaticToolTip = nullptr;

    /// @brief toolTip text (if set, tooltip will be always show)
    FXString myToolTipText;

    /// @brief FOX need this
    MFXTextFieldIcon();

    /// @brief index
    FXint index(FXint x) const;

    /// @brief coordinates
    FXint coord(FXint i) const;

    /// @brief draw myCursorPosition
    void drawCursor(FXuint state);

    /// @brief draw text range
    void drawTextRange(FXDCWindow& dc, FXint fm, FXint to);

    /// @brief draw text fragment
    void drawTextFragment(FXDCWindow& dc, FXint x, FXint y, FXint fm, FXint to);

    /// @brief draw password text fragment
    void drawPWDTextFragment(FXDCWindow& dc, FXint x, FXint y, FXint fm, FXint to);

    /// @brief right word
    FXint rightWord(FXint pos) const;

    /// @brief left word
    FXint leftWord(FXint pos) const;

    /// @brief word start
    FXint wordStart(FXint pos) const;

    /// @brief word end
    FXint wordEnd(FXint pos) const;

private:
    /// @brief invalidate copy constructor
    MFXTextFieldIcon(const MFXTextFieldIcon&) = delete;

    /// @brief invalidate assigment constructor
    MFXTextFieldIcon& operator=(const MFXTextFieldIcon&) = delete;
};
