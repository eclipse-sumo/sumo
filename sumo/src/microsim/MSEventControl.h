/****************************************************************************/
/// @file    MSEventControl.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Matthias Heppner
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Stores time-dependant events and executes them at the proper time
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSEventControl_h
#define MSEventControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utility>
#include <queue>
#include <vector>
#include <map>
#include <utils/common/SUMOTime.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class Command;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSEventControl
 * @brief Stores time-dependant events and executes them at the proper time
 */
class MSEventControl {
public:
    /// @brief Combination of an event and the time it shall be executed at
    typedef std::pair< Command*, SUMOTime > Event;


    /**
     * @enum AdaptType
     * @brief Defines what to do if the insertion time lies before the current simulation time
     */
    enum AdaptType {
        /// @brief Patch the time in a way that it is at least as high as the simulation begin time
        ADAPT_AFTER_EXECUTION = 1,
        /// @brief Do nothing
        NO_CHANGE = 2
    };


public:
    /// @brief Default constructor.
    MSEventControl();


    /// @brief Destructor.
    virtual ~MSEventControl();


    /** @brief Adds an Event.
     *
     * If the given execution time step lies before the current and ADAPT_AFTER_EXECUTION
     *  is passed for adaptation type, the execution time step will be set to the
     *  current time step.
     *
     * Returns the time the event will be executed, really.
     *
     * @param[in] operation The event to add
     * @param[in] execTimeStep The time the event shall be executed at
     * @param[in] type The adaptation type
     * @see Command
     * @see AdaptType
     */
    virtual SUMOTime addEvent(Command* operation, SUMOTime execTimeStep,
                              AdaptType type);


    /** @brief Executes time-dependant commands
     *
     * Loops over all stored events, continuing until the first event which
     *  execution time lies beyond the given time + deltaT. If the event
     *  had to be executed before the given time, a warning is generated and
     *  the event deleted. Otherwise (the event is valid), the event is executed.
     *
     * Each executed event must return the time that has to pass until it shall
     *  be executed again. If the returned time is 0, the event is deleted.
     *  Otherwise it is readded, after the new execution time (returned + current)
     *  is computed.
     *
     * ProcessErrors thrown by executed commands are rethrown.
     *
     * @param[in] time The current simulation time
     * @exception ProcessError From an executed Command
     */
    virtual void execute(SUMOTime time);


    /** @brief Returns whether events are in the que.
     *
     * @return  whether events are in the que
     */
    bool isEmpty();


    /** @brief Set the current Time.
     *
     * This method is only for Unit Testing.
     * Set the current TimeStep used in addEvent.
     * Normally the time is set automatically from an instance of MSNet.
     */
    void setCurrentTimeStep(SUMOTime time);


protected:
    /** @brief Sort-criterion for events.
     *
     * Sorts events by their execution time
     */
    class EventSortCrit {
    public:
        /// @brief compares two events
        bool operator()(const Event& e1, const Event& e2) const {
            return e1.second > e2.second;
        }
    };


private:
    /// @brief Container for time-dependant events, e.g. traffic-light-change.
    typedef std::priority_queue< Event, std::vector< Event >, EventSortCrit > EventCont;

    /// The Current TimeStep
    SUMOTime currentTimeStep;

    /// @brief Event-container, holds executable events.
    EventCont myEvents;

    /// get the Current TimeStep used in addEvent.
    SUMOTime getCurrentTimeStep();


private:
    /// @brief invalid copy constructor.
    MSEventControl(const MSEventControl&);

    /// @brief invalid assignment operator.
    MSEventControl& operator=(const MSEventControl&);


};


#endif

/****************************************************************************/

