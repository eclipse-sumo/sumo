/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Simdauer.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
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
#include <utils/common/MsgHandler.h>
#include "../NIImporter_Vissim.h"
#include "NIVissimSingleTypeParser_Simdauer.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Simdauer::NIVissimSingleTypeParser_Simdauer(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Simdauer::~NIVissimSingleTypeParser_Simdauer() {}


bool
NIVissimSingleTypeParser_Simdauer::parse(std::istream& from) {
    std::string duration;
    from >> duration;
    // !!!
    try {
        StringUtils::toDouble(duration);
    } catch (...) {
        WRITE_ERROR("Simulation duration could not be parsed");
        return false;
    }
    return true;
}



/****************************************************************************/

