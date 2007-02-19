/****************************************************************************/
/// @file    MSEventControl.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// time-dependant events
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
#ifndef MSEventControl_h
#define MSEventControl_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utility>
#include <queue>
#include <vector>
#include <map>
#include <string>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class Command;


// ===========================================================================
// class definitions
// ===========================================================================
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
        bool operator()(const Event& e1, const Event& e2) const
        {
            return e1.second > e2.second;
        }
    };

    /// Destructor.
    ~MSEventControl();

    /** @brief Adds an Event.
     *
     * Returns the time the event will be executed, really */
    SUMOTime addEvent(Command* operation, SUMOTime execTimeStep,
                      AdaptType type);

    /** @brief Executes time-dependant commands
        Events are things such as switching traffic-lights, writing output,
        etc. */
    void execute(SUMOTime time);

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


#endif

/****************************************************************************/

