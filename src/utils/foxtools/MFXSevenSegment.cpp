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
/// @file    MFXSevenSegment.cpp
/// @author  Mathew Robertson
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004-03-19
///
//
/****************************************************************************/


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <config.h>

#define NOMINMAX
#undef NOMINMAX
#include "fxheader.h"
/*
#include <FXStream.h>
#include <FXString.h>
#include <FXSize.h>
#include <FXPoint.h>
#include <FXRectangle.h>
#include <FXRegistry.h>
#include <FXHash.h>
#include <FXApp.h>
#include <FXDCWindow.h>
*/
using namespace FX;
#include "MFXSevenSegment.h"

using namespace FXEX;
namespace FXEX {

/* note: this class may change into FXLCDsegment, so as to support 7 or 14 segment display */
#define ASCII_ZERO 48

// map
FXDEFMAP(MFXSevenSegment) MFXSevenSegmentMap[] = {
    FXMAPFUNC(SEL_PAINT, 0, MFXSevenSegment::onPaint),
    FXMAPFUNC(SEL_COMMAND, FXWindow::ID_SETVALUE, MFXSevenSegment::onCmdSetValue),
    FXMAPFUNC(SEL_COMMAND, FXWindow::ID_SETINTVALUE, MFXSevenSegment::onCmdSetIntValue),
    FXMAPFUNC(SEL_COMMAND, FXWindow::ID_GETINTVALUE, MFXSevenSegment::onCmdGetIntValue),
    FXMAPFUNC(SEL_COMMAND, FXWindow::ID_SETSTRINGVALUE, MFXSevenSegment::onCmdSetStringValue),
    FXMAPFUNC(SEL_COMMAND, FXWindow::ID_GETSTRINGVALUE, MFXSevenSegment::onCmdGetStringValue),
    //  FXMAPFUNC(SEL_UPDATE,FXWindow::ID_QUERY_TIP,MFXSevenSegment::onQueryTip),
    //  FXMAPFUNC(SEL_UPDATE,FXWindow::ID_QUERY_HELP,MFXSevenSegment::onQueryHelp),
};
FXIMPLEMENT(MFXSevenSegment, FXFrame, MFXSevenSegmentMap, ARRAYNUMBER(MFXSevenSegmentMap))

// ctor
MFXSevenSegment::MFXSevenSegment(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint pl, FXint pr, FXint pt, FXint pb) : FXFrame(p, opts, 0, 0, 0, 0, pl, pr, pt, pb), value(' '), fgcolor(FXRGB(0, 255, 0)), bgcolor(FXRGB(0, 0, 0)), hsl(8), vsl(8), st(3), groove(1) {
    setTarget(tgt);
    setSelector(sel);
    enable();
}

// minimum width
FXint MFXSevenSegment::getDefaultWidth() {
    return padleft + (groove << 1) + hsl + padright + (border << 1);
}

// minimum height
FXint MFXSevenSegment::getDefaultHeight() {
    return padtop + (groove << 2) + (vsl << 1) + padbottom + (border << 1);
}

// set value on widget
void MFXSevenSegment::setText(FXchar val) {
    if (FXString(val, 1).upper() != FXString(value, 1).upper()) {
        value = val;
        recalc();
        update();
    }
}

// set foreground color
void MFXSevenSegment::setFgColor(const FXColor clr) {
    if (fgcolor != clr) {
        fgcolor = clr;
        recalc();
        update();
    }
}

// set backgound color
void MFXSevenSegment::setBgColor(const FXColor clr) {
    if (bgcolor != clr) {
        bgcolor = clr;
        recalc();
        update();
    }
}

// set horizontal segment length
void MFXSevenSegment::setHorizontal(const FXint len) {
    if (len != hsl) {
        hsl = (FXshort)len;
        checkSize();
        recalc();
        update();
    }
}

// set vertical segment length
void MFXSevenSegment::setVertical(const FXint len) {
    if (len != vsl) {
        vsl = (FXshort)len;
        checkSize();
        recalc();
        update();
    }
}

// set segment thickness
void MFXSevenSegment::setThickness(const FXint w) {
    if (w != st) {
        st = (FXshort)w;
        checkSize();
        recalc();
        update();
    }
}

// set groove thickness
void MFXSevenSegment::setGroove(const FXint w) {
    if (w != groove) {
        groove = (FXshort)w;
        checkSize();
        recalc();
        update();
    }
}

// draw/redraw object
long MFXSevenSegment::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*) ptr;
    FXDCWindow dc(this, event);
    drawFrame(dc, 0, 0, width, height);
    dc.setForeground(bgcolor);
    dc.fillRectangle(border, border, width - (border << 1), height - (border << 1));
    dc.setForeground(fgcolor);
    drawFigure(dc, value);
    return 1;
}

