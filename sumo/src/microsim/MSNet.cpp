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
// simulate-bug (ending on first step) patched - was due to yet unloaded vehicles
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
// New method "static double deltaT()" and member "static double myDeltaT"
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
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#ifdef _SPEEDCHECK
#include <ctime>
#endif

#include <iostream>
#include <sstream>
#include <typeinfo>
#include <algorithm>
#include <cassert>
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
#include "MSTLLogicControl.h"
#include <utils/common/MsgHandler.h>
#include <helpers/PreStartInitialised.h>
#include <utils/convert/ToString.h>
#include "helpers/SingletonDictionary.h"
#include "MSLaneState.h"
#include "MSTravelcostDetector.h"
#include <microsim/MSDetectorSubSys.h>
#include <microsim/MSVehicleTransfer.h>
#include "MSTrafficLightLogic.h"
#include "MSDetectorHaltingContainerWrapper.h"
#include "MSE2DetectorInterface.h"
#include "MSDetectorOccupancyCorrection.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member defintions
 * ======================================================================= */
MSNet* MSNet::myInstance = 0;
MSNet::DictType MSNet::myDict;
double MSNet::myDeltaT = 1;
double MSNet::myCellLength = 1;

MSNet::Time MSNet::globaltime;

#ifdef ABS_DEBUG
MSNet::Time MSNet::searchedtime = 0;
std::string MSNet::searched1 = "Rand9";
std::string MSNet::searched2 = "715a0";
std::string MSNet::searchedJunction = "536";
#endif


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSNet::MSNet()
    : myLoadedVehNo(0), myEmittedVehNo(0), myRunningVehNo(0), myEndedVehNo(0)
{
}


MSNet*
MSNet::getInstance( void )
{
    if ( myInstance != 0 ) {
        return myInstance;
    }
    assert( false );
    return 0;
}

void
MSNet::preInit( MSVehicleTransfer *vt,
                MSNet::Time startTimeStep, TimeVector dumpMeanDataIntervalls,
                std::string baseNameDumpFiles)
{
    assert(vt!=0);
    myInstance = new MSNet();
    MSVehicleTransfer::setInstance(vt);
	myInstance->myLoadedVehNo = 0;
	myInstance->myEmittedVehNo = 0;
	myInstance->myRunningVehNo = 0;
	myInstance->myEndedVehNo = 0;

    myInstance->myStep = startTimeStep;
    initMeanData( dumpMeanDataIntervalls, baseNameDumpFiles/*, withGUI*/ );
	myInstance->myEmitter = new MSEmitControl("");
    MSDetectorSubSys::createDictionaries();
}


