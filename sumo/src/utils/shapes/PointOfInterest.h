/****************************************************************************/
/// @file    PointOfInterest.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    2005-09-15
/// @version $Id$
///
// A point-of-interest (2D)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/geom/Position.h>
#include <utils/common/Named.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PointOfInterest
 * @brief A point-of-interest
 */
class PointOfInterest : public RGBColor, public Position, public Named {
public:
    /** @brief Constrcutor
     * @param[in] id The id of the POI
     * @param[in] type The (abstract) type of the poi
     * @param[in] p The position of the poi
     * @param[in] c The color of the poi
     */
    PointOfInterest(const std::string& id, const std::string& type,
                    const Position& p, const RGBColor& c)
        : RGBColor(c), Position(p), Named(id), myType(type) { }


    /// @brief Destructor
    virtual ~PointOfInterest() { }



    /** @brief Returns the (abstract) type of the poi
     * @return The polygon's (abstract) type
     */
    const std::string& getType() const {
        return myType;
    }


    /** @brief Sets a new type
     * @param[in] type The new type to use
     */
    void setType(const std::string& type) {
        myType = type;
    }


protected:
    /// @brief the type of the poi
    std::string myType;

};


#endif

/****************************************************************************/

