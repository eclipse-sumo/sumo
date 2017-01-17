/****************************************************************************/
/// @file    RONet.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Yun-Pang Floetteroed
/// @date    Sept 2002
/// @version $Id$
///
// The router's network representation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <queue>
#include <vector>
#include <utils/common/MsgHandler.h>
#include <utils/common/NamedObjectCont.h>
#include <utils/common/RandomDistributor.h>
#include <utils/vehicle/PedestrianRouter.h>
#include <utils/vehicle/SUMOAbstractRouter.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include "ROLane.h"
#include "RORoutable.h"
#include "RORouteDef.h"

#ifdef HAVE_FOX
#include <utils/foxtools/FXWorkerThread.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class ROLane;
class RONode;
class ROPerson;
class RORoutable;
class ROVehicle;
class OptionsCont;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RONet
 * @brief The router's network representation.
 *
 * A router network is responsible for watching loaded edges, nodes,!!!
 */
class RONet {
public:

    typedef std::map<const SUMOTime, std::deque<RORoutable*> > RoutablesMap;

    /// @brief Constructor
    RONet();


    /** @brief Returns the pointer to the unique instance of RONet (singleton).
     * @return Pointer to the unique RONet-instance
     */
    static RONet* getInstance();


    /// @brief Destructor
    virtual ~RONet();


    /** @brief Adds a restriction for an edge type
     * @param[in] id The id of the type
     * @param[in] svc The vehicle class the restriction refers to
     * @param[in] speed The restricted speed
     */
    void addRestriction(const std::string& id, const SUMOVehicleClass svc, const SUMOReal speed);


    /** @brief Returns the restrictions for an edge type
     * If no restrictions are present, 0 is returned.
     * @param[in] id The id of the type
     * @return The mapping of vehicle classes to maximum speeds
     */
    const std::map<SUMOVehicleClass, SUMOReal>* getRestrictions(const std::string& id) const;


    /// @name Insertion and retrieval of graph parts
    //@{

    /* @brief Adds a read edge to the network
     *
     * If the edge is already known (another one with the same id exists),
     *  an error is generated and given to msg-error-handler. The edge
     *  is deleted in this case and false is returned.
     *
     * @param[in] edge The edge to add
     * @return Whether the edge was added (if not, it was deleted, too)
     */
    virtual bool addEdge(ROEdge* edge);


    /* @brief Adds a district and connecting edges to the network
     *
     * If the district is already known (another one with the same id exists),
     *  an error is generated and given to msg-error-handler. The edges
     *  are deleted in this case and false is returned.
     *
     * @param[in] id The district to add
     * @return Whether the district was added
     */
    bool addDistrict(const std::string id, ROEdge* source, ROEdge* sink);


    /* @brief Adds a district and connecting edges to the network
     *
     * If the district is already known (another one with the same id exists),
     *  an error is generated and given to msg-error-handler. The edges
     *  are deleted in this case and false is returned.
     *
     * @param[in] id The district to add
     * @return Whether the district was added
     */
    bool addDistrictEdge(const std::string tazID, const std::string edgeID, const bool isSource);

    /** @brief Retrieves all TAZ (districts) from the network
     *
     * @return The map of all districts
     */
    const std::map<std::string, std::pair<std::vector<std::string>, std::vector<std::string> > >& getDistricts() const {
        return myDistricts;
    }

    /** @brief Retrieves an edge from the network
     *
     * This is not very pretty, but necessary, though, as routes run
     *  over instances, not over ids.
     *
     * @param[in] name The name of the edge to retrieve
     * @return The named edge if known, otherwise 0
     */
    ROEdge* getEdge(const std::string& name) const {
        return myEdges.get(name);
    }


    /** @brief Retrieves an edge from the network when the lane id is given
     *
     * @param[in] laneID The name of the lane to retrieve the edge for
     * @return The edge of the named lane if known, otherwise 0
     */
    ROEdge* getEdgeForLaneID(const std::string& laneID) const {
        return getEdge(laneID.substr(0, laneID.rfind("_")));
    }


    /* @brief Adds a read node to the network
     *
     * If the node is already known (another one with the same id exists),
     *  an error is generated and given to msg-error-handler. The node
     *  is deleted in this case
     *
     * @param[in] node The node to add
     */
    void addNode(RONode* node);


