/****************************************************************************/
/// @file    MSRoute.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A vehicle route
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
#ifndef MSRoute_h
#define MSRoute_h


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
#include <utils/common/Named.h>
#include <utils/common/RandomDistributor.h>
#include <utils/common/RGBColor.h>
#include <utils/common/SUMOVehicleParameter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class BinaryInputDevice;
class OutputDevice;


typedef std::vector<const MSEdge*> MSEdgeVector;
typedef MSEdgeVector::const_iterator MSRouteIterator;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSRoute
 */
class MSRoute : public Named {
public:
    /// Constructor
    MSRoute(const std::string &id, const MSEdgeVector &edges,
            unsigned int references, const RGBColor &c,
            const std::vector<SUMOVehicleParameter::Stop> &stops) throw();

    /// Destructor
    virtual ~MSRoute() throw();

    /// Returns the begin of the list of edges to pass
    MSRouteIterator begin() const;

    /// Returns the end of the list of edges to pass
    MSRouteIterator end() const;

    /// Returns the number of edges to pass
    unsigned size() const;

    /// returns the destination edge
    const MSEdge *getLastEdge() const;

    /** @brief increments the reference counter for the route */
    void addReference() const;

    /** @brief deletes the route if there are no further references to it*/
    void release() const;

    /// output the edge ids up to but not including the id of the given edge
    unsigned int writeEdgeIDs(OutputDevice &os, const unsigned int from, const MSEdge *upTo=0) const;

    bool contains(const MSEdge * const edge) const throw() {
        return find(edge)!=myEdges.end();
    }

    bool containsAnyOf(const std::vector<MSEdge*> &edgelist) const;

    const MSEdge *operator[](unsigned index) const;

#ifdef HAVE_MESOSIM
    /// @name State I/O (mesosim only)
    /// @{

    /** @brief Saves all known routes into the given stream
     *
     * @param[in] os The stream to write the routes into (binary)
     */
    static void dict_saveState(std::ostream &os) throw();


    /** @brief Loads routes from the state.
     *
     * @param[in] bis The input to read the routes from (binary)
     */
    static void dict_loadState(BinaryInputDevice &bis) throw();
    /// @}
#endif

    const MSEdgeVector &getEdges() const {
        return myEdges;
    }

    SUMOReal getLength() const;

    /** @brief Compute the distance between 2 given edges on this route, including the length of internal lanes.
     *
     * @param[in] fromPos  position on the first edge, at wich the computed distance begins
     * @param[in] toPos	   position on the last edge, at which the coumputed distance endsance
     * @param[in] fromEdge edge at wich computation begins
     * @param[in] toEdge   edge at which distance computation shall stop
     * @return             distance between the position fromPos on fromEdge and toPos on toEdge
     */
    SUMOReal getDistanceBetween(SUMOReal fromPos, SUMOReal toPos, const MSEdge* fromEdge, const MSEdge* toEdge) const;

    /// Returns the color
    const RGBColor &getColor() const;

    /// Returns the stops
    const std::vector<SUMOVehicleParameter::Stop> &getStops() const;

public:
    /** @brief Adds a route to the dictionary.
     *
     *  Returns true if the route could be added,
     *  false if a route (distribution) with the same name already exists.
     *
     * @param[in] id    the id for the new route
     * @param[in] route pointer to the route object
     * @return          whether adding was successful
     */
    static bool dictionary(const std::string &id, const MSRoute *route);

    /** @brief Adds a route distribution to the dictionary.
     *
     *  Returns true if the distribution could be added,
     *  false if a route (distribution) with the same name already exists.
     *
     * @param[in] id    the id for the new route distribution
     * @param[in] route pointer to the distribution object
     * @return          whether adding was successful
     */
    static bool dictionary(const std::string &id, RandomDistributor<const MSRoute*> *routeDist);

    /** @brief Returns the named route or a sample from the named distribution.
     *
     *  Returns 0 if no route and no distribution with the given name exists
     *  or if the distribution exists and is empty.
     *
     * @param[in] id    the id of the route or the distribution
     * @return          the route (sample)
     */
    static const MSRoute *dictionary(const std::string &id);

    /** @brief Returns the named route distribution.
     *
     *  Returns 0 if no route distribution with the given name exists.
     *
     * @param[in] id    the id of the route distribution
     * @return          the route distribution
     */
    static RandomDistributor<const MSRoute*> *distDictionary(const std::string &id);

    /// Clears the dictionary (delete all known routes, too)
    static void clear();

    static void insertIDs(std::vector<std::string> &into);

    MSRouteIterator find(const MSEdge *e) const;
    MSRouteIterator find(const MSEdge *e, const MSRouteIterator &startingAt) const;


private:
    /// The list of edges to pass
    MSEdgeVector myEdges;

    /// Information by how many vehicles the route is used
    mutable unsigned int myReferenceCounter;

    /// The color
    RGBColor myColor;

    /// @brief List of the stops on the parsed route
    std::vector<SUMOVehicleParameter::Stop> myStops;

private:
    /// Definition of the dictionary container
    typedef std::map<std::string, const MSRoute*> RouteDict;

    /// The dictionary container
    static RouteDict myDict;

    /// Definition of the dictionary container
    typedef std::map<std::string, RandomDistributor<const MSRoute*>*> RouteDistDict;

    /// The dictionary container
    static RouteDistDict myDistDict;

};


#endif

/****************************************************************************/

