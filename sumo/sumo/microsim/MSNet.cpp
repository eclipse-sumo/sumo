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
// Revision 1.1  2002/04/08 07:21:23  traffic
// Initial revision
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
// New method "static float deltaT()" and member "static float myDeltaT"
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

#ifdef _SPEEDCHECK
extern long novehicles;
extern time_t begin;
extern time_t end;
#endif

#include <iostream>
//#include <fstream>
#include <sstream>
#include <typeinfo>
#include "MSNet.h"
#include "MSEdgeControl.h"
#include "MSJunctionControl.h"
#include "MSEmitControl.h"
#include "MSEventControl.h"
#include "MSPersonControl.h"
#include "MSPerson.h"
#include "MSEdge.h"

using namespace std;


// Init static member.
MSNet::DictType MSNet::myDict;
MSNet::RouteDict MSNet::myRoutes;
float MSNet::myDeltaT = 1;

MSNet::MSNet(string id, MSEdgeControl* ec,
             MSJunctionControl* jc,  
             MSEmitControl* emc,
             MSEventControl* evc,
             MSPersonControl* wpc) :
    myID(id),
    myEdges(ec), 
    myJunctions(jc),
    myEmitter(emc), 
    myEvents(evc),
    myPersons(wpc)
{
}


MSNet::~MSNet()
{
}


void 
MSNet::addVehicles(MSEmitControl *cont) {
    if(cont==0) return;
    myEmitter->add(cont);
}


bool
MSNet::simulate( ostream *craw, Time start, Time stop )
{
    // prepare the "raw" output and print the first line
    bool usingFile = false;
    ostringstream header;
    header << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl 
           << "<sumo-results>" << endl; 
    if ( craw ) {
        (*craw) << header.str();
    }
        
    // the simulation loop
    for (Time step = start; step <= stop; ++step) {
    
#ifdef _SPEEDCHECK
        if(step==100) {
            time(&begin);
            novehicles = 0;
        }
        if(step==10000) {
            time(&end);
            int bla = 0;
        }
#endif    

        // process the persons which are no longer waiting or walking
        processWaitingPersons(step);

        // emit Vehicles
        myEmitter->emitVehicles(step);
        myEdges->detectCollisions();
          
        // execute Events
        myEvents->execute(step);
          
        // load waiting persons and unload the persons which vehicle 
        // route ends here
        myEdges->loadPersons();
        myEdges->unloadPersons(this, step);

        // move Vehicles
        myEdges->moveExceptFirst();
        myJunctions->moveFirstVehicles();
        myEdges->detectCollisions();
          
        // Vehicles change Lanes (maybe)
        myEdges->changeLanes();
        myEdges->detectCollisions();

        // simple output.     
        ostringstream XMLOut;
        XMLOut << "    <timestep id=\"" << step << "\">" << endl;
        XMLOut << MSEdgeControl::XMLOut( *myEdges, 8 );
        XMLOut << "    </timestep>" << endl;
        if ( craw ) { 
            (*craw) << XMLOut.str();
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

float
MSNet::deltaT()
{
    return myDeltaT;
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "MSNet.iC"
//#endif

// Local Variables:
// mode:C++
// End:





