/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition.h
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
#ifndef NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition_h
#define NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition_h


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
 * @class NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition
 *
 */
class NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition :
            public NIVissimLoader::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition(NIVissimLoader &parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

private:
    /// ?? (unknown vissim-description)
    bool parseOnlyMe(std::istream &from);

    /// parses a disturbance described by its position
    bool parsePositionDescribed(std::istream &from);

    /// parses a full description of a disturbance
    bool parseNumbered(std::istream &from);

    NIVissimExtendedEdgePoint parsePos(std::istream &from);

};


#endif

/****************************************************************************/

