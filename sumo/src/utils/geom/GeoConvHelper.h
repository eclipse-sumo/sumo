/****************************************************************************/
/// @file    GeoConvHelper.h
/// @author  Daniel Krajzewicz
/// @date    2006-08-01
/// @version $Id$
///
// static methods for processing the coordinates conversion for the current net
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
#ifndef GeoConvHelper_h
#define GeoConvHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>
#include <utils/geom/Position2D.h>
#include <utils/geom/Boundary.h>

#ifdef HAVE_PROJ
#include <proj_api.h>
#endif


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
                     const Position2D &offset,
                     bool inverse=false);

    /// Initialises the subsystem using the given proj.4-definition and complete network parameter
    static bool init(const std::string &proj,
                     const Position2D &offset,
                     const Boundary &orig,
                     const Boundary &conv,
                     bool inverse=false);

    /// Closes the subsystem
    static void close();

    /// Converts the given cartesian (shifted) position to its geo (lat/long) representation
    static void cartesian2geo(Position2D &cartesian);

    /// Converts the given coordinate into a cartesian using the previous initialisation
    static void x2cartesian(Position2D &from, bool includeInBoundary=true);

    /// Returns the information whether the subsystem was initialised
    static bool usingGeoProjection();

    /// Lets this subsystem know that the given position is within the regarded area
    static void includeInOriginal(SUMOReal x, SUMOReal y);

    /// Lets this subsystem know that the given position is within the regarded area
    static void includeInOriginal(const Position2D &p);

    /// Lets this subsystem know that the given boundary is within the regarded area
    static void includeInOriginal(const Boundary &b);

    /// Shifts the converted boundary by the given amounts
    static void moveConvertedBy(SUMOReal x, SUMOReal y);

    /// Returns the original boundary
    static const Boundary &getOrigBoundary();

    /// Returns the converted boundary
    static const Boundary &getConvBoundary();

    /// Returns the network offset
    static const Position2D &getOffset();

private:
#ifdef HAVE_PROJ
    /// The proj.4-projectsion to use
    static projPJ myProjection;
#endif

    /// The offset to apply
    static Position2D myOffset;

    /// Information whether no projection shall be done
    static bool myDisableProjection;

    /// Information whether inverse projection shall be used
    static bool myUseInverseProjection;

    /// The initial x/y-coordinates for a very simple geocoordinates conversion
    static SUMOReal myInitX, myInitY;

    /// The boundary before conversion (x2cartesian)
    static Boundary myOrigBoundary;

    /// The boundary after conversion (x2cartesian)
    static Boundary myConvBoundary;

};


#endif

/****************************************************************************/

