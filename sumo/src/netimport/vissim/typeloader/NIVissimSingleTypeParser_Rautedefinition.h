/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Rautedefinition.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id: NIVissimSingleTypeParser_Rautedefinition.h 4389 2007-08-28 10:21:00Z behrisch $
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
#ifndef NIVissimSingleTypeParser_Rautedefinition_h
#define NIVissimSingleTypeParser_Rautedefinition_h


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
 * @class NIVissimSingleTypeParser_Rautedefinition
 *
 */
class NIVissimSingleTypeParser_Rautedefinition :
            public NIVissimLoader::VissimSingleTypeParser
{
public:
    /// Constructor
    NIVissimSingleTypeParser_Rautedefinition(NIVissimLoader &parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Rautedefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

};


#endif

/****************************************************************************/

