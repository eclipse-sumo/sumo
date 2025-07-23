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
/// @file    GNEInternalTest.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Internal test system used for testing netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/tests/InternalTest.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEInternalTest : public InternalTest {

public:
    /// @brief constructor
    GNEInternalTest(const std::string& testFile);

    /// @brief destructor
    ~GNEInternalTest();

    /// @brief run netedit internal test
    void runNeteditInternalTests(GNEApplicationWindow* applicationWindow);

private:
    /// @brief Invalidated default constructor.
    GNEInternalTest() = delete;

    /// @brief Invalidated copy constructor.
    GNEInternalTest(const GNEInternalTest&) = delete;

    /// @brief Invalidated assignment operator
    GNEInternalTest& operator=(const GNEInternalTest& src) = delete;
};
