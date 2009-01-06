/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fensterdefinition.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 21 Mar 2003
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
#ifndef NIVissimSingleTypeParser_Fensterdefinition_h
#define NIVissimSingleTypeParser_Fensterdefinition_h


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
 * @class NIVissimSingleTypeParser_Fensterdefinition
 *
 */
class NIVissimSingleTypeParser_Fensterdefinition :
            public NIVissimLoader::VissimSingleTypeParser
{
public:
    /// Constructor
    NIVissimSingleTypeParser_Fensterdefinition(NIVissimLoader &parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Fensterdefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

};


#endif

/****************************************************************************/

