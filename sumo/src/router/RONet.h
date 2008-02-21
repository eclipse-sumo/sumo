/****************************************************************************/
/// @file    RONet.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The router's network representation
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
#ifndef RONet_h
#define RONet_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <set>
#include <fstream>
#include <deque>
#include <vector>
#include "RONodeCont.h"
#include "ROEdgeCont.h"
#include "ROVehTypeCont.h"
#include "RORouteDefCont.h"
#include "ROVehicleCont.h"
#include "ROVehicle.h"
#include "RORouteSnippletCont.h"
#include "ROEdgeVector.h"


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class RONode;
class RORouteDef;
class ROVehicle;
class OptionsCont;
class ROAbstractEdgeBuilder;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RONet
 * @brief The router's network representation.
 */
class RONet
{
public:
    /// Constructor
    RONet(bool multireferencedRoutes);

    /// Destructor
    virtual ~RONet();

    /** @brief opens the output for computed routes
        if the second parameter is true, a second file for route alternatives
        will be opened.
    If one of the file outputs could not be build, a ProcessError exception is thrown */
    void openOutput(const std::string &filename, bool useAlternatives);

    /** closes the file output for computed routes */
    void closeOutput();


    /// Adds a read edge to the network
    virtual void addEdge(ROEdge *edge);

    /** @brief Retrieves an edge from the network
        This is not very pretty, but necessary, though, as the route r
        runs over instances, not over ids */
    ROEdge *getEdge(const std::string &name) const;

    /// Adds a read node to the network
    void addNode(RONode *node);

    /// Retrieves a node from the network
    RONode *getNode(const std::string &id) const;

    /// Returns the information whether the named vehicle is already in the network
    bool isKnownVehicleID(const std::string &id) const;

    /// Returns the named route definition
    virtual RORouteDef *getRouteDef(const std::string &name) const;

    /// Adds a read route definition to the network
    virtual void addRouteDef(RORouteDef *def);


    /// Adds a complete, read vehicle type to the network
    virtual void addVehicleType(ROVehicleType *type);

    /** @brief Retrieves the named vehicle type
        Returns the default type if the type is not known */
    ROVehicleType *getVehicleTypeSecure(const std::string &id);

    /** @brief Adds a vehicle id
        As some route formats do not contain vehicle description, the ids
        are saved to check for SUMOReal occurences, but are not assigned to
        vehicles before the routes are not build */
    void addVehicleID(const std::string &id);

    /// Adds a completely build vehicle to the net
    virtual void addVehicle(const std::string &id, ROVehicle *veh);

    /// Adds a known route snipplet to the network (!!! somekind of deprecated)
    bool addRouteSnipplet(const ROEdgeVector &item);

    /// Retrieves the optimal route snipplet between both edges (!!! somekind of deprecated)
    const ROEdgeVector &getRouteSnipplet(ROEdge *from, ROEdge *to) const;

    /// Returns the information whether the way between the given edges has already been computed (!!! somekind of deprecated)
    bool knowsRouteSnipplet(ROEdge *from, ROEdge *to) const;

    /** @brief Computes routes described by their definitions and saves them */
    void saveAndRemoveRoutesUntil(OptionsCont &options,
                                  SUMOAbstractRouter<ROEdge,ROVehicle> &router, SUMOTime time);

    /// Returns the information whether further vehicles are stored
    virtual bool furtherStored();

    /// Returns a random edge which may be used as a starting point
    ROEdge *getRandomSource();

    /// Returns a random edge which may be used as the end of a route
    ROEdge *getRandomDestination();

    /// Returns the number of edges thenetwork contains
    unsigned int getEdgeNo() const;

    const std::map<std::string, ROEdge*> &getEdgeMap() const;

    /** @brief Removes the route from the net when no further usage is needed */
    virtual void removeRouteSecure(const RORouteDef * const route); // !!! was protected before

    bool hasRestrictions() const;

    void setRestrictionFound();

protected:
    /** Saves the given route together with her alternatives */
    const RORouteDef * const computeRoute(OptionsCont &options,
                                          SUMOAbstractRouter<ROEdge,ROVehicle> &router, const ROVehicle * const veh);

    /// Initialises the lists of source and destination edges
    void checkSourceAndDestinations();

protected:
    /// Container for known vehicle ids
    typedef std::vector<std::string> VehIDCont; // should be something else

    /// Known vehicle ids
    VehIDCont myVehIDs;

    /// Known nodes
    RONodeCont myNodes;

    /// Known edges
    ROEdgeCont myEdges;

    /// Known vehicle types
    ROVehTypeCont myVehicleTypes;

    /// Known routes
    RORouteDefCont myRoutes;

    /// Known vehicles
    ROVehicleCont myVehicles;

    /// Known route sipplets
    RORouteSnippletCont mySnipplets;

    /// List of source edges
    std::vector<ROEdge*> mySourceEdges;

    /// List of destination edges
    std::vector<ROEdge*> myDestinationEdges;

    /// The file to write the computed routes into
    OutputDevice *myRoutesOutput;

    /// The file to write the computed route alternatives into
    OutputDevice *myRouteAlternativesOutput;

    /// number of read routes
    size_t myReadRouteNo;

    /// number of discarded routes
    size_t myDiscardedRouteNo;

    /// number of written routes
    size_t myWrittenRouteNo;

    bool myHaveRestrictions;

private:
    /// we made the copy constructor invalid
    RONet(const RONet &src);

    /// we made the assignment operator invalid
    RONet &operator=(const RONet &src);

};


#endif

/****************************************************************************/

