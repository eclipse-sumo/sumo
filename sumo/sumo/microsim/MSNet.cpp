/***************************************************************************
                          MSNet.C  -  We will simulate on this
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
// Revision 1.10  2002/05/14 07:45:21  dkrajzew
// new _SPEEDCHECK functions: all methods in MSNet, computation of UPS and MUPS
//
// Revision 1.9  2002/05/06 06:25:29  dkrajzew
// The output is now directed directly into the output file, no longer via a buffer
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#ifdef _SPEEDCHECK
#include <ctime>
#endif

#include <iostream>
//#include <fstream>
#include <sstream>
#include <typeinfo>
#include <algorithm>
#include "MSNet.h"
#include "MSEdgeControl.h"
#include "MSJunctionControl.h"
#include "MSEmitControl.h"
#include "MSEventControl.h"
#include "MSPersonControl.h"
#include "MSPerson.h"
#include "MSEdge.h"
#include "MSDetector.h"

using namespace std;


// Init static member.
MSNet::DictType MSNet::myDict;
MSNet::RouteDict MSNet::myRoutes;
double MSNet::myDeltaT = 1;

#ifdef _DEBUG
MSNet::Time MSNet::globaltime;
#endif

#ifdef _SPEEDCHECK
long MSNet::noVehicles;
time_t MSNet::begin;
time_t MSNet::end;
#endif

MSNet::MSNet(string id, MSEdgeControl* ec,
             MSJunctionControl* jc,
             MSEmitControl* emc,
             MSEventControl* evc,
             MSPersonControl* wpc,
             DetectorCont* detectors ) :
    myID(id),
    myEdges(ec),
    myJunctions(jc),
    myEmitter(emc),
    myEvents(evc),
    myPersons(wpc),
    myStep(0),
    myDetectors( detectors )
{
}


MSNet::~MSNet()
{
}


bool
MSNet::simulate( ostream *craw, Time start, Time stop )
{
    // prepare the "raw" output and print the first line
    ostringstream header;
    header << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl
           << "<sumo-results>" << endl;
    if ( craw ) {
        (*craw) << header.str();
    }

    // the simulation loop
    for (Time myStep = start; myStep <= stop; ++myStep) {
#ifdef _DEBUG
        globaltime = myStep;
#endif

#ifdef _SPEEDCHECK
        if(myStep==1) {
            time(&begin);
            noVehicles = 0;
        }
        if(myStep==stop) {
            time(&end);
            double ups = ((double) noVehicles / (double) (end-begin));
            double mups = ups / 1000000.0;
            cout << noVehicles << " vehicles in " << (end-begin) << " sec" << endl;
            cout << ups << "UPS; " << mups << "MUPS" << endl;
        }
#endif

        // process the persons which are no longer waiting or walking
        processWaitingPersons(myStep);

        // emit Vehicles
        myEmitter->emitVehicles(myStep);
        myEdges->detectCollisions( myStep );

        // execute Events
        myEvents->execute(myStep);

        // load waiting persons and unload the persons which vehicle
        // route ends here
        myEdges->loadPersons();
        myEdges->unloadPersons(this, myStep);

        // move Vehicles
        myEdges->moveExceptFirst();
        myJunctions->moveFirstVehicles();
        myEdges->detectCollisions( myStep );

        // Let's detect.
        for( DetectorCont::iterator detec = myDetectors->begin();
             detec != myDetectors->end(); ++detec ) {

            ( *detec )->sample( simSeconds() );
        }

        // Vehicles change Lanes (maybe)
        myEdges->changeLanes();
        myEdges->detectCollisions( myStep );

        // simple output.
        if ( craw ) {
            (*craw) << "    <timestep id=\"" << myStep << "\">" << endl;
            (*craw) << MSEdgeControl::XMLOut( *myEdges, 8 );
            (*craw) << "    </timestep>" << endl;
        }
    }

    // print the last line of the "raw" output
    ostringstream footer;
    footer <<  "</sumo-results>" << endl;
    if ( craw ) {
        (*craw) << footer.str();
    }
    // exit simulation loop
    return true;
}

void
MSNet::processWaitingPersons(unsigned int time) {
    PersonCont *persons = myPersons->getPersons(time);
    if(persons==0) return;
    for(PersonCont::iterator i=persons->begin(); i!=persons->end(); i++)
        (*i)->proceed(this, time);
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


bool
MSNet::routeDict(string id, Route* route)
{
    RouteDict::iterator it = myRoutes.find(id);
    if (it == myRoutes.end()) {
        // id not in myDict.
        myRoutes.insert(RouteDict::value_type(id, route));
        return true;
    }
    return false;
}


const MSNet::Route*
MSNet::routeDict(string id)
{
    RouteDict::iterator it = myRoutes.find(id);
    if (it == myRoutes.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}

double
MSNet::deltaT()
{
    return myDeltaT;
}

double
MSNet::simSeconds()
{
    return static_cast< double >( myStep ) * myDeltaT;
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "MSNet.iC"
//#endif

// Local Variables:
// mode:C++
// End:





