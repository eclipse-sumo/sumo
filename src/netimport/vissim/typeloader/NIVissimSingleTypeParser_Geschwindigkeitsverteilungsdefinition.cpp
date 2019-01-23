/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition.cpp
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
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/distribution/Distribution_Points.h>
#include <utils/distribution/DistributionCont.h>
#include "../NIImporter_Vissim.h"
#include "NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition::NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition::~NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition() {}


bool
NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition::parse(std::istream& from) {
    // id
    std::string id;
    from >> id;
    // list of points
    Distribution_Points* points = new Distribution_Points(id);
    std::string tag;
    do {
        tag = readEndSecure(from);
        if (tag == "name") {
            readName(from);
            tag = readEndSecure(from);
        }
        if (tag != "DATAEND") {
            const double p1 = StringUtils::toDouble(tag);
            from >> tag;
            const double p2 = StringUtils::toDouble(tag);
            points->add(p1, p2);
        }
    } while (tag != "DATAEND");
    DistributionCont::dictionary("speed", id, points);
    return true;
}



/****************************************************************************/

