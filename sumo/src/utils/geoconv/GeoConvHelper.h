/****************************************************************************/
/// @file    GeoConvHelper.h
/// @author  Daniel Krajzewicz
/// @date    2006-08-01
/// @version $Id$
///
// missing_desc
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
#ifndef GeoConvHelper_h
#define GeoConvHelper_h
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

#include <map>
#include <string>
#include <proj_api.h>
#include <utils/geom/Position2D.h>

class GeoConvHelper
{
public:
    static bool init(const std::string &proj,
                     const Position2D &offset);
    static bool initialised();
    static void close();
    static void cartesian2geo(Position2D &cartesian);
    static void remap(Position2D &from);

private:
    static projPJ myProjection;
    static Position2D myOffset;
    static bool myDisableProjection;
    static SUMOReal myInitX, myInitY;

};


#endif

/****************************************************************************/

