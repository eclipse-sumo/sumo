/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GUISumoTestSystem.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Thread used for testing sumo
/****************************************************************************/

#include "GUIGlobalTestSystem.h"
#include "GUISumoTestSystem.h"
#include "GUITestSystemStep.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GUISumoTestSystem::GUISumoTestSystem() {}


GUISumoTestSystem::~GUISumoTestSystem() {}


void
GUISumoTestSystem::createTestSystem() {
    gTestSystem = new GUISumoTestSystem();
}


void GUISumoTestSystem::setSpecificMainWindow(GUIMainWindow* /*mainWindow*/) {
    // waiting for implementation in SUMO GUI
}


void GUISumoTestSystem::runSpecificTest(const GUITestSystemStep* /*testStep*/) {
    // waiting for implementation in SUMO GUI
}

/****************************************************************************/