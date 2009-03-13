/****************************************************************************/
/// @file    GUIGeomShapeBuilder.h
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
// Factory for building geometry objects (gui version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIGeomShapeBuilder_h
#define GUIGeomShapeBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <netload/NLGeomShapeBuilder.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ShapeContainer;
class GUIGlObjectStorage;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIGeomShapeBuilder
 * @brief Factory for building geometry objects (gui version)
 *
 * The main distinction to NLGeomShapeBuilder is that objects in gui-mode
 *  need an additional gl-id in order to make them clickable. This is retrieved
 *  from the stored GUIGlObjectStorage.
 *
 * @see NLGeomShapeBuilder
 */
class GUIGeomShapeBuilder : public NLGeomShapeBuilder {
public:
    /** @brief Constructor
     *
     * @param[in] net The network to get the shape container from
     * @param[in] idStorage The storage to obtain new gl-ids from
     */
    GUIGeomShapeBuilder(MSNet &net, GUIGlObjectStorage &idStorage) throw();


    /// @brief Destructor
    ~GUIGeomShapeBuilder() throw();


    /** @brief Ends the parsing of the polygon allocating it.
     *
     * Builds a GUIPolygon2D. Tries to add it to the container and throws an
     *  InvalidArgument if a polygon with the same id already exists therein.
     *
     * @param[in] shape The shape of the polygon
     * @exception InvalidArgument If a polygon with the same id already exists
     */
    void polygonEnd(const Position2DVector &shape) throw(InvalidArgument);


    /** @brief Adds the described PointOfInterest to the geometry container
     *
     * Determines the position to use using "getPointPosition" throwing an
     *  InvalidArgument if this fails.
     * Builds a GUIPointOfInterest. Tries to add it to the container and throws an
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
    void addPoint(const std::string &name, int layer, const std::string &type,
                  const RGBColor &c, SUMOReal x, SUMOReal y,
                  const std::string &lane, SUMOReal posOnLane) throw(InvalidArgument);


protected:
    /// @brief The id storage to retrieve gl-ids from
    GUIGlObjectStorage &myIdStorage;


};


#endif

/****************************************************************************/

