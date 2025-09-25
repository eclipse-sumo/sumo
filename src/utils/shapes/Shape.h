/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    Shape.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Oct 2012
///
// A 2D- or 3D-Shape
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <map>
#include <utils/common/Named.h>
#include <utils/common/RGBColor.h>
#include <utils/common/Parameterised.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Shape
 * @brief A 2D- or 3D-Shape
 */
class Shape : public Named {
public:
    /// @name default shape's values
    /// @{
    static const std::string DEFAULT_TYPE;
    static const double DEFAULT_LAYER;
    static const double DEFAULT_LINEWIDTH;
    static const double DEFAULT_LAYER_POI;
    static const double DEFAULT_ANGLE;
    static const std::string DEFAULT_IMG_FILE;
    static const double DEFAULT_IMG_WIDTH;
    static const double DEFAULT_IMG_HEIGHT;
    static const std::string DEFAULT_NAME;
    static const Parameterised::Map DEFAULT_PARAMETERS;
    /// @}

    /** @brief default consructor
     * @param[in] id The name of the shape
     */
    Shape(const std::string& id);

    /** @brief Constructor
     * @param[in] id The name of the shape
     * @param[in] type The (abstract) type of the shape
     * @param[in] color The color of the shape
     * @param[in] layer The layer of the shape
     * @param[in] angle The rotation of the shape in navigational degrees
     * @param[in] imgFile The raster image of the shape
     * @param[in] name shape name
     */
    Shape(const std::string& id, const std::string& type, const RGBColor& color, double layer,
          double angle, const std::string& imgFile, const std::string& name);

    /// @brief Destructor
    virtual ~Shape();

    /**@brief write shape attributes in a xml file
    * @param[in] device device in which write parameters of shape
    */
    void writeShapeAttributes(OutputDevice& device, const RGBColor& defaultColor, const double defaultLayer) const;

    /// @name Getter
    /// @{

    /** @brief Returns the (abstract) type of the Shape
     * @return The Shape's (abstract) type
     */
    const std::string& getShapeType() const;

    /** @brief Returns the color of the Shape
     * @return The Shape's color
     */
    const RGBColor& getShapeColor() const;

    /** @brief Returns the layer of the Shape
     * @return The Shape's layer
     */
    double getShapeLayer() const;

    /** @brief Returns the angle of the Shape in navigational degrees
     * @return The Shape's rotation angle
     */
    double getShapeNaviDegree() const;

    /** @brief Returns the imgFile of the Shape
     * @return The Shape's rotation imgFile
     */
    const std::string& getShapeImgFile() const;

    /// @brief Returns the name of the Shape
    const std::string& getShapeName() const;

    /// @}

    /// @name Setter
    /// @{

    /** @brief Sets a new type
     * @param[in] type The new type to use
     */
    void setShapeType(const std::string& type);

    /** @brief Sets a new color
     * @param[in] col The new color to use
     */
    void setShapeColor(const RGBColor& col);

    /** @brief Sets a new alpha value
     * @param[in] alpha The new value to use
     */
    void setShapeAlpha(unsigned char alpha);

    /** @brief Sets a new layer
     * @param[in] layer The new layer to use
     */
    void setShapeLayer(const double layer);

    /** @brief Sets a new angle in navigational degrees
     * @param[in] layer The new angle to use
     */
    virtual void setShapeNaviDegree(const double angle);

    /** @brief Sets a new imgFile
     * @param[in] imgFile The new imgFile to use
     */
    void setShapeImgFile(const std::string& imgFile);

    /// @brief Sets a new shape name
    void setShapeName(const std::string& name);

    /// @}

private:
    /// @brief The type of the Shape
    std::string myType;

    /// @brief The color of the Shape
    RGBColor myColor;

    /// @brief The layer of the Shape
    double myLayer;

    /// @brief The angle of the Shape
    double myNaviDegreeAngle;

    /// @brief The img file (include path)
    std::string myImgFile;

    /// @brief shape name
    std::string myName;
};
