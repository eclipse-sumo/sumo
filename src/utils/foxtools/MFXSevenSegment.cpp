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
/// @file    MFXSevenSegment.cpp
/// @author  Mathew Robertson
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004-03-19
///
//
/****************************************************************************/
#include <config.h>

#include "MFXSevenSegment.h"


/// @brief note: this class may change into FXLCDsegment, so as to support 7 or 14 segment display
#define ASCII_ZERO 48

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXSevenSegment) MFXSevenSegmentMap[] = {
    FXMAPFUNC(SEL_PAINT,    0,                              MFXSevenSegment::onPaint),
    FXMAPFUNC(SEL_COMMAND,  FXWindow::ID_SETVALUE,          MFXSevenSegment::onCmdSetValue),
    FXMAPFUNC(SEL_COMMAND,  FXWindow::ID_SETINTVALUE,       MFXSevenSegment::onCmdSetIntValue),
    FXMAPFUNC(SEL_COMMAND,  FXWindow::ID_GETINTVALUE,       MFXSevenSegment::onCmdGetIntValue),
    FXMAPFUNC(SEL_COMMAND,  FXWindow::ID_SETSTRINGVALUE,    MFXSevenSegment::onCmdSetStringValue),
    FXMAPFUNC(SEL_COMMAND,  FXWindow::ID_GETSTRINGVALUE,    MFXSevenSegment::onCmdGetStringValue),
    //FXMAPFUNC(SEL_UPDATE, FXWindow::ID_QUERY_TIP,         MFXSevenSegment::onQueryTip),
    //FXMAPFUNC(SEL_UPDATE, FXWindow::ID_QUERY_HELP,        MFXSevenSegment::onQueryHelp),
};

// Object implementation
FXIMPLEMENT(MFXSevenSegment, FXFrame, MFXSevenSegmentMap, ARRAYNUMBER(MFXSevenSegmentMap))


// ===========================================================================
// method definitions
// ===========================================================================

MFXSevenSegment::MFXSevenSegment(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint pl, FXint pr, FXint pt, FXint pb) :
    FXFrame(p, opts, 0, 0, 0, 0, pl, pr, pt, pb),
    myValue(' '),
    myLCDTextColor(FXRGB(0, 255, 0)),
    myBackGroundColor(FXRGB(0, 0, 0)),
    myHorizontalSegmentLength(8),
    myVerticalSegmentLength(8),
    mySegmentThickness(3),
    myGroove(1) {
    setTarget(tgt);
    setSelector(sel);
    enable();
}


FXint
MFXSevenSegment::getDefaultWidth() {
    return padleft + (myGroove << 1) + myHorizontalSegmentLength + padright + (border << 1);
}


FXint
MFXSevenSegment::getDefaultHeight() {
    return padtop + (myGroove << 2) + (myVerticalSegmentLength << 1) + padbottom + (border << 1);
}


void
MFXSevenSegment::setText(FXchar val) {
    if (FXString(val, 1).upper() != FXString(myValue, 1).upper()) {
        myValue = val;
        recalc();
        update();
    }
}


void
MFXSevenSegment::setFgColor(const FXColor clr) {
    if (myLCDTextColor != clr) {
        myLCDTextColor = clr;
        recalc();
        update();
    }
}


void
MFXSevenSegment::setBgColor(const FXColor clr) {
    if (myBackGroundColor != clr) {
        myBackGroundColor = clr;
        recalc();
        update();
    }
}


void
MFXSevenSegment::setHorizontal(const FXint len) {
    if (len != myHorizontalSegmentLength) {
        myHorizontalSegmentLength = (FXshort)len;
        checkSize();
        recalc();
        update();
    }
}


void
MFXSevenSegment::setVertical(const FXint len) {
    if (len != myVerticalSegmentLength) {
        myVerticalSegmentLength = (FXshort)len;
        checkSize();
        recalc();
        update();
    }
}


void
MFXSevenSegment::setThickness(const FXint w) {
    if (w != mySegmentThickness) {
        mySegmentThickness = (FXshort)w;
        checkSize();
        recalc();
        update();
    }
}


void
MFXSevenSegment::setGroove(const FXint w) {
    if (w != myGroove) {
        myGroove = (FXshort)w;
        checkSize();
        recalc();
        update();
    }
}


long
MFXSevenSegment::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*) ptr;
    FXDCWindow dc(this, event);
    drawFrame(dc, 0, 0, width, height);
    dc.setForeground(myBackGroundColor);
    dc.fillRectangle(border, border, width - (border << 1), height - (border << 1));
    dc.setForeground(myLCDTextColor);
    drawFigure(dc, myValue);
    return 1;
}


