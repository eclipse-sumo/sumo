#ifndef MSNet_H
#define MSNet_H
/***************************************************************************
                          MSNet.h  -  We will simulate on this object.
                          Holds all necessary objects for micro-simulation.
                             -------------------
    begin                : Mon, 12 Mar 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.28  2003/08/04 11:45:54  dkrajzew
// missing deletion of traffic light logics on closing a network added; vehicle coloring scheme applied
//
// Revision 1.27  2003/07/30 17:27:21  roessel
// Removed superflous casts in getSeconds and getSteps.
//
// Changed type argument type of getSeconds from Time to double.
//
// Revision 1.26  2003/07/30 09:11:22  dkrajzew
// a better (correct?) processing of yellow lights added; output corrigued; debugging
//
// Revision 1.25  2003/07/22 15:08:28  dkrajzew
// new detector usage applied
//
// Revision 1.24  2003/07/21 18:12:33  roessel
// Comment out MSDetector specific staff.
//
// Revision 1.23  2003/07/21 15:44:21  roessel
// Added two conversion methods from steps to seconds and vice versa.
//
// Revision 1.22  2003/07/21 11:00:38  dkrajzew
// informing the network about vehicles still left within the emitters added
//
// Revision 1.21  2003/06/24 14:49:52  dkrajzew
// unneded members removed (will be replaced, soon)
//
// Revision 1.20  2003/06/24 14:31:58  dkrajzew
// retrieval of current time step added
//
// Revision 1.19  2003/06/19 10:56:55  dkrajzew
// the simulation now also ends when the last vehicle vanishes
//
// Revision 1.18  2003/06/18 11:33:06  dkrajzew
// messaging system added; speedcheck removed; clearing of all structures moved from the destructor to an own method (needed for the gui when loading fails)
//
// Revision 1.17  2003/06/06 10:39:16  dkrajzew
// new usage of MSEventControl applied
//
// Revision 1.16  2003/06/05 16:06:47  dkrajzew
// the initialisation and the ending of a simulation must be available to the gui - simulation mathod was split therefore
//
// Revision 1.15  2003/06/05 10:29:54  roessel
// Modified the event-handling in the simulation loop. Added the new MSTravelcostDetector< MSLaneState > which will replace the old MeanDataDetectors as an example. Needs to be shifted to the proper place (where?).
//
// Revision 1.14  2003/05/27 18:34:41  roessel
// Removed parameter MSEventControl* evc from MSNet::init.
// MSEventControl now accessible via the singleton-mechanism.
//
// Revision 1.13  2003/05/21 16:20:44  dkrajzew
// further work detectors
//
// Revision 1.12  2003/05/21 15:15:42  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.11  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output
// debugged; setting and retrieval of some parameter added
//
// Revision 1.10  2003/04/07 10:29:02  dkrajzew
// usage of globaltime temporary fixed (is still used in
// MSActuatedTrafficLightControl)
//
// Revision 1.9  2003/04/04 15:31:48  roessel
// Commented out the #ifdef _DEBUG because some files claimed that globaltime
// is unknown. There are several files accessing the "debug" variables
// globaltime and searchedtime.
//
// Revision 1.8  2003/03/20 16:21:12  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.7  2003/03/03 14:56:22  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.6  2003/02/07 10:41:51  dkrajzew
// updated
//
// Revision 1.5  2002/10/21 09:55:40  dkrajzew
// begin of the implementation of multireferenced, dynamically loadable routes
//
// Revision 1.4  2002/10/18 11:49:32  dkrajzew
// usage of MeanData rechecked for closing of the generated files and the
// destruction of allocated ressources
//
// Revision 1.3  2002/10/17 10:45:17  dkrajzew
// preinitialisation added; errors due to usage of local myStep instead of
// instance-global myStep patched
//
// Revision 1.2  2002/10/16 16:44:23  dkrajzew
// globa file include; no usage of MSPerson; single step execution implemented
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.13  2002/09/25 17:14:42  roessel
// MeanData calculation and output implemented.
//
// Revision 1.12  2002/08/06 15:40:34  roessel
// Default constructor needs to be (dummy) implemented.
//
// Revision 1.11  2002/08/06 14:13:27  roessel
// New method preInit() and changes in init().
//
// Revision 1.10  2002/07/31 17:33:01  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.10  2002/07/30 15:17:47  croessel
// Made MSNet-class a singleton-class.
//
// Revision 1.9  2002/05/29 17:06:03  croessel
// Inlined some methods. See the .icc files.
//
// Revision 1.8  2002/05/14 07:53:09  dkrajzew
// Windows eol removed
//
// Revision 1.7  2002/05/14 07:45:21  dkrajzew
// new _SPEEDCHECK functions: all methods in MSNet, computation of UPS and MUPS
//
// Revision 1.6  2002/04/17 10:44:13  croessel
// (Windows) Carriage returns removed.
//
// Revision 1.5 2002/04/15 07:38:52 dkrajzew
// Addition of routes and detectors removed; a static information
// about the current time step (globaltime) implemented; output
// computation is now only invoked when needed
//
// Revision 1.4  2002/04/11 15:25:56  croessel
// Changed float to double.
//
// Revision 1.3  2002/04/11 10:33:25  dkrajzew
// Addition of detectors added
//
// Revision 1.2  2002/04/10 16:19:34  croessel
// Modifications due to detector-implementation.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.2  2002/03/14 08:09:26  traffic
// Option for no raw output added
//
// Revision 2.1  2002/03/07 07:55:02  traffic
// implemented the usage of stdout as the default raw output
//
// Revision 2.0  2002/02/14 14:43:18  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.14  2002/02/13 16:30:54  croessel
// Output goes in a file now.
//
// Revision 1.13  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.12  2002/01/16 10:03:34  croessel
// New method "static double deltaT()" and member "static double myDeltaT"
// added. DeltaT is the length of a timestep in seconds.
//
// Revision 1.11  2001/12/20 14:35:54  croessel
// using namespace std replaced by std::
//
// Revision 1.10  2001/12/19 16:36:25  croessel
// Moved methods (assignment, copy-ctor) to private and removed outcommented
// code.
//
// Revision 1.9  2001/11/15 17:12:14  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.8  2001/11/14 11:45:54  croessel
// Resolved conflicts which appeared during suffix-change and
// CR-line-end commits.
//
// Revision 1.7  2001/11/14 10:49:07  croessel
// CR-line-end removed.
//
// Revision 1.6  2001/10/23 09:31:29  traffic
// parser bugs removed
//
// Revision 1.4  2001/09/06 15:39:12  croessel
// Added simple text output to simulation-loop.
//
// Revision 1.3  2001/07/16 16:00:52  croessel
// Changed Route-Container type to map<string, Route*>. Added static dictionary
// methods to access it (same as id-handling).
//
// Revision 1.2  2001/07/16 12:55:47  croessel
// Changed id type from unsigned int to string. Added string-pointer
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef _SPEEDCHECK
#include <ctime>
#endif

#include <typeinfo>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>



/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEdge;
class MSEdgeControl;
class MSJunctionControl;
class MSEmitControl;
class MSEventControl;
class MSRouteLoaderControl;
class Event;
//class MSDetector;
class PreStartInitialised;
class MSVehicle;
class MSRoute;
class MSVehicleType;
class MSLane;
class MSLaneState;
class MSTLLogicControl;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * MSNet
 * The main simulation class. Holds the network and indirectly vehicles which
 * are stored within a MSEmitControl - emitter which itself is a part of MSNet.
 */
