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
/// @file    NIVissimSingleTypeParser_LSAKopplungsdefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 30 Apr 2003
///
//
/****************************************************************************/
#include <config.h>

#include <iostream>
#include <utils/common/StringUtils.h>
#include <netimport/vissim/NIImporter_Vissim.h>
#include <netimport/vissim/tempstructs/NIVissimSource.h>
#include "NIVissimSingleTypeParser_LSAKopplungsdefinition.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_LSAKopplungsdefinition::NIVissimSingleTypeParser_LSAKopplungsdefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_LSAKopplungsdefinition::~NIVissimSingleTypeParser_LSAKopplungsdefinition() {}


bool
NIVissimSingleTypeParser_LSAKopplungsdefinition::parse(std::istream&) {
    return true;
}


/****************************************************************************/
