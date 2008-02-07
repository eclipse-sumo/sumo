/****************************************************************************/
/// @file    Command.h
/// @author  Christian Roessel
/// @date    Thu, 20 Dec 2001
/// @version $Id:Command.h 4699 2007-11-09 14:05:13Z dkrajzew $
///
// -------------------
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
 * @brief Base class for all Command classes.
 *
 * When executed, the current time step will be given to the command.
 * The command must then return the time to the next call in simulation
 *  seconds. If zero is returned, this command will be descheduled and
 *  deleted by the event handler.
 *
 * @see Design Patterns, Gamma et al.
 * @see WrappingCommand
 * @see MSEventControl
 */
class Command
{
public:
    Command() throw() { }

    /// Destructor.
    virtual ~Command() throw() { }

    /**
     * @brief Executes the command.
     *
     * @param currentTime The current simulation time
     *
     * @return The receivers operation should return the next interval
     * in steps for recurring commands and 0 for single-execution
     * commands.
     */
    virtual SUMOTime execute(SUMOTime currentTime) throw(ProcessError) = 0;

};


#endif

/****************************************************************************/

