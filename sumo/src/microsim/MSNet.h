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
#include "MSPhoneNet.h"
#include <microsim/trigger/MSBusStop.h>

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
#ifdef _MESSAGES
class MSMessageEmitter;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSNet
 * @brief The simulated network and simulation perfomer
 *
 * The main simulation class.
 */
class MSNet {
public:
    /** Get a pointer to the unique instance of MSNet (singleton).
     * @return Pointer to the unique MSNet-instance.
     */
    static MSNet* getInstance();

    MSNet(MSVehicleControl *vc, MSEventControl *beginOfTimestepEvents,
          MSEventControl *endOfTimestepEvents, MSEventControl *emissionEvents);


    /// Destructor.
    virtual ~MSNet();

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

    /** @brief Simulates from timestep start to stop.
        start and stop in timesteps.
        In each timestep we emit Vehicles, move Vehicles,
        the Vehicles change Lanes.  The method returns true when the
        simulation could be finished without errors, otherwise
        false. */
    int simulate(SUMOTime start, SUMOTime stop);

    void initialiseSimulation();

    void closeSimulation(SUMOTime start, SUMOTime stop);


    /// performs a single simulation step
    void simulationStep(SUMOTime start, SUMOTime step);

    /** Clears all dictionaries */
    static void clearAll();


    long getSimStepDurationInMillis() const;

    /// route handler may add routes and vehicles
    friend class MSRouteHandler;

    SUMOTime getCurrentTimeStep() const;


    void writeOutput();

    virtual void closeBuilding(MSEdgeControl *edges,
                               MSJunctionControl *junctions, MSRouteLoaderControl *routeLoaders,
                               MSTLLogicControl *tlc,
                               std::vector<int> stateDumpTimes, std::string stateDumpFiles);

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
    void loadState(BinaryInputDevice &bis) throw();
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


    /** @brief Returns the GSM-network
     * @return The GSM-network representation
     * @deprecated MSPhoneNet should not be a part of MSNet
     * @todo return a reference
     */
    MSPhoneNet * getMSPhoneNet() throw() {
        return myMSPhoneNet;
    }
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


protected:
    /** initialises the MeanData-container */
    static void initMeanData(std::vector<int> dumpMeanDataIntervals,
                             std::string baseNameDumpFiles);

    /// Unique instance of MSNet
    static MSNet* myInstance;

    /// Unique ID.
    std::string myID;

    /** route loader for dynamic loading of routes */
    MSRouteLoaderControl *myRouteLoaders;

    /// Current time step.
    SUMOTime myStep;



    /// @name Substructures
    /// @{

    /** @brief Controls vehicle building and deletion; @see MSVehicleControl */
    MSVehicleControl *myVehicleControl;
    /** @brief Controls person building and deletion; @see MSPersonControl */
    MSPersonControl *myPersonControl;
    /** @brief Controls edges, performs vehicle movement; @see MSEdgeControl */
    MSEdgeControl* myEdges;
    /** @brief Controls junctions, realizes right-of-way rules; @see MSJunctionControl */
    MSJunctionControl* myJunctions;
    /** @brief Controls tls logics, realizes waiting on tls rules; @see MSJunctionControl */
    MSTLLogicControl *myLogics;
    /** @brief Controls vehicle emissions; @see MSEmitControl */
    MSEmitControl* myEmitter;
    /** @brief Controls detectors; @see MSDetectorControl */
    MSDetectorControl *myDetectorControl;
    /** @brief Controls events executed at the begin of a time step; @see MSEventControl */
    MSEventControl *myBeginOfTimestepEvents;
    /** @brief Controls events executed at the end of a time step; @see MSEventControl */
    MSEventControl *myEndOfTimestepEvents;
    /** @brief Controls emission events; @see MSEventControl */
    MSEventControl *myEmissionEvents;
    /** @brief A container for geometrical shapes; @see  ShapeContainer*/
    ShapeContainer *myShapeContainer; // could be a direct member
    /** @brief A GSM-network representation
     * @deprecated Should not be a member of MSNet
     */
    MSPhoneNet * myMSPhoneNet;
    /// @}


    /// @name data needed for computing performance values
    //{
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

    std::vector<int> myStateDumpTimes;
    std::string myStateDumpFiles;

    int myTooManyVehicles;

    /// @brief Bus stop dictionary type
    typedef std::map< std::string, MSBusStop* > BusStopDictType;
    /// @brief Dictionary of bus stops
    BusStopDictType myBusStopDict;


private:
    /// @brief Invalidated copy constructor.
    MSNet(const MSNet&);

    /// @brief Invalidated assignment operator.
    MSNet& operator=(const MSNet&);

#ifdef _MESSAGES
    /// The message Emitter
    MsgEmitterDict myMsgEmitter;

    std::vector<MSMessageEmitter*> msgEmitVec;
#endif

};


#endif

/****************************************************************************/

