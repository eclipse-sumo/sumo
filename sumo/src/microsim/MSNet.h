/****************************************************************************/
/// @file    MSNet.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Holds all necessary objects for micro-simulation.
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _SPEEDCHECK
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <ctime>
#endif

#include <typeinfo>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>

#include "MSInterface_NetRun.h"
#include "output/meandata/MSMeanData_Net_Cont.h"
#include "MSVehicleControl.h"
#include "person/MSPersonControl.h"
#include "MSEventControl.h"
#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include <utils/common/SUMOTime.h>
#include "MSPhoneNet.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSEdgeControl;
class MSJunctionControl;
class MSEmitControl;
class MSRouteLoaderControl;
class Event;
class RGBColor;
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
class MSBuildCells;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSNet
 * The main simulation class. Holds the network and indirectly vehicles which
 * are stored within a MSEmitControl - emitter which itself is a part of
 * MSNet.
 */
class MSNet
            : public MSInterface_NetRun
{
public:
    /**
     * @enum MSNetOutputs
     * List of network-wide outputs
     */
    enum MSNetOutputs {
        /// netstate output
        OS_NETSTATE = 0,
        /// emissions output
        OS_EMISSIONS,
        /// trip information output
        OS_TRIPDURATIONS,
        /// route information output
        OS_VEHROUTE,
        /// TrafficOnline-SS2 output
        OS_DEVICE_TO_SS2,
        /// TrafficOnline-SS2 Cell output
        OS_CELL_TO_SS2,
        /// TrafficOnline-SS2 LA output
        OS_LA_TO_SS2,
        /// TrafficOnline-SS2-SQL output
        OS_DEVICE_TO_SS2_SQL,
        /// TrafficOnline-SS2-SQL Cell output
        OS_CELL_TO_SS2_SQL,
        /// TrafficOnline-SS2-SQL LA output
        OS_LA_TO_SS2_SQL,
        /// TrafficOnline-Cellphone output
        OS_CELLPHONE_DUMP_TO,
        /// Cluster's Information output
        OS_CLUSTER_INFO,
        /// Edge Neighborhood output
        OS_EDGE_NEAR,
        /// Saved Informations output
        OS_SAVED_INFO,
        /// Saved Informations output
        OS_SAVED_INFO_FREQ,
        /// Transmitted Informations output
        OS_TRANS_INFO,
        /// Vehicle in range
        OS_VEH_IN_RANGE,
        /// maximum value
        OS_MAX
    };

public:
    /** Get a pointer to the unique instance of MSNet (singleton).
     * @return Pointer to the unique MSNet-instance.
     */
    static MSNet* getInstance(void);

    /// List of times (intervals or similar)
    typedef std::vector< SUMOTime > TimeVector;

    MSNet(SUMOTime startTimestep, SUMOTime stopTimestep,
          SUMOReal tooSlowRTF, bool logExecTime);
    MSNet(SUMOTime startTimestep, SUMOTime stopTimestep, MSVehicleControl *vc,
          SUMOReal tooSlowRTF, bool logExecTime);


    /// Destructor.
    virtual ~MSNet();

    /** @brief Simulates from timestep start to stop.
        start and stop in timesteps.
        In each timestep we emit Vehicles, move Vehicles,
        the Vehicles change Lanes.  The method returns true when the
        simulation could be finished without errors, otherwise
        false. */
    bool simulate(SUMOTime start, SUMOTime stop);

    void initialiseSimulation();

    void closeSimulation(SUMOTime start, SUMOTime stop);


    /// performs a single simulation step
    void simulationStep(SUMOTime start, SUMOTime step);

    /// compute the distance between equipped vehicle
    void computeCar2Car(void);

    /** @brief Inserts a MSNet into the static dictionary
        Returns true if the key id isn't already in the dictionary.
        Otherwise returns false. */
    static bool dictionary(std::string id, MSNet* net);

    /** @brief Returns the MSNet associated to the key id if exists,
        otherwise returns 0. */
    static MSNet* dictionary(std::string id);

    /** Clears the dictionary */
    static void clear();

    /** Clears all dictionaries */
    static void clearAll();

    /// Returns the timestep-length in seconds.
    static SUMOReal deltaT()
    {
        return myDeltaT;
    }

    /** @brief Returns the current simulation time in seconds.
        Current means start-time plus runtime. */
    SUMOTime simSeconds()
    {
        return (SUMOTime)(myStep * myDeltaT);
    }


    /** @brief Returns the number of unique mean-data-dump-intervalls.
        In vehicles and lanes you will need one element more for the
        GUI-dump. */
    unsigned getNDumpIntervalls(void);

    long getSimStepDurationInMillis() const;

    /// route handler may add routes and vehicles
    friend class MSRouteHandler;

    /// ----------------- debug variables -------------
    /*
    #ifdef ABSCHECK_MEMORY_LEAKS
    static SUMOTime searchedtime;
    static std::string searched1, searched2, searchedJunction;
    #endif
    */
    /// ----------------- speedcheck variables -------------


    SUMOTime getCurrentTimeStep() const;

    static SUMOReal getSeconds(SUMOReal steps)
    {
        return steps * myDeltaT;
    }

    static SUMOTime getSteps(SUMOReal seconds)
    {
        return static_cast< SUMOTime >(
                   floor(seconds / myDeltaT));
    }

    static SUMOReal getMeterPerSecond(SUMOReal cellsPerTimestep)
    {
        return cellsPerTimestep / myDeltaT;
    }

    static SUMOReal getVehPerKm(SUMOReal vehPerCell)
    {
        return (SUMOReal)(vehPerCell * 1000.0);
    }

    static SUMOReal getVehPerHour(SUMOReal vehPerStep)
    {
        return (SUMOReal)(vehPerStep / myDeltaT * 3600.0);
    }

    MSVehicleControl &getVehicleControl() const;
    MSPersonControl &getPersonControl() const;

    void writeOutput();

    friend class MSTriggeredSource;

    bool haveAllVehiclesQuit();

    size_t getMeanDataSize() const;
    MSEdgeControl &getEdgeControl();
    MSDetectorControl &getDetectorControl();
    MSTriggerControl &getTriggerControl();
    MSTLLogicControl &getTLSControl();
    void addMeanData(MSMeanData_Net *newMeanData);

    virtual void closeBuilding(MSEdgeControl *edges,
                               MSJunctionControl *junctions, MSRouteLoaderControl *routeLoaders,
                               MSTLLogicControl *tlc, //ShapeContainer *sc,
                               std::vector<OutputDevice*> streams,
                               const MSMeanData_Net_Cont &meanData,
                               TimeVector stateDumpTimes, std::string stateDumpFiles);

    bool logSimulationDuration() const
    {
        return myLogExecutionTime;
    }

    //{
    /// to be called before a simulation step is done, this prints the current step number
    void preSimStepOutput() const
    {
        std::cout << "Step #" << myStep;
        if (!myLogExecutionTime) {
            std::cout << (char) 13;
        }
    }

    /// to be called after a simulation step is done, this prints some further statistics
    void postSimStepOutput() const
    {
        if (myLogExecutionTime) {
            if (mySimStepDuration!=0) {
                std::cout.setf(std::ios::fixed , std::ios::floatfield) ;    // use decimal format
                std::cout.setf(std::ios::showpoint) ;    // print decimal point
                std::cout << std::setprecision(2) ;
                std::cout << " (" << mySimStepDuration << "ms ~= "
                << (1000./ (SUMOReal) mySimStepDuration) << "*RT, ~"
                << ((SUMOReal) myVehicleControl->getRunningVehicleNo()/(SUMOReal) mySimStepDuration*1000.)
                << "UPS, vehicles"
                << " TOT " << myVehicleControl->getEmittedVehicleNo()
                << " ACT " << myVehicleControl->getRunningVehicleNo()
                << "       "
                << (char) 13;
            } else {
                std::cout << " (" << mySimStepDuration << "ms; further information not available"
                << "       "
                << (char) 13;
            }
        }
    }
    //}

    void saveState(std::ostream &os, long what);
    void loadState(BinaryInputDevice &bis, long what);

    ShapeContainer &getShapeContainer() const
    {
        return *myShapeContainer;
    }

    virtual MSRouteLoader *buildRouteLoader(const std::string &file, int incDUABase, int incDUAStage);

    SUMOReal getTooSlowRTF() const;

    MSEventControl &getBeginOfTimestepEvents()
    {
        return myBeginOfTimestepEvents;
    }

    MSEventControl &getEndOfTimestepEvents()
    {
        return myEndOfTimestepEvents;
    }

    void setOffset(const Position2D &p);
    void setOrigBoundary(const Boundary &p);
    void setConvBoundary(const Boundary &p);
    void setOrigProj(const std::string &proj);


    const Position2D &getOffset() const;
    const Boundary &getOrigBoundary() const;
    const Boundary &getConvBoundary() const;
    const std::string &getOrigProj() const;

    /////////////////////////////////////////////
    MSPhoneNet * getMSPhoneNet()
    {
        return myMSPhoneNet;
    } ;
    /////////////////////////////////////////////

protected:
    MSPhoneNet * myMSPhoneNet;
    /** initialises the MeanData-container */
    static void initMeanData(TimeVector dumpMeanDataIntervalls,
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

    // cells Builder
    MSBuildCells *myCellsBuilder;

    /// Timestep [sec]
    static SUMOReal myDeltaT;

    /// Current time step.
    SUMOTime myStep;



    /** @brief List of intervals and filehandles.
        At the end of each intervall the mean data (flow, density, speed ...)
        of each lane is calculated and written to file. */
    MSMeanData_Net_Cont myMeanData;

    /** @brief An instance responsible for vehicle */
    MSVehicleControl *myVehicleControl;
    MSPersonControl *myPersonControl;
    MSDetectorControl *myDetectorControl;
    MSTriggerControl *myTriggerControl;
    MSEventControl myBeginOfTimestepEvents;
    MSEventControl myEndOfTimestepEvents;
    ShapeContainer *myShapeContainer; // could be a direct member
    /// List of output (files)
    std::vector<OutputDevice*> myOutputStreams;

    //{@ data needed for computing performance values
    /// Information whether the simulation duration shall be logged
    bool myLogExecutionTime;

    /// The last simulation step begin, end and duration
    long mySimStepBegin, mySimStepEnd, mySimStepDuration;

    /// The overall simulation duration
    long mySimDuration;

    /// The overall number of vehicle movements
    long myVehiclesMoved;
    //}

    TimeVector myStateDumpTimes;
    std::string myStateDumpFiles;

    SUMOReal myTooSlowRTF;

    Position2D myOffset;
    Boundary myOrigBoundary, myConvBoundary;
    std::string myOrigProj;

    std::vector<MSVehicle*> myConnected;
    std::vector<MSVehicle*> myClusterHeaders;
    std::vector<MSEdge*> myAllEdges;

private:
    /// Copy constructor.
    MSNet(const MSNet&);

    /// Assignment operator.
    MSNet& operator=(const MSNet&);

};


#endif

/****************************************************************************/

