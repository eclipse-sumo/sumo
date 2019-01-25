/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fensterdefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Fri, 21 Mar 2003
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
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser_Fensterdefinition.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Fensterdefinition::NIVissimSingleTypeParser_Fensterdefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Fensterdefinition::~NIVissimSingleTypeParser_Fensterdefinition() {}


bool
NIVissimSingleTypeParser_Fensterdefinition::parse(std::istream& from) {
    std::string id;
    from >> id; // "typ"
    std::string type = myRead(from);
    if (type == "fzinfo") {
        std::string tmp;
        from >> tmp;
        from >> tmp;
    } else if (type == "ldp" || type == "szp") {
        std::string tmp;
        readUntil(from, "lsa");
    }
    return true;
}



/****************************************************************************/

