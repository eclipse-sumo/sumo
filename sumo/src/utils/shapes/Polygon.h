/****************************************************************************/
/// @file    Polygon.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Jun 2004
/// @version $Id$
///
// A 2D- or 3D-polygon
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
#ifndef Polygon_h
#define Polygon_h


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
#include <utils/geom/PositionVector.h>
#include <utils/common/VectorHelper.h>
#include <utils/geom/Position.h>
#include <map>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Polygon
 * @brief A 2D- or 3D-polygon
 */
class Polygon {
public:
    /** @brief Constructor
     * @param[in] name The name of the polygon
     * @param[in] type The (abstract) type of the polygon
     * @param[in] color The color of the polygon
     * @param[in] shape The shape of the poslygon
     * @param[in] fill Whether the polygon shall be filled
     */
    Polygon(const std::string& name, const std::string& type,
            const RGBColor& color, const PositionVector& shape, bool fill) ;


    /// @brief Destructor
    virtual ~Polygon() ;



    /// @name Getter
    /// @{

    /** @brief Returns the name of the polygon
     * @return The polygon's id
     */
    const std::string& getID() const {
        return myName;
    }


    /** @brief Returns the (abstract) type of the polygon
     * @return The polygon's (abstract) type
     */
    const std::string& getType() const {
        return myType;
    }


    /** @brief Returns the color of the polygon
     * @return The polygon's color
     */
    const RGBColor& getColor() const {
        return myColor;
    }


    /** @brief Returns the shape of the polygon
     * @return The polygon's shape
     */
    const PositionVector& getShape() const {
        return myShape;
    }


    /** @brief Returns whether the polygon is filled
     * @return Whether the polygon is filled
     */
    bool fill() const {
        return myFill;
    }
    /// @}



    /// @name Setter
    /// @{

    /** @brief Sets a new type
     * @param[in] type The new type to use
     */
    void setType(const std::string& type) {
        myType = type;
    }


    /** @brief Sets a new color
     * @param[in] col The new color to use
     */
    void setColor(const RGBColor& col) {
        myColor = col;
    }


    /** @brief Sets a new shape
     * @param[in] shape The new shape to use
     */
    void setShape(const PositionVector& shape) {
        myShape = shape;
    }


    /** @brief Sets whether the polygon shall be filled
     * @param[in] fill Whether the polygon shall be filled
     */
    void setFill(bool fill) {
        myFill = fill;
    }
    /// @}


protected:
    /// @brief The name of the polygon
    std::string myName;

    /// @brief The type of the polygon
    std::string myType;

    /// @brief The color of the polygon
    RGBColor myColor;

    /// @brief The positions of the polygon
    PositionVector myShape;

    /// @brief Information whether the polygon has to be filled
    bool myFill;

};


#endif

/****************************************************************************/

