/****************************************************************************/
/// @file    findIncludingRoutes.h
/// @author  unknown_author
/// @date    July 2005
/// @version $Id: $
///
// Revision 1.2  2005/10/07 11:42:59  dkrajzew
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
#ifndef findIncludingRoutes_h
#define findIncludingRoutes_h

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
#include <fstream>
#include <string>
#include <vector>
#include <map>



// ===========================================================================
// class definitions
// ===========================================================================

class IncludedRoutes
{

public:
    /// Constructor
    IncludedRoutes(const char *route1, const char *route2);

    /// Destructor
    ~IncludedRoutes();

    /// load route-file and save the routes into a Dictionary
    /// a is for the Route's Number (e.g. a=1, load route 1)
    void loadRoutes(const int a);

    // compare all route to find the routes in dictionnary2
    // that include the route in dictionnary1
    void result(const char *output);

private:

    /// the Name of the route-file1
    const char *route_1;

    /// the type of the route-file2
    const char *route_2;

    /// Definition of the dictionary type
    typedef std::map<std::string, std::string> DictTypeRoute;

    /// The dictionary
    static DictTypeRoute myRouteDict1;
    static DictTypeRoute myRouteDict2;

};


#endif

/****************************************************************************/

