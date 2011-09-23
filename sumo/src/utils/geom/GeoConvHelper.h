/****************************************************************************/
/// @file    GeoConvHelper.h
/// @author  Daniel Krajzewicz
/// @date    2006-08-01
/// @version $Id$
///
// static methods for processing the coordinates conversion for the current net
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <utils/geom/Position.h>
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

    /** @brief Constructor based on the stored options
     * @param[in] oc The OptionsCont from which to read options
     */
    GeoConvHelper(OptionsCont &oc); 

    /** @brief Constructor 
     */
    GeoConvHelper(const std::string &proj, const Position &offset, 
        const Boundary &orig, const Boundary &conv, int shift=0, bool inverse=false, bool baseFound=false); 


    /// @brief Destructor
    ~GeoConvHelper();


    /** @brief Adds projection options to the given container
     *
     * @param[in] oc The options container to add the options to
     * @todo let the container be retrieved
     */
    static void addProjectionOptions(OptionsCont &oc);

    /// Initialises the default instance using the given options
    static bool init(OptionsCont &oc);

    /// Initialises the default instance using the given proj.4-definition and complete network parameter
    static void init(const std::string &proj,
                     const Position &offset,
                     const Boundary &orig,
                     const Boundary &conv);

    /// instance is modified during use: boundary adapts may adapt to know coordinates etc
    static GeoConvHelper& getDefaultInstance() {
        return myDefault;
    }

    /// Converts the given cartesian (shifted) position to its geo (lat/long) representation
    void cartesian2geo(Position &cartesian);

    /// Converts the given coordinate into a cartesian using the previous initialisation
    bool x2cartesian(Position &from, bool includeInBoundary=true);

    /// Returns whether a transformation from geo to metric coordinates will be performed
    bool usingGeoProjection();

    /// Returns the information whether an inverse transformation will happen
    bool usingInverseGeoProjection();

    /// Shifts the converted boundary by the given amounts
    void moveConvertedBy(SUMOReal x, SUMOReal y);

    /// Returns the original boundary
    const Boundary &getOrigBoundary();

    /// Returns the converted boundary
    const Boundary &getConvBoundary();

    /// Returns the network offset
    const Position getOffset();

    /// Returns the network base
    const Position getOffsetBase();

    /// Returns the network offset
    const std::string &getProjString();

private:
    enum ProjectionMethod {
        NONE,
        SIMPLE,
        UTM,
        DHDN,
        PROJ
    };

    /// A proj options string describing the proj.4-projection to use
    std::string myProjString;

#ifdef HAVE_PROJ
    /// The proj.4-projection to use
    projPJ myProjection;
#endif

    /// The offset to apply
    Position myOffset;

    /// The scaling to apply to geo-coordinates
    double myGeoScale;

    /// Information whether no projection shall be done
    ProjectionMethod myProjectionMethod;

    /// Information whether inverse projection shall be used
    bool myUseInverseProjection;

    /// Information whether the first node conversion was done
    bool myBaseFound;

    /// The initial x/y-coordinates for a very simple geocoordinates conversion
    double myBaseX, myBaseY;

    /// The boundary before conversion (x2cartesian)
    Boundary myOrigBoundary;

    /// The boundary after conversion (x2cartesian)
    Boundary myConvBoundary;

    /// initi projection based on the known type and a given position
    void initProjection(double x, double y);

    /// @brief coordinate transformation which shows up in the location tag of the output
    static GeoConvHelper myDefault;

    /// @brief assignment operator.
    GeoConvHelper& operator=(const GeoConvHelper&);

    /// @brief invalidated copy constructor.
    GeoConvHelper(const GeoConvHelper&);

};


#endif

/****************************************************************************/

