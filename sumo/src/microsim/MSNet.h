/****************************************************************************/
/// @file    MSNet.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// The simulated network and simulation perfomer
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/geom/Position2D.h>
#include <utils/common/SUMOTime.h>
#include <microsim/trigger/MSBusStop.h>
#include <utils/common/UtilExceptions.h>

#ifdef _MESSAGES
#include <utils/common/NamedObjectCont.h>
#endif

// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSEdgeControl;
class MSJunctionControl;
class MSEmitControl;
class MSRouteLoaderControl;
class MSPersonControl;
class MSVehicle;
class MSRoute;
class MSLane;
class MSTLLogicControl;
class MSTrigger;
class MSDetectorControl;
class ShapeContainer;
class BinaryInputDevice;
class MSRouteLoader;
class MSEdgeWeightsStorage;
class SUMOVehicle;
#ifdef _MESSAGES
class MSMessageEmitter;
#endif


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
    static MSNet* getInstance() throw(ProcessError);


    /** @brief Constructor
     *
     * This constructor builds a net of which only some basic structures are initialised.
     * It prepares the network for being filled while loading.
     * As soon as all edge/junction/traffic lights and other containers are build, they
     *  must be initialised using "closeBuilding".
     * @param[in] vc The vehicle control to use
     * @param[in] beginOfTimestepEvents The event control to use for simulation step begin events
     * @param[in] endOfTimestepEvents The event control to use for simulation step end events
     * @param[in] emissionEvents The event control to use for emission events
     * @param[in] shapeCont The shape container to use
     * @exception ProcessError If a network was already constructed
     * @see closeBuilding
     */
    MSNet(MSVehicleControl *vc, MSEventControl *beginOfTimestepEvents,
          MSEventControl *endOfTimestepEvents, MSEventControl *emissionEvents,
          ShapeContainer *shapeCont=0) throw(ProcessError);


    /// @brief Destructor
    virtual ~MSNet() throw();


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
    void closeBuilding(MSEdgeControl *edges, MSJunctionControl *junctions,
                       MSRouteLoaderControl *routeLoaders, MSTLLogicControl *tlc,
                       std::vector<SUMOTime> stateDumpTimes, std::vector<std::string> stateDumpFiles) throw();


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
    SimulationState simulationState(SUMOTime stopTime) const throw();


    /** @brief Returns the message to show if a certain state occurs
     * @return Readable description of the state
     */
    static std::string getStateMessage(SimulationState state) throw();


    /** @brief Returns the current simulation step (in s)
     * @return the current simulation step
     */
    SUMOTime getCurrentTimeStep() const;


    /** @brief Write netstate, emission and detector output
     * @todo Which exceptions may occur?
     */
    void writeOutput();


    /** @brief Returns whether duration shall be logged
     * @return Whether duration shall be logged
     */
    bool logSimulationDuration() const throw();


    /// @name Output during the simulation
    //@{

    /** @brief Prints the current step number
     *
     * Called on the begin of a simulation step
     */
    void preSimStepOutput() const throw();


    /** @brief Prints the statistics of the step at its end
     *
     * Called on the end of a simulation step
     */
    void postSimStepOutput() const throw();
    //}



#ifdef HAVE_MESOSIM
    /// @name State I/O (mesosim only)
    /// @{

    /** @brief Saves the current state
     *
     * @param[in] os The stream to write the state into (binary)
     */
    void saveState(std::ostream &os) throw();


    /** @brief Loads the network state
     *
     * @param[in] bis The input to read the state from (binary)
     */
    unsigned int loadState(BinaryInputDevice &bis) throw();
    /// @}
