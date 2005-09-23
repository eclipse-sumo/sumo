/***************************************************************************
                          MSNet.cpp  -  We will simulate on this
                          object. Holds all necessary objects for
                          micro-simulation.
                             -------------------
    begin                : Tue, 06 Mar 2001
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.62  2005/09/23 13:16:40  dkrajzew
// debugging the building process
//
// Revision 1.61  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.60  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.59  2005/07/12 12:25:39  dkrajzew
// made checking for accidents optional; further work on mean data usage
//
// Revision 1.58  2005/05/04 08:29:28  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; output of simulation speed added
//
// Revision 1.57  2005/02/01 10:08:24  dkrajzew
// performance computation added; got rid of MSNet::Time
//
// Revision 1.56  2004/12/16 12:25:26  dkrajzew
// started a better vss handling
//
// Revision 1.55  2004/11/29 09:21:45  dkrajzew
// detectors debugging
//
// Revision 1.54  2004/11/23 10:20:10  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.53  2004/08/02 12:08:39  dkrajzew
// raw-output extracted; output device handling rechecked
//
// Revision 1.52  2004/07/02 09:55:13  dkrajzew
// MeanData refactored (moved to microsim/output)
//
// Revision 1.51  2004/06/17 13:07:59  dkrajzew
// Polygon visualisation added
//
// Revision 1.50  2004/04/23 12:38:43  dkrajzew
// warnings and errors are now reported to MsgHandler, not cerr
//
// Revision 1.49  2004/04/02 11:36:27  dkrajzew
// "compute or not"-structure added; added two further simulation-wide output
//  (emission-stats and single vehicle trip-infos)
//
// Revision 1.48  2004/02/05 16:39:45  dkrajzew
// removed some memory leaks
//
// Revision 1.47  2003/12/11 06:31:45  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.46  2003/12/02 21:11:50  roessel
// Changes due to renaming of detector files and classes.
//
// Revision 1.45  2003/11/28 14:08:50  roessel
// Changes due to new E3 detectors.
//
// Revision 1.44  2003/11/20 14:45:08  dkrajzew
// dead code removed
//
// Revision 1.43  2003/11/20 13:27:42  dkrajzew
// loading and using of a predefined vehicle color added
//
// Revision 1.42  2003/10/24 15:24:20  roessel
// Changes due to new MSUpdateEachTimestep mechanism.
//
// Revision 1.41  2003/10/22 15:45:51  dkrajzew
// we have to distinct between two teleporter versions now
//
// Revision 1.40  2003/10/20 07:59:43  dkrajzew
// grid lock dissolving by vehicle teleportation added
//
// Revision 1.39  2003/10/06 07:42:36  dkrajzew
// simulate-bug (ending on first step) patched - was due to yet unloaded
//  vehicles
//
// Revision 1.38  2003/10/01 11:31:22  dkrajzew
// globaltime is now always set
//
// Revision 1.37  2003/09/23 14:21:31  dkrajzew
// removed some dead code
//
// Revision 1.36  2003/09/22 11:47:53  roessel
// Portability fixes.
//
// Revision 1.35  2003/09/22 08:50:34  roessel
// Added detection of E2_ZS_Collector objects to simulationStep.
//
// Revision 1.34  2003/08/21 12:54:01  dkrajzew
// cleaned up
//
// Revision 1.33  2003/08/07 10:06:30  roessel
// Added static member myCellLength and some conversion methods from
// cells to meters, steps to seconds etc.. This will be moved out to a
// class MSUnit soon. The cellLength for space-continuous models is 1,
// but not for space-discrete models.
//
// Revision 1.32  2003/08/04 11:45:54  dkrajzew
// missing deletion of traffic light logics on closing a network added;
// vehicle coloring scheme applied
//
// Revision 1.31  2003/07/30 09:11:22  dkrajzew
// a better (correct?) processing of yellow lights added; output corrigued;
// debugging
//
// Revision 1.30  2003/07/22 15:08:28  dkrajzew
// new detector usage applied
//
// Revision 1.29  2003/07/21 18:12:33  roessel
// Comment out MSDetector specific staff.
//
// Revision 1.28  2003/07/21 11:00:37  dkrajzew
// informing the network about vehicles still left within the emitters added
//
// Revision 1.27  2003/07/16 15:28:00  dkrajzew
// MSEmitControl now only simulates lanes which do have vehicles; the edges do
// not go through the lanes, the EdgeControl does
//
// Revision 1.26  2003/06/24 14:49:52  dkrajzew
// unneded members removed (will be replaced, soon)
//
// Revision 1.25  2003/06/24 14:31:58  dkrajzew
// retrieval of current time step added
//
// Revision 1.24  2003/06/19 10:56:55  dkrajzew
// the simulation now also ends when the last vehicle vanishes
//
// Revision 1.23  2003/06/18 11:33:06  dkrajzew
// messaging system added; speedcheck removed; clearing of all structures moved
// from the destructor to an own method (needed for the gui when loading fails)
//
// Revision 1.22  2003/06/06 14:04:05  dkrajzew
// Default building of MSLaneStates removed
//
// Revision 1.21  2003/06/06 10:39:16  dkrajzew
// new usage of MSEventControl applied
//
// Revision 1.20  2003/06/05 16:06:47  dkrajzew
// the initialisation and the ending of a simulation must be available to the
// gui - simulation mathod was split therefore
//
// Revision 1.19  2003/06/05 10:29:54  roessel
// Modified the event-handling in the simulation loop. Added the new
// MSTravelcostDetector< MSLaneState > which will replace the old
// MeanDataDetectors as an example. Needs to be shifted to the proper place
// (where?).
//
// Revision 1.18  2003/05/27 18:36:06  roessel
// Removed parameter MSEventControl* evc from MSNet::init.
// MSEventControl now accessible via the singleton-mechanism.
//
// Revision 1.17  2003/05/25 17:54:10  roessel
// Added a for_each call to MSLaneState::actionBeforeMove and
// MSLaneState::actionAfterMove to MSNet::simulationStep.
//
// Revision 1.16  2003/05/22 11:20:21  roessel
// Refined the mean-data xml-comment.
//
// Revision 1.15  2003/05/21 16:20:44  dkrajzew
// further work detectors
//
// Revision 1.14  2003/05/21 15:15:42  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.13  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output
// debugged; setting and retrieval of some parameter added
//
// Revision 1.12  2003/04/16 10:05:06  dkrajzew
// uah, debugging
//
// Revision 1.11  2003/04/14 08:33:01  dkrajzew
// some further bugs removed
//
// Revision 1.10  2003/04/07 10:29:02  dkrajzew
// usage of globaltime temporary fixed (is still used in
// MSActuatedTrafficLightControl)
//
// Revision 1.9  2003/03/20 16:21:12  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.8  2003/03/04 08:44:33  dkrajzew
// tyni profiler-macros removed
//
// Revision 1.7  2003/03/03 14:56:23  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.6  2003/02/07 10:41:50  dkrajzew
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
// Revision 1.17  2002/09/25 17:14:42  roessel
// MeanData calculation and output implemented.
//
// Revision 1.16  2002/08/07 12:44:52  roessel
// Added #include <cassert>
//
// Revision 1.15  2002/08/06 14:13:27  roessel
// New method preInit() and changes in init().
//
// Revision 1.14  2002/07/31 17:33:01  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.15  2002/07/30 15:17:47  croessel
// Made MSNet-class a singleton-class.
//
// Revision 1.14  2002/07/26 11:44:29  dkrajzew
// Adaptation of past event execution time implemented
//
// Revision 1.13  2002/06/06 17:54:03  croessel
// The member myStep was hidden in the simulation-loop "for( Time myStep;
// ...)", so return of simSeconds() was always 0.
//
// Revision 1.12  2002/05/29 17:06:03  croessel
// Inlined some methods. See the .icc files.
//
// Revision 1.11  2002/05/14 07:53:09  dkrajzew
// Windows eol removed
//
// Revision 1.10  2002/05/14 07:45:21  dkrajzew
// new _SPEEDCHECK functions: all methods in MSNet, computation of UPS and MUPS
//
// Revision 1.9  2002/05/06 06:25:29  dkrajzew
// The output is now directed directly into the output file, no longer via
// a buffer
//
// Revision 1.8  2002/04/25 13:42:11  croessel
// Removed unused variable.
//
// Revision 1.7  2002/04/24 13:06:47  croessel
// Changed signature of void detectCollisions() to void detectCollisions(
// MSNet::Time )
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
// Changed SUMOReal to SUMOReal.
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
// Revision 2.4  2002/03/14 10:42:10  croessel
// << ends removed because we use stringstreams.
// Some curly braces added for if -blocks.
//
// Revision 2.3  2002/03/14 08:09:26  traffic
// Option for no raw output added
//
// Revision 2.2  2002/03/13 16:56:35  croessel
// Changed the simpleOutput to XMLOutput by introducing nested classes
// XMLOut. Output is now indented.
//
// Revision 2.1  2002/03/07 07:55:01  traffic
// implemented the usage of stdout as the default raw output
//
// Revision 2.0  2002/02/14 14:43:18  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.10  2002/02/13 16:30:54  croessel
// Output goes in a file now.
//
// Revision 1.9  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.8  2002/01/16 10:03:35  croessel
// New method "static SUMOReal deltaT()" and member "static SUMOReal myDeltaT"
// added. DeltaT is the length of a timestep in seconds.
//
// Revision 1.7  2001/12/19 16:34:07  croessel
// New std::-files included. Unneeded methods removed.
//
// Revision 1.6  2001/12/13 11:58:14  croessel
// SPEEDCKECK Defines moved/introduced.
//
// Revision 1.5  2001/12/06 13:14:32  traffic
// speed computation added (use -D _SPEEDCHECK)
//
// Revision 1.4  2001/11/15 17:12:13  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.3  2001/11/14 15:47:34  croessel
// Merged the diffs between the .C and .cpp versions. Numerous changes
// in MSLane, MSVehicle and MSJunction.
//
// Revision 1.2  2001/11/14 10:49:06  croessel
// CR-line-end removed.
//
// Revision 1.1  2001/10/24 07:14:02  traffic
// new extension
//
// Revision 1.7  2001/10/23 09:31:18  traffic
// parser bugs removed
//
// Revision 1.5  2001/09/06 15:39:12  croessel
// Added simple text output to simulation-loop.
//
// Revision 1.4  2001/07/25 12:17:46  traffic
// CC problems with make_pair repaired
//
// Revision 1.3  2001/07/16 16:00:52  croessel
// Changed Route-Container type to map<string, Route*>. Added static
// dictionary
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
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#ifdef _SPEEDCHECK
#include <ctime>
#endif

#include <iostream>
#include <sstream>
#include <typeinfo>
#include <algorithm>
#include <cassert>
#include <vector>
#include <utils/common/UtilExceptions.h>
#include "MSNet.h"
#include "MSEdgeControl.h"
#include "MSJunctionControl.h"
#include "MSEmitControl.h"
#include "MSEventControl.h"
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSJunctionLogic.h"
#include "MSLane.h"
#include "MSVehicleTransfer.h"
#include "MSRoute.h"
#include "MSRouteLoaderControl.h"
#include "traffic_lights/MSTLLogicControl.h"
#include "MSVehicleControl.h"
#include "trigger/MSTrigger.h"
#include "MSCORN.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/MSVehicleTransfer.h>
#include "traffic_lights/MSTrafficLightLogic.h"
#include <microsim/output/MSDetectorHaltingContainerWrapper.h>
#include <microsim/output/MSTDDetectorInterface.h>
#include <microsim/output/MSDetectorOccupancyCorrection.h>
#include <utils/shapes/Polygon2D.h>
#include <utils/shapes/ShapeContainer.h>
#include "output/meandata/MSMeanData_Net.h"
#include "output/MSXMLRawOut.h"
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/SysUtils.h>
#include "trigger/MSTriggerControl.h"
#include "MSGlobals.h"
#include "MSSaveState.h"
#include "MSDebugHelper.h" // !!!
#include "MSRouteHandler.h"
#include "MSRouteLoader.h"

#include <ctime>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member defintions
 * ======================================================================= */
