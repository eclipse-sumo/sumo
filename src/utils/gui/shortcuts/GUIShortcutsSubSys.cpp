/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIShortcutsSubSys.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2019
/// @version $Id$
///
// Helper for Shortcuts
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <cassert>
#include "GUIShortcuts.h"
#include "GUIShortcutsSubSys.h"

/** includes **/

// ===========================================================================
// static member variable definitions
// ===========================================================================

GUIShortcutsSubSys* GUIShortcutsSubSys::myInstance = nullptr;

// ===========================================================================
// member definitions
// ===========================================================================

GUIShortcutsSubSys::GUIShortcutsSubSys(FXApp* a) {

}


GUIShortcutsSubSys::~GUIShortcutsSubSys() {
    /*
    for (int i = 0; i < Shortcut_MAX; i++) {
        delete myShortcuts[i];
    }
    */
}


void
GUIShortcutsSubSys::initShortcuts(FXApp* a) {
    assert(myInstance == 0);
    myInstance = new GUIShortcutsSubSys(a);
}


void
GUIShortcutsSubSys::close() {
    delete myInstance;
    myInstance = nullptr;
}


/****************************************************************************/

