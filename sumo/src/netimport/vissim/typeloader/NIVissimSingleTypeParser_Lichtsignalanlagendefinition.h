/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Lichtsignalanlagendefinition.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id: $
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimSingleTypeParser_Lichtsignalanlagendefinition_h
#define NIVissimSingleTypeParser_Lichtsignalanlagendefinition_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <string>
#include "../NIVissimLoader.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVissimSingleTypeParser_Lichtsignalanlagendefinition
 *
 */
class NIVissimSingleTypeParser_Lichtsignalanlagendefinition :
            public NIVissimLoader::VissimSingleTypeParser
{
public:
    /// Constructor
    NIVissimSingleTypeParser_Lichtsignalanlagendefinition(NIVissimLoader &parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Lichtsignalanlagendefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

private:
    /// parses a traffic light with fixed times (no other types are supported by now)
    bool parseFixedTime(int id, std::string name, std::istream &from);

    /** @brief Parses a vas-traffic light;
        All other actuated traffic lights are parsed using "parseRestActuated"
        as they have a different format */
    bool parseVAS(int id, std::string name, std::istream &from);

    /// Parses actuated traffic lights (beside VAS)
    bool parseRestActuated(int id, std::string name, std::istream &from,
                           const std::string &type);

};


#endif

/****************************************************************************/