void
MSNet::initMeanData( TimeVector dumpMeanDataIntervalls,
                    std::string baseNameDumpFiles/*, bool withGUI*/)
{
    if ( dumpMeanDataIntervalls.size() > 0 ) {

        sort( dumpMeanDataIntervalls.begin(),
              dumpMeanDataIntervalls.end() );
        vector< Time >::iterator newEnd =
            unique( dumpMeanDataIntervalls.begin(),
                    dumpMeanDataIntervalls.end() );

        if ( newEnd != dumpMeanDataIntervalls.end() ) {

            cerr << "MSNet::MSNet(): Removed duplicate dump-intervalls"
                 << endl;
            dumpMeanDataIntervalls.erase(
                newEnd, dumpMeanDataIntervalls.end() );
        }

        // Prepare MeanData container, e.g. assign intervals and open files.
        for ( std::vector< Time >::iterator it =
                  dumpMeanDataIntervalls.begin();
              it != dumpMeanDataIntervalls.end(); ++it ) {

            string fileName   = baseNameDumpFiles + "_" + toString( *it ) +
                string(".xml");
            ofstream* filePtr = new ofstream( fileName.c_str() );
            if( *filePtr==0 ) {
                MsgHandler::getErrorInstance()->inform(
                    string("The following file containing aggregated values could not been build:\n")
                    + fileName);
                throw ProcessError();
            }
            // Write xml-comment
            *filePtr << "<!--\n"
                "- noVehContrib is the number of vehicles have been on the lane for\n"
                "  at least one timestep during the current intervall.\n"
                "  They contribute to speed, speedsquare and density.\n"
                "  They may not have passed the entire lane.\n"
                "- noVehEntireLane is the number of vehicles that have passed the\n"
                "  entire lane and left the lane during the current intervall. They\n"
                "  may have started their journey on this lane in a previous intervall.\n"
                "  Only those vehicles contribute to traveltime. \n"
                "- noVehEntered is the number of vehicles that entered this lane\n"
                "  during the current intervall either by move, emit or lanechange.\n"
                "  Note that noVehEntered might be high if vehicles are emitted on\n"
                "  this lane.\n"
                "- noVehLeft is the number of vehicles that left this lane during\n"
                "  the current intervall by move.\n"
                "- traveltime [s]\n"
                "  If noVehContrib==0 then traveltime is set to laneLength / laneMaxSpeed. \n"
                "  If noVehContrib!=0 && noVehEntireLane==0 then traveltime is set to\n"
                "  laneLength / speed.\n"
                "  Else traveltime is calculated from the data of the vehicles that\n"
                "  passed the entire lane.\n"
                "- speed [m/s]\n"
                "  If noVehContrib==0 then speed is set to laneMaxSpeed.\n"
                "- speedsquare [(m/s)^2]\n"
                "  If noVehContrib==0 then speedsquare is set to -1.\n"
                "- density [veh/km]\n"
                "  If noVehContrib==0 then density is set to 0.\n"
                "-->\n" << endl;

            MSNet::myInstance->myMeanData.push_back(
                new MeanData( *it, filePtr ) );
        }
    }
}

void
MSNet::init( string id, MSEdgeControl* ec,
             MSJunctionControl* jc,
             MSRouteLoaderControl *rlc,
             MSTLLogicControl *tlc)
{
    myInstance->myID           = id;
    myInstance->myEdges        = ec;
    myInstance->myJunctions    = jc;
    myInstance->myRouteLoaders = rlc;
    myInstance->myLogics       = tlc;
    MSDetectorSubSys::setDictionariesFindMode();
}


MSNet::~MSNet()
{
    clearAll();
    // close the net statistics
    for( std::vector< MeanData* >::iterator i1=myMeanData.begin();
         i1!=myMeanData.end(); i1++) {
        delete (*i1);
    }
    myMeanData.clear();
    // delete controls
    delete myEdges;
    delete myJunctions;
    delete myEmitter;
    delete myLogics;
    delete myRouteLoaders;
    MSDetectorSubSys::deleteDictionariesAndContents();
}


bool
MSNet::simulate( ostream *craw, Time start, Time stop )
{
    initialiseSimulation(craw);
    // the simulation loop
    myStep = start;
    do {
		cout << myStep << (char) 13;
        simulationStep(craw, start, myStep);
        myStep++;
    }   while( myStep <= stop&&myLoadedVehNo>myEndedVehNo);
    // exit simulation loop
    closeSimulation(craw);
    return true;
}



void
MSNet::initialiseSimulation(std::ostream *craw)
{
    // prepare the "raw" output and print the first line
    if ( craw ) {
        (*craw) << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl
            << "<sumo-results>" << endl;
    }
}

void
MSNet::closeSimulation(std::ostream *craw)
{
	cout << endl;
    // print the last line of the "raw" output
    if ( craw ) {
        (*craw) << "</sumo-results>" << endl;
    }
}