MSNet* MSNet::myInstance = 0;
//MSNet::DictType MSNet::myDict;
SUMOReal MSNet::myDeltaT = 1;
SUMOReal MSNet::myCellLength = 1;
/*
SUMOTime MSNet::globaltime;

#ifdef ABS_DEBUG
SUMOTime MSNet::searchedtime = 18193;
std::string MSNet::searched1 = "107";
std::string MSNet::searched2 = "2858";
std::string MSNet::searchedJunction = "536";
#endif
*/

/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSNet*
MSNet::getInstance( void )
{
    if ( myInstance != 0 ) {
        return myInstance;
    }
    assert( false );
    return 0;
}

/*
void
MSNet::preInitMSNet(SUMOTime startTimeStep,
                    MSVehicleControl *vc)
{
    myInstance = new MSNet();
    preInit(startTimeStep, vc);
}
*/


MSNet::MSNet(SUMOTime startTimeStep, SUMOTime stopTimeStep)
{
    MSCORN::init();
//    MSDetectorControl::createDictionaries();
    MSVehicleTransfer::setInstance(new MSVehicleTransfer());
    myStep = startTimeStep;
    myEmitter = new MSEmitControl("");
    myVehicleControl = new MSVehicleControl();
    myDetectorControl = new MSDetectorControl();
    myEdges = 0;//new MSEdgeControl();
    myJunctions = 0;//new MSJunctionControl();
    myRouteLoaders = 0;//new MSRouteLoaderControl();
    myLogics = 0;//new MSTLLogicControl();
    myTriggerControl = new MSTriggerControl();
    myShapeContainer = 0;//new ShapeContainer();
    myInstance = this;
}



