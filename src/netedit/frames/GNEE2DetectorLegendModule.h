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
/// @file    GNEE2DetectorLegendModule.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2023
///
// Frame for E2 detector legend
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEE2DetectorLegendModule : public MFXGroupBoxModule {

public:
    /// @brief constructor
    GNEE2DetectorLegendModule(GNEFrame* frameParent);

    /// @brief destructor
    ~GNEE2DetectorLegendModule();

    /// @brief show Legend modul
    void showE2DetectorLegend();

    /// @brief hide Legend modul
    void hideE2DetectorLegend();
};
