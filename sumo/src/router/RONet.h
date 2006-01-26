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
// Revision 1.18  2006/01/26 08:44:14  dkrajzew
// adapted the new router API
//
// Revision 1.17  2006/01/24 13:43:53  dkrajzew
// added vehicle classes to the routing modules
//
// Revision 1.16  2006/01/09 12:00:58  dkrajzew
// debugging vehicle color usage
//
// Revision 1.15  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.14  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.13  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.12  2005/05/04 08:48:35  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; handling of vehicles debugged
//
// Revision 1.11  2004/12/16 12:26:52  dkrajzew
// debugging
//
// Revision 1.10  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.9  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived
//  for an online-routing
//
// Revision 1.8  2004/04/02 11:26:26  dkrajzew
// moving the vehicle forward if it shall start at a too short edge added;
//  output of the number of loaded, build, and discarded vehicles added
//
// Revision 1.7  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work; documentation added
//
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
class ROVehicle;
class OptionsCont;
class ROAbstractEdgeBuilder;


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
    void addNode(const std::string &name, RONode *node);

    /// Returns the information whether the named vehicle is already in the network
    bool isKnownVehicleID(const std::string &id) const;

    /// Returns the named route definition
    virtual RORouteDef *getRouteDef(const std::string &name) const;

    /// Adds a read route definition to the network
    virtual void addRouteDef(RORouteDef *def);


    /// Adds a complete, read vehicle type to the network
    virtual void addVehicleType(ROVehicleType *type);

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
        ROAbstractRouter &router, SUMOTime time);

    /// Returns the information whether further vehicles are stored
    virtual bool furtherStored();

    /// Returns a random edge which may be used as a starting point
    ROEdge *getRandomSource();

    /// Returns a random edge which may be used as the end of a route
    ROEdge *getRandomDestination();

    /// Returns the number of edges thenetwork contains
    unsigned int getEdgeNo() const;



	static RONet* getNetInstance();

	static void setNetInstance(RONet *net);

    ROEdgeCont *getMyEdgeCont();

    /** @brief Removes the route from the net when no further usage is needed */
    virtual void removeRouteSecure(const RORouteDef * const route); // !!! was protected before

protected:
    /** Saves the given route together with her alternatives */
    const RORouteDef * const computeRoute(OptionsCont &options,
		ROAbstractRouter &router, ROVehicle *veh);

    /// Initialises the lists of source and destination edges
    void checkSourceAndDestinations();

    /** @brief Builds a single output file checking it
        If something went wrong on building, a ProcessError is thrown */
    std::ofstream *buildOutput(const std::string &name);


protected:
    /// a Unique Instance of a RONet#
	static RONet* myInstance;

    /// Container for known vehicle ids
    typedef std::vector<std::string> VehIDCont; // should be something else

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

    /// The file to write the computed routes into
    std::ofstream *myRoutesOutput;

    /// The file to write the computed route alternatives into
    std::ofstream *myRouteAlternativesOutput;

    /// number of read routes
    size_t myReadRouteNo;

    /// number of discarded routes
    size_t myDiscardedRouteNo;

    /// number of written routes
    size_t myWrittenRouteNo;

private:
    /// we made the copy constructor invalid
    RONet(const RONet &src);

    /// we made the assignment operator invalid
    RONet &operator=(const RONet &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

