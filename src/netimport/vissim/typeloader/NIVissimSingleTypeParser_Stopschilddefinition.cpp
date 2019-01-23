/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Stopschilddefinition.cpp
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
#include "NIVissimSingleTypeParser_Stopschilddefinition.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Stopschilddefinition::NIVissimSingleTypeParser_Stopschilddefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Stopschilddefinition::~NIVissimSingleTypeParser_Stopschilddefinition() {}


bool
NIVissimSingleTypeParser_Stopschilddefinition::parse(std::istream& from) {
    readUntil(from, "strecke");
    std::string tag;
    from >> tag; // edge name
    from >> tag; // "spur"
    from >> tag; // lane no
    from >> tag; // "bei"
    from >> tag; // pos
    from >> tag;
    if (tag == "RTOR") {
        from >> tag; // "lsa"
        from >> tag; // lsa id
        from >> tag; // "gruppe"
        from >> tag; // gruppe id
    } else {
        while (tag == "fahrzeugklasse") {
            from >> tag; // class no
            from >> tag; // "zeiten"
            from >> tag; // times no
            tag = readEndSecure(from, "fahrzeugklasse");
        }
    }
    return true;
}



/****************************************************************************/

