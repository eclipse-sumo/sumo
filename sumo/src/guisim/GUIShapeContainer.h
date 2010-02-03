/****************************************************************************/
/// @file    GUIShapeContainer.h
/// @author  Daniel Krajzewicz
/// @date    08.10.2009
/// @version $Id$
///
// Storage for geometrical objects extended by mutexes
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
    GUIShapeContainer() throw();


    /// @brief Destructor
    virtual ~GUIShapeContainer() throw();


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
     * @see ShapeContainer::removePOI
     */
    bool removePOI(int layer, const std::string &id) throw();


private:
    /// @brief The mutex for adding/removing operations
    MFXMutex myLock;

};


#endif

/****************************************************************************/

