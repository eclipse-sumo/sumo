/****************************************************************************/
/// @file    ShapeContainer.h
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
// Storage for geometrical objects, sorted by the layers they are in
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
#ifndef ShapeContainer_h
#define ShapeContainer_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/NamedObjectCont.h>
#include <utils/shapes/Polygon2D.h>
#include "PointOfInterest.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ShapeContainer
 * @brief Storage for geometrical objects, sorted by the layers they are in
 */
class ShapeContainer {
public:
    /// @brief Constructor
    ShapeContainer() throw();


    /// @brief Destructor
    virtual ~ShapeContainer() throw();


    /** @brief Adds a polygon to the container
     * @param[in] layer The layer the polygon is located in
     * @param[in] p The polygon to add
     * @return Whether the polygon could be added (no other with same name existed before)
     */
    bool add(int layer, Polygon2D *p) throw();


    /** @brief Adds a POI to the container
     * @param[in] layer The layer the poi is located in
     * @param[in] p The poi to add
     * @return Whether the poi could be added (no other with same name existed before)
     */
    bool add(int layer, PointOfInterest *p) throw();


    /** @brief Returns the polygons located at the given layer
     * @param[in] layer The layer of polygons to return
     * @return The polygons at this layer
     * @see NamedObjectCont
     */
    const NamedObjectCont<Polygon2D*> &getPolygonCont(int layer) const throw();


    /** @brief Returns the polygons located at the given layer
     * @param[in] layer The layer of polygons to return
     * @return The polygons at this layer
     * @see NamedObjectCont
     */
    const NamedObjectCont<PointOfInterest*> &getPOICont(int layer) const throw();


    /** @brief Returns the lowest layer 
     * @return The lowest layer
     */
    int getMinLayer() const throw() {
        return myMinLayer;
    }


    /** @brief Returns the highest layer 
     * @return The highest layer
     */
    int getMaxLayer() const throw() {
        return myMaxLayer;
    }


private:
    /// @brief Polygon layers
    mutable std::map<int, NamedObjectCont<Polygon2D*> > myPolygonLayers;

    /// @brief POI layers
    mutable std::map<int, NamedObjectCont<PointOfInterest*> > myPOILayers;

    /// @brief The minimum and the maximum layers
    mutable int myMinLayer, myMaxLayer;

};


#endif

/****************************************************************************/

