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
/// @file    GNETestSystem.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Thread used for testing netedit
/****************************************************************************/

#include <netedit/frames/GNETagSelector.h>
#include <netedit/frames/network/GNEAdditionalFrame.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <utils/gui/tests/GUITestSystemStep.h>

#include "GNETestSystem.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNETestSystem::GNETestSystem(const std::string &testFile) :
    GUITestSystem(testFile) {
}


GNETestSystem::~GNETestSystem() {}


void
GNETestSystem::runNeteditTests(GNEApplicationWindow* applicationWindow) {
    // run rest only once
    if (myTestStarted == false) {
        myTestStarted = true;
        // process every step
        for (const auto &testStep : myTestSteps) {
            // check if we have to process it in main windows, abstract view or specific view
            if (testStep->getCategory() == GUITestSystemStep::Category::MAINWINDOW) {
                applicationWindow->handle(this, testStep->getSelector(), testStep->getEvent());
            } else if (testStep->getCategory() == GUITestSystemStep::Category::VIEW) {
                applicationWindow->getViewNet()->handle(this, testStep->getSelector(), testStep->getEvent());
            } else if (testStep->getCategory() == GUITestSystemStep::Category::FRAME_ADDITIONAL_TAGSELECTOR) {
                applicationWindow->getViewNet()->getViewParent()->getAdditionalFrame()->getAdditionalTagSelector()->handle(this, testStep->getSelector(), (void*)testStep->getText());
            }
            // update view after every test, except if test is quit() or category is virtual
            if ((testStep->getCategory() != GUITestSystemStep::Category::VIRTUAL) && 
                (testStep->getMessageID() != MID_HOTKEY_CTRL_Q_CLOSE)) {
                applicationWindow->getViewNet()->handle(this, FXSEL(SEL_PAINT, 0), nullptr);
            }
        }
    }
}

/****************************************************************************/