#endif

    /// @name Retrieval of references to substructures
    /// @{

    /** @brief Returns the vehicle control
     * @return The vehicle control
     * @see MSVehicleControl
     * @see myVehicleControl
     */
    MSVehicleControl &getVehicleControl() throw() {
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
    MSPersonControl &getPersonControl() throw();


    /** @brief Returns the edge control
     * @return The edge control
     * @see MSEdgeControl
     * @see myEdges
     */
    MSEdgeControl &getEdgeControl() throw() {
        return *myEdges;
    }


    /** @brief Returns the emission control
     * @return The emission control
     * @see MSEmitControl
     * @see myEmitter
     */
    MSEmitControl &getEmitControl() throw() {
        return *myEmitter;
    }


    /** @brief Returns the detector control
     * @return The detector control
     * @see MSDetectorControl
     * @see myDetectorControl
     */
    MSDetectorControl &getDetectorControl() throw() {
        return *myDetectorControl;
    }


    /** @brief Returns the tls logics control
     * @return The tls logics control
     * @see MSTLLogicControl
     * @see myLogics
     */
    MSTLLogicControl &getTLSControl() throw() {
        return *myLogics;
    }


    /** @brief Returns the junctions control
     * @return The junctions control
     * @see MSJunctionControl
     * @see myJunctions
     */
    MSJunctionControl &getJunctionControl() throw() {
        return *myJunctions;
    }


    /** @brief Returns the event control for events executed at the begin of a time step
     * @return The control reponsible for events that are executed at the begin of a time step
     * @see MSEventControl
     * @see myBeginOfTimestepEvents
     */
    MSEventControl &getBeginOfTimestepEvents() throw() {
        return *myBeginOfTimestepEvents;
    }


    /** @brief Returns the event control for events executed at the end of a time step
     * @return The control reponsible for events that are executed at the end of a time step
     * @see MSEventControl
     * @see myEndOfTimestepEvents
     */
    MSEventControl &getEndOfTimestepEvents() throw() {
        return *myEndOfTimestepEvents;
    }


    /** @brief Returns the event control for emission events
     * @return The control reponsible for emission events
     * @see MSEventControl
     * @see myEmissionEvents
     */
    MSEventControl &getEmissionEvents() throw() {
        return *myEmissionEvents;
    }


    /** @brief Returns the shapes container
     * @return The shapes container
     * @see ShapeContainer
     * @see myShapeContainer
     */
    ShapeContainer &getShapeContainer() throw() {
        return *myShapeContainer;
    }


    /** @brief Returns the net's internal edge travel times/efforts container
     *
     * If the net does not have such a container, it is built.
     * @return The net's knowledge about edge weights
     */
    MSEdgeWeightsStorage &getWeightsStorage() throw();
    /// @}



    /// @name Insertion and retrieval of bus stops
    /// @{

    /** @brief Adds a bus stop
     *
     * If another bus stop with the same id exists, false is returned.
     *  Otherwise, the bus stop is added to the internal bus stop
     *  container "myBusStopDict".
     *
     * This control get responsible for deletion of the added bus stop.
     *
     * @param[in] busStop The bus stop to add
     * @return Whether the bus stop could be added
     */
    bool addBusStop(MSBusStop* busStop) throw() {
        if (myBusStopDict.find(busStop->getID()) == myBusStopDict.end()) {
            myBusStopDict[busStop->getID()] = busStop;
            return true;
        }
        return false;
    }


    /** @brief Returns the named bus stop
     * @param[in] id The id of the bus stop to return.
     * @return The named bus stop, or 0 if no such stop exists
     */
    MSBusStop *getBusStop(const std::string &id) throw() {
        BusStopDictType::iterator it = myBusStopDict.find(id);
        if (it == myBusStopDict.end()) {
            return 0;
        }
        return it->second;
    }


    /** @brief Returns the bus stop close to the given position
     * @param[in] lane the lane of the bus stop to return.
     * @param[in] pos the position of the bus stop to return.
     * @return The bus stop id on the location, or "" if no such stop exists
     */
    std::string getBusStopID(const MSLane* lane, const SUMOReal pos) throw() {
        for (BusStopDictType::iterator it = myBusStopDict.begin(); it != myBusStopDict.end(); ++it) {
            MSBusStop *stop = it->second;
            if (&stop->getLane() == lane && fabs(stop->getEndLanePosition() - pos) < POSITION_EPS) {
                return stop->getID();
            }
        }
        return "";
    }
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
        VEHICLE_STATE_NEWROUTE
    };


    /** @class VehicleStateListener
     * @brief Interface for objects listening to vehicle state changes
     */
    class VehicleStateListener {
    public:
        /// @brief Constructor
        VehicleStateListener() throw() { }

        /// @brief Destructor
        virtual ~VehicleStateListener() throw() { }

        /** @brief Called if a vehicle changes its state
         * @param[in] vehicle The vehicle which changed its state
         * @param[in] to The state the vehicle has changed to
         */
        virtual void vehicleStateChanged(const SUMOVehicle * const vehicle, VehicleState to) throw() = 0;

    };


    /** @brief Adds a vehicle states listener
     * @param[in] listener The listener to add
     */
    void addVehicleStateListener(VehicleStateListener *listener) throw();


    /** @brief Removes a vehicle states listener
     * @param[in] listener The listener to remove
     */
    void removeVehicleStateListener(VehicleStateListener *listener) throw();


    /** @brief Informs all added listeners about a vehicle's state change
     * @param[in] vehicle The vehicle which changed its state
     * @param[in] to The state the vehicle has changed to
     * @see VehicleStateListener:vehicleStateChanged
     */
    void informVehicleStateListener(const SUMOVehicle * const vehicle, VehicleState to) throw();
    /// @}


    /** @class EdgeWeightsProxi
     * @brief A proxi for edge weights known by a vehicle/known globally
     *
     * Both getter methods try to return the vehicle's knowledge about the edge/time, first.
     *  If not existing,they try to retrieve it from the global knowledge. If not existing,
     *  the travel time retrieval method returns the edges' length divided by the maximum speed
     *  (information from the first lane is used). The default value for the effort is 0.
     * @see MSEdgeWeightsStorage
     */
    class EdgeWeightsProxi {
    public:
        /** @brief Constructor
         * @param[in] vehKnowledge The vehicle's edge weights knowledge
         * @param[in] netKnowledge The global edge weights knowledge
         */
        EdgeWeightsProxi(const MSEdgeWeightsStorage &vehKnowledge,
                         const MSEdgeWeightsStorage &netKnowledge)
                : myVehicleKnowledge(vehKnowledge), myNetKnowledge(netKnowledge) {}


        /// @brief Destructor
        ~EdgeWeightsProxi() {}


        /** @brief Returns the travel time to pass an edge
         * @param[in] e The edge for which the travel time to be passed shall be returned
         * @param[in] v The vehicle that is rerouted
         * @param[in] t The time for which the travel time shall be returned [s]
         * @return The travel time for an edge
         * @see DijkstraRouterTT_ByProxi
         */
        SUMOReal getTravelTime(const MSEdge * const e, const SUMOVehicle * const v, SUMOReal t) const;


        /** @brief Returns the effort to pass an edge
         * @param[in] e The edge for which the effort to be passed shall be returned
         * @param[in] v The vehicle that is rerouted
         * @param[in] t The time for which the effort shall be returned [s]
         * @return The effort (abstract) for an edge
         * @see DijkstraRouterTT_ByProxi
         */
        SUMOReal getEffort(const MSEdge * const e, const SUMOVehicle * const v, SUMOReal t) const;

    private:
        EdgeWeightsProxi& operator=(const EdgeWeightsProxi&); // just to avoid a compiler warning

    private:
        /// @brief The vehicle's knownledge
        const MSEdgeWeightsStorage &myVehicleKnowledge;

        /// @brief The global knownledge
        const MSEdgeWeightsStorage &myNetKnowledge;

    };