MSNet::MSNet(SUMOTime startTimeStep, SUMOTime stopTimeStep,
             MSVehicleControl *vc)
{
    MSCORN::init();
//    MSDetectorControl::createDictionaries();
    MSVehicleTransfer::setInstance(new MSVehicleTransfer());
    myStep = startTimeStep;
    myEmitter = new MSEmitControl("");
    myVehicleControl = vc;
    myDetectorControl = new MSDetectorControl();
    myEdges = 0;//new MSEdgeControl();
    myJunctions = 0;//new MSJunctionControl();
    myRouteLoaders = 0;//new MSRouteLoaderControl();
    myLogics = 0;//new MSTLLogicControl();
    myTriggerControl = new MSTriggerControl();
    myShapeContainer = new ShapeContainer();

    myInstance = this;
}




void
MSNet::closeBuilding(MSEdgeControl *edges, MSJunctionControl *junctions,
                     MSRouteLoaderControl *routeLoaders,
                     MSTLLogicControl *tlc, ShapeContainer *sc,
                     std::vector<OutputDevice*> streams,
                     const MSMeanData_Net_Cont &meanData,
                     TimeVector stateDumpTimes,
                     std::string stateDumpFiles)
{
    myEdges = edges;//new MSEdgeControl();
    myJunctions = junctions;//new MSJunctionControl();
    myRouteLoaders = routeLoaders;//new MSRouteLoaderControl();
    myLogics = tlc;//new MSTLLogicControl();
    MSCORN::setTripDurationsOutput(streams[OS_TRIPDURATIONS]);
	MSCORN::setVehicleRouteOutput(streams[OS_VEHROUTE]);
    myOutputStreams = streams;
    myMeanData = meanData;
    myShapeContainer = sc;
    // we may add it before the network is loaded
    if(myEdges!=0) {
        myEdges->insertMeanData(myMeanData.size());
    }

    myStateDumpTimes = stateDumpTimes;
    myStateDumpFiles = stateDumpFiles;

    // set requests/responses
    MSJunction::postloadInitContainer();
    // make detectors accessable
    //myDetectorControl->setDictionariesFindMode();
    /*
    // initialise lane mean data and readers

    for(PreStartVector::iterator i=myPreStartInitialiseItems.begin();
        i!=myPreStartInitialiseItems.end(); i++) {
        (*i)->init(*this);
    }
    */
}


