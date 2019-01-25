/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fahrzeugtypdefinition.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
#ifndef NIVissimSingleTypeParser_Fahrzeugtypdefinition_h
#define NIVissimSingleTypeParser_Fahrzeugtypdefinition_h


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
 * @class NIVissimSingleTypeParser_Fahrzeugtypdefinition
 *
 */
class NIVissimSingleTypeParser_Fahrzeugtypdefinition :
    public NIImporter_Vissim::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser_Fahrzeugtypdefinition(NIImporter_Vissim& parent,
            NIImporter_Vissim::ColorMap& colorMap);

    /// Destructor
    ~NIVissimSingleTypeParser_Fahrzeugtypdefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream& from);

private:
    /// The color map to use
    NIImporter_Vissim::ColorMap& myColorMap;
};


#endif

/****************************************************************************/