    /** @brief Retrieves an node from the network
     *
     * @param[in] name The name of the node to retrieve
     * @return The named node if known, otherwise 0
     * @todo Check whether a const pointer may be returned
     */
    RONode* getNode(const std::string& id) const {
        return myNodes.get(id);
    }


    /* @brief Adds a read bus stop to the network
     *
     * If the bus stop is already known (another one with the same id exists),
     *  an error is generated and given to msg-error-handler. The stop
     *  is deleted in this case
     *
     * @param[in] node The stop to add
     */
    void addBusStop(const std::string& id, SUMOVehicleParameter::Stop* stop);


    /* @brief Adds a read container stop to the network
     *
     * If the container stop is already known (another one with the same id exists),
     *  an error is generated and given to msg-error-handler. The stop
     *  is deleted in this case
     *
     * @param[in] node The stop to add
     */
    void addContainerStop(const std::string& id, SUMOVehicleParameter::Stop* stop);


    /* @brief Adds a read parking area to the network
     *
     * If the parking area is already known (another one with the same id exists),
     *  an error is generated and given to msg-error-handler. The stop
     *  is deleted in this case
     *
     * @param[in] node The stop to add
     */
    void addParkingArea(const std::string& id, SUMOVehicleParameter::Stop* stop);

    /** @brief Retrieves a bus stop from the network
     *
     * @param[in] name The name of the stop to retrieve
     * @return The named stop if known, otherwise 0
     */
    const SUMOVehicleParameter::Stop* getBusStop(const std::string& id) const {
        std::map<std::string, SUMOVehicleParameter::Stop*>::const_iterator it = myBusStops.find(id);
        if (it == myBusStops.end()) {
            return 0;
        }
        return it->second;
    }


    /** @brief Retrieves a container stop from the network
     *
     * @param[in] name The name of the stop to retrieve
     * @return The named stop if known, otherwise 0
     */
    const SUMOVehicleParameter::Stop* getContainerStop(const std::string& id) const {
        std::map<std::string, SUMOVehicleParameter::Stop*>::const_iterator it = myContainerStops.find(id);
        if (it == myContainerStops.end()) {
            return 0;
        }
        return it->second;
    }


    /** @brief Retrieves a parking area from the network
     *
     * @param[in] name The name of the stop to retrieve
     * @return The named stop if known, otherwise 0
     */
    const SUMOVehicleParameter::Stop* getParkingArea(const std::string& id) const {
        std::map<std::string, SUMOVehicleParameter::Stop*>::const_iterator it = myParkingAreas.find(id);
        if (it == myParkingAreas.end()) {
            return 0;
        }
        return it->second;
    }
    //@}



    /// @name Insertion and retrieval of vehicle types, vehicles, routes, and route definitions
    //@{

    /** @brief Checks whether the vehicle type (distribution) may be added
     *
     * This method checks also whether the default type may still be replaced
     * @param[in] id The id of the vehicle type (distribution) to add
     * @return Whether the type (distribution) may be added
     */
    bool checkVType(const std::string& id);


    /** @brief Adds a read vehicle type definition to the network
     *
     * If the vehicle type definition is already known (another one with
     *  the same id exists), false is returned, and the vehicle type
     *  is deleted.
     *
     * @param[in] def The vehicle type to add
     * @return Whether the vehicle type could be added
     */
    virtual bool addVehicleType(SUMOVTypeParameter* type);


    /** @brief Adds a vehicle type distribution
     *
     * If another vehicle type (or distribution) with the same id exists, false is returned.
     *  Otherwise, the vehicle type distribution is added to the internal vehicle type distribution
     *  container "myVTypeDistDict".
     *
     * This control get responsible for deletion of the added vehicle
     *  type distribution.
     *
     * @param[in] id The id of the distribution to add
     * @param[in] vehTypeDistribution The vehicle type distribution to add
     * @return Whether the vehicle type could be added
     */
    bool addVTypeDistribution(const std::string& id, RandomDistributor<SUMOVTypeParameter*>* vehTypeDistribution);


    /** @brief Retrieves the named vehicle type
     *
     * If the name is "" the default type is returned.
     * If the named vehicle type (or typeDistribution) was not added to the net before
     * 0 is returned
     *
     * @param[in] id The id of the vehicle type to return
     * @return The named vehicle type
     * @todo Check whether a const pointer may be returned
     */
    SUMOVTypeParameter* getVehicleTypeSecure(const std::string& id);


