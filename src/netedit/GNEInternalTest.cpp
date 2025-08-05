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
/// @file    GNEInternalTest.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Internal test system used for testing netedit
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/dialogs/fix/GNEFixAdditionalElementsDialog.h>
#include <netedit/dialogs/fix/GNEFixDemandElementsDialog.h>
#include <netedit/dialogs/fix/GNEFixNetworkElements.h>
#include <netedit/frames/GNETLSTable.h>

#include "GNEInternalTest.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEInternalTest::GNEInternalTest(const std::string& testFile) :
    InternalTest(testFile) {
}


GNEInternalTest::~GNEInternalTest() {}


void
GNEInternalTest::runNeteditInternalTests(GNEApplicationWindow* applicationWindow) {
    bool writeClosedSucessfully = false;
    myRunning = true;
    const auto viewNet = applicationWindow->getViewNet();
    const auto viewParent = viewNet->getViewParent();
    // process every step
    while (getCurrentStep()) {
        // get current step and set next step
        const auto testStep = setNextStep();
        // check if we have to process it in main windows, abstract view or specific view
        if (testStep->getCategory() == InternalTestStep::Category::APP) {
            applicationWindow->handle(this, testStep->getSelector(), testStep->getEvent());
        } else if (testStep->getCategory() == InternalTestStep::Category::VIEW) {
            viewNet->handle(this, testStep->getSelector(), testStep->getEvent());
        } else if (testStep->getCategory() == InternalTestStep::Category::TLS_PHASES) {
            viewParent->getTLSEditorFrame()->getTLSPhases()->handle(this, testStep->getSelector(), testStep->getEvent());
        } else if (testStep->getCategory() == InternalTestStep::Category::TLS_PHASETABLE) {
            viewParent->getTLSEditorFrame()->getTLSPhases()->getPhaseTable()->testTable(testStep->getTLSTableTest());
        } else if (testStep->getCategory() == InternalTestStep::Category::INIT) {
            writeClosedSucessfully = true;
        }
        // check if update view after execute step
        if (testStep->updateView()) {
            viewNet->handle(this, FXSEL(SEL_PAINT, 0), nullptr);
        }
    }
    // check if print netedit closed sucessfully
    if (writeClosedSucessfully) {
        std::cout << "TestFunctions: Netedit closed successfully" << std::endl;
    }
}

/****************************************************************************/
