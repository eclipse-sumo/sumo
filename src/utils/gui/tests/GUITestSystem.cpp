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
/// @file    GUITestSystem.h
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
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/options/OptionsCont.h>

#include "GUITestSystem.h"
#include "GUITestSystemStep.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GUITestSystem::GUITestSystem(const std::string &testFile) {
    // open file
    std::ifstream strm(testFile);
    // check if file can be opened
    if (!strm.good()) {
        WRITE_ERRORF(TL("Could not open test file '%'."), testFile);
    } else {
        // continue while stream exist
        while (strm.good()) {
            std::string line;
            strm >> line;
            // check if line isn't empty
            if ((line.size() > 0) && line[0] != '#') {
                myTestSteps.push_back(new GUITestSystemStep(this, line));
            }
        }
    }
}


GUITestSystem::~GUITestSystem() {
    for (auto testStep : myTestSteps) {
        delete testStep;
    }
}


void
GUITestSystem::runNeteditTests(GNEViewNet* viewNet) {
    // run rest only once
    if (myTestStarted == false) {
        myTestStarted = true;
        // process every step
        for (const auto &testStep : myTestSteps) {
            // check if we have to process it in main windows, abstract view or specific view
            if (testStep->getCategory() == GUITestSystemStep::Category::VIEW) {
                viewNet->handle(this, testStep->getSelector(), testStep->getEvent());
            } else if (testStep->getCategory() == GUITestSystemStep::Category::MAINWINDOW) {
                viewNet->getViewParent()->getGNEAppWindows()->handle(this, testStep->getSelector(), testStep->getEvent());
            } else if (testStep->getCategory() == GUITestSystemStep::Category::FRAME_ADDITIONAL_TAGSELECTOR) {
                viewNet->getViewParent()->getAdditionalFrame()->getAdditionalTagSelector()->handle(this, testStep->getSelector(), (void*)testStep->getText());
            }
            // update view after every test, except if test is quit() or category is virtual
            if ((testStep->getCategory() != GUITestSystemStep::Category::VIRTUAL) && 
                (testStep->getMessageID() != MID_HOTKEY_CTRL_Q_CLOSE)) {
                viewNet->handle(this, FXSEL(SEL_PAINT, 0), nullptr);
            }
        }
    }
}


void
GUITestSystem::addTestStep(const GUITestSystemStep* step) {
    myTestSteps.push_back(step);
}

/****************************************************************************/