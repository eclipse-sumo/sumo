/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2023 German Aerospace Center (DLR) and others.
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
/// @file    SUMOPolygon.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Melanie Knocke
/// @date    Jun 2004
///
// A 2D- or 3D-polygon
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/geom/PositionVector.h>
#include <utils/common/Parameterised.h>
#include "Shape.h"


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Polygon
 * @brief A 2D- or 3D-polygon
 */
class SUMOPolygon : public Shape, public Parameterised {

public:
    /// @brief friend class
    friend class PolygonDynamics;

    /** @brief Constructor
     * @param[in] id The name of the polygon
     * @param[in] type The (abstract) type of the polygon
     * @param[in] color The color of the polygon
     * @param[in] layer The layer of the polygon
     * @param[in] angle The rotation of the polygon
     * @param[in] imgFile The raster image of the polygon
     * @param[in] shape The shape of the polygon
     * @param[in] geo specify if shape was loaded as GEO
     * @param[in] fill Whether the polygon shall be filled
     * @param[in] lineWidth The line with for drawing an unfilled polygon
     * @param[in] relativePath set image file as relative path
     * @param[in] name Polygon name
     * @param[in] parameters generic parameters
     */
    SUMOPolygon(const std::string& id, const std::string& type, const RGBColor& color,
                const PositionVector& shape, bool geo, bool fill, double lineWidth,
                double layer = DEFAULT_LAYER,
                double angle = DEFAULT_ANGLE,
                const std::string& imgFile = DEFAULT_IMG_FILE,
                bool relativePath = DEFAULT_RELATIVEPATH,
                const std::string& name = DEFAULT_NAME,
                const Parameterised::Map& parameters = DEFAULT_PARAMETERS);

    /// @brief Destructor
    ~SUMOPolygon();

    /// @name Getter
    /// @{

    /** @brief Returns whether the shape of the polygon
     * @return The shape of the polygon
     */
    const PositionVector& getShape() const;

    /** @brief Returns whether the polygon is filled
     * @return Whether the polygon is filled
     */
    bool getFill() const;

    /** @brief Returns whether the polygon is filled
     * @return Whether the polygon is filled
     */
    double getLineWidth() const;
    /// @}

    /// @name Setter
    /// @{

    /** @brief Sets whether the polygon shall be filled
     * @param[in] fill Whether the polygon shall be filled
     */
    void setFill(bool fill);

    /// @brief set line width
    void setLineWidth(double lineWidth);

    /** @brief Sets the shape of the polygon
     * @param[in] shape  The new shape of the polygon
     */
    virtual void setShape(const PositionVector& shape);

    /// @}

    /* @brief polygon definition to the given device
     * @param[in] geo  Whether to write the output in geo-coordinates
     */
    void writeXML(OutputDevice& out, bool geo = false) const;

protected:
    /// @brief The positions of the polygon
    PositionVector myShape;

    /// @brief specify if shape is handled as GEO coordinate (Main used in netedit)
    bool myGEO;

    /// @brief Information whether the polygon has to be filled
    bool myFill;

    /// @brief The line width for drawing an unfilled polygon
    double myLineWidth;
};
