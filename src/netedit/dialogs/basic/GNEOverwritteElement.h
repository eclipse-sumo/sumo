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
/// @file    GNEOverwritteElement.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
///
// Dialog used to ask user if overwrite elements during loading
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEOverwritteElement : public GNEDialog {

public:
    /// @brief Constructor
    GNEOverwritteElement(const GNEAttributeCarrier* AC);

    /// @brief destructor
    ~GNEOverwritteElement();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

private:
    /// @brief Invalidated copy constructor.
    GNEOverwritteElement(const GNEOverwritteElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEOverwritteElement& operator=(const GNEOverwritteElement&) = delete;
};
