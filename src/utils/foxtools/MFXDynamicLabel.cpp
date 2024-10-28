/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
    FXMAPFUNC(SEL_UPDATE,       0,                          MFXDynamicLabel::onUpdate),
};

// Object implementation
FXIMPLEMENT(MFXDynamicLabel, FXLabel, MFXLabelMap, ARRAYNUMBER(MFXLabelMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// MFXLabel - public methods
// ---------------------------------------------------------------------------

MFXDynamicLabel::MFXDynamicLabel(FXComposite* p, const FXString& text, FXIcon* ic, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb, std::string indent):
    FXLabel(p, text, ic, opts, x, y, w, h, pl, pr, pt, pb), myOriginalString(text.text()), myIndentString(indent), myPreviousWidth(0) {
    computeIndentation();
}


long
MFXDynamicLabel::onUpdate(FXObject* sender, FXSelector sel, void* ptr) {
    const int curWidth = getWidth();
    if (myPreviousWidth - curWidth != 0 && curWidth > 1) {
        reformatLineBreaks(curWidth);
        myPreviousWidth = curWidth;
    }
    return FXLabel::onUpdate(sender, sel, ptr);
}


void
MFXDynamicLabel::setText(const FXString& text) {
    FXLabel::setText(text);
    myOriginalString = text.text();
    computeIndentation();
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
MFXDynamicLabel::computeIndentation() {
    if (myOriginalString.find(myIndentString) == 0 || myOriginalString.find("\n" + myIndentString) != std::string::npos) {
        myIndent = (int)myIndentString.size();
    } else {
        myIndent = 0;
    }
}


void
MFXDynamicLabel::reformatLineBreaks(const int curWidth) {
    const int preferredWidth = curWidth - getPadLeft() - getPadRight();
    const int textWidth = getApp()->getNormalFont()->getTextWidth(label);
    if (textWidth <= preferredWidth || preferredWidth < 1) {
        return;
    }
    std::string newLine = "\n" + std::string(myIndent, ' ');
    const int newLineOffset = (int)newLine.size();
    std::string msg = myOriginalString;
    int pos = 0;
    int finalPos = (int)msg.size() - 1;
    std::string::size_type nextLineBreak;
    while (pos < finalPos) {
        nextLineBreak = msg.find('\n', pos);
        int subPos = (nextLineBreak != std::string::npos) ? (int)nextLineBreak : finalPos;
        if (getApp()->getNormalFont()->getTextWidth(msg.substr(pos, subPos - pos).c_str()) <= preferredWidth) {
            pos = subPos + 1;
            continue;
        }
        if (myIndent > 0 && msg.substr(pos, myIndent) == myIndentString) {
            pos += myIndent;
        }
        // select position for next line break
        const int endPos = (nextLineBreak != std::string::npos) ? (int)nextLineBreak - 1 : finalPos;
        std::string::size_type nextSpace = std::string::npos;
        int lastSpacePos = -1;
        int pos2 = pos;
        while (pos2 < endPos) {
            nextSpace = msg.find(' ', pos2);
            if (nextSpace != std::string::npos && (int)nextSpace <= pos + myIndent) {
                nextSpace = std::string::npos;
                pos2 += myIndent + 1;
            } else if (nextSpace != std::string::npos && (int)nextSpace < endPos) {
                std::string testString = msg.substr(pos, nextSpace - pos);
                if (getApp()->getNormalFont()->getTextWidth(msg.substr(pos, nextSpace - pos).c_str()) > preferredWidth) {
                    if (lastSpacePos > 0) {
                        msg.replace(lastSpacePos, 1, newLine);
                        pos2 = lastSpacePos + newLineOffset;
                        finalPos += newLineOffset;
                    } else {
                        msg.replace(nextSpace, 1, newLine);
                        pos2 = (int)nextSpace + newLineOffset;
                        finalPos += newLineOffset;
                    }
                    break;
                } else {
                    pos2 = (int)nextSpace + 1;
                    if (msg.find(' ', pos2) == std::string::npos && lastSpacePos > 0) { // string end condition
                        msg.replace(lastSpacePos, 1, newLine);
                        pos2 += newLineOffset;
                        break;
                    }
                }
                lastSpacePos = (int)nextSpace;
            } else {
                pos2 = endPos + 2;
            }
        }
        pos = pos2;
    }
    label = msg.c_str();
}


MFXDynamicLabel::MFXDynamicLabel() {}
