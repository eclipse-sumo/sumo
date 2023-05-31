/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
/// @file    MFXListIcon.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2023
///
//
/****************************************************************************/

/* =========================================================================
 * included modules
 * ======================================================================= */

#include <utils/common/UtilExceptions.h>

#include "MFXListIcon.h"
#include "MFXListItemIcon.h"


#define LINE_SPACING    4   // Line spacing between items
#define ICON_SIZE       16

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXListIcon) MFXListIconMap[] = {
    FXMAPFUNC(SEL_PAINT, 0, MFXListIcon::onPaint),
};

// Object implementation
FXIMPLEMENT(MFXListIcon, FXList, MFXListIconMap, ARRAYNUMBER(MFXListIconMap))

// ===========================================================================
// member method definitions
// ===========================================================================

MFXListIcon::MFXListIcon(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXList(p, tgt, sel, opts, x, y, w, h) {
}


FXint
MFXListIcon::getDefaultHeight() {
    if (visible) {
        return visible * (LINE_SPACING + FXMAX(font->getFontHeight(), ICON_SIZE));
    } else {
        return FXScrollArea::getDefaultHeight();
    }
}


long
MFXListIcon::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXDCWindow dc(this, event);
    FXint i, y, h;
    // Paint items
    y = pos_y;
    for (i = 0; i < items.no(); i++) {
        const auto listIcon = dynamic_cast<MFXListItemIcon*>(items[i]);
        if (listIcon) {
            h = listIcon->getHeight(this);
            if (event->rect.y <= (y + h) && y < (event->rect.y + event->rect.h)) {
                listIcon->draw(this, dc, pos_x, y, FXMAX(listWidth, viewport_w), h);
            }
            y += h;
        }
    }
    // Paint blank area below items
    if (y < (event->rect.y + event->rect.h)) {
        dc.setForeground(backColor);
        dc.fillRectangle(event->rect.x, y, event->rect.w, event->rect.y + event->rect.h - y);
    }
    return 1;
}

MFXListIcon::MFXListIcon() :
    FXList() {
}
