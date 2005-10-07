#ifndef IncludedRoutes_h
#define IncludedRoutes_h
//---------------------------------------------------------------------------//
//                        IncludedRoutes.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : July 2005
//  copyright            : (C) 2005 by Danilo Boyom
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : danilo.tete-boyom@dlr.com
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.2  2005/10/07 11:42:59  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.1  2005/09/15 12:09:27  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2005/09/09 12:53:16  dksumo
// tools added
//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>



/* =========================================================================
 * class definitions
 * ======================================================================= */

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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
