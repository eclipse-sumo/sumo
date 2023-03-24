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
/// @file    MFXLCDLabel.cpp
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

#include "MFXSevenSegment.h"
#include "MFXLCDLabel.h"
#include "MFXBaseObject.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXLCDLabel) MFXLCDLabelMap[] = {
    FXMAPFUNC(SEL_PAINT,        0,                              MFXLCDLabel::onPaint),
    FXMAPFUNC(SEL_ENTER,        0,                              MFXLCDLabel::onEnter),
    FXMAPFUNC(SEL_LEAVE,        0,                              MFXLCDLabel::onLeave),
    FXMAPFUNC(SEL_MOTION,       0,                              MFXLCDLabel::onMotion),
    FXMAPFUNC(SEL_COMMAND,      FXWindow::ID_SETVALUE,          MFXLCDLabel::onCmdSetValue),
    FXMAPFUNC(SEL_COMMAND,      FXWindow::ID_SETINTVALUE,       MFXLCDLabel::onCmdSetIntValue),
    FXMAPFUNC(SEL_COMMAND,      FXWindow::ID_SETREALVALUE,      MFXLCDLabel::onCmdSetRealValue),
    FXMAPFUNC(SEL_COMMAND,      FXWindow::ID_SETSTRINGVALUE,    MFXLCDLabel::onCmdSetStringValue),
    FXMAPFUNC(SEL_COMMAND,      FXWindow::ID_GETINTVALUE,       MFXLCDLabel::onCmdGetIntValue),
    FXMAPFUNC(SEL_COMMAND,      FXWindow::ID_GETREALVALUE,      MFXLCDLabel::onCmdGetRealValue),
    FXMAPFUNC(SEL_COMMAND,      FXWindow::ID_GETSTRINGVALUE,    MFXLCDLabel::onCmdGetStringValue),
    //FXMAPFUNC(SEL_UPDATE,     FXWindow::ID_QUERY_TIP,         MFXLCDLabel::onQueryTip),
    //FXMAPFUNC(SEL_UPDATE,     FXWindow::ID_QUERY_HELP,        MFXLCDLabel::onQueryHelp),
    // map key
    FXMAPKEY(MFXLCDLabel::ID_SEVENSEGMENT,  MFXLCDLabel::onRedirectEvent),
};

// Object implementation
FXIMPLEMENT(MFXLCDLabel, FXHorizontalFrame, MFXLCDLabelMap, ARRAYNUMBER(MFXLCDLabelMap))


// ===========================================================================
// method definitions
// ===========================================================================

MFXLCDLabel::MFXLCDLabel(FXComposite* p, MFXStaticToolTip* staticToolTip, FXuint nfig, FXObject* tgt, FXSelector sel, FXuint opts, FXint pl, FXint pr, FXint pt, FXint pb, FXint hs) :
    FXHorizontalFrame(p, opts, 0, 0, 0, 0, pl, pr, pt, pb, hs, 0),
    myNFigures(nfig),
    myStaticToolTip(staticToolTip) {
    if (nfig == 0) {
        fxerror("%s: must have at least one figure.\n", getClassName());
    }
    setTarget(tgt);
    setSelector(sel);
    enable();
    for (FXint i = 0; i < myNFigures; i++) {
        new MFXSevenSegment(this, this, ID_SEVENSEGMENT, 0, 0, 0, 0);
    }
}


MFXLCDLabel::~MFXLCDLabel() {
    /*
      for (MFXSevenSegment *child=(MFXSevenSegment*)getFirst(); child; child=(MFXSevenSegment*)child->getNext()){
        delete child;
        }
    */
}


void
MFXLCDLabel::create() {
    FXHorizontalFrame::create();
    for (FXWindow* child = getFirst(); child; child = child->getNext()) {
        child->create();
    }
}


void
MFXLCDLabel::detach() {
    for (FXWindow* child = getFirst(); child; child = child->getNext()) {
        child->detach();
    }
    FXHorizontalFrame::detach();
}


void
MFXLCDLabel::destroy() {
    for (FXWindow* child = getFirst(); child; child = child->getNext()) {
        child->destroy();
    }
    FXHorizontalFrame::destroy();
}


FXColor
MFXLCDLabel::getFgColor() const {
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    return child->getFgColor();
}


void
MFXLCDLabel::setFgColor(FXColor clr) {
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    if (clr != child->getFgColor()) {
        for (child = (MFXSevenSegment*)getFirst(); child; child = (MFXSevenSegment*)child->getNext()) {
            child->setFgColor(clr);
        }
    }
}


