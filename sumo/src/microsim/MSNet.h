/****************************************************************************/
/// @file    MSNet.h
/// @author  Christian Roessel
/// @author  Jakob Erdmann
/// @author  Daniel Krajzewicz
/// @author  Thimor Bohn
/// @author  Eric Nicolay
/// @author  Clemens Honomichl
/// @author  Michael Behrisch
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// The simulated network and simulation perfomer
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSNet_h
#define MSNet_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <typeinfo>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>
#include "MSVehicleControl.h"
#include "MSEventControl.h"
#include <utils/geom/Boundary.h>
#include <utils/geom/Position.h>
#include <utils/common/SUMOTime.h>
#include <microsim/trigger/MSBusStop.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/NamedObjectCont.h>
#include <utils/common/SUMOAbstractRouter.h>
#include <utils/common/DijkstraRouterTT.h>
#include <utils/common/DijkstraRouterEffort.h>
#include <utils/common/AStarRouter.h>
#include <utils/common/NamedRTree.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSEdgeControl;
class MSJunctionControl;
class MSInsertionControl;
class SUMORouteLoaderControl;
class MSPersonControl;
class MSVehicle;
class MSRoute;
class MSLane;
class MSTLLogicControl;
class MSDetectorControl;
class ShapeContainer;
class BinaryInputDevice;
class MSEdgeWeightsStorage;
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSNet
 * @brief The simulated network and simulation perfomer
 */
class MSNet {
public:
    /** @enum SimulationState
     * @brief Possible states of a simulation - running or stopped with different reasons
     */
    enum SimulationState {
        /// @brief The simulation is running
        SIMSTATE_RUNNING,
        /// @brief The final simulation step has been performed
        SIMSTATE_END_STEP_REACHED,
        /// @brief The simulation does not contain further vehicles
        SIMSTATE_NO_FURTHER_VEHICLES,
        /// @brief The connection to a client was closed by the client
        SIMSTATE_CONNECTION_CLOSED,
        /// @brief An error occured during the simulation step
        SIMSTATE_ERROR_IN_SIM,
        /// @brief The simulation contains too many vehicles (@deprecated)
        SIMSTATE_TOO_MANY_VEHICLES
    };


public:
    /** @brief Returns the pointer to the unique instance of MSNet (singleton).
     * @return Pointer to the unique MSNet-instance
     * @exception ProcessError If a network was not yet constructed
     */
    static MSNet* getInstance();


    /** @brief Constructor
     *
     * This constructor builds a net of which only some basic structures are initialised.
     * It prepares the network for being filled while loading.
     * As soon as all edge/junction/traffic lights and other containers are build, they
     *  must be initialised using "closeBuilding".
     * @param[in] vc The vehicle control to use
     * @param[in] beginOfTimestepEvents The event control to use for simulation step begin events
     * @param[in] endOfTimestepEvents The event control to use for simulation step end events
     * @param[in] insertionEvents The event control to use for insertion events
     * @param[in] shapeCont The shape container to use
     * @exception ProcessError If a network was already constructed
     * @see closeBuilding
     */
    MSNet(MSVehicleControl* vc, MSEventControl* beginOfTimestepEvents,
          MSEventControl* endOfTimestepEvents, MSEventControl* insertionEvents,
          ShapeContainer* shapeCont = 0);


    /// @brief Destructor
    virtual ~MSNet();


    /** @brief Closes the network's building process
     *
     * Assigns the structures built while loading to this network.
     * @param[in] edges The control of edges which belong to this network
     * @param[in] junctions The control of junctions which belong to this network
     * @param[in] routeLoaders The route loaders used
     * @param[in] tlc The control of traffic lights which belong to this network
     * @param[in] stateDumpTimes List of time steps at which state shall be written
     * @param[in] stateDumpFiles Filenames for states
     * @todo Try to move all this to the constructor?
     */
    void closeBuilding(MSEdgeControl* edges, MSJunctionControl* junctions,
                       SUMORouteLoaderControl* routeLoaders, MSTLLogicControl* tlc,
                       std::vector<SUMOTime> stateDumpTimes, std::vector<std::string> stateDumpFiles);


    /** @brief Clears all dictionaries
     * @todo Try to move all this to the destructor
     */
    static void clearAll();


    /** @brief Simulates from timestep start to stop
     * @param[in] start The begin time step of the simulation
     * @param[in] stop The end time step of the simulation
     * @return Returns always 0
     * @todo Recheck return value
     * @todo What exceptions may occure?
     */
    int simulate(SUMOTime start, SUMOTime stop);