long
MFXSevenSegment::onCmdSetValue(FXObject*, FXSelector, void* ptr) {
    FXchar* c = (FXchar*)ptr;
    if (c[0] != '\0') {
        setText(c[0]);
    }
    return 1;
}


long
MFXSevenSegment::onCmdGetIntValue(FXObject* sender, FXSelector, void*) {
    FXint i = myValue - ASCII_ZERO;
    if (i < 0) {
        i = 0;
    }
    if (i > 9) {
        i = 9;
    }
    sender->handle(this, FXSEL(SEL_COMMAND, ID_SETINTVALUE), (void*)&i);
    return 1;
}


long
MFXSevenSegment::onCmdSetIntValue(FXObject*, FXSelector, void* ptr) {
    FXint i = *((FXint*)ptr);
    if (i < 0) {
        i = 0;
    }
    if (i > 9) {
        i = 9;
    }
    setText((FXchar)(i + ASCII_ZERO));
    return 1;
}


long
MFXSevenSegment::onCmdGetStringValue(FXObject* sender, FXSelector, void*) {
    FXString s(myValue, 1);
    sender->handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), (void*)&s);
    return 1;
}


long
MFXSevenSegment::onCmdSetStringValue(FXObject*, FXSelector, void* ptr) {
    FXString* s = (FXString*)ptr;
    if (s->length()) {
        setText(s->at(0));
    }
    return 1;
}


void
MFXSevenSegment::save(FXStream& store) const {
    FXFrame::save(store);
    store << myValue;
    store << myLCDTextColor;
    store << myBackGroundColor;
    store << myHorizontalSegmentLength;
    store << myVerticalSegmentLength;
    store << mySegmentThickness;
    store << myGroove;
}


void
MFXSevenSegment::load(FXStream& store) {
    FXFrame::load(store);
    store >> myValue;
    store >> myLCDTextColor;
    store >> myBackGroundColor;
    store >> myHorizontalSegmentLength;
    store >> myVerticalSegmentLength;
    store >> mySegmentThickness;
    store >> myGroove;
}


long
MFXSevenSegment::onQueryTip(FXObject* sender, FXSelector sel, void* ptr) {
    if (getParent()) {
        return getParent()->handle(sender, sel, ptr);
    }
    return 0;
}


long
MFXSevenSegment::onQueryHelp(FXObject* sender, FXSelector sel, void* ptr) {
    if (getParent()) {
        return getParent()->handle(sender, sel, ptr);
    }
    return 0;
}


void
MFXSevenSegment::drawTopSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[4];
    points[0].x = x;
    points[0].y = y;
    points[1].x = x + myHorizontalSegmentLength;
    points[1].y = y;
    points[2].x = x + myHorizontalSegmentLength - mySegmentThickness;
    points[2].y = y + mySegmentThickness;
    points[3].x = x + mySegmentThickness;
    points[3].y = y + mySegmentThickness;
    dc.fillPolygon(points, 4);
}


void
MFXSevenSegment::drawLeftTopSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[4];
    points[0].x = x;
    points[0].y = y;
    points[1].x = x + mySegmentThickness;
    points[1].y = y + mySegmentThickness;
    points[2].x = x + mySegmentThickness;
    points[2].y = y + myVerticalSegmentLength - (mySegmentThickness >> 1);
    points[3].x = x;
    points[3].y = y + myVerticalSegmentLength;
    dc.fillPolygon(points, 4);
}


void
MFXSevenSegment::drawRightTopSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[4];
    points[0].x = x + mySegmentThickness;
    points[0].y = y;
    points[1].x = x + mySegmentThickness;
    points[1].y = y + myVerticalSegmentLength;
    points[2].x = x;
    points[2].y = y + myVerticalSegmentLength - (mySegmentThickness >> 1);
    points[3].x = x;
    points[3].y = y + mySegmentThickness;
    dc.fillPolygon(points, 4);
}


void
MFXSevenSegment::drawMiddleSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[6];
    points[0].x = x + mySegmentThickness;
    points[0].y = y;
    points[1].x = x + myHorizontalSegmentLength - mySegmentThickness;
    points[1].y = y;
    points[2].x = x + myHorizontalSegmentLength;
    points[2].y = y + (mySegmentThickness >> 1);
    points[3].x = x + myHorizontalSegmentLength - mySegmentThickness;
    points[3].y = y + mySegmentThickness;
    points[4].x = x + mySegmentThickness;
    points[4].y = y + mySegmentThickness;
    points[5].x = x;
    points[5].y = y + (mySegmentThickness >> 1);
    dc.fillPolygon(points, 6);
}


