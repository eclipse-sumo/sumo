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
/// @file    GNERunNetgenerate.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Thread for run netgenerate
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/options/OptionsCont.h>

#include "GNERun.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNERunNetgenerate : public GNERun<OptionsCont> {

public:
    /// @brief Constructor
    GNERunNetgenerate(GNERunDialog* runDialog, MFXSynchQue<GUIEvent*>& eq, FXEX::MFXThreadEvent& ev);

    /// @brief destructor
    ~GNERunNetgenerate();

    /// @brief run netgenerate
    void run(const OptionsCont* netgenerateOptions) override;

private:
    /// @brief Invalidated copy constructor.
    GNERunNetgenerate(const GNERunNetgenerate&) = delete;

    /// @brief Invalidated assignment operator.
    GNERunNetgenerate& operator=(const GNERunNetgenerate&) = delete;
};
