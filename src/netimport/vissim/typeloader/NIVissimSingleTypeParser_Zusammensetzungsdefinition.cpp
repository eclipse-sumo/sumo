/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Zusammensetzungsdefinition.cpp
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
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser_Zusammensetzungsdefinition.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Zusammensetzungsdefinition::NIVissimSingleTypeParser_Zusammensetzungsdefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Zusammensetzungsdefinition::~NIVissimSingleTypeParser_Zusammensetzungsdefinition() {}


bool
NIVissimSingleTypeParser_Zusammensetzungsdefinition::parse(std::istream& from) {
    std::string tag = myRead(from);
    while (tag != "fahrzeugtyp") {
        tag = readEndSecure(from, "fahrzeugtyp");
    }
    do {
        tag = myRead(from); // id
        tag = myRead(from); // "anteil"
        tag = myRead(from); // value
        tag = myRead(from); // "VWunsch"
        tag = myRead(from); // value
        tag = readEndSecure(from, "fahrzeugtyp"); // "fahrzeugtyp"?
    } while (tag == "fahrzeugtyp");
    return true;
}



/****************************************************************************/

