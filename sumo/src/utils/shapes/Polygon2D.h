/****************************************************************************/
/// @file    Polygon2D.h
/// @author  Daniel Krajzewicz
/// @date    Jun 2004
/// @version $Id$
///
// A 2D-polygon
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
#ifndef Polygon2D_h
#define Polygon2D_h


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
#include <utils/geom/Position2DVector.h>
#include <utils/common/VectorHelper.h>
#include <utils/geom/Position2D.h>
#include <map>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Polygon2D
 * @brief A 2D-polygon
 */
class Polygon2D {
public:
    /** @brief Constructor
     * @param[in] name The name of the polygon
     * @param[in] type The (abstract) type of the polygon
     * @param[in] color The color of the polygon
     * @param[in] shape The shape of the poslygon
     * @param[in] fill Whether the polygon shall be filled
     */
    Polygon2D(const std::string &name, const std::string &type,
              const RGBColor &color, const Position2DVector &shape, bool fill) throw();


    /// @brief Destructor
    virtual ~Polygon2D() throw();



    /// @name Getter
    /// @{

    /** @brief Returns the name of the polygon
     * @return The polygon's id
     */
    const std::string &getID() const throw() {
        return myName;
    }


    /** @brief Returns the (abstract) type of the polygon
     * @return The polygon's (abstract) type
     */
    const std::string &getType() const throw() {
        return myType;
    }


    /** @brief Returns the color of the polygon
     * @return The polygon's color
     */
    const RGBColor &getColor() const throw() {
        return myColor;
    }


    /** @brief Returns the shape of the polygon
     * @return The polygon's shape
     */
    const Position2DVector &getShape() const throw() {
        return myShape;
    }


    /** @brief Returns whether the polygon is filled
     * @return Whether the polygon is filled
     */
    bool fill() const throw() {
        return myFill;
    }
    /// @}



    /// @name Setter
    /// @{

    /** @brief Sets a new type
     * @param[in] type The new type to use
     */
    void setType(const std::string &type) throw() {
        myType = type;
    }


    /** @brief Sets a new color
     * @param[in] col The new color to use
     */
    void setColor(const RGBColor &col) throw() {
        myColor = col;
    }


    /** @brief Sets a new shape
     * @param[in] shape The new shape to use
     */
    void setShape(const Position2DVector &shape) throw() {
        myShape = shape;
    }


    /** @brief Sets whether the polygon shall be filled
     * @param[in] fill Whether the polygon shall be filled
     */
    void setFill(bool fill) throw() {
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
    Position2DVector myShape;

    /// @brief Information whether the polygon has to be filled
    bool myFill;

};


#endif

/****************************************************************************/

