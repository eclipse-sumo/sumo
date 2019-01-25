/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSingleTypeParser_DynUml.cpp
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
#include <vector>
#include <utils/common/StringUtils.h>
#include "../NIImporter_Vissim.h"
#include "NIVissimSingleTypeParser_DynUml.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_DynUml::NIVissimSingleTypeParser_DynUml(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_DynUml::~NIVissimSingleTypeParser_DynUml() {}


bool
NIVissimSingleTypeParser_DynUml::parse(std::istream& from) {
    std::vector<std::string> tmp;
    tmp.push_back("reisezeit");
    tmp.push_back("kante");
    readUntil(from, "kirchhoffexponent");
    std::string tag = readEndSecure(from, tmp);
    while (tag != "DATAEND") {
        tag = readEndSecure(from, tmp);
    }
    return true;
}



/****************************************************************************/

