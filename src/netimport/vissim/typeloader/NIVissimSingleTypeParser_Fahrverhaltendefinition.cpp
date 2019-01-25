/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fahrverhaltendefinition.cpp
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
#include "../NIImporter_Vissim.h"
#include "NIVissimSingleTypeParser_Fahrverhaltendefinition.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Fahrverhaltendefinition::NIVissimSingleTypeParser_Fahrverhaltendefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Fahrverhaltendefinition::~NIVissimSingleTypeParser_Fahrverhaltendefinition() {}


bool
NIVissimSingleTypeParser_Fahrverhaltendefinition::parse(std::istream& from) {
    std::string tmp;
    from >> tmp;
    // in the both next cases, we do not have to overread anything
    if (tmp == "BEHALT_ALTE_PARA" || tmp == "ANZ_VM") {
        return true;
    }
    //
    from >> tmp;
    if (tmp == "NAME") {
        readUntil(from, "gelbverhalten");
    }
    return true;
}



/****************************************************************************/

