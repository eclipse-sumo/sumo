/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Routenentscheidungsdefinition.h
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
#ifndef NIVissimSingleTypeParser_Routenentscheidungsdefinition_h
#define NIVissimSingleTypeParser_Routenentscheidungsdefinition_h


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
 * @class NIVissimSingleTypeParser_Routenentscheidungsdefinition
 *
 */
class NIVissimSingleTypeParser_Routenentscheidungsdefinition :
            public NIImporter_Vissim::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser_Routenentscheidungsdefinition(NIImporter_Vissim &parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Routenentscheidungsdefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

};


#endif

/****************************************************************************/