void
MFXSevenSegment::drawLeftBottomSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[4];
    points[0].x = x;
    points[0].y = y;
    points[1].x = x + mySegmentThickness;
    points[1].y = y + (mySegmentThickness >> 1);
    points[2].x = x + mySegmentThickness;
    points[2].y = y + myVerticalSegmentLength - mySegmentThickness;
    points[3].x = x;
    points[3].y = y + myVerticalSegmentLength;
    dc.fillPolygon(points, 4);
}


void
MFXSevenSegment::drawRightBottomSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[4];
    points[0].x = x + mySegmentThickness;
    points[0].y = y;
    points[1].x = x + mySegmentThickness;
    points[1].y = y + myVerticalSegmentLength;
    points[2].x = x;
    points[2].y = y + myVerticalSegmentLength - mySegmentThickness;
    points[3].x = x;
    points[3].y = y + (mySegmentThickness >> 1);
    dc.fillPolygon(points, 4);
}


void
MFXSevenSegment::drawBottomSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[4];
    points[0].x = x + mySegmentThickness;
    points[0].y = y;
    points[1].x = x + myHorizontalSegmentLength - mySegmentThickness;
    points[1].y = y;
    points[2].x = x + myHorizontalSegmentLength;
    points[2].y = y + mySegmentThickness;
    points[3].x = x;
    points[3].y = y + mySegmentThickness;
    dc.fillPolygon(points, 4);
}


void
MFXSevenSegment::drawSegments(FXDCWindow& dc, FXbool s1, FXbool s2, FXbool s3, FXbool s4, FXbool s5, FXbool s6, FXbool s7) {
    FXshort sx = (FXshort)(border + padleft), sy = (FXshort)(border + padtop);
    FXshort x, y;
    if (options & LAYOUT_FILL) {
        if (options & LAYOUT_FILL_X) {
            myHorizontalSegmentLength = (FXshort)(width - padleft - padright - (border << 1));
            if (myHorizontalSegmentLength < 4) {
                myHorizontalSegmentLength = 4;
            }
        }
        if (options & LAYOUT_FILL_Y) {
            myVerticalSegmentLength = (FXshort)(height - padtop - padbottom - (border << 1)) >> 1;
            if (myVerticalSegmentLength < 4) {
                myVerticalSegmentLength = 4;
            }
        }
        mySegmentThickness = FXMIN(myHorizontalSegmentLength, myVerticalSegmentLength) / 4;
        myGroove = mySegmentThickness / 4;
        if (mySegmentThickness < 1) {
            mySegmentThickness = 1;
        }
        if (myGroove < 1) {
            myGroove = 1;
        }
        if (options & LAYOUT_FILL_X) {
            myHorizontalSegmentLength -= myGroove << 1;
        }
        if (options & LAYOUT_FILL_Y) {
            myVerticalSegmentLength -= myGroove << 1;
        }
    }
    if (s1) {
        x = sx + myGroove;
        y = sy;
        drawTopSegment(dc, x, y);
    }
    if (s2) {
        x = sx;
        y = sy + myGroove;
        drawLeftTopSegment(dc, x, y);
    }
    if (s3) {
        x = sx + myGroove + myHorizontalSegmentLength - mySegmentThickness + myGroove;
        y = sy + myGroove;
        drawRightTopSegment(dc, x, y);
    }
    if (s4) {
        x = sx + myGroove;
        y = sy + myGroove + myVerticalSegmentLength - (mySegmentThickness >> 1) + myGroove;
        drawMiddleSegment(dc, x, y);
    }
    if (s5) {
        x = sx;
        y = sy + (myGroove << 1) + myVerticalSegmentLength + myGroove;
        drawLeftBottomSegment(dc, x, y);
    }
    if (s6) {
        x = sx + myGroove + myHorizontalSegmentLength - mySegmentThickness + myGroove;
        y = sy + (myGroove << 1) + myVerticalSegmentLength + myGroove;
        drawRightBottomSegment(dc, x, y);
    }
    if (s7) {
        x = sx + myGroove;
        y = sy + (myGroove << 1) + myVerticalSegmentLength + myGroove + myVerticalSegmentLength + myGroove - mySegmentThickness;
        drawBottomSegment(dc, x, y);
    }
}


