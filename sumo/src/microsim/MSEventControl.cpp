/***************************************************************************
                          MSEventControl.cpp  -  Coordinates
                          time-dependant events
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

namespace
{
    const char rcsid[] =
    "$Id$";
}

// $Log$
// Revision 1.5  2003/05/27 18:50:00  roessel
// Made MSEventControl a singleton class.
// Moved EventSortCrit::operator() into header-file.
//
// Revision 1.4  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.3  2002/10/17 10:42:13  dkrajzew
// usage of adaption type for mismatched times reimplemented
//
// Revision 1.2  2002/10/16 16:39:02  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.2  2002/07/31 17:33:00  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.4  2002/07/30 15:20:20  croessel
// Made previous changes compilable.
//
// Revision 1.3  2002/07/26 11:44:29  dkrajzew
// Adaptation of past event execution time implemented
//
// Revision 1.2  2002/07/26 11:05:12  dkrajzew
// sort criterium debugged; addEvent now returns a bool; problems with
// parallel insertion of items with the execute-method debugged
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:14  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.10  2002/02/05 13:51:51  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.9  2002/01/30 11:19:24  croessel
// execute(): Return value of event->execute() is interpreted as
// time-offset instead of absolute time.
//
// Revision 1.8  2002/01/17 15:34:57  croessel
// execute() distinguishes now between recurring and nonrecurring
// events. The former will be reinserted with a new execution-time, the
// latter ones will be destroyed.
//
// Revision 1.7  2002/01/17 15:22:31  croessel
// Removed superfluous include.
//
// Revision 1.6  2002/01/17 15:21:03  croessel
// Changed the return-type for event-execution to MSNet::Time
//
// Revision 1.5  2002/01/10 11:52:43  croessel
// Method addEvent() added and implemented. execute() implemented.
//
// Revision 1.4  2001/12/19 17:04:17  croessel
// Default-ctor, copy-ctor and assignment-operator removed.
//
// Revision 1.3  2001/11/15 17:12:13  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.2  2001/11/14 15:47:33  croessel
// Merged the diffs between the .C and .cpp versions. Numerous changes
// in MSLane, MSVehicle and MSJunction.
//
// Revision 1.1  2001/10/24 07:10:05  traffic
// new extension
//
// Revision 1.3  2001/07/25 12:17:02  traffic
// CC problems with make_pair repaired
//
// Revision 1.2  2001/07/16 12:55:46  croessel
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

#include <cassert>
#include "MSEventControl.h"
#include <helpers/Command.h>
#include "MSNet.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
MSEventControl::DictType MSEventControl::myDict;
MSEventControl* MSEventControl::myInstance = 0;

/* =========================================================================
 * member definitions
 * ======================================================================= */
// /* -------------------------------------------------------------------------
//  * methods from MSEventControl::EventSortCrit
//  * ----------------------------------------------------------------------- */

MSEventControl*
MSEventControl::getInstance( void )
{
    if ( myInstance == 0 ) {
        myInstance = new MSEventControl();
    }
    return myInstance;
}

MSEventControl::MSEventControl( ) :
    myEvents()
{
}

/* -------------------------------------------------------------------------
 * methods from MSEventControl
 * ----------------------------------------------------------------------- */


MSEventControl::~MSEventControl()
{
    // Empty the event-container and delete the commands.
    while ( ! myEvents.empty() ) {
        Event e = myEvents.top();
        delete e.first;
        myEvents.pop();
    }
}


bool
MSEventControl::addEvent( Command* operation, MSNet::Time execTime,
                          AdaptType type )
{
    MSNet::Time currTime = MSNet::getInstance()->timestep();
    if(type==ADAPT_AFTER_EXECUTION&&execTime<=currTime) {
        execTime = currTime+1;
    }
    Event newEvent = Event( operation, execTime );
    if (myEvents.empty()) { cout << "guenther" << endl;}
    myEvents.push( newEvent );
    return true;
}


void
MSEventControl::execute(MSNet::Time execTime)
{
    // Don't access empty prio_queues.
    if ( ! myEvents.empty() ) {

        // Execute all events that are scheduled for execTime.
        for (;;) {

            Event currEvent = myEvents.top();

            if ( currEvent.second == execTime ) {

                Command *command = currEvent.first;
                myEvents.pop();
                MSNet::Time time = command->execute( );

                // Delete nonrecurring events, reinsert recurring ones
                // with new execution time = execTime + returned offset.
                if ( time == 0 ) {

                    delete currEvent.first;
                }
                else {

                    assert( time > 0 );
                    currEvent.second = execTime + time;
                    myEvents.push( currEvent );
                }
            }
            else {
                break;
            }
        }
    }
}


bool
MSEventControl::dictionary(string id, MSEventControl* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}


MSEventControl*
MSEventControl::dictionary(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


void
MSEventControl::clear()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "MSEventControl.icc"
//#endif

// Local Variables:
// mode:C++
// End:
