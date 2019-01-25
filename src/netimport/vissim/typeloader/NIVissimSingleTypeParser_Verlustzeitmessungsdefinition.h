/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Verlustzeitmessungsdefinition.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
#ifndef NIVissimSingleTypeParser_Verlustzeitmessungsdefinition_h
#define NIVissimSingleTypeParser_Verlustzeitmessungsdefinition_h


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
 * @class NIVissimSingleTypeParser_Verlustzeitmessungsdefinition
 *
 */
class NIVissimSingleTypeParser_Verlustzeitmessungsdefinition :
    public NIImporter_Vissim::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser_Verlustzeitmessungsdefinition(NIImporter_Vissim& parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Verlustzeitmessungsdefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream& from);

};


#endif

/****************************************************************************/