#ifdef _MESSAGES
    /// @brief Map of MSMsgEmitter by ID
    typedef NamedObjectCont< MSMessageEmitter* > MsgEmitterDict;

    // TODO
    /**
     * @brief Returns the Message Emitter needed
     *
     * @param whatemit std::string defining the requested MSMessageEmitter.
     * @return the first MessageEmitter found, which has the requested element enabled
     */
    MSMessageEmitter *getMsgEmitter(const std::string& whatemit);

    /**
     *
     *
     */
    void createMsgEmitter(std::string& id,
                          std::string& file,
                          const std::string& base,
                          std::string& whatemit,
                          bool reverse,
                          bool table,
                          bool xy,
                          SUMOReal step);
#endif

protected:
    /// @brief Unique instance of MSNet
    static MSNet* myInstance;

    /// @brief Route loader for dynamic loading of routes
    MSRouteLoaderControl *myRouteLoaders;

    /// @brief Current time step.
    SUMOTime myStep;



    /// @name Substructures
    /// @{

    /// @brief Controls vehicle building and deletion; @see MSVehicleControl
    MSVehicleControl *myVehicleControl;
    /// @brief Controls person building and deletion; @see MSPersonControl
    MSPersonControl *myPersonControl;
    /// @brief Controls edges, performs vehicle movement; @see MSEdgeControl
    MSEdgeControl* myEdges;
    /// @brief Controls junctions, realizes right-of-way rules; @see MSJunctionControl
    MSJunctionControl* myJunctions;
    /// @brief Controls tls logics, realizes waiting on tls rules; @see MSJunctionControl
    MSTLLogicControl *myLogics;
    /// @brief Controls vehicle emissions; @see MSEmitControl
    MSEmitControl* myEmitter;
    /// @brief Controls detectors; @see MSDetectorControl
    MSDetectorControl *myDetectorControl;
    /// @brief Controls events executed at the begin of a time step; @see MSEventControl
    MSEventControl *myBeginOfTimestepEvents;
    /// @brief Controls events executed at the end of a time step; @see MSEventControl
    MSEventControl *myEndOfTimestepEvents;
    /// @brief Controls emission events; @see MSEventControl
    MSEventControl *myEmissionEvents;
    /// @brief A container for geometrical shapes; @see ShapeContainer
    ShapeContainer *myShapeContainer;
    /// @brief The net's knowledge about edge efforts/travel times; @see MSEdgeWeightsStorage
    MSEdgeWeightsStorage *myEdgeWeights;
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
    long myVehiclesMoved;
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


    /// @brief Bus stop dictionary type
    typedef std::map< std::string, MSBusStop* > BusStopDictType;
    /// @brief Dictionary of bus stops
    BusStopDictType myBusStopDict;

    /// @brief Container for vehicle state listener
    std::vector<VehicleStateListener*> myVehicleStateListeners;


#ifdef _MESSAGES
    /// @brief The message emitter map
    MsgEmitterDict myMsgEmitter;

    /// @brief List of message emitters
    std::vector<MSMessageEmitter*> msgEmitVec;
#endif


private:
    /// @brief Invalidated copy constructor.
    MSNet(const MSNet&);

    /// @brief Invalidated assignment operator.
    MSNet& operator=(const MSNet&);


};


#endif

/****************************************************************************/

