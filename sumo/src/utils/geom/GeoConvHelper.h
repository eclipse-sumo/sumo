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
// class declarations
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GeoConvHelper
 * @brief static methods for processing the coordinates conversion for the current net
 */
class GeoConvHelper {
public:
    /** @brief Adds projection options to the given container
     *
     * @param[in] oc The options container to add the options to
     * @todo let the container be retrieved
     */
    static void addProjectionOptions(OptionsCont &oc);

    /// Initialises the subsystem using the given options
    static bool init(OptionsCont &oc);

    /// Initialises the subsystem using the given proj.4-definition and a network offset
    static bool init(const std::string &proj,
                     const SUMOReal scale=1.0f,
                     bool inverse=false);

    /// Initialises the subsystem using the given proj.4-definition and complete network parameter
    static bool init(const std::string &proj,
                     const Position2D &offset,
                     const Boundary &orig,
                     const Boundary &conv);

    /// Closes the subsystem
    static void close();

    /// Converts the given cartesian (shifted) position to its geo (lat/long) representation
    static void cartesian2geo(Position2D &cartesian);

    /// Converts the given coordinate into a cartesian using the previous initialisation
    static bool x2cartesian(Position2D &from, bool includeInBoundary=true);

    /// Returns whether a transformation from geo to metric coordinates will be performed
    static bool usingGeoProjection();

    /// Returns the information whether an inverse transformation will happen
    static bool usingInverseGeoProjection();

    /// Shifts the converted boundary by the given amounts
    static void moveConvertedBy(SUMOReal x, SUMOReal y);

    /// Returns the original boundary
    static const Boundary &getOrigBoundary();

    /// Returns the converted boundary
    static const Boundary &getConvBoundary();

    /// Returns the network offset
    static const Position2D &getOffset();

    /// Returns the network base
    static const Position2D getOffsetBase();

    /// Returns the network offset
    static const std::string &getProjString();

private:
    enum ProjectionMethod {
        NONE,
        SIMPLE,
        UTM,
        DHDN,
        PROJ
    };

    /// A proj options string describing the proj.4-projection to use
    static std::string myProjString;

#ifdef HAVE_PROJ
    /// The proj.4-projection to use
    static projPJ myProjection;
#endif

    /// The offset to apply
    static Position2D myOffset;

    /// The scaling to apply to geo-coordinates
    static SUMOReal myGeoScale;

    /// Information whether no projection shall be done
    static ProjectionMethod myProjectionMethod;

    /// Information whether inverse projection shall be used
    static bool myUseInverseProjection;

    /// Information whether the first node conversion was done
    static bool myBaseFound;

    /// The initial x/y-coordinates for a very simple geocoordinates conversion
    static Position2D myBase;

    /// The boundary before conversion (x2cartesian)
    static Boundary myOrigBoundary;

    /// The boundary after conversion (x2cartesian)
    static Boundary myConvBoundary;

};


#endif

/****************************************************************************/

