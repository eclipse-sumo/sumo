/****************************************************************************/
/// @file    NLGeomShapeBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Sep 2005
/// @version $Id$
///
// Factory for building geometry objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NLGeomShapeBuilder_h
#define NLGeomShapeBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/RGBColor.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ShapeContainer;
class MSNet;


#define INVALID_POSITION -1000000

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NLGeomShapeBuilder
 * @brief Factory for building geometry objects
 * @todo Check why the container is not given directly
 * @todo find a better solution than using INVALID_POSITION
 */
class NLGeomShapeBuilder {
public:
    /** @brief Constructor
     *
     * Retrieves the net's shape container and stores him in "myShapeContainer"
     *
     * @param[in] net The network to get the shape container from
     * @todo Check why the container is not given directly
     * @todo Check why the net is not const
     */
    NLGeomShapeBuilder(MSNet &net) throw();


    /// @brief Destructor
    virtual ~NLGeomShapeBuilder() throw();


    /** @brief Called when a polygon begins
     *
     * The values are stored in order to allocate the complete polygon after
     *  the shape has been parsed, too.
     *
     * @param[in] name The name (ID) of the polygon
     * @param[in] layer The layer the polygon shall be placed in
     * @param[in] type The abstract type of the polygon
     * @param[in] c The color of the polygon
     * @param[in] fill Whether this polygon shall be filled
     */
    virtual void polygonBegin(const std::string &name, int layer,
                              const std::string &type, const RGBColor &c, bool fill) throw();


    /** @brief Ends the parsing of the polygon allocating it.
     *
     * Builds a Polygon2D. Tries to add it to the container and throws an
     *  InvalidArgument if a polygon with the same id already exists therein.
     *
     * @param[in] shape The shape of the polygon
     * @exception InvalidArgument If a polygon with the same id already exists
     */
    virtual void polygonEnd(const Position2DVector &shape) throw(InvalidArgument);


    /** @brief Adds the described PointOfInterest to the geometry container
     *
     * Determines the position to use using "getPointPosition" throwing an
     *  InvalidArgument if this fails.
     * Builds a PointOfInterest. Tries to add it to the container and throws an
     *  InvalidArgument if a pos with the same id already exists therein.
     *
     * @param[in] name The name (ID) of the poi
     * @param[in] layer The layer the poi shall be placed in
     * @param[in] type The abstract type of the poi
     * @param[in] c The color of the poi
     * @param[in] x The x-position of the poi
     * @param[in] y The y-position of the poi
     * @param[in] lane The id of the lane the poi shall be placed at
     * @param[in] posOnLane Position on the lane the poi shall be placed at
     * @see getPointPosition
     * @exception InvalidArgument If a poi with the same id already exists or the position of the poi is invalid
     */
    virtual void addPoint(const std::string &name, int layer,
                          const std::string &type, const RGBColor &c, SUMOReal x, SUMOReal y,
                          const std::string &lane, SUMOReal posOnLane) throw(InvalidArgument);


protected:
    /** @brief Determines the position of a poi to use
     *
     * If x- and y-positions are not INVALID_POSITION, then this position is returned.
     * Otherwise, the lane is tried to be obtained and the position
     *  resulting from the lane's geometry and the lane position information
     *  is computed. Throws an InvalidArgument if the lane is not known.
     *
     * @param[in] x The x-position of the poi
     * @param[in] y The y-position of the poi
     * @param[in] laneID The id of the lane the poi shall be placed at
     * @param[in] posOnLane Position on the lane the poi shall be placed at
     * @return The position of the poi to use
     * @todo No check whether the position on the lane is valid
     * @todo No friendly_pos is regarded
     * @todo Using defined INVALID_POSITION is not very pretty...
     */
    Position2D getPointPosition(SUMOReal x, SUMOReal y,
                                const std::string &laneID,  SUMOReal posOnLane) const throw(InvalidArgument);

private:
    NLGeomShapeBuilder& operator=(const NLGeomShapeBuilder&); // just to avoid a compiler warning

protected:
    /// @name Temporary stored values of the currently parsed polygon
    /// @{

    /// @brief The current polygon's name
    std::string myCurrentName;

    /// @brief The current polygon's type
    std::string myCurrentType;

    /// @brief The current polygon's color
    RGBColor myCurrentColor;

    /// @brief The layer thepolygon shall be added to
    int myCurrentLayer;

    /// @brief Information whether the polygon shall be filled
    bool myFillPoly;
    /// @}


    /// @brief The shape container
    ShapeContainer &myShapeContainer;

};


#endif

/****************************************************************************/

