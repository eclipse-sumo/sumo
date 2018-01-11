/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIShapeContainer.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    08.10.2009
/// @version $Id$
///
// Storage for geometrical objects extended by mutexes
/****************************************************************************/
#ifndef GUIShapeContainer_h
#define GUIShapeContainer_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/shapes/ShapeContainer.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/foxtools/MFXMutex.h>

// ===========================================================================
// class declarations
// ===========================================================================
class SUMORTree;
class Position;
class MFXMutex;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIShapeContainer
 * @brief Storage for geometrical objects extended by mutexes
 * @see ShapeContainer
 */
class GUIShapeContainer : public ShapeContainer {
public:
    /// @brief Constructor
    GUIShapeContainer(SUMORTree& vis);


    /// @brief Destructor
    virtual ~GUIShapeContainer();

    /** @brief Builds a polygon using the given values and adds it to the container
     * @param[in] id The name of the polygon
     * @param[in] type The (abstract) type of the polygon
     * @param[in] color The color of the polygon
     * @param[in] layer The layer of the polygon
     * @param[in] angle The rotation of the polygon
     * @param[in] imgFile The raster image of the polygon
     * @param[in] shape The shape of the polygon
     * @param[in] geo specify if shape was loaded as GEO coordinate
     * @param[in] fill Whether the polygon shall be filled
     * @return whether the polygon could be added
     */
    virtual bool addPolygon(const std::string& id, const std::string& type, const RGBColor& color, double layer,
                            double angle, const std::string& imgFile, const PositionVector& shape, bool geo,
                            bool fill, bool ignorePruning = false);


    /** @brief Builds a POI using the given values and adds it to the container
     * @param[in] id The name of the POI
     * @param[in] type The (abstract) type of the POI
     * @param[in] color The color of the POI
     * @param[in] pos The position of the POI
     * @param[in[ geo use GEO coordinates (lon/lat)
     * @param[in] lane The Lane in which this POI is placed
     * @param[in] posOverLane The position over Lane
     * @param[in] posLat The position lateral over Lane
     * @param[in] layer The layer of the POI
     * @param[in] angle The rotation of the POI
     * @param[in] imgFile The raster image of the POI
     * @param[in] width The width of the POI image
     * @param[in] height The height of the POI image
     * @return whether the poi could be added
     */
    virtual bool addPOI(const std::string& id, const std::string& type, const RGBColor& color, const Position& pos, bool geo,
                        const std::string& lane, double posOverLane, double posLat, double layer, double angle,
                        const std::string& imgFile, double width, double height, bool ignorePruning = false);



    /** @brief Removes a polygon from the container
     * @param[in] id The id of the polygon
     * @return Whether the polygon could be removed
     */
    virtual bool removePolygon(const std::string& id);


    /** @brief Removes a PoI from the container
     * @param[in] id The id of the PoI
     * @return Whether the poi could be removed
     */
    virtual bool removePOI(const std::string& id);



    /** @brief Assigns a new position to the named PoI
     * @param[in] id The id of the PoI to move
     * @param[in] pos The PoI's new position
     */
    virtual void movePOI(const std::string& id, const Position& pos);


    /** @brief Assigns a shape to the named polygon
     * @param[in] id The id of the polygon to reshape
     * @param[in] shape The polygon's new shape
     */
    virtual void reshapePolygon(const std::string& id, const PositionVector& shape);



    /// Returns the gl-ids of all pois
    std::vector<GUIGlID> getPOIIds() const;
    /// Returns the gl-ids of all polygons
    std::vector<GUIGlID> getPolygonIDs() const;


private:
    /// @brief The mutex for adding/removing operations
    mutable MFXMutex myLock;

    /// @brief The RTree structure to add and remove visualization elements
    SUMORTree& myVis;

};


#endif

/****************************************************************************/