void
MSNet::simulationStep( ostream *craw, Time start, Time step )
{
    myStep = step;
    globaltime = myStep;
#ifdef ABS_DEBUG
    globaltime = myStep;
#endif
    // execute beginOfTimestepEvents
    MSEventControl::getBeginOfTimestepEvents()->execute(myStep);

    MSLaneState::actionsBeforeMoveAndEmit();

    // load routes
    myEmitter->moveFrom(myRouteLoaders->loadNext(step));

    // emit Vehicles
    size_t emittedVehNo = myEmitter->emitVehicles(myStep);
	myEmittedVehNo += emittedVehNo;
	myRunningVehNo += emittedVehNo;
    myEdges->detectCollisions( myStep );
    MSVehicleTransfer::getInstance()->checkEmissions(myStep);

    myEdges->detectCollisions( myStep );
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
    myEdges->detectCollisions( myStep );


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

    myEdges->detectCollisions( myStep );

    // Check if mean-lane-data is due
    unsigned passedSteps = myStep - start + 1;
    for ( unsigned i = 0; i < myMeanData.size(); ++i ) {

        assert(myMeanData.size()>i);
        Time interval = myMeanData[ i ]->interval;
        if ( passedSteps % interval == 0 ) {

            *(myMeanData[ i ]->file)
                << "<interval begin=\""
                << passedSteps - interval + start
                << "\" end=\"" << myStep << "\">\n"
                << MSEdgeControl::MeanData( *myEdges, i , interval )
                << "</interval>\n";
        }
    }

    // raw output.
    if ( craw ) {
        (*craw) << "    <timestep id=\"" << myStep << "\">" << endl;
        (*craw) << MSEdgeControl::XMLOut( *myEdges, 8 );
        (*craw) << "    </timestep>" << endl;
    }

    // execute endOfTimestepEvents
    MSEventControl::getEndOfTimestepEvents()->execute(myStep);

}


bool
MSNet::dictionary(string id, MSNet* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}


MSNet*
MSNet::dictionary(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


void
MSNet::clearAll()
{
    // clear container
    MSEdge::clear();
    MSEdgeControl::clear();
    MSEmitControl::clear();
    MSEventControl::clear();
    MSJunction::clear();
    MSJunctionControl::clear();
    MSJunctionLogic::clear();
    MSLane::clear();
    MSNet::clear();
    MSVehicle::clear();
    MSVehicleType::clear();
    MSRoute::clear();
    MSTrafficLightLogic::clear();
    clear();
    delete MSVehicleTransfer::getInstance();
}


void
MSNet::clear()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}


unsigned
MSNet::getNDumpIntervalls( void )
{
    return myMeanData.size();
}


void
MSNet::addPreStartInitialisedItem(PreStartInitialised *preinit)
{
    myPreStartInitialiseItems.push_back(preinit);
}


void
MSNet::preStartInit()
{
    for(PreStartVector::iterator i=myPreStartInitialiseItems.begin();
        i!=myPreStartInitialiseItems.end(); i++) {
        (*i)->init(*this);
    }
}


MSVehicle *
MSNet::buildNewVehicle( std::string id, MSRoute* route,
                         MSNet::Time departTime, const MSVehicleType* type,
                         int repNo, int repOffset, const RGBColor &col)
{
    size_t noIntervals = getNDumpIntervalls();
	myLoadedVehNo++;
    return new MSVehicle(id, route, departTime, type, noIntervals,
        repNo, repOffset);
}


void
MSNet::vehicleHasLeft(const std::string &)
{
    assert(myRunningVehNo>0);
	myRunningVehNo--;
	myEndedVehNo++;
}


size_t
MSNet::getLoadedVehicleNo() const
{
    return myLoadedVehNo;
}


size_t
MSNet::getEndedVehicleNo() const
{
    return myEndedVehNo;
}


size_t
MSNet::getRunningVehicleNo() const
{
    return myRunningVehNo;
}


size_t
MSNet::getEmittedVehicleNo() const
{
    return myEmittedVehNo;
}


MSNet::Time
MSNet::getCurrentTimeStep() const
{
    return myStep;
}


void
MSNet::newUnbuildVehicleLoaded()
{
    myLoadedVehNo++;
}


void
MSNet::newUnbuildVehicleBuild()
{
    myLoadedVehNo--;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#ifdef DISABLE_INLINE
#include "MSNet.icc"
#endif

// Local Variables:
// mode:C++
// End:
