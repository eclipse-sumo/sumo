/****************************************************************************/
/// @file    PointOfInterest.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Melanie Knocke
/// @date    2005-09-15
/// @version $Id$
///
// A point-of-interest (2D)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2005-2014 DLR (http://www.dlr.de/) and contributors
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

#include <utils/geom/Position.h>
#include <utils/common/Parameterised.h>
#include "Shape.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PointOfInterest
 * @brief A point-of-interest
 */
class PointOfInterest : public Shape, public Position, public Parameterised {
public:
    /** @brief Constructor
     * @param[in] id The name of the POI
     * @param[in] type The (abstract) type of the POI
     * @param[in] color The color of the POI
     * @param[in] layer The layer of the POI
     * @param[in] angle The rotation of the POI
     * @param[in] imgFile The raster image of the shape
     * @param[in] pos The position of the POI
     * @param[in] width The width of the POI image
     * @param[in] height The height of the POI image
     */
    PointOfInterest(const std::string& id, const std::string& type,
                    const RGBColor& color, const Position& pos,
                    SUMOReal layer = DEFAULT_LAYER,
                    SUMOReal angle = DEFAULT_ANGLE,
                    const std::string& imgFile = DEFAULT_IMG_FILE,
                    SUMOReal width = DEFAULT_IMG_WIDTH,
                    SUMOReal height = DEFAULT_IMG_HEIGHT) :
        Shape(id, type, color, layer, angle, imgFile),
        Position(pos),
        myHalfImgWidth(width / 2.0),
        myHalfImgHeight(height / 2.0)
    {}


    /// @brief Destructor
    virtual ~PointOfInterest() { }



    /// @name Getter
    /// @{

    /// @brief Returns whether the image width of the POI
    inline SUMOReal getWidth() const {
        return myHalfImgWidth * 2.0;
    }

    /// @brief Returns whether the image hidth of the POI
    inline SUMOReal getHeight() const {
        return myHalfImgHeight * 2.0;
    }
    /// @}



    /// @name Setter
    /// @{

    /// @brief set the image width of the POI
    inline void setWidth(SUMOReal width) {
        myHalfImgWidth = width / 2.0;
    }

    /// @brief set the image height of the POI
    inline void setHeight(SUMOReal height) {
        myHalfImgHeight = height / 2.0;
    }
    /// @}



protected:
    ///@brief The half width of the image when rendering this POI
    SUMOReal myHalfImgWidth;

    ///@brief The half height of the image when rendering this POI
    SUMOReal myHalfImgHeight;

};


#endif

/****************************************************************************/

