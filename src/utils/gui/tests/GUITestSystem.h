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
#pragma once
#include <config.h>

#include <string>
#include <vector>

#include <utils/foxtools/fxheader.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GUIMainWindow;
class GUISUMOAbstractView;
class GUITestSystemStep;

// ===========================================================================
// class definitions
// ===========================================================================

class GUITestSystem : public FXObject {

public:
    /// @brief constructor
    GUITestSystem();

    /// @brief destructor
    ~GUITestSystem();

    /// @brief start test. The argument is either GNEApplicationWindow or GUIApplicationWindows
    void runTests(GUISUMOAbstractView* view, GUIMainWindow* mainWindow);

    /// @brief add test steps
    void addTestStep(const GUITestSystemStep* step);

protected:
    /// @brief run specific test
    virtual void setSpecificMainWindow(GUIMainWindow* mainWindow) = 0;

    /// @brief run specific test
    virtual void runSpecificTest(const GUITestSystemStep* testStep) = 0;

private:
    /// @brief process test file
    void processTestFile();

    /// @brief test steps
    std::vector<const GUITestSystemStep*> myTestSteps;

    /// @brief flag to check if test are started
    bool myTestStarted = false;

    /// @brief abstract view
    GUISUMOAbstractView* myAbstractView = nullptr;
};
