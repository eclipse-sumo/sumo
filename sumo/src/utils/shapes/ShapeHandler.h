
/****************************************************************************/
/// @file    ShapeHandler.h
/// @author  Jakob Erdmann
/// @date    Feb 2015
/// @version $Id$
///
// The XML-Handler for network loading
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
#ifndef ShapeHandler_h
#define ShapeHandler_h


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


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ShapeHandler
 * @brief The XML-Handler for network loading
 *
 * The SAX2-handler responsible for parsing networks and routes to load.
 * This is an extension of the MSRouteHandler as routes and vehicles may also
 *  be loaded from network descriptions.
 */
class ShapeHandler : public SUMOSAXHandler {
public:

public:
    /** @brief Constructor
     *
     * @param[in] file Name of the parsed file
     * @param[in, out] net The network to fill
     * @param[in] detBuilder The detector builder to use
     * @param[in] triggerBuilder The trigger builder to use
     * @param[in] edgeBuilder The builder of edges to use
     * @param[in] junctionBuilder The builder of junctions to use
     */
    ShapeHandler(const std::string& file, ShapeContainer& sc);

    /// @brief Destructor
    virtual ~ShapeHandler();

    /// @brief loads all of the given files
    static bool loadFiles(const std::vector<std::string>& files, ShapeHandler& sh);


protected:
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


    /// @brief get position for a given laneID
    virtual Position getLanePos(const std::string& poiID, const std::string& laneID, SUMOReal lanePos) = 0;

protected:
    void setDefaults(const std::string& prefix, const RGBColor& color, const SUMOReal layer, const bool fill = false);

    /// adds a POI
    void addPOI(const SUMOSAXAttributes& attrs, const bool ignorePruning, const bool useProcessing);

    /// adds a polygon
    void addPoly(const SUMOSAXAttributes& attrs, const bool ignorePruning, const bool useProcessing);

protected:
    ShapeContainer& myShapeContainer;

private:
    /// @brief The prefix to use
    std::string myPrefix;
    /// @brief The default color to use
    RGBColor myDefaultColor;
    /// @brief The default layer to use
    SUMOReal myDefaultLayer;
    /// @brief Information whether polygons should be filled
    bool myDefaultFill;

private:
    /** invalid copy constructor */
    ShapeHandler(const ShapeHandler& s);

    /** invalid assignment operator */
    ShapeHandler& operator=(const ShapeHandler& s);

};


#endif

/****************************************************************************/

