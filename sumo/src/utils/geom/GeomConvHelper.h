/****************************************************************************/
/// @file    GeomConvHelper.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2003
/// @version $Id$
///
// Some helping functions for geometry parsing
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
#ifndef GeomConvHelper_h
#define GeomConvHelper_h
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

#include <string>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/Boundary.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GeomConvHelper
 * This class holds some helping functions for the parsing of geometries
 */
class GeomConvHelper
{
public:
    /** @brief This method builds a Position2DVector from a string representation
     *
     * It is assumed, the vector is stored as "x,y{ x,y}*" where x and y are SUMOReals. */
    static Position2DVector parseShape(const std::string &shpdef);

    /** @brief This method builds a boundary from its string representation
     *
     * It is assumed that the boundary is stored as a quadruple of SUMOReal, divided by
     * ','
     */
    static Boundary parseBoundary(const std::string &def);

};


#endif

/****************************************************************************/