MSNet::~MSNet()
{
    // delete controls
//?    MSDetectorControl::deleteDictionariesAndContents();
    delete myJunctions;
    delete myDetectorControl;
    // delete mean data
    for(MSMeanData_Net_Cont::iterator i1=myMeanData.begin(); i1!=myMeanData.end(); ++i1) {
        delete *i1;
    }
    // close outputs
    for(size_t i2=0; i2<OS_MAX; i2++) {
        delete myOutputStreams[i2];
    }
    myMeanData.clear();
    delete myEdges;
    delete myEmitter;
    delete myLogics;
    delete myRouteLoaders;
    delete myVehicleControl;
    delete myShapeContainer;
    /*
    for(PolyDic::iterator j=poly_dic.begin(); j != poly_dic.end(); j++){
       delete (*j).second;
    }
    poly_dic.clear();
    */
    delete myTriggerControl;
    clearAll();
}


bool
MSNet::simulate( SUMOTime start, SUMOTime stop )
{
    initialiseSimulation();
    // the simulation loop
    myStep = start;
    try {
        do {
            preSimStepOutput();
            simulationStep(start, myStep);
            postSimStepOutput();
            myStep++;
        } while( myStep<=stop&&!myVehicleControl->haveAllVehiclesQuit());
        if(myStep>stop) {
            WRITE_MESSAGE("Simulation End: The final simulation step has been reached.");
        } else {
            WRITE_MESSAGE("Simulation End: All vehicles have left the simulation.");
        }
    } catch (ProcessError &) {
        WRITE_MESSAGE("Simulation End: An error occured (see log).");
    }
    // exit simulation loop
    closeSimulation(start, stop);
    return true;
}


