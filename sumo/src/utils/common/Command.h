/****************************************************************************/
/// @file    Command.h
/// @author  Christian Roessel
/// @date    Thu, 20 Dec 2001
/// @version $Id:Command.h 4699 2007-11-09 14:05:13Z dkrajzew $
///
// Base (microsim) event class
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
#ifndef Command_h
#define Command_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Command
 * @brief Base (microsim) event class
 *
 * Classes derived from Command may be added to MSEventControl instances in 
 *  order to be executed at a certain time step.
 *
 * As soon as the simulation reaches the desired time step, the command (event)
 *  is executed by calling "execute" with the current time step. The method must
 *  return either 0, if the event shall not be executed again or a positive value
 *  (in simulation seconds) that described when it shall be executed again. The method
 *  must not return a value below zero, the behaviour is undefined in this case.
 *
 * @warning The EventControl the Command is added to gets responsible for
 *  this command's deletion.
 *
 * @see Design Patterns, Gamma et al.
 * @see WrappingCommand
 * @see MSEventControl
 */
class Command
{
public:
    /// @brief Constructor
    Command() throw() { }


    /// @brief Destructor.
    virtual ~Command() throw() { }


    /** @brief Executes the command.
     *
     * The implementations should return 0 if the command shall not be repeated,
     *  or a value larger than 0 that describe the time after which the command
     *  shall be executed again. Values below 0 must not be returned.
     *
     * @param[in] currentTime The current simulation time
     * @return The time after which the command shall be executed again, 0 if this command shall be descheduled.
     * @exception ProcessError Derived actions may throw this exception
     */
    virtual SUMOTime execute(SUMOTime currentTime) throw(ProcessError) = 0;


};


#endif

/****************************************************************************/

