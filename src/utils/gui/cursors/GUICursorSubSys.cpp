/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUICursorSubSys.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2018
/// @version $Id$
///
// Helper for cursors loading and usage
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <cassert>
#include "GUICursors.h"
#include "GUICursorSubSys.h"

/** includes **/

// ===========================================================================
// static member variable definitions
// ===========================================================================

GUICursorSubSys* GUICursorSubSys::myInstance = nullptr;

// ===========================================================================
// member definitions
// ===========================================================================

GUICursorSubSys::GUICursorSubSys(FXApp* a) {
    // already created cursor
    myCursors[SUMOCURSOR_DEFAULT] = a->getDefaultCursor(DEF_ARROW_CURSOR);
    myCursors[SUMOCURSOR_MOVE] = a->getDefaultCursor(DEF_MOVE_CURSOR);

    /*
    myCursors[CURSOR_SUMO] = new FXXPMCursor(a, sumo_icon64_xpm);

    // ... and create them
    for (int i = 0; i < CURSOR_MAX; i++) {
        if (myCursors[i] != nullptr) {
            myCursors[i]->create();
        }
    }
    */
}


GUICursorSubSys::~GUICursorSubSys() {
    /*
    for (int i = 0; i < CURSOR_MAX; i++) {
        delete myCursors[i];
    }
    */
}


void
GUICursorSubSys::initCursors(FXApp* a) {
    assert(myInstance == 0);
    myInstance = new GUICursorSubSys(a);
}


FXCursor*
GUICursorSubSys::getCursor(GUICursor which) {
    return myInstance->myCursors[which];
}


void
GUICursorSubSys::close() {
    delete myInstance;
    myInstance = nullptr;
}


/****************************************************************************/

