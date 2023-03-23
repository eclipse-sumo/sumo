/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXDynamicLabel.cpp
/// @author  Mirko Barthauer
/// @date    31.03.2023
///
// Text label with dynamic multi-line text (inserts line line breaks on the fly)
/****************************************************************************/
#include <config.h>
#include "MFXDynamicLabel.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXDynamicLabel) MFXLabelMap[] = {
    FXMAPFUNC(SEL_UPDATE,       0,                     MFXDynamicLabel::onUpdate),
};

// Object implementation
FXIMPLEMENT(MFXDynamicLabel,FXLabel,MFXLabelMap,ARRAYNUMBER(MFXLabelMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// MFXLabel - public methods
// ---------------------------------------------------------------------------

MFXDynamicLabel::MFXDynamicLabel(FXComposite* p,const FXString& text,FXIcon* ic,FXuint opts,FXint x,FXint y,FXint w,FXint h,FXint pl,FXint pr,FXint pt,FXint pb):
    FXLabel(p, text, ic, opts, x, y, w, h, pl, pr, pt, pb), myOriginalString(text.text()), myPreviousWidth(0) {
}


long
MFXDynamicLabel::onUpdate(FXObject* sender, FXSelector sel, void* ptr) {
    int width = getWidth();
    if (myPreviousWidth - width != 0 && width > 1) {
        reformatLineBreaks(width);
        myPreviousWidth = width;
    }
    return FXLabel::onUpdate(sender, sel, ptr);
}


void
MFXDynamicLabel::position(FXint x, FXint y, FXint w, FXint h) {
    FXLabel::position(x, y, w, h);
    reformatLineBreaks(w);
}


FXint
MFXDynamicLabel::getDefaultHeight() {
    int pWidth = getParent()->getWidth();
    if (pWidth > 1) { // sign that it has been initialised
        FXFrame* frame = dynamic_cast<FXFrame*>(getParent());
        int padding = 0;
        if (frame != nullptr) {
            padding = frame->getPadLeft() + frame->getPadRight();
        }
        reformatLineBreaks(pWidth - padding);
    }
    return FXLabel::getDefaultHeight();
}



void
MFXDynamicLabel::reformatLineBreaks(int width) {
    int preferredWidth = width - getPadLeft() - getPadRight();
    int currentWidth = getApp()->getNormalFont()->getTextWidth(label);
    if (currentWidth <= preferredWidth || preferredWidth < 1) {
        return;
    }
    std::string msg = myOriginalString;
    int pos = 0;
    int finalPos = msg.size() - 1;
    int nextLineBreak, currentLineWidth;
    while (pos < finalPos) {
        nextLineBreak = msg.find('\n', pos);
        int subPos = (nextLineBreak != std::string::npos) ? nextLineBreak : finalPos;
        if(getApp()->getNormalFont()->getTextWidth(msg.substr(pos, subPos - pos).c_str()) <= preferredWidth) {
            pos = subPos + 1;
            continue;
        }
        // select position for next line break
        int endPos = (nextLineBreak != std::string::npos) ? nextLineBreak - 1 : finalPos;
        int nextSpace = -1;
        int lastSpacePos = -1;
        int pos2 = pos;
        while (pos2 < endPos) {
            nextSpace = msg.find(' ', pos2);
            if (nextSpace != std::string::npos) {
                std::string testString = msg.substr(pos, nextSpace - pos);
                if (getApp()->getNormalFont()->getTextWidth(msg.substr(pos, nextSpace - pos).c_str()) > preferredWidth) {
                    if (lastSpacePos > 0) {
                        msg.replace(lastSpacePos, 1, "\n");
                        pos2 = lastSpacePos + 1;
                    } else {
                        msg.replace(nextSpace, 1, "\n");
                        pos2 = nextSpace + 1;
                    }
                    break;
                } else {
                    pos2 = nextSpace + 1;
                    if (msg.find(' ', pos2) == std::string::npos && lastSpacePos > 0) { // string end condition
                        msg.replace(lastSpacePos, 1, "\n");
                        pos2++;
                        break;
                    }
                }
                lastSpacePos = nextSpace;
            } else {
                pos2 = endPos + 2;
            }
        }
        pos = pos2;
    }


    label = msg.c_str();
}


MFXDynamicLabel::MFXDynamicLabel() {}