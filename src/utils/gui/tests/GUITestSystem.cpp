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
/// @file    GUITestSystem.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Abstract class used for test systems
/****************************************************************************/

#include <fstream>
#include <utils/common/MsgHandler.h>

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
GUITestSystem::addTestStep(const GUITestSystemStep* step) {
    myTestSteps.push_back(step);
}

/****************************************************************************/