    /* @brief Adds a route definition to the network
     *
     * If the route definition is already known (another one with
     *  the same id exists), false is returned, but the route definition
     *  is not deleted.
     *
     * @param[in] def The route definition to add
     * @return Whether the route definition could be added
     * @todo Rename myRoutes to myRouteDefinitions
     */
    bool addRouteDef(RORouteDef* def);


    /** @brief Returns the named route definition
     *
     * @param[in] name The name of the route definition to retrieve
     * @return The named route definition if known, otherwise 0
     * @todo Check whether a const pointer may be returned
     * @todo Rename myRoutes to myRouteDefinitions
     */
    RORouteDef* getRouteDef(const std::string& name) const {
        return myRoutes.get(name);
    }


    /* @brief Adds a vehicle to the network
     *
     * If the vehicle is already known (another one with the same id
     *  exists), false is returned, but the vehicle is not deleted.
     *
     * Otherwise, the number of loaded routes ("myReadRouteNo") is increased.
     *
     * @param[in] id The id of the vehicle to add
     * @param[in] veh The vehicle to add
     * @return Whether the vehicle could be added
     */
    virtual bool addVehicle(const std::string& id, ROVehicle* veh);


    /* @brief Adds a flow of vehicles to the network
     *
     * If the flow is already known (another one with the same id
     *  exists), false is returned, but the vehicle parameter are not deleted.
     *
     * Otherwise, the number of loaded routes ("myReadRouteNo") is increased.
     *
     * @param[in] flow The parameter of the flow to add
     * @return Whether the flow could be added
     */
    bool addFlow(SUMOVehicleParameter* flow, const bool randomize);


    /* @brief Adds a person to the network
     *
     * @param[in] person   The person to add
     */
    bool addPerson(ROPerson* person);


    /* @brief Adds a container to the network
     *
     * @param[in] depart The departure time of the container
     * @param[in] desc   The xml description of the container
     */
    void addContainer(const SUMOTime depart, const std::string desc);
    // @}


    /// @name Processing stored vehicle definitions
    //@{

    /** @brief Computes routes described by their definitions and saves them
     *
     * As long as a vehicle with a departure time smaller than the given
     *  exists, its route is computed and it is written and removed from
     *  the internal container.
     *
     * @param[in] options The options used during this process
     * @param[in] provider The router provider for routes computation
     * @param[in] time The time until which route definitions shall be processed
     * @return The last seen departure time>=time
     */
    SUMOTime saveAndRemoveRoutesUntil(OptionsCont& options,
                                      const RORouterProvider& provider, SUMOTime time);


    /// Returns the information whether further vehicles, persons or containers are stored
    virtual bool furtherStored();
    //@}





    /** @brief Opens the output for computed routes
     *
     * If the second parameter is set, a second file for route alternatives
     *  will be opened.
     * If one of the file outputs can not be build, an IOError is thrown.
     *
     * @param[in] options The options to be asked for "output-file" and "vtype-output"
     * @param[in] altFilename The name of the file for writing alternatives, "" means no alternatives
     */
    void openOutput(const OptionsCont& options, const std::string altFilename = "");


    /** @brief closes the file output for computed routes and deletes associated threads if necessary */
    void cleanup();


    /// Returns the total number of edges the network contains including internal edges
    int getEdgeNo() const;

    /// Returns the number of internal edges the network contains
    int getInternalEdgeNumber() const;

    const std::map<std::string, ROEdge*>& getEdgeMap() const;

    static void adaptIntermodalRouter(ROIntermodalRouter& router);

    bool hasPermissions() const;

    void setPermissionsFound();

    OutputDevice* getRouteOutput(const bool alternative = false) {
        if (alternative) {
            return myRouteAlternativesOutput;
        }
        return myRoutesOutput;
    }

#ifdef HAVE_FOX
    FXWorkerThread::Pool& getThreadPool() {
        return myThreadPool;
    }

    class WorkerThread : public FXWorkerThread, public RORouterProvider {
    public:
        WorkerThread(FXWorkerThread::Pool& pool,
                     const RORouterProvider& original)
            : FXWorkerThread(pool), RORouterProvider(original) {}
        virtual ~WorkerThread() {
            stop();
        }
    };

