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


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSNet
 * @brief The simulated network and simulation perfomer
 *
 * The main simulation class. Holds the network and indirectly vehicles which
 * are stored within a MSEmitControl - emitter which itself is a part of
 * MSNet.
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

    MSVehicleControl &getVehicleControl() const;
    MSPersonControl &getPersonControl() const;

    void writeOutput();

    /// Returns the number of vehicles that wait for being emitted
    size_t getWaitingVehicleNo() const;

    size_t getMeanDataSize() const;
    MSEdgeControl &getEdgeControl();
    MSDetectorControl &getDetectorControl();
    MSTriggerControl &getTriggerControl();
    MSTLLogicControl &getTLSControl();
    MSJunctionControl &getJunctionControl();
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

    ShapeContainer &getShapeContainer() const {
        return *myShapeContainer;
    }

    virtual MSRouteLoader *buildRouteLoader(const std::string &file, int incDUABase, int incDUAStage);

    SUMOReal getTooSlowRTF() const;

    MSEventControl &getBeginOfTimestepEvents() {
        return *myBeginOfTimestepEvents;
    }

    MSEventControl &getEndOfTimestepEvents() {
        return *myEndOfTimestepEvents;
    }

    MSEventControl &getEmissionEvents() {
        return *myEmissionEvents;
    }

    /////////////////////////////////////////////
    MSPhoneNet * getMSPhoneNet() {
        return myMSPhoneNet;
    } ;
    /////////////////////////////////////////////

protected:
    MSPhoneNet * myMSPhoneNet;
    /** initialises the MeanData-container */
    static void initMeanData(std::vector<int> dumpMeanDataIntervals,
                             std::string baseNameDumpFiles);

    /// Unique instance of MSNet
    static MSNet* myInstance;

    /// Unique ID.
    std::string myID;

    /** Lane-changing is done by this object. */
    MSEdgeControl* myEdges;

    /// Sets the right-of-way rules and moves first cars.
    MSJunctionControl* myJunctions;

    /// Masks requests from cars having red
    MSTLLogicControl *myLogics;

    /// Emits cars into the lanes.
    MSEmitControl* myEmitter;

    /** route loader for dynamic loading of routes */
    MSRouteLoaderControl *myRouteLoaders;

    /// Current time step.
    SUMOTime myStep;



    /** @brief List of intervals and filehandles.
        At the end of each intervall the mean data (flow, density, speed ...)
        of each lane is calculated and written to file. */
    std::vector<MSMeanData_Net*> myMeanData;

    /** @brief An instance responsible for vehicle */
    MSVehicleControl *myVehicleControl;
    mutable MSPersonControl *myPersonControl;
    MSDetectorControl *myDetectorControl;
    MSTriggerControl *myTriggerControl;
    MSEventControl *myBeginOfTimestepEvents;
    MSEventControl *myEndOfTimestepEvents;
    MSEventControl *myEmissionEvents;
    ShapeContainer *myShapeContainer; // could be a direct member

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
    /// Copy constructor.
    MSNet(const MSNet&);

    /// Assignment operator.
    MSNet& operator=(const MSNet&);

};


#endif

/****************************************************************************/

