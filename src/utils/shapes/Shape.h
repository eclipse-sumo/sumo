/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Shape.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Oct 2012
/// @version $Id$
///
// A 2D- or 3D-Shape
/****************************************************************************/
#ifndef Shape_h
#define Shape_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/Named.h>
#include <utils/common/RGBColor.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Shape
 * @brief A 2D- or 3D-Shape
 */
class Shape : public Named {
public:
    /** @brief Constructor
     * @param[in] id The name of the shape
     * @param[in] type The (abstract) type of the shape
     * @param[in] color The color of the shape
     * @param[in] layer The layer of the shape
     * @param[in] angle The rotation of the shape in navigational degrees
     * @param[in] imgFile The raster image of the shape
     */
    Shape(const std::string& id, const std::string& type,
          const RGBColor& color, double layer,
          double angle, const std::string& imgFile);


    /// @brief Destructor
    virtual ~Shape();


    /// @name Getter
    /// @{

    /** @brief Returns the (abstract) type of the Shape
     * @return The Shape's (abstract) type
     */
    inline const std::string& getType() const {
        return myType;
    }


    /** @brief Returns the color of the Shape
     * @return The Shape's color
     */
    inline const RGBColor& getColor() const {
        return myColor;
    }


    /** @brief Returns the layer of the Shape
     * @return The Shape's layer
     */
    inline double getLayer() const {
        return myLayer;
    }

    /** @brief Returns the angle of the Shape in navigational degrees
     * @return The Shape's rotation angle
     */
    inline double getNaviDegree() const {
        return myNaviDegreeAngle;
    }

    /** @brief Returns the imgFile of the Shape
     * @return The Shape's rotation imgFile
     */
    inline const std::string& getImgFile() const {
        return myImgFile;
    }
    /// @}


    /// @name Setter
    /// @{

    /** @brief Sets a new type
     * @param[in] type The new type to use
     */
    inline void setType(const std::string& type) {
        myType = type;
    }


    /** @brief Sets a new color
     * @param[in] col The new color to use
     */
    inline void setColor(const RGBColor& col) {
        myColor = col;
    }


    /** @brief Sets a new layer
     * @param[in] layer The new layer to use
     */
    inline void setLayer(const double layer) {
        myLayer = layer;
    }


    /** @brief Sets a new angle in navigational degrees
     * @param[in] layer The new angle to use
     */
    inline void setNaviDegree(const double angle) {
        myNaviDegreeAngle = angle;
    }

    /** @brief Sets a new imgFile
     * @param[in] imgFile The new imgFile to use
     */
    inline void setImgFile(const std::string& imgFile) {
        myImgFile = imgFile;
    }
    /// @}

    static const std::string DEFAULT_TYPE;
    static const double DEFAULT_LAYER;
    static const double DEFAULT_ANGLE;
    static const std::string DEFAULT_IMG_FILE;
    static const double DEFAULT_IMG_WIDTH;
    static const double DEFAULT_IMG_HEIGHT;

protected:
    /// @brief The type of the Shape
    std::string myType;

    /// @brief The color of the Shape
    RGBColor myColor;

    /// @brief The layer of the Shape
    double myLayer;

    /// @brief The angle of the Shape
    double myNaviDegreeAngle;

    /// @brief The angle of the Shape
    std::string myImgFile;
};


#endif

/****************************************************************************/