// set from value
long MFXSevenSegment::onCmdSetValue(FXObject*, FXSelector, void* ptr) {
    FXchar* c = (FXchar*)ptr;
    if (c[0] != '\0') {
        setText(c[0]);
    }
    return 1;
}

// get value from int
long MFXSevenSegment::onCmdGetIntValue(FXObject* sender, FXSelector, void*) {
    FXint i = value - ASCII_ZERO;
    if (i < 0) {
        i = 0;
    }
    if (i > 9) {
        i = 9;
    }
    sender->handle(this, FXSEL(SEL_COMMAND, ID_SETINTVALUE), (void*)&i);
    return 1;
}

// set from int value
long MFXSevenSegment::onCmdSetIntValue(FXObject*, FXSelector, void* ptr) {
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

// get value from string
long MFXSevenSegment::onCmdGetStringValue(FXObject* sender, FXSelector, void*) {
    FXString s(value, 1);
    sender->handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), (void*)&s);
    return 1;
}

// set from string value
long MFXSevenSegment::onCmdSetStringValue(FXObject*, FXSelector, void* ptr) {
    FXString* s = (FXString*)ptr;
    if ((*s).length()) {
        setText((*s)[0]);
    }
    return 1;
}

// draw the specific character - figure out which segments to draw
void MFXSevenSegment::drawFigure(FXDCWindow& dc, FXchar figure) {
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
            fxerror("MFXSevenSegment doesnt support: %c\n", figure);
    }
}

// validates the sizes of the segment dimensions
void MFXSevenSegment::checkSize() {
    if (hsl < 3) {
        hsl = 3;
        st = 1;
    }
    if (vsl < 3) {
        vsl = 3;
        st = 1;
    }
    if (st < 1) {
        st = 1;
    }
    if (hsl < (st << 1)) {
        hsl = (st << 1) + 1;
    }
    if (vsl < (st << 1)) {
        vsl = (st << 1) + 1;
    }
    if (hsl < 8 || vsl < 8) {
        groove = 2;
    }
    if (hsl < 1 || vsl < 3 || st < 3) {
        groove = 1;
    }
    if (groove >= st) {
        groove = st - 1;
    }
}

// draw each segment, into the available drawing space
// if widget is resizeable, caculate new sizes for length/width/grove of each segment
void MFXSevenSegment::drawSegments(FXDCWindow& dc, FXbool s1, FXbool s2, FXbool s3, FXbool s4, FXbool s5, FXbool s6, FXbool s7) {
    FXshort sx = (FXshort)(border + padleft), sy = (FXshort)(border + padtop);
    FXshort x, y;
    if (options & LAYOUT_FILL) {
        if (options & LAYOUT_FILL_X) {
            hsl = (FXshort)(width - padleft - padright - (border << 1));
            if (hsl < 4) {
                hsl = 4;
            }
        }
        if (options & LAYOUT_FILL_Y) {
            vsl = (FXshort)(height - padtop - padbottom - (border << 1)) >> 1;
            if (vsl < 4) {
                vsl = 4;
            }
        }
        st = FXMIN(hsl, vsl) / 4;
        groove = st / 4;
        if (st < 1) {
            st = 1;
        }
        if (groove < 1) {
            groove = 1;
        }
        if (options & LAYOUT_FILL_X) {
            hsl -= groove << 1;
        }
        if (options & LAYOUT_FILL_Y) {
            vsl -= groove << 1;
        }
    }
    if (s1) {
        x = sx + groove;
        y = sy;
        drawTopSegment(dc, x, y);
    }
    if (s2) {
        x = sx;
        y = sy + groove;
        drawLeftTopSegment(dc, x, y);
    }
    if (s3) {
        x = sx + groove + hsl - st + groove;
        y = sy + groove;
        drawRightTopSegment(dc, x, y);
    }
    if (s4) {
        x = sx + groove;
        y = sy + groove + vsl - (st >> 1) + groove;
        drawMiddleSegment(dc, x, y);
    }
    if (s5) {
        x = sx;
        y = sy + (groove << 1) + vsl + groove;
        drawLeftBottomSegment(dc, x, y);
    }
    if (s6) {
        x = sx + groove + hsl - st + groove;
        y = sy + (groove << 1) + vsl + groove;
        drawRightBottomSegment(dc, x, y);
    }
    if (s7) {
        x = sx + groove;
        y = sy + (groove << 1) + vsl + groove + vsl + groove - st;
        drawBottomSegment(dc, x, y);
    }
}

