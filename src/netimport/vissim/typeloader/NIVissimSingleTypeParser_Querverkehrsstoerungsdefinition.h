/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
#ifndef NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition_h
#define NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include "../NIImporter_Vissim.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition
 *
 */
class NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition :
    public NIImporter_Vissim::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition(NIImporter_Vissim& parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream& from);

private:
    /// ?? (unknown vissim-description)
    bool parseOnlyMe(std::istream& from);

    /// parses a disturbance described by its position
    bool parsePositionDescribed(std::istream& from);

    /// parses a full description of a disturbance
    bool parseNumbered(std::istream& from);

    NIVissimExtendedEdgePoint parsePos(std::istream& from);

};


#endif

/****************************************************************************/

