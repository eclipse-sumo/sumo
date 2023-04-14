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
/// @file    GNENetDiffTool.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2023
///
// Netdiff tool used in netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>
#include <utils/options/OptionsCont.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================

class GNENetDiffTool {

public:
    /// @brief Constructor
    GNENetDiffTool(GNEApplicationWindow* GNEApp, const std::string &pythonPath, 
                  const std::string &templateStr, FXMenuPane* menu);

    /// @brief get command (python + script + arguments)
    std::string getCommand() const;
    
private:
    /// @brief Invalidated copy constructor.
    GNENetDiffTool(const GNENetDiffTool&) = delete;

    /// @brief Invalidated assignment operator.
    GNENetDiffTool& operator=(const GNENetDiffTool&) = delete;
};

