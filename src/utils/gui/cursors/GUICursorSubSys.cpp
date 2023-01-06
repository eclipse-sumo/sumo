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

#include "Delete_cursor.cpp"
#include "Select_cursor.cpp"
#include "SelectLane_cursor.cpp"
#include "Inspect_cursor.cpp"
#include "InspectLane_cursor.cpp"
#include "MoveElement_cursor.cpp"

// ===========================================================================
// static member variable definitions
// ===========================================================================

GUICursorSubSys* GUICursorSubSys::myInstance = nullptr;

// ===========================================================================
// member definitions
// ===========================================================================

GUICursorSubSys::GUICursorSubSys(FXApp* a) {
    // default cursors (already created)
    myCursors[GUICursor::DEFAULT] = a->getDefaultCursor(DEF_ARROW_CURSOR);
    myCursors[GUICursor::MOVEVIEW] = a->getDefaultCursor(DEF_MOVE_CURSOR);

    // custom cursors (must be created)
    myCursors[GUICursor::DELETE_CURSOR] = new FXGIFCursor(a, Delete_cursor, 1, 2);
    myCursors[GUICursor::SELECT] = new FXGIFCursor(a, Select_cursor, 1, 1);
    myCursors[GUICursor::SELECT_LANE] = new FXGIFCursor(a, SelectLane_cursor, 1, 1);
    myCursors[GUICursor::INSPECT] = new FXGIFCursor(a, Inspect_cursor, 1, 2);
    myCursors[GUICursor::INSPECT_LANE] = new FXGIFCursor(a, InspectLane_cursor, 1, 2);
    myCursors[GUICursor::MOVEELEMENT] = new FXGIFCursor(a, MoveElement_cursor, 1, 2);

    // ... and create them
    for (const auto& cursor : myCursors) {
        if (cursor.second != nullptr) {
            cursor.second->create();
        }
    }

}


GUICursorSubSys::~GUICursorSubSys() {
    // delete all cursors
    for (const auto& cursor : myCursors) {
        if (cursor.first != GUICursor::DEFAULT && cursor.first != GUICursor::MOVEVIEW) {
            delete cursor.second;
        }
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
