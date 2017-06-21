
/****************************************************************************/
/// @file    GNEShapeHandler.h
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
/// @version $Id: GNEShapeHandler.h 23150 2017-02-27 12:08:30Z behrisch $
///
// The XML-Handler for Polys/POIs in netedit
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEShapeHandler_h
#define GNEShapeHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/RGBColor.h>
#include <utils/geom/Position.h>
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ShapeContainer;
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEShapeHandler
 * @brief The XML-Handler for network loading
 *
 * The SAX2-handler responsible for parsing networks and routes to load.
 * This is an extension of the MSRouteHandler as routes and vehicles may also
 *  be loaded from network descriptions.
 */
class GNEShapeHandler : public SUMOSAXHandler {
public:

public:
    /** @brief Constructor
     *
     * @param[in] file Name of the parsed file
     * @param[in] sc shapeContainer in which shapes will be saved
     */
    GNEShapeHandler(GNENet* net, const std::string& file, ShapeContainer& sc);

    /// @brief Destructor
    virtual ~GNEShapeHandler();

    /// @brief loads all of the given files
    static bool loadFiles(const std::vector<std::string>& files, GNEShapeHandler& sh);

    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     * @todo Refactor/describe
     */
    virtual void myStartElement(int element,
                                const SUMOSAXAttributes& attrs);
    //@}

    /// @brief set default values
    void setDefaults(const std::string& prefix, const RGBColor& color, const double layer, const bool fill = false);

    /// @brief parse and build POI
    void parseAndBuildPOI(const SUMOSAXAttributes& attrs, const bool ignorePruning, const bool useProcessing);

    /// @brief parse and build polygon
    void parseAndBuildPolygon(const SUMOSAXAttributes& attrs, const bool ignorePruning, const bool useProcessing);

    /** @brief Builds a POI using the given values and adds it to the container
     * @param[in] id The name of the POI
     * @param[in] type The (abstract) type of the POI
     * @param[in] color The color of the POI
     * @param[in] layer The layer of the POI
     * @param[in] angle The rotation of the POI
     * @param[in] imgFile The raster image of the POI
     * @param[in] pos The position of the POI
     * @param[in] width The width of the POI image
     * @param[in] height The height of the POI image
     * @return whether the poi could be added
     */
    static bool buildPOI(const std::string& id, const std::string& type, const RGBColor& color, double layer, double angle, 
                          const std::string& imgFile, const Position& pos, double width, double height, bool ignorePruning = false);

    /** @brief Builds a polygon using the given values and adds it to the container
     * @param[in] id The name of the polygon
     * @param[in] type The (abstract) type of the polygon
     * @param[in] color The color of the polygon
     * @param[in] layer The layer of the polygon
     * @param[in] angle The rotation of the polygon
     * @param[in] imgFile The raster image of the polygon
     * @param[in] shape The shape of the polygon
     * @param[in] fill Whether the polygon shall be filled
     * @return whether the polygon could be added
     */
    static bool buildPolygon(const std::string& id, const std::string& type, const RGBColor& color, double layer, double angle, 
                              const std::string& imgFile, const PositionVector& shape, bool fill, bool ignorePruning = false);

    /**@brief get lane position
     * @param[in] poi poi ID
     * @param[in] laneID lane ID
     * @param[in] SlanePos position in the lane
    */
    Position getLanePos(const std::string& poiID, const std::string& laneID, double lanePos) ;

private:
    /// @brief pointer to net
    GNENet* myNet;

    /// @brief shape container
    ShapeContainer& myShapeContainer;

    /// @brief The prefix to use
    std::string myPrefix;

    /// @brief The default color to use
    RGBColor myDefaultColor;

    /// @brief The default layer to use
    double myDefaultLayer;

    /// @brief Information whether polygons should be filled
    bool myDefaultFill;

    /// @brief invalid copy constructor
    GNEShapeHandler(const GNEShapeHandler& s);

    /// @brief invalid assignment operator
    GNEShapeHandler& operator=(const GNEShapeHandler& s);

};


#endif

/****************************************************************************/

