/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEToolAddStops2Routes.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Dialog for add stops to routes
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEToolDialog.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEToolAddStops2Routes : public GNEToolDialog {

public:
    /// @brief Constructor
    GNEToolAddStops2Routes(GNEApplicationWindow* GNEApp);

    /// @brief destructor
    ~GNEToolAddStops2Routes();

private:

    /// @brief Invalidated copy constructor.
    GNEToolAddStops2Routes(const GNEToolAddStops2Routes&) = delete;

    /// @brief Invalidated assignment operator.
    GNEToolAddStops2Routes& operator=(const GNEToolAddStops2Routes&) = delete;
};

