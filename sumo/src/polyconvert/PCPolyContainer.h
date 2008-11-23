/****************************************************************************/
/// @file    PCPolyContainer.h
/// @author  Daniel krajzewicz
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// A storage for loaded polygons and pois
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/shapes/Polygon2D.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/geom/Boundary.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PCPolyContainer
 * @brief A storage for loaded polygons and pois
 */
class PCPolyContainer
{
public:
    /** @brief Constructor
     * @param[in] prune Whether added polygons/pois shall be prunned
     * @param[in] prunningBoundary The pruning boundary (onyl valid if prune==true)
     * @param[in] removeByNames Names of objects that shall not be added
     */
    PCPolyContainer(bool prune, const Boundary &prunningBoundary,
                    const std::vector<std::string> &removeByNames) throw();


    /// @brief Destructor
    ~PCPolyContainer() throw();


    /** @brief Adds a polygon to the storage
     *
     * If prunning if enabled, "ignorePrunning" is false and the polygon lies outside 
     *  the prunning boundary, or if the polygon's name is within the names of 
     *  objects to discard, the polygon is deleted and true is returned.
     *
     * Otherwise, it is tested whether a polygon with the same name is already stored.
     *  If so, false is returned (the polygon is not deleted), otherwise true.
     *
     * @param[in] id The id of the polygon to add
     * @param[in] poly The polygon to add
     * @param[in] layer The layer the polygon shall be located within
     * @param[in] ignorePrunning Whether the polygon shall be kept, even though it would be prunned
     * @return Whether the polygon could been added (no one with the same id was added before)
     */
    bool insert(const std::string &id, Polygon2D *poly, int layer,
                bool ignorePrunning=false) throw();


    /** @brief Adds a poi to the storage
     *
     * If prunning if enabled, "ignorePrunning" is false and the poi lies outside 
     *  the prunning boundary, or if the poi's name is within the names of 
     *  objects to discard, the poi is deleted and true is returned.
     *
     * Otherwise, it is tested whether a poi with the same name is already stored.
     *  If so, false is returned (the poi is not deleted), otherwise true.
     *
     * @param[in] id The id of the poi to add
     * @param[in] poly The poi to add
     * @param[in] layer The layer the poi shall be located within
     * @param[in] ignorePrunning Whether the poi shall be kept, even though it would be prunned
     * @return Whether the poi could been added (no one with the same id was added before)
     */
    bool insert(const std::string &id, PointOfInterest *poi, int layer,
                bool ignorePrunning=false) throw();


    /** @brief Returns the number of stored polygons
     * @return How many polygons were added before
     */
    unsigned int getNoPolygons() throw() {
        return (unsigned int) myPolyCont.size();
    }


    /** @brief Returns the number of stored pois
     * @return How many pois were added before
     */
    unsigned int getNoPOIs() throw() {
        return (unsigned int) myPOICont.size();
    }


    /** @brief Removes all stored objects (polygons and pois)
     *
     * All items are deleted
     */
    void clear() throw();


    /** @brief Reports how many polygons and pois were added */
    void report() throw();


    /** @brief Returns the information whether a polygon with the given key is in the container
     * @param[in] id The id of the polygon to get the information about
     * @return Whether the named polygon was added before
     */
    bool containsPolygon(const std::string &kidey) throw();


    /** @brief Saves the stored polygons into the given file
     * @param[in] file The name of the file to write stored objects' definitions into
     * @xception IOError If the file could not been opened
     */
    void save(const std::string &file) throw(IOError);


    /** @brief Retuns a unique id for a given name
     *
     * The unique id is generated by having an internal map of ids to running numbers.
     * The first call to this method will return 0, all subsequent with the same
     *  key will return numbers increased by one at each call.
     * @param[in] key The key to get a running number for
     * @return Unique id (running number of calls that used this key)
     */
    int getEnumIDFor(const std::string &key) throw();


public:
    /** @brief Definition of a container of polygons, accessed by the string key */
    typedef std::map<std::string, Polygon2D*> PolyCont;
    /** @brief The polygon container, accessed by the polygons' ids */
    PolyCont myPolyCont;

    /** @brief Definition of a container of pois, accessed by the string key */
    typedef std::map<std::string, PointOfInterest*> POICont;
    /** @brief The poi container, accessed by the pois' ids */
    POICont myPOICont;

    /// @brief An id to int map for proper enumeration
    std::map<std::string, int> myIDEnums;

    /// @brief A map from polygons to the layers they are located in
    std::map<Polygon2D*, int> myPolyLayerMap;

    /// @brief A map from pois to the layers they are located in
    std::map<PointOfInterest*, int> myPOILayerMap;


    /// @brief The boundary that described the rectangle within which an object must be in order to be kept
    Boundary myPrunningBoundary;

    /// @brief Information whether the prunning boundary shall be used
    bool myDoPrunne;

    /// @brief List of names of polygons/pois that shall be removed
    std::vector<std::string> myRemoveByNames;


private:
    /** @brief invalid copy constructor */
    PCPolyContainer(const PCPolyContainer &s);

    /** @brief invalid assignment operator */
    PCPolyContainer &operator=(const PCPolyContainer &s);


};


#endif

/****************************************************************************/

