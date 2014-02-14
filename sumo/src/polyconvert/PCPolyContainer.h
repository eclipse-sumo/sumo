/****************************************************************************/
/// @file    PCPolyContainer.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// A storage for loaded polygons and pois
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef PCPolyContainer_h
#define PCPolyContainer_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <vector>
#include <utils/shapes/Polygon.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/geom/Boundary.h>
#include <utils/common/UtilExceptions.h>
using namespace SUMO;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PCPolyContainer
 * @brief A storage for loaded polygons and pois
 */
class PCPolyContainer {
public:
    /** @brief Constructor
     * @param[in] prune Whether added polygons/pois shall be pruned
     * @param[in] pruningBoundary The pruning boundary (only valid if prune==true)
     * @param[in] removeByNames Names of objects that shall not be added
     */
    PCPolyContainer(bool prune, const Boundary& pruningBoundary,
                    const std::vector<std::string>& removeByNames);


    /// @brief Destructor
    ~PCPolyContainer();


    /** @brief Adds a polygon to the storage
     *
     * If pruning if enabled, "ignorePruning" is false and the polygon lies outside
     *  the pruning boundary, or if the polygon's name is within the names of
     *  objects to discard, the polygon is deleted and false is returned.
     *
     * Afterwards it is tested whether a polygon with the same name is already stored.
     *  If so, an error message is printed, the polygon is deleted and false is returned, otherwise true.
     *
     * @param[in] id The id of the polygon to add
     * @param[in] poly The polygon to add
     * @param[in] layer The layer the polygon shall be located within
     * @param[in] ignorePruning Whether the polygon shall be kept, even though it would be pruned
     * @return Whether the polygon could be added
     */
    bool insert(const std::string& id, Polygon* poly, int layer,
                bool ignorePruning = false);


    /** @brief Adds a poi to the storage
     *
     * If pruning if enabled, "ignorePruning" is false and the poi lies outside
     *  the pruning boundary, or if the poi's name is within the names of
     *  objects to discard, the poi is deleted and false is returned.
     *
     * Afterwards it is tested whether a poi with the same name is already stored.
     *  If so, an error message is printed, the poi is deleted and false is returned, otherwise true.
     *
     * @param[in] id The id of the poi to add
     * @param[in] poly The poi to add
     * @param[in] layer The layer the poi shall be located within
     * @param[in] ignorePruning Whether the poi shall be kept, even though it would be pruned
     * @return Whether the poi could be added
     */
    bool insert(const std::string& id, PointOfInterest* poi, int layer,
                bool ignorePruning = false);


    /** @brief Returns the number of stored polygons
     * @return How many polygons were added before
     */
    unsigned int getNoPolygons() {
        return (unsigned int) myPolyCont.size();
    }


    /** @brief Returns the number of stored pois
     * @return How many pois were added before
     */
    unsigned int getNoPOIs() {
        return (unsigned int) myPOICont.size();
    }


    /** @brief Removes all stored objects (polygons and pois)
     *
     * All items are deleted
     */
    void clear();


    /** @brief Reports how many polygons and pois were added */
    void report();


    /** @brief Returns the information whether a polygon with the given key is in the container
     * @param[in] id The id of the polygon to get the information about
     * @return Whether the named polygon was added before
     */
    bool containsPolygon(const std::string& kidey);


    /** @brief Saves the stored polygons into the given file
     * @param[in] file The name of the file to write stored objects' definitions into
     * @exception IOError If the file could not be opened
     */
    void save(const std::string& file);


    /** @brief Retuns a unique id for a given name
     *
     * The unique id is generated by having an internal map of ids to running numbers.
     * The first call to this method will return 0, all subsequent with the same
     *  key will return numbers increased by one at each call.
     * @param[in] key The key to get a running number for
     * @return Unique id (running number of calls that used this key)
     */
    int getEnumIDFor(const std::string& key);


public:
    /** @brief Definition of a container of polygons, accessed by the string key */
    typedef std::map<std::string, Polygon*> PolyCont;
    /** @brief The polygon container, accessed by the polygons' ids */
    PolyCont myPolyCont;

    /** @brief Definition of a container of pois, accessed by the string key */
    typedef std::map<std::string, PointOfInterest*> POICont;
    /** @brief The poi container, accessed by the pois' ids */
    POICont myPOICont;

    /// @brief An id to int map for proper enumeration
    std::map<std::string, int> myIDEnums;

    /// @brief A map from polygons to the layers they are located in
    std::map<Polygon*, int> myPolyLayerMap;

    /// @brief A map from pois to the layers they are located in
    std::map<PointOfInterest*, int> myPOILayerMap;


    /// @brief The boundary that described the rectangle within which an object must be in order to be kept
    Boundary myPruningBoundary;

    /// @brief Information whether the pruning boundary shall be used
    bool myDoPrune;

    /// @brief List of names of polygons/pois that shall be removed
    std::vector<std::string> myRemoveByNames;


private:
    /// @brief Invalidated copy constructor
    PCPolyContainer(const PCPolyContainer& s);

    /// @brief Invalidated assignment operator
    PCPolyContainer& operator=(const PCPolyContainer& s);


};


#endif

/****************************************************************************/

