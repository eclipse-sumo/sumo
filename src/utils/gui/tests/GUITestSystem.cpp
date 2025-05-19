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

#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/options/OptionsCont.h>

#include "GUITestSystem.h"
#include "GUITestSystemStep.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GUITestSystem::GUITestSystem() {}


GUITestSystem::~GUITestSystem() {
    for (auto testStep : myTestSteps) {
        delete testStep;
    }
}


void
GUITestSystem::runTests(GUISUMOAbstractView* view, GUIMainWindow* mainWindow) {
    // run rest only once
    if (myTestStarted == false) {
        myTestStarted = true;
        // set common abstract view
        myAbstractView = view;
        // set specific parameter in children
        setSpecificMainWindow(mainWindow);
        // check if run test thread
        if (OptionsCont::getOptions().getString("test-file").size() > 0) {
            processTestFile();
        }
        // process every step
        for (const auto &testStep : myTestSteps) {
            // continue depending of step type
            if (testStep->getCategory() == "abstractView") {
                myAbstractView->handle(this, testStep->getSelector(), testStep->getEvent());
            } else {
                runSpecificTest(testStep);
            }
            // update view after every test, except if test is quit()
            if (testStep->getFunction() != "quit") {
                myAbstractView->handle(this, FXSEL(SEL_PAINT, 0), nullptr);
            }
        }
    }
}


void
GUITestSystem::addTestStep(const GUITestSystemStep* step) {
    myTestSteps.push_back(step);
}


void
GUITestSystem::processTestFile() {
    const std::string testFile = OptionsCont::getOptions().getString("test-file");
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
                auto step = new GUITestSystemStep(this, line);
                // only add to list if this is not a virtual attribute (for example, click)
                if (step->getCategory() != "virtual") {
                    myTestSteps.push_back(step);
                }
            }
        }
    }
}

/****************************************************************************/