void
MFXSevenSegment::drawFigure(FXDCWindow& dc, FXchar figure) {
    switch (figure) {
        case ' ' :
            drawSegments(dc, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
            break;
        case '(' :
            drawSegments(dc, TRUE, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE);
            break;
        case ')' :
            drawSegments(dc, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE, TRUE);
            break;
        case '[' :
            drawSegments(dc, TRUE, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE);
            break;
        case ']' :
            drawSegments(dc, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE, TRUE);
            break;
        case '=' :
            drawSegments(dc, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, TRUE);
            break;
//    case '+' : drawSegments (dc, FALSE,FALSE,FALSE,TRUE ,FALSE,FALSE,FALSE); break;
        case '-' :
        case ':' :
            drawSegments(dc, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE);
            break;
        case '_' :
        case '.' :
        case ',' :
            drawSegments(dc, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE);
            break;
        case '0' :
            drawSegments(dc, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE);
            break;
        case '1' :
            drawSegments(dc, FALSE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE);
            break;
        case '2' :
            drawSegments(dc, TRUE, FALSE, TRUE, TRUE, TRUE, FALSE, TRUE);
            break;
        case '3' :
            drawSegments(dc, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE);
            break;
        case '4' :
            drawSegments(dc, FALSE, TRUE, TRUE, TRUE, FALSE, TRUE, FALSE);
            break;
        case '5' :
            drawSegments(dc, TRUE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE);
            break;
        case '6' :
            drawSegments(dc, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE);
            break;
        case '7' :
            drawSegments(dc, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE);
            break;
        case '8' :
            drawSegments(dc, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
            break;
        case '9' :
            drawSegments(dc, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE);
            break;
        case 'a' :
        case 'A' :
            drawSegments(dc, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE);
            break;
        case 'b' :
        case 'B' :
            drawSegments(dc, FALSE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE);
            break;
        case 'c' :
        case 'C' :
            drawSegments(dc, TRUE, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE);
            break;
        case 'd' :
        case 'D' :
            drawSegments(dc, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE);
            break;
        case 'e' :
        case 'E' :
            drawSegments(dc, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE);
            break;
        case 'f' :
        case 'F' :
            drawSegments(dc, TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, FALSE);
            break;
        case 'g' :
        case 'G' :
            drawSegments(dc, TRUE, TRUE, FALSE, FALSE, TRUE, TRUE, TRUE);
            break;
        case 'h' :
        case 'H' :
            drawSegments(dc, FALSE, TRUE, FALSE, TRUE, TRUE, TRUE, FALSE);
            break;
        case 'i' :
        case 'I' :
            drawSegments(dc, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE);
            break;
        case 'j' :
        case 'J' :
            drawSegments(dc, FALSE, FALSE, TRUE, FALSE, TRUE, TRUE, TRUE);
            break;
//    case 'k' :
//    case 'k' : drawSegments (dc, FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE); break;
        case 'l' :
        case 'L' :
            drawSegments(dc, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE);
            break;
//    case 'm' :
//    case 'M' : drawSegments (dc, FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE); break;
        case 'n' :
        case 'N' :
            drawSegments(dc, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE);
            break;
        case 'o' :
        case 'O' :
            drawSegments(dc, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE);
            break;
        case 'p' :
        case 'P' :
            drawSegments(dc, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE);
            break;
        case 'q' :
        case 'Q' :
            drawSegments(dc, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, FALSE);
            break;
        case 'r' :
        case 'R' :
            drawSegments(dc, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE);
            break;
        case 's' :
        case 'S' :
            drawSegments(dc, TRUE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE);
            break;
        case 't' :
        case 'T' :
            drawSegments(dc, FALSE, TRUE, FALSE, TRUE, TRUE, FALSE, FALSE);
            break;
        case 'u' :
        case 'U' :
            drawSegments(dc, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE);
            break;
//    case 'v' :
//    case 'V' : drawSegments (dc, FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE); break;
//    case 'w' :
//    case 'W' : drawSegments (dc, FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE); break;
        case 'x' :
        case 'X' :
            drawSegments(dc, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE);
            break;
        case 'y' :
        case 'Y' :
            drawSegments(dc, FALSE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE);
            break;
//    case 'z' :
//    case 'Z' :
        default  :
            fxerror("MFXSevenSegment doesn't support: %c\n", figure);
    }
}


void
MFXSevenSegment::checkSize() {
    if (myHorizontalSegmentLength < 3) {
        myHorizontalSegmentLength = 3;
        mySegmentThickness = 1;
    }
    if (myVerticalSegmentLength < 3) {
        myVerticalSegmentLength = 3;
        mySegmentThickness = 1;
    }
    if (mySegmentThickness < 1) {
        mySegmentThickness = 1;
    }
    if (myHorizontalSegmentLength < (mySegmentThickness << 1)) {
        myHorizontalSegmentLength = (mySegmentThickness << 1) + 1;
    }
    if (myVerticalSegmentLength < (mySegmentThickness << 1)) {
        myVerticalSegmentLength = (mySegmentThickness << 1) + 1;
    }
    if (myHorizontalSegmentLength < 8 || myVerticalSegmentLength < 8) {
        myGroove = 2;
    }
    if (myHorizontalSegmentLength < 1 || myVerticalSegmentLength < 3 || mySegmentThickness < 3) {
        myGroove = 1;
    }
    if (myGroove >= mySegmentThickness) {
        myGroove = mySegmentThickness - 1;
    }
}