void
MSNet::initialiseSimulation()
{
    // prepare the "netstate" output and print the first line
    if ( myOutputStreams[OS_NETSTATE]!=0 ) {
        myOutputStreams[OS_NETSTATE]->getOStream()
            << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl
            << "<sumo-netstate>" << endl;
    }
    // ... the same for the vehicle emission state
    if ( myOutputStreams[OS_EMISSIONS]!=0 ) {
        myOutputStreams[OS_EMISSIONS]->getOStream()
            << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl
            << "<emissions>" << endl;
        MSCORN::setWished(MSCORN::CORN_OUT_EMISSIONS);
    }
    // ... the same for the vehicle trip durations
    if ( myOutputStreams[OS_TRIPDURATIONS]!=0 ) {
        myOutputStreams[OS_TRIPDURATIONS]->getOStream()
            << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl
            << "<tripinfos>" << endl;
        MSCORN::setWished(MSCORN::CORN_OUT_TRIPDURATIONS);
    }
    // ... the same for the vehicle route information
    if ( myOutputStreams[OS_VEHROUTE]!=0 ) {
        myOutputStreams[OS_VEHROUTE]->getOStream()
            << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl
            << "<vehicleroutes>" << endl;
        MSCORN::setWished(MSCORN::CORN_OUT_VEHROUTES);
    }
}


void
MSNet::closeSimulation(SUMOTime start, SUMOTime stop)
{
    // print the last line of the "netstate" output
    if ( myOutputStreams[OS_NETSTATE]!=0 ) {
        myOutputStreams[OS_NETSTATE]->getOStream() << "</sumo-netstate>" << endl;
    }
    // ... the same for the vehicle emission state
    if ( myOutputStreams[OS_EMISSIONS]!=0 ) {
        myOutputStreams[OS_EMISSIONS]->getOStream() << "</emissions>" << endl;
    }
    // ... the same for the vehicle trip information
    if ( myOutputStreams[OS_TRIPDURATIONS]!=0 ) {
        myOutputStreams[OS_TRIPDURATIONS]->getOStream() << "</tripinfos>" << endl;
    }
    // ... the same for the vehicle trip information
    if ( myOutputStreams[OS_VEHROUTE]!=0 ) {
        myOutputStreams[OS_VEHROUTE]->getOStream() << "</vehicleroutes>" << endl;
    }
    if(myLogExecutionTime!=0&&mySimDuration!=0) {
        cout << "Performance: " << endl
            << " Duration: " << mySimDuration << "ms" << endl
            << " Real time factor: " << ((SUMOReal)(stop-start)*1000./(SUMOReal)mySimDuration) << endl
            << " UPS: " << ((SUMOReal) myVehiclesMoved / (SUMOReal) mySimDuration * 1000.) << endl;
    }
}