    /** @brief Performs a single simulation step
     * @todo Which exceptions may occur?
     */
    void simulationStep();


    /** @brief loads routes for the next few steps */
    void loadRoutes();


    /** @brief Closes the simulation (all files, connections, etc.)
     *
     * Writes also performance output
     *
     * @param[in] start The step the simulation was started with
     * @todo What exceptions may occure?
     */
    void closeSimulation(SUMOTime start);


    /** @brief Called after a simulation step, this method returns the current simulation state
     * @param[in] stopTime The time the simulation shall stop at
     * @return The current simulation state
     * @see SimulationState
     */
    SimulationState simulationState(SUMOTime stopTime) const;


    /** @brief Returns the message to show if a certain state occurs
     * @return Readable description of the state
     */
    static std::string getStateMessage(SimulationState state);


    /** @brief Returns the current simulation step
     * @return the current simulation step
     */
    inline SUMOTime getCurrentTimeStep() const {
        return myStep;
    }


    /** @brief Sets the current simulation step (used by state loading)
     * @param step the current simulation step
     */
    inline void setCurrentTimeStep(const SUMOTime step) {
        myStep = step;
    }


    /** @brief Write netstate, summary and detector output
     * @todo Which exceptions may occur?
     */
    void writeOutput();


    /** @brief Returns whether duration shall be logged
     * @return Whether duration shall be logged
     */
    bool logSimulationDuration() const;



    /// @name Output during the simulation
    //@{

    /** @brief Prints the current step number
     *
     * Called on the begin of a simulation step
     */
    void preSimStepOutput() const;


    /** @brief Prints the statistics of the step at its end
     *
     * Called on the end of a simulation step
     */
    void postSimStepOutput() const;
    //}



    /// @name Retrieval of references to substructures
    /// @{

    /** @brief Returns the vehicle control
     * @return The vehicle control
     * @see MSVehicleControl
     * @see myVehicleControl
     */
    MSVehicleControl& getVehicleControl() {
        return *myVehicleControl;
    }


    /** @brief Returns the person control
     *
     * If the person control does not exist, yet, it is created.
     *
     * @return The person control
     * @see MSPersonControl
     * @see myPersonControl
     */
    virtual MSPersonControl& getPersonControl();


    /** @brief Returns the edge control
     * @return The edge control
     * @see MSEdgeControl
     * @see myEdges
     */
    MSEdgeControl& getEdgeControl() {
        return *myEdges;
    }


    /** @brief Returns the insertion control
     * @return The insertion control
     * @see MSInsertionControl
     * @see myInserter
     */
    MSInsertionControl& getInsertionControl() {
        return *myInserter;
    }


    /** @brief Returns the detector control
     * @return The detector control
     * @see MSDetectorControl
     * @see myDetectorControl
     */
    MSDetectorControl& getDetectorControl() {
        return *myDetectorControl;
    }


    /** @brief Returns the tls logics control
     * @return The tls logics control
     * @see MSTLLogicControl
     * @see myLogics
     */
    MSTLLogicControl& getTLSControl() {
        return *myLogics;
    }


    /** @brief Returns the junctions control
     * @return The junctions control
     * @see MSJunctionControl
     * @see myJunctions
     */
    MSJunctionControl& getJunctionControl() {
        return *myJunctions;
    }


    /** @brief Returns the event control for events executed at the begin of a time step
     * @return The control reponsible for events that are executed at the begin of a time step
     * @see MSEventControl
     * @see myBeginOfTimestepEvents
     */
    MSEventControl& getBeginOfTimestepEvents() {
        return *myBeginOfTimestepEvents;
    }


    /** @brief Returns the event control for events executed at the end of a time step
     * @return The control reponsible for events that are executed at the end of a time step
     * @see MSEventControl
     * @see myEndOfTimestepEvents
     */
    MSEventControl& getEndOfTimestepEvents() {
        return *myEndOfTimestepEvents;
    }


    /** @brief Returns the event control for insertion events
     * @return The control reponsible for insertion events
     * @see MSEventControl
     * @see myInsertionEvents
     */
    MSEventControl& getInsertionEvents() {
        return *myInsertionEvents;
    }


    /** @brief Returns the shapes container
     * @return The shapes container
     * @see ShapeContainer
     * @see myShapeContainer
     */
    ShapeContainer& getShapeContainer() {
        return *myShapeContainer;
    }


