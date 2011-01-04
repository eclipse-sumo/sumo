/****************************************************************************/
/// @file    GUIEventControl.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 04 Feb 2008
/// @version $Id$
///
// Stores time-dependant events and executes them at the proper time (guisim)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIEventControl_h
#define GUIEventControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/foxtools/MFXMutex.h>
#include <microsim/MSEventControl.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIEventControl
 * @brief Stores time-dependant events and executes them at the proper time (guisim)
 *
 * Encapsulates MSEventControl-methods using a lock, prohibiting parallel addition /
 *  processing of events what may yield in application break due to broken containers.
 */
class GUIEventControl : public MSEventControl {
public:
    /// @brief Default constructor.
    GUIEventControl() throw();


    /// @brief Destructor.
    ~GUIEventControl() throw();


    /** @brief Adds an Event.
     *
     * Locks itself before calling MSEventControl::addEvent. Unlock itself
     *  after the call.
     *
     * @param[in] operation The event to add
     * @param[in] execTimeStep The time the event shall be executed at
     * @param[in] type The adaptation type
     * @see MSEventControl::addEvent
     */
    SUMOTime addEvent(Command* operation, SUMOTime execTimeStep,
                      AdaptType type) throw();


    /** @brief Executes time-dependant commands
     *
     * Locks itself before calling MSEventControl::execute. Unlock itself
     *  after the call.
     *
     * @param[in] time The current simulation time
     * @exception ProcessError From an executed Command
     * @see MSEventControl::execute
     */
    void execute(SUMOTime time) throw(ProcessError);


private:
    /// @brief The lock used to prohibit parallel addition and processing of events
    MFXMutex myLock;


private:
    /// @brief invalid copy constructor.
    GUIEventControl(const GUIEventControl&);

    /// @brief invalid assignment operator.
    GUIEventControl& operator=(const GUIEventControl&);


};


#endif

/****************************************************************************/