class MSNet
{
    /// for some reasons, we won't be able to use static protected variables
    /// otherwise
    friend class GUINet;

public:

    /** Get a pointer to the unique instance of MSNet (singleton).
     * @return Pointer to the unique MSNet-instance.
     */
    static MSNet* getInstance( void );

    /// Type for time (seconds).
    typedef unsigned int Time;

//     /// Detector-container type.
//     typedef std::vector< MSDetector* > DetectorCont;

    /// List of times (intervals or similar)
    typedef std::vector< Time > TimeVector;

    /**
     * @brief Create unique instance of MSNet and initialize with the
     * beginning timestep.
     * To finish the initialization call &ref init.
     * @param startTimestep Timestep the simulation will start with.
     */
    static void preInit( Time startTimestep,
        TimeVector dumpMeanDataIntervalls,
        std::string baseNameDumpFiles/*,
        bool withGUI */);

    /** Initialize the unique MSNet-instance after creation in @ref preInit.
     */
    static void init( std::string id,
                      MSEdgeControl* ec,
                      MSJunctionControl* jc,
//                       DetectorCont* detectors,
                      MSRouteLoaderControl *rlc,
                      MSTLLogicControl *tlc);

    /// Destructor.
    virtual ~MSNet();



    /** @brief Simulates from timestep start to stop.
        start and stop in timesteps.
        In each timestep we emit Vehicles, move Vehicles,
        the Vehicles change Lanes.  The method returns true when the
        simulation could be finished without errors, otherwise
        false. */
    bool simulate( std::ostream *craw, Time start, Time stop );

    void initialiseSimulation(std::ostream *craw/*, Time start, Time stop*/);

    void closeSimulation(std::ostream *craw/*, Time start, Time stop*/);


    /// performs a single simulation step
    void simulationStep( std::ostream *craw, Time start, Time step);

    /** @brief Inserts a MSNet into the static dictionary
        Returns true if the key id isn't already in the dictionary.
        Otherwise returns false. */
    static bool dictionary( std::string id, MSNet* net );

