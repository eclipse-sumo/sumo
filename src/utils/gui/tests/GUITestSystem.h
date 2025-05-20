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
// Abstract class used for test systems
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>

#include <utils/foxtools/fxheader.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GUITestSystemStep;

// ===========================================================================
// class definitions
// ===========================================================================

class GUITestSystem : public FXObject {

public:
    /// @brief constructor
    GUITestSystem(const std::string &testFile);

    /// @brief destructor
    ~GUITestSystem();

    /// @brief add test steps
    void addTestStep(const GUITestSystemStep* step);

protected:
    /// @brief test steps
    std::vector<const GUITestSystemStep*> myTestSteps;

    /// @brief flag to check if test are started
    bool myTestStarted = false;
};
