#ifndef RONet_h
#define RONet_h
//---------------------------------------------------------------------------//
//                        RONet.h -
//  The router's network representation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.6  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.5  2003/03/17 14:25:28  dkrajzew
// windows eol removed
//
// Revision 1.4  2003/03/03 15:08:21  dkrajzew
// debugging
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

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


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;
class RONode;
class RORouteDef;
class RORouter;
class ROVehicle;
class OptionsCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RONet
 * The network holding all currently needed routes and vehicles and also
 * edges and nodes.
 */
class RONet {
public:
    /// Constructor
    RONet(bool multireferencedRoutes);

    /// Destructor
    ~RONet();

    /** @brief Performs some initialisations after the net (and the weights) have been loaded
        The weight-timelines must be computed for the edges */
    void postloadInit();

    /// Adds a read edge to the network
    void addEdge(const std::string &name, ROEdge *edge);

    /** @brief Retrieves an edge from the network
        This is not very pretty, but necessary, though, as the route r
        runs over instances, not over ids */
    ROEdge *getEdge(const std::string &name) const;

    /// Adds a read node to the network
    void addNode(const std::string &name, RONode *node);

    /// Returns the information whether the named vehicle is already in the network
    bool isKnownVehicleID(const std::string &id) const;

    /// Returns the named route definition
    RORouteDef *getRouteDef(const std::string &name) const;

    /// Adds a read route definition to the network
    void addRouteDef(RORouteDef *def);


    /// Adds a complete, read vehicle type to the network
    void addVehicleType(ROVehicleType *type);

    /// Retrieves the default vehicle type
    ROVehicleType *getDefaultVehicleType() const;

    /** @brief Retrieves the named vehicle type
        Returns 0 if the type is not known */
    ROVehicleType *getVehicleType(const std::string &name) const;

    /** @brief Retrieves the named vehicle type
        Returns the default type if the type is not known */
    ROVehicleType *getVehicleTypeSecure(const std::string &id);

    /** @brief Adds a vehicle id
        As some route formats do not contain vehicle description, the ids
        are saved to check for double occurences, but are not assigned to
        vehicles before the routes are not build */
    void addVehicleID(const std::string &id);

    /// Adds a completely build vehicle to the net
    void addVehicle(const std::string &id, ROVehicle *veh);

    /// Adds a known route snipplet to the network (!!! somekind of deprecated)
    bool addRouteSnipplet(const ROEdgeVector &item);

    /// Retrieves the optimal route snipplet between both edges (!!! somekind of deprecated)
    const ROEdgeVector &getRouteSnipplet(ROEdge *from, ROEdge *to) const;

    /// Returns the information whether the way between the given edges has already been computed (!!! somekind of deprecated)
    bool knowsRouteSnipplet(ROEdge *from, ROEdge *to) const;

    /** @brief Computes routes described by their definitions and saves them */
    void saveAndRemoveRoutesUntil(OptionsCont &options,
        std::ofstream &res, std::ofstream &altres, long time);

    /// Returns the information whether further vehicles are stored
    bool furtherStored();

    /// Returns a random edge which may be used as a starting point
    ROEdge *getRandomSource();

    /// Returns a random edge which may be used as the end of a route
    ROEdge *getRandomDestination();

private:
    /** @brief Saves the given vehicle type
        If the type is not known, a warning is printed including the name of
        the car */
    void saveType(std::ostream &os, ROVehicleType *type,
        const std::string &vehID);

    /** Saves the given route together with her alternatives */
    bool saveRoute(OptionsCont &options, RORouter &router,
        std::ostream &res, std::ostream &altres, ROVehicle *veh);

    /** @brief Removes the route from the net when no further usage is needed */
    void removeRouteSecure(RORouteDef *route);

    /// Initialises the lists of source and destination edges
    void checkSourceAndDestinations();

private:
    /// Container for known vehicle ids
    typedef std::set<std::string> VehIDCont;

    /// Known vehicle ids
    VehIDCont _vehIDs;

    /// Known nodes
    RONodeCont _nodes;

    /// Known edges
    ROEdgeCont _edges;

    /// Known vehicle types
    ROVehTypeCont _vehicleTypes;

    /// Known routes
    RORouteDefCont _routes;

    /// Known vehicles
    ROVehicleCont _vehicles;

    /// Known route sipplets
    RORouteSnippletCont _snipplets;

    /// List of source edges
    std::vector<ROEdge*> mySourceEdges;

    /// List of destination edges
    std::vector<ROEdge*> myDestinationEdges;

private:
    /// we made the copy constructor invalid
    RONet(const RONet &src);

    /// we made the assignment operator invalid
    RONet &operator=(const RONet &src);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "RONet.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