FXColor
MFXLCDLabel::getBgColor() const {
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    return child->getBgColor();
}


void
MFXLCDLabel::setBgColor(FXColor clr) {
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    if (clr != child->getBgColor()) {
        for (child = (MFXSevenSegment*)getFirst(); child; child = (MFXSevenSegment*)child->getNext()) {
            child->setBgColor(clr);
        }
    }
}


void
MFXLCDLabel::setText(FXString lbl) {
    if (lbl != myLabel) {
        myLabel = lbl;
        recalc();
        update();
    }
}


FXString
MFXLCDLabel::getText() const {
    return myLabel;
}


FXint
MFXLCDLabel::getHorizontal() const {
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    return child->getHorizontal();
}


void
MFXLCDLabel::setHorizontal(const FXint len) {
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    if (len != child->getHorizontal()) {
        for (child = (MFXSevenSegment*)getFirst(); child; child = (MFXSevenSegment*)child->getNext()) {
            child->setHorizontal(len);
        }
        recalc();
        update();
    }
}


FXint
MFXLCDLabel::getVertical() const {
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    return child->getVertical();
}


void
MFXLCDLabel::setVertical(const FXint len) {
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    if (len != child->getVertical()) {
        for (child = (MFXSevenSegment*)getFirst(); child; child = (MFXSevenSegment*)child->getNext()) {
            child->setVertical(len);
        }
        recalc();
        update();
    }
}


FXint
MFXLCDLabel::getThickness() const {
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    return child->getThickness();
}


void
MFXLCDLabel::setThickness(const FXint w) {
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    if (w != child->getThickness()) {
        for (child = (MFXSevenSegment*)getFirst(); child; child = (MFXSevenSegment*)child->getNext()) {
            child->setThickness(w);
        }
        recalc();
        update();
    }
}


FXint
MFXLCDLabel::getGroove() const {
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    return child->getGroove();
}


void
MFXLCDLabel::setGroove(const FXint w) {
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    if (w != child->getGroove()) {
        for (child = (MFXSevenSegment*)getFirst(); child; child = (MFXSevenSegment*)child->getNext()) {
            child->setGroove(w);
        }
        recalc();
        update();
    }
}


long
MFXLCDLabel::onCmdSetValue(FXObject*, FXSelector, void* ptr) {
    setText((const FXchar*)ptr);
    return 1;
}


long
MFXLCDLabel::onCmdSetIntValue(FXObject*, FXSelector, void* ptr) {
    setText(FXStringVal(*((FXint*)ptr)));
    return 1;
}


long
MFXLCDLabel::onCmdSetRealValue(FXObject*, FXSelector, void* ptr) {
    setText(FXStringVal(*((FXdouble*)ptr)));
    return 1;
}


long
MFXLCDLabel::onCmdSetStringValue(FXObject*, FXSelector, void* ptr) {
    setText(*((FXString*)ptr));
    return 1;
}


long
MFXLCDLabel::onCmdGetIntValue(FXObject*, FXSelector, void* ptr) {
    *((FXint*)ptr) = FXIntVal(getText());
    return 1;
}


long
MFXLCDLabel::onCmdGetRealValue(FXObject*, FXSelector, void* ptr) {
    *((FXdouble*)ptr) = FXDoubleVal(getText());
    return 1;
}


long
MFXLCDLabel::onCmdGetStringValue(FXObject*, FXSelector, void* ptr) {
    *((FXString*)ptr) = getText();
    return 1;
}


long
MFXLCDLabel::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*) ptr;
    FXDCWindow dc(this, event);
    drawFrame(dc, 0, 0, width, height);
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    // Fill the background
    dc.setForeground(child->getBgColor());
    dc.fillRectangle(border, border, width - (border << 1), height - (border << 1));
    // Draw the current string
    dc.setForeground(child->getFgColor());
    drawString(myLabel);
    return 1;
}


long
MFXLCDLabel::onEnter(FXObject* obj, FXSelector sel, void* ptr) {
    // show static toolTip depending of myToolTipText
    if (!myToolTipText.empty()) {
        // show tip show
        myStaticToolTip->showStaticToolTip(myToolTipText);
    } else {
        myStaticToolTip->hideStaticToolTip();
    }
    return FXHorizontalFrame::onEnter(obj, sel, ptr);
}


long
MFXLCDLabel::onLeave(FXObject* obj, FXSelector sel, void* ptr) {
    // hide static toolTip
    myStaticToolTip->hideStaticToolTip();
    return FXHorizontalFrame::onLeave(obj, sel, ptr);
}


