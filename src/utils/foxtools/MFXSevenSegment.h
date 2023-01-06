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
/// @file    MFXSevenSegment.h
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

/// @brief Seven-segment (eg LCD/watch style) widget
class MFXSevenSegment : public FXFrame {
    /// @brief FOX declaration
    FXDECLARE(MFXSevenSegment)

public:
    /// @brief create a seven segment display
    MFXSevenSegment(FXComposite* p, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = FRAME_NONE, FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// @brief destructor
    virtual ~MFXSevenSegment() {}

    /// @brief set the text on the display
    void setText(const FXchar val);

    /// @brief get the text on the display
    FXchar getText() const {
        return myValue;
    }

    /// @brief get/set foreground color
    void setFgColor(const FXColor clr);
    FXColor getFgColor() const {
        return myLCDTextColor;
    }

    /// @brief get/set background color
    void setBgColor(const FXColor clr);
    FXColor getBgColor() const {
        return myBackGroundColor;
    }

    /// @brief get/set horizontal segment length
    void setHorizontal(const FXint len);
    FXint getHorizontal() const {
        return myHorizontalSegmentLength;
    }

    /// @brief get/set vertical segment length
    void setVertical(const FXint len);
    FXint getVertical() const {
        return myVerticalSegmentLength;
    }

    /// @brief get/set segment thickness
    void setThickness(const FXint w);
    FXint getThickness() const {
        return mySegmentThickness;
    }

    /// @brief get/set myGroove thickness
    void setGroove(const FXint w);
    FXint getGroove() const {
        return myGroove;
    }

    /// @brief draw/redraw object
    long onPaint(FXObject*, FXSelector, void*);

    /// @brief set from value
    long onCmdSetValue(FXObject*, FXSelector, void*);

    /// @brief set from int value
    long onCmdSetIntValue(FXObject*, FXSelector, void*);

    /// @brief get from int value
    long onCmdGetIntValue(FXObject*, FXSelector, void*);

    /// @brief set from string value
    long onCmdSetStringValue(FXObject*, FXSelector, void*);

    /// @brief get from string value
    long onCmdGetStringValue(FXObject*, FXSelector, void*);

    /// @brief let parent show tip if appropriate
    long onQueryTip(FXObject*, FXSelector, void*);

    /// @brief let parent show help if appropriate
    long onQueryHelp(FXObject*, FXSelector, void*);

    /// @brief Return minimum width
    virtual FXint getDefaultWidth();

    /// @brief Return minimum height
    virtual FXint getDefaultHeight();

    /// @brief save resources
    virtual void save(FXStream& store) const;

    /// @brief load resources
    virtual void load(FXStream& store);

protected:
    /// @brief FOX constructor
    FOX_CONSTRUCTOR(MFXSevenSegment)

    /// @brief Draws the individual segment types
    void drawTopSegment(FXDCWindow& dc, FXshort x, FXshort y);
    void drawLeftTopSegment(FXDCWindow& dc, FXshort x, FXshort y);
    void drawRightTopSegment(FXDCWindow& dc, FXshort x, FXshort y);
    void drawMiddleSegment(FXDCWindow& dc, FXshort x, FXshort y);
    void drawLeftBottomSegment(FXDCWindow& dc, FXshort x, FXshort y);
    void drawRightBottomSegment(FXDCWindow& dc, FXshort x, FXshort y);
    void drawBottomSegment(FXDCWindow& dc, FXshort x, FXshort y);

    /// @brief Draw a seven-segment unit (each segment can be set indepentantly)
    void drawSegments(FXDCWindow& dc, FXbool s1, FXbool s2, FXbool s3, FXbool s4, FXbool s5, FXbool s6, FXbool s7);

    /// @brief Draw an alphanumeric figure (consisting of seven segments)
    virtual void drawFigure(FXDCWindow& dc, FXchar figure);

private:
    /// @brief The currently shown character
    FXchar myValue;

    /// @brief The color of the LCD text
    FXColor myLCDTextColor;

    /// @brief The color of the LCD background
    FXColor myBackGroundColor;

    /// @brief This is pixel length of a horizontal segment
    FXshort myHorizontalSegmentLength;

    /// @brief This is pixel length of a vertical segment
    FXshort myVerticalSegmentLength;

    /// @brief This is segment thickness, in pixels
    FXshort mySegmentThickness;

    /// @brief Groove between segments
    FXshort myGroove;

    /// @brief validates the sizes of the segment dimensions
    void checkSize();
};
