/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Langsamfahrbereichdefinition.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimSingleTypeParser_Langsamfahrbereichdefinition_h
#define NIVissimSingleTypeParser_Langsamfahrbereichdefinition_h


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
 * @class NIVissimSingleTypeParser_Langsamfahrbereichdefinition
 *
 */
class NIVissimSingleTypeParser_Langsamfahrbereichdefinition :
            public NIImporter_Vissim::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser_Langsamfahrbereichdefinition(NIImporter_Vissim &parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Langsamfahrbereichdefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

};


#endif

/****************************************************************************/

