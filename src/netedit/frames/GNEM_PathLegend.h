/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEFrameModules.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// Auxiliar class for GNEFrame Modules
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/GNEViewNetHelper.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <utils/foxtools/FXGroupBoxModule.h>
#include <utils/foxtools/FXTreeListDinamic.h>
#include <utils/foxtools/MFXIconComboBox.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEM_PathLegend : public FXGroupBoxModule {

public:
    /// @brief constructor
    GNEM_PathLegend(GNEFrame* frameParent);

    /// @brief destructor
    ~GNEM_PathLegend();

    /// @brief show Legend modul
    void showPathLegendModule();

    /// @brief hide Legend modul
    void hidePathLegendModule();
};