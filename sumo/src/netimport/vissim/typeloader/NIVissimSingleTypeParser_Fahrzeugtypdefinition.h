/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fahrzeugtypdefinition.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimSingleTypeParser_Fahrzeugtypdefinition_h
#define NIVissimSingleTypeParser_Fahrzeugtypdefinition_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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