long
MFXLCDLabel::onMotion(FXObject* sender, FXSelector sel, void* ptr) {
    // update static tooltip
    myStaticToolTip->onUpdate(sender, sel, ptr);
    return FXHorizontalFrame::onMotion(sender, sel, ptr);
}


long
MFXLCDLabel::onRedirectEvent(FXObject*, FXSelector sel, void* ptr) {
    FXuint seltype = FXSELTYPE(sel);
    if (isEnabled()) {
        if (target) {
            target->handle(this, FXSEL(seltype, message), ptr);
        }
    }
    return 1;
}


FXint
MFXLCDLabel::getDefaultWidth() {
    return padleft + getFirst()->getDefaultWidth() * myNFigures + hspacing * (myNFigures - 1) + padright + (border << 1);
}


FXint
MFXLCDLabel::getDefaultHeight() {
    return padtop + getFirst()->getDefaultHeight() + padbottom + (border << 1);
}


void
MFXLCDLabel::setToolTipText(const FXString& text) {
    myToolTipText = text;
}


void
MFXLCDLabel::save(FXStream& store) const {
    FXHorizontalFrame::save(store);
    store << myLabel;
    store << myNFigures;
}


void
MFXLCDLabel::load(FXStream& store) {
    FXHorizontalFrame::load(store);
    store >> myLabel;
    store >> myNFigures;
}


long
MFXLCDLabel::onQueryTip(FXObject* sender, FXSelector sel, void* ptr) {
    if (getParent()) {
        return getParent()->handle(sender, sel, ptr);
    }
    return 0;
}


long
MFXLCDLabel::onQueryHelp(FXObject* sender, FXSelector sel, void* ptr) {
    if (getParent()) {
        return getParent()->handle(sender, sel, ptr);
    }
    return 0;
}


void
MFXLCDLabel::drawString(const FXString& lbl) {
    FXint i = 0;
    FXString displayString(' ', myNFigures);
    if (options & LCDLABEL_LEADING_ZEROS && (FXIntVal(lbl) || lbl == "0")) {
        FXString txt = lbl;
        if (txt[0] == '-') {
            displayString.replace(0, '-');
            txt.erase(0);
            i = 1;
        }
        for (; (i + txt.length()) < myNFigures; i++) {
            displayString.replace(i, '0');
        }
        displayString.insert(i, txt);
    } else if (options & JUSTIFY_RIGHT) {
        for (; (i + lbl.length()) < myNFigures; i++) {}
        displayString.insert(i, lbl);
    } else {
        displayString.insert(0, lbl);
    }
    displayString.trunc(myNFigures);
    i = 0;

    // FIXME: at the moment, if we resize the parent widget, we must use integer multiples
    //        of the SevenSegment width.  The problem is that it makes the padding on the
    //        RHS look wrong.  What we need to do is to extend the horizontal segment width
    //        for the last sevensegment, so as to fill the remaining space.
    MFXSevenSegment* child = (MFXSevenSegment*)getFirst();
    if (options & LAYOUT_FILL) {
        const FXint w = this->width - padleft - padright - (border << 1);
        const FXint h = this->height - padtop - padbottom - (border << 1);
        hspacing = FXMAX(w, h) / 50;
        if (hspacing < 1) {
            hspacing = 1;
        }
        FXint hsl = (w - (myNFigures - 1) * hspacing) / myNFigures;
        if (hsl < 5) {
            hsl = 5;
        }
        FXint vsl = h >> 1;
        if (vsl < 5) {
            vsl = 5;
        }
        FXint st = FXMIN(hsl, vsl) / 4;
        if (st < 1) {
            st = 1;
        }
        FXint groove = st / 4;
        if (groove < 1) {
            groove = 1;
        }
        if (options & LAYOUT_FILL_X) {
            hsl -= groove << 1;
            for (; child; child = (MFXSevenSegment*)child->getNext()) {
                child->setHorizontal(hsl);
            }
            child = (MFXSevenSegment*)getFirst();
        }
        if (options & LAYOUT_FILL_Y) {
            vsl -= groove << 1;
            for (; child; child = (MFXSevenSegment*)child->getNext()) {
                child->setVertical(vsl);
            }
            child = (MFXSevenSegment*)getFirst();
        }
        for (; child; child = (MFXSevenSegment*)child->getNext()) {
            child->setText(displayString[i++]);
            child->setGroove(groove);
            child->setThickness(st);
        }
    } else {
        for (; child; child = (MFXSevenSegment*)child->getNext()) {
            child->setText(displayString[i++]);
        }
    }
}
