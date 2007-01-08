#ifndef PCPolyContainer_h
#define PCPolyContainer_h
/***************************************************************************
                          PCPolyContainer.h
    A storage for loaded polygons and pois
                             -------------------
    project              : SUMO
    subproject           : PolyConvert
    begin                : Mon, 05 Dec 2005
    copyright            : (C) 2005 by DLR http://ivf.dlr.de/
    author               : Danilo Boyom
    email                : Danilot.Tete-Boyom@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.4  2007/01/08 14:43:58  dkrajzew
// code beautifying; prliminary import for Visum points added
//
// Revision 1.3  2006/11/28 14:51:48  dkrajzew
// possibility to prune the plygons to import on a bounding box added
//
// Revision 1.2  2006/11/02 12:19:50  dkrajzew
// added parsing of Elmar's pointcollections
//
// Revision 1.1  2006/08/01 07:52:46  dkrajzew
// polyconvert added
//
// Revision 1.1  2006/03/27 07:22:27  dksumo
// initial checkin
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <map>
#include <utils/shapes/Polygon2D.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/geom/Boundary.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class PCPolyContainer
 * @brief A storage for loaded polygons
 */
class PCPolyContainer {
public:
    /// Constructor
    PCPolyContainer(bool prune, const Boundary &prunningBoundary);

    /// Destructor
    ~PCPolyContainer();

    /** adds a polygon */
    bool insert(std::string key, Polygon2D *poly, int layer);

    /** adds a point of interest */
    bool insert(std::string key, PointOfInterest *poi, int layer);

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

private:
    /** invalid copy constructor */
    PCPolyContainer(const PCPolyContainer &s);

    /** invalid assignment operator */
    PCPolyContainer &operator=(const PCPolyContainer &s);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
