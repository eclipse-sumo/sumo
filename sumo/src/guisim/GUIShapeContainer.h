/****************************************************************************/
/// @file    GUIShapeContainer.h
/// @author  Daniel Krajzewicz
/// @date    08.10.2009
/// @version $Id$
///
// Storage for geometrical objects extended by mutexes
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
#include <utils/foxtools/MFXMutex.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;


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
    GUIShapeContainer(GUINet &net) throw();


    /// @brief Destructor
    virtual ~GUIShapeContainer() throw();



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
     * @param[in] pos The position of the polygon to add
     * @return Whether the polygon could be added (no other with same id exists in the layer)
     * @see ShapeContainer::addPolygon
     */
    virtual bool addPolygon(const std::string &name, int layer, 
        const std::string &type, const RGBColor &c, bool filled, const Position2DVector &shape) throw();



    /** @brief Removes a polygon from the container
     * @param[in] layer The layer the polygon is located in
     * @param[in] id The id of the polygon
     * @return Whether the polygon could be removed
     * @see ShapeContainer::removePolygon
     */
    bool removePolygon(int layer, const std::string &id) throw();


    /** @brief Removes a PoI from the container
     * @param[in] layer The layer the PoI is located in
     * @param[in] id The id of the PoI
     * @return Whether the poi could be removed
     * @see ShapeContainer::removePoI
     */
    bool removePoI(int layer, const std::string &id) throw();



    /** @brief Assigns a new position to the named PoI
     * @param[in] layer The layer the PoI is located in
     * @param[in] id The id of the PoI to move
     * @param[in] pos The PoI's new position
     * @see ShapeContainer::movePoI
     */
    void movePoI(int layer, const std::string &id, const Position2D &pos) throw(); 


    /** @brief Assigns a shape to the named polygon
     * @param[in] layer The layer the polygon is located in
     * @param[in] id The id of the polygon to reshape
     * @param[in] shape The polygon's new shape
     */
    void reshapePolygon(int layer, const std::string &id, const Position2DVector &shape) throw();



protected:
    /** @brief Adds a polygon to the container
     * @param[in] layer The layer the polygon is located in
     * @param[in] p The polygon to add
     * @return Whether the polygon could be added (no other with same name existed before)
     * @see ShapeContainer::add
     */
    bool add(int layer, Polygon2D *p) throw();


    /** @brief Adds a POI to the container
     * @param[in] layer The layer the poi is located in
     * @param[in] p The poi to add
     * @return Whether the poi could be added (no other with same name existed before)
     * @see ShapeContainer::add
     */
    bool add(int layer, PointOfInterest *p) throw();


private:
    /// @brief The mutex for adding/removing operations
    MFXMutex myLock;

    /// @brief The network for interaction with the RTree strcuture
    GUINet &myNet;

};


#endif

/****************************************************************************/

