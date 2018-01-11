/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SUMOPolygon.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Melanie Knocke
/// @date    Jun 2004
/// @version $Id$
///
// A 2D- or 3D-polygon
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
    /** @brief Constructor
     * @param[in] id The name of the polygon
     * @param[in] type The (abstract) type of the polygon
     * @param[in] color The color of the polygon
     * @param[in] layer The layer of the polygon
     * @param[in] angle The rotation of the polygon
     * @param[in] imgFile The raster image of the polygon
     * @param[in] shape The shape of the polygon
     * @param[in] geo specifiy if shape was loaded as GEO
     * @param[in] fill Whether the polygon shall be filled
     */
    SUMOPolygon(const std::string& id, const std::string& type,
                const RGBColor& color, const PositionVector& shape,
                bool geo, bool fill,
                double layer = DEFAULT_LAYER,
                double angle = DEFAULT_ANGLE,
                const std::string& imgFile = DEFAULT_IMG_FILE);


    /// @brief Destructor
    virtual ~SUMOPolygon();


    /// @name Getter
    /// @{

    /** @brief Returns whether the shape of the polygon
     * @return The shape of the polygon
     */
    inline const PositionVector& getShape() const {
        return myShape;
    }


    /** @brief Returns whether the polygon is filled
     * @return Whether the polygon is filled
     */
    inline bool getFill() const {
        return myFill;
    }
    /// @}


    /// @name Setter
    /// @{

    /** @brief Sets whether the polygon shall be filled
     * @param[in] fill Whether the polygon shall be filled
     */
    inline void setFill(bool fill) {
        myFill = fill;
    }


    /** @brief Sets the shape of the polygon
     * @param[in] shape  The new shape of the polygon
     */
    inline virtual void setShape(const PositionVector& shape) {
        myShape = shape;
    }
    /// @}

    /* @brief polygon definition to the given device
     * @param[in] geo  Whether to write the output in geo-coordinates
     */
    void writeXML(OutputDevice& out, bool geo = false);

protected:
    /// @brief The positions of the polygon
    PositionVector myShape;

    /// @brief specify if shape is handled as GEO coordinate (Main used in netedit)
    bool myGEO;

    /// @brief Information whether the polygon has to be filled
    bool myFill;
};


#endif

/****************************************************************************/