    /** @brief Returns the MSNet associated to the key id if exists,
        otherwise returns 0. */
    static MSNet* dictionary( std::string id );

    /** Clears the dictionary */
    static void clear();

    /** Clears all dictionaries */
    static void clearAll();

    /// Returns the timestep-length in seconds.
    static double deltaT();

    /** @brief Returns the current simulation time in seconds.
        Current means start-time plus runtime. */
    double simSeconds();

    /** Returns the current timestep. */
    Time timestep( void );

    /** @brief Returns the number of unique mean-data-dump-intervalls.
        In vehicles and lanes you will need one element more for the
        GUI-dump. */
    unsigned getNDumpIntervalls( void );

    /** @brief Returns wether we are using a GUI or not.
        The use of a GUI increases the elements of a meanData container. */
//    bool withGUI( void );

    /// adds an item that must be initialised every time the simulation starts
    void addPreStartInitialisedItem(PreStartInitialised *preinit);

    /// initialises items that require it befor simulation start
    void preStartInit();

    /// builds a new vehicle
    virtual MSVehicle *buildNewMSVehicle( std::string id, MSRoute* route,
        MSNet::Time departTime, const MSVehicleType* type,
        int repNo, int repOffset);

    virtual MSVehicle *buildNewVehicle( std::string id, MSRoute* route,
        MSNet::Time departTime, const MSVehicleType* type,
        int repNo, int repOffset);

	/// informs the simulation about the destruction of a vehicle
	void vehicleHasLeft(const std::string &id);



    /// route handler may add routes and vehicles
    friend class MSRouteHandler;

	/// The current simulation time for debugging purposes
    static Time globaltime;

    /// ----------------- debug variables -------------
#ifdef ABS_DEBUG
    static Time searchedtime;
    static std::string searched1, searched2, searchedJunction;
#endif

    /// ----------------- speedcheck variables -------------

    size_t getLoadedVehicleNo() const;

    size_t getEndedVehicleNo() const;

    size_t getRunningVehicleNo() const;

    size_t getEmittedVehicleNo() const;

    Time getCurrentTimeStep() const;

    static double getSeconds( double steps )
        {
            return steps * myDeltaT;
        }

    static Time getSteps( double seconds )
        {
            return static_cast< Time >(
                floor( seconds / myDeltaT ) );
        }


    void newUnbuildVehicleLoaded();

    void newUnbuildVehicleBuild();

protected:
    /** initialises the MeanData-container */
    static void initMeanData( TimeVector dumpMeanDataIntervalls,
        std::string baseNameDumpFiles/*, bool withGUI*/);

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

    /// Definition of the static dictionary to associate string-ids with
    /// objects.
    typedef std::map< std::string, MSNet* > DictType;

    /// Static dictionary to associate string-ids with objects.
    static DictType myDict;

    /// Definition of the container for items to initialise before starting
    typedef std::vector<PreStartInitialised*> PreStartVector;

    /// Container for items to initialise before starting
    PreStartVector myPreStartInitialiseItems;

    /// Timestep [sec]
    static double myDeltaT;

    /// Current time step.
    Time myStep;

//     /// Container of detectors.
//     DetectorCont* myDetectors;

    /// The Net's meanData is a pair of an interval-length and a filehandle.
    class MeanData
    {
    public:
	    /// constructor
        MeanData( Time t, std::ofstream* of )
            : interval( t ),
              file( of )
        {
            (*file) << "<netstats>" << std::endl;
        }

	    /// destructor
        ~MeanData()
        {
            (*file) << "</netstats>" << std::endl;
            file->close();
        }

    	/// the time interval the data shall be aggregated over
        Time interval;

	    /** @brief The file to write aggregated data into.
            For each aggregation time, a single file should be used */
        std::ofstream* file;
    };

    /** @briefList of intervals and filehandles.
        At the end of each intervall the mean data (flow, density, speed ...)
        of each lane is calculated and written to file. */
    std::vector< MeanData* > myMeanData;

    /// Indicates if we are using a GUI.
//    bool myWithGUI;

    /** Last timestep when mean-data was send to GUI. We need it to
     * calculate the intervall which may be not const for the GUI. */
//    Time myLastGUIdumpTimestep;

private:
    /// Copy constructor.
    MSNet( const MSNet& );

    /// Assignment operator.
    MSNet& operator=( const MSNet& );

    /** Default constructor. It makes no sense to build a net without
        initialisation. */
    MSNet();

	size_t myLoadedVehNo;

	size_t myEmittedVehNo;

	size_t myRunningVehNo;

	size_t myEndedVehNo;

//     std::vector< MSLaneState* > laneStateDetectorsM;
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#ifndef DISABLE_INLINE
#include "MSNet.icc"
#endif

#endif

// Local Variables:
// mode:C++
// End:

