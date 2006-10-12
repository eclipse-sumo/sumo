#ifndef MSEventControl_H
#define MSEventControl_H
/***************************************************************************
                          MSEventControl.h  -  Coordinates
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
// $Log$
// Revision 1.15  2006/10/12 10:14:27  dkrajzew
// synchronized with internal CVS (mainly the documentation has changed)
//
// Revision 1.14  2006/03/17 08:59:18  dkrajzew
// changed the Event-interface (execute now gets the current simulation time, event handlers are non-static)
//
// Revision 1.13  2005/10/17 08:58:24  dkrajzew
// trigger rework#1
//
// Revision 1.12  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.11  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.10  2005/05/04 08:25:29  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.9  2005/02/01 10:10:40  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.8  2003/09/05 15:09:16  dkrajzew
// changed the return value of the event adding method for a better
//  handling of the events by the setters
//
// Revision 1.7  2003/08/06 16:49:57  roessel
// Better distinction between steps and seconds added.
//
// Revision 1.6  2003/07/16 15:28:00  dkrajzew
// MSEmitControl now only simulates lanes which do have vehicles;
//  the edges do not go through the lanes, the EdgeControl does
//
// Revision 1.5  2003/06/04 16:16:23  roessel
// MSEventControl has now two MSEventControl* (instead of one),
//  myBeginOfTimestepEvents and myEndOfTimestepEvents.
//  Added the static accss-methods getBeginOfTimestepEvents() and
//  getEndOfTimestepEvents().
//
// Revision 1.4  2003/05/27 18:51:46  roessel
// Made MSEventControl a singleton class.
// Moved EventSortCrit::operator() into header-file.
//
// Revision 1.3  2003/02/07 10:41:51  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:39:02  dkrajzew
// complete deletion within destructors implemented; clear-operator added for
// container; global file include
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
// Revision 2.0  2002/02/14 14:43:15  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.7  2002/02/05 13:51:51  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.6  2002/01/10 11:52:43  croessel
// Method addEvent() added and implemented. execute() implemented.
//
// Revision 1.5  2001/12/19 17:04:35  croessel
// using namespace std replaced by std::
// Copy-ctor and assignment-operator moved to private.
//
// Revision 1.4  2001/11/15 17:12:14  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.3  2001/11/14 11:45:53  croessel
// Resolved conflicts which appeared during suffix-change and
// CR-line-end commits.
//
// Revision 1.2  2001/07/16 12:55:46  croessel
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utility>
#include <queue>
#include <vector>
#include <map>
#include <string>
#include <utils/common/SUMOTime.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class Command;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSEventControl
 * Class which holds incoming events and executes them at the time they shall
 * be executed at.
 */
class MSEventControl
{
public:
    /// Default constructor.
    MSEventControl();

    /// Events that should be executed at time.
    typedef std::pair< Command*, SUMOTime > Event;

    /**
     * @enum AdaptType
     * Defines what to do if the insertion time lies before the current simulation
     *  time
     */
    enum AdaptType {
        /// Patch the time in a way that it is at least as high as the simulation begin time
        ADAPT_AFTER_EXECUTION = 1,
        /// Do nothing
        NO_CHANGE = 2
    };

    /// Sort-criterion for events.
    class EventSortCrit
    {
    public:
        /// compares two events
        bool operator() ( const Event& e1, const Event& e2 ) const
            {
                return e1.second > e2.second;
            }
    };

    /// Destructor.
    ~MSEventControl();

    /** @brief Adds an Event.
     *
     * Returns the time the event will be executed, really */
    SUMOTime addEvent( Command* operation, SUMOTime execTimeStep,
        AdaptType type );

    /** @brief Executes time-dependant commands
        Events are things such as switching traffic-lights, writing output,
        etc. */
    void execute( SUMOTime time );

private:
    /// Container for time-dependant events, e.g. traffic-light-change.
    typedef std::priority_queue< Event, std::vector< Event >,
                                 EventSortCrit > EventCont;

    /// Event-container, holds executable events.
    EventCont myEvents;

    /// Copy constructor.
    MSEventControl(const MSEventControl&);

    /// Assignment operator.
    MSEventControl& operator=(const MSEventControl&);

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