    /** @brief Returns the net's internal edge travel times/efforts container
     *
     * If the net does not have such a container, it is built.
     * @return The net's knowledge about edge weights
     */
    MSEdgeWeightsStorage& getWeightsStorage();
    /// @}



    /// @name Insertion and retrieval of bus stops
    /// @{

    /** @brief Adds a bus stop
     *
     * If another bus stop with the same id exists, false is returned.
     *  Otherwise, the bus stop is added to the internal bus stop
     *  container "myBusStopDict".
     *
     * This control gets responsible for deletion of the added bus stop.
     *
     * @param[in] busStop The bus stop to add
     * @return Whether the bus stop could be added
     */
    bool addBusStop(MSBusStop* busStop);


    /** @brief Returns the named bus stop
     * @param[in] id The id of the bus stop to return.
     * @return The named bus stop, or 0 if no such stop exists
     */
    MSBusStop* getBusStop(const std::string& id) const;


    /** @brief Returns the bus stop close to the given position
     * @param[in] lane the lane of the bus stop to return.
     * @param[in] pos the position of the bus stop to return.
     * @return The bus stop id on the location, or "" if no such stop exists
     */
    std::string getBusStopID(const MSLane* lane, const SUMOReal pos) const;
    /// @}



    /// @name Notification about vehicle state changes
    /// @{

    /// @brief Definition of a vehicle state
    enum VehicleState {
        /// @brief The vehicle was built, but has not yet departed
        VEHICLE_STATE_BUILT,
        /// @brief The vehicle has departed (was inserted into the network)
        VEHICLE_STATE_DEPARTED,
        /// @brief The vehicle started to teleport
        VEHICLE_STATE_STARTING_TELEPORT,
        /// @brief The vehicle ended being teleported
        VEHICLE_STATE_ENDING_TELEPORT,
        /// @brief The vehicle arrived at his destination (is deleted)
        VEHICLE_STATE_ARRIVED,
        /// @brief The vehicle got a new route
        VEHICLE_STATE_NEWROUTE,
        /// @brief The vehicles starts to park
        VEHICLE_STATE_STARTING_PARKING,
        /// @brief The vehicle ends to park
        VEHICLE_STATE_ENDING_PARKING,
        /// @brief The vehicles starts to stop
        VEHICLE_STATE_STARTING_STOP,
        /// @brief The vehicle ends to stop
        VEHICLE_STATE_ENDING_STOP
    };


    /** @class VehicleStateListener
     * @brief Interface for objects listening to vehicle state changes
     */
    class VehicleStateListener {
    public:
        /// @brief Constructor
        VehicleStateListener() { }

        /// @brief Destructor
        virtual ~VehicleStateListener() { }

        /** @brief Called if a vehicle changes its state
         * @param[in] vehicle The vehicle which changed its state
         * @param[in] to The state the vehicle has changed to
         */
        virtual void vehicleStateChanged(const SUMOVehicle* const vehicle, VehicleState to) = 0;

    };


    /** @brief Adds a vehicle states listener
     * @param[in] listener The listener to add
     */
    void addVehicleStateListener(VehicleStateListener* listener);


    /** @brief Removes a vehicle states listener
     * @param[in] listener The listener to remove
     */
    void removeVehicleStateListener(VehicleStateListener* listener);


    /** @brief Informs all added listeners about a vehicle's state change
     * @param[in] vehicle The vehicle which changed its state
     * @param[in] to The state the vehicle has changed to
     * @see VehicleStateListener:vehicleStateChanged
     */
    void informVehicleStateListener(const SUMOVehicle* const vehicle, VehicleState to);
    /// @}



    /** @brief Returns the travel time to pass an edge
     * @param[in] e The edge for which the travel time to be passed shall be returned
     * @param[in] v The vehicle that is rerouted
     * @param[in] t The time for which the travel time shall be returned [s]
     * @return The travel time for an edge
     * @see DijkstraRouterTT_ByProxi
     */
    static SUMOReal getTravelTime(const MSEdge* const e, const SUMOVehicle* const v, SUMOReal t);


    /** @brief Returns the effort to pass an edge
     * @param[in] e The edge for which the effort to be passed shall be returned
     * @param[in] v The vehicle that is rerouted
     * @param[in] t The time for which the effort shall be returned [s]
     * @return The effort (abstract) for an edge
     * @see DijkstraRouterTT_ByProxi
     */
    static SUMOReal getEffort(const MSEdge* const e, const SUMOVehicle* const v, SUMOReal t);


