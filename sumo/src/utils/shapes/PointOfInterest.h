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
// Copyright (C) 2005-2017 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/Parameterised.h>
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
     * @param[in] layer The layer of the POI
     * @param[in] angle The rotation of the POI
     * @param[in] imgFile The raster image of the shape
     * @param[in] pos The position of the POI
     * @param[in] width The width of the POI image
     * @param[in] height The height of the POI image
     */
    PointOfInterest(const std::string& id, const std::string& type,
                    const RGBColor& color, const Position& pos,
                    double layer = DEFAULT_LAYER,
                    double angle = DEFAULT_ANGLE,
                    const std::string& imgFile = DEFAULT_IMG_FILE,
                    double width = DEFAULT_IMG_WIDTH,
                    double height = DEFAULT_IMG_HEIGHT) :
        Shape(id, type, color, layer, angle, imgFile),
        Position(pos),
        myHalfImgWidth(width / 2.0),
        myHalfImgHeight(height / 2.0) {
    }


    /// @brief Destructor
    virtual ~PointOfInterest() { }



    /// @name Getter
    /// @{

    /// @brief Returns the image width of the POI
    inline double getWidth() const {
        return myHalfImgWidth * 2.0;
    }

    /// @brief Returns the image height of the POI
    inline double getHeight() const {
        return myHalfImgHeight * 2.0;
    }
    /// @}



    /// @name Setter
    /// @{

    /// @brief set the image width of the POI
    inline void setWidth(double width) {
        myHalfImgWidth = width / 2.0;
    }

    /// @brief set the image height of the POI
    inline void setHeight(double height) {
        myHalfImgHeight = height / 2.0;
    }
    /// @}


    /* @brief POI definition to the given device
     * @param[in] geo  Whether to write the output in geo-coordinates
     */
    void writeXML(OutputDevice& out, const bool geo = false, const double zOffset = 0., const std::string laneID = "", const double pos = 0., const double posLat = 0.) {
        out.openTag(SUMO_TAG_POI);
        out.writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML(getID()));
        if (getType().size() > 0) {
            out.writeAttr(SUMO_ATTR_TYPE, StringUtils::escapeXML(getType()));
        }
        out.writeAttr(SUMO_ATTR_COLOR, getColor());
        out.writeAttr(SUMO_ATTR_LAYER, getLayer() + zOffset);
        if (laneID != "") {
            out.writeAttr(SUMO_ATTR_LANE, laneID);
            out.writeAttr(SUMO_ATTR_POSITION, pos);
            if (posLat != 0) {
                out.writeAttr(SUMO_ATTR_POSITION_LAT, posLat);
            }
        } else {
            if (geo) {
                Position POICartesianPos(*this);
                GeoConvHelper::getFinal().cartesian2geo(POICartesianPos);
                out.writeAttr(SUMO_ATTR_LON, POICartesianPos.x());
                out.writeAttr(SUMO_ATTR_LAT, POICartesianPos.y());
            } else {
                out.writeAttr(SUMO_ATTR_X, x());
                out.writeAttr(SUMO_ATTR_Y, y());
            }
        }
        if (getNaviDegree() != Shape::DEFAULT_ANGLE) {
            out.writeAttr(SUMO_ATTR_ANGLE, getNaviDegree());
        }
        if (getImgFile() != Shape::DEFAULT_IMG_FILE) {
            out.writeAttr(SUMO_ATTR_IMGFILE, getImgFile());
        }
        if (getWidth() != Shape::DEFAULT_IMG_WIDTH) {
            out.writeAttr(SUMO_ATTR_WIDTH, getWidth());
        }
        if (getHeight() != Shape::DEFAULT_IMG_HEIGHT) {
            out.writeAttr(SUMO_ATTR_HEIGHT, getHeight());
        }
        writeParams(out);
        out.closeTag();
    }


protected:
    ///@brief The half width of the image when rendering this POI
    double myHalfImgWidth;

    ///@brief The half height of the image when rendering this POI
    double myHalfImgHeight;

};


#endif

/****************************************************************************/

