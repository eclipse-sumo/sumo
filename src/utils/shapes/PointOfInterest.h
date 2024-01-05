/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2005-2024 German Aerospace Center (DLR) and others.
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
/// @file    PointOfInterest.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Melanie Knocke
/// @date    2005-09-15
///
// A point-of-interest (2D)
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/FileHelpers.h>
#include <utils/common/Parameterised.h>
#include <utils/common/StringBijection.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/Position.h>
#include <utils/iodevices/OutputDevice.h>

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
     * @param[in] pos The position of the POI
     * @param[in[ geo use GEO coordinates (lon/lat)
     * @param[in] lane The Lane in which this POI is placed
     * @param[in] friendlyPos friendly position
     * @param[in] posOverLane The position over Lane
     * @param[in] posLat The position lateral over Lane
     * @param[in] icon The icon of the POI
     * @param[in] layer The layer of the POI
     * @param[in] angle The rotation of the POI
     * @param[in] imgFile The raster image of the shape
     * @param[in] relativePath set image file as relative path
     * @param[in] width The width of the POI image
     * @param[in] height The height of the POI image
     * @param[in] name POI name
     * @param[in] parameters generic parameters
     */
    PointOfInterest(const std::string& id, const std::string& type,
                    const RGBColor& color, const Position& pos, bool geo,
                    const std::string& lane, double posOverLane,
                    bool friendlyPos, double posLat,
                    const std::string& icon,
                    double layer = DEFAULT_LAYER,
                    double angle = DEFAULT_ANGLE,
                    const std::string& imgFile = DEFAULT_IMG_FILE,
                    bool relativePath = DEFAULT_RELATIVEPATH,
                    double width = DEFAULT_IMG_WIDTH,
                    double height = DEFAULT_IMG_HEIGHT,
                    const std::string& name = DEFAULT_NAME,
                    const Parameterised::Map& parameters = DEFAULT_PARAMETERS);

    /// @brief Destructor
    ~PointOfInterest();

    /// @name Getter
    /// @{

    /// @brief get icon
    POIIcon getIcon() const;

    /// @brief get icon(in string format)
    const std::string& getIconStr() const;

    /// @brief Returns the image width of the POI
    double getWidth() const;

    /// @brief Returns the image height of the POI
    double getHeight() const;

    /// @brief Returns the image center of the POI
    Position getCenter() const;

    /// @brief returns friendly position
    bool getFriendlyPos() const;

    /// @}


    /// @name Setter
    /// @{

    /// @brief set icon
    void setIcon(const std::string& icon);

    /// @brief set the image width of the POI
    void setWidth(double width);

    /// @brief set the image height of the POI
    void setHeight(double height);

    /// @brief set friendly position
    void setFriendlyPos(const bool friendlyPos);

    /// @}

    /* @brief POI definition to the given device
     * @param[in] geo  Whether to write the output in geo-coordinates
     */
    void writeXML(OutputDevice& out, const bool geo = false, const double zOffset = 0., const std::string laneID = "", const double pos = 0., const bool friendlyPos = false, const double posLat = 0.) const;

protected:
    /// @brief flag to check if POI was loaded as GEO Position (main used by netedit)
    bool myGeo;

    /// @brief ID of lane in which this POI is placed (main used by netedit)
    std::string myLane;

    /// @brief position over lane in which this POI is placed (main used by netedit)
    double myPosOverLane;

    /// @brief friendlyPos enable or disable friendly position for position over lane
    bool myFriendlyPos;

    /// @brief lateral position over lane in which this POI is placed (main used by netedit)
    double myPosLat;

    /// @brief POI icon
    POIIcon myIcon;

    /// @brief The half width of the image when rendering this POI
    double myHalfImgWidth;

    /// @brief The half height of the image when rendering this POI
    double myHalfImgHeight;
};
