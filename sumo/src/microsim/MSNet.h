/****************************************************************************/
/// @file    MSNet.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// The simulated network and simulation perfomer
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include "MSPersonControl.h"
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
class Event;
class MSVehicle;
class MSRoute;
class MSVehicleType;
class MSLane;
class MSTLLogicControl;
class MSVehicleTransfer;
class OutputDevice;
class NLBuilder;
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
    /** @brief Returns the pointer to the unique instance of MSNet (singleton).
     * @return Pointer to the unique MSNet-instance
     * @exception ProcessError If a network was not yet constructed
     */
    static MSNet* getInstance() throw(ProcessError);


    /** @brief Constructor
     * @param[in] vc The vehicle control to use
     * @param[in] beginOfTimestepEvents The event control to use for simulation step begin events
     * @param[in] endOfTimestepEvents The event control to use for simulation step end events
     * @param[in] emissionEvents The event control to use for emission events
     * @param[in] shapeCont The shape container to use
     * @exception ProcessError If a network was already constructed
     */
    MSNet(MSVehicleControl *vc, MSEventControl *beginOfTimestepEvents,
          MSEventControl *endOfTimestepEvents, MSEventControl *emissionEvents,
          ShapeContainer *shapeCont=0) throw(ProcessError);


    /// @brief Destructor
    virtual ~MSNet() throw();


    /** @brief Closes the network's building process
     * @param[in] edges The control of edges which belong to this network
     * @param[in] junctions The control of junctions which belong to this network
     * @param[in] routeLoaders The route loaders used
     * @param[in] tlc The control of traffic lights which belong to this network
     * @param[in] stateDumpTimes List of time steps at which state shall be written
     * @param[in] stateDumpFiles Base name for states
     * @todo Try to move all this to the constructor?
     */
    void closeBuilding(MSEdgeControl *edges,
                       MSJunctionControl *junctions, MSRouteLoaderControl *routeLoaders,
                       MSTLLogicControl *tlc, std::vector<int> stateDumpTimes, std::string stateDumpFiles) throw();


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
     * @todo What exceptions may occure?
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




    long getSimStepDurationInMillis() const;

    /// route handler may add routes and vehicles
    friend class MSRouteHandler;

    SUMOTime getCurrentTimeStep() const;


    void writeOutput();


    bool logSimulationDuration() const {
        return myLogExecutionTime;
    }


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

    virtual MSRouteLoader *buildRouteLoader(const std::string &file);



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
     * @return The bus stop on the location, or 0 if no such stop exists
     */
    MSBusStop *getBusStop(const MSLane* lane, const SUMOReal pos) throw() {
        for (BusStopDictType::iterator it = myBusStopDict.begin(); it != myBusStopDict.end(); ++it) {
            MSBusStop *stop = it->second;
            if (&stop->getLane() == lane && fabs(stop->getEndLanePosition() - pos) < POSITION_EPS) {
                return stop;
            }
        }
        return 0;
    }
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
         * @param[in] t The time for which the travel time shall be returned
         * @return The travel time for an edge
         * @see DijkstraRouterTT_ByProxi
         */
        SUMOReal getTravelTime(const MSEdge * const e, const SUMOVehicle * const v, SUMOTime t) const;


        /** @brief Returns the effort to pass an edge
         * @param[in] e The edge for which the effort to be passed shall be returned
         * @param[in] v The vehicle that is rerouted
         * @param[in] t The time for which the effort shall be returned
         * @return The effort (abstract) for an edge
         * @see DijkstraRouterTT_ByProxi
         */
        SUMOReal getEffort(const MSEdge * const e, const SUMOVehicle * const v, SUMOTime t) const;

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
    std::vector<int> myStateDumpTimes;
    /// @brief The base name for states
    std::string myStateDumpFiles;
    /// @}


    /// @brief Storage for maximum vehicle number
    int myTooManyVehicles;


    /// @brief Bus stop dictionary type
    typedef std::map< std::string, MSBusStop* > BusStopDictType;
    /// @brief Dictionary of bus stops
    BusStopDictType myBusStopDict;


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

