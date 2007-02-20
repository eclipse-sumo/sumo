/****************************************************************************/
/// @file    GeoConvHelper.h
/// @author  Daniel Krajzewicz
/// @date    2006-08-01
/// @version $Id$
///
// static methods for processing the coordinates conversion for the current net
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
#include <utils/geom/Boundary.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GeoConvHelper
 * @brief static methods for processing the coordinates conversion for the current net
 */
class GeoConvHelper
{
public:
    /// Initialises the subsystem using the given proj.4-definition and a network offset
    static bool init(const std::string &proj,
                     const Position2D &offset);

    /// Closes the subsystem
    static void close();

    /// Converts the given cartesian (shifted) position to its geo (lat/long) representation
    static void cartesian2geo(Position2D &cartesian);

    /// Converts the given coordinate into a cartesian using the previous initialisation
    static void x2cartesian(Position2D &from);

    /// Returns the information whether the subsystem was initialised
    static bool usingGeoProjection();

    /// Lets this subsystem know that the given value is within the regarded area
    static void includeInOriginal(SUMOReal x, SUMOReal y);

    /// Shifts the converted boundary by the given amounts
    static void moveConvertedBy(SUMOReal x, SUMOReal y);

    /// Returns the original boundary
    static const Boundary &getOrigBoundary();

    /// Returns the converted boundary
    static const Boundary &getConvBoundary();

    /// Returns the network offset
    static const Position2D &getOffset();

private:
    /// The proj.4-projectsion to use
    static projPJ myProjection;

    /// The offset to apply
    static Position2D myOffset;

    /// Information whether no projection shall be done
    static bool myDisableProjection;

    /// The initial x/y-coordinates for a very simple geocoordinates conversion
    static SUMOReal myInitX, myInitY;

    /// The boundary before conversion (x2cartesian)
    static Boundary myOrigBoundary;

    /// The boundary after conversion (x2cartesian)
    static Boundary myConvBoundary;

};


#endif

/****************************************************************************/

