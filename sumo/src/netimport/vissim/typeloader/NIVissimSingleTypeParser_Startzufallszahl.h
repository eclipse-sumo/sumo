/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Startzufallszahl.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 30 Apr 2003
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
#ifndef NIVissimSingleTypeParser_Startzufallszahl_h
#define NIVissimSingleTypeParser_Startzufallszahl_h


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
 * @class NIVissimSingleTypeParser_Startzufallszahl
 *
 */
class NIVissimSingleTypeParser_Startzufallszahl :
            public NIVissimLoader::VissimSingleTypeParser
{
public:
    /// Constructor
    NIVissimSingleTypeParser_Startzufallszahl(NIVissimLoader &parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Startzufallszahl();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

};


#endif

/****************************************************************************/

