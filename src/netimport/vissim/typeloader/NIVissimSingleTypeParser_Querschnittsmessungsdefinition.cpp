/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Querschnittsmessungsdefinition.cpp
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
#include "../tempstructs/NIVissimClosures.h"
#include "NIVissimSingleTypeParser_Querschnittsmessungsdefinition.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Querschnittsmessungsdefinition::NIVissimSingleTypeParser_Querschnittsmessungsdefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Querschnittsmessungsdefinition::~NIVissimSingleTypeParser_Querschnittsmessungsdefinition() {}


bool
NIVissimSingleTypeParser_Querschnittsmessungsdefinition::parse(std::istream& from) {
    std::string tag = myRead(from);
    if (tag == "von") {
        while (tag != "fahrzeugklassen" && tag != "konfdatei") {
            tag = myRead(from);
        }
        tag = myRead(from);
        while (tag != "DATAEND") {
            tag = readEndSecure(from);
        }
        return true;
    }
    tag = myRead(from);
    tag = myRead(from);
    while (tag != "DATAEND") {
        tag = readEndSecure(from);
    }
    return true;
}



/****************************************************************************/

