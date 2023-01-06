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
/// @file    MFXLCDLabel.h
/// @author  Mathew Robertson
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Pablo Alvarez Lopez
/// @date    2004-03-19
///
//
/****************************************************************************/
#pragma once
#include <config.h>

#include "fxheader.h"
#include "MFXStaticToolTip.h"


/** @brief A widget which has the seven-segment display used as the drawing object for each letter
 * in the label.  Supports display of leading zeros, such as when you need to display a
 * number.  Also supports the 'JUSTIFY_RIGHT' option, for right alignment.
 */
class MFXLCDLabel : public FXHorizontalFrame {
    /// @brief FOX-declaration
    FXDECLARE(MFXLCDLabel)

public:
    /// @brief LCD Label styles
    enum {
        LCDLABEL_NORMAL        = FRAME_SUNKEN | FRAME_THICK,
        LCDLABEL_LEADING_ZEROS = 0x01000000    /// leading zero's on label - useful for integers
    };

    /// @brief call messages
    enum {
        ID_SEVENSEGMENT = FXHorizontalFrame::ID_LAST,
        ID_LAST
    };

public:
    /// @brief constructor
    MFXLCDLabel(FXComposite* p, MFXStaticToolTip* staticToolTip, FXuint nfig, FXObject* tgt, FXSelector sel, FXuint opts = LCDLABEL_NORMAL, FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD, FXint hs = DEFAULT_PAD);

    /// @brief destructor
    virtual ~MFXLCDLabel();

    /// @brief create resrouces
    virtual void create();

    /// @brief detach resources
    virtual void detach();

    /// @brief destroy resources
    virtual void destroy();

    /// @brief manipulate text in LCD label
    void setText(FXString lbl);

    /// @brief get text
    FXString getText() const;

    /// @brief set forground color
    void setFgColor(FXColor clr);

    /// @brief get forground color
    FXColor getFgColor() const;

    /// @brief set background color
    void setBgColor(FXColor clr);

    /// @brief get background color
    FXColor getBgColor() const;

    /// @brief set segment horizontal length - must be more than twice the segment width
    void setHorizontal(const FXint len);

    /// @brief get segment horizontal length - must be more than twice the segment width
    FXint getHorizontal() const;

    /// @brief set segment vertical length - must be more than twice the segment width
    void setVertical(const FXint len);

    /// @brief get segment vertical length - must be more than twice the segment width
    FXint getVertical() const;

    /// @brief set segment width - must be less than half the segment length
    void setThickness(const FXint w);

    /// @brief get segment width - must be less than half the segment length
    FXint getThickness() const;

    /// @brief set groove width - must be less than segment width
    void setGroove(const FXint w);

    /// @brief get groove width - must be less than segment width
    FXint getGroove() const;

    /// @brief return minimum width
    virtual FXint getDefaultWidth();

    /// @brief return minimum height
    virtual FXint getDefaultHeight();

    /// @brief set tooltip text
    void setToolTipText(const FXString& text);

    /// @brief save resources
    virtual void save(FXStream& store) const;

    /// @brief load resources
    virtual void load(FXStream& store);

    /// @brief handle paint event
    long onPaint(FXObject*, FXSelector, void*);

    /// @brief called when mouse enter in MFXMenuButtonTooltip
    long onEnter(FXObject* obj, FXSelector sel, void* ptr);

    /// @brief called when mouse leaves in MFXMenuButtonTooltip
    long onLeave(FXObject* obj, FXSelector sel, void* ptr);

    /// @brief called when mouse motion in MFXMenuButtonTooltip
    long onMotion(FXObject* obj, FXSelector sel, void* ptr);

    /// @brief Update value from a message
    long onCmdSetValue(FXObject*, FXSelector, void* ptr);

    /// @brief Update int value from a message
    long onCmdSetIntValue(FXObject*, FXSelector, void* ptr);

    /// @brief Update real value from a message
    long onCmdSetRealValue(FXObject*, FXSelector, void* ptr);

    /// @brief Update string value from a message
    long onCmdSetStringValue(FXObject*, FXSelector, void* ptr);

    /// @brief Obtain int value from text field
    long onCmdGetIntValue(FXObject*, FXSelector, void* ptr);

    /// @brief Obtain real value from text field
    long onCmdGetRealValue(FXObject*, FXSelector, void* ptr);

    /// @brief Obtain string value from text field
    long onCmdGetStringValue(FXObject*, FXSelector, void* ptr);

    /// @brief redirect events to main window
    long onRedirectEvent(FXObject*, FXSelector, void* ptr);

    /// @brief let parent show tip if appropriate
    long onQueryTip(FXObject*, FXSelector, void* ptr);

    /// @brief let parent show help if appropriate
    long onQueryHelp(FXObject*, FXSelector, void* ptr);

protected:
    /// @brief FOX constructor
    FOX_CONSTRUCTOR(MFXLCDLabel)

    /// @brief The currently shown label
    FXString myLabel;

    /// @brief The number of shown figuresi, greater than zero
    FXint myNFigures;

    /// @brief pointer to staticToolTip
    MFXStaticToolTip* myStaticToolTip;

    /// @brief toolTip text
    FXString myToolTipText;

    /// @brief Draw a string
    void drawString(const FXString& lbl);
};
