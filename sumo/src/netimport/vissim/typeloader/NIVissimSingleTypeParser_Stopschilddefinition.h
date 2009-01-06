/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Stopschilddefinition.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimSingleTypeParser_Stopschilddefinition_h
#define NIVissimSingleTypeParser_Stopschilddefinition_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include "../NIVissimLoader.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVissimSingleTypeParser_Stopschilddefinition
 *
 */
class NIVissimSingleTypeParser_Stopschilddefinition :
            public NIVissimLoader::VissimSingleTypeParser
{
public:
    /// Constructor
    NIVissimSingleTypeParser_Stopschilddefinition(NIVissimLoader &parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Stopschilddefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

};


#endif

/****************************************************************************/

