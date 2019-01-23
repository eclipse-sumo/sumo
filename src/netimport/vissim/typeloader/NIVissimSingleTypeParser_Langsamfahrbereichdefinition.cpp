/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Langsamfahrbereichdefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <utils/common/StringUtils.h>
#include "../NIImporter_Vissim.h"
#include "NIVissimSingleTypeParser_Langsamfahrbereichdefinition.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Langsamfahrbereichdefinition::NIVissimSingleTypeParser_Langsamfahrbereichdefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Langsamfahrbereichdefinition::~NIVissimSingleTypeParser_Langsamfahrbereichdefinition() {}


bool
NIVissimSingleTypeParser_Langsamfahrbereichdefinition::parse(std::istream& from) {
    std::string id;
    from >> id;
    readUntil(from, "fahrzeugklasse");
    std::string tag = "fahrzeugklasse";
    while (tag == "fahrzeugklasse") {
        readUntil(from, "maxverzoegerung");
        tag = myRead(from);
        tag = myRead(from);
    }
    return true;
}



/****************************************************************************/

