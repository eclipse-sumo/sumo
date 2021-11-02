/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2021 German Aerospace Center (DLR) and others.
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
/// @file    MFXTextFieldIconIcon.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2021
///
//
/****************************************************************************/

#include "MFXTextFieldIcon.h"

// Map
FXDEFMAP(MFXTextFieldIcon) MFXTextFieldIconMap[]={
  FXMAPFUNC(SEL_PAINT,  0,  MFXTextFieldIcon::onPaint),
 
};


// Object implementation
FXIMPLEMENT(MFXTextFieldIcon, FXTextField, MFXTextFieldIconMap,ARRAYNUMBER(MFXTextFieldIconMap))


MFXTextFieldIcon::MFXTextFieldIcon(FXComposite* p,FXint ncols,FXObject* tgt,FXSelector sel,FXuint opt, FXint x ,FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) :
    FXTextField(p, ncols, tgt, sel, opt, x, y, w, h, pl, pr, pt, pb) {
}


long 
MFXTextFieldIcon::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent *ev=(FXEvent*)ptr;
    FXDCWindow dc(this, ev);
    // Draw frame
    drawFrame(dc, 0, 0, width, height);
    // Gray background if disabled
    if(isEnabled()) {
        dc.setForeground(backColor);
    } else {
        dc.setForeground(baseColor);
    }
    // Draw background
    dc.fillRectangle(border, border, width - (border << 1), height - (border << 1));
    // Draw text,  clipped against frame interior
    dc.setClipRectangle(border, border, width - (border << 1), height - (border << 1));
    drawTextRange(dc, 0, contents.length());
    // Draw caret
    if(flags & FLAG_CARET){
        int xx=coord(cursor) - 1;
        dc.setForeground(cursorColor);
        dc.fillRectangle(xx, padtop+border, 1, height - padbottom - padtop - (border << 1));
        dc.fillRectangle(xx - 2, padtop+border, 5, 1);
        dc.fillRectangle(xx - 2, height - border - padbottom - 1, 5, 1);
    }
    return 1;
}


MFXTextFieldIcon::MFXTextFieldIcon() {}