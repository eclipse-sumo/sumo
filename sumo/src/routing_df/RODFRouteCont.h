/****************************************************************************/
/// @file    RODFRouteCont.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A container for routes
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
#ifndef RODFRouteCont_h
#define RODFRouteCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include <utils/common/UtilExceptions.h>
#include "RODFRouteDesc.h"


// ===========================================================================
// class declarations
// ===========================================================================
class RODFNet;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RODFRouteCont
 * @brief A container for DFROUTER-routes
 *
 * The route id is (re)set as soon as the route is added.
 *
 * As sometimes several routes can be used between two edges and have to be
 *  identified, the number of routes connecting them is stored for each
 *  edge pair "myConnectionOccurences" and the route is named using this
 *  information, @see addRouteDesc.
 *
 * @see RODFRouteDesc
 */
class RODFRouteCont
{
public:
    /// @brief Constructor
    RODFRouteCont() throw();

    /// @brief Destructor
    ~RODFRouteCont() throw();


    /** @brief Adds a route to the container
     *
     * If the same route is already known, its "overallProb" is increased
     *  by the value stored in the given route.
     *
     * An id for the route is generated if it is unset, yet. The id is
     *  <FIRST_EDGE>_to_<LAST_EDGE>_<RUNNING> where <RUNNING> is the number
     *  of routes which connect <FIRST_EDGE> and <LAST_EDGE>.
     *
     * @param[in] desc The route description to add
     */
    void addRouteDesc(RODFRouteDesc &desc) throw();


    /** @brief Removes the given route description from the container
     *
     * All routes are regarded as being same if they pass the same edges.
     *  This is done via the "route_finder".
     *
     * @param[in] desc The route description to remove
     * @return Whether the route was removed (a similar was found)
     * @see RODFRouteCont::route_finder
     */
    bool removeRouteDesc(RODFRouteDesc &desc) throw();


    /** @brief Saves routes
     *
     * @param[in, out] saved The list of ids of routes that shall not be saved (were saved before)
     * @param[in] prependix The prependix for route names
     * @param[out] out The device the routes shall written to
     * @return Whether at least one route was saved
     * @exception IOError not yet implemented
     */
    bool save(std::vector<std::string> &saved,
              const std::string &prependix, OutputDevice& out) throw(IOError);


    /** @brief Returns the container of stored routes
     * @return The stored routes
     */
    std::vector<RODFRouteDesc> &get() throw() {
        return myRoutes;
    }


    /** @brief Sorts routes by their distance (length)
     *
     * Done using by_distance_sorter.
     * @see RODFRouteCont::by_distance_sorter
     */
    void sortByDistance() throw();


    /** @brief Removes "illegal" routes
     *
     * "illegal" routes means edge combinations that shall not be passed.
     *
     * @param[in] illegals List of edge combinations that shall not be passed
     * @todo Not used, yet
     */
    void removeIllegal(const std::vector<std::vector<ROEdge*> > &illegals) throw();

    class RoutesMap
    {
    public:
        RoutesMap();
        ~RoutesMap();
        friend std::ostream &operator<<(std::ostream &os, const RoutesMap &rm);
        void write(std::ostream &os, size_t offset) const;

        std::vector<ROEdge*> common;
        std::map<ROEdge*, RoutesMap*> splitMap;
        std::vector<std::string> lastDetectors;
        ROEdge* lastDetectorEdge;
    };

    RoutesMap *getRouteMap(const RODFNet &net) const;

    void determineEndDetector(const RODFNet &net, RODFRouteCont::RoutesMap *rmap) const;

protected:
    /** @brief A class for sorting route descriptions by their length */
    class by_distance_sorter
    {
    public:
        /// @brief Constructor
        explicit by_distance_sorter() { }

        /// @brief Sorting function; compares RODFRouteDesc::distance2Last
        int operator()(const RODFRouteDesc &p1, const RODFRouteDesc &p2) {
            return p1.distance2Last<p2.distance2Last;
        }
    };


    /** @brief A class for finding a same route (one that passes the same edges) */
    class route_finder
    {
    public:
        /** @brief onstructor
         * @param[in] desc The route description to which a same shall be found
         */
        explicit route_finder(const RODFRouteDesc &desc) : myDesc(desc) { }

        /**  @brief The comparing function; compares passed edges */
        bool operator()(const RODFRouteDesc &desc) {
            return myDesc.edges2Pass==desc.edges2Pass;
        }

    private:
        /// @brief The route description for which a same shall be found
        const RODFRouteDesc &myDesc;

    };

protected:
    /// @brief Stored route descriptions
    std::vector<RODFRouteDesc> myRoutes;

    /// @brief Counts how many routes connecting the key-edges were already stored
    std::map<std::pair<ROEdge*, ROEdge*>, int> myConnectionOccurences;


};


#endif

/****************************************************************************/

