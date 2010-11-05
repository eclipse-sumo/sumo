/****************************************************************************/
/// @file    PointOfInterest.h
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
// A point-of-interest (2D)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef PointOfInterest_h
#define PointOfInterest_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/RGBColor.h>
#include <utils/geom/Position2D.h>
#include <utils/common/Named.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PointOfInterest
 * @brief A point-of-interest (2D)
 */
class PointOfInterest :
            public RGBColor, public Position2D, public Named {
public:
    /** @brief Constrcutor
     * @param[in] id The id of the POI
     * @param[in] type The (abstract) type of the poi
     * @param[in] p The position of the poi
     * @param[in] c The color of the poi
     */
    PointOfInterest(const std::string &id, const std::string &type,
                    const Position2D &p, const RGBColor &c) throw()
            : RGBColor(c), Position2D(p), Named(id), myType(type) { }


    /// @brief Destructor
    virtual ~PointOfInterest() throw() { }



    /** @brief Returns the (abstract) type of the poi
     * @return The polygon's (abstract) type
     */
    const std::string &getType() const throw() {
        return myType;
    }


    /** @brief Sets a new type
     * @param[in] type The new type to use
     */
    void setType(const std::string &type) throw() {
        myType = type;
    }


protected:
    /// @brief the type of the poi
    std::string myType;

};


#endif

/****************************************************************************/

