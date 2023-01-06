/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXRecentNetworks.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2021
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include <map>
#include "fxheader.h"

/// @brief MFXRecentNetworks
class MFXRecentNetworks : public FXRecentFiles {
    /// @brief FOX-declaration
    FXDECLARE(MFXRecentNetworks)

public:
    /// @brief enum for nofiles
    enum {
        ID_NOFILES = 100,
    };

    /// @brief default constructor
    MFXRecentNetworks();

    /// @brief parameter constructor
    MFXRecentNetworks(FXApp* a, const FXString& gp);

    /// @name FOX calls
    /// @{
    long onUpdFile(FXObject*, FXSelector, void*);

    long onUpdNoFiles(FXObject*, FXSelector, void*);
    /// @}

private:
    /// @brief map with index and strings
    std::map<FXint, FXString> myIndexFilenames;
};
