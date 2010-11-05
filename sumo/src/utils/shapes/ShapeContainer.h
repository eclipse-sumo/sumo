/****************************************************************************/
/// @file    ShapeContainer.h
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
// Storage for geometrical objects, sorted by the layers they are in
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



    /** @brief Builds a PoI using the given values and adds it to the according layer
     * @param[in] name The name of the PoI to add
     * @param[in] layer The layer to add the PoI to
     * @param[in] type The type of the PoI to add
     * @param[in] c The color of the PoI to add
     * @param[in] pos The position of the PoI to add
     * @return Whether the PoI could be added (no other with same id exists in the layer)
     * @see ShapeContainer::addPoI
     */
    virtual bool addPoI(const std::string &name, int layer, const std::string &type, 
        const RGBColor &c, const Position2D &pos) throw();


    /** @brief Builds a polygon using the given values and adds it to the according layer
     * @param[in] name The name of the polygon to add
     * @param[in] layer The layer to add the polygon to
     * @param[in] type The type of the polygon to add
     * @param[in] c The color of the polygon to add
     * @param[in] shape The shape of the polygon to add
     * @return Whether the polygon could be added (no other with same id exists in the layer)
     * @see ShapeContainer::addPolygon
     */
    virtual bool addPolygon(const std::string &name, int layer, 
        const std::string &type, const RGBColor &c, bool filled, const Position2DVector &shape) throw();



    /** @brief Removes a polygon from the container
     * @param[in] layer The layer the polygon is located in
     * @param[in] id The id of the polygon
     * @return Whether the polygon could be removed
     */
    virtual bool removePolygon(int layer, const std::string &id) throw();


    /** @brief Removes a PoI from the container
     * @param[in] layer The layer the PoI is located in
     * @param[in] id The id of the PoI
     * @return Whether the poi could be removed
     */
    virtual bool removePoI(int layer, const std::string &id) throw();



    /** @brief Assigns a new position to the named PoI
     * @param[in] layer The layer the PoI is located in
     * @param[in] id The id of the PoI to move
     * @param[in] pos The PoI's new position
     */
    virtual void movePoI(int layer, const std::string &id, const Position2D &pos) throw();


    /** @brief Assigns a shape to the named polygon
     * @param[in] layer The layer the polygon is located in
     * @param[in] id The id of the polygon to reshape
     * @param[in] shape The polygon's new shape
     */
    virtual void reshapePolygon(int layer, const std::string &id, const Position2DVector &shape) throw();



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


protected:
    /** @brief Adds a polygon to the container
     * @param[in] layer The layer the polygon is located in
     * @param[in] p The polygon to add
     * @return Whether the polygon could be added (no other with same name existed before)
     */
    virtual bool add(int layer, Polygon2D *p) throw();


    /** @brief Adds a PoI to the container
     * @param[in] layer The layer the poi is located in
     * @param[in] p The PoI to add
     * @return Whether the PoI could be added (no other with same name existed before)
     */
    virtual bool add(int layer, PointOfInterest *p) throw();


protected:
    /// @brief Polygon layers
    mutable std::map<int, NamedObjectCont<Polygon2D*> > myPolygonLayers;

    /// @brief POI layers
    mutable std::map<int, NamedObjectCont<PointOfInterest*> > myPOILayers;

    /// @brief The minimum and the maximum layers
    mutable int myMinLayer, myMaxLayer;

};


#endif

/****************************************************************************/