void
MSNet::simulationStep( SUMOTime start, SUMOTime step )
{
    myStep = step;
    debug_globaltime = step;
    // execute beginOfTimestepEvents
    if(myLogExecutionTime) {
        mySimStepBegin = SysUtils::getCurrentMillis();
    }
    MSEventControl::getBeginOfTimestepEvents()->execute(myStep);

    MSLaneState::actionsBeforeMoveAndEmit();

    // load routes
    myEmitter->moveFrom(myRouteLoaders->loadNext(step));
    // emit Vehicles
    size_t emittedVehNo = myEmitter->emitVehicles(myStep);
    myVehicleControl->vehiclesEmitted(emittedVehNo);
    if(MSGlobals::gCheck4Accidents) {
        myEdges->detectCollisions( step );
    }
    MSVehicleTransfer::getInstance()->checkEmissions(myStep);

    if(MSGlobals::gCheck4Accidents) {
        myEdges->detectCollisions( step );
    }
    myJunctions->resetRequests();

    // move Vehicles
        // move vehicles which do not interact with the lane end
    myEdges->moveNonCritical();
        // precompute possible positions for vehicles that do interact with
        // their lane's end
    myEdges->moveCritical();

    // set information about which vehicles may drive at all
    myLogics->maskRedLinks();
    // check the right-of-way for all junctions
    myJunctions->setAllowed();
    // set information which vehicles should decelerate
    myLogics->maskYellowLinks();

    // move vehicles which do interact with their lane's end
    //  (it is now known whether they may drive
    myEdges->moveFirst();
    if(MSGlobals::gCheck4Accidents) {
        myEdges->detectCollisions( step );
    }
    // detect

    MSLaneState::actionsAfterMoveAndEmit();

    MSUpdateEachTimestepContainer<
        DetectorContainer::UpdateHaltings >::getInstance()->updateAll();
    MSUpdateEachTimestepContainer<
        DetectorContainer::UpdateE3Haltings >::getInstance()->updateAll();
    MSUpdateEachTimestepContainer<
        Detector::UpdateE2Detectors >::getInstance()->updateAll();
    MSUpdateEachTimestepContainer<
        Detector::UpdateOccupancyCorrections >::getInstance()->updateAll();

    // Vehicles change Lanes (maybe)
    myEdges->changeLanes();

    if(MSGlobals::gCheck4Accidents) {
        myEdges->detectCollisions( step );
    }
    // Check if mean-lane-data is due
    long ve = SysUtils::getCurrentMillis();
    writeOutput();
    // execute endOfTimestepEvents
    MSEventControl::getEndOfTimestepEvents()->execute(myStep);

    // check state dumps
    if(find(myStateDumpTimes.begin(), myStateDumpTimes.end(), myStep)!=myStateDumpTimes.end()) {
        string name = myStateDumpFiles + '_' + toString(myStep) + ".bin";
        ofstream strm(name.c_str(), fstream::out|fstream::binary);
        saveState(strm, (long) 0xffffffff);
    }

    if(myLogExecutionTime) {
        mySimStepEnd = SysUtils::getCurrentMillis();
        mySimStepDuration = mySimStepEnd - mySimStepBegin;
        mySimDuration += mySimStepDuration;
        myVehiclesMoved += myVehicleControl->getRunningVehicleNo();
    }
}


void
MSNet::clearAll()
{
    // clear container
    MSEdge::clear();
//    MSEdgeControl::clear();
    MSEmitControl::clear();
    MSEventControl::clear();
    MSJunction::clear();
    MSJunctionControl::clear();
    MSJunctionLogic::clear();
    MSLane::clear();
    MSVehicle::clear();
    MSVehicleType::clear();
    MSRoute::clear();
    MSTrafficLightLogic::clear();
//    NamedObjectContSingleton<MSTrigger*>::getInstance().clear();
//    clear();
    delete MSVehicleTransfer::getInstance();
}


unsigned
MSNet::getNDumpIntervalls( void )
{
    return myMeanData.size();
}

/*
void
MSNet::addPreStartInitialisedItem(PreStartInitialised *preinit)
{
    myPreStartInitialiseItems.push_back(preinit);
}
*/


SUMOTime
MSNet::getCurrentTimeStep() const
{
    return myStep;
}


MSVehicleControl &
MSNet::getVehicleControl() const
{
    return *myVehicleControl;
}


void
MSNet::writeOutput()
{
    // netstate output.
    if ( myOutputStreams[OS_NETSTATE]!=0 ) {
        MSXMLRawOut::write(myOutputStreams[OS_NETSTATE], *myEdges, myStep, 3);
    }
    // emission output
    if ( myOutputStreams[OS_EMISSIONS]!=0 ) {
        myOutputStreams[OS_EMISSIONS]->getOStream()
            << "    <emission-state id=\"" << myStep << "\" "
            << "loaded=\"" << myVehicleControl->getLoadedVehicleNo() << "\" "
            << "emitted=\"" << myVehicleControl->getEmittedVehicleNo() << "\" "
            << "running=\"" << myVehicleControl->getRunningVehicleNo() << "\" "
            << "waiting=\"" << myEmitter->getWaitingVehicleNo() << "\" "
            << "ended=\"" << myVehicleControl->getEndedVehicleNo() << "\" "
            << "meanWaitingTime=\"" << myVehicleControl->getMeanWaitingTime() << "\" "
            << "meanTravelTime=\"" << myVehicleControl->getMeanTravelTime() << "\" ";
        if(myLogExecutionTime) {
            myOutputStreams[OS_EMISSIONS]->getOStream()
                << "duration=\"" << mySimStepDuration << "\" ";
        }
        myOutputStreams[OS_EMISSIONS]->getOStream()
            << "/>" << endl;
    }
}

/*
bool
MSNet::addPoly(const std::string &name, const std::string &type,
               const RGBColor &color)
{
    if(MSNet::poly_dic[name] != 0) {
        return false;
    }
    Polygon2D *poly = new Polygon2D(name, type, color);
    MSNet::poly_dic[name] = poly;
    return true;
}
*/
bool
MSNet::haveAllVehiclesQuit()
{
    return myVehicleControl->haveAllVehiclesQuit();
}


void
MSNet::addMeanData(MSMeanData_Net *newMeanData)
{
    myMeanData.push_back(newMeanData);
    // we may add it before the network is loaded
    if(myEdges!=0) {
        myEdges->insertMeanData(1);
    }
}


size_t
MSNet::getMeanDataSize() const
{
    return myMeanData.size();
}


MSEdgeControl &
MSNet::getEdgeControl()
{
    return *myEdges;
}


MSDetectorControl &
MSNet::getDetectorControl()
{
    return *myDetectorControl;
}


MSTriggerControl &
MSNet::getTriggerControl()
{
    return *myTriggerControl;
}



/*
void
MSNet::addTrigger(MSTrigger *t)
{
    myTrigger.push_back(t);
}


MSTrigger *
MSNet::getTrigger(const std::string &id)
{
    for(TriggerVector::iterator i=myTrigger.begin(); i!=myTrigger.end(); ++i) {
        if((*i)->getID()==id) {
            return (*i);
        }
    }
    return 0;
}
*/

long
MSNet::getSimStepDurationInMillis() const
{
    return mySimStepDuration;
}


void
MSNet::saveState(std::ostream &os, long what)
{
    myVehicleControl->saveState(os, what);
    myEdges->saveState(os, what);
        /*
    if((what&(long) SAVESTATE_EDGES)!=0) myEdges->saveState(os);
    if((what&(long) SAVESTATE_EMITTER)!=0) myEmitter->saveState(os);
    if((what&(long) SAVESTATE_LOGICS)!=0) myLogics->saveState(os);
    if((what&(long) SAVESTATE_ROUTES)!=0) myRouteLoaders->saveState(os);
    if((what&(long) SAVESTATE_VEHICLES)!=0) myVehicleControl->saveState(os);
    */
}


void
MSNet::loadState(BinaryInputDevice &bis, long what)
{
    myVehicleControl->loadState(bis, what);
    myEdges->loadState(bis, what);
        /*
    if((what&(long) SAVESTATE_EDGES)!=0) myEdges->saveState(os);
    if((what&(long) SAVESTATE_EMITTER)!=0) myEmitter->saveState(os);
    if((what&(long) SAVESTATE_LOGICS)!=0) myLogics->saveState(os);
    if((what&(long) SAVESTATE_ROUTES)!=0) myRouteLoaders->saveState(os);
    if((what&(long) SAVESTATE_VEHICLES)!=0) myVehicleControl->saveState(os);
    */
}


MSRouteLoader *
MSNet::buildRouteLoader(const std::string &file)
{
    return new MSRouteLoader(*this, new MSRouteHandler(file, false));
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#ifdef DISABLE_INLINE
#include "MSNet.icc"
#endif

// Local Variables:
// mode:C++
// End:
