/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include "../NIImporter_Vissim.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition
 *
 */
class NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition :
    public NIImporter_Vissim::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition(NIImporter_Vissim& parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream& from);

private:
    /// ?? (unknown vissim-description)
    bool parseOnlyMe(std::istream& from);

    /// parses a disturbance described by its position
    bool parsePositionDescribed(std::istream& from);

    /// parses a full description of a disturbance
    bool parseNumbered(std::istream& from);

    NIVissimExtendedEdgePoint parsePos(std::istream& from);

};


#endif

/****************************************************************************/

