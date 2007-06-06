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


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PCPolyContainer
 * @brief A storage for loaded polygons
 */
class PCPolyContainer
{
public:
    /// Constructor
    PCPolyContainer(bool prune, const Boundary &prunningBoundary,
                    const std::vector<std::string> &removeByNames);

    /// Destructor
    ~PCPolyContainer();

    /** adds a polygon */
    bool insert(std::string key, Polygon2D *poly, int layer,
                bool ignorePrunning=false);

    /** adds a point of interest */
    bool insert(std::string key, PointOfInterest *poi, int layer,
                bool ignorePrunning=false);

    /** returns the number of contained polygons */
    size_t getNoPolygons();

    /** returns the number of contained pois */
    size_t getNoPOIs();

    /** deletes all types */
    void clear();

    /// reports how many nodes were loaded
    void report();

    /// Returns the information whether a polygon with the given key is in the container
    bool contains(const std::string &key);

    /// Saves the stored polygons into the given file
    void save(const std::string &file, int layer);

    /// Retuns a unique id for a given name
    int getEnumIDFor(const std::string &key);


public:
    /** a container of types, accessed by the string key */
    typedef std::map<std::string, Polygon2D*> PolyCont;
    /** the container of types**/
    PolyCont myPolyCont;

    /** a container of types, accessed by the string key */
    typedef std::map<std::string, PointOfInterest*> POICont;
    /** the container of types**/
    POICont myPOICont;

    /// An id to int map for proper enumeration
    std::map<std::string, int> myIDEnums;

    /// A map from polygon to layer
    std::map<Polygon2D*, int> myPolyLayerMap;

    /// A map from poi to layer
    std::map<PointOfInterest*, int> myPOILayerMap;


    /// The boundary that described the rectangle within which an object must be in order to be kept
    Boundary myPrunningBoundary;

    /// Information whether the prunning boundary shall be used
    bool myDoPrunne;

    /// List of names of polygons/pois that shall be removed
    std::vector<std::string> myRemoveByNames;

private:
    /** invalid copy constructor */
    PCPolyContainer(const PCPolyContainer &s);

    /** invalid assignment operator */
    PCPolyContainer &operator=(const PCPolyContainer &s);

};


#endif

/****************************************************************************/

