/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GUICursorSubSys.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2018
///
// Helper for cursors loading and usage
/****************************************************************************/
#include <config.h>

#include <utils/common/UtilExceptions.h>

#include "GUICursors.h"
#include "GUICursorSubSys.h"

#include "select_cursor.xbm"
#include "select_cursor_mask.xbm"
#include "select_lane_cursor.xbm"
#include "select_lane_cursor_mask.xbm"
#include "inspect_cursor.xbm"
#include "inspect_cursor_mask.xbm"
#include "move_element_cursor.xbm"
#include "move_element_cursor_mask.xbm"

// ===========================================================================
// static member variable definitions
// ===========================================================================

GUICursorSubSys* GUICursorSubSys::myInstance = nullptr;

// ===========================================================================
// member definitions
// ===========================================================================

GUICursorSubSys::GUICursorSubSys(FXApp* a) {
    // default cursors (already created)
    myCursors[GUICursor::SUMOCURSOR_DEFAULT] = a->getDefaultCursor(DEF_ARROW_CURSOR);
    myCursors[GUICursor::SUMOCURSOR_MOVEVIEW] = a->getDefaultCursor(DEF_MOVE_CURSOR);

    // custom cursors (must be created)
    myCursors[GUICursor::SUMOCURSOR_SELECT] = new FXCursor(a, select_cursor_bits, select_cursor_mask_bits, select_cursor_width, select_cursor_height, select_cursor_x_hot, select_cursor_y_hot);
    myCursors[GUICursor::SUMOCURSOR_SELECT_LANE] = new FXCursor(a, select_lane_cursor_bits, select_lane_cursor_mask_bits, select_lane_cursor_width, select_lane_cursor_height, select_lane_cursor_x_hot, select_lane_cursor_y_hot);
    myCursors[GUICursor::SUMOCURSOR_INSPECT] = new FXCursor(a, inspect_cursor_bits, inspect_cursor_mask_bits, inspect_cursor_width, inspect_cursor_height, inspect_cursor_x_hot, inspect_cursor_y_hot);
    myCursors[GUICursor::SUMOCURSOR_MOVEELEMENT] = new FXCursor(a, move_element_cursor_bits, move_element_cursor_mask_bits, move_element_cursor_width, move_element_cursor_height, move_element_cursor_x_hot, move_element_cursor_y_hot);

    // ... and create them
    for (const auto &cursor : myCursors) {
        if (cursor.second != nullptr) {
            cursor.second->create();
        }
    }
    
}


GUICursorSubSys::~GUICursorSubSys() {
    // delete all cursors
    for (const auto& cursor : myCursors) {
        delete cursor.second;
    }
}


void
GUICursorSubSys::initCursors(FXApp* a) {
    if (myInstance == nullptr) {
        myInstance = new GUICursorSubSys(a);
    } else {
        throw ProcessError("GUICursorSubSys already init");
    }
}


FXCursor*
GUICursorSubSys::getCursor(GUICursor which) {
    return myInstance->myCursors[which];
}


void
GUICursorSubSys::close() {
    // delete and reset instance
    delete myInstance;
    myInstance = nullptr;
}


/****************************************************************************/
