/****************************************************************************/
/// @file    MSNet.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// The simulated network and simulation perfomer
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
#include <microsim/output/MSMeanData_Net.h>

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
class MSTriggerControl;
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
class MSNet
{
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
						  bool xy);
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


    /** @brief Returns the number of unique mean-data-dump-intervals.
        In vehicles and lanes you will need one element more for the
        GUI-dump. */
    unsigned getNDumpIntervals(void);

    long getSimStepDurationInMillis() const;

    /// route handler may add routes and vehicles
    friend class MSRouteHandler;

    SUMOTime getCurrentTimeStep() const;


    void writeOutput();

    size_t getMeanDataSize() const;
    void addMeanData(MSMeanData_Net *newMeanData);

    virtual void closeBuilding(MSEdgeControl *edges,
                               MSJunctionControl *junctions, MSRouteLoaderControl *routeLoaders,
                               MSTLLogicControl *tlc,
                               const std::vector<MSMeanData_Net*> &meanData,
                               std::vector<int> stateDumpTimes, std::string stateDumpFiles);

    bool logSimulationDuration() const {
        return myLogExecutionTime;
    }

    //{
    /// to be called before a simulation step is done, this prints the current step number
    void preSimStepOutput() const;

    /// to be called after a simulation step is done, this prints some further statistics
    void postSimStepOutput() const;
    //}

    void saveState(std::ostream &os);
    void loadState(BinaryInputDevice &bis);

    virtual MSRouteLoader *buildRouteLoader(const std::string &file, int incDUABase, int incDUAStage);

    SUMOReal getTooSlowRTF() const;


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


    /** @brief Returns the trigger control
     * @return The trigger control
     * @see MSTriggerControl
     * @see myTriggerControl
     */
    MSTriggerControl &getTriggerControl() throw() {
        return *myTriggerControl;
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



    /** @brief List of intervals and filehandles.
        At the end of each intervall the mean data (flow, density, speed ...)
        of each lane is calculated and written to file. */
    std::vector<MSMeanData_Net*> myMeanData;

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
    /** @brief Controls triggers; @see MSTriggerControl */
    MSTriggerControl *myTriggerControl;
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

    //{@ data needed for computing performance values
    /// Information whether the simulation duration shall be logged
    bool myLogExecutionTime;

    /// Information whether the number of the simulation step shall be logged
    bool myLogStepNumber;

    /// The last simulation step begin, end and duration
    long mySimStepBegin, mySimStepEnd, mySimStepDuration;

    /// The overall simulation duration
    long mySimDuration;

    /// The overall number of vehicle movements
    long myVehiclesMoved;
    //}

    std::vector<int> myStateDumpTimes;
    std::string myStateDumpFiles;

    SUMOReal myTooSlowRTF;
    int myTooManyVehicles;


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