void MFXSevenSegment::drawTopSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[4];
    points[0].x = x;
    points[0].y = y;
    points[1].x = x + hsl;
    points[1].y = y;
    points[2].x = x + hsl - st;
    points[2].y = y + st;
    points[3].x = x + st;
    points[3].y = y + st;
    dc.fillPolygon(points, 4);
}

void MFXSevenSegment::drawLeftTopSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[4];
    points[0].x = x;
    points[0].y = y;
    points[1].x = x + st;
    points[1].y = y + st;
    points[2].x = x + st;
    points[2].y = y + vsl - (st >> 1);
    points[3].x = x;
    points[3].y = y + vsl;
    dc.fillPolygon(points, 4);
}

void MFXSevenSegment::drawRightTopSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[4];
    points[0].x = x + st;
    points[0].y = y;
    points[1].x = x + st;
    points[1].y = y + vsl;
    points[2].x = x;
    points[2].y = y + vsl - (st >> 1);
    points[3].x = x;
    points[3].y = y + st;
    dc.fillPolygon(points, 4);
}

void MFXSevenSegment::drawMiddleSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[6];
    points[0].x = x + st;
    points[0].y = y;
    points[1].x = x + hsl - st;
    points[1].y = y;
    points[2].x = x + hsl;
    points[2].y = y + (st >> 1);
    points[3].x = x + hsl - st;
    points[3].y = y + st;
    points[4].x = x + st;
    points[4].y = y + st;
    points[5].x = x;
    points[5].y = y + (st >> 1);
    dc.fillPolygon(points, 6);
}

void MFXSevenSegment::drawLeftBottomSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[4];
    points[0].x = x;
    points[0].y = y;
    points[1].x = x + st;
    points[1].y = y + (st >> 1);
    points[2].x = x + st;
    points[2].y = y + vsl - st;
    points[3].x = x;
    points[3].y = y + vsl;
    dc.fillPolygon(points, 4);
}

void MFXSevenSegment::drawRightBottomSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[4];
    points[0].x = x + st;
    points[0].y = y;
    points[1].x = x + st;
    points[1].y = y + vsl;
    points[2].x = x;
    points[2].y = y + vsl - st;
    points[3].x = x;
    points[3].y = y + (st >> 1);
    dc.fillPolygon(points, 4);
}

void MFXSevenSegment::drawBottomSegment(FXDCWindow& dc, FXshort x, FXshort y) {
    FXPoint points[4];
    points[0].x = x + st;
    points[0].y = y;
    points[1].x = x + hsl - st;
    points[1].y = y;
    points[2].x = x + hsl;
    points[2].y = y + st;
    points[3].x = x;
    points[3].y = y + st;
    dc.fillPolygon(points, 4);
}

void MFXSevenSegment::save(FXStream& store) const {
    FXFrame::save(store);
    store << value;
    store << fgcolor;
    store << bgcolor;
    store << hsl;
    store << vsl;
    store << st;
    store << groove;
}

void MFXSevenSegment::load(FXStream& store) {
    FXFrame::load(store);
    store >> value;
    store >> fgcolor;
    store >> bgcolor;
    store >> hsl;
    store >> vsl;
    store >> st;
    store >> groove;
}

// let parent show tip if appropriate
long MFXSevenSegment::onQueryTip(FXObject* sender, FXSelector sel, void* ptr) {
    if (getParent()) {
        return getParent()->handle(sender, sel, ptr);
    }
    return 0;
}

// let parent show help if appropriate
long MFXSevenSegment::onQueryHelp(FXObject* sender, FXSelector sel, void* ptr) {
    if (getParent()) {
        return getParent()->handle(sender, sel, ptr);
    }
    return 0;
}

}