    /* @brief get the router, initialize on first use
     * @param[in] prohibited The vector of forbidden edges (optional)
     */
    SUMOAbstractRouter<MSEdge, SUMOVehicle>& getRouterTT(
        const std::vector<MSEdge*>& prohibited = std::vector<MSEdge*>()) const;
    SUMOAbstractRouter<MSEdge, SUMOVehicle>& getRouterEffort(
        const std::vector<MSEdge*>& prohibited = std::vector<MSEdge*>()) const;


    /** @brief Returns an RTree that contains lane IDs
     * @return An Rtree containing lane IDs
     */
    const NamedRTree& getLanesRTree() const;


protected:
    /// @brief Unique instance of MSNet
    static MSNet* myInstance;

    /// @brief Route loader for dynamic loading of routes
    SUMORouteLoaderControl* myRouteLoaders;

    /// @brief Current time step.
    SUMOTime myStep;



    /// @name Substructures
    /// @{

    /// @brief Controls vehicle building and deletion; @see MSVehicleControl
    MSVehicleControl* myVehicleControl;
    /// @brief Controls person building and deletion; @see MSPersonControl
    MSPersonControl* myPersonControl;
    /// @brief Controls edges, performs vehicle movement; @see MSEdgeControl
    MSEdgeControl* myEdges;
    /// @brief Controls junctions, realizes right-of-way rules; @see MSJunctionControl
    MSJunctionControl* myJunctions;
    /// @brief Controls tls logics, realizes waiting on tls rules; @see MSJunctionControl
    MSTLLogicControl* myLogics;
    /// @brief Controls vehicle insertion; @see MSInsertionControl
    MSInsertionControl* myInserter;
    /// @brief Controls detectors; @see MSDetectorControl
    MSDetectorControl* myDetectorControl;
    /// @brief Controls events executed at the begin of a time step; @see MSEventControl
    MSEventControl* myBeginOfTimestepEvents;
    /// @brief Controls events executed at the end of a time step; @see MSEventControl
    MSEventControl* myEndOfTimestepEvents;
    /// @brief Controls insertion events; @see MSEventControl
    MSEventControl* myInsertionEvents;
    /// @brief A container for geometrical shapes; @see ShapeContainer
    ShapeContainer* myShapeContainer;
    /// @brief The net's knowledge about edge efforts/travel times; @see MSEdgeWeightsStorage
    MSEdgeWeightsStorage* myEdgeWeights;
    /// @}



    /// @name data needed for computing performance values
    /// @{

    /// @brief Information whether the simulation duration shall be logged
    bool myLogExecutionTime;

    /// @brief Information whether the number of the simulation step shall be logged
    bool myLogStepNumber;

    /// @brief The last simulation step begin, end and duration
    long mySimStepBegin, mySimStepEnd, mySimStepDuration;

    /// @brief The overall simulation duration
    long mySimBeginMillis;

    /// @brief The overall number of vehicle movements
    SUMOLong myVehiclesMoved;
    //}



    /// @name State output variables
    /// @{

    /// @brief Times at which a state shall be written
    std::vector<SUMOTime> myStateDumpTimes;
    /// @brief The names for the state files
    std::vector<std::string> myStateDumpFiles;
    /// @}



    /// @brief Storage for maximum vehicle number
    int myTooManyVehicles;

    /// @brief Dictionary of bus stops
    NamedObjectCont<MSBusStop*> myBusStopDict;

    /// @brief Container for vehicle state listener
    std::vector<VehicleStateListener*> myVehicleStateListeners;


    /* @brief The router instance for routing by trigger and by traci
     * @note MSDevice_Routing has its own instance since it uses a different weight function
     * @note we provide one member for every switchable router type
     * because the class structure makes it inconvenient to use a superclass*/
    mutable bool myRouterTTInitialized;
    mutable DijkstraRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle> >* myRouterTTDijkstra;
    mutable AStarRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle> >* myRouterTTAStar;
    mutable DijkstraRouterEffort_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle> >* myRouterEffort;


    /// @brief An RTree structure holding lane IDs
    mutable std::pair<bool, NamedRTree> myLanesRTree;


    /// @brief string constants for simstep stages
    static const std::string STAGE_EVENTS;
    static const std::string STAGE_MOVEMENTS;
    static const std::string STAGE_LANECHANGE;
    static const std::string STAGE_INSERTIONS;

private:
    /// @brief Invalidated copy constructor.
    MSNet(const MSNet&);

    /// @brief Invalidated assignment operator.
    MSNet& operator=(const MSNet&);


};


#endif

/****************************************************************************/