    class BulkmodeTask : public FXWorkerThread::Task {
    public:
        BulkmodeTask(const bool value) : myValue(value) {}
        void run(FXWorkerThread* context) {
            static_cast<WorkerThread*>(context)->getVehicleRouter().setBulkMode(myValue);
        }
    private:
        const bool myValue;
    private:
        /// @brief Invalidated assignment operator.
        BulkmodeTask& operator=(const BulkmodeTask&);
    };
#endif


private:
    void checkFlows(SUMOTime time, MsgHandler* errorHandler);

    void createBulkRouteRequests(const RORouterProvider& provider, const SUMOTime time, const bool removeLoops);

private:
    /// @brief Unique instance of RONet
    static RONet* myInstance;

    /// @brief Known vehicle ids
    std::set<std::string> myVehIDs;

    /// @brief Known person ids
    std::set<std::string> myPersonIDs;

    /// @brief Known nodes
    NamedObjectCont<RONode*> myNodes;

    /// @brief Known edges
    NamedObjectCont<ROEdge*> myEdges;

    /// @brief Known bus stops
    std::map<std::string, SUMOVehicleParameter::Stop*> myBusStops;

    /// @brief Known container stops
    std::map<std::string, SUMOVehicleParameter::Stop*> myContainerStops;

    /// @brief Known parking areas
    std::map<std::string, SUMOVehicleParameter::Stop*> myParkingAreas;

    /// @brief Known vehicle types
    NamedObjectCont<SUMOVTypeParameter*> myVehicleTypes;

    /// @brief Vehicle type distribution dictionary type
    typedef std::map< std::string, RandomDistributor<SUMOVTypeParameter*>* > VTypeDistDictType;
    /// @brief A distribution of vehicle types (probability->vehicle type)
    VTypeDistDictType myVTypeDistDict;

    /// @brief Whether no vehicle type has been loaded yet
    bool myDefaultVTypeMayBeDeleted;

    /// @brief Whether no pedestrian type has been loaded yet
    bool myDefaultPedTypeMayBeDeleted;

    /// @brief Known routes
    NamedObjectCont<RORouteDef*> myRoutes;

    /// @brief Known routables
    RoutablesMap myRoutables;

    /// @brief Known flows
    NamedObjectCont<SUMOVehicleParameter*> myFlows;

    /// @brief Known containers
    typedef std::multimap<const SUMOTime, const std::string> ContainerMap;
    ContainerMap myContainers;

    /// @brief Departure times for randomized flows
    std::map<std::string, std::vector<SUMOTime> > myDepartures;

    /// @brief traffic assignment zones with sources and sinks
    std::map<std::string, std::pair<std::vector<std::string>, std::vector<std::string> > > myDistricts;

    /// @brief The file to write the computed routes into
    OutputDevice* myRoutesOutput;

    /// @brief The file to write the computed route alternatives into
    OutputDevice* myRouteAlternativesOutput;

    /// @brief The file to write the vehicle types into
    OutputDevice* myTypesOutput;

    /// @brief The number of read routes
    int myReadRouteNo;

    /// @brief The number of discarded routes
    int myDiscardedRouteNo;

    /// @brief The number of written routes
    int myWrittenRouteNo;

    /// @brief Whether the network contains edges which not all vehicles may pass
    bool myHavePermissions;

    /// @brief The vehicle class specific speed restrictions
    std::map<std::string, std::map<SUMOVehicleClass, SUMOReal> > myRestrictions;

    /// @brief The number of internal edges in the dictionary
    int myNumInternalEdges;

    /// @brief handler for ignorable error messages
    MsgHandler* myErrorHandler;

#ifdef HAVE_FOX
private:
    class RoutingTask : public FXWorkerThread::Task {
    public:
        RoutingTask(RORoutable* v, const bool removeLoops, MsgHandler* errorHandler)
            : myRoutable(v), myRemoveLoops(removeLoops), myErrorHandler(errorHandler) {}
        void run(FXWorkerThread* context);
    private:
        RORoutable* const myRoutable;
        const bool myRemoveLoops;
        MsgHandler* const myErrorHandler;
    private:
        /// @brief Invalidated assignment operator.
        RoutingTask& operator=(const RoutingTask&);
    };


private:
    /// @brief for multi threaded routing
    FXWorkerThread::Pool myThreadPool;
#endif

private:
    /// @brief Invalidated copy constructor
    RONet(const RONet& src);

    /// @brief Invalidated assignment operator
    RONet& operator=(const RONet& src);

};


#endif

/****************************************************************************/

