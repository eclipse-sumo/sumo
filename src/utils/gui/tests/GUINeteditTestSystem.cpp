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
/// @file    GUINeteditTestSystem.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Thread used for testing netedit
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/GNETagSelector.h>
#include <netedit/frames/network/GNEAdditionalFrame.h>

#include "GUIGlobalTestSystem.h"
#include "GUINeteditTestSystem.h"
#include "GUITestSystemStep.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GUINeteditTestSystem::GUINeteditTestSystem() {}


GUINeteditTestSystem::~GUINeteditTestSystem() {}


void
GUINeteditTestSystem::createTestSystem() {
    gTestSystem = new GUINeteditTestSystem();
}


void 
GUINeteditTestSystem::setSpecificMainWindow(GUIMainWindow* mainWindow) {
    myGNEApplicationWindow = dynamic_cast<GNEApplicationWindow*>(mainWindow);
    // check that GNEApplicationWindow was casted successfully
    if (myGNEApplicationWindow == nullptr) {
        throw ProcessError("Invalid GNEApplicationWindow used in GUINeteditTestSystem");
    }
}


void
GUINeteditTestSystem::runSpecificTest(const GUITestSystemStep* testStep) {
    // continue depending of step type
    if (testStep->getCategory() == "additionalFrame") {
        myGNEApplicationWindow->getViewNet()->getViewParent()->getAdditionalFrame()->getAdditionalTagSelector()->handle(this, testStep->getSelector(), (void*)testStep->getText());
    } else if (testStep->getCategory() == "applicationWindow") {
        myGNEApplicationWindow->handle(this, testStep->getSelector(), nullptr);
    }
}

/****************************************************************************